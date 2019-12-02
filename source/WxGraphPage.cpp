#include "sopview/WxGraphPage.h"
#include "sopview/MessageID.h"
#include "sopview/SceneTree.h"
#include "sopview/Evaluator.h"
#include "sopview/RegistNodes.h"
#include "sopview/NodeSelectOP.h"
#include "sopview/SOPView.h"
#include "sopview/RightPopupMenu.h"
#include "sopview/WxStageCanvas.h"
#include "sopview/WxToolbarPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee0/WxStageCanvas.h>
#include <ee2/ArrangeNodeCfg.h>
#include <blueprint/MessageID.h>
#include <blueprint/CompNode.h>
#include <blueprint/ArrangeNodeOP.h>
#include <blueprint/ConnectPinOP.h>
#include <vopview/WxGraphPage.h>
#include <vopview/VOPView.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node0/NodeFlagsHelper.h>
#include <node0/NodeFlags.h>
#include <node2/CompBoundingBox.h>
#include <ns/NodeFactory.h>
#include <sop/node/AttributeVOP.h>

namespace
{

const uint32_t MESSAGES[] =
{
    ee0::MSG_SCENE_NODE_INSERT,
    ee0::MSG_SCENE_NODE_DELETE,
    ee0::MSG_SCENE_NODE_CLEAR,

    bp::MSG_BP_CONN_INSERT,
    bp::MSG_BP_CONN_DELETE,
    bp::MSG_BP_CONN_REBUILD,
    bp::MSG_BP_NODE_PROP_CHANGED,

    sopv::MSG_CLEAR_NODE_DISPLAY_TAG,
    sopv::MSG_SCENE_ROOT_TO_NEXT_LEVEL,
    sopv::MSG_SCENE_ROOT_SEEK_TO_PREV_LEVEL,
    sopv::MSG_REFRESH_PREVIEW_CANVAS,
};

}

namespace sopv
{

WxGraphPage::WxGraphPage(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
                         const ee0::GameObj& root)
    : ee0::WxStagePage(parent, sub_mgr)
    , m_root(root)
{
    m_stree = std::make_shared<SceneTree>();
    if (!m_root->HasUniqueComp<bp::CompNode>())
    {
        auto& cnode = m_root->AddUniqueComp<bp::CompNode>();
        auto node = std::make_shared<node::Subnetwork>();
        node->SetName("obj");
        cnode.SetNode(node);
    }
    m_stree->Add(m_root);
#ifdef SOPV_SCENE_TREE_DUMMY_ROOT
    m_stree->ToNextLevel(m_root);
#endif // SOPV_SCENE_TREE_DUMMY_ROOT

    GetImpl().SetPopupMenu(std::make_shared<RightPopupMenu>(this));

    for (auto& msg : MESSAGES) {
        m_sub_mgr->RegisterObserver(msg, this);
    }
}

WxGraphPage::~WxGraphPage()
{
    for (auto& msg : MESSAGES) {
        m_sub_mgr->UnregisterObserver(msg, this);
    }
}

void WxGraphPage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
    ee0::WxStagePage::OnNotify(msg, variants);

    if (m_mode == EditOpMode::VOP)
    {
        if (msg == MSG_SCENE_ROOT_SEEK_TO_PREV_LEVEL)
        {
            if (PathPopToPrev(variants)) {
                m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
                m_preview_canvas->SetDirty();
            }
        }
        else
        {
            m_vop_view->OnNotify(msg, variants);

            switch (msg)
            {
            case bp::MSG_BP_CONN_INSERT:
            case bp::MSG_BP_CONN_DELETE:
            case bp::MSG_BP_CONN_REBUILD:
            case bp::MSG_BP_NODE_PROP_CHANGED:
            {
                auto node = m_stree->GetLastNode();
                if (node->HasUniqueComp<bp::CompNode>()) {
                    auto bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
                    m_stree->GetLastEval()->OnNodeChanged(bp_node);
                }
            }
            break;
            }
        }
        return;
    }

	bool dirty = false;
	switch (msg)
	{
	case ee0::MSG_SCENE_NODE_INSERT:
		dirty = InsertSceneObj(variants);
		break;
	case ee0::MSG_SCENE_NODE_DELETE:
		dirty = DeleteSceneObj(variants);
		break;
	case ee0::MSG_SCENE_NODE_CLEAR:
        dirty = m_stree->Clear();
		break;

    case bp::MSG_BP_CONN_INSERT:
        dirty = AfterInsertNodeConn(variants);
        break;
    case bp::MSG_BP_CONN_DELETE:
        dirty = BeforeDeleteNodeConn(variants);
        break;
    case bp::MSG_BP_CONN_REBUILD:
    {
        auto eval = m_stree->GetCurrEval();
        assert(eval);
        eval->OnRebuildConnection();
        dirty = true;
    }
        break;
    case bp::MSG_BP_NODE_PROP_CHANGED:
        dirty = UpdateNodeProp(variants);
        break;

    case MSG_CLEAR_NODE_DISPLAY_TAG:
        m_stree->ClearNodeDisplayTag();
        dirty = true;
        break;
    case MSG_SCENE_ROOT_TO_NEXT_LEVEL:
        dirty = PathPushToNext(variants);
        break;
    case MSG_SCENE_ROOT_SEEK_TO_PREV_LEVEL:
        dirty = PathPopToPrev(variants);
        break;
    case MSG_REFRESH_PREVIEW_CANVAS:
        m_preview_canvas->SetDirty();
        break;
	}

