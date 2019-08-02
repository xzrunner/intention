#pragma once

#include <blueprint/typedef.h>

#include <boost/noncopyable.hpp>

#include <vector>
#include <unordered_map>

#include <everything/typedef.h>

namespace bp { class Connecting; }

namespace itt
{

class Evaluator : boost::noncopyable
{
public:
    // update node
    void OnAddNode(const bp::Node& node);
    void OnRemoveNode(const bp::Node& node);
    void OnClearAllNodes();

    void OnNodePropChanged(const bp::NodePtr& node);

    // update connection
    void OnConnected(const bp::Connecting& conn);
    void OnDisconnecting(const bp::Connecting& conn);
    void OnRebuildConnection();

    // update context
    void UpdateContext();

    auto& GetAllNodes() const { return m_nodes_map; }

    evt::NodePtr QueryBackNode(const bp::Node& front_node) const;

private:
    std::unordered_map<const bp::Node*, evt::NodePtr> m_nodes_map;

}; // Evaluator

}