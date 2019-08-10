#include "intention/RenderSystem.h"
#include "intention/Node.h"
#include "intention/RegistNodes.h"
#include "intention/NodeHelper.h"

#include <polymesh3/Geometry.h>
#include <everything/Node.h>
#include <everything/BrushGroup.h>
#include <everything/NodeHelper.h>
#include <everything/node/GroupCreate.h>
#include <painting3/RenderSystem.h>
#include <painting3/Viewport.h>
#include <node3/RenderSystem.h>
#include <model/BrushBuilder.h>

namespace
{

static const uint32_t LIGHT_SELECT_COLOR   = 0x88000088;

}

namespace itt
{

RenderSystem::RenderSystem(const pt3::Viewport& vp,
                           const sm::mat4& cam_mat)
    : m_vp(vp)
    , m_cam_mat(cam_mat)
{
}

void RenderSystem::DrawNode(const pt0::RenderContext& rc,
                            const evt::Node& back,
                            const bp::Node& front) const
{
    if (!front.get_type().is_derived_from<itt::Node>()) {
        return;
    }

    auto itt_node = static_cast<const Node&>(front);
    if (!itt_node.GetDisplay()) {
        return;
    }

    auto sn = back.GetSceneNode();
    if (!sn) {
        return;
    }

    pt3::RenderParams rp;

    if (itt_node.GetDisplay())
    {
        // draw face
        rp.type = pt3::RenderParams::DRAW_MESH;
        n3::RenderSystem::Draw(*sn, rp, rc);
    }

    // draw edge
    rp.type = pt3::RenderParams::DRAW_BORDER_MESH;
    n3::RenderSystem::Draw(*sn, rp, rc);

    auto type = front.get_type();
    if (type == rttr::type::get<node::GroupCreate>())
    {
        auto& gc = static_cast<const evt::node::GroupCreate&>(back);
        auto group = gc.GetGroup();

        auto brush_model = evt::NodeHelper::GetBrushModel(*sn);
        auto& brushes = brush_model->GetBrushes();

        assert(group->parts.size() == brushes.size());
        for (int i = 0, n = brushes.size(); i < n; ++i) {
            for (auto& part : group->parts) {
                for (auto& f : part.faces) {
                    DrawFace(*brushes[i].impl, f, LIGHT_SELECT_COLOR, m_cam_mat);
                }
            }
        }
    }
}

void RenderSystem::DrawFace(const pm3::Polytope& poly, size_t face_idx,
                            uint32_t color, const sm::mat4& cam_mat) const
{
	std::vector<sm::vec2> polygon;
    auto& face = poly.Faces()[face_idx];
	polygon.reserve(face->points.size());
	for (auto& v : face->points) {
		polygon.push_back(m_vp.TransPosProj3ToProj2(poly.Points()[v], cam_mat));
	}
	m_pt.AddPolygonFilled(polygon.data(), polygon.size(), color);
}

}