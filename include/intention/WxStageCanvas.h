#pragma once

#include <ee3/WxStageCanvas.h>

namespace itt
{

class SceneTree;

class WxStageCanvas : public ee3::WxStageCanvas
{
public:
    WxStageCanvas(ee0::WxStagePage* stage, ECS_WORLD_PARAM
        const ee0::RenderContext& rc);

    void SetSceneTree(const std::shared_ptr<SceneTree>& stree) {
        m_stree = stree;
    }

protected:
    virtual bool OnUpdate() override;

    virtual void DrawBackground3D() const override;
    virtual void DrawForeground3D() const override;
    virtual void DrawForeground2D() const override;

private:
    std::shared_ptr<SceneTree> m_stree = nullptr;

}; // WxStageCanvas

}