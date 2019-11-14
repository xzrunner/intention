#pragma once

#include "sopview/Node.h"
#include "sopview/ReflectPropTypes.h"

#include <blueprint/Pin.h>

namespace sopv
{

void nodes_regist_rttr();

namespace node
{

#define SOPV_DEFINE_NODE(type, name, prop) \
class type : public Node                   \
{                                          \
public:                                    \
	type()                                 \
		: Node(#type)                      \
	{                                      \
		InitPins(#name);                   \
	}                                      \
                                           \
	prop                                   \
	RTTR_ENABLE(Node)                      \
};

#define SOPV_DEFINE_PROPS_NODE(type, name, prop) \
class type : public Node                         \
{                                                \
public:                                          \
	type()                                       \
		: Node(#type, true)                      \
	{                                            \
		InitPins(#name);                         \
	}                                            \
                                                 \
	prop                                         \
	RTTR_ENABLE(Node)                            \
};

#define SOPV_DEFINE_IMPORT_EXT_NODE(type, name, prop)                      \
class type : public Node                                             \
{                                                                    \
public:                                                              \
	type()                                                           \
		: Node(#type)                                                \
	{                                                                \
		InitPins(#name);                                             \
        SetExtensibleInputPorts(true);                               \
	}                                                                \
private:                                                             \
    virtual std::string GenInputPinName(size_t idx) const override { \
        return "in" + std::to_string(idx);                           \
    }                                                                \
                                                                     \
public:                                                              \
	prop                                                             \
	RTTR_ENABLE(Node)                                                \
};

#define SOPV_NODE_PROP

// base
SOPV_DEFINE_PROPS_NODE(Geometry, geometry,
    std::vector<bp::NodePtr> children; \
)

// attribute
SOPV_DEFINE_NODE(AttributeCreate, attribcreate::2.0,
    GroupName group_name;                             \
    GroupType group_type = GroupType::GuessFromGroup; \
    AttrCreateItem item0;                             \
    AttrCreateItem item1;                             \
    AttrCreateItem item2;                             \
    AttrCreateItem item3;                             \
)
SOPV_DEFINE_NODE(AttributePromote, attribpromote,
    AttributeName attr_name = GeoAttrClass::Point; \
    GeoAttrClass  from_cls  = GeoAttrClass::Point; \
    GeoAttrClass  to_cls    = GeoAttrClass::Point; \
)
SOPV_DEFINE_NODE(AttributeTransfer, attribtransfer,
    AttributeName points_attrs     = GeoAttrClass::Point;     \
    AttributeName vertices_attrs   = GeoAttrClass::Vertex;    \
    AttributeName primitives_attrs = GeoAttrClass::Primitive; \
    AttributeName detail_attrs     = GeoAttrClass::Detail;    \
)
SOPV_DEFINE_NODE(AttributeWrangle, attribwrangle,
    std::string vex_expr; \
)
SOPV_DEFINE_NODE(Measure, measure,
    MeasureType ms_type = MeasureType::Perimeter; \
    std::string ms_name;                          \
)
SOPV_DEFINE_NODE(Sort, sort,
    SortKey     key           = SortKey::NoChange; \
    bool        point_reverse = false;             \
    std::string point_offset  = "0";               \
)

// group
SOPV_DEFINE_NODE(GroupCreate, groupcreate,
    std::string group_name;                         \
    GroupType   group_type = GroupType::Primitives; \
    GroupMerge  merge_op = GroupMerge::Replace;     \
    bool        base_group = false;                 \
    std::string base_group_expr;                    \
    bool        keep_in_bounding = false;           \
    bool        keep_by_normals = false;            \
    sm::vec3    direction = sm::vec3(0, 0, 1);      \
    float       spread_angle = 180;                 \
)
SOPV_DEFINE_NODE(GroupExpression, groupexpression,
    GroupType   group_type = GroupType::Primitives; \
    GroupExprInst inst0;                            \
    GroupExprInst inst1;                            \
    GroupExprInst inst2;                            \
    GroupExprInst inst3;                            \
)
SOPV_DEFINE_NODE(GroupPromote, grouppromote,
    GroupName group_name;                           \
    GroupType src_type = GroupType::GuessFromGroup; \
    GroupType dst_type = GroupType::Points;         \
)

// import
SOPV_DEFINE_NODE(File, file,
    std::string filepath; \
)

// manipulate
SOPV_DEFINE_NODE(Delete, delete,
    bool             delete_non_selected = false;            \
    DeleteEntityType entity_type = DeleteEntityType::Points; \
    std::string      filter_exp;                             \
)
SOPV_DEFINE_NODE(Peak, peak,
    GroupName   group_name;                             \
    GroupType   group_type = GroupType::GuessFromGroup; \
    std::string distance = "0";                         \
)
SOPV_DEFINE_NODE(Transform, xform,
    GroupName group_name;                             \
    GroupType group_type = GroupType::GuessFromGroup; \
    StrVec3 translate = StrVec3("0", "0", "0");    \
    StrVec3 rotate = StrVec3("0", "0", "0");    \
    StrVec3 scale = StrVec3("1", "1", "1");    \
    StrVec3 shear = StrVec3("0", "0", "0");    \
)

// material
SOPV_DEFINE_NODE(Color, color,
    sm::vec3 color = sm::vec3(1.0f, 1.0f, 1.0f); \
)
SOPV_DEFINE_NODE(UVLayout, uvlayout, SOPV_NODE_PROP)
SOPV_DEFINE_NODE(UVQuickShade, uvquickshade,
    std::string image_file;    \
)
SOPV_DEFINE_NODE(UVTransform, uvtransform::2.0,
    GroupName group_name;                             \
    GroupType group_type = GroupType::GuessFromGroup; \
    StrVec3 translate = StrVec3("0", "0", "0");    \
    StrVec3 rotate = StrVec3("0", "0", "0");    \
    StrVec3 scale = StrVec3("1", "1", "1");    \
    StrVec3 shear = StrVec3("0", "0", "0");    \
)
SOPV_DEFINE_NODE(UVUnwrap, uvunwrap, SOPV_NODE_PROP)

// NURBs
SOPV_DEFINE_NODE(Carve, carve,
    std::string first_u = "0"; \
    std::string second_u = "1"; \
    std::string first_v = "0"; \
    std::string second_v = "1"; \
)

// polygon
SOPV_DEFINE_NODE(Add, add,
    bool use_p0 = false; \
    sm::vec3 p0;         \
    bool use_p1 = false; \
    sm::vec3 p1;         \
    bool use_p2 = false; \
    sm::vec3 p2;         \
    bool use_p3 = false; \
    sm::vec3 p3;         \
)
SOPV_DEFINE_NODE(Boolean, boolean::2.0,
    BooleanOperator op = BooleanOperator::Intersect; \
)
SOPV_DEFINE_NODE(Fuse, fuse,
    FuseOperator op = FuseOperator::Consolidate; \
    float distance = 0.001f;                     \
)
SOPV_DEFINE_NODE(Knife, knife,
    StrVec3   origin = StrVec3("0", "0", "0"); \
    sm::vec3  direction = sm::vec3(0, 1, 0);      \
    KnifeKeep keep = KnifeKeep::KeepAll;     \
)
SOPV_DEFINE_NODE(Normal, normal,
    GeoAttrClass attr_add_norm_to = GeoAttrClass::Vertex; \
)
SOPV_DEFINE_NODE(PolyExtrude, polyextrude::2.0,
    GroupName group_name;      \
    float distance = 0;        \
    bool output_front = true;  \
    bool output_back = false; \
    bool output_side = true;  \
    std::string front_group;   \
    std::string back_group;    \
    std::string side_group;    \
)
SOPV_DEFINE_NODE(PolyFill, polyfill, SOPV_NODE_PROP)
SOPV_DEFINE_NODE(PolyFrame, polyframe,
    GroupType      entity_type = GroupType::Primitives;     \
    PolyFrameStyle frame_style = PolyFrameStyle::TwoEdges;  \
    std::string    normal_name;                             \
    std::string    tangent_name;                            \
    std::string    bitangent_name;                          \
)

// primitive
SOPV_DEFINE_NODE(Box, box,
    StrVec3 size   = StrVec3("1", "1", "1"); \
    StrVec3 center = StrVec3("0", "0", "0"); \
    float scale    = 1.0f;                   \
)
SOPV_DEFINE_NODE(Curve, curve,
    std::vector<sm::vec3> vertices; \
)
SOPV_DEFINE_NODE(Grid, grid,
    size_t size_x  = 10; \
    size_t size_y  = 10; \
    size_t rows    = 10; \
    size_t columns = 10; \
)
SOPV_DEFINE_NODE(Line, line,
    sm::vec3    origin    = sm::vec3(0, 0, 0); \
    sm::vec3    direction = sm::vec3(0, 1, 0); \
    std::string length    = "1";               \
    size_t      points    = 2;                 \
)
SOPV_DEFINE_NODE(Primitive, primitive,
    GroupName group_name;                   \
    sm::vec3 translate = sm::vec3(0, 0, 0); \
    sm::vec3 rotate    = sm::vec3(0, 0, 0); \
    sm::vec3 scale     = sm::vec3(1, 1, 1); \
    sm::vec3 shear     = sm::vec3(0, 0, 0); \
)
SOPV_DEFINE_NODE(Sphere, sphere, SOPV_NODE_PROP)
// primitive extern
SOPV_DEFINE_NODE(Dungeon, dungeon,
    sm::vec2 size     = sm::vec2(1, 1);         \
    sm::vec2 split_sz = sm::vec2(0.5f, 0.5f);   \
    sm::vec2 min_sz   = sm::vec2(0.25f, 0.25f); \
    uint32_t seed     = 0;                      \
)

// utility
SOPV_DEFINE_NODE(Blast, blast,
    GroupName group_name;                             \
    GroupType group_type = GroupType::GuessFromGroup; \
    bool delete_non_selected = false;                 \
)
SOPV_DEFINE_NODE(CopyToPoints, copytopoints,
    GroupName   src_group    = GroupName(0); \
    GroupName   target_group = GroupName(1); \
    std::string target_group_str;            \
    bool        use_pt_dir = false;          \
)
SOPV_DEFINE_NODE(ForeachPrimBegin, block_begin, SOPV_NODE_PROP)
SOPV_DEFINE_NODE(ForeachPrimEnd, block_end,
    bool do_single_pass     = false; \
    int  single_pass_offset = 0;     \
)
SOPV_DEFINE_IMPORT_EXT_NODE(Merge, merge, SOPV_NODE_PROP)
SOPV_DEFINE_PROPS_NODE(Null, null, SOPV_NODE_PROP)
SOPV_DEFINE_NODE(Output, output,
    int output_idx = -1; \
)
SOPV_DEFINE_IMPORT_EXT_NODE(Split, split,
    GroupName group_name;          \
)
SOPV_DEFINE_IMPORT_EXT_NODE(Switch, switch,
    std::string selected = "0";     \
)

}

}