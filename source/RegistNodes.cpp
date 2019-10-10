#include "sopview/RegistNodes.h"

#include <ee0/ReflectPropTypes.h>

#define REGIST_NODE_RTTI(name, prop)                            \
	rttr::registration::class_<sopv::node::name>("sopv::"#name) \
		.constructor<>()                                        \
		prop                                                    \
	;

#define PROP
#define REGIST_NODE_RTTI_DEFAULT(name) REGIST_NODE_RTTI(name, PROP)

RTTR_REGISTRATION
{

// base
rttr::registration::class_<sopv::Node>("sopv::node")
.property("name", &sopv::Node::GetName, &sopv::Node::SetName)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))
)
.property("display", &sopv::Node::GetDisplay, &sopv::Node::SetDisplay)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo(sopv::Node::STR_PROP_DISPLAY))
)
.property("template", &sopv::Node::GetTemplate, &sopv::Node::SetTemplate)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Template"))
)
;

REGIST_NODE_RTTI_DEFAULT(Geometry)

// attribute
REGIST_NODE_RTTI(AttributeCreate,
.property("item0", &sopv::node::AttributeCreate::item0)                \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Item0"))     \
)                                                                      \
.property("item1", &sopv::node::AttributeCreate::item1)                \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Item1"))     \
)                                                                      \
.property("item2", &sopv::node::AttributeCreate::item2)                \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Item2"))     \
)                                                                      \
.property("item3", &sopv::node::AttributeCreate::item3)                \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Item3"))     \
)                                                                      \
)
REGIST_NODE_RTTI(AttributeWrangle,
.property("vex_expr", &sopv::node::AttributeWrangle::vex_expr)        \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("VexExpr")), \
    rttr::metadata(ee0::PropLongStringTag(), true)                    \
)                                                                     \
)
REGIST_NODE_RTTI(Measure,
.property("ms_type", &sopv::node::Measure::ms_type)                      \
(                                                                        \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("MeasureType")) \
)                                                                        \
.property("ms_name", &sopv::node::Measure::ms_name)                      \
(                                                                        \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("MeasureName")) \
)                                                                        \
)
REGIST_NODE_RTTI(Sort,
.property("key", &sopv::node::Sort::key)                         \
(                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Key")) \
)                                                                \
)

// group
REGIST_NODE_RTTI(GroupCreate,                                              \
.property("group_name", &sopv::node::GroupCreate::group_name)              \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName"))     \
)                                                                          \
.property("group_type", &sopv::node::GroupCreate::group_type)              \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupType"))     \
)                                                                          \
.property("merge_op", &sopv::node::GroupCreate::merge_op)                  \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("MergeOp"))       \
)                                                                          \
.property("base_group", &sopv::node::GroupCreate::base_group)              \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("BaseGroup"))     \
)                                                                          \
.property("base_group_expr", &sopv::node::GroupCreate::base_group_expr)    \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("BaseGroupExpr")) \
)                                                                          \
.property("keep_by_normals", &sopv::node::GroupCreate::keep_by_normals)    \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("KeepByNormals")) \
)                                                                          \
.property("direction", &sopv::node::GroupCreate::direction)                \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Direction"))     \
)                                                                          \
.property("spread_angle", &sopv::node::GroupCreate::spread_angle)          \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SpreadAngle"))   \
)                                                                          \
)
REGIST_NODE_RTTI(GroupExpression,                                      \
.property("group_type", &sopv::node::GroupExpression::group_type)      \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupType")) \
)                                                                      \
.property("inst0", &sopv::node::GroupExpression::inst0)                \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Inst0"))     \
)                                                                      \
.property("inst1", &sopv::node::GroupExpression::inst1)                \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Inst1"))     \
)                                                                      \
.property("inst2", &sopv::node::GroupExpression::inst2)                \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Inst2"))     \
)                                                                      \
.property("inst3", &sopv::node::GroupExpression::inst3)                \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Inst3"))     \
)                                                                      \
)

// manipulate
REGIST_NODE_RTTI(Delete,
.property("delete_non_selected", &sopv::node::Delete::delete_non_selected)  \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("DelNonSelected")) \
)                                                                           \
.property("entity_type", &sopv::node::Delete::entity_type)                  \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("EntityType"))     \
)                                                                           \
.property("filter_exp", &sopv::node::Delete::filter_exp)                    \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("FilterExp"))      \
)                                                                           \
)
REGIST_NODE_RTTI(Transform,
.property("group_name", &sopv::node::Transform::group_name)            \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName")) \
)                                                                      \
.property("group_type", &sopv::node::Transform::group_type)            \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupType")) \
)                                                                      \
.property("translate", &sopv::node::Transform::translate)              \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Translate")) \
)                                                                      \
.property("rotate", &sopv::node::Transform::rotate)                    \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Rotate"))    \
)                                                                      \
.property("scale", &sopv::node::Transform::scale)                      \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Scale"))     \
)                                                                      \
.property("shear", &sopv::node::Transform::shear)                      \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Shear"))     \
)                                                                      \
)

