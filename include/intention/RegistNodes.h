#pragma once

#include "intention/Node.h"
#include "intention/ReflectPropTypes.h"

#include <blueprint/Pin.h>

namespace itt
{

void nodes_regist_rttr();

namespace node
{

#define ITT_DEFINE_NODE(name, prop)          \
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

#define ITT_DEFINE_PROPS_NODE(name, prop)    \
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

#define ITT_DEFINE_IMPORT_EXT_NODE(name, prop)                       \
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

#define ITT_NODE_PROP

// base
ITT_DEFINE_PROPS_NODE(Geometry,        \
    std::vector<bp::NodePtr> children; \
)

// attribute
ITT_DEFINE_NODE(Sort,                \
    SortKey key = SortKey::NoChange; \
)

// group
ITT_DEFINE_NODE(GroupCreate,                          \
    std::string group_name;                           \
    GroupType   group_type = GroupType::Primitives;   \
    GroupMerge  merge_op   = GroupMerge::Replace;     \
    bool        base_group = false;                   \
    std::string base_group_expr;                      \
    bool        keep_by_normals  = false;             \
    sm::vec3    direction        = sm::vec3(0, 0, 1); \
    float       spread_angle     = 180;               \
)
ITT_DEFINE_NODE(GroupExpression,                    \
    GroupType   group_type = GroupType::Primitives; \
    GroupExprInst inst0;                            \
    GroupExprInst inst1;                            \
    GroupExprInst inst2;                            \
    GroupExprInst inst3;                            \
)

// manipulate
ITT_DEFINE_NODE(Delete,                                      \
    bool             delete_non_selected = false;            \
    DeleteEntityType entity_type = DeleteEntityType::Points; \
    std::string      filter_exp;                             \
)
ITT_DEFINE_NODE(Transform,                            \
    GroupName group_name;                             \
    GroupType group_type = GroupType::GuessFromGroup; \
    StrVec3 translate = StrVec3("0", "0", "0");       \
    StrVec3 rotate = StrVec3("0", "0", "0");          \
    StrVec3 scale = StrVec3("1", "1", "1");           \
    StrVec3 shear = StrVec3("0", "0", "0");           \
)

// NURBs
ITT_DEFINE_NODE(Carve,  \
    float first_u = 0;  \
    float second_u = 1; \
    float first_v = 0;  \
    float second_v = 1; \
)

// polygon
ITT_DEFINE_NODE(Add,              \
    std::vector<sm::vec3> points; \
)
ITT_DEFINE_NODE(Boolean,                             \
    BooleanOperator op = BooleanOperator::Intersect; \
)
ITT_DEFINE_NODE(Knife,                      \
    sm::vec3 origin;                        \
    sm::vec3 direction = sm::vec3(0, 1, 0); \
    KnifeKeep keep = KnifeKeep::KeepAll;    \
)
ITT_DEFINE_NODE(PolyExtrude, \
    GroupName group_name;    \
    float distance = 0;      \
)
ITT_DEFINE_NODE(PolyFill, ITT_NODE_PROP)

// primitive
ITT_DEFINE_NODE(Box,                         \
    StrVec3 size = StrVec3("1", "1", "1");   \
    StrVec3 center = StrVec3("0", "0", "0"); \
    float scale = 1.0f;                      \
)
ITT_DEFINE_NODE(Curve,              \
    std::vector<sm::vec3> vertices; \
)
ITT_DEFINE_NODE(Line,                       \
    sm::vec3 origin = sm::vec3(0, 0, 0);    \
    sm::vec3 direction = sm::vec3(0, 0, 1); \
    float    length = 1;                    \
    size_t   points = 2;                    \
)
ITT_DEFINE_NODE(Sphere, ITT_NODE_PROP)

// utility
ITT_DEFINE_NODE(Blast,                            \
    GroupName group_name;                         \
    GroupType group_type = GroupType::Primitives; \
    bool delete_non_selected = false;             \
)
ITT_DEFINE_NODE(CopyToPoints, ITT_NODE_PROP)
ITT_DEFINE_NODE(ForeachPrimBegin, ITT_NODE_PROP)
ITT_DEFINE_NODE(ForeachPrimEnd, ITT_NODE_PROP)
ITT_DEFINE_IMPORT_EXT_NODE(Merge, ITT_NODE_PROP)
ITT_DEFINE_PROPS_NODE(Null, ITT_NODE_PROP)
ITT_DEFINE_IMPORT_EXT_NODE(Switch, \
    std::string selected = "0";    \
)

}

}