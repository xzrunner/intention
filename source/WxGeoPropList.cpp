#include "sopview/WxGeoPropList.h"

#include <sop/GeometryImpl.h>
#include <sop/ParmList.h>

namespace
{

const size_t BASE_COUNT[] = { 4, 2, 1, 1 };

size_t get_parm_type_size(sop::ParmType type)
{
    int num = 0;
    switch (type)
    {
#define PARM_INFO(type, name, label, size) \
        case sop::ParmType::##type:        \
            num = size;                    \
            break;
#include <sop/parm_cfg.h>
#undef PARM_INFO
    default:
        num = 1;
    }
    return num;
}

}

namespace sopv
{

WxGeoPropList::WxGeoPropList(wxWindow* parent, sop::GeoAttrClass type)
    : wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_VIRTUAL)
    , m_type(type)
{
    InitColumns();
}

std::vector<size_t> WxGeoPropList::GetSelectedIndices() const
{
    std::vector<size_t> ret;

    int item = -1;
    for (;;)
    {
        item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (item == -1) {
            break;
        }

        ret.push_back(item);
    }

    return ret;
}

void WxGeoPropList::ClearAll()
{
    DeleteAllItems();

    const auto& base_n = BASE_COUNT[static_cast<int>(m_type)];
    for (int i = base_n - 1, n = GetColumnCount(); i < n; ++i) {
        DeleteColumn(base_n);
    }
}

void WxGeoPropList::SetGeoData(const std::shared_ptr<sop::GeometryImpl>& geo)
{
    m_geo = geo;

    if (!m_geo) {
        return;
    }

    auto& attr = m_geo->GetAttr();

    // set item count
    SetItemCount(attr.GetSize(m_type));

    // prepare column
    auto& lists = attr.GetAllParmLists()[static_cast<int>(m_type)];
    for (auto& list : lists)
    {
        const auto num = get_parm_type_size(list->GetType());
        if (list->GetAttr() == sop::GEO_ATTR_UNKNOWN)
        {
            for (size_t i = 0; i < num; ++i)
            {
                auto item_idx = GetColumnCount();
                auto name = GetAttrName(list->GetAttr(), i);
                InsertColumn(item_idx, list->GetName(), wxLIST_FORMAT_LEFT);
            }
        }
        else
        {
            for (size_t i = 0; i < num; ++i)
            {
                auto item_idx = GetColumnCount();
                auto name = GetAttrName(list->GetAttr(), i);
                InsertColumn(item_idx, name, wxLIST_FORMAT_LEFT);
            }
        }
    }
}

wxString WxGeoPropList::OnGetItemText(long item, long column) const
{
    if (!m_geo) {
        return "";
    }

    switch (m_type)
    {
    case sop::GeoAttrClass::Point:
        return GetPointItemText(item, column);
    case sop::GeoAttrClass::Vertex:
        return GetVertexItemText(item, column);
    case sop::GeoAttrClass::Primitive:
        return GetPrimitiveItemText(item, column);
    case sop::GeoAttrClass::Detail:
        return GetDetailItemText(item, column);
    }

    return "";
}

void WxGeoPropList::InitColumns()
{
    switch (m_type)
    {
    case sop::GeoAttrClass::Point:
        InsertColumn(0, "ID", wxLIST_FORMAT_LEFT);
        InsertColumn(1, GetAttrName(sop::GEO_ATTR_POS, 0), wxLIST_FORMAT_LEFT);
        InsertColumn(2, GetAttrName(sop::GEO_ATTR_POS, 1), wxLIST_FORMAT_LEFT);
        InsertColumn(3, GetAttrName(sop::GEO_ATTR_POS, 2), wxLIST_FORMAT_LEFT);
        break;
    case sop::GeoAttrClass::Vertex:
        InsertColumn(0, "ID", wxLIST_FORMAT_LEFT);
        InsertColumn(1, "Point Num", wxLIST_FORMAT_LEFT);
        break;
    case sop::GeoAttrClass::Primitive:
        InsertColumn(0, "ID", wxLIST_FORMAT_LEFT);
        break;
    case sop::GeoAttrClass::Detail:
        InsertColumn(0, "ID", wxLIST_FORMAT_LEFT);
        break;
    }
}

