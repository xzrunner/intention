#include "intention/ReflectPropTypes.h"

#include <rttr/registration.h>

RTTR_REGISTRATION
{

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

}

namespace itt
{

void prop_types_regist_rttr()
{
}

}