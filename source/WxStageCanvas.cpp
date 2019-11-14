#include "sopview/WxStageCanvas.h"
#include "sopview/RenderSystem.h"
#include "sopview/Evaluator.h"
#include "sopview/SceneTree.h"
#include "sopview/WxGeoProperty.h"
#include "sopview/Node.h"

#include <ee0/WxStagePage.h>
#include <ee0/EditOP.h>
#include <ee0/color_config.h>
#include <ee2/CamControlOP.h>
#include <blueprint/CompNode.h>

#include <node0/SceneNode.h>
#include <painting2/RenderSystem.h>
#include <painting2/OrthoCamera.h>
#include <painting2/Blackboard.h>
#include <painting2/WindowContext.h>
#include <painting3/MaterialMgr.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <painting3/PerspCam.h>
#include <painting3/OrthoCam.h>
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
    , m_viewports(*this)
{
    m_uv_op = std::make_shared<ee2::CamControlOP>(m_viewports.m_cam_uv,
        m_stage->GetSubjectMgr(), ee2::CamControlOP::MIDDLE_TAP);
}

bool WxStageCanvas::OnUpdate()
{
    if (!m_default_op) {
        m_default_op = m_stage->GetImpl().GetEditOP();
    }

    return false;
}

void WxStageCanvas::DrawBackground3D() const
{
    if (m_uv_mode) {
        return;
    }

    ee3::WxStageCanvas::DrawBackgroundGrids(10.0f, 0.2f);
//    ee3::WxStageCanvas::DrawBackgroundCross();
}

void WxStageCanvas::DrawForeground3D() const
{
    if (m_uv_mode || !m_stree) {
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

    rs.DrawNode3D(rc, m_stree);
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

    if (m_uv_mode)
    {
        // draw border
        rs.GetPainter().AddRect({ 0, 0 }, { RenderSystem::UV_SCALE, RenderSystem::UV_SCALE }, ee0::BLACK.ToABGR());

        // draw curr node uv
        auto display = GetDisplayNode();
        if (display) {
            rs.DrawNodeUV(*display);
        }
    }
    else
    {
        // draw all node 2d
        auto& nodes = eval->GetAllNodes();
        for (auto& n : nodes) {
            rs.DrawNode2D(*n.second, *n.first);
        }

        // draw selected node prop
        if (m_prop_view) {
            auto selected = GetSelectedNode();
            if (selected) {
                rs.DrawNodeAttr(*selected, *m_prop_view);
            }
        }
    }

    pt2::RenderSystem::DrawPainter(rs.GetPainter());
}

void WxStageCanvas::OnKeyDownImpl(wxKeyEvent& event)
{
    ee3::WxStageCanvas::OnKeyDownImpl(event);

    m_uv_mode = false;
    bool dirty = false;
    switch (event.GetKeyCode())
    {
    case '1':
        dirty = m_viewports.ChangeVP(ViewportType::Perspective);
        break;

    case '2':
        dirty = m_viewports.ChangeVP(ViewportType::Top);
        break;

    case '3':
        dirty = m_viewports.ChangeVP(ViewportType::Front);
        break;

    case '4':
        dirty = m_viewports.ChangeVP(ViewportType::Right);
        break;

    case '5':
        m_uv_mode = true;
        dirty = m_viewports.ChangeVP(ViewportType::UV);
        break;
    }

    if (dirty) {
        this->SetDirty();
    }
}

sop::NodePtr WxStageCanvas::GetSelectedNode() const
{
    if (!m_prop_view || !m_graph_stage) {
        return nullptr;
    }

    auto& selection = m_graph_stage->GetSelection();
    if (selection.Size() != 1) {
        return nullptr;
    }

    auto eval = m_stree->GetCurrEval();
    if (!eval) {
        return nullptr;
    }

    n0::SceneNodePtr node = nullptr;
    selection.Traverse([&](const ee0::GameObjWithPos& nwp)->bool
    {
        node = nwp.GetNode();
        return false;
    });
    assert(node);

    if (!node->HasUniqueComp<bp::CompNode>()) {
        return nullptr;
    }

    auto& cnode = node->GetUniqueComp<bp::CompNode>();
    return eval->QueryBackNode(*cnode.GetNode());
}

sop::NodePtr WxStageCanvas::GetDisplayNode() const
{
    if (!m_stree) {
        return nullptr;
    }
    auto eval = m_stree->GetCurrEval();
    if (!eval) {
        return nullptr;
    }

    auto& nodes = eval->GetAllNodes();
    for (auto& n : nodes) {
        if (n.first->get_type().is_derived_from<Node>()) {
            auto& sopv_node = static_cast<const Node&>(*n.first);
            if (sopv_node.GetDisplay()) {
                return n.second;
            }
        }
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// class WxStageCanvas::Viewports
//////////////////////////////////////////////////////////////////////////

WxStageCanvas::Viewports::Viewports(WxStageCanvas& canvas)
    : m_canvas(canvas)
{
    m_cam_3d = canvas.GetCamera();

    m_cam_xz = std::make_shared<pt3::OrthoCam>(pt3::OrthoCam::VP_XZ);
    m_cam_xy = std::make_shared<pt3::OrthoCam>(pt3::OrthoCam::VP_XY);
    m_cam_zy = std::make_shared<pt3::OrthoCam>(pt3::OrthoCam::VP_ZY);

    m_cam_uv = std::make_shared<pt2::OrthoCamera>();
}

bool WxStageCanvas::Viewports::ChangeVP(ViewportType type)
{
    if (type == m_curr_vp) {
        return false;
    }

    pt0::CameraPtr cam = nullptr;
    switch (type)
    {
    case ViewportType::Perspective:
        cam = m_cam_3d;
        break;
    case ViewportType::Top:
        cam = m_cam_xz;
        break;
    case ViewportType::Front:
        cam = m_cam_xy;
        break;
    case ViewportType::Right:
        cam = m_cam_zy;
        break;
    case ViewportType::UV:
        cam = m_cam_uv;
        break;
    }

    if (type == ViewportType::UV) {
        m_canvas.m_stage->GetImpl().SetEditOP(m_canvas.m_uv_op);
    } else {
        m_canvas.m_stage->GetImpl().SetEditOP(m_canvas.m_default_op);
    }

    if (cam)
    {
        m_canvas.m_stage->GetImpl().GetEditOP()->SetCamera(cam);

        auto& vp = m_canvas.GetViewport();
        cam->OnSize(vp.Width(), vp.Height());

        // update 2d render ctx
        auto& wc2 = pt2::Blackboard::Instance()->GetWindowContext();
        if (wc2)
        {
            if (cam->TypeID() == pt0::GetCamTypeID<pt2::OrthoCamera>()) {
                auto ortho2d = std::static_pointer_cast<pt2::OrthoCamera>(cam);
                wc2->SetView(ortho2d->GetPosition(), ortho2d->GetScale());
            } else {
                wc2->SetView({ 0, 0 }, 1);
            }
        }

        // update 3d render ctx
        auto& wc3 = pt3::Blackboard::Instance()->GetWindowContext();
        if (wc3) {
            wc3->SetProjection(cam->GetProjectionMat());
        }

        m_canvas.SetCamera(cam);
    }

    m_curr_vp = type;

    return true;
}

}
