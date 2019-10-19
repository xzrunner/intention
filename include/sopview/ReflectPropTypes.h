#pragma once

#include <SM_Vector.h>

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

enum class GeoAttrClass
{
    Point = 0,
    Vertex,
    Primitive,
    Detail,
};

enum class GeoAttrType
{
    Int,

    // todo
    Bool,
    Double,

    Float,
    Float2,
    Float3,
    Float4,

    String,

    Vector,
    Vector4,

    Matrix2,
    Matrix3,
    Matrix4,
};

struct AttrCreateItem
{
    std::string  name;
    GeoAttrClass cls  = GeoAttrClass::Point;
    GeoAttrType  type = GeoAttrType::Float;
    sm::vec4     value;
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

enum class PolyFrameStyle
{
    FirstEdge,
    TwoEdges,
    PrimitiveCentroid,
    TextureUV,
    TextureUVGradient,
    AttributeGradient,
    MikkT,
};

void prop_types_regist_rttr();

}