#pragma once

#include <string>

namespace itt
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

enum class GroupType
{
    GuessFromGroup,
    Primitives,
    Points,
    Edges,
    Vertices,
};

enum class GroupMerge
{
    Replace,
    Union,
    Intersect,
    Subtract,
};

struct GroupExprInst
{
    std::string group_name;
    std::string expr_str;
    GroupMerge  merge_op = GroupMerge::Union;

}; // GroupExprInst

enum class BooleanOperator
{
    Union,
    Intersect,
    Subtract,
};

enum class KnifeKeep
{
    KeepAbove,
    KeepBelow,
    KeepAll,
};

enum class GeoAttrType
{
    Point = 0,
    Vertex,
    Primitive,
    Detail,
};

enum class SortKey
{
    NoChange,
    X,
    Y,
    Z
};

enum class DeleteEntityType
{
    Points,
    Edges,
    Faces,
};

enum class MeasureType
{
    Perimeter,
    Area
};

void prop_types_regist_rttr();

}