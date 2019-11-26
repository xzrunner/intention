#pragma once

#include <blueprint/typedef.h>

#include <sop/Evaluator.h>
#include <sop/typedef.h>

#include <boost/noncopyable.hpp>

#include <vector>
#include <unordered_map>

namespace bp { class Connecting; }

namespace sopv
{

class Evaluator : boost::noncopyable
{
public:
    // update nodes
    void OnAddNode(const bp::Node& node, bool need_update = true);
    void OnRemoveNode(const bp::Node& node);
    void OnClearAllNodes();

    // update node parm
    void OnNodePropChanged(const bp::NodePtr& node);

    // update node conn
    void OnConnected(const bp::Connecting& conn);
    void OnDisconnecting(const bp::Connecting& conn);
    void OnRebuildConnection();

    // calc
    float CalcFloat(const std::string& expr, const sop::Node& node, float expect = 0.0f) const;
    int CalcInt(const std::string& expr, const sop::Node& node, int expect = 0) const;

    auto& GetAllNodes() const { return m_nodes_map; }

    sop::NodePtr QueryBackNode(const bp::Node& front_node) const;

    void EnableUpdateSopEval(bool up);

private:
    void Update();

    void UpdateGroupName();

private:
    sop::Evaluator m_eval;

    bool m_update_sop_eval = true;

    std::unordered_map<const bp::Node*, sop::NodePtr> m_nodes_map;

}; // Evaluator

}