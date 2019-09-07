#pragma once

#include <node0/typedef.h>

#include <vector>
#include <map>

namespace bp { class Node; }

namespace itt
{

class Evaluator;

class SceneTree
{
public:
    void Init();

    bool Add(const n0::SceneNodePtr& node);
    bool Remove(const n0::SceneNodePtr& node);
    bool Clear();

    auto GetCurrEval() const { return m_path.patrs.empty() ? nullptr : m_path.patrs.back().eval; }
    auto GetCurrNode() const { return m_path.patrs.empty() ? nullptr : m_path.patrs.back().node; }

    bool ToNextLevel(const n0::SceneNodePtr& node);
    bool SetDepth(size_t depth);

    void ClearNodeDisplayTag();

    n0::SceneNodePtr GetRoot() const;

private:
    bool IsCurrChild(const n0::SceneNodePtr& node) const;

    void SetupCurrNode();

    void AddToParent(const n0::SceneNodePtr& node);
    void RemoveFromParent(const n0::SceneNodePtr& node);
    void ClearParent();

private:
    struct PathPart
    {
        PathPart(const n0::SceneNodePtr& node, const std::shared_ptr<Evaluator>& eval)
            : node(node), eval(eval) {}

        n0::SceneNodePtr node           = nullptr;
        std::shared_ptr<Evaluator> eval = nullptr;
    };

    struct Path
    {
        std::vector<PathPart> patrs;
    };

private:
    Path m_path;

    std::map<n0::SceneNodePtr, std::shared_ptr<Evaluator>> m_eval_cache;

}; // SceneTree

}