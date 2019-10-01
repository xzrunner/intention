#include "sopview/ReflectPropTypes.h"

#include <rttr/registration.h>

RTTR_REGISTRATION
{

rttr::registration::class_<sopv::StrVec3>("itt_str_vec3")
    .property("x", &sopv::StrVec3::x)
    .property("y", &sopv::StrVec3::y)
    .property("z", &sopv::StrVec3::z)
;

rttr::registration::class_<sopv::GroupName>("itt_group_name")
    .property("str", &sopv::GroupName::str)
;

rttr::registration::enumeration<sopv::GroupType>("itt_group_type")
(
    rttr::value("guess_from_group", sopv::GroupType::GuessFromGroup),
	rttr::value("primitives",       sopv::GroupType::Primitives),
    rttr::value("points",           sopv::GroupType::Points),
    rttr::value("edges",            sopv::GroupType::Edges),
    rttr::value("vertices",         sopv::GroupType::Vertices)
);

rttr::registration::enumeration<sopv::GroupMerge>("itt_group_merge")
(
	rttr::value("replace",   sopv::GroupMerge::Replace),
    rttr::value("union",     sopv::GroupMerge::Union),
    rttr::value("intersect", sopv::GroupMerge::Intersect),
    rttr::value("subtract",  sopv::GroupMerge::Subtract)
);

rttr::registration::class_<sopv::GroupExprInst>("itt_group_expr_inst")
    .property("group_name", &sopv::GroupExprInst::group_name)
    .property("expr_str",   &sopv::GroupExprInst::expr_str)
    .property("merge_op",   &sopv::GroupExprInst::merge_op)
;

rttr::registration::enumeration<sopv::BooleanOperator>("itt_boolean_operator")
(
	rttr::value("union",     sopv::BooleanOperator::Union),
	rttr::value("intersect", sopv::BooleanOperator::Intersect),
    rttr::value("subtract",  sopv::BooleanOperator::Subtract)
);

rttr::registration::enumeration<sopv::KnifeKeep>("itt_knife_keep")
(
	rttr::value("above", sopv::KnifeKeep::KeepAbove),
	rttr::value("below", sopv::KnifeKeep::KeepBelow),
    rttr::value("all",   sopv::KnifeKeep::KeepAll)
);

rttr::registration::enumeration<sopv::GeoAttrType>("itt_geo_attr_type")
(
	rttr::value("point",     sopv::GeoAttrType::Point),
    rttr::value("vertex",    sopv::GeoAttrType::Vertex),
    rttr::value("primitive", sopv::GeoAttrType::Primitive),
    rttr::value("detail",    sopv::GeoAttrType::Detail)
);

rttr::registration::enumeration<sopv::SortKey>("itt_sort_key")
(
	rttr::value("no_change", sopv::SortKey::NoChange),
	rttr::value("x", sopv::SortKey::X),
    rttr::value("y", sopv::SortKey::Y),
    rttr::value("z", sopv::SortKey::Z)
);

rttr::registration::enumeration<sopv::DeleteEntityType>("itt_delete_entity_type")
(
	rttr::value("points", sopv::DeleteEntityType::Points),
	rttr::value("edges",  sopv::DeleteEntityType::Edges),
    rttr::value("faces",  sopv::DeleteEntityType::Faces)
);

rttr::registration::enumeration<sopv::MeasureType>("itt_measure_type")
(
	rttr::value("perimeter", sopv::MeasureType::Perimeter),
	rttr::value("area",      sopv::MeasureType::Area)
);

}

namespace sopv
{

void prop_types_regist_rttr()
{
}

}