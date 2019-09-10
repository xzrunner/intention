#pragma once

#include <tessellation/Painter.h>
#include <painting0/RenderContext.h>

namespace evt { class Node; struct Group; class GeometryImpl; }
namespace bp { class Node; }
namespace pm3 { class Polytope; }
namespace pt3 { class Viewport; }

namespace itt
{

class RenderSystem
{
public:
    RenderSystem(const pt3::Viewport& vp, const sm::mat4& cam_mat);

    void DrawNode3D(const pt0::RenderContext& rc,
        const evt::Node& back, const bp::Node& front) const;
    void DrawNode2D(const evt::Node& back, const bp::Node& front) const;

    auto& GetPainter() const { return m_pt; }

private:
    void DrawGroup(const evt::Group& group, const evt::GeometryImpl& geo) const;

    void DrawFace(const pm3::Polytope& poly, size_t face_idx,
        uint32_t color, const sm::mat4& cam_mat) const;

private:
    const pt3::Viewport& m_vp;
    const sm::mat4&      m_cam_mat;

    mutable tess::Painter m_pt;

}; // RenderSystem

}