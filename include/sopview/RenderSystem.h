#pragma once

#include "sopview/HeightfieldRenderer.h"

#include <tessellation/Painter.h>
#include <painting0/RenderContext.h>

namespace sop { class Node; class Group; class GeometryImpl; }
namespace bp { class Node; }
namespace pm3 { class Polytope; }
namespace pt3 { class Viewport; }

namespace sopv
{

class WxGeoProperty;
class SceneTree;
class Node;

class RenderSystem
{
public:
    RenderSystem(const ur2::Device& dev,
        const pt3::Viewport& vp, const sm::mat4& cam_mat);

    void DrawNode3D(const ur2::Device& dev, ur2::Context& ctx,
        const pt0::RenderContext& rc, const std::shared_ptr<SceneTree>& stre) const;
    void DrawNode2D(const sop::Node& back, const bp::Node& front) const;

    void DrawNodeAttr(const sop::Node& node, const WxGeoProperty& prop_view);
    void DrawNodeUV(const sop::Node& node);

    auto& GetPainter() const { return m_pt; }

    auto& GetHfRenderer() const { return m_hf_renderer; }

public:
    static const float UV_SCALE;

private:
    void DrawNode3D(const ur2::Device& dev, ur2::Context& ctx,
        const pt0::RenderContext& rc, const sop::Node& back, const Node& front) const;

    void DrawGroup(const sop::Group& group, const sop::GeometryImpl& geo) const;

    void DrawFace(const pm3::Polytope& poly, size_t face_idx,
        uint32_t color, const sm::mat4& cam_mat) const;

    void DrawGeometry(const ur2::Device& dev, ur2::Context& ctx,
        const pt0::RenderContext& rc, const sop::GeometryImpl& geo, const Node& front) const;
    void DrawVolume(const ur2::Device& dev, ur2::Context& ctx,
        const std::shared_ptr<sop::Volume>& vol) const;

private:
    const pt3::Viewport& m_vp;
    const sm::mat4&      m_cam_mat;

    mutable tess::Painter m_pt;

    mutable HeightfieldRenderer m_hf_renderer;

}; // RenderSystem

}