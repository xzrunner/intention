#include "sopview/WxStageCanvas.h"
#include "sopview/RenderSystem.h"
#include "sopview/Evaluator.h"
#include "sopview/SceneTree.h"
#include "sopview/WxGeoProperty.h"

#include <ee0/WxStagePage.h>
#include <blueprint/CompNode.h>

#include <geoshape/Point3D.h>
#include <geoshape/Polygon3D.h>
#include <node0/SceneNode.h>
#include <painting2/RenderSystem.h>
#include <painting3/MaterialMgr.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <painting3/PerspCam.h>
#include <sop/GeometryImpl.h>

namespace
{

const float    NODE_RADIUS = 10;
const uint32_t LIGHT_SELECT_COLOR = 0x88000088;

}

namespace sopv
{

WxStageCanvas::WxStageCanvas(ee0::WxStagePage* stage, ECS_WORLD_PARAM
                             const ee0::RenderContext& rc)
    : ee3::WxStageCanvas(stage, ECS_WORLD_VAR &rc, nullptr, true)
{
}

bool WxStageCanvas::OnUpdate()
{
    return false;
}

void WxStageCanvas::DrawBackground3D() const
{
    ee3::WxStageCanvas::DrawBackgroundGrids(10.0f, 0.2f);
//    ee3::WxStageCanvas::DrawBackgroundCross();
}

void WxStageCanvas::DrawForeground3D() const
{
    if (!m_stree) {
        return;
    }
    auto eval = m_stree->GetCurrEval();
    if (!eval) {
        return;
    }

    pt0::RenderContext rc;
    rc.AddVar(
        pt3::MaterialMgr::PositionUniforms::light_pos.name,
        pt0::RenderVariant(sm::vec3(0, 2, -4))
    );
    if (m_camera->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>())
    {
        auto persp = std::static_pointer_cast<pt3::PerspCam>(m_camera);
        rc.AddVar(
            pt3::MaterialMgr::PositionUniforms::cam_pos.name,
            pt0::RenderVariant(persp->GetPos())
        );
    }
    auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
    assert(wc);
    rc.AddVar(
        pt3::MaterialMgr::PosTransUniforms::view.name,
        pt0::RenderVariant(wc->GetViewMat())
    );
    rc.AddVar(
        pt3::MaterialMgr::PosTransUniforms::projection.name,
        pt0::RenderVariant(wc->GetProjMat())
    );

    auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();
    RenderSystem rs(GetViewport(), cam_mat);

    auto& nodes = eval->GetAllNodes();
    for (auto& n : nodes) {
        rs.DrawNode3D(rc, *n.second, *n.first);
    }
}

void WxStageCanvas::DrawForeground2D() const
{
    if (!m_stree) {
        return;
    }
    auto eval = m_stree->GetCurrEval();
    if (!eval) {
        return;
    }

    auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();
    RenderSystem rs(GetViewport(), cam_mat);

    auto& nodes = eval->GetAllNodes();
    for (auto& n : nodes) {
        rs.DrawNode2D(*n.second, *n.first);
    }

    DrawAttrSelected(rs.GetPainter(), cam_mat);

    pt2::RenderSystem::DrawPainter(rs.GetPainter());
}

void WxStageCanvas::DrawAttrSelected(tess::Painter& pt, const sm::mat4& cam_mat) const
{
    if (!m_prop_view || !m_graph_stage) {
        return;
    }

    auto& selection = m_graph_stage->GetSelection();
    if (selection.Size() != 1) {
        return;
    }

    auto eval = m_stree->GetCurrEval();
    if (!eval) {
        return;
    }

    n0::SceneNodePtr node = nullptr;
    selection.Traverse([&](const ee0::GameObjWithPos& nwp)->bool
    {
        node = nwp.GetNode();
        return false;
    });
    assert(node);

    if (!node->HasUniqueComp<bp::CompNode>()) {
        return;
    }

    auto& cnode = node->GetUniqueComp<bp::CompNode>();
    auto sop_node = eval->QueryBackNode(*cnode.GetNode());
    auto geo = sop_node->GetGeometry();
    if (!geo) {
        return;
    }

    pt3::RenderParams rp;
    rp.painter = &pt;
    rp.viewport = &GetViewport();
    rp.cam_mat = &cam_mat;
    rp.radius = NODE_RADIUS;
    rp.color = LIGHT_SELECT_COLOR;

    auto sel_pts = m_prop_view->GetSelectedIndices(GeoAttrClass::Point);
    if (!sel_pts.empty())
    {
        auto& pts = geo->GetAttr().GetPoints();
        for (auto i : sel_pts) {
            pt3::RenderSystem::DrawShape(gs::Point3D(pts[i]->pos), rp);
        }
    }

    auto sel_vts = m_prop_view->GetSelectedIndices(GeoAttrClass::Vertex);
    if (!sel_vts.empty())
    {
        auto& vts = geo->GetAttr().GetVertices();
        for (auto i : sel_vts) {
            pt3::RenderSystem::DrawShape(gs::Point3D(vts[i]->point->pos), rp);
        }
    }

    auto sel_prims = m_prop_view->GetSelectedIndices(GeoAttrClass::Primitive);
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

}
