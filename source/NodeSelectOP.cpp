#include "sopview/NodeSelectOP.h"
#include "sopview/RegistNodes.h"
#include "sopview/SceneTree.h"
#include "sopview/MessageID.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>
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

    if (m_stree && m_stree->ToNextLevel(node))
    {
        ee0::VariantSet vars;
        ee0::Variant var;
        var.m_type = ee0::VT_PVOID;
        var.m_val.pv = &node;
        vars.SetVariant("obj", var);
        m_stage.GetSubjectMgr()->NotifyObservers(MSG_SCENE_ROOT_TO_NEXT_LEVEL, vars);
    }

    return false;
}

}