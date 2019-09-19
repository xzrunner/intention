#include "intention/WxGeoProperty.h"
#include "intention/Node.h"
#include "intention/SceneTree.h"
#include "intention/Evaluator.h"

#include <blueprint/CompNode.h>

#include <node0/SceneNode.h>
#include <everything/GeometryImpl.h>
#include <everything/GeoAttrName.h>

#include <wx/listctrl.h>

namespace
{

void QueryVertexIndex(const evt::GeoAttribute& attr,
                      const std::shared_ptr<evt::GeoAttribute::Vertex>& vert,
                      int& prim_idx, int& vert_idx)
{
    auto& prims = attr.GetPrimtives();

    prim_idx = -1;
    vert_idx = -1;
    auto prim = vert->prim.lock();
    for (int i = 0, n = prims.size(); i < n; ++i)
    {
        if (prims[i] != prim) {
            continue;
        }

        prim_idx = i;

        for (int j = 0, m = prim->vertices.size(); j < m; ++j) {
            if (vert == prim->vertices[j]) {
                vert_idx = j;
                break;
            }
        }

        break;
    }
}

std::string VarToString(const evt::VarType& type, const evt::VarValue& val)
{
    switch (type)
    {
    case evt::VarType::Bool:
        return val.b ? "true" : "false";
    case evt::VarType::Int:
        return std::to_string(val.i);
    case evt::VarType::Float:
        return std::to_string(val.f);
    case evt::VarType::Double:
        return std::to_string(val.d);
    default:
        assert(0);
        return "";
    }
}

const size_t BASE_COUNT[] = { 4, 2, 1, 1 };

}

namespace itt
{

WxGeoProperty::WxGeoProperty(wxWindow* parent, const std::shared_ptr<SceneTree>& stree)
    : wxNotebook(parent, wxID_ANY, wxDefaultPosition, wxSize(500, -1))
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
    auto itt_node = std::static_pointer_cast<Node>(bp_node);
    auto evt_node = m_stree->GetCurrEval()->QueryBackNode(*itt_node);
    if (!evt_node) {
        return;
    }

    auto geo = evt_node->GetGeometry();
    if (!geo) {
        return;
    }
    LoadDefault(geo->GetAttr());
    LoadGroups(geo->GetGroup());
}

void WxGeoProperty::InitLayout()
{
    for (int i = 0; i < MAX_LIST_COUNT; ++i) {
        m_lists[i] = new wxListCtrl(this, wxID_ANY, wxDefaultPosition,
            wxDefaultSize, wxLC_REPORT);
    }
    AddPage(m_lists[POINT],     "Points");
    AddPage(m_lists[VERTEX],    "Vertices");
    AddPage(m_lists[PRIMITIVE], "Primitives");
    AddPage(m_lists[DETAIL],    "Detail");

    m_lists[POINT]->InsertColumn(0, "ID",   wxLIST_FORMAT_LEFT);
    m_lists[POINT]->InsertColumn(1, evt::GeoAttrName::vert_x, wxLIST_FORMAT_LEFT);
    m_lists[POINT]->InsertColumn(2, evt::GeoAttrName::vert_y, wxLIST_FORMAT_LEFT);
    m_lists[POINT]->InsertColumn(3, evt::GeoAttrName::vert_z, wxLIST_FORMAT_LEFT);

    m_lists[VERTEX]->InsertColumn(0, "ID",        wxLIST_FORMAT_LEFT);
    m_lists[VERTEX]->InsertColumn(1, "Point Num", wxLIST_FORMAT_LEFT);

    m_lists[PRIMITIVE]->InsertColumn(0, "ID", wxLIST_FORMAT_LEFT);

    m_lists[DETAIL]->InsertColumn(0, "ID", wxLIST_FORMAT_LEFT);
}

void WxGeoProperty::Clear()
{
    for (int i = 0; i < MAX_LIST_COUNT; ++i)
    {
        auto list = m_lists[i];

        list->DeleteAllItems();

        const auto& base_n = BASE_COUNT[i];
        for (int j = base_n - 1, m = list->GetColumnCount(); j < m; ++j) {
            list->DeleteColumn(base_n);
        }
    }
}

