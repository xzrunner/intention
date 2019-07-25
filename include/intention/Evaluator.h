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

    bool Execute(const std::vector<bp::NodePtr>& nodes);

    evt::NodePtr QueryEvtNode(const bp::Node* bp_node) const;
    void AddNodeMap(const bp::Node* bp_node, const evt::NodePtr& rg_node);

private:
    void Clear();

private:
    std::unordered_map<const bp::Node*, evt::NodePtr> m_nodes_map;

}; // Evaluator

}