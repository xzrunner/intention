#include "intention/Evaluator.h"
#include "intention/Everything.h"

namespace itt
{

Evaluator::Evaluator()
{
}

Evaluator::~Evaluator()
{
}

void Evaluator::Rebuild(const std::vector<bp::NodePtr>& nodes)
{
    Clear();

    m_back_nodes.reserve(nodes.size());
    for (auto& node : nodes)
    {
        auto back_node = Everything::CreateGraphNode(*this, node.get());
        back_node->Execute();
        m_back_nodes.push_back(back_node);
    }
}

evt::NodePtr Evaluator::QueryEvtNode(const bp::Node* bp_node) const
{
    auto itr = m_nodes_map.find(bp_node);
    return itr == m_nodes_map.end() ? nullptr : itr->second;
}

void Evaluator::AddNodeMap(const bp::Node* bp_node, const evt::NodePtr& rg_node)
{
    m_nodes_map.insert({ bp_node, rg_node });
}

void Evaluator::Clear()
{
    m_back_nodes.clear();
    m_nodes_map.clear();
}

}