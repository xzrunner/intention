#pragma once

#include <SM_Vector.h>
#include <unirender/typedef.h>
#include <renderpipeline/IRenderer.h>
#include <renderpipeline/RendererImpl.h>

#include <boost/noncopyable.hpp>

namespace sop { class Volume; }

namespace sopv
{

struct HeightfieldVertex
{
    HeightfieldVertex() {}
	HeightfieldVertex(size_t ix, size_t iz, size_t size_x, size_t size_z)
	{
		static const float off = -1.0f;
		static const float scale = 1 / 50.0f;

		position.Set(off + ix * scale, 0, off + iz * scale);
		texcoords.Set(
            ix / static_cast<float>(size_x),
            iz / static_cast<float>(size_z)
        );
	}

	sm::vec3 position;
	sm::vec2 texcoords;
};

class HeightfieldRenderer : public rp::IRenderer, public rp::RendererImpl<HeightfieldVertex, uint32_t>, private boost::noncopyable
{
public:
    HeightfieldRenderer(const ur::Device& dev);

    virtual void Flush(ur::Context& ctx) override {}

    void Clear();

    void Draw(const ur::Device& dev, ur::Context& ctx,
        const std::shared_ptr<sop::Volume>& vol);

private:
    void InitTextuers(const ur::Device& dev);
    void InitShader(const ur::Device& dev);

    void Setup(const ur::Device& dev, ur::Context& ctx,
        const std::shared_ptr<sop::Volume>& vol);

    void BuildVertBuf(ur::Context& ctx);
    void DrawVertBuf(ur::Context& ctx) const;

    static ur::TexturePtr
        GenHeightMap(const ur::Device& dev, const sop::Volume& vol);

private:
    std::shared_ptr<sop::Volume> m_vol = nullptr;

    ur::TexturePtr m_height_map = nullptr;

    ur::TexturePtr m_detail_map = nullptr;
    ur::TexturePtr m_splat_map[4];

}; // HeightfieldRenderer

}