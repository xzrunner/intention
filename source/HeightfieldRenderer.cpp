#include "sopview/HeightfieldRenderer.h"

#include <sop/Volume.h>
#include <unirender/ShaderProgram.h>
#include <unirender/Texture.h>
#include <unirender/TextureDescription.h>
#include <painting0/Shader.h>
#include <painting0/ModelMatUpdater.h>
#include <painting3/ViewMatUpdater.h>
#include <painting3/ProjectMatUpdater.h>
#include <model/TextureLoader.h>

namespace
{

const char* vs = R"(

attribute vec4 position;
attribute vec2 texcoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform sampler2D u_heightmap;

varying float v_height;
varying vec2  v_texcoord;

void main()
{
	v_height = texture2D(u_heightmap, texcoord).a;

	vec4 pos = position;
	pos.y = v_height;
	gl_Position = u_projection * u_view * u_model * pos;

	v_texcoord = texcoord;
}

)";

const char* fs = R"(

uniform sampler2D u_detailmap;

uniform sampler2D u_splatmap0;
uniform sampler2D u_splatmap1;
uniform sampler2D u_splatmap2;
uniform sampler2D u_splatmap3;

varying float v_height;
varying vec2  v_texcoord;

void main()
{
	vec4 splat_col;
	if (v_height < 0.33) {
		float t = v_height / 0.33;
		splat_col = texture2D(u_splatmap0, v_texcoord) * (1 - t) + texture2D(u_splatmap1, v_texcoord) * t;
	} else if (v_height < 0.66) {
		float t = (v_height - 0.33) / 0.33;
		splat_col = texture2D(u_splatmap1, v_texcoord) * (1 - t) + texture2D(u_splatmap2, v_texcoord) * t;
	} else {
		float t = (v_height - 0.66) / 0.33;
		splat_col = texture2D(u_splatmap2, v_texcoord) * (1 - t) + texture2D(u_splatmap3, v_texcoord) * t;
	}

	vec4 detail_col = texture2D(u_detailmap, v_texcoord);
	gl_FragColor = splat_col * detail_col;
}

)";

}

