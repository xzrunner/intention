#include "sopview/WxGraphPage.h"
#include "sopview/MessageID.h"
#include "sopview/SceneTree.h"
#include "sopview/Node.h"
#include "sopview/Evaluator.h"
#include "sopview/RegistNodes.h"

#include <ee0/SubjectMgr.h>
#include <ee0/EditPanelImpl.h>
#include <ee0/GameObj.h>
#include <ee0/WxStageCanvas.h>
#include <ee0/MsgHelper.h>
#include <blueprint/Blueprint.h>
#include <blueprint/MessageID.h>
#include <blueprint/NodeHelper.h>
#include <blueprint/NSCompNode.h>
#include <blueprint/Node.h>
#include <blueprint/CompNode.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/CompBoundingBox.h>
#include <node2/AABBSystem.h>
#include <js/RapidJsonHelper.h>
#include <ns/CompFactory.h>

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
        auto node = std::make_shared<node::Geometry>();
        node->SetName("obj");
        cnode.SetNode(node);
    }
    m_stree->Add(m_root);
#ifdef SOPV_SCENE_TREE_DUMMY_ROOT
    m_stree->ToNextLevel(m_root);
#endif // SOPV_SCENE_TREE_DUMMY_ROOT

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
        dirty = ChangeSceneRoot(variants);
        break;
    case MSG_SCENE_ROOT_SEEK_TO_PREV_LEVEL:
        dirty = PathSeekToPrev(variants);
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
    m_root->GetSharedComp<n0::CompComplex>().Traverse(func, inverse);
}

void WxGraphPage::SetPreviewCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas)
{
    m_preview_canvas = canvas;
}

bool WxGraphPage::InsertSceneObj(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("obj");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
    GD_ASSERT(obj, "err scene obj");

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

bool WxGraphPage::ChangeSceneRoot(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("obj");
    GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
    GD_ASSERT(obj, "err scene obj");

    if (m_root == *obj) {
        return false;
    } else {
        m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
        m_root = *obj;
        return true;
    }
}

bool WxGraphPage::PathSeekToPrev(const ee0::VariantSet& variants)
{
    auto var = variants.GetVariant("depth");
    GD_ASSERT(var.m_type == ee0::VT_ULONG, "no var in vars: obj");
    auto depth = var.m_val.ul;
    if (m_stree->SetDepth(depth)) {
        m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
        m_root = m_stree->GetCurrNode();
        return true;
    } else {
        return false;
    }
}

}
