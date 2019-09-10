#pragma once

#include <blueprint/typedef.h>

#include <everything/Evaluator.h>

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
    // update nodes
    void OnAddNode(const bp::Node& node);
    void OnRemoveNode(const bp::Node& node);
    void OnClearAllNodes();

    // update node prop
    void OnNodePropChanged(const bp::NodePtr& node);

    // update node conn
    void OnConnected(const bp::Connecting& conn);
    void OnDisconnecting(const bp::Connecting& conn);
    void OnRebuildConnection();

    // calc
    float CalcFloat(const std::string& expr, const evt::Node& node, float expect = 0.0f) const;
    int CalcInt(const std::string& expr, const evt::Node& node, int expect = 0) const;

    auto& GetAllNodes() const { return m_nodes_map; }

    evt::NodePtr QueryBackNode(const bp::Node& front_node) const;

private:
    void Update();

    void UpdateGroupName();

private:
    evt::Evaluator m_eval;

    std::unordered_map<const bp::Node*, evt::NodePtr> m_nodes_map;

}; // Evaluator

}