#include "sopview/Serializer.h"
#include "sopview/RegistNodes.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>
#include <ee0/MsgHelper.h>
#include <blueprint/CommentaryNodeHelper.h>
#include <blueprint/NSCompNode.h>
#include <blueprint/MessageID.h>
#include <blueprint/CompNode.h>

#include <memmgr/LinearAllocator.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/AABBSystem.h>
#include <node2/CompBoundingBox.h>
#include <ns/CompFactory.h>
#include <ns/CompSerializer.h>
#include <ns/N0CompComplex.h>

namespace sopv
{

void Serializer::Init()
{
    ns::CompSerializer::Instance()->AddFromJsonFunc(n0::CompComplex::TYPE_NAME,
        [](n0::NodeComp& comp, const std::string& dir, const rapidjson::Value& val)
    {
        auto& ccomplex = static_cast<n0::CompComplex&>(comp);

        ns::N0CompComplex seri;
        mm::LinearAllocator alloc;
        seri.LoadFromJson(alloc, dir, val);
        seri.StoreToMem(ccomplex);

        bp::NSCompNode::LoadConnection(ccomplex.GetAllChildren(), val["nodes"]);
    }, true);

    ns::CompSerializer::Instance()->AddToJsonFunc(n0::CompComplex::TYPE_NAME,
        [](const n0::NodeComp& comp, const std::string& dir, rapidjson::Value& val,
            rapidjson::MemoryPoolAllocator<>& alloc, bool skip_asset)->bool
    {
        auto& ccomplex = static_cast<const n0::CompComplex&>(comp);

        ns::N0CompComplex seri;
        seri.LoadFromMem(ccomplex);
        seri.StoreToJson(dir, val, alloc);

        bp::NSCompNode::StoreConnection(ccomplex.GetAllChildren(), val["nodes"], alloc);

        return true;
    }, true);
}

void Serializer::LoadFromJson(ee0::WxStagePage& stage, const n0::SceneNodePtr& root,
                              const rapidjson::Value& val, const std::string& dir)
{
    auto sub_mgr = stage.GetSubjectMgr();

    sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
    sub_mgr->NotifyObservers(ee0::MSG_SCENE_NODE_CLEAR);

    n0::CompAssetPtr casset = ns::CompFactory::Instance()->CreateAsset(val, dir);
    if (casset)
    {
        if (root->HasSharedComp<n0::CompAsset>()) {
            root->RemoveSharedComp<n0::CompAsset>();
        }
        root->AddSharedCompNoCreate<n0::CompAsset>(casset);
    }
    else
    {
        casset = root->GetSharedCompPtr<n0::CompAsset>();
    }

    InitParentChildren(root);

    // FIXME: reinsert, for send insert msg to other panel
    if (root->HasSharedComp<n0::CompComplex>())
    {
        auto& ccomplex = root->GetSharedComp<n0::CompComplex>();
        auto children = ccomplex.GetAllChildren();
        ccomplex.RemoveAllChildren();
        for (auto& c : children) {
            ee0::MsgHelper::InsertNode(*sub_mgr, c, false);
        }
    }

    // connection
    bp::CommentaryNodeHelper::InsertNodeToCommentary(stage);
    sub_mgr->NotifyObservers(bp::MSG_BP_CONN_REBUILD);

    // aabb
	if (root->HasUniqueComp<n2::CompBoundingBox>())
	{
		auto& cbb = root->GetUniqueComp<n2::CompBoundingBox>();
		auto aabb = n2::AABBSystem::Instance()->GetBounding(*root);
		cbb.SetSize(*root, aabb);
	}

	sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void Serializer::StoreToJson(const n0::SceneNodePtr& root, const std::string& dir,
                             rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc)
{
    rapidjson::Value bp_val;

    assert(root->HasSharedComp<n0::CompComplex>());
    ns::CompSerializer::Instance()->ToJson(
        root->GetSharedComp<n0::CompComplex>(), dir, bp_val, alloc, false
    );

    val.AddMember("graph", bp_val, alloc);
}

void Serializer::InitParentChildren(const n0::SceneNodePtr& node)
{
    assert(node);

    if (!node->HasSharedComp<n0::CompComplex>()) {
        return;
    }

    std::shared_ptr<node::Compound> parent = nullptr;
    if (node->HasUniqueComp<bp::CompNode>())
    {
        auto& cnode = node->GetUniqueComp<bp::CompNode>();
        auto bp_node = cnode.GetNode();
        auto bp_type = bp_node->get_type();
        if (bp_type.is_derived_from<node::Compound>()) {
            parent = std::static_pointer_cast<node::Compound>(bp_node);
        }
    }

    auto& ccomplex = node->GetSharedComp<n0::CompComplex>();

    if (parent)
    {
        parent->ClearAllChildren();
        for (auto& c : ccomplex.GetAllChildren())
        {
            if (!c->HasUniqueComp<bp::CompNode>()) {
                continue;
            }
            auto& cnode = c->GetUniqueComp<bp::CompNode>();
            auto bp_node = cnode.GetNode();
            if (bp_node) {
                parent->AddChild(bp_node);
            }
        }
    }

    for (auto& c : ccomplex.GetAllChildren()) {
        InitParentChildren(c);
    }
}

}