void WxGeoProperty::LoadDefault(const evt::GeoAttribute& attr)
{
    // prepare column
    for (size_t i = 0; i < MAX_LIST_COUNT; ++i)
    {
        auto dst = m_lists[i];
        auto& attr_desc = attr.GetAttrDesc(static_cast<evt::GeoAttrType>(i));
        for (auto& desc : attr_desc) {
            auto item_idx = dst->GetColumnCount();
            dst->InsertColumn(item_idx, desc.name, wxLIST_FORMAT_LEFT);
        }
    }

    // points
    auto p_list = m_lists[POINT];
    auto& pts = attr.GetPoints();
    auto& p_desc = attr.GetAttrDesc(evt::GeoAttrType::Point);
    for (int i = 0, n = pts.size(); i < n; ++i)
    {
        auto& p = pts[i];
        long item = p_list->InsertItem(i, "");
        p_list->SetItem(item, 0, std::to_string(i));
        p_list->SetItem(item, 1, std::to_string(p->pos.x));
        p_list->SetItem(item, 2, std::to_string(p->pos.y));
        p_list->SetItem(item, 3, std::to_string(p->pos.z));

        assert(p_desc.size() == p->vars.size());
        for (int j = 0, m = p->vars.size(); j < m; ++j) {
            auto str = VarToString(p_desc[j].type, p->vars[j]);
            p_list->SetItem(item, 4 + j, str);
        }
    }

    // vertices
    auto v_list = m_lists[VERTEX];
    auto& vts = attr.GetVertices();
    auto& v_desc = attr.GetAttrDesc(evt::GeoAttrType::Vertex);
    for (int i = 0, n = vts.size(); i < n; ++i)
    {
        auto& v = vts[i];
        long item = v_list->InsertItem(i, "");

        int prim_idx, vert_idx;
        QueryVertexIndex(attr, v, prim_idx, vert_idx);
        std::string idx_str = std::to_string(prim_idx) + ":" + std::to_string(vert_idx);
        v_list->SetItem(item, 0, idx_str);

        const int point_idx = attr.QueryIndex(v->point);
        v_list->SetItem(item, 1, std::to_string(point_idx));

        assert(v_desc.size() == v->vars.size());
        for (int j = 0, m = v->vars.size(); j < m; ++j) {
            auto str = VarToString(v_desc[j].type, v->vars[j]);
            v_list->SetItem(item, 2 + j, str);
        }
    }

    // primitives
    auto prim_list = m_lists[PRIMITIVE];
    auto& prims = attr.GetPrimtives();
    auto& prim_desc = attr.GetAttrDesc(evt::GeoAttrType::Primitive);
    for (int i = 0, n = prims.size(); i < n; ++i)
    {
        auto& p = prims[i];
        long item = prim_list->InsertItem(i, "");
        prim_list->SetItem(item, 0, std::to_string(i));

        assert(prim_desc.size() == p->vars.size());
        for (int j = 0, m = p->vars.size(); j < m; ++j) {
            auto str = VarToString(prim_desc[j].type, p->vars[j]);
            prim_list->SetItem(item, 1 + j, str);
        }
    }

    // detail
    auto detail_list = m_lists[DETAIL];
    auto& detail = attr.GetDetail();
    auto& detail_desc = attr.GetAttrDesc(evt::GeoAttrType::Detail);
    long item = detail_list->InsertItem(0, "");
    detail_list->SetItem(item, 0, "");
    assert(detail_desc.size() == detail.vars.size());
    for (int i = 0, n = detail.vars.size(); i < n; ++i) {
        auto str = VarToString(detail_desc[i].type, detail.vars[i]);
        prim_list->SetItem(item, i, str);
    }
}

void WxGeoProperty::LoadGroups(const evt::GroupMgr& groups)
{
    groups.Traverse([&](const evt::Group& group)->bool
    {
        ListIndex idx = MAX_LIST_COUNT;
        switch (group.type)
        {
        case evt::GroupType::Points:
            idx = POINT;
            break;
        case evt::GroupType::Vertices:
            idx = VERTEX;
            break;
        case evt::GroupType::Primitives:
            idx = PRIMITIVE;
            break;
        default:
            assert(0);
            return true;
        }

        auto list = m_lists[idx];
        auto item_idx = list->GetColumnCount();
        list->InsertColumn(item_idx, "group:" + group.name, wxLIST_FORMAT_LEFT);
        for (int i = 0, n = list->GetItemCount(); i < n; ++i) {
            list->SetItem(i, item_idx, "0");
        }
        auto n = list->GetItemCount();
        for (int i = 0, n = group.items.size(); i < n; ++i) {
            list->SetItem(group.items[i], item_idx, "1");
        }

        return true;
    });
}

}