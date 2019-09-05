#include "intention/Serializer.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>
#include <ee0/MsgHelper.h>
#include <blueprint/CommentaryNodeHelper.h>
#include <blueprint/NSCompNode.h>
#include <blueprint/MessageID.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/AABBSystem.h>
#include <node2/CompBoundingBox.h>
#include <ns/CompFactory.h>
#include <ns/CompSerializer.h>

namespace itt
{

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
    // FIXME: reinsert, for send insert msg to other panel
    if (root->HasSharedComp<n0::CompComplex>())
    {
        auto& ccomplex = root->GetSharedComp<n0::CompComplex>();
        auto nodes = ccomplex.GetAllChildren();
        ccomplex.RemoveAllChildren();
        for (auto& node : nodes) {
            ee0::MsgHelper::InsertNode(*sub_mgr, node, false);
        }
    }

    // copy from
    // LoadFromFileExt(filepath);
    bp::CommentaryNodeHelper::InsertNodeToCommentary(stage);
    auto& ccomplex = root->GetSharedComp<n0::CompComplex>();
    bp::NSCompNode::LoadConnection(ccomplex.GetAllChildren(), val["nodes"]);
    sub_mgr->NotifyObservers(bp::MSG_BP_CONN_REBUILD);

	if (root->HasUniqueComp<n2::CompBoundingBox>())
	{
		auto& cbb = root->GetUniqueComp<n2::CompBoundingBox>();
		auto aabb = n2::AABBSystem::Instance()->GetBounding(*root);
		cbb.SetSize(*root, aabb);
	}

	sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void Serializer::StoreToJson(const ee0::WxStagePage& stage, const n0::SceneNodePtr& root,
                             const std::string& dir, rapidjson::Value& val,
                             rapidjson::MemoryPoolAllocator<>& alloc)
{
    rapidjson::Value graph_val;

    assert(root->HasSharedComp<n0::CompComplex>());
    ns::CompSerializer::Instance()->ToJson(
        root->GetSharedComp<n0::CompComplex>(), dir, graph_val, alloc
    );

    // connection
    auto& ccomplex = root->GetSharedComp<n0::CompComplex>();
    bp::NSCompNode::StoreConnection(ccomplex.GetAllChildren(), graph_val["nodes"], alloc);

    val.AddMember("graph", graph_val, alloc);
}

}