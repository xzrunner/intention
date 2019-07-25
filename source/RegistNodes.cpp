#include "intention/RegistNodes.h"

#include <ee0/ReflectPropTypes.h>

#define REGIST_NODE_RTTI(name, prop)                          \
	rttr::registration::class_<itt::node::name>("itt::"#name) \
		.constructor<>()                                      \
		prop                                                  \
	;

#define PROP
#define REGIST_NODE_RTTI_DEFAULT(name) REGIST_NODE_RTTI(name, PROP)

RTTR_REGISTRATION
{

// base
rttr::registration::class_<itt::Node>("itt::node")
.property("display", &itt::Node::GetDisplay, &itt::Node::SetDisplay)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Display"))
)
;

// manipulate
REGIST_NODE_RTTI(Transform,
.property("translate", &itt::node::Transform::translate)               \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Translate")) \
)                                                                      \
.property("rotate", &itt::node::Transform::rotate)                     \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Rotate"))    \
)                                                                      \
.property("scale", &itt::node::Transform::scale)                       \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Scale"))     \
)                                                                      \
.property("shear", &itt::node::Transform::shear)                       \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Shear"))     \
)                                                                      \
)

// primitive
REGIST_NODE_RTTI(Box,
.property("size", &itt::node::Box::size)                            \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Size"))   \
)                                                                   \
.property("center", &itt::node::Box::center)                        \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Center")) \
)                                                                   \
.property("scale", &itt::node::Box::scale)                          \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Scale"))  \
)
)
REGIST_NODE_RTTI_DEFAULT(Sphere)
REGIST_NODE_RTTI_DEFAULT(Curve)

}

namespace itt
{

void nodes_regist_rttr()
{
}

}