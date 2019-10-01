#include "sopview/RightPopupMenu.h"
#include "sopview/WxNodePropsDlg.h"
#include "sopview/Node.h"

#include <ee0/EditPanelImpl.h>
#include <ee0/WxStagePage.h>
#include <blueprint/CompNode.h>
#include <blueprint/Node.h>

#include <node0/SceneNode.h>

#include <wx/window.h>

namespace
{

enum MenuID
{
    NodePropsMenu = 20000,
};

}

namespace sopv
{

RightPopupMenu::RightPopupMenu(ee0::WxStagePage* stage)
    : ee0::RightPopupMenu(stage, &stage->GetImpl())
    , m_stage(stage)
{
}

void RightPopupMenu::SetRightPopupMenu(wxMenu& menu, int x, int y)
{
    auto& selection = m_stage->GetSelection();
    if (selection.Size() == 1)
    {
        m_stage->Bind(wxEVT_COMMAND_MENU_SELECTED,
            &ee0::EditPanelImpl::OnRightPopupMenu, &m_stage->GetImpl(), NodePropsMenu);
        menu.Append(NodePropsMenu, "Props...", "Node Props");
    }
}

void RightPopupMenu::OnRightPopupMenu(int id)
{
    switch (id)
    {
    case NodePropsMenu:
    {
        auto& selection = m_stage->GetSelection();
        assert(selection.Size() == 1);
        n0::SceneNodePtr node = nullptr;
        selection.Traverse([&](const ee0::GameObjWithPos& nwp)->bool
        {
            node = nwp.GetNode();
            return false;
        });
        assert(node && node->HasUniqueComp<bp::CompNode>());

        auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
        if (bp_node->get_type().is_derived_from<Node>())
        {
            auto itt_node = std::static_pointer_cast<Node>(bp_node);
            auto& props = itt_node->GetProps();
            if (props) {
                WxNodePropsDlg dlg(m_stage, *props);
                if (dlg.ShowModal() == wxID_OK) {
                    dlg.StoreToProps(*props);
                }
            }
        }
    }
        break;
    }
}

}