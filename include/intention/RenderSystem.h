#pragma once

#include <tessellation/Painter.h>
#include <painting0/RenderContext.h>

namespace evt { class Node; }
namespace bp { class Node; }
namespace pt3 { class Viewport; }
namespace pm3 { struct Brush; struct BrushFace; }

namespace itt
{

class RenderSystem
{
public:
    RenderSystem(const pt3::Viewport& vp, const sm::mat4& cam_mat);

    void DrawNode(const pt0::RenderContext& rc,
        const evt::Node& back, const bp::Node& front) const;

    auto& GetPainter() const { return m_pt; }

private:
    void DrawFace(const pm3::Brush& brush, size_t face_idx,
        uint32_t color, const sm::mat4& cam_mat) const;

private:
    const pt3::Viewport& m_vp;
    const sm::mat4&      m_cam_mat;

    mutable tess::Painter m_pt;

}; // RenderSystem

}