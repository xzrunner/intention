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
ITT_DEFINE_NODE(Box,    ITT_NODE_PROP)
ITT_DEFINE_NODE(Sphere, ITT_NODE_PROP)
ITT_DEFINE_NODE(Curve,  ITT_NODE_PROP)

}

}