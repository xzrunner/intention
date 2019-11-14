#pragma once

#include <string>

namespace sopv
{

class SceneTree;

class PyLoader
{
public:
    PyLoader(std::shared_ptr<SceneTree> stree);

    void RunFile(const std::string& filepath);

private:
    void PrepareContext();

private:
    std::shared_ptr<SceneTree> m_stree = nullptr;

}; // PyLoader

}