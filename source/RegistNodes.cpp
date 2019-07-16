#include "intention/RegistNodes.h"

#define REGIST_NODE_RTTI(name, prop)                          \
	rttr::registration::class_<itt::node::name>("itt::"#name) \
		.constructor<>()                                      \
		prop                                                  \
	;

#define PROP
#define REGIST_NODE_RTTI_DEFAULT(name) REGIST_NODE_RTTI(name, PROP)

RTTR_REGISTRATION
{

// primitive
REGIST_NODE_RTTI_DEFAULT(Box)
REGIST_NODE_RTTI_DEFAULT(Sphere)
REGIST_NODE_RTTI_DEFAULT(Curve)

}

namespace itt
{

void nodes_regist_rttr()
{
}

}