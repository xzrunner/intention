#pragma once

#include "sopview/ReflectPropTypes.h"

#include <node0/typedef.h>

#include <wx/notebook.h>

class wxListCtrl;
class wxListEvent;

namespace sop { class GeoAttribute; class GroupMgr; }
namespace ee0 { class WxStageCanvas; }

namespace sopv
{

class SceneTree;

class WxGeoProperty : public wxNotebook
{
public:
    WxGeoProperty(wxWindow* parent, const std::shared_ptr<SceneTree>& stree);

    void LoadFromNode(const n0::SceneNodePtr& node);

    std::vector<size_t> GetSelectedIndices(GeoAttrClass cls) const;

    void SetPreviewCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas) {
        m_preview_canvas = canvas;
    }

private:
    void InitLayout();

    void Clear();

    void LoadDefault(const sop::GeoAttribute& attr);
    void LoadGroups(const sop::GroupMgr& groups);

    void SetPreviewCanvasDirty(wxListEvent& event);

private:
    enum ListIndex
    {
        POINT = 0,
        VERTEX,
        PRIMITIVE,
        DETAIL,

        MAX_LIST_COUNT,
    };
    wxListCtrl* m_lists[MAX_LIST_COUNT];

    std::shared_ptr<SceneTree> m_stree = nullptr;

    // for refresh
    std::shared_ptr<ee0::WxStageCanvas> m_preview_canvas = nullptr;

}; // WxGeoProperty

}