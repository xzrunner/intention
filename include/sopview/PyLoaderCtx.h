#pragma once

#include <memory>
#include <map>

namespace ur { class Device; }

namespace sopv
{

class SceneTree;

namespace py
{

class NodeProxy;

struct PyLoaderCtx
{
    std::shared_ptr<ur::Device> dev = nullptr;

    std::shared_ptr<SceneTree> stree = nullptr;

    std::map<std::string, std::shared_ptr<NodeProxy>> network_nodes;

}; // PyLoaderCtx

}
}