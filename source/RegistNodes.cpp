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
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo(itt::Node::STR_PROP_DISPLAY))
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

// polygon
REGIST_NODE_RTTI(PolyExtrude,
.property("group_name", &itt::node::PolyExtrude::group_name)           \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName")) \
)                                                                      \
.property("distance", &itt::node::PolyExtrude::distance)               \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Distance"))  \
)
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

// utility
REGIST_NODE_RTTI(Blast,
.property("group_name", &itt::node::Blast::group_name)                      \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName"))      \
)                                                                           \
.property("group_type", &itt::node::Blast::group_type)                      \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupType"))      \
)                                                                           \
.property("delete_non_selected", &itt::node::Blast::delete_non_selected)    \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("DelNonSelected")) \
)
)
REGIST_NODE_RTTI(GroupCreate,                                              \
.property("name", &itt::node::GroupCreate::name)                           \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))          \
)                                                                          \
.property("type", &itt::node::GroupCreate::type)                           \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Type"))          \
)                                                                          \
.property("keep_by_normals", &itt::node::GroupCreate::keep_by_normals)     \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("KeepByNormals")) \
)                                                                          \
.property("direction", &itt::node::GroupCreate::direction)                 \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Direction"))     \
)                                                                          \
.property("spread_angle", &itt::node::GroupCreate::spread_angle)           \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SpreadAngle"))   \
)                                                                          \
)
REGIST_NODE_RTTI_DEFAULT(Merge)

}

namespace itt
{

void nodes_regist_rttr()
{
}

}