	if (dirty)
    {
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
        m_preview_canvas->SetDirty();
	}
}

void WxGraphPage::Traverse(std::function<bool(const ee0::GameObj&)> func,
                           const ee0::VariantSet& variants , bool inverse) const
{
    switch (m_mode)
    {
    case EditOpMode::SOP:
        m_root->GetSharedComp<n0::CompComplex>().Traverse(func, inverse);
        break;
    case EditOpMode::VOP:
        m_vop_view->GetRootNode()->GetSharedComp<n0::CompComplex>().Traverse(func, inverse);
        break;
    default:
        assert(0);
    }
}

void WxGraphPage::SetupCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas,
                              const std::shared_ptr<ee0::WxStageCanvas>& preview_canvas)
{
    GetImpl().SetCanvas(canvas);

    m_preview_canvas = preview_canvas;
    std::static_pointer_cast<WxStageCanvas>(preview_canvas)->SetSceneTree(m_stree);

    if (preview_canvas)
    {
        auto sop_preview_canvas = std::static_pointer_cast<WxStageCanvas>(preview_canvas);
        sop_preview_canvas->SetPropView(m_toolbar->GetGeoPropView());
        sop_preview_canvas->SetGraphStage(this);
    }

    auto cam = canvas->GetCamera();
    m_sop_op = CreateSopEditOP(cam);
    m_vop_op = CreateVopEditOP(cam);

    GetImpl().SetEditOP(m_sop_op);
}

ee0::EditOPPtr WxGraphPage::CreateSopEditOP(const std::shared_ptr<pt0::Camera>& cam)
{
    auto select_op_sop = std::make_shared<NodeSelectOP>(cam, *this);
    select_op_sop->SetSceneTree(m_stree);

    ee2::ArrangeNodeCfg cfg;
    cfg.is_auto_align_open = false;
    cfg.is_deform_open     = false;
    cfg.is_offset_open     = false;
    cfg.is_rotate_open     = false;
    auto arrange_op_sop = std::make_shared<bp::ArrangeNodeOP>(
        cam, *this, ECS_WORLD_VAR cfg, select_op_sop
    );

    auto& nodes = SOPView::Instance()->GetAllNodes();
    auto op = std::make_shared<bp::ConnectPinOP>(cam, *this, nodes);
    op->SetPrevEditOP(arrange_op_sop);
    return op;
}

ee0::EditOPPtr WxGraphPage::CreateVopEditOP(const std::shared_ptr<pt0::Camera>& cam)
{
    ee2::ArrangeNodeCfg cfg;
    cfg.is_auto_align_open = false;
    cfg.is_deform_open     = false;
    cfg.is_offset_open     = false;
    cfg.is_rotate_open     = false;
    auto arrange_op_sop = std::make_shared<bp::ArrangeNodeOP>(
        cam, *this, ECS_WORLD_VAR cfg, nullptr
    );

    auto& nodes = vopv::VOPView::Instance()->GetAllNodes();
    auto op = std::make_shared<bp::ConnectPinOP>(cam, *this, nodes);
    op->SetPrevEditOP(arrange_op_sop);
    return op;
}

bool WxGraphPage::InsertSceneObj(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("obj");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
    GD_ASSERT(obj, "err scene obj");

    // prepare CompComplex
    if ((*obj)->HasUniqueComp<bp::CompNode>())
    {
        auto& bp_node = (*obj)->GetUniqueComp<bp::CompNode>().GetNode();
        auto type = bp_node->get_type();
        if (type.is_derived_from<node::Compound>())
        {
            auto& bp_children = std::static_pointer_cast<node::Compound>(bp_node)->GetAllChildren();
            if (!(*obj)->HasSharedComp<n0::CompComplex>())
            {
                auto& ccomplex = (*obj)->AddSharedComp<n0::CompComplex>();
                for (auto& child : bp_children)
                {
                    auto child_node = ns::NodeFactory::Create2D();

                    auto& cnode = child_node->AddUniqueComp<bp::CompNode>();
                    cnode.SetNode(child);

                    //if (child->get_type().is_derived_from<node::Subnetwork>()) {
                    //    child_node->AddSharedComp<n0::CompComplex>();
                    //}

                    auto& caabb = child_node->GetUniqueComp<n2::CompBoundingBox>();
                    auto& style = child->GetStyle();
                    caabb.SetSize(*child_node, sm::rect(style.width, style.height));

                    n0::NodeFlagsHelper::SetFlag<n0::NodeNotVisible>(*child_node, true);

                    ccomplex.AddChild(child_node);
                }
            }
            assert((*obj)->HasSharedComp<n0::CompComplex>()
                && (*obj)->GetSharedComp<n0::CompComplex>().GetAllChildren().size() == bp_children.size());
        }
    }

    return m_stree->Add(*obj);
}

