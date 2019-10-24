#include "sopview/ReflectPropTypes.h"

#include <rttr/registration.h>

RTTR_REGISTRATION
{

rttr::registration::class_<sopv::StrVec3>("sopv_str_vec3")
    .property("x", &sopv::StrVec3::x)
    .property("y", &sopv::StrVec3::y)
    .property("z", &sopv::StrVec3::z)
;

rttr::registration::class_<sopv::GroupName>("sopv_group_name")
    .property("str", &sopv::GroupName::str)
;

rttr::registration::enumeration<sopv::GroupType>("sopv_group_type")
(
    rttr::value("guess_from_group", sopv::GroupType::GuessFromGroup),
	rttr::value("primitives",       sopv::GroupType::Primitives),
    rttr::value("points",           sopv::GroupType::Points),
    rttr::value("edges",            sopv::GroupType::Edges),
    rttr::value("vertices",         sopv::GroupType::Vertices)
);

rttr::registration::enumeration<sopv::GroupMerge>("sopv_group_merge")
(
	rttr::value("replace",   sopv::GroupMerge::Replace),
    rttr::value("union",     sopv::GroupMerge::Union),
    rttr::value("intersect", sopv::GroupMerge::Intersect),
    rttr::value("subtract",  sopv::GroupMerge::Subtract)
);

rttr::registration::class_<sopv::GroupExprInst>("sopv_group_expr_inst")
    .property("group_name", &sopv::GroupExprInst::group_name)
    .property("expr_str",   &sopv::GroupExprInst::expr_str)
    .property("merge_op",   &sopv::GroupExprInst::merge_op)
;

rttr::registration::enumeration<sopv::BooleanOperator>("sopv_boolean_operator")
(
	rttr::value("union",     sopv::BooleanOperator::Union),
	rttr::value("intersect", sopv::BooleanOperator::Intersect),
    rttr::value("subtract",  sopv::BooleanOperator::Subtract)
);

rttr::registration::enumeration<sopv::KnifeKeep>("sopv_knife_keep")
(
	rttr::value("above", sopv::KnifeKeep::KeepAbove),
	rttr::value("below", sopv::KnifeKeep::KeepBelow),
    rttr::value("all",   sopv::KnifeKeep::KeepAll)
);

rttr::registration::enumeration<sopv::GeoAttrType>("sopv_geo_attr_type")
(
	rttr::value("int",     sopv::GeoAttrType::Int),
    rttr::value("bool",    sopv::GeoAttrType::Bool),
    rttr::value("double",  sopv::GeoAttrType::Double),
    rttr::value("float",   sopv::GeoAttrType::Float),
    rttr::value("float2",  sopv::GeoAttrType::Float2),
    rttr::value("float3",  sopv::GeoAttrType::Float3),
    rttr::value("float4",  sopv::GeoAttrType::Float4),
    rttr::value("string",  sopv::GeoAttrType::String),
    rttr::value("vector",  sopv::GeoAttrType::Vector),
    rttr::value("vector4", sopv::GeoAttrType::Vector4),
    rttr::value("matrix2", sopv::GeoAttrType::Matrix2),
    rttr::value("matrix3", sopv::GeoAttrType::Matrix3),
    rttr::value("matrix4", sopv::GeoAttrType::Matrix4)
);

rttr::registration::class_<sopv::AttrCreateItem>("sopv_attr_create_item")
    .property("name",        &sopv::AttrCreateItem::name)
    .property("cls",         &sopv::AttrCreateItem::cls)
    .property("type",        &sopv::AttrCreateItem::type)
    .property("value",       &sopv::AttrCreateItem::value)
    .property("default_val", &sopv::AttrCreateItem::default_val)
;

rttr::registration::enumeration<sopv::GeoAttrClass>("sopv_geo_attr_cls")
(
	rttr::value("point",     sopv::GeoAttrClass::Point),
    rttr::value("vertex",    sopv::GeoAttrClass::Vertex),
    rttr::value("primitive", sopv::GeoAttrClass::Primitive),
    rttr::value("detail",    sopv::GeoAttrClass::Detail)
);

rttr::registration::class_<sopv::AttributeName>("sopv_attr_name")
    .property("cls", &sopv::AttributeName::cls)
    .property("str", &sopv::AttributeName::str)
;

rttr::registration::enumeration<sopv::SortKey>("sopv_sort_key")
(
	rttr::value("no_change", sopv::SortKey::NoChange),
	rttr::value("x",         sopv::SortKey::X),
    rttr::value("y",         sopv::SortKey::Y),
    rttr::value("z",         sopv::SortKey::Z),
    rttr::value("shift",     sopv::SortKey::Shift)
);

rttr::registration::enumeration<sopv::DeleteEntityType>("sopv_delete_entity_type")
(
	rttr::value("points", sopv::DeleteEntityType::Points),
	rttr::value("edges",  sopv::DeleteEntityType::Edges),
    rttr::value("faces",  sopv::DeleteEntityType::Faces)
);

rttr::registration::enumeration<sopv::MeasureType>("sopv_measure_type")
(
	rttr::value("perimeter", sopv::MeasureType::Perimeter),
	rttr::value("area",      sopv::MeasureType::Area)
);

rttr::registration::enumeration<sopv::PolyFrameStyle>("sopv_polyframe_style")
(
	rttr::value("first_edge",          sopv::PolyFrameStyle::FirstEdge),
    rttr::value("two_edges",           sopv::PolyFrameStyle::TwoEdges),
    rttr::value("primitive_centroid",  sopv::PolyFrameStyle::PrimitiveCentroid),
    rttr::value("texture_uv",          sopv::PolyFrameStyle::TextureUV),
    rttr::value("texture_uv_gradient", sopv::PolyFrameStyle::TextureUVGradient),
    rttr::value("attribute_gradient",  sopv::PolyFrameStyle::AttributeGradient),
    rttr::value("mikkt",               sopv::PolyFrameStyle::MikkT)
);

rttr::registration::enumeration<sopv::FuseOperator>("sopv_fuse_op")
(
	rttr::value("consolidate",   sopv::FuseOperator::Consolidate),
	rttr::value("unique_points", sopv::FuseOperator::UniquePoints),
    rttr::value("snap",          sopv::FuseOperator::Snap)
);

}

namespace sopv
{

void prop_types_regist_rttr()
{
}

}