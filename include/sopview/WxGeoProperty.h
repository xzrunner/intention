#pragma once

#include <node0/typedef.h>

#include <wx/notebook.h>

class wxListCtrl;

namespace sop { class GeoAttribute; class GroupMgr; }

namespace sopv
{

class SceneTree;

class WxGeoProperty : public wxNotebook
{
public:
    WxGeoProperty(wxWindow* parent, const std::shared_ptr<SceneTree>& stree);

    void LoadFromNode(const n0::SceneNodePtr& node);

private:
    void InitLayout();

    void Clear();

    void LoadDefault(const sop::GeoAttribute& attr);
    void LoadGroups(const sop::GroupMgr& groups);

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

}; // WxGeoProperty

}