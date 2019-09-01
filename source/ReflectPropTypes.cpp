#include "intention/ReflectPropTypes.h"

#include <rttr/registration.h>

RTTR_REGISTRATION
{

rttr::registration::class_<itt::StrVec3>("itt_str_vec3")
    .property("x", &itt::StrVec3::x)
    .property("y", &itt::StrVec3::y)
    .property("z", &itt::StrVec3::z)
;

rttr::registration::class_<itt::GroupName>("itt_group_name")
    .property("str", &itt::GroupName::str)
;

rttr::registration::enumeration<itt::BooleanOperator>("itt_boolean_operator")
(
	rttr::value("union",     itt::BooleanOperator::Union),
	rttr::value("intersect", itt::BooleanOperator::Intersect),
    rttr::value("subtract",  itt::BooleanOperator::Subtract)
);

rttr::registration::enumeration<itt::KnifeKeep>("itt_knife_keep")
(
	rttr::value("above", itt::KnifeKeep::KeepAbove),
	rttr::value("below", itt::KnifeKeep::KeepBelow),
    rttr::value("all",   itt::KnifeKeep::KeepAll)
);

rttr::registration::enumeration<itt::SortKey>("itt_sort_key")
(
	rttr::value("no_change", itt::SortKey::NoChange),
	rttr::value("x", itt::SortKey::X),
    rttr::value("y", itt::SortKey::Y),
    rttr::value("z", itt::SortKey::Z)
);

rttr::registration::enumeration<itt::DeleteEntityType>("itt_delete_entity_type")
(
	rttr::value("points", itt::DeleteEntityType::Points),
	rttr::value("edges",  itt::DeleteEntityType::Edges),
    rttr::value("faces",  itt::DeleteEntityType::Faces)
);

}

namespace itt
{

void prop_types_regist_rttr()
{
}

}