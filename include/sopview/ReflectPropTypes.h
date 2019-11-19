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

enum class GroupTypes
{
    Auto = 0,
    Primitives,
    Points,
    Edges,
    Vertices,
};

enum class GroupBoundingType
{
    Box,
    Sphere,
    Object,
    Volume,
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
    bool        enable = false;
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

enum class BooleanGeoType
{
    Solid,
    Surface,
};

enum class BooleanSubType
{
    AMinusB,
    BMinusA,
    Both,
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

enum class GeoAttrClassType
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

enum class AttrCreateType
{
    Float,
    Integer,
    Vector,
    String,
    FloatArray,
    IntegerArray,
    StringArray,
};

enum class AttrCreateFloatInfo
{
    Guess,
    None,
    Position,
    Vector,
    Normal,
    Color,
    Quaternion,
    TransformMatrix,
    TextureCoordinate,
};

struct AttrCreateItem
{
    std::string    name;
    GeoAttrClass   cls  = GeoAttrClass::Point;
    AttrCreateType type = AttrCreateType::Float;
    size_t         size = 1;
    sm::vec4       value;
    sm::vec4       default_val;
    std::string    string;
    AttrCreateFloatInfo flt_info = AttrCreateFloatInfo::Guess;
};

struct AttributeName
{
    AttributeName(GeoAttrClass cls)
        : cls(cls)
    {
    }

    GeoAttrClass cls;

    std::string str;

}; // AttributeName

enum class SortKey
{
    NoChange,
    X,
    Y,
    Z,
    Shift,
};

enum class DeleteEntityType
{
    Points,
    Edges,
    Faces,
};

enum class DeleteOperation
{
    Pattern,
    Range,
    Expression,
};

enum class MeasureType
{
    Perimeter,
    Area,
    Curvature,
    Volume,
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

enum class PolyExtrudeExtrusion
{
    PrimEdgeNorm,
    PointNorm,
};

enum class PolyFillMode
{
    SinglePolygon,
    Triangles,
    TriangleFan,
    QuadrilateralFan,
    Quadrilaterals,
    QuadrilateralGrid,
};

enum class FuseOperator
{
    Consolidate,
    UniquePoints,
    Snap,
};

enum class ForeachBeginMethod
{
    Feedback,
    PieceOrPoint,
    Metadata,
    Input,
};

enum class ForeachIterMethod
{
    AutoDetectFromInputs,
    ByPiecesOrPoints,
    ByCount,
};

enum class ForeachGatherMethod
{
    Feedback,
    Merge,
};

enum class ForeachPieceElements
{
    Primitives,
    Points,
};

enum class UVScale
{
    None,
    Uniform,
    Stretch,
};

void prop_types_regist_rttr();

}