namespace sopv
{

HeightfieldRenderer::HeightfieldRenderer(const ur::Device& dev)
    : rp::RendererImpl<HeightfieldVertex, uint32_t>(dev)
{
    InitTextuers(dev);
    InitShader(dev);
}

void HeightfieldRenderer::Clear()
{
    m_vol.reset();
}

void HeightfieldRenderer::Draw(const ur::Device& dev, ur::Context& ctx,
                               const std::shared_ptr<sop::Volume>& vol)
{
    if (m_shaders.empty()) {
        return;
    }

    if (m_vol != vol) {
        Setup(dev, ctx, vol);
    }

    //DrawVertBuf();
}

void HeightfieldRenderer::InitTextuers(const ur::Device& dev)
{
    m_detail_map = model::TextureLoader::LoadFromFile(dev, "D:\\OneDrive\\asset\\terrain\\detailMap.tga");

    m_splat_map[0] = model::TextureLoader::LoadFromFile(dev, "D:\\OneDrive\\asset\\terrain\\lowestTile.tga");
    m_splat_map[1] = model::TextureLoader::LoadFromFile(dev, "D:\\OneDrive\\asset\\terrain\\lowTile.tga");
    m_splat_map[2] = model::TextureLoader::LoadFromFile(dev, "D:\\OneDrive\\asset\\terrain\\highTile.tga");
    m_splat_map[3] = model::TextureLoader::LoadFromFile(dev, "D:\\OneDrive\\asset\\terrain\\highestTile.tga");
}

void HeightfieldRenderer::InitShader(const ur::Device& dev)
{
    //std::vector<ur::VertexAttrib> layout;
    //layout.push_back(ur::VertexAttrib(rp::VERT_POSITION_NAME, 3, 4, 20, 0));
    //layout.push_back(ur::VertexAttrib(rp::VERT_TEXCOORD_NAME, 2, 4, 20, 12));
    //rc.CreateVertexLayout(layout);

    auto shader = dev.CreateShaderProgram(vs, fs);
    shader->AddUniformUpdater(std::make_shared<pt0::ModelMatUpdater>(*shader, rp::MODEL_MAT_NAME));
    shader->AddUniformUpdater(std::make_shared<pt3::ViewMatUpdater>(*shader, rp::VIEW_MAT_NAME));
    shader->AddUniformUpdater(std::make_shared<pt3::ProjectMatUpdater>(*shader, rp::PROJ_MAT_NAME));
    m_shaders.push_back(shader);
}

void HeightfieldRenderer::Setup(const ur::Device& dev, ur::Context& ctx,
                                const std::shared_ptr<sop::Volume>& vol)
{
    m_vol = vol;
    if (!m_vol) {
        return;
    }

    assert(vol);
    auto old = m_height_map;
    m_height_map = GenHeightMap(dev, *vol);

    // textures
    if (m_height_map != old)
    {
        assert(m_shaders.size() == 1);
        auto shader = m_shaders.front();
        ctx.SetTexture(shader->QueryTexSlot("u_heightmap"), m_height_map);
        ctx.SetTexture(shader->QueryTexSlot("u_detailmap"), m_detail_map);
        ctx.SetTexture(shader->QueryTexSlot("u_splatmap0"), m_splat_map[0]);
        ctx.SetTexture(shader->QueryTexSlot("u_splatmap1"), m_splat_map[1]);
        ctx.SetTexture(shader->QueryTexSlot("u_splatmap2"), m_splat_map[2]);
        ctx.SetTexture(shader->QueryTexSlot("u_splatmap3"), m_splat_map[3]);
    }

    // vertex buffer
    if (!old ||
        old->GetWidth() != m_height_map->GetWidth() ||
        old->GetHeight() != m_height_map->GetHeight()) {
        BuildVertBuf(ctx);
    }
}

void HeightfieldRenderer::BuildVertBuf(ur::Context& ctx)
{
    assert(m_vol);
    auto& sz = m_vol->GetSize();
    const auto w = sz.x;
    const auto h = sz.z;
    assert(w > 0 && h > 0 && sz.y == 1);

    m_buf.Clear();
    m_buf.Reserve((w - 1) * (h - 1) * 6, w * h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            *m_buf.vert_ptr++ = HeightfieldVertex(x, y, w, h);
        }
    }

    for (int y = 0; y < h - 1; ++y) {
        for (int x = 0; x < w - 1; ++x) {
            size_t ll = y * w + x;
            size_t rl = ll + 1;
            size_t lh = ll + w;
            size_t rh = lh + 1;
            *m_buf.index_ptr++ = ll;
            *m_buf.index_ptr++ = lh;
            *m_buf.index_ptr++ = rh;
            *m_buf.index_ptr++ = ll;
            *m_buf.index_ptr++ = rh;
            *m_buf.index_ptr++ = rl;
        }
    }

    ur::RenderState rs;

    assert(m_shaders.size() == 1);
    FlushBuffer(ctx, ur::PrimitiveType::Triangles, rs, m_shaders[0]);
}

void HeightfieldRenderer::DrawVertBuf(ur::Context& ctx) const
{
    if (!m_vol || !m_va) {
        return;
    }

    ur::DrawState ds;
    assert(m_shaders.size() == 1);
    ds.program = m_shaders.front();
    ds.vertex_array = m_va;
    ctx.Draw(ur::PrimitiveType::Triangles, ds, nullptr);
}

ur::TexturePtr HeightfieldRenderer::GenHeightMap(const ur::Device& dev, const sop::Volume& vol)
{
    auto& sz = vol.GetSize();
    if (sz.x == 0 || sz.z == 0 || sz.y != 1) {
        return nullptr;
    }

    std::vector<unsigned char> pixels;
    pixels.resize(sz.x * sz.z);
    auto& vals = vol.GetValues();
    assert(vals.size() == pixels.size());
    for (size_t i = 0, n = vals.size(); i < n; ++i) {
        pixels[i] = static_cast<unsigned char>(vals[i] * 255.0f);
    }

    ur::TextureDescription desc;
    desc.target = ur::TextureTarget::Texture2D;
    desc.width  = sz.x;
    desc.height = sz.z;
    desc.format = ur::TextureFormat::A8;
    return dev.CreateTexture(desc, pixels.data());
}

}