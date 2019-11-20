#pragma once

#include "sopview/ReflectPropTypes.h"

#include <node0/typedef.h>
#include <sop/GeoAttrClass.h>

#include <wx/notebook.h>

class wxListEvent;

namespace sop { class GeoAttribute; class GroupMgr; }
namespace ee0 { class WxStageCanvas; }

namespace sopv
{

class SceneTree;
class WxGeoPropList;

class WxGeoProperty : public wxNotebook
{
public:
    WxGeoProperty(wxWindow* parent, const std::shared_ptr<SceneTree>& stree);

    void LoadFromNode(const n0::SceneNodePtr& node);

    std::vector<size_t> GetSelectedIndices(sop::GeoAttrClass cls) const;

    void SetPreviewCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas) {
        m_preview_canvas = canvas;
    }

private:
    void InitLayout();

    void Clear();

    void SetPreviewCanvasDirty(wxListEvent& event);

private:
    WxGeoPropList* m_lists2[static_cast<int>(sop::GeoAttrClass::MaxTypeNum)];

    std::shared_ptr<SceneTree> m_stree = nullptr;

    // for refresh
    std::shared_ptr<ee0::WxStageCanvas> m_preview_canvas = nullptr;

}; // WxGeoProperty

}