#include "intention/Evaluator.h"
#include "intention/Everything.h"

#include <blueprint/Node.h>
#include <blueprint/Pin.h>
#include <blueprint/Connecting.h>

namespace itt
{

void Evaluator::OnAddNode(const bp::Node& node)
{
    auto back = Everything::CreateBackFromFront(node);
    if (back)
    {
        m_eval.AddNode(back);
        m_nodes_map.insert({ &node, back });

        Everything::UpdatePropBackFromFront(node, *back);

        m_eval.Update();
    }
}

void Evaluator::OnRemoveNode(const bp::Node& node)
{
    auto itr = m_nodes_map.find(&node);
    assert(itr != m_nodes_map.end());

    m_eval.RemoveNode(itr->second);
    m_nodes_map.erase(itr);

    m_eval.Update();
}

void Evaluator::OnClearAllNodes()
{
    m_eval.ClearAllNodes();
    m_nodes_map.clear();

    m_eval.Update();
}

void Evaluator::OnNodePropChanged(const bp::NodePtr& node)
{
    auto itr = m_nodes_map.find(node.get());
    assert(itr != m_nodes_map.end());
    Everything::UpdatePropBackFromFront(*node, *itr->second);

    m_eval.PropChanged(itr->second);

    m_eval.Update();
}

void Evaluator::OnConnected(const bp::Connecting& conn)
{
    auto f_pin = conn.GetFrom();
    auto t_pin = conn.GetTo();

    auto f_itr = m_nodes_map.find(&f_pin->GetParent());
    auto t_itr = m_nodes_map.find(&t_pin->GetParent());
    assert(f_itr != m_nodes_map.end() && t_itr != m_nodes_map.end());

    m_eval.Connect(
        { f_itr->second, f_pin->GetPosIdx() },
        { t_itr->second, t_pin->GetPosIdx() }
    );

    m_eval.Update();
}

void Evaluator::OnDisconnecting(const bp::Connecting& conn)
{
    auto f_pin = conn.GetFrom();
    auto t_pin = conn.GetTo();

    auto f_itr = m_nodes_map.find(&f_pin->GetParent());
    auto t_itr = m_nodes_map.find(&t_pin->GetParent());
    assert(f_itr != m_nodes_map.end() && t_itr != m_nodes_map.end());

    m_eval.Disconnect(
        { f_itr->second, f_pin->GetPosIdx() },
        { t_itr->second, t_pin->GetPosIdx() }
    );

    m_eval.Update();
}

void Evaluator::OnRebuildConnection()
{
    std::vector<std::pair<evt::Node::PortAddr, evt::Node::PortAddr>> conns;
    for (auto& itr : m_nodes_map)
    {
        auto& front = itr.first;
        auto& back  = itr.second;
        for (auto& in : front->GetAllInput())
        {
            for (auto& conn : in->GetConnecting())
            {
                auto f_pin = conn->GetFrom();
                auto t_pin = conn->GetTo();

                auto f_itr = m_nodes_map.find(&f_pin->GetParent());
                auto t_itr = m_nodes_map.find(&t_pin->GetParent());
                assert(f_itr != m_nodes_map.end() && t_itr != m_nodes_map.end());

                conns.push_back({
                    { f_itr->second, f_pin->GetPosIdx() },
                    { t_itr->second, t_pin->GetPosIdx() }
                });
            }
        }
    }

    m_eval.RebuildConnections(conns);

    m_eval.Update();
}

evt::NodePtr Evaluator::QueryBackNode(const bp::Node& front_node) const
{
    auto itr = m_nodes_map.find(&front_node);
    return itr == m_nodes_map.end() ? nullptr : itr->second;
}

}