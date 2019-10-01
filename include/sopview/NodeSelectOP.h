#pragma once

#include <blueprint/NodeSelectOP.h>

#include <functional>

namespace pt0 { class Camera; }

namespace sopv
{

class SceneTree;

class NodeSelectOP : public bp::NodeSelectOP
{
public:
    NodeSelectOP(const std::shared_ptr<pt0::Camera>& camera,
        ee0::WxStagePage& stage);

    virtual bool OnMouseLeftDClick(int x, int y) override;

    void SetSceneTree(const std::shared_ptr<SceneTree>& stree) {
        m_stree = stree;
    }

private:
    std::shared_ptr<SceneTree> m_stree = nullptr;

}; // NodeSelectOP

}