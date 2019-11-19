#pragma once

#include <memory>
#include <map>

namespace sopv
{

class SceneTree;

namespace py
{

class NodeProxy;

struct PyLoaderCtx
{
    std::shared_ptr<SceneTree> stree = nullptr;

    std::map<std::string, std::shared_ptr<NodeProxy>> network_nodes;

}; // PyLoaderCtx

}
}