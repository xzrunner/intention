#include "sopview/RenderSystem.h"
#include "sopview/Node.h"
#include "sopview/RegistNodes.h"
#include "sopview/WxGeoProperty.h"

#include <geoshape/Point3D.h>
#include <geoshape/Polygon3D.h>
#include <geoshape/Box.h>
#include <polymesh3/Geometry.h>
#include <sop/Node.h>
#include <sop/GeometryImpl.h>
#include <sop/node/GroupCreate.h>
#include <sop/node/GroupPromote.h>
#include <painting3/RenderSystem.h>
#include <painting3/Viewport.h>
#include <node0/SceneNode.h>
#include <node3/RenderSystem.h>
#include <node3/CompShape.h>
#include <model/BrushBuilder.h>

namespace
{

const float    NODE_RADIUS = 10;
const uint32_t LIGHT_SELECT_COLOR   = 0x88000088;

}

namespace sopv
{

const float RenderSystem::UV_SCALE = 512;

RenderSystem::RenderSystem(const pt3::Viewport& vp,
                           const sm::mat4& cam_mat)
    : m_vp(vp)
    , m_cam_mat(cam_mat)
{
}

void RenderSystem::DrawNode3D(const pt0::RenderContext& rc,
                              const sop::Node& back,
                              const bp::Node& front) const
{
    if (!front.get_type().is_derived_from<sopv::Node>()) {
        return;
    }

    auto& sopv_node = static_cast<const Node&>(front);
    if (!sopv_node.GetDisplay() &&
        !sopv_node.GetTemplate()) {
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

    if (sopv_node.GetDisplay())
    {
        // draw face
        rp.type = pt3::RenderParams::DRAW_MESH;
        n3::RenderSystem::Draw(*sn, rp, rc);
    }

    // draw edge
    rp.type = pt3::RenderParams::DRAW_BORDER_MESH;
    n3::RenderSystem::Draw(*sn, rp, rc);
}

void RenderSystem::DrawNode2D(const sop::Node& back, const bp::Node& front) const
{
    auto& sopv_node = static_cast<const Node&>(front);
    if (!sopv_node.GetDisplay()) {
        return;
    }

    auto geo = back.GetGeometry();
    if (!geo) {
        return;
    }

    auto node = geo->GetNode();
    if (node->HasSharedComp<n3::CompShape>())
    {
        auto& shapes = node->GetSharedComp<n3::CompShape>().GetShapes();
        if (!shapes.empty())
        {
            pt3::RenderParams rp;
            rp.painter = &m_pt;
            rp.viewport = &m_vp;
            rp.cam_mat = &m_cam_mat;
            rp.radius = NODE_RADIUS;
            rp.color = LIGHT_SELECT_COLOR;
            rp.draw_ctrl_node = true;

            for (auto& s : shapes) {
                pt3::RenderSystem::DrawShape(*s, rp);
            }
        }
    }

    auto type = front.get_type();
    if (type == rttr::type::get<node::GroupCreate>())
    {
        auto& group_create = static_cast<const node::GroupCreate&>(front);
        auto group = geo->GetGroup().Query(group_create.group_name);
        if (group) {
            DrawGroup(*group, *geo);
        }

        pt3::RenderParams rp;
        rp.painter = &m_pt;
        rp.viewport = &m_vp;
        rp.cam_mat = &m_cam_mat;
        rp.radius = NODE_RADIUS;
        rp.color = 0xffff0000;
        rp.draw_ctrl_node = true;

        // draw bounding
        if (group_create.keep_in_bounding)
        {
            auto& bound_conns = back.GetImports()[sop::node::GroupCreate::IDX_BOUNDING_OBJ].conns;
            if (!bound_conns.empty()) {
                assert(bound_conns.size() == 1);
                auto bound_node = bound_conns.front().node.lock();
                if (bound_node) {
                    auto bound_geo = bound_node->GetGeometry();
                    if (bound_geo) {
                        auto& b_aabb = bound_geo->GetAttr().GetAABB();
                        pt3::RenderSystem::DrawShape(gs::Box(b_aabb), rp);
                    }
                }
            }
        }
    }
    else if (type == rttr::type::get<node::GroupExpression>())
    {
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
    else if (type == rttr::type::get<node::GroupPromote>())
    {
        auto& group_promote = static_cast<const node::GroupPromote&>(front);
        auto group = geo->GetGroup().Query(group_promote.group_name.str);
        if (group) {
            DrawGroup(*group, *geo);
        }
    }
}

void RenderSystem::DrawNodeAttr(const sop::Node& node, const WxGeoProperty& prop_view)
{
    auto geo = node.GetGeometry();
    if (!geo) {
        return;
    }

    pt3::RenderParams rp;
    rp.painter  = &m_pt;
    rp.viewport = &m_vp;
    rp.cam_mat  = &m_cam_mat;
    rp.radius   = NODE_RADIUS;
    rp.color    = LIGHT_SELECT_COLOR;

    auto sel_pts = prop_view.GetSelectedIndices(GeoAttrClass::Point);
    if (!sel_pts.empty())
    {
        auto& pts = geo->GetAttr().GetPoints();
        for (auto i : sel_pts) {
            pt3::RenderSystem::DrawShape(gs::Point3D(pts[i]->pos), rp);
        }
    }

    auto sel_vts = prop_view.GetSelectedIndices(GeoAttrClass::Vertex);
    if (!sel_vts.empty())
    {
        auto& vts = geo->GetAttr().GetVertices();
        for (auto i : sel_vts) {
            pt3::RenderSystem::DrawShape(gs::Point3D(vts[i]->point->pos), rp);
        }
    }

    auto sel_prims = prop_view.GetSelectedIndices(GeoAttrClass::Primitive);
    if (!sel_prims.empty())
    {
        auto& prims = geo->GetAttr().GetPrimtives();
        for (auto i : sel_prims)
        {
            auto& prim = prims[i];
            std::vector<sm::vec3> vts;
            vts.reserve(prim->vertices.size());
            for (auto& v : prim->vertices) {
                vts.push_back(v->point->pos);
            }
            pt3::RenderSystem::DrawShape(gs::Polygon3D(vts), rp);
        }
    }
}

void RenderSystem::DrawNodeUV(const sop::Node& node)
{
    auto geo = node.GetGeometry();
    if (!geo) {
        return;
    }

    auto& attr = geo->GetAttr();
    auto uv_idx = attr.QueryAttrIdx(sop::GeoAttrClass::Point, sop::GeoAttr::GEO_ATTR_UV);
    if (uv_idx >= 0)
    {
        auto& pts = attr.GetPoints();
        for (auto& prim : attr.GetPrimtives())
        {
            std::vector<sm::vec2> border;
            border.reserve(prim->vertices.size());
            for (auto& v : prim->vertices) {
                auto uv = static_cast<const sm::vec3*>(pts[v->point->attr_idx]->vars[uv_idx].p);
                border.push_back({ uv->x * UV_SCALE, uv->y * UV_SCALE });
            }
            m_pt.AddPolygon(border.data(), border.size(), 0xff000000);
        }

        return;
    }

    uv_idx = attr.QueryAttrIdx(sop::GeoAttrClass::Vertex, sop::GeoAttr::GEO_ATTR_UV);
    if (uv_idx >= 0)
    {
        auto& vts = attr.GetVertices();
        for (auto& prim : attr.GetPrimtives())
        {
            std::vector<sm::vec2> border;
            border.reserve(prim->vertices.size());
            for (auto& v : prim->vertices) {
                auto uv = static_cast<const sm::vec3*>(vts[v->attr_idx]->vars[uv_idx].p);
                border.push_back({ uv->x * UV_SCALE, uv->y * UV_SCALE });
            }
            m_pt.AddPolygon(border.data(), border.size(), 0xff000000);
        }

        return;
    }
}

void RenderSystem::DrawGroup(const sop::Group& group, const sop::GeometryImpl& geo) const
{
    switch (group.GetType())
    {
    case sop::GroupType::Points:
    {
        auto& points = geo.GetAttr().GetPoints();
        for (auto& f : group.GetItems()) {
            auto pos = m_vp.TransPosProj3ToProj2(points[f]->pos, m_cam_mat);
            m_pt.AddCircleFilled(pos, NODE_RADIUS, LIGHT_SELECT_COLOR);
        }
    }
        break;
    case sop::GroupType::Vertices:
        break;
    case sop::GroupType::Edges:
        break;
    case sop::GroupType::Primitives:
    {
        auto brush_model = geo.GetBrushModel();
        if (brush_model)
        {
            size_t p_off = 0, f_off = 0;
            for (auto& brush : brush_model->GetBrushes())
            {
                auto& poly = *brush.impl;
                for (auto& f : group.GetItems()) {
                    DrawFace(poly, f - f_off, LIGHT_SELECT_COLOR, m_cam_mat);
                }
                p_off += poly.Points().size();
                f_off += poly.Faces().size();
            }
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
		polygon.push_back(m_vp.TransPosProj3ToProj2(poly.Points()[v]->pos, cam_mat));
	}
	m_pt.AddPolygonFilled(polygon.data(), polygon.size(), color);
}

}