#pragma once

#include <ee3/WxStageCanvas.h>

namespace itt
{

class WxStagePage;

class WxStageCanvas : public ee3::WxStageCanvas
{
public:
    WxStageCanvas(ee0::WxStagePage* stage, ECS_WORLD_PARAM
        const ee0::RenderContext& rc);

protected:
    virtual bool OnUpdate() override;

    virtual void DrawBackground3D() const override;
    virtual void DrawForeground3D() const override;

}; // WxStageCanvas

}