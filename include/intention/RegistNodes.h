#pragma once

#include "intention/Node.h"
#include "intention/ReflectPropTypes.h"

#include <blueprint/Pin.h>

#include <polymesh3/Geometry.h>

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

#define ITT_NODE_PROP

// manipulate
ITT_DEFINE_NODE(Transform,                  \
    sm::vec3 translate = sm::vec3(0, 0, 0); \
    sm::vec3 rotate    = sm::vec3(0, 0, 0); \
    sm::vec3 scale     = sm::vec3(1, 1, 1); \
    sm::vec3 shear     = sm::vec3(0, 0, 0); \
)

// polygon
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
ITT_DEFINE_NODE(Sphere, ITT_NODE_PROP)
ITT_DEFINE_NODE(Curve, ITT_NODE_PROP)

// utility
ITT_DEFINE_NODE(Blast,                                \
    GroupName group_name;                             \
    pm3::GroupType group_type = pm3::GroupType::Face; \
    bool delete_non_selected = false;                 \
)
ITT_DEFINE_NODE(CopyToPoints, ITT_NODE_PROP)
ITT_DEFINE_NODE(GroupCreate,                       \
    std::string name;                              \
    pm3::GroupType type = pm3::GroupType::Face;    \
    bool     keep_by_normals  = false;             \
    sm::vec3 direction        = sm::vec3(0, 0, 1); \
    float    spread_angle     = 180;               \
)
ITT_DEFINE_NODE(Merge, ITT_NODE_PROP)

}

}