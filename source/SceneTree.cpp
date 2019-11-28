#include "sopview/SceneTree.h"
#include "sopview/Evaluator.h"
#include "sopview/Node.h"
#include "sopview/RegistNodes.h"
#include "sopview/SOPAdapter.h"

#include <blueprint/CompNode.h>
#include <blueprint/Node.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node0/NodeFlagsHelper.h>
#include <node0/NodeFlags.h>
#include <ns/NodeFactory.h>
#include <sop/node/Subnetwork.h>

#include <assert.h>

namespace
{

void RebuildBackFromFront(std::shared_ptr<sop::node::Subnetwork>& dst,
                          const std::shared_ptr<sopv::node::Subnetwork>& src,
                          const sopv::Evaluator& eval)
{
    dst->ClearChildren();
    for (auto& c : src->children)
    {
        auto dst_c = eval.QueryBackNode(*c);
        if (!dst_c) {
            continue;
        }
        sop::node::Subnetwork::AddChild(dst, dst_c);

        //// calc again, for expr which need level info
        //sopv::SOPAdapter::UpdatePropBackFromFront(*c, *dst_c, eval);
    }

    // calc again, for expr which need level info
    for (auto& c : src->children)
    {
        auto dst_c = eval.QueryBackNode(*c);
        sopv::SOPAdapter::UpdatePropBackFromFront(*c, *dst_c, eval);
    }
}

}

