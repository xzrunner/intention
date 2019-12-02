#include "sopview/WxToolbarPanel.h"
#include "sopview/WxGeoProperty.h"
#include "sopview/WxNodeProperty.h"
#include "sopview/MessageID.h"
#include "sopview/Node.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>
#include <ee0/WxNavigationBar.h>
#include <blueprint/CompNode.h>
#include <vopview/WxNodeProperty.h>

#include <guard/check.h>
#include <node0/SceneNode.h>

namespace sopv
{

WxToolbarPanel::WxToolbarPanel(wxWindow* parent, ee0::WxStagePage* graph_stage,
                               const std::shared_ptr<SceneTree>& stree)
	: wxPanel(parent)
    , m_graph_stage(graph_stage)
{
	InitLayout(stree);

    auto& sub_mgr = graph_stage->GetSubjectMgr();
    sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
    sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);
    sub_mgr->RegisterObserver(MSG_SCENE_ROOT_TO_NEXT_LEVEL, this);
}

void WxToolbarPanel::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_NODE_SELECTION_INSERT:
		OnSelectionInsert(variants);
		break;
    case ee0::MSG_NODE_SELECTION_CLEAR:
        OnSelectionClear(variants);
        break;
    case MSG_SCENE_ROOT_TO_NEXT_LEVEL:
        OnRootToNextLevel(variants);
        break;
	}
}

void WxToolbarPanel::ChangeEditOpMode(EditOpMode mode)
{
    if (mode == m_mode) {
        return;
    }

    m_mode = mode;
    switch (mode)
    {
    case EditOpMode::SOP:
        m_sop_node_prop->Show();
        m_vop_node_prop->Hide();
        break;
    case EditOpMode::VOP:
        m_sop_node_prop->Hide();
        m_vop_node_prop->Show();
        Layout();
        break;
    default:
        assert(0);
    }
}

void WxToolbarPanel::InitLayout(const std::shared_ptr<SceneTree>& stree)
{
    auto sub_mgr = m_graph_stage->GetSubjectMgr();

	auto sizer = new wxBoxSizer(wxVERTICAL);
    // geometry spreadsheet
    sizer->Add(m_geo_prop = new WxGeoProperty(this, sub_mgr, stree), wxEXPAND);
    // property
	sizer->Add(m_sop_node_prop = new WxNodeProperty(this, sub_mgr, stree), wxEXPAND);
    sizer->Add(m_vop_node_prop = new vopv::WxNodeProperty(this, sub_mgr), wxEXPAND);
    m_vop_node_prop->Hide();
    // nav bar
    m_nav_bar = new ee0::WxNavigationBar(this);
    m_nav_bar->Push("obj");
    m_nav_bar->SetSeekCallback([&](size_t depth)
    {
        ee0::VariantSet vars;

        ee0::Variant var;
        var.m_type = ee0::VT_ULONG;
        var.m_val.ul = depth;
        vars.SetVariant("depth", var);

        m_graph_stage->GetSubjectMgr()->NotifyObservers(MSG_SCENE_ROOT_SEEK_TO_PREV_LEVEL, vars);
    });
    sizer->Add(m_nav_bar);

	SetSizer(sizer);
}

void WxToolbarPanel::OnSelectionInsert(const ee0::VariantSet& variants)
{
	auto var_obj = variants.GetVariant("obj");
	GD_ASSERT(var_obj.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj obj = *static_cast<const ee0::GameObj*>(var_obj.m_val.pv);
	GD_ASSERT(GAME_OBJ_VALID(obj), "err scene obj");

    m_geo_prop->LoadFromNode(obj);

	auto& cnode = obj->GetUniqueComp<bp::CompNode>();
    auto prop = GetCurrNodeProp();
    assert(prop);
    prop->LoadFromNode(obj, cnode.GetNode());
}

void WxToolbarPanel::OnSelectionClear(const ee0::VariantSet& variants)
{
    auto prop = GetCurrNodeProp();
    assert(prop);
    prop->Clear();
}

void WxToolbarPanel::OnRootToNextLevel(const ee0::VariantSet& variants)
{
    auto var_obj = variants.GetVariant("obj");
    GD_ASSERT(var_obj.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj obj = *static_cast<const ee0::GameObj*>(var_obj.m_val.pv);
    GD_ASSERT(GAME_OBJ_VALID(obj), "err scene obj");

    if (!obj->HasUniqueComp<bp::CompNode>()) {
        return;
    }
    auto& cnode = obj->GetUniqueComp<bp::CompNode>();
    auto& bp_node = cnode.GetNode();
    if (!bp_node->get_type().is_derived_from<Node>()) {
        return;
    }

    auto sopv_node = std::static_pointer_cast<Node>(bp_node);
    m_nav_bar->Push(sopv_node->GetName());
}

bp::WxNodeProperty* WxToolbarPanel::GetCurrNodeProp() const
{
    switch (m_mode)
    {
    case EditOpMode::SOP:
        return m_sop_node_prop;
    case EditOpMode::VOP:
        return m_vop_node_prop;
    default:
        assert(0);
        return nullptr;
    }
}

}