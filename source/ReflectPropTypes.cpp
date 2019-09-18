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

rttr::registration::enumeration<itt::GroupType>("itt_group_type")
(
    rttr::value("guess_from_group", itt::GroupType::GuessFromGroup),
	rttr::value("primitives",       itt::GroupType::Primitives),
    rttr::value("points",           itt::GroupType::Points),
    rttr::value("edges",            itt::GroupType::Edges),
    rttr::value("vertices",         itt::GroupType::Vertices)
);

rttr::registration::enumeration<itt::GroupMerge>("itt_group_merge")
(
	rttr::value("replace",   itt::GroupMerge::Replace),
    rttr::value("union",     itt::GroupMerge::Union),
    rttr::value("intersect", itt::GroupMerge::Intersect),
    rttr::value("subtract",  itt::GroupMerge::Subtract)
);

rttr::registration::class_<itt::GroupExprInst>("itt_group_expr_inst")
    .property("group_name", &itt::GroupExprInst::group_name)
    .property("expr_str",   &itt::GroupExprInst::expr_str)
    .property("merge_op",   &itt::GroupExprInst::merge_op)
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

rttr::registration::enumeration<itt::GeoAttrType>("itt_geo_attr_type")
(
	rttr::value("point",     itt::GeoAttrType::Point),
    rttr::value("vertex",    itt::GeoAttrType::Vertex),
    rttr::value("primitive", itt::GeoAttrType::Primitive),
    rttr::value("detail",    itt::GeoAttrType::Detail)
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

rttr::registration::enumeration<itt::MeasureType>("itt_measure_type")
(
	rttr::value("perimeter", itt::MeasureType::Perimeter),
	rttr::value("area",      itt::MeasureType::Area)
);

}

namespace itt
{

void prop_types_regist_rttr()
{
}

}