bool WxGraphPage::DeleteSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

    return m_stree->Remove(*obj);
}

bool WxGraphPage::AfterInsertNodeConn(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("conn");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: conn");
    const std::shared_ptr<bp::Connecting>* conn = static_cast<const std::shared_ptr<bp::Connecting>*>(var.m_val.pv);
    GD_ASSERT(conn, "err conn");

    auto eval = m_stree->GetCurrEval();
    assert(eval);
    eval->OnConnected(**conn);

    return true;
}

bool WxGraphPage::BeforeDeleteNodeConn(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("conn");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: conn");
    const std::shared_ptr<bp::Connecting>* conn = static_cast<const std::shared_ptr<bp::Connecting>*>(var.m_val.pv);
    GD_ASSERT(conn, "err conn");

    auto eval = m_stree->GetCurrEval();
    assert(eval);
    eval->OnDisconnecting(**conn);

    return true;
}

bool WxGraphPage::UpdateNodeProp(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("obj");
    if (var.m_type == ee0::VT_EMPTY) {
        return false;
    }

    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
    GD_ASSERT(obj, "err scene obj");

    if ((*obj)->HasUniqueComp<bp::CompNode>())
    {
        auto& bp_node = (*obj)->GetUniqueComp<bp::CompNode>().GetNode();
        auto eval = m_stree->GetCurrEval();
        assert(eval);
        eval->OnNodePropChanged(bp_node);
        return true;
    }
    else
    {
        return false;
    }
}

bool WxGraphPage::PathPushToNext(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("obj");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
    GD_ASSERT(obj, "err scene obj");

    if (m_root == *obj) {
        return false;
    }

    assert((*obj)->HasUniqueComp<bp::CompNode>());
    auto& cnode = (*obj)->GetUniqueComp<bp::CompNode>();
    auto bp_node = cnode.GetNode();
    if (bp_node && bp_node->get_type() == rttr::type::get<node::AttributeVOP>())
    {
        if (!m_vop_view) {
            m_vop_view = std::make_shared<vopv::WxGraphPage>(m_parent, nullptr, m_root);
            m_vop_view->SetPreviewCanvas(m_preview_canvas);
        }

        // reset visible
        // todo: move to vopview's SceneTree
        assert((*obj)->HasSharedComp<n0::CompComplex>());
        auto& ccomplex = (*obj)->GetSharedComp<n0::CompComplex>();
        for (auto& c : ccomplex.GetAllChildren()) {
            n0::NodeFlagsHelper::SetFlag<n0::NodeNotVisible>(*c, false);
        }

        auto eval = m_stree->GetLastEval();
        assert(eval);
        auto attr_vop = std::static_pointer_cast<sop::node::AttributeVOP>(eval->QueryBackNode(*bp_node));
        assert(attr_vop);
        m_vop_view->SetRootNode(*obj, attr_vop->GetEval());
        ChangeEditOpMode(EditOpMode::VOP);
    }
    else
    {
        m_root = *obj;
    }

    m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);

    return true;
}

bool WxGraphPage::PathPopToPrev(const ee0::VariantSet& variants)
{
    auto var_depth = variants.GetVariant("depth");
    GD_ASSERT(var_depth.m_type == ee0::VT_ULONG, "no var in vars: obj");
    auto depth = var_depth.m_val.ul;
    if (!m_stree->SetDepth(depth)) {
        return false;
    }

    auto node = m_stree->GetCurrNode();
    assert(node->HasUniqueComp<bp::CompNode>());
    auto& cnode = node->GetUniqueComp<bp::CompNode>();
    auto bp_node = cnode.GetNode();
    if (bp_node && bp_node->get_type() == rttr::type::get<node::AttributeVOP>())
    {
        assert(0);
        //m_vop_view->SetRootNode(node);
        //ChangeEditOpMode(EditOpMode::VOP);
    }
    else
    {
        m_root = m_stree->GetCurrNode();
        ChangeEditOpMode(EditOpMode::SOP);
    }

    m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);

    return true;
}

void WxGraphPage::ChangeEditOpMode(EditOpMode mode)
{
    m_toolbar->ChangeEditOpMode(mode);

    if (mode == m_mode) {
        return;
    }

    m_mode = mode;
    switch (mode)
    {
    case EditOpMode::SOP:
        GetImpl().SetEditOP(m_sop_op);
        break;
    case EditOpMode::VOP:
        GetImpl().SetEditOP(m_vop_op);
        break;
    default:
        assert(0);
    }
}

}
