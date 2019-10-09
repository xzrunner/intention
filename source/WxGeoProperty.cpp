#include "sopview/WxGeoProperty.h"
#include "sopview/Node.h"
#include "sopview/SceneTree.h"
#include "sopview/Evaluator.h"

#include <blueprint/CompNode.h>

#include <node0/SceneNode.h>
#include <sop/GeometryImpl.h>
#include <sop/GeoAttrDefine.h>

#include <wx/listctrl.h>

namespace
{

void QueryVertexIndex(const sop::GeoAttribute& attr,
                      const std::shared_ptr<sop::GeoAttribute::Vertex>& vert,
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

std::string VarToString(sop::GeoAttrType type, const sop::VarValue& val, int index = -1)
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

std::string GetAttrName(sop::GeoAttr attr, int index = -1)
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

int GetAttrVarNum(sop::GeoAttrType type)
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

std::string TopoIDToString(const he::TopoID& id)
{
    std::string ret;
    for (auto& id : id.Path()) {
        ret += std::to_string(id) + ", ";
    }
    return ret.substr(0, ret.find_last_of(","));
}

std::string IDToString(const he::TopoID& topo_id, size_t group_id)
{
    return TopoIDToString(topo_id) + "; " + std::to_string(group_id);
}

const size_t BASE_COUNT[] = { 4, 2, 1, 1 };

}

namespace sopv
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
    m_lists[POINT]->InsertColumn(1, GetAttrName(sop::GEO_ATTR_POS, 0), wxLIST_FORMAT_LEFT);
    m_lists[POINT]->InsertColumn(2, GetAttrName(sop::GEO_ATTR_POS, 1), wxLIST_FORMAT_LEFT);
    m_lists[POINT]->InsertColumn(3, GetAttrName(sop::GEO_ATTR_POS, 2), wxLIST_FORMAT_LEFT);

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

void WxGeoProperty::LoadDefault(const sop::GeoAttribute& attr)
{
    // prepare column
    for (size_t i = 0; i < MAX_LIST_COUNT; ++i)
    {
        auto dst = m_lists[i];
        auto& attr_desc = attr.GetAttrDesc(static_cast<sop::GeoAttrClass>(i));
        for (auto& desc : attr_desc) 
        {
            int num;
            switch (desc.type)
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

            if (desc.attr == sop::GEO_ATTR_UNKNOWN)
            {
                for (int i = 0; i < num; ++i)
                {
                    auto item_idx = dst->GetColumnCount();
                    auto name = GetAttrName(desc.attr, i);
                    dst->InsertColumn(item_idx, desc.name, wxLIST_FORMAT_LEFT);
                }
            }
            else
            {
                for (int i = 0; i < num; ++i) 
                {
                    auto item_idx = dst->GetColumnCount();
                    auto name = GetAttrName(desc.attr, i);
                    dst->InsertColumn(item_idx, name, wxLIST_FORMAT_LEFT);
                }
            }
        }
    }

    // points
    auto p_list = m_lists[POINT];
    auto& pts = attr.GetPoints();
    auto& p_desc = attr.GetAttrDesc(sop::GeoAttrClass::Point);
    for (int i = 0, n = pts.size(); i < n; ++i)
    {
        auto& p = pts[i];
        long item = p_list->InsertItem(i, "");

        auto id_str = std::to_string(i);
        if (!p->topo_id.Empty()) {
            id_str += "(" + IDToString(p->topo_id, p->prim_id) + ")";
        }
        p_list->SetItem(item, 0, id_str);

        p_list->SetItem(item, 1, std::to_string(p->pos.x));
        p_list->SetItem(item, 2, std::to_string(p->pos.y));
        p_list->SetItem(item, 3, std::to_string(p->pos.z));

        assert(p_desc.size() == p->vars.size());

        int idx = 4;
        for (int j = 0, m = p->vars.size(); j < m; ++j) {
            auto var_n = GetAttrVarNum(p_desc[j].type);
            for (int k = 0; k < var_n; ++k) {
                auto str = VarToString(p_desc[j].type, p->vars[j], k);
                p_list->SetItem(item, idx++, str);
            }
        }
    }

    // vertices
    auto v_list = m_lists[VERTEX];
    auto& vts = attr.GetVertices();
    auto& v_desc = attr.GetAttrDesc(sop::GeoAttrClass::Vertex);
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

        int idx = 2;
        for (int j = 0, m = v->vars.size(); j < m; ++j) {
            auto var_n = GetAttrVarNum(v_desc[j].type);
            for (int k = 0; k < var_n; ++k) {
                auto str = VarToString(v_desc[j].type, v->vars[j], k);
                v_list->SetItem(item, idx++, str);
            }
        }
    }

    // primitives
    auto prim_list = m_lists[PRIMITIVE];
    auto& prims = attr.GetPrimtives();
    auto& prim_desc = attr.GetAttrDesc(sop::GeoAttrClass::Primitive);
    for (int i = 0, n = prims.size(); i < n; ++i)
    {
        auto& prim = prims[i];
        long item = prim_list->InsertItem(i, "");

        auto id_str = std::to_string(i);
        if (!prim->topo_id.Empty()) {
            id_str += "(" + IDToString(prim->topo_id, prim->prim_id) + ")";
        }
        prim_list->SetItem(item, 0, id_str);

        assert(prim_desc.size() == prim->vars.size());

        int idx = 1;
        for (int j = 0, m = prim->vars.size(); j < m; ++j) {
            auto var_n = GetAttrVarNum(prim_desc[j].type);
            for (int k = 0; k < var_n; ++k) {
                auto str = VarToString(prim_desc[j].type, prim->vars[j], k);
                prim_list->SetItem(item, idx++, str);
            }
        }
    }

    // detail
    auto detail_list = m_lists[DETAIL];
    auto& detail = attr.GetDetail();
    auto& detail_desc = attr.GetAttrDesc(sop::GeoAttrClass::Detail);
    long item = detail_list->InsertItem(0, "");
    detail_list->SetItem(item, 0, "");

    assert(detail_desc.size() == detail.vars.size());

    int idx = 0;
    for (int i = 0, n = detail.vars.size(); i < n; ++i) {
        auto var_n = GetAttrVarNum(detail_desc[i].type);
        for (int k = 0; k < var_n; ++k) {
            auto str = VarToString(detail_desc[i].type, detail.vars[i], k);
            detail_list->SetItem(item, idx++, str);
        }
    }
}

void WxGeoProperty::LoadGroups(const sop::GroupMgr& groups)
{
    groups.Traverse([&](const sop::Group& group)->bool
    {
        ListIndex idx = MAX_LIST_COUNT;
        switch (group.type)
        {
        case sop::GroupType::Points:
            idx = POINT;
            break;
        case sop::GroupType::Vertices:
            idx = VERTEX;
            break;
        case sop::GroupType::Primitives:
            idx = PRIMITIVE;
            break;
        default:
            assert(0);
            return true;
        }

        auto list = m_lists[idx];
        auto col = list->GetColumnCount();
        list->InsertColumn(col, "G:" + group.name, wxLIST_FORMAT_LEFT, 100);
        for (int i = 0, n = list->GetItemCount(); i < n; ++i) {
            list->SetItem(i, col, "0");
        }
        auto n = list->GetItemCount();
        for (int i = 0, n = group.items.size(); i < n; ++i)
        {
            auto idx = group.items[i];
            assert(static_cast<int>(idx) < list->GetItemCount());
            list->SetItem(idx, col, "1");
        }

        return true;
    });
}

}