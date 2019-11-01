#include "sopview/WxGeoPropList.h"

#include <sop/GeometryImpl.h>

namespace
{

const size_t BASE_COUNT[] = { 4, 2, 1, 1 };

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

    // set item count
    auto& attr = m_geo->GetAttr();
    switch (m_type)
    {
    case sop::GeoAttrClass::Point:
        SetItemCount(attr.GetPoints().size());
        break;
    case sop::GeoAttrClass::Vertex:
        SetItemCount(attr.GetVertices().size());
        break;
    case sop::GeoAttrClass::Primitive:
        SetItemCount(attr.GetPrimtives().size());
        break;
    case sop::GeoAttrClass::Detail:
        SetItemCount(1);
        break;
    }

    // prepare column
    auto& attr_desc = attr.GetAttrDesc(m_type);
    for (auto& desc : attr_desc)
    {
        int num;
        switch (desc.GetType())
        {
        case sop::GeoAttrType::Float2:
            num = 2;
            break;
        case sop::GeoAttrType::Float3:
            num = 3;
            break;
        case sop::GeoAttrType::Float4:
            num = 4;
            break;
        case sop::GeoAttrType::Vector:
            num = 3;
            break;
        case sop::GeoAttrType::Vector4:
            num = 4;
            break;
        default:
            num = 1;
        }

        if (desc.GetAttr() == sop::GEO_ATTR_UNKNOWN)
        {
            for (int i = 0; i < num; ++i)
            {
                auto item_idx = GetColumnCount();
                auto name = GetAttrName(desc.GetAttr(), i);
                InsertColumn(item_idx, desc.GetName(), wxLIST_FORMAT_LEFT);
            }
        }
        else
        {
            for (int i = 0; i < num; ++i)
            {
                auto item_idx = GetColumnCount();
                auto name = GetAttrName(desc.GetAttr(), i);
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
        return GetOthersPropText(column, p->vars);
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
        return std::to_string(m_geo->GetAttr().QueryIndex(v->point));
    }
    // others
    else
    {
        return GetOthersPropText(column, v->vars);
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
        return GetOthersPropText(column, prim->vars);
    }    
}

wxString WxGeoPropList::GetDetailItemText(long item, long column) const
{
    assert(m_geo);
    auto& detail = m_geo->GetAttr().GetDetail();

    // name
    if (column == 0)
    {
        return "";
    }
    // others
    else
    {
        return GetOthersPropText(column, detail.vars);
    }
}

wxString WxGeoPropList::GetOthersPropText(size_t idx, const std::vector<sop::VarValue>& vars) const
{
    auto& desc = m_geo->GetAttr().GetAttrDesc(m_type);
    assert(desc.size() == vars.size());
    const size_t start_offset = BASE_COUNT[static_cast<int>(m_type)];
    int curr_idx = idx - start_offset;
    int var_idx = 0;
    while (true) 
    {
        assert(var_idx < static_cast<long>(vars.size()));
        auto var_n = GetAttrVarNum(desc[var_idx].GetType());
        if (curr_idx >= var_n) {
            curr_idx -= var_n;
        } else {
            return VarToString(desc[var_idx].GetType(), vars[var_idx], curr_idx);
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
    case sop::GeoAttrType::Float2:
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
    case sop::GeoAttrType::Float3:
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
    case sop::GeoAttrType::Float4:
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
    case sop::GeoAttrType::Vector:
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
    case sop::GeoAttrType::Vector4:
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

int WxGeoPropList::GetAttrVarNum(sop::GeoAttrType type) const
{
    switch (type)
    {
    case sop::GeoAttrType::Float2:
        return 2;
    case sop::GeoAttrType::Float3:
    case sop::GeoAttrType::Vector:
        return 3;
    case sop::GeoAttrType::Float4:
    case sop::GeoAttrType::Vector4:
        return 4;
    default:
        return 1;
    }
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

std::string WxGeoPropList::VarToString(sop::GeoAttrType type, const sop::VarValue& val, int index) const
{
    switch (type)
    {
    case sop::GeoAttrType::Bool:
        return val.b ? "true" : "false";
    case sop::GeoAttrType::Int:
        return std::to_string(val.i);
    case sop::GeoAttrType::Float:
        return std::to_string(val.f);
    case sop::GeoAttrType::Double:
        return std::to_string(val.d);
    case sop::GeoAttrType::Float2:
    {
        switch (index)
        {
        case 0:
            return std::to_string(static_cast<const sm::vec2*>(val.p)->x);
        case 1:
            return std::to_string(static_cast<const sm::vec2*>(val.p)->y);
        default:
            assert(0);
            return "";
        }
    }
    case sop::GeoAttrType::Float3:
    case sop::GeoAttrType::Vector:
    {
        switch (index)
        {
        case 0:
            return std::to_string(static_cast<const sm::vec3*>(val.p)->x);
        case 1:
            return std::to_string(static_cast<const sm::vec3*>(val.p)->y);
        case 2:
            return std::to_string(static_cast<const sm::vec3*>(val.p)->z);
        default:
            assert(0);
            return "";
        }
    }
    case sop::GeoAttrType::Float4:
    case sop::GeoAttrType::Vector4:
    {
        switch (index)
        {
        case 0:
            return std::to_string(static_cast<const sm::vec4*>(val.p)->x);
        case 1:
            return std::to_string(static_cast<const sm::vec4*>(val.p)->y);
        case 2:
            return std::to_string(static_cast<const sm::vec4*>(val.p)->z);
        case 3:
            return std::to_string(static_cast<const sm::vec4*>(val.p)->w);
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