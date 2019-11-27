#include "sopview/WxGeoProperty.h"
#include "sopview/Node.h"
#include "sopview/SceneTree.h"
#include "sopview/Evaluator.h"
#include "sopview/WxGeoPropList.h"
#include "sopview/MessageID.h"

#include <ee0/WxStageCanvas.h>
#include <ee0/SubjectMgr.h>
#include <blueprint/CompNode.h>

#include <node0/SceneNode.h>

namespace sopv
{

WxGeoProperty::WxGeoProperty(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
                             const std::shared_ptr<SceneTree>& stree)
    : wxNotebook(parent, wxID_ANY, wxDefaultPosition, wxSize(500, -1))
    , m_sub_mgr(sub_mgr)
    , m_stree(stree)
{
    InitLayout();
}

void WxGeoProperty::LoadFromNode(const n0::SceneNodePtr& node)
{
    Clear();

    if (!node->HasUniqueComp<bp::CompNode>()) {
        return;
    }

    auto& cnode = node->GetUniqueComp<bp::CompNode>();
    auto bp_node = cnode.GetNode();
    if (!bp_node) {
        return;
    }

    if (!bp_node->get_type().is_derived_from<Node>()) {
        return;
    }
    auto sopv_node = std::static_pointer_cast<Node>(bp_node);
    auto sop_node = m_stree->GetCurrEval()->QueryBackNode(*sopv_node);
    if (!sop_node) {
        return;
    }

    auto geo = sop_node->GetGeometry();
    if (!geo) {
        return;
    }

    for (auto& list : m_lists2) {
        list->SetGeoData(geo);
    }

    //LoadDefault(geo->GetAttr());
    //LoadGroups(geo->GetGroup());
}

std::vector<size_t>
WxGeoProperty::GetSelectedIndices(sop::GeoAttrClass cls) const
{
    return m_lists2[static_cast<int>(cls)]->GetSelectedIndices();
}

void WxGeoProperty::InitLayout()
{
    for (int i = 0; i < static_cast<int>(sop::GeoAttrClass::MaxTypeNum); ++i)
    {
        auto list = new WxGeoPropList(this, static_cast<sop::GeoAttrClass>(i));
        Bind(wxEVT_LIST_ITEM_SELECTED, &WxGeoProperty::SetPreviewCanvasDirty, this, list->GetId());
        Bind(wxEVT_LIST_ITEM_DESELECTED, &WxGeoProperty::SetPreviewCanvasDirty, this, list->GetId());
        m_lists2[i] = list;
    }

    AddPage(m_lists2[static_cast<int>(sop::GeoAttrClass::Point)],     "Points");
    AddPage(m_lists2[static_cast<int>(sop::GeoAttrClass::Vertex)],    "Vertices");
    AddPage(m_lists2[static_cast<int>(sop::GeoAttrClass::Primitive)], "Primitives");
    AddPage(m_lists2[static_cast<int>(sop::GeoAttrClass::Detail)],    "Detail");
}

void WxGeoProperty::Clear()
{
    for (auto list : m_lists2) {
        list->ClearAll();
    }
}

void WxGeoProperty::SetPreviewCanvasDirty(wxListEvent& event)
{
    m_sub_mgr->NotifyObservers(MSG_REFRESH_PREVIEW_CANVAS);
}

}