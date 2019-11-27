#pragma once

#include "sopview/ReflectPropTypes.h"

#include <ee0/typedef.h>

#include <node0/typedef.h>
#include <sop/GeoAttrClass.h>

#include <wx/notebook.h>

class wxListEvent;

namespace sop { class GeoAttribute; class GroupMgr; }

namespace sopv
{

class SceneTree;
class WxGeoPropList;

class WxGeoProperty : public wxNotebook
{
public:
    WxGeoProperty(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
        const std::shared_ptr<SceneTree>& stree);

    void LoadFromNode(const n0::SceneNodePtr& node);

    std::vector<size_t> GetSelectedIndices(sop::GeoAttrClass cls) const;

private:
    void InitLayout();

    void Clear();

    void SetPreviewCanvasDirty(wxListEvent& event);

private:
    WxGeoPropList* m_lists2[static_cast<int>(sop::GeoAttrClass::MaxTypeNum)];

    ee0::SubjectMgrPtr m_sub_mgr = nullptr;

    std::shared_ptr<SceneTree> m_stree = nullptr;

}; // WxGeoProperty

}