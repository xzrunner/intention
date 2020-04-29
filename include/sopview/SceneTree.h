#pragma once

//#define SOPV_SCENE_TREE_DUMMY_ROOT

#include <node0/typedef.h>

#include <vector>
#include <map>

namespace ur { class Device; }
namespace bp { class Node; }

namespace sopv
{

class Evaluator;

class SceneTree
{
public:
    SceneTree();

    void AfterLoadFromFile();

    bool Add(const ur::Device& dev, const n0::SceneNodePtr& node);
    bool Remove(const n0::SceneNodePtr& node);
    bool Clear();

    auto GetCurrEval() const { return m_path.parts.empty() ? nullptr : m_path.parts.back().eval; }
    auto GetCurrNode() const { return m_path.parts.empty() ? nullptr : m_path.parts.back().node; }
    auto GetLastEval() const { return m_path.parts.size() >= 2 ? m_path.parts[m_path.parts.size() - 2].eval : nullptr; }
    auto GetLastNode() const { return m_path.parts.size() >= 2 ? m_path.parts[m_path.parts.size() - 2].node : nullptr; }

    std::shared_ptr<Evaluator> QueryEval(const n0::SceneNodePtr& node) const;

    bool Push(const ur::Device& dev, const n0::SceneNodePtr& node);
    n0::SceneNodePtr Pop();
    bool SetDepth(size_t depth);

    void ClearNodeDisplayTag();

    n0::SceneNodePtr GetRoot() const;

    void LoadBegin();
    void LoadEnd();

private:
    void InitDummyRoot(const ur::Device& dev);

    bool IsCurrChild(const n0::SceneNodePtr& node) const;

    void SetupCurrNode();

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
        std::vector<PathPart> parts;
    };

private:
    Path m_path;

    std::map<n0::SceneNodePtr, std::shared_ptr<Evaluator>> m_eval_cache;

    bool m_is_loading = false;

}; // SceneTree

}