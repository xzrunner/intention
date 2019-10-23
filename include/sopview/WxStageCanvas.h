#pragma once

#include <ee3/WxStageCanvas.h>

namespace tess { class Painter; }

namespace sopv
{

class SceneTree;
class WxGeoProperty;

class WxStageCanvas : public ee3::WxStageCanvas
{
public:
    WxStageCanvas(ee0::WxStagePage* stage, ECS_WORLD_PARAM
        const ee0::RenderContext& rc);

    void SetSceneTree(const std::shared_ptr<SceneTree>& stree) {
        m_stree = stree;
    }

    void SetPropView(WxGeoProperty* view) { m_prop_view = view; }

    void SetGraphStage(ee0::WxStagePage* stage) { m_graph_stage = stage; }

protected:
    virtual bool OnUpdate() override;

    virtual void DrawBackground3D() const override;
    virtual void DrawForeground3D() const override;
    virtual void DrawForeground2D() const override;

private:
    void DrawAttrSelected(tess::Painter& pt, const sm::mat4& cam_mat) const;

private:
    std::shared_ptr<SceneTree> m_stree = nullptr;

    WxGeoProperty* m_prop_view = nullptr;

    ee0::WxStagePage* m_graph_stage = nullptr;

}; // WxStageCanvas

}