#include "intention/SceneTree.h"
#include "intention/Evaluator.h"
#include "intention/Node.h"
#include "intention/RegistNodes.h"

#include <blueprint/CompNode.h>
#include <blueprint/Node.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node0/NodeFlagsHelper.h>
#include <node0/NodeFlags.h>
#include <everything/node/Geometry.h>

#include <assert.h>

namespace itt
{

void SceneTree::Init()
{
    SetupCurrNode();
}

bool SceneTree::Add(const n0::SceneNodePtr& node)
{
    ClearNodeDisplayTag();

    if (m_path.patrs.empty())
    {
        auto eval = std::make_shared<Evaluator>();
        m_eval_cache.insert({ node, eval });

        m_path.patrs.push_back(PathPart(node, eval));
    }
    else
    {
        auto& curr = m_path.patrs.back();
        assert(curr.node->HasSharedComp<n0::CompComplex>());
        auto& ccomplex = curr.node->GetSharedComp<n0::CompComplex>();
        ccomplex.AddChild(node);

        if (node->HasUniqueComp<bp::CompNode>()) {
            auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
            curr.eval->OnAddNode(*bp_node);
        }

        AddToParent(node);
    }

    if (node->HasUniqueComp<bp::CompNode>())
    {
        auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
        auto type = bp_node->get_type();
        if (type == rttr::type::get<node::Geometry>()) {
            if (!node->HasSharedComp<n0::CompComplex>()) {
                node->AddSharedComp<n0::CompComplex>();
            }
        }
        if (type.is_derived_from<Node>()) {
            auto itt_node = std::static_pointer_cast<Node>(bp_node);
            itt_node->SetDisplay(true);
        }
    }

    return true;
}

bool SceneTree::Remove(const n0::SceneNodePtr& node)
{
    if (m_path.patrs.empty()) {
        return false;
    }

    if (node == m_path.patrs.front().node)
    {
        return Clear();
    }
    else
    {
        auto& curr = m_path.patrs.back();
        assert(curr.node->HasSharedComp<n0::CompComplex>());
        auto& ccomplex = curr.node->GetSharedComp<n0::CompComplex>();
        bool dirty = ccomplex.RemoveChild(node);

        if (dirty && node->HasUniqueComp<bp::CompNode>()) {
            auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
            curr.eval->OnRemoveNode(*bp_node);
        }

        RemoveFromParent(node);

        return dirty;
    }
}

bool SceneTree::Clear()
{
    if (m_path.patrs.empty()) {
        return false;
    }

    auto& curr = m_path.patrs.back();
    assert(curr.node->HasSharedComp<n0::CompComplex>());
    auto& ccomplex = curr.node->GetSharedComp<n0::CompComplex>();
    bool dirty = !ccomplex.GetAllChildren().empty();
	ccomplex.RemoveAllChildren();

    assert(curr.eval);
    curr.eval->OnClearAllNodes();

    ClearParent();

    return dirty;
}

bool SceneTree::ToNextLevel(const n0::SceneNodePtr& node)
{
    if (!node->HasUniqueComp<bp::CompNode>()) {
        return false;
    }

    auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
    auto type = bp_node->get_type();
    if (type != rttr::type::get<node::Geometry>()) {
        return false;
    }

    assert(IsCurrChild(node));
    auto itr = m_eval_cache.find(node);
    if (itr == m_eval_cache.end())
    {
        auto eval = std::make_shared<Evaluator>();
        if (node->HasSharedComp<n0::CompComplex>())
        {
            auto& children = node->GetSharedComp<n0::CompComplex>().GetAllChildren();

            for (auto& c : children) {
                if (c->HasUniqueComp<bp::CompNode>()) {
                    auto& bp_node = c->GetUniqueComp<bp::CompNode>().GetNode();
                    eval->OnAddNode(*bp_node);
                }
            }

            if (node->HasUniqueComp<bp::CompNode>())
            {
                auto bp_parent = node->GetUniqueComp<bp::CompNode>().GetNode();
                if (bp_parent->get_type() == rttr::type::get<node::Geometry>())
                {
                    auto src = std::static_pointer_cast<node::Geometry>(bp_parent);
                    auto dst = GetCurrEval()->QueryBackNode(*src);
                    assert(dst && dst->get_type() == rttr::type::get<evt::node::Geometry>());
                    auto dst_geo = std::static_pointer_cast<evt::node::Geometry>(dst);
                    dst_geo->ClearChildren();
                    for (auto& c : src->children)
                    {
                        auto dst_c = eval->QueryBackNode(*c);
                        assert(dst_c);
                        evt::node::Geometry::AddChild(dst_geo, dst_c);
                    }
                }
            }
        }

        m_eval_cache.insert({ node, eval });
        m_path.patrs.push_back({ node, eval });
    }
    else
    {
        m_path.patrs.push_back({ node, itr->second });
    }

    SetupCurrNode();

    return true;
}

bool SceneTree::SetDepth(size_t depth)
{
    if (depth >= m_path.patrs.size()) {
        return false;
    }

    m_path.patrs.erase(m_path.patrs.begin() + depth + 1, m_path.patrs.end());

    SetupCurrNode();

    return true;
}

void SceneTree::ClearNodeDisplayTag()
{
    if (m_path.patrs.empty()) {
        return;
    }

    auto curr = m_path.patrs.back();
    for (auto& itr : curr.eval->GetAllNodes())
    {
        auto bp_node = itr.first;
        if (bp_node->get_type().is_derived_from<Node>())
        {
            auto itt_node = static_cast<const Node*>(bp_node);
            const_cast<Node*>(itt_node)->SetDisplay(false);
        }
    }
}

n0::SceneNodePtr SceneTree::GetRoot() const
{
    return m_path.patrs.empty() ? nullptr : m_path.patrs.front().node;
}

bool SceneTree::IsCurrChild(const n0::SceneNodePtr& node) const
{
    if (m_path.patrs.empty()) {
        return false;
    }

    auto curr = m_path.patrs.back();
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
    if (m_path.patrs.empty()) {
        return;
    }

    auto curr = m_path.patrs.back();
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

void SceneTree::AddToParent(const n0::SceneNodePtr& node)
{
    if (!node->HasUniqueComp<bp::CompNode>()) {
        return;
    }

    auto& curr = m_path.patrs.back();
    if (!curr.node->HasUniqueComp<bp::CompNode>()) {
        return;
    }

    auto bp_parent = curr.node->GetUniqueComp<bp::CompNode>().GetNode();
    if (!bp_parent || bp_parent->get_type() != rttr::type::get<node::Geometry>()) {
        return;
    }

    auto geo = std::static_pointer_cast<node::Geometry>(bp_parent);
    auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
    geo->children.push_back(bp_node);
}

void SceneTree::RemoveFromParent(const n0::SceneNodePtr& node)
{
    if (!node->HasUniqueComp<bp::CompNode>()) {
        return;
    }

    auto& curr = m_path.patrs.back();
    if (!curr.node->HasUniqueComp<bp::CompNode>()) {
        return;
    }

    auto bp_parent = curr.node->GetUniqueComp<bp::CompNode>().GetNode();
    if (!bp_parent || bp_parent->get_type() != rttr::type::get<node::Geometry>()) {
        return;
    }

    auto geo = std::static_pointer_cast<node::Geometry>(bp_parent);
    auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
    for (auto itr = geo->children.begin(); itr != geo->children.end(); )
    {
        if (*itr == bp_node) {
            itr = geo->children.erase(itr);
        } else {
            ++itr;
        }
    }
}

void SceneTree::ClearParent()
{
    auto& curr = m_path.patrs.back();
    if (!curr.node->HasUniqueComp<bp::CompNode>()) {
        return;
    }

    auto bp_parent = curr.node->GetUniqueComp<bp::CompNode>().GetNode();
    if (!bp_parent || bp_parent->get_type() != rttr::type::get<node::Geometry>()) {
        return;
    }

    auto geo = std::static_pointer_cast<node::Geometry>(bp_parent);
    geo->children.clear();
}

}