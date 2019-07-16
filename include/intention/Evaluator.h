#pragma once

#include <blueprint/typedef.h>

#include <boost/noncopyable.hpp>

#include <vector>
#include <unordered_map>

#include <everything/typedef.h>

namespace itt
{

class Evaluator : boost::noncopyable
{
public:
    Evaluator();
    ~Evaluator();

    void Rebuild(const std::vector<bp::NodePtr>& nodes);

    evt::NodePtr QueryEvtNode(const bp::Node* bp_node) const;
    void AddNodeMap(const bp::Node* bp_node, const evt::NodePtr& rg_node);

    auto& GetBackNodes() const { return m_back_nodes; }

private:
    void Clear();

private:
    std::vector<evt::NodePtr> m_back_nodes;

    std::unordered_map<const bp::Node*, evt::NodePtr> m_nodes_map;

}; // Evaluator

}