namespace sopv
{

SceneTree::SceneTree()
{
#ifdef SOPV_SCENE_TREE_DUMMY_ROOT
    InitDummyRoot();
#endif // SOPV_SCENE_TREE_DUMMY_ROOT
}

void SceneTree::AfterLoadFromFile()
{
    SetupCurrNode();
}

bool SceneTree::Add(const n0::SceneNodePtr& node)
{
#ifndef SOPV_SCENE_TREE_DUMMY_ROOT
    if (m_path.parts.empty())
    {
        auto eval = std::make_shared<Evaluator>();
        if (m_is_loading) {
            eval->EnableUpdateSopEval(false);
        }
        m_eval_cache.insert({ node, eval });
        m_path.parts.push_back(PathPart(node, eval));

        return true;
    }
#endif // SOPV_SCENE_TREE_DUMMY_ROOT

    assert(!m_path.parts.empty());

    // reset flags
    ClearNodeDisplayTag();

    // update scene node
    auto& curr = m_path.parts.back();
    assert(curr.node->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = curr.node->GetSharedComp<n0::CompComplex>();
    ccomplex.AddChild(node);

    if (node->HasUniqueComp<bp::CompNode>())
    {
        auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
        auto& curr = m_path.parts.back();

        // update front
        if (curr.node->HasUniqueComp<bp::CompNode>())
        {
            auto bp_parent = curr.node->GetUniqueComp<bp::CompNode>().GetNode();
            if (bp_parent && bp_parent->get_type().is_derived_from<node::Subnetwork>())
            {
                auto sub_nw = std::static_pointer_cast<node::Subnetwork>(bp_parent);
                sub_nw->children.push_back(bp_node);
            }
        }

        // front eval cb
        m_path.parts.back().eval->OnAddNode(*bp_node);

        // update back
        if (m_path.parts.size() > 1)
        {
            auto& prev_eval = m_path.parts[m_path.parts.size() - 2].eval;
            auto& curr_node = m_path.parts.back().node;
            assert(curr_node->HasUniqueComp<bp::CompNode>());
            auto parent = prev_eval->QueryBackNode(*curr_node->GetUniqueComp<bp::CompNode>().GetNode());
            auto child = m_path.parts.back().eval->QueryBackNode(*bp_node);
            assert(parent->get_type().is_derived_from<sop::node::Subnetwork>());
            sop::node::Subnetwork::AddChild(std::static_pointer_cast<sop::node::Subnetwork>(parent), child);
        }

        // prepare ccomplex
        auto type = bp_node->get_type();
        if (type.is_derived_from<node::Subnetwork>())
        {
            if (!node->HasSharedComp<n0::CompComplex>()) {
                node->AddSharedComp<n0::CompComplex>();
            }
        }

        // update flags
        if (type.is_derived_from<Node>()) {
            auto sopv_node = std::static_pointer_cast<Node>(bp_node);
            if (!m_is_loading) {
                sopv_node->SetDisplay(true);
            }
        }
    }

    return true;
}

bool SceneTree::Remove(const n0::SceneNodePtr& node)
{
    if (m_path.parts.empty()) {
        return false;
    }

    if (node == m_path.parts.front().node)
    {
        return Clear();
    }
    else
    {
        // update scene node
        auto& curr = m_path.parts.back();
        assert(curr.node->HasSharedComp<n0::CompComplex>());
        auto& ccomplex = curr.node->GetSharedComp<n0::CompComplex>();
        bool dirty = ccomplex.RemoveChild(node);

        if (dirty && node->HasUniqueComp<bp::CompNode>())
        {
            // update front
            if (curr.node->HasUniqueComp<bp::CompNode>())
            {
                auto bp_parent = curr.node->GetUniqueComp<bp::CompNode>().GetNode();
                if (bp_parent && bp_parent->get_type().is_derived_from<node::Subnetwork>())
                {
                    auto sub_nw = std::static_pointer_cast<node::Subnetwork>(bp_parent);
                    auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
                    for (auto itr = sub_nw->children.begin(); itr != sub_nw->children.end(); )
                    {
                        if (*itr == bp_node) {
                            itr = sub_nw->children.erase(itr);
                        } else {
                            ++itr;
                        }
                    }
                }
            }

            // front eval cb
            auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
            curr.eval->OnRemoveNode(*bp_node);

            // update back
            if (curr.node->HasUniqueComp<bp::CompNode>() && m_path.parts.size() > 1)
            {
                auto bp_parent = curr.node->GetUniqueComp<bp::CompNode>().GetNode();
                if (bp_parent && bp_parent->get_type().is_derived_from<node::Subnetwork>())
                {
                    auto sub_nw = std::static_pointer_cast<node::Subnetwork>(bp_parent);

                    auto& prev_eval = m_path.parts[m_path.parts.size() - 2].eval;
                    auto& curr_node = m_path.parts.back().node;
                    assert(curr_node->HasUniqueComp<bp::CompNode>());
                    auto parent = prev_eval->QueryBackNode(*curr_node->GetUniqueComp<bp::CompNode>().GetNode());
                    auto child = m_path.parts.back().eval->QueryBackNode(*bp_node);
                    assert(parent->get_type().is_derived_from<sop::node::Subnetwork>());
                    RebuildBackFromFront(std::static_pointer_cast<sop::node::Subnetwork>(parent), sub_nw, *curr.eval);
                }
            }
        }

        return dirty;
    }
}

bool SceneTree::Clear()
{
    if (m_path.parts.empty()) {
        return false;
    }

    // update scene node
    auto& curr = m_path.parts.back();
    assert(curr.node->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = curr.node->GetSharedComp<n0::CompComplex>();
    bool dirty = !ccomplex.GetAllChildren().empty();
	ccomplex.RemoveAllChildren();

    // update front
    if (curr.node->HasUniqueComp<bp::CompNode>())
    {
        auto bp_parent = curr.node->GetUniqueComp<bp::CompNode>().GetNode();
        if (bp_parent && bp_parent->get_type().is_derived_from<node::Subnetwork>())
        {
            auto sub_nw = std::static_pointer_cast<node::Subnetwork>(bp_parent);
            sub_nw->children.clear();
        }
    }

    // front eval cb
    assert(curr.eval);
    curr.eval->OnClearAllNodes();

    // update back
    if (m_path.parts.size() > 1)
    {
        auto& prev_eval = m_path.parts[m_path.parts.size() - 2].eval;
        auto& curr_node = m_path.parts.back().node;
        assert(curr_node->HasUniqueComp<bp::CompNode>());
        auto parent = prev_eval->QueryBackNode(*curr_node->GetUniqueComp<bp::CompNode>().GetNode());
        assert(parent->get_type().is_derived_from<sop::node::Subnetwork>());
        std::static_pointer_cast<sop::node::Subnetwork>(parent)->ClearChildren();
    }

    return dirty;
}

std::shared_ptr<Evaluator>
SceneTree::QueryEval(const n0::SceneNodePtr& node) const
{
    auto itr = m_eval_cache.find(node);
    return itr == m_eval_cache.end() ? nullptr : itr->second;
}

bool SceneTree::Push(const n0::SceneNodePtr& node)
{
    if (!node->HasUniqueComp<bp::CompNode>()) {
        return false;
    }

    auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
    auto type = bp_node->get_type();
    if (!type.is_derived_from<node::Subnetwork>()) {
        return false;
    }

    assert(IsCurrChild(node));
    auto itr = m_eval_cache.find(node);
    if (itr == m_eval_cache.end())
    {
        auto eval = std::make_shared<Evaluator>();
        if (m_is_loading) {
            eval->EnableUpdateSopEval(false);
        }
        if (node->HasSharedComp<n0::CompComplex>())
        {
            for (auto& c : node->GetSharedComp<n0::CompComplex>().GetAllChildren()) {
                if (c->HasUniqueComp<bp::CompNode>()) {
                    auto& bp_node = c->GetUniqueComp<bp::CompNode>().GetNode();
                    eval->OnAddNode(*bp_node, false);
                }
            }

            if (node->HasUniqueComp<bp::CompNode>())
            {
                auto bp_parent = node->GetUniqueComp<bp::CompNode>().GetNode();
                if (bp_parent->get_type().is_derived_from<node::Subnetwork>())
                {
                    auto src = std::static_pointer_cast<node::Subnetwork>(bp_parent);
                    auto dst = GetCurrEval()->QueryBackNode(*src);
                    assert(dst && dst->get_type().is_derived_from<sop::node::Subnetwork>());
                    auto dst_sub_nw = std::static_pointer_cast<sop::node::Subnetwork>(dst);
                    RebuildBackFromFront(dst_sub_nw, src, *eval);
                }
            }
        }

        m_eval_cache.insert({ node, eval });
        m_path.parts.push_back({ node, eval });

        eval->OnRebuildConnection();
    }
    else
    {
        m_path.parts.push_back({ node, itr->second });
    }

    SetupCurrNode();

    return true;
}

bool SceneTree::Pop()
{
    if (m_path.parts.empty()) {
        return false;
    }

    m_path.parts.pop_back();

    SetupCurrNode();

    return true;
}

bool SceneTree::SetDepth(size_t depth)
{
    if (depth >= m_path.parts.size()) {
        return false;
    }

    m_path.parts.erase(m_path.parts.begin() + depth + 1, m_path.parts.end());

    SetupCurrNode();

    return true;
}

void SceneTree::ClearNodeDisplayTag()
{
    if (m_is_loading || m_path.parts.empty()) {
        return;
    }

    auto curr = m_path.parts.back();
    for (auto& itr : curr.eval->GetAllNodes())
    {
        auto bp_node = itr.first;
        if (bp_node->get_type().is_derived_from<Node>())
        {
            auto sopv_node = static_cast<const Node*>(bp_node);
            const_cast<Node*>(sopv_node)->SetDisplay(false);
        }
    }
}

n0::SceneNodePtr SceneTree::GetRoot() const
{
#ifdef SOPV_SCENE_TREE_DUMMY_ROOT
    return m_path.parts.size() <= 1 ? nullptr : m_path.parts[m_path.parts.size() - 2].node;
#else
    return m_path.parts.empty() ? nullptr : m_path.parts.front().node;
#endif // SOPV_SCENE_TREE_DUMMY_ROOT
}

void SceneTree::LoadBegin()
{
    m_is_loading = true;

    for (auto& itr : m_eval_cache) {
        itr.second->EnableUpdateSopEval(false);
    }
}

void SceneTree::LoadEnd()
{
    m_is_loading = false;

    for (auto& itr : m_eval_cache) {
        itr.second->EnableUpdateSopEval(true);
    }
}

void SceneTree::InitDummyRoot()
{
    auto node = ns::NodeFactory::Create2D();
    node->AddSharedComp<n0::CompComplex>();

    auto eval = std::make_shared<Evaluator>();
    if (m_is_loading) {
        eval->EnableUpdateSopEval(false);
    }
    m_eval_cache.insert({ node, eval });

    m_path.parts.push_back(PathPart(node, eval));
}

bool SceneTree::IsCurrChild(const n0::SceneNodePtr& node) const
{
    if (m_path.parts.empty()) {
        return false;
    }

    auto curr = m_path.parts.back();
    assert(curr.node->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = curr.node->GetSharedComp<n0::CompComplex>();
    for (auto& child : ccomplex.GetAllChildren()) {
        if (child == node) {
            return true;
        }
    }
    return false;
}

void SceneTree::SetupCurrNode()
{
    if (m_path.parts.empty()) {
        return;
    }

    auto curr = m_path.parts.back();
    assert(curr.node->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = curr.node->GetSharedComp<n0::CompComplex>();
    for (auto& c : ccomplex.GetAllChildren())
    {
        n0::NodeFlagsHelper::SetFlag<n0::NodeNotVisible>(*c, false);
        if (c->HasSharedComp<n0::CompComplex>()) {
            for (auto& cc : c->GetSharedComp<n0::CompComplex>().GetAllChildren()) {
                n0::NodeFlagsHelper::SetFlag<n0::NodeNotVisible>(*cc, true);
            }
        }
    }
}

}