#include "intention/RenderSystem.h"
#include "intention/Node.h"
#include "intention/RegistNodes.h"
#include "intention/NodeHelper.h"

#include <polymesh3/Geometry.h>
#include <everything/Node.h>
#include <everything/GeometryImpl.h>
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

void RenderSystem::DrawNode3D(const pt0::RenderContext& rc,
                              const evt::Node& back,
                              const bp::Node& front) const
{
    if (!front.get_type().is_derived_from<itt::Node>()) {
        return;
    }

    auto& itt_node = static_cast<const Node&>(front);
    if (!itt_node.GetDisplay()) {
        return;
    }

    auto geo = back.GetGeometry();
    if (!geo) {
        return;
    }

    auto sn = geo->GetNode();
    if (!sn) {
        return;
    }

    pt3::RenderParams rp;
    rp.painter  = &m_pt;
    rp.viewport = &m_vp;
    rp.cam_mat  = &m_cam_mat;

    if (itt_node.GetDisplay())
    {
        // draw face
        rp.type = pt3::RenderParams::DRAW_MESH;
        n3::RenderSystem::Draw(*sn, rp, rc);
    }

    // draw edge
    rp.type = pt3::RenderParams::DRAW_BORDER_MESH;
    n3::RenderSystem::Draw(*sn, rp, rc);
}

void RenderSystem::DrawNode2D(const evt::Node& back, const bp::Node& front) const
{
    auto& itt_node = static_cast<const Node&>(front);
    if (!itt_node.GetDisplay()) {
        return;
    }

    auto type = front.get_type();
    if (type == rttr::type::get<node::GroupCreate>())
    {
        auto geo = back.GetGeometry();
        if (!geo) {
            return;
        }

        auto& group_create = static_cast<const node::GroupCreate&>(front);
        auto group = geo->GetGroup().Query(group_create.group_name);
        if (group) {
            DrawGroup(*group, *geo);
        }
    }
    else if (type == rttr::type::get<node::GroupExpression>())
    {
        auto geo = back.GetGeometry();
        if (!geo) {
            return;
        }

        auto& group_expr = static_cast<const node::GroupExpression&>(front);
        auto group0 = geo->GetGroup().Query(group_expr.inst0.group_name);
        if (group0)  {
            DrawGroup(*group0, *geo);
        }
        auto group1 = geo->GetGroup().Query(group_expr.inst1.group_name);
        if (group1) {
            DrawGroup(*group1, *geo);
        }
        auto group2 = geo->GetGroup().Query(group_expr.inst2.group_name);
        if (group2) {
            DrawGroup(*group2, *geo);
        }
        auto group3 = geo->GetGroup().Query(group_expr.inst3.group_name);
        if (group3) {
            DrawGroup(*group3, *geo);
        }
    }
}

void RenderSystem::DrawGroup(const evt::Group& group, const evt::GeometryImpl& geo) const
{
    switch (group.type)
    {
    case evt::GroupType::Points:
    {
        auto& points = geo.GetAttr().GetPoints();
        for (auto& f : group.items) {
            auto pos = m_vp.TransPosProj3ToProj2(points[f]->pos, m_cam_mat);
            m_pt.AddCircleFilled(pos, 10, LIGHT_SELECT_COLOR);
        }
    }
        break;
    case evt::GroupType::Vertices:
        break;
    case evt::GroupType::Edges:
        break;
    case evt::GroupType::Primitives:
    {
        auto brush_model = geo.GetBrushModel();
        auto& brushes = brush_model->GetBrushes();
        assert(brushes.size() == 1);
        auto& brush = brushes[0];

        for (auto& f : group.items) {
            DrawFace(*brush.impl, f, LIGHT_SELECT_COLOR, m_cam_mat);
        }
    }
        break;
    default:
        assert(0);
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