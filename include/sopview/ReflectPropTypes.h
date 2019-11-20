#pragma once

#include <SM_Vector.h>
#include <sop/node/AttributeCreate.h>

#include <string>

namespace sopv
{

struct StrVec3
{
    StrVec3(const std::string& x,
            const std::string& y,
            const std::string& z)
        : x(x), y(y), z(z) {}

    std::string x;
    std::string y;
    std::string z;

}; // StrVec3

struct GroupName
{
    GroupName(int idx = -1) : idx(idx) {}

    std::string str;
    int idx = -1;   // input num, -1 for self

}; // GroupName

enum class GroupTypes
{
    Auto = 0,
    Primitives,
    Points,
    Edges,
    Vertices,
};

struct GroupExprInst
{
    bool        enable = false;
    std::string group_name;
    std::string expr_str;
    sop::GroupMerge  merge_op = sop::GroupMerge::Union;

}; // GroupExprInst

enum class NormalGeoAttrClass
{
    Point = 0,
    Vertex,
    Primitive,
    Detail,
};

struct AttrCreateItem
{
    std::string    name;
    sop::GeoAttrClass   cls  = sop::GeoAttrClass::Point;
    sop::node::AttributeCreate::ItemType type = sop::node::AttributeCreate::ItemType::Float;
    size_t         size = 1;
    sm::vec4       value;
    sm::vec4       default_val;
    std::string    string;
    sop::node::AttributeCreate::ItemFltInfo flt_info = sop::node::AttributeCreate::ItemFltInfo::Guess;
};

struct AttributeName
{
    AttributeName(sop::GeoAttrClass cls)
        : cls(cls)
    {
    }

    sop::GeoAttrClass cls;

    std::string str;

}; // AttributeName

void prop_types_regist_rttr();

}