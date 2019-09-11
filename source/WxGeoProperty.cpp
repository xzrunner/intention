#include "intention/WxGeoProperty.h"
#include "intention/Node.h"
#include "intention/SceneTree.h"
#include "intention/Evaluator.h"

#include <blueprint/CompNode.h>

#include <node0/SceneNode.h>
#include <everything/GeometryImpl.h>

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

const size_t BASE_COUNT[] = { 4, 2, 1, 0 };

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

    m_lists[POINT]->InsertColumn(0, "",     wxLIST_FORMAT_LEFT);
    m_lists[POINT]->InsertColumn(1, "P[x]", wxLIST_FORMAT_LEFT);
    m_lists[POINT]->InsertColumn(2, "P[y]", wxLIST_FORMAT_LEFT);
    m_lists[POINT]->InsertColumn(3, "P[z]", wxLIST_FORMAT_LEFT);

    m_lists[VERTEX]->InsertColumn(0, "",          wxLIST_FORMAT_LEFT);
    m_lists[VERTEX]->InsertColumn(1, "Point Num", wxLIST_FORMAT_LEFT);

    m_lists[PRIMITIVE]->InsertColumn(0, "", wxLIST_FORMAT_LEFT);
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
    // points
    auto p_list = m_lists[POINT];
    auto& pts = attr.GetPoints();
    for (int i = 0, n = pts.size(); i < n; ++i)
    {
        auto& p = pts[i];
        long item = p_list->InsertItem(i, "");
        p_list->SetItem(item, 0, std::to_string(i));
        p_list->SetItem(item, 1, std::to_string(p->pos.x));
        p_list->SetItem(item, 2, std::to_string(p->pos.y));
        p_list->SetItem(item, 3, std::to_string(p->pos.z));
    }

    // vertices
    auto v_list = m_lists[VERTEX];
    auto& vts = attr.GetVertices();
    for (int i = 0, n = vts.size(); i < n; ++i)
    {
        auto& v = vts[i];
        long item = v_list->InsertItem(i, "");

        int prim_idx, vert_idx;
        QueryVertexIndex(attr, v, prim_idx, vert_idx);
        std::string idx_str = std::to_string(prim_idx) + ":" + std::to_string(vert_idx);
        v_list->SetItem(item, 0, idx_str);

        v_list->SetItem(item, 1, std::to_string(v->point->order));
    }

    // primitives
    auto prim_list = m_lists[PRIMITIVE];
    auto& prims = attr.GetPrimtives();
    for (int i = 0, n = prims.size(); i < n; ++i)
    {
        auto& p = prims[i];
        long item = prim_list->InsertItem(i, "");
        prim_list->SetItem(item, 0, std::to_string(i));
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