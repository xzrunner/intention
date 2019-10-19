#pragma once

#include "sopview/Node.h"
#include "sopview/ReflectPropTypes.h"

#include <blueprint/Pin.h>

namespace sopv
{

void nodes_regist_rttr();

namespace node
{

#define SOPV_DEFINE_NODE(name, prop)         \
class name : public Node                     \
{                                            \
public:                                      \
	name()                                   \
		: Node(#name)                        \
	{                                        \
		InitPins(#name);                     \
	}                                        \
                                             \
	prop                                     \
	RTTR_ENABLE(Node)                        \
};

#define SOPV_DEFINE_PROPS_NODE(name, prop)   \
class name : public Node                     \
{                                            \
public:                                      \
	name()                                   \
		: Node(#name, true)                  \
	{                                        \
		InitPins(#name);                     \
	}                                        \
                                             \
	prop                                     \
	RTTR_ENABLE(Node)                        \
};

#define SOPV_DEFINE_IMPORT_EXT_NODE(name, prop)                      \
class name : public Node                                             \
{                                                                    \
public:                                                              \
	name()                                                           \
		: Node(#name)                                                \
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
SOPV_DEFINE_PROPS_NODE(Geometry,       \
    std::vector<bp::NodePtr> children; \
)

// attribute
SOPV_DEFINE_NODE(AttributeCreate, \
    AttrCreateItem item0;         \
    AttrCreateItem item1;         \
    AttrCreateItem item2;         \
    AttrCreateItem item3;         \
)
SOPV_DEFINE_NODE(AttributeTransfer, \
    std::string points_attrs;       \
    std::string vertices_attrs;     \
    std::string primitives_attrs;   \
    std::string detail_attrs;       \
)
SOPV_DEFINE_NODE(AttributeWrangle, \
    std::string vex_expr;          \
)
SOPV_DEFINE_NODE(Measure,                         \
    MeasureType ms_type = MeasureType::Perimeter; \
    std::string ms_name;                          \
)
SOPV_DEFINE_NODE(Sort,               \
    SortKey key = SortKey::NoChange; \
)

// group
SOPV_DEFINE_NODE(GroupCreate,                         \
    std::string group_name;                           \
    GroupType   group_type = GroupType::Primitives;   \
    GroupMerge  merge_op   = GroupMerge::Replace;     \
    bool        base_group = false;                   \
    std::string base_group_expr;                      \
    bool        keep_in_bounding = false;             \
    bool        keep_by_normals  = false;             \
    sm::vec3    direction        = sm::vec3(0, 0, 1); \
    float       spread_angle     = 180;               \
)
SOPV_DEFINE_NODE(GroupExpression,                   \
    GroupType   group_type = GroupType::Primitives; \
    GroupExprInst inst0;                            \
    GroupExprInst inst1;                            \
    GroupExprInst inst2;                            \
    GroupExprInst inst3;                            \
)
SOPV_DEFINE_NODE(GroupPromote,                      \
    GroupName group_name;                           \
    GroupType src_type = GroupType::GuessFromGroup; \
    GroupType dst_type = GroupType::Points;         \
)

// manipulate
SOPV_DEFINE_NODE(Delete,                                     \
    bool             delete_non_selected = false;            \
    DeleteEntityType entity_type = DeleteEntityType::Points; \
    std::string      filter_exp;                             \
)
SOPV_DEFINE_NODE(Transform,                           \
    GroupName group_name;                             \
    GroupType group_type = GroupType::GuessFromGroup; \
    StrVec3 translate    = StrVec3("0", "0", "0");    \
    StrVec3 rotate       = StrVec3("0", "0", "0");    \
    StrVec3 scale        = StrVec3("1", "1", "1");    \
    StrVec3 shear        = StrVec3("0", "0", "0");    \
)

// material
SOPV_DEFINE_NODE(Color,                          \
    sm::vec3 color = sm::vec3(1.0f, 1.0f, 1.0f); \
)

// NURBs
SOPV_DEFINE_NODE(Carve,         \
    std::string first_u  = "0"; \
    std::string second_u = "1"; \
    std::string first_v  = "0"; \
    std::string second_v = "1"; \
)

// polygon
SOPV_DEFINE_NODE(Add,    \
    bool use_p0 = false; \
    sm::vec3 p0;         \
    bool use_p1 = false; \
    sm::vec3 p1;         \
    bool use_p2 = false; \
    sm::vec3 p2;         \
    bool use_p3 = false; \
    sm::vec3 p3;         \
)
SOPV_DEFINE_NODE(Boolean,                            \
    BooleanOperator op = BooleanOperator::Intersect; \
)
SOPV_DEFINE_NODE(Fuse,       \
    float distance = 0.001f; \
)
SOPV_DEFINE_NODE(Knife,                     \
    sm::vec3 origin;                        \
    sm::vec3 direction = sm::vec3(0, 1, 0); \
    KnifeKeep keep = KnifeKeep::KeepAll;    \
)
SOPV_DEFINE_NODE(Normal,                                  \
    GeoAttrClass attr_add_norm_to = GeoAttrClass::Vertex; \
)
SOPV_DEFINE_NODE(PolyExtrude,  \
    GroupName group_name;      \
    float distance = 0;        \
    bool output_front = true;  \
    bool output_back  = false; \
    bool output_side  = true;  \
    std::string front_group;   \
    std::string back_group;    \
    std::string side_group;    \
)
SOPV_DEFINE_NODE(PolyFill, SOPV_NODE_PROP)
SOPV_DEFINE_NODE(PolyFrame,                                 \
    GroupType      entity_type = GroupType::Primitives;     \
    PolyFrameStyle frame_style = PolyFrameStyle::TwoEdges;  \
    std::string    normal_name;                             \
    std::string    tangent_name;                            \
    std::string    bitangent_name;                          \
)

// primitive
SOPV_DEFINE_NODE(Box,                        \
    StrVec3 size   = StrVec3("1", "1", "1"); \
    StrVec3 center = StrVec3("0", "0", "0"); \
    float scale    = 1.0f;                   \
)
SOPV_DEFINE_NODE(Curve,             \
    std::vector<sm::vec3> vertices; \
)
SOPV_DEFINE_NODE(Line,                         \
    sm::vec3    origin    = sm::vec3(0, 0, 0); \
    sm::vec3    direction = sm::vec3(0, 1, 0); \
    std::string length    = "1";               \
    size_t      points    = 2;                 \
)
SOPV_DEFINE_NODE(Primitive,                 \
    sm::vec3 translate = sm::vec3(0, 0, 0); \
    sm::vec3 rotate    = sm::vec3(0, 0, 0); \
    sm::vec3 scale     = sm::vec3(1, 1, 1); \
    sm::vec3 shear     = sm::vec3(0, 0, 0); \
)
SOPV_DEFINE_NODE(Sphere, SOPV_NODE_PROP)

// utility
SOPV_DEFINE_NODE(Blast,                               \
    GroupName group_name;                             \
    GroupType group_type = GroupType::GuessFromGroup; \
    bool delete_non_selected = false;                 \
)
SOPV_DEFINE_NODE(CopyToPoints,             \
    GroupName src_group    = GroupName(0); \
    GroupName target_group = GroupName(1); \
    bool use_pt_dir = false;               \
)
SOPV_DEFINE_NODE(ForeachPrimBegin, SOPV_NODE_PROP)
SOPV_DEFINE_NODE(ForeachPrimEnd,     \
    bool do_single_pass     = false; \
    int  single_pass_offset = 0;     \
)
SOPV_DEFINE_IMPORT_EXT_NODE(Merge, SOPV_NODE_PROP)
SOPV_DEFINE_PROPS_NODE(Null, SOPV_NODE_PROP)
SOPV_DEFINE_IMPORT_EXT_NODE(Switch, \
    std::string selected = "0";     \
)

}

}