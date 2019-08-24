#pragma once

#include "intention/Node.h"
#include "intention/ReflectPropTypes.h"

#include <blueprint/Pin.h>

#include <everything/Group.h>

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

// attribute
ITT_DEFINE_NODE(Sort,                \
    SortKey key = SortKey::NoChange; \
)

// manipulate
ITT_DEFINE_NODE(Delete,                                      \
    bool             delete_non_selected = false;            \
    DeleteEntityType entity_type = DeleteEntityType::Points; \
    std::string      filter_exp;                             \
)
ITT_DEFINE_NODE(Transform,                  \
    sm::vec3 translate = sm::vec3(0, 0, 0); \
    sm::vec3 rotate    = sm::vec3(0, 0, 0); \
    sm::vec3 scale     = sm::vec3(1, 1, 1); \
    sm::vec3 shear     = sm::vec3(0, 0, 0); \
)

// NURBs
ITT_DEFINE_NODE(Carve,  \
    float first_u  = 0; \
    float second_u = 1; \
    float first_v  = 0; \
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

// primitive
ITT_DEFINE_NODE(Box,                     \
    sm::vec3 size   = sm::vec3(1, 1, 1); \
    sm::vec3 center = sm::vec3(0, 0, 0); \
    float scale     = 1.0f;              \
)
ITT_DEFINE_NODE(Curve,              \
    std::vector<sm::vec3> vertices; \
)
ITT_DEFINE_NODE(Line,                       \
    sm::vec3 origin    = sm::vec3(0, 0, 0); \
    sm::vec3 direction = sm::vec3(0, 0, 1); \
    float    length = 1;                    \
    size_t   points = 2;                    \
)
ITT_DEFINE_NODE(Sphere, ITT_NODE_PROP)

// utility
ITT_DEFINE_NODE(Blast,                                      \
    GroupName group_name;                                   \
    evt::GroupType group_type = evt::GroupType::Primitives; \
    bool delete_non_selected = false;                       \
)
ITT_DEFINE_NODE(CopyToPoints, ITT_NODE_PROP)
ITT_DEFINE_NODE(ForeachPrimBegin, ITT_NODE_PROP)
ITT_DEFINE_NODE(ForeachPrimEnd, ITT_NODE_PROP)
ITT_DEFINE_NODE(GroupCreate,                          \
    std::string name;                                 \
    evt::GroupType type = evt::GroupType::Primitives; \
    bool     keep_by_normals  = false;                \
    sm::vec3 direction        = sm::vec3(0, 0, 1);    \
    float    spread_angle     = 180;                  \
)
ITT_DEFINE_IMPORT_EXT_NODE(Merge, ITT_NODE_PROP)
ITT_DEFINE_IMPORT_EXT_NODE(Switch, \
    size_t selected = 0;           \
)

}

}