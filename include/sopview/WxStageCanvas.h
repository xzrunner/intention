#pragma once

#include <ee0/typedef.h>
#include <ee3/WxStageCanvas.h>

#include <sop/typedef.h>

namespace tess { class Painter; }

namespace sopv
{

class SceneTree;
class WxGeoProperty;

class WxStageCanvas : public ee3::WxStageCanvas
{
public:
    WxStageCanvas(const ur::Device& dev, ee0::WxStagePage* stage, ECS_WORLD_PARAM
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

    virtual void OnKeyDownImpl(wxKeyEvent& event) override;

private:
    sop::NodePtr GetSelectedNode() const;
    sop::NodePtr GetDisplayNode() const;

private:
    enum class ViewportType
    {
        Perspective,
        Top,
        Front,
        Right,
        UV,
    };

    class Viewports
    {
    public:
        Viewports(WxStageCanvas& canvas);

        bool ChangeVP(ViewportType type);

    private:
        WxStageCanvas& m_canvas;

        ViewportType m_curr_vp = ViewportType::Perspective;

        // 3d presp
        pt0::CameraPtr m_cam_3d = nullptr;

        // 2d ortho
        pt0::CameraPtr m_cam_xz = nullptr;
        pt0::CameraPtr m_cam_xy = nullptr;
        pt0::CameraPtr m_cam_zy = nullptr;

        // uv
        pt0::CameraPtr m_cam_uv = nullptr;

        friend class WxStageCanvas;

    }; // Viewports

private:
    std::shared_ptr<SceneTree> m_stree = nullptr;

    WxGeoProperty* m_prop_view = nullptr;

    ee0::WxStagePage* m_graph_stage = nullptr;

    Viewports m_viewports;

    bool m_uv_mode = false;

    ee0::EditOPPtr m_default_op = nullptr;
    ee0::EditOPPtr m_uv_op      = nullptr;

    friend class Viewports;

}; // WxStageCanvas

}