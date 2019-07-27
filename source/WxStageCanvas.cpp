#include "intention/WxStageCanvas.h"
#include "intention/Node.h"

#include <ee0/WxStagePage.h>
#include <ee0/MsgHelper.h>
#include <blueprint/Node.h>
#include <blueprint/CompNode.h>
#include <intention/Evaluator.h>
#include <intention/Node.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node3/RenderSystem.h>
#include <painting3/MaterialMgr.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <painting3/PerspCam.h>
#include <everything/Evaluator.h>
#include <everything/Node.h>

namespace itt
{

WxStageCanvas::WxStageCanvas(ee0::WxStagePage* stage, ECS_WORLD_PARAM
                             const ee0::RenderContext& rc)
    : ee3::WxStageCanvas(stage, ECS_WORLD_VAR &rc)
{
}

void WxStageCanvas::SetEval(const std::shared_ptr<itt::Evaluator>& eval)
{
    if (m_eval == eval) {
        return;
    }

    m_eval = eval;
    SetDirty(); // refresh
}

bool WxStageCanvas::OnUpdate()
{
    return false;
}

void WxStageCanvas::DrawBackground3D() const
{
    ee3::WxStageCanvas::DrawBackgroundGrids(10.0f, 0.2f);
}

void WxStageCanvas::DrawForeground3D() const
{
    if (!m_eval) {
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

    auto& nodes = m_eval->GetAllNodes();
    for (auto& n : nodes)
    {
        auto bp_node = n.first;
        if (!bp_node->get_type().is_derived_from<itt::Node>()) {
            continue;
        }

        auto itt_node = static_cast<const Node*>(bp_node);
        if (!itt_node->GetDisplay()) {
            continue;
        }

        auto evt_node = n.second;
        if (!evt_node) {
            continue;
        }

        auto sn = evt_node->GetSceneNode();
        if (!sn) {
            continue;
        }

        pt3::RenderParams rp;

        if (itt_node->GetDisplay())
        {
            // draw face
            rp.type = pt3::RenderParams::DRAW_MESH;
            n3::RenderSystem::Draw(*sn, rp, rc);
        }

        // draw edge
        rp.type = pt3::RenderParams::DRAW_BORDER_MESH;
        n3::RenderSystem::Draw(*sn, rp, rc);
    }
}

}