wxString WxGeoPropList::GetPointItemText(long item, long column) const
{
    assert(m_geo);
    auto& pts = m_geo->GetAttr().GetPoints();
    assert(item < static_cast<long>(pts.size()));
    auto& p = pts[item];

    // name
    if (column == 0)
    {
        auto str = std::to_string(item);
        if (!p->topo_id.Empty()) {
            str += "(" + IDToString(p->topo_id, p->prim_id) + ")";
        }
        return str;
    }
    // pos
    else if (column >= 1 && column <= 3)
    {
        return std::to_string(p->pos.xyz[column - 1]);
    }
    // others
    else
    {
        assert(m_geo);
        auto& lists = m_geo->GetAttr().GetAllParmLists()[static_cast<int>(sop::GeoAttrClass::Point)];
        return GetOthersPropText(item, column, lists);
    }
}

wxString WxGeoPropList::GetVertexItemText(long item, long column) const
{
    assert(m_geo);
    auto& vts = m_geo->GetAttr().GetVertices();
    assert(item < static_cast<long>(vts.size()));
    auto& v = vts[item];

    // name
    if (column == 0)
    {
        int prim_idx, vert_idx;
        QueryVertexIndex(m_geo->GetAttr(), v, prim_idx, vert_idx);
        return std::to_string(prim_idx) + ":" + std::to_string(vert_idx);
    }
    // point idx
    else if (column == 1)
    {
        return std::to_string(v->point->attr_idx);
    }
    // others
    else
    {
        assert(m_geo);
        auto& lists = m_geo->GetAttr().GetAllParmLists()[static_cast<int>(sop::GeoAttrClass::Vertex)];
        return GetOthersPropText(item, column, lists);
    }
}

wxString WxGeoPropList::GetPrimitiveItemText(long item, long column) const
{
    assert(m_geo);
    auto& prims = m_geo->GetAttr().GetPrimtives();
    assert(item < static_cast<long>(prims.size()));
    auto& prim = prims[item];

    // name
    if (column == 0)
    {
        auto str = std::to_string(item);
        if (!prim->topo_id.Empty()) {
            str += "(" + IDToString(prim->topo_id, prim->prim_id) + ")";
        }
        return str;
    }
    // others
    else
    {
        assert(m_geo);
        auto& lists = m_geo->GetAttr().GetAllParmLists()[static_cast<int>(sop::GeoAttrClass::Primitive)];
        return GetOthersPropText(item, column, lists);
    }
}

wxString WxGeoPropList::GetDetailItemText(long item, long column) const
{
    // name
    if (column == 0)
    {
        return "";
    }
    // others
    else
    {
        assert(m_geo);
        auto& lists = m_geo->GetAttr().GetAllParmLists()[static_cast<int>(sop::GeoAttrClass::Detail)];
        return GetOthersPropText(item, column, lists);
    }
}

wxString WxGeoPropList::GetOthersPropText(long item, long column, const std::vector<std::shared_ptr<sop::ParmList>>& lists) const
{
    const size_t start_offset = BASE_COUNT[static_cast<int>(m_type)];
    int curr_idx = column - start_offset;
    int list_idx = 0;
    while (true)
    {
        const auto var_n = get_parm_type_size(lists[list_idx]->GetType());
        if (curr_idx >= static_cast<int>(var_n)) {
            curr_idx -= var_n;
            ++list_idx;
        } else {
            return VarToString(lists[list_idx]->GetType(), *lists[list_idx], item, curr_idx);
        }
    }
    return "";
}


void WxGeoPropList::QueryVertexIndex(const sop::GeoAttribute& attr, const std::shared_ptr<sop::GeoAttribute::Vertex>& vert,
                                     int& prim_idx, int& vert_idx) const
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

