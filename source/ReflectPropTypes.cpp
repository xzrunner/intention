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
    rttr::value("guess",     sopv::GroupType::GuessFromGroup),
	rttr::value("primitive", sopv::GroupType::Primitives),
    rttr::value("point",     sopv::GroupType::Points),
    rttr::value("edge",      sopv::GroupType::Edges),
    rttr::value("vertex",    sopv::GroupType::Vertices),
    rttr::metadata(sopv::GroupType::GuessFromGroup, "Guess From Group"),
    rttr::metadata(sopv::GroupType::Primitives,     "Primitives"),
    rttr::metadata(sopv::GroupType::Points,         "Points"),
    rttr::metadata(sopv::GroupType::Edges,          "Edges"),
    rttr::metadata(sopv::GroupType::Vertices,       "Vertices")
);

rttr::registration::enumeration<sopv::GroupTypes>("sopv_group_types")
(
	rttr::value("auto",     sopv::GroupTypes::Auto),
    rttr::value("prims",    sopv::GroupTypes::Primitives),
    rttr::value("points",   sopv::GroupTypes::Points),
    rttr::value("edges",    sopv::GroupTypes::Edges),
    rttr::value("vertices", sopv::GroupTypes::Vertices)
);

rttr::registration::enumeration<sopv::GroupBoundingType>("sopv_group_bounding_type")
(
	rttr::value("usebbox",    sopv::GroupBoundingType::Box),
    rttr::value("usebsphere", sopv::GroupBoundingType::Sphere),
    rttr::value("usebobject", sopv::GroupBoundingType::Object),
    rttr::value("usebvolume", sopv::GroupBoundingType::Volume)
);

rttr::registration::enumeration<sopv::GroupMerge>("sopv_group_merge")
(
	rttr::value("replace",   sopv::GroupMerge::Replace),
    rttr::value("union",     sopv::GroupMerge::Union),
    rttr::value("intersect", sopv::GroupMerge::Intersect),
    rttr::value("subtract",  sopv::GroupMerge::Subtract)
);

rttr::registration::class_<sopv::GroupExprInst>("sopv_group_expr_inst")
    .property("enable",    &sopv::GroupExprInst::enable)
    .property("groupname", &sopv::GroupExprInst::group_name)
    .property("snippet",   &sopv::GroupExprInst::expr_str)
    .property("mergeop",   &sopv::GroupExprInst::merge_op)
;

rttr::registration::enumeration<sopv::BooleanOperator>("sopv_boolean_operator")
(
	rttr::value("union",     sopv::BooleanOperator::Union),
	rttr::value("intersect", sopv::BooleanOperator::Intersect),
    rttr::value("subtract",  sopv::BooleanOperator::Subtract)
);

rttr::registration::enumeration<sopv::BooleanGeoType>("sopv_boolean_geo_type")
(
	rttr::value("solid",   sopv::BooleanGeoType::Solid),
	rttr::value("surface", sopv::BooleanGeoType::Surface)
);

rttr::registration::enumeration<sopv::BooleanSubType>("sopv_boolean_sub_type")
(
	rttr::value("aminusb", sopv::BooleanSubType::AMinusB),
	rttr::value("bminusa", sopv::BooleanSubType::BMinusA),
    rttr::value("both",    sopv::BooleanSubType::Both)
);

