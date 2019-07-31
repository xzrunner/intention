#include "intention/ReflectPropTypes.h"

#include <rttr/registration.h>

RTTR_REGISTRATION
{

rttr::registration::class_<itt::GroupName>("itt_group_name")
    .property("str", &itt::GroupName::str)
;

}

namespace itt
{

void prop_types_regist_rttr()
{
}

}