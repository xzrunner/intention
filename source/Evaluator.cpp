#include "intention/Evaluator.h"
#include "intention/Everything.h"

#include <blueprint/Evaluator.h>
#include <blueprint/Node.h>
#include <blueprint/Pin.h>
#include <blueprint/Connecting.h>

#include <everything/Node.h>
#include <everything/TreeContext.h>

#include <queue>

namespace itt
{

void Evaluator::OnAddNode(const bp::Node& node)
{
    auto back = Everything::CreateBackFromFront(node);
    if (back)
    {
        back->Execute(false);

        Everything::UpdatePropBackFromFront(node, *back);
        m_nodes_map.insert({ &node, back });
    }
}

void Evaluator::OnRemoveNode(const bp::Node& node)
{
    auto itr = m_nodes_map.find(&node);
    assert(itr != m_nodes_map.end());

    auto back = itr->second;

    std::vector<evt::NodePtr> children;

    // update conns
    for (auto& out : back->GetExports())
    {
        for (auto& conn : out.conns)
        {
            auto n = conn.node.lock();
            if (!n) {
                continue;
            }

            auto& in_port = n->GetImports()[conn.idx];
            assert(in_port.conns.size() == 1);
            const_cast<evt::Node::Port&>(in_port).conns.clear();

            children.push_back(n);
        }
    }

    m_nodes_map.erase(itr);

    // update context
    UpdateContext();

    // execute
    for (auto& c : children) {
        c->Execute(false);
    }
}

void Evaluator::OnClearAllNodes()
{
    m_nodes_map.clear();
}

void Evaluator::OnNodePropChanged(const bp::NodePtr& node)
{
    // update prop
    auto itr = m_nodes_map.find(node.get());
    assert(itr != m_nodes_map.end());
    Everything::UpdatePropBackFromFront(*node, *itr->second);

    // update context
    UpdateContext();

    // execute
    std::queue<const bp::Node*> buf;
    buf.push(node.get());
    while (!buf.empty())
    {
        auto n = buf.front(); buf.pop();
        auto itr = m_nodes_map.find(n);
        assert(itr != m_nodes_map.end());
        itr->second->Execute(false);

        for (auto& pin : n->GetAllOutput()) {
            for (auto& conn : pin->GetConnecting()) {
                buf.push(&conn->GetTo()->GetParent());
            }
        }
    }
}

void Evaluator::OnConnected(const bp::Connecting& conn)
{
    auto f_pin = conn.GetFrom();
    auto t_pin = conn.GetTo();

    auto f_itr = m_nodes_map.find(&f_pin->GetParent());
    auto t_itr = m_nodes_map.find(&t_pin->GetParent());
    assert(f_itr != m_nodes_map.end() && t_itr != m_nodes_map.end());

    evt::make_connecting(
        { f_itr->second, f_pin->GetPosIdx() },
        { t_itr->second, t_pin->GetPosIdx() }
    );

    UpdateContext();

    t_itr->second->Execute(false);
}

void Evaluator::OnDisconnecting(const bp::Connecting& conn)
{
    auto f_pin = conn.GetFrom();
    auto t_pin = conn.GetTo();

    auto f_itr = m_nodes_map.find(&f_pin->GetParent());
    auto t_itr = m_nodes_map.find(&t_pin->GetParent());
    assert(f_itr != m_nodes_map.end() && t_itr != m_nodes_map.end());

    // update connections
    evt::disconnect(
        { f_itr->second, f_pin->GetPosIdx() },
        { t_itr->second, t_pin->GetPosIdx() }
    );

    // update context
    UpdateContext();

    // execute
    t_itr->second->Execute(false);
}

void Evaluator::OnRebuildConnection()
{
    // remove conns
    for (auto& itr : m_nodes_map)
    {
        for (auto& in : itr.second->GetImports()) {
            const_cast<evt::Node::Port&>(in).conns.clear();
        }
        for (auto& out : itr.second->GetExports()) {
            const_cast<evt::Node::Port&>(out).conns.clear();
        }
    }

    // add conns
    for (auto& itr : m_nodes_map)
    {
        auto& front = itr.first;
        auto& back  = itr.second;
        for (auto& in : front->GetAllInput()) {
            for (auto& conn : in->GetConnecting()) {
                OnConnected(*conn);
            }
        }
    }

    // update context
    UpdateContext();

    // execute
    std::vector<const bp::Node*> bp_nodes;
    bp_nodes.reserve(m_nodes_map.size());
    for (auto& itr : m_nodes_map) {
        bp_nodes.push_back(itr.first);
    }

    bp::Evaluator::TopologicalSorting(bp_nodes);

    for (auto& front : bp_nodes)
    {
        auto itr = m_nodes_map.find(front);
        assert(itr != m_nodes_map.end());
        itr->second->Execute(true);
    }
}

void Evaluator::UpdateContext()
{
    for (auto& itr : m_nodes_map) {
        itr.second->BeforeUpdateContext();
    }

    for (auto& itr : m_nodes_map)
    {
        bool is_root = true;
        auto inputs = itr.second->GetImports();
        for (auto& i : inputs) {
            if (!i.conns.empty()) {
                is_root = false;
            }
        }
        if (is_root) {
            itr.second->UpdateContext(evt::TreeContext());
        }
    }
}

evt::NodePtr Evaluator::QueryBackNode(const bp::Node& front_node) const
{
    auto itr = m_nodes_map.find(&front_node);
    return itr == m_nodes_map.end() ? nullptr : itr->second;
}

}