// material
REGIST_NODE_RTTI(Color,
.property("color", &sopv::node::Color::color)                      \
(                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Color")) \
)                                                                  \
)

// NURBs
REGIST_NODE_RTTI(Carve,
.property("first_u", &sopv::node::Carve::first_u)                    \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("FirstU"))  \
)                                                                    \
.property("second_u", &sopv::node::Carve::second_u)                  \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SecondU")) \
)                                                                    \
.property("first_v", &sopv::node::Carve::first_v)                    \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("FirstV"))  \
)                                                                    \
.property("second_v", &sopv::node::Carve::second_v)                  \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SecondV")) \
)                                                                    \
)

// polygon
REGIST_NODE_RTTI(Add,
.property("points", &sopv::node::Add::points)                       \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Points")) \
)                                                                   \
)
REGIST_NODE_RTTI(Boolean,
.property("operator", &sopv::node::Boolean::op)                       \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Operator")) \
)                                                                     \
)
REGIST_NODE_RTTI(Fuse,
.property("distance", &sopv::node::Fuse::distance)                    \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Distance")) \
)                                                                     \
)
REGIST_NODE_RTTI(Knife,
.property("origin", &sopv::node::Knife::origin)                        \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Origin"))    \
)                                                                      \
.property("direction", &sopv::node::Knife::direction)                  \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Direction")) \
)                                                                      \
.property("keep", &sopv::node::Knife::keep)                            \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Keep"))      \
)
)
REGIST_NODE_RTTI(Normal,
.property("attr_add_norm_to", &sopv::node::Normal::attr_add_norm_to)   \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("AddNormTo")) \
)                                                                      \
)
REGIST_NODE_RTTI(PolyExtrude,
.property("group_name", &sopv::node::PolyExtrude::group_name)          \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName")) \
)                                                                      \
.property("distance", &sopv::node::PolyExtrude::distance)              \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Distance"))  \
)
)
REGIST_NODE_RTTI_DEFAULT(PolyFill)

// primitive
REGIST_NODE_RTTI(Box,
.property("size", &sopv::node::Box::size)                           \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Size"))   \
)                                                                   \
.property("center", &sopv::node::Box::center)                       \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Center")) \
)                                                                   \
.property("scale", &sopv::node::Box::scale)                         \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Scale"))  \
)
)
REGIST_NODE_RTTI(Curve,
.property("vertices", &sopv::node::Curve::vertices)                   \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Vertices")) \
)                                                                     \
)
REGIST_NODE_RTTI(Line,
.property("origin", &sopv::node::Line::origin)                         \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Origin"))    \
)                                                                      \
.property("direction", &sopv::node::Line::direction)                   \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Direction")) \
)                                                                      \
.property("length", &sopv::node::Line::length)                         \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Length"))    \
)                                                                      \
.property("points", &sopv::node::Line::points)                         \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Points"))    \
)                                                                      \
)
REGIST_NODE_RTTI(Primitive,
.property("translate", &sopv::node::Primitive::translate)              \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Translate")) \
)                                                                      \
.property("rotate", &sopv::node::Primitive::rotate)                    \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Rotate"))    \
)                                                                      \
.property("scale", &sopv::node::Primitive::scale)                      \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Scale"))     \
)                                                                      \
.property("shear", &sopv::node::Primitive::shear)                      \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Shear"))     \
)                                                                      \
)
REGIST_NODE_RTTI_DEFAULT(Sphere)

// utility
REGIST_NODE_RTTI(Blast,
.property("group_name", &sopv::node::Blast::group_name)                     \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName"))      \
)                                                                           \
.property("group_type", &sopv::node::Blast::group_type)                     \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupType"))      \
)                                                                           \
.property("delete_non_selected", &sopv::node::Blast::delete_non_selected)   \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("DelNonSelected")) \
)
)
REGIST_NODE_RTTI(CopyToPoints,
.property("src_group", &sopv::node::CopyToPoints::src_group)             \
(                                                                        \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SrcGroup"))    \
)                                                                        \
.property("target_group", &sopv::node::CopyToPoints::target_group)       \
(                                                                        \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("TargetGroup")) \
)                                                                        \
.property("use_pt_dir", &sopv::node::CopyToPoints::use_pt_dir)           \
(                                                                        \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("UsePtDir"))    \
)                                                                        \
)
REGIST_NODE_RTTI_DEFAULT(ForeachPrimBegin)
REGIST_NODE_RTTI(ForeachPrimEnd,
.property("do_single_pass", &sopv::node::ForeachPrimEnd::do_single_pass)         \
(                                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SinglePass"))          \
)                                                                                \
.property("single_pass_offset", &sopv::node::ForeachPrimEnd::single_pass_offset) \
(                                                                                \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SinglePassOffset"))    \
)                                                                                \
)
REGIST_NODE_RTTI_DEFAULT(Merge)
REGIST_NODE_RTTI_DEFAULT(Null)
REGIST_NODE_RTTI(Switch,
.property("selected", &sopv::node::Switch::selected)                  \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Selected")) \
)                                                                     \
)

}

namespace sopv
{

void nodes_regist_rttr()
{
}

}