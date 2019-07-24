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

#define ITT_NODE_PROP

// primitive
ITT_DEFINE_NODE(Box,                     \
    sm::vec3 size   = sm::vec3(1, 1, 1); \
    sm::vec3 center = sm::vec3(0, 0, 0); \
    float scale     = 1.0f;              \
)
ITT_DEFINE_NODE(Sphere, ITT_NODE_PROP)
ITT_DEFINE_NODE(Curve,  ITT_NODE_PROP)

}

}