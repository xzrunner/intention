#include "sopview/WxStageCanvas.h"
#include "sopview/RenderSystem.h"
#include "sopview/Evaluator.h"
#include "sopview/SceneTree.h"

#include <painting2/RenderSystem.h>
#include <painting3/MaterialMgr.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <painting3/PerspCam.h>

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

    pt2::RenderSystem::DrawPainter(rs.GetPainter());
}

}