rttr::registration::enumeration<sopv::KnifeKeep>("sopv_knife_keep")
(
	rttr::value("above", sopv::KnifeKeep::KeepAbove),
	rttr::value("below", sopv::KnifeKeep::KeepBelow),
    rttr::value("both",  sopv::KnifeKeep::KeepAll)
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

rttr::registration::enumeration<sopv::AttrCreateType>("sopv_attr_create_type")
(
	rttr::value("float",       sopv::AttrCreateType::Float),
    rttr::value("int",         sopv::AttrCreateType::Integer),
    rttr::value("vector",      sopv::AttrCreateType::Vector),
    rttr::value("index",       sopv::AttrCreateType::String),
    rttr::value("floatarray",  sopv::AttrCreateType::FloatArray),
    rttr::value("intarray",    sopv::AttrCreateType::IntegerArray),
    rttr::value("stringarray", sopv::AttrCreateType::StringArray)
);

rttr::registration::enumeration<sopv::AttrCreateFloatInfo>("sopv_attr_float_info")
(
	rttr::value("guess",        sopv::AttrCreateFloatInfo::Guess),
    rttr::value("none",         sopv::AttrCreateFloatInfo::None),
    rttr::value("point",        sopv::AttrCreateFloatInfo::Position),
    rttr::value("vector",       sopv::AttrCreateFloatInfo::Vector),
    rttr::value("normal",       sopv::AttrCreateFloatInfo::Normal),
    rttr::value("color",        sopv::AttrCreateFloatInfo::Color),
    rttr::value("quaternion",   sopv::AttrCreateFloatInfo::Quaternion),
    rttr::value("tranform",     sopv::AttrCreateFloatInfo::TransformMatrix),
    rttr::value("texturecoord", sopv::AttrCreateFloatInfo::TextureCoordinate)
);

rttr::registration::class_<sopv::AttrCreateItem>("sopv_attr_create_item")
    .property("name",     &sopv::AttrCreateItem::name)
    .property("class",    &sopv::AttrCreateItem::cls)
    .property("type",     &sopv::AttrCreateItem::type)
    .property("size",     &sopv::AttrCreateItem::size)
    .property("value",    &sopv::AttrCreateItem::value)
    .property("default",  &sopv::AttrCreateItem::default_val)
    .property("string",   &sopv::AttrCreateItem::string)
    .property("typeinfo", &sopv::AttrCreateItem::flt_info)
;

rttr::registration::enumeration<sopv::GeoAttrClass>("sopv_geo_attr_cls")
(
	rttr::value("point",     sopv::GeoAttrClass::Point),
    rttr::value("vertex",    sopv::GeoAttrClass::Vertex),
    rttr::value("primitive", sopv::GeoAttrClass::Primitive),
    rttr::value("detail",    sopv::GeoAttrClass::Detail)
);

rttr::registration::enumeration<sopv::GeoAttrClassType>("sopv_geo_attr_cls_type")
(
	rttr::value("typepoint",  sopv::GeoAttrClassType::Point),
    rttr::value("typevertex", sopv::GeoAttrClassType::Vertex),
    rttr::value("typeprim",   sopv::GeoAttrClassType::Primitive),
    rttr::value("typedetail", sopv::GeoAttrClassType::Detail)
);

rttr::registration::class_<sopv::AttributeName>("sopv_attr_name")
    .property("cls", &sopv::AttributeName::cls)
    .property("str", &sopv::AttributeName::str)
;

rttr::registration::enumeration<sopv::SortKey>("sopv_sort_key")
(
	rttr::value("none", sopv::SortKey::NoChange),
	rttr::value("byx",  sopv::SortKey::X),
    rttr::value("byy",  sopv::SortKey::Y),
    rttr::value("byz",  sopv::SortKey::Z),
    rttr::value("shif", sopv::SortKey::Shift)
);

rttr::registration::enumeration<sopv::DeleteEntityType>("sopv_delete_entity_type")
(
	rttr::value("point", sopv::DeleteEntityType::Points),
	rttr::value("edge",  sopv::DeleteEntityType::Edges),
    rttr::value("face",  sopv::DeleteEntityType::Faces)
);

rttr::registration::enumeration<sopv::DeleteOperation>("sopv_delete_operation")
(
	rttr::value("pattern", sopv::DeleteOperation::Pattern),
	rttr::value("range",   sopv::DeleteOperation::Range),
    rttr::value("filter",  sopv::DeleteOperation::Expression)
);

rttr::registration::enumeration<sopv::MeasureType>("sopv_measure_type")
(
	rttr::value("perimeter", sopv::MeasureType::Perimeter),
	rttr::value("area",      sopv::MeasureType::Area),
    rttr::value("curvature", sopv::MeasureType::Curvature),
    rttr::value("volume",    sopv::MeasureType::Volume)
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

rttr::registration::enumeration<sopv::PolyExtrudeExtrusion>("sopv_polyextrude_extrusion")
(
	rttr::value("primnormal",  sopv::PolyExtrudeExtrusion::PrimEdgeNorm),
    rttr::value("pointnormal", sopv::PolyExtrudeExtrusion::PointNorm)
);

rttr::registration::enumeration<sopv::PolyFillMode>("sopv_polyfill_mode")
(
	rttr::value("none",      sopv::PolyFillMode::SinglePolygon),
    rttr::value("tris",      sopv::PolyFillMode::Triangles),
    rttr::value("trifan",    sopv::PolyFillMode::TriangleFan),
    rttr::value("quadfan",   sopv::PolyFillMode::QuadrilateralFan),
    rttr::value("quads",     sopv::PolyFillMode::Quadrilaterals),
    rttr::value("gridquads", sopv::PolyFillMode::QuadrilateralGrid)
);

rttr::registration::enumeration<sopv::FuseOperator>("sopv_fuse_op")
(
	rttr::value("consolidate",   sopv::FuseOperator::Consolidate),
	rttr::value("unique_points", sopv::FuseOperator::UniquePoints),
    rttr::value("snap",          sopv::FuseOperator::Snap)
);

rttr::registration::enumeration<sopv::ForeachBeginMethod>("sopv_foreach_begin_method")
(
	rttr::value("feedback", sopv::ForeachBeginMethod::Feedback),
	rttr::value("piece",    sopv::ForeachBeginMethod::PieceOrPoint),
    rttr::value("metadata", sopv::ForeachBeginMethod::Metadata),
    rttr::value("input",    sopv::ForeachBeginMethod::Input)
);

rttr::registration::enumeration<sopv::ForeachIterMethod>("sopv_foreach_iter")
(
	rttr::value("auto",   sopv::ForeachIterMethod::AutoDetectFromInputs),
	rttr::value("pieces", sopv::ForeachIterMethod::ByPiecesOrPoints),
    rttr::value("count",  sopv::ForeachIterMethod::ByCount)
);

rttr::registration::enumeration<sopv::ForeachGatherMethod>("sopv_foreach_gather")
(
	rttr::value("feedback", sopv::ForeachGatherMethod::Feedback),
	rttr::value("merge",    sopv::ForeachGatherMethod::Merge)
);

rttr::registration::enumeration<sopv::ForeachPieceElements>("sopv_foreach_piece")
(
	rttr::value("primitive", sopv::ForeachPieceElements::Primitives),
	rttr::value("point",     sopv::ForeachPieceElements::Points)
);

rttr::registration::enumeration<sopv::UVScale>("sopv_uvscale")
(
	rttr::value("none",    sopv::UVScale::None),
    rttr::value("uniform", sopv::UVScale::Uniform),
    rttr::value("stretch", sopv::UVScale::Stretch)
);

}

namespace sopv
{

void prop_types_regist_rttr()
{
}

}