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

rttr::registration::enumeration<sopv::GroupTypes>("sopv_group_types")
(
	rttr::value("auto",     sopv::GroupTypes::Auto),
    rttr::value("prims",    sopv::GroupTypes::Primitives),
    rttr::value("points",   sopv::GroupTypes::Points),
    rttr::value("edges",    sopv::GroupTypes::Edges),
    rttr::value("vertices", sopv::GroupTypes::Vertices),
    rttr::metadata(sopv::GroupTypes::Auto,       "Auto"),
    rttr::metadata(sopv::GroupTypes::Primitives, "Primitives"),
    rttr::metadata(sopv::GroupTypes::Points,     "Points"),
    rttr::metadata(sopv::GroupTypes::Edges,      "Edges"),
    rttr::metadata(sopv::GroupTypes::Vertices,   "Vertices")
);

rttr::registration::class_<sopv::GroupExprInst>("sopv_group_expr_inst")
    .property("enable",    &sopv::GroupExprInst::enable)
    .property("groupname", &sopv::GroupExprInst::group_name)
    .property("snippet",   &sopv::GroupExprInst::expr_str)
    .property("mergeop",   &sopv::GroupExprInst::merge_op)
;

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

rttr::registration::enumeration<sopv::NormalGeoAttrClass>("sopv_norm_geo_attr_cls_type")
(
	rttr::value("typepoint",  sopv::NormalGeoAttrClass::Point),
    rttr::value("typevertex", sopv::NormalGeoAttrClass::Vertex),
    rttr::value("typeprim",   sopv::NormalGeoAttrClass::Primitive),
    rttr::value("typedetail", sopv::NormalGeoAttrClass::Detail),
    rttr::metadata(sopv::NormalGeoAttrClass::Point,     "Point"),
    rttr::metadata(sopv::NormalGeoAttrClass::Vertex,    "Vertex"),
    rttr::metadata(sopv::NormalGeoAttrClass::Primitive, "Primitive"),
    rttr::metadata(sopv::NormalGeoAttrClass::Detail,    "Detail")
);

rttr::registration::class_<sopv::AttributeName>("sopv_attr_name")
    .property("cls", &sopv::AttributeName::cls)
    .property("str", &sopv::AttributeName::str)
;

}

namespace sopv
{

void prop_types_regist_rttr()
{
}

}