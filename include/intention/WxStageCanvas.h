#pragma once

#include <ee3/WxStageCanvas.h>

namespace itt
{

class Evaluator;

class WxStageCanvas : public ee3::WxStageCanvas
{
public:
    WxStageCanvas(ee0::WxStagePage* stage, ECS_WORLD_PARAM
        const ee0::RenderContext& rc);

    void SetEval(const std::shared_ptr<Evaluator>& eval);

protected:
    virtual bool OnUpdate() override;

    virtual void DrawBackground3D() const override;
    virtual void DrawForeground3D() const override;

private:
    std::shared_ptr<Evaluator> m_eval = nullptr;

}; // WxStageCanvas

}