std::string WxGeoPropList::GetAttrName(sop::GeoAttr attr, int index) const
{
    std::string name = sop::GeoAttrNames[attr];
    switch (sop::GeoAttrTypes[attr])
    {
    case sop::ParmType::Float2:
        switch (index)
        {
        case 0:
            name += "[0]";
            break;
        case 1:
            name += "[1]";
            break;
        default:
            assert(0);
        }
        break;
    case sop::ParmType::Float3:
        switch (index)
        {
        case 0:
            name += "[0]";
            break;
        case 1:
            name += "[1]";
            break;
        case 2:
            name += "[2]";
            break;
        default:
            assert(0);
        }
        break;
    case sop::ParmType::Float4:
        switch (index)
        {
        case 0:
            name += "[0]";
            break;
        case 1:
            name += "[1]";
            break;
        case 2:
            name += "[2]";
            break;
        case 3:
            name += "[3]";
            break;
        default:
            assert(0);
        }
        break;
    case sop::ParmType::Vector:
        switch (index)
        {
        case 0:
            name += "[x]";
            break;
        case 1:
            name += "[y]";
            break;
        case 2:
            name += "[z]";
            break;
        default:
            assert(0);
        }
        break;
    case sop::ParmType::Vector4:
        switch (index)
        {
        case 0:
            name += "[x]";
            break;
        case 1:
            name += "[y]";
            break;
        case 2:
            name += "[z]";
            break;
        case 3:
            name += "[w]";
            break;
        default:
            assert(0);
        }
        break;
    }
    return name;
}

std::string WxGeoPropList::TopoIDToString(const he::TopoID& id) const
{
    std::string ret;
    for (auto& id : id.Path()) {
        ret += std::to_string(id) + ", ";
    }
    return ret.substr(0, ret.find_last_of(","));
}

std::string WxGeoPropList::IDToString(const he::TopoID& topo_id, size_t group_id) const
{
    return TopoIDToString(topo_id) + "; " + std::to_string(group_id);
}

std::string WxGeoPropList::VarToString(sop::ParmType type, const sop::ParmList& list, size_t item, int comp_idx) const
{
    assert(list.Type() == type);
    switch (type)
    {
    case sop::ParmType::Boolean:
    {
        auto& data = static_cast<const sop::ParmBoolList&>(list).GetAllItems();
        assert(item < data.size());
        return data[item] ? "true" : "false";
    }
    case sop::ParmType::Int:
    {
        auto& data = static_cast<const sop::ParmIntList&>(list).GetAllItems();
        assert(item < data.size());
        return std::to_string(data[item]);
    }
    case sop::ParmType::Float:
    {
        auto& data = static_cast<const sop::ParmFltList&>(list).GetAllItems();
        assert(item < data.size());
        return std::to_string(data[item]);
    }
    case sop::ParmType::Float2:
    {
        auto& data = static_cast<const sop::ParmFlt2List&>(list).GetAllItems();
        assert(item < data.size());
        switch (comp_idx)
        {
        case 0:
            return std::to_string(data[item].x);
        case 1:
            return std::to_string(data[item].y);
        default:
            assert(0);
            return "";
        }
    }
    case sop::ParmType::Float3:
    case sop::ParmType::Vector:
    {
        auto& data = static_cast<const sop::ParmFlt3List&>(list).GetAllItems();
        assert(item < data.size());
        switch (comp_idx)
        {
        case 0:
            return std::to_string(data[item].x);
        case 1:
            return std::to_string(data[item].y);
        case 2:
            return std::to_string(data[item].z);
        default:
            assert(0);
            return "";
        }
    }
    case sop::ParmType::Float4:
    case sop::ParmType::Vector4:
    {
        auto& data = static_cast<const sop::ParmFlt4List&>(list).GetAllItems();
        assert(item < data.size());
        switch (comp_idx)
        {
        case 0:
            return std::to_string(data[item].x);
        case 1:
            return std::to_string(data[item].y);
        case 2:
            return std::to_string(data[item].z);
        case 3:
            return std::to_string(data[item].w);
        default:
            assert(0);
            return "";
        }
    }
    default:
        assert(0);
        return "";
    }
}

}