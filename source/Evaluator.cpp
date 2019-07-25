#include "intention/Evaluator.h"
#include "intention/Everything.h"

#include <blueprint/Evaluator.h>
#include <blueprint/Node.h>

namespace itt
{

Evaluator::Evaluator()
{
}

Evaluator::~Evaluator()
{
}

bool Evaluator::Execute(const std::vector<bp::NodePtr>& nodes)
{
    bool dirty = false;

    auto sorted = nodes;
    bp::Evaluator::TopologicalSorting(sorted);

    for (auto& node : sorted)
    {
        if (node->IsEditNotDirty()) {
            continue;
        }

        dirty = true;

        auto back_node = Everything::CreateGraphNode(*this, node.get());
        back_node->Execute();
        node->SetEditNotDirty(true);
    }

    return dirty;
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
    m_nodes_map.clear();
}

}