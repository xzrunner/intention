#include "sopview/NodeSelectOP.h"
#include "sopview/RegistNodes.h"
#include "sopview/SceneTree.h"
#include "sopview/MessageID.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>
#include <ee0/MsgHelper.h>
#include <ee0/WxStageCanvas.h>
#include <blueprint/CompNode.h>

#include <node0/SceneNode.h>

namespace sopv
{

NodeSelectOP::NodeSelectOP(const std::shared_ptr<pt0::Camera>& camera,
                           ee0::WxStagePage& stage)
    : bp::NodeSelectOP(camera, stage, ee2::CamControlOP::DEFAULT_FLAG, false)
{
}

bool NodeSelectOP::OnMouseLeftDClick(int x, int y)
{
    if (bp::NodeSelectOP::OnMouseLeftDClick(x, y)) {
        return true;
    }

    auto& selection = m_stage.GetSelection();
    if (selection.Size() != 1) {
        return false;
    }

    n0::SceneNodePtr node = nullptr;
    selection.Traverse([&](const ee0::GameObjWithPos& nwp)->bool {
        node = nwp.GetNode();
        return true;
    });

    if (!node || !node->HasUniqueComp<bp::CompNode>()) {
        return false;
    }

    auto& cnode = node->GetUniqueComp<bp::CompNode>();
    auto bp_node = cnode.GetNode();
    if (!bp_node) {
        return false;
    }

    auto& dev = m_stage.GetImpl().GetCanvas()->GetRenderDevice();
    if (m_stree && m_stree->Push(dev, node) ||
        bp_node->get_type().is_derived_from<node::Compound>()) {
        ee0::MsgHelper::SendObjMsg(*m_stage.GetSubjectMgr(), node, MSG_SCENE_ROOT_TO_NEXT_LEVEL);
        return true;
    }

    //auto bp_type = bp_node->get_type();
    //if (bp_type == rttr::type::get<node::AttributeVOP>())
    //{
    //}
    //else if (bp_type.is_derived_from<node::Compound>())
    //{

    //}

    return false;
}

}