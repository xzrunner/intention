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

    static std::string FixCode(const std::string& code);

private:
    std::shared_ptr<SceneTree> m_stree = nullptr;

}; // PyLoader

}