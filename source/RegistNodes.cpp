#include "sopview/RegistNodes.h"

#include <ee0/ReflectPropTypes.h>

#include <js/RTTR.h>

#define REGIST_NODE_RTTI(type, name, prop)                      \
	rttr::registration::class_<sopv::node::type>("sopv::"#name) \
		.constructor<>()                                        \
		prop                                                    \
	;

#define PROP
#define REGIST_NODE_RTTI_DEFAULT(type, name) REGIST_NODE_RTTI(type, name, PROP)

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

// attribute
REGIST_NODE_RTTI(AttributeCreate, attribcreate::2.0,
.property("group", &sopv::node::AttributeCreate::group_name)           \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName")) \
)                                                                      \
.property("grouptype", &sopv::node::AttributeCreate::group_type)       \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupType")) \
)                                                                      \
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
REGIST_NODE_RTTI(AttributePromote, attribpromote,
.property("inname", &sopv::node::AttributePromote::attr_name)          \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Attr Name")) \
)                                                                      \
.property("inclass", &sopv::node::AttributePromote::from_cls)          \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("From Type")) \
)                                                                      \
.property("outclass", &sopv::node::AttributePromote::to_cls)           \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("To Type"))   \
)                                                                      \
)
REGIST_NODE_RTTI(AttributeTransfer, attribtransfer,
.property("pointattribs", &sopv::node::AttributeTransfer::points_attrs_toggle)  \
(                                                                               \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("PointsToggle"))       \
)                                                                               \
.property("vertexattribs", &sopv::node::AttributeTransfer::vertices_attrs_toggle)   \
(                                                                               \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("VerticesToggle"))     \
)                                                                               \
.property("primitiveattribs", &sopv::node::AttributeTransfer::prims_attrs_toggle)   \
(                                                                               \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("PrimsToggle"))        \
)                                                                               \
.property("detailattribs", &sopv::node::AttributeTransfer::detail_attrs_toggle) \
(                                                                               \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("DetailToggle"))       \
)                                                                               \
.property("pointattriblist", &sopv::node::AttributeTransfer::points_attrs)      \
(                                                                               \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Points"))             \
)                                                                               \
.property("vertexattriblist", &sopv::node::AttributeTransfer::vertices_attrs)   \
(                                                                               \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Vertices"))           \
)                                                                               \
.property("primattriblist", &sopv::node::AttributeTransfer::primitives_attrs)   \
(                                                                               \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Primitives"))         \
)                                                                               \
.property("detailattriblist", &sopv::node::AttributeTransfer::detail_attrs)     \
(                                                                               \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Detail"))             \
)                                                                               \
)
REGIST_NODE_RTTI_DEFAULT(AttributeVOP, attribvop)
REGIST_NODE_RTTI(AttributeWrangle, attribwrangle,
.property("snippet", &sopv::node::AttributeWrangle::vex_expr)         \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("VexExpr")), \
    rttr::metadata(ee0::PropLongStringTag(), true)                    \
)                                                                     \
)
REGIST_NODE_RTTI(Measure, measure,
.property("type", &sopv::node::Measure::measure_type)                     \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("MeasureType"))  \
)                                                                         \
.property("override", &sopv::node::Measure::override_name)                \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("OverrideName")) \
)                                                                         \
.property("attribname", &sopv::node::Measure::attr_name)                  \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Attribute"))    \
)                                                                         \
)
REGIST_NODE_RTTI(Sort, sort,
.property("ptsort", &sopv::node::Sort::key)                               \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Key"))          \
)                                                                         \
.property("pointreverse", &sopv::node::Sort::point_reverse)               \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("PointReverse")) \
)                                                                         \
.property("pointoffset", &sopv::node::Sort::point_offset)                 \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("PointOffset"))  \
)                                                                         \
)

// group
REGIST_NODE_RTTI(GroupCreate, groupcreate,
.property("groupname", &sopv::node::GroupCreate::group_name)                \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName"))      \
)                                                                           \
.property("grouptype", &sopv::node::GroupCreate::group_type)                \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupType"))      \
)                                                                           \
.property("mergeop", &sopv::node::GroupCreate::merge_op)                    \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("MergeOp"))        \
)                                                                           \
.property("groupbase", &sopv::node::GroupCreate::base_group)                \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("BaseGroup"))      \
)                                                                           \
.property("basegroup", &sopv::node::GroupCreate::base_group_expr)           \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("BaseGroupExpr"))  \
)                                                                           \
.property("groupbounding", &sopv::node::GroupCreate::keep_in_bounding)      \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("KeepInBounding")) \
)                                                                           \
.property("boundtype", &sopv::node::GroupCreate::bounding_type)             \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("BoundingType"))   \
)                                                                           \
.property("groupnormal", &sopv::node::GroupCreate::keep_by_normals)         \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("KeepByNormals"))  \
)                                                                           \
.property("dir", &sopv::node::GroupCreate::direction)                       \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Direction"))      \
)                                                                           \
.property("angle", &sopv::node::GroupCreate::spread_angle)                  \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SpreadAngle"))    \
)                                                                           \
)
REGIST_NODE_RTTI(GroupExpression, groupexpression,
.property("grouptype", &sopv::node::GroupExpression::group_type)       \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupType")) \
)                                                                      \
.property("expressions", &sopv::node::GroupExpression::inst_num)       \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupVEXpressions")) \
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
REGIST_NODE_RTTI(GroupPromote, grouppromote,
.property("group1", &sopv::node::GroupPromote::group_name)             \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName")) \
)                                                                      \
.property("fromtype1", &sopv::node::GroupPromote::src_type1)           \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SrcType"))   \
)                                                                      \
.property("totype1", &sopv::node::GroupPromote::dst_type1)             \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("DstType"))   \
)                                                                      \
)

// import
REGIST_NODE_RTTI(File, file,
.property("file", &sopv::node::File::filepath)                         \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")), \
    rttr::metadata(js::RTTR::FilePathTag(), true),                     \
    rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.*"))   \
)                                                                      \
)
REGIST_NODE_RTTI(ObjectMerge, object_merge,
.property("objpath1", &sopv::node::ObjectMerge::obj0)                 \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Object 1")) \
)                                                                     \
.property("objpath2", &sopv::node::ObjectMerge::obj1)                 \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Object 2")) \
)                                                                     \
.property("objpath3", &sopv::node::ObjectMerge::obj2)                 \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Object 3")) \
)                                                                     \
.property("objpath4", &sopv::node::ObjectMerge::obj3)                 \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Object 4")) \
)                                                                     \
)

// manipulate
REGIST_NODE_RTTI(Delete, delete,
.property("negate", &sopv::node::Delete::del_selected)                      \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("DelNonSelected")) \
)                                                                           \
.property("entity", &sopv::node::Delete::entity_type)                       \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("EntityType"))     \
)                                                                           \
.property("groupop", &sopv::node::Delete::op_rule)                          \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Operation"))      \
)                                                                           \
.property("filter", &sopv::node::Delete::filter_exp)                        \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("FilterExp"))      \
)                                                                           \
)
REGIST_NODE_RTTI(Peak, peak,
.property("group", &sopv::node::Peak::group_name)                      \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName")) \
)                                                                      \
.property("grouptype", &sopv::node::Peak::group_type)                  \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupType")) \
)                                                                      \
.property("dist", &sopv::node::Peak::distance)                         \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Distance"))  \
)                                                                      \
.property("updatenmls", &sopv::node::Peak::update_norm)                \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Recompute Point Normals"))  \
)                                                                      \
)
REGIST_NODE_RTTI(Transform, xform,
.property("group", &sopv::node::Transform::group_name)                 \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName")) \
)                                                                      \
.property("grouptype", &sopv::node::Transform::group_type)             \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupType")) \
)                                                                      \
.property("t", &sopv::node::Transform::translate)                      \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Translate")) \
)                                                                      \
.property("r", &sopv::node::Transform::rotate)                         \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Rotate"))    \
)                                                                      \
.property("s", &sopv::node::Transform::scale)                          \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Scale"))     \
)                                                                      \
.property("shear", &sopv::node::Transform::shear)                      \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Shear"))     \
)                                                                      \
)

// material
REGIST_NODE_RTTI(Color, color,
.property("color", &sopv::node::Color::color)                      \
(                                                                  \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Color")) \
)                                                                  \
)
REGIST_NODE_RTTI_DEFAULT(UVLayout, uvlayout)
REGIST_NODE_RTTI(UVQuickShade, uvquickshade,
.property("texture", &sopv::node::UVQuickShade::image_file)             \
(                                                                       \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ImageFile")), \
    rttr::metadata(js::RTTR::FilePathTag(), true),                      \
    rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.*"))    \
)                                                                       \
)
REGIST_NODE_RTTI(UVTransform, uvtransform::2.0,
.property("group", &sopv::node::UVTransform::group_name)               \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName")) \
)                                                                      \
.property("grouptype", &sopv::node::UVTransform::group_type)           \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupType")) \
)                                                                      \
.property("t", &sopv::node::UVTransform::translate)                    \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Translate")) \
)                                                                      \
.property("r", &sopv::node::UVTransform::rotate)                       \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Rotate"))    \
)                                                                      \
.property("s", &sopv::node::UVTransform::scale)                        \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Scale"))     \
)                                                                      \
.property("shear", &sopv::node::UVTransform::shear)                    \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Shear"))     \
)                                                                      \
)
//REGIST_NODE_RTTI_DEFAULT(UVUnwrap, uvunwrap)
REGIST_NODE_RTTI(UVUnwrap, uvunwrap,
.property("scale", &sopv::node::UVUnwrap::scale)                    \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Scale"))  \
)                                                                   \
.property("r", &sopv::node::UVUnwrap::rotate)                       \
(                                                                   \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Rotate")) \
)                                                                   \
)

// NURBs
REGIST_NODE_RTTI(Carve, carve,
.property("firstu", &sopv::node::Carve::first_u_toggle)                    \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("FirstUToggle"))  \
)                                                                          \
.property("secondu", &sopv::node::Carve::second_u_toggle)                  \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SecondUToggle")) \
)                                                                          \
.property("firstv", &sopv::node::Carve::first_v_toggle)                    \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("FirstVToggle"))  \
)                                                                          \
.property("secondv", &sopv::node::Carve::second_v_toggle)                  \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SecondVToggle")) \
)                                                                          \
.property("domainu1", &sopv::node::Carve::first_u)                         \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("FirstU"))        \
)                                                                          \
.property("domainu2", &sopv::node::Carve::second_u)                        \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SecondU"))       \
)                                                                          \
.property("domainv1", &sopv::node::Carve::first_v)                         \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("FirstV"))        \
)                                                                          \
.property("domainv2", &sopv::node::Carve::second_v)                        \
(                                                                          \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SecondV"))       \
)                                                                          \
)

// polygon
REGIST_NODE_RTTI(Add, add,
.property("stdswitcher1", &sopv::node::Add::top_switch)               \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Top"))      \
)                                                                     \
.property("switcher1", &sopv::node::Add::poly_switch)                 \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Poly"))     \
)                                                                     \
.property("points", &sopv::node::Add::pt_num)                         \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("PointNum")) \
)                                                                     \
.property("usept0", &sopv::node::Add::p0_toggle)                      \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("P0Toggle")) \
)                                                                     \
.property("usept1", &sopv::node::Add::p1_toggle)                      \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("P1Toggle")) \
)                                                                     \
.property("usept2", &sopv::node::Add::p2_toggle)                      \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("P2Toggle")) \
)                                                                     \
.property("usept3", &sopv::node::Add::p3_toggle)                      \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("P3Toggle")) \
)                                                                     \
.property("pt0", &sopv::node::Add::p0)                                \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("P0"))       \
)                                                                     \
.property("pt1", &sopv::node::Add::p1)                                \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("P1"))       \
)                                                                     \
.property("pt2", &sopv::node::Add::p2)                                \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("P2"))       \
)                                                                     \
.property("pt3", &sopv::node::Add::p3)                                \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("P3"))       \
)                                                                     \
)
REGIST_NODE_RTTI(Boolean, boolean::2.0,
.property("asurface", &sopv::node::Boolean::a_surface)                        \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("A Treat As"))       \
)                                                                             \
.property("bsurface", &sopv::node::Boolean::b_surface)                        \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("B Treat As"))       \
)                                                                             \
.property("booleanop", &sopv::node::Boolean::op)                              \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Operator"))         \
)                                                                             \
.property("subtractchoices", &sopv::node::Boolean::sub_type)                  \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SubType"))          \
)                                                                             \
.property("useabseamedges", &sopv::node::Boolean::ab_seam_edges_toggle)       \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("A-B Seams toggle")) \
)                                                                             \
.property("abseamedges", &sopv::node::Boolean::ab_seam_edges)                 \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("A-B Seams"))        \
)                                                                             \
)
REGIST_NODE_RTTI(Divide, divide,
.property("avoidsmallangles", &sopv::node::Divide::avoid_small_angles)          \
(                                                                               \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Avoid Small Angles")) \
)                                                                               \
)
REGIST_NODE_RTTI(Fuse, fuse,
.property("switcher1", &sopv::node::Fuse::op)                         \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("op"))       \
)                                                                     \
.property("dist", &sopv::node::Fuse::distance)                        \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Distance")) \
)                                                                     \
.property("keepunusedpoints", &sopv::node::Fuse::keep_unused_points)  \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("KeepUnusedPoints")) \
)                                                                     \
.property("updatenml", &sopv::node::Fuse::update_point_norm)          \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("UpdatePointNormals")) \
)                                                                     \
)
REGIST_NODE_RTTI(Knife, knife,
.property("knifeop", &sopv::node::Knife::knife_op)                     \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Keep"))      \
)                                                                      \
.property("origin", &sopv::node::Knife::origin)                        \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Origin"))    \
)                                                                      \
.property("dir", &sopv::node::Knife::direction)                        \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Direction")) \
)
)
REGIST_NODE_RTTI(Normal, normal,
.property("type", &sopv::node::Normal::attr_add_norm_to)               \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("AddNormTo")) \
)                                                                      \
)
REGIST_NODE_RTTI(PolyExtrude, polyextrude::2.0,
.property("group", &sopv::node::PolyExtrude::group_name)                      \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName"))        \
)                                                                             \
.property("extrusionmode", &sopv::node::PolyExtrude::extrusion)               \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ExtrusionMode"))    \
)                                                                             \
.property("dist", &sopv::node::PolyExtrude::distance)                         \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Distance"))         \
)                                                                             \
.property("outputfront", &sopv::node::PolyExtrude::output_front)              \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("OutputFront"))      \
)                                                                             \
.property("outputback", &sopv::node::PolyExtrude::output_back)                \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("OutputBack"))       \
)                                                                             \
.property("outputside", &sopv::node::PolyExtrude::output_side)                \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("OutputSide"))       \
)                                                                             \
.property("outputfrontgrp", &sopv::node::PolyExtrude::front_group_toggle)     \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("FrontGroupToggle")) \
)                                                                             \
.property("outputbackgrp", &sopv::node::PolyExtrude::back_group_toggle)       \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("BackGroupToggle"))  \
)                                                                             \
.property("outputsidegrp", &sopv::node::PolyExtrude::side_group_toggle)       \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SideGroupToggle"))  \
)                                                                             \
.property("frontgrp", &sopv::node::PolyExtrude::front_group)                  \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("FrontGroup"))       \
)                                                                             \
.property("backgrp", &sopv::node::PolyExtrude::back_group)                    \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("BackGroup"))        \
)                                                                             \
.property("sidegrp", &sopv::node::PolyExtrude::side_group)                    \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SideGroup"))        \
)
)
REGIST_NODE_RTTI(PolyFill, polyfill,
.property("fillmode", &sopv::node::PolyFill::fill_mode)                   \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("FillMode"))     \
)                                                                         \
.property("smoothtoggle", &sopv::node::PolyFill::smooth_toggle)           \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SmoothToggle")) \
)                                                                         \
.property("smoothstrength", &sopv::node::PolyFill::smooth_strength)       \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Smooth"))       \
)                                                                         \
)
REGIST_NODE_RTTI(PolyFrame, polyframe,
.property("entity", &sopv::node::PolyFrame::entity_type)                     \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("EntityType"))      \
)                                                                            \
.property("style", &sopv::node::PolyFrame::frame_style)                      \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("FrameStyle"))      \
)                                                                            \
.property("Non", &sopv::node::PolyFrame::normal_toggle)                      \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("NormalToggle"))    \
)                                                                            \
.property("tangentuon", &sopv::node::PolyFrame::tangent_toggle)              \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("TangentToggle"))   \
)                                                                            \
.property("tangentvon", &sopv::node::PolyFrame::bitangent_toggle)            \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("BitangentToggle")) \
)                                                                            \
.property("N", &sopv::node::PolyFrame::normal_name)                          \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("NormalName"))      \
)                                                                            \
.property("tangentu", &sopv::node::PolyFrame::tangent_name)                  \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("TangentName"))     \
)                                                                            \
.property("tangentv", &sopv::node::PolyFrame::bitangent_name)                \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("BitangentName"))   \
)                                                                            \
)

// primitive
REGIST_NODE_RTTI(Box, box,
.property("size", &sopv::node::Box::size)                                    \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Size"))            \
)                                                                            \
.property("t", &sopv::node::Box::center)                                     \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Center"))          \
)                                                                            \
.property("scale", &sopv::node::Box::scale)                                  \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Scale"))           \
)                                                                            \
.property("dodivs", &sopv::node::Box::divis_toggle)                          \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("DivisionsToggle")) \
)                                                                            \
.property("divs", &sopv::node::Box::divis)                                   \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Divisions"))       \
)                                                                            \
)
REGIST_NODE_RTTI(Curve, curve,
.property("vertices", &sopv::node::Curve::vertices)                   \
(                                                                     \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Vertices")) \
)                                                                     \
)
REGIST_NODE_RTTI(Grid, grid,
.property("size", &sopv::node::Grid::size)                           \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Size"))    \
)                                                                    \
.property("rows", &sopv::node::Grid::rows)                           \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Rows"))    \
)                                                                    \
.property("cols", &sopv::node::Grid::columns)                        \
(                                                                    \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Columns")) \
)                                                                    \
)
REGIST_NODE_RTTI(Line, line,
.property("origin", &sopv::node::Line::origin)                         \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Origin"))    \
)                                                                      \
.property("dir", &sopv::node::Line::direction)                         \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Direction")) \
)                                                                      \
.property("dist", &sopv::node::Line::length)                           \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Length"))    \
)                                                                      \
.property("points", &sopv::node::Line::points)                         \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Points"))    \
)                                                                      \
)
REGIST_NODE_RTTI(Primitive, primitive,
.property("group", &sopv::node::Primitive::group_name)                                      \
(                                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName"))                      \
)                                                                                           \
.property("doxform", &sopv::node::Primitive::trans_toggle)                                  \
(                                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("TransToggle"))                    \
)                                                                                           \
.property("t", &sopv::node::Primitive::translate)                                           \
(                                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Translate"))                      \
)                                                                                           \
.property("r", &sopv::node::Primitive::rotate)                                              \
(                                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Rotate"))                         \
)                                                                                           \
.property("s", &sopv::node::Primitive::scale)                                               \
(                                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Scale"))                          \
)                                                                                           \
.property("shear", &sopv::node::Primitive::shear)                                           \
(                                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Shear"))                          \
)                                                                                           \
.property("p", &sopv::node::Primitive::pivot_trans)                                         \
(                                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Pivot Translate"))                \
)                                                                                           \
.property("pr", &sopv::node::Primitive::pivot_rotate)                                       \
(                                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Pivot Rotate"))                   \
)                                                                                           \
.property("xformattribs", &sopv::node::Primitive::vec_attrs_to_trans)                       \
(                                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Vector Attributes To Transform")) \
)                                                                                           \
)
REGIST_NODE_RTTI_DEFAULT(Sphere, sphere)
// primitive extern
REGIST_NODE_RTTI(Dungeon, dungeon,
.property("size", &sopv::node::Dungeon::size)                          \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Size"))      \
)                                                                      \
.property("split_sz", &sopv::node::Dungeon::split_sz)                  \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SplitSize")) \
)                                                                      \
.property("min_size", &sopv::node::Dungeon::min_sz)                    \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("MinSize"))   \
)                                                                      \
.property("seed", &sopv::node::Dungeon::seed)                          \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Seed"))      \
)                                                                      \
)

// rigging
REGIST_NODE_RTTI_DEFAULT(Lattice, lattice)

// terrain
REGIST_NODE_RTTI(HeightField, heightfield,
.property("sizex", &sopv::node::HeightField::sizex)                          \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Width"))           \
)                                                                            \
.property("sizey", &sopv::node::HeightField::sizey)                          \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Height"))          \
)                                                                            \
)

// utility
REGIST_NODE_RTTI(Blast, blast,
.property("group", &sopv::node::Blast::group_name)                           \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName"))       \
)                                                                            \
.property("grouptype", &sopv::node::Blast::group_type)                       \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupType"))       \
)                                                                            \
.property("negate", &sopv::node::Blast::del_non_selected)                    \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("DelNonSelected"))  \
)                                                                            \
.property("removegrp", &sopv::node::Blast::del_unused_groups)                \
(                                                                            \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("DelUnusedGroups")) \
)                                                                            \
)
REGIST_NODE_RTTI(CopyToPoints, copytopoints,
.property("sourcegroup", &sopv::node::CopyToPoints::src_group)              \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SrcGroup"))       \
)                                                                           \
.property("targetgroup", &sopv::node::CopyToPoints::target_group)           \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("TargetGroup"))    \
)                                                                           \
.property("targetgroup_str", &sopv::node::CopyToPoints::target_group_str)   \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("TargetGroupStr")) \
)                                                                           \
.property("showguide", &sopv::node::CopyToPoints::show_guide)               \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("ShowGuide"))      \
)                                                                           \
.property("transform", &sopv::node::CopyToPoints::trans_dir)                \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("TransUsingPtOri"))\
)                                                                           \
.property("doattr", &sopv::node::CopyToPoints::copy_attr)                   \
(                                                                           \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("CopyPtAttr"))     \
)                                                                           \
)
REGIST_NODE_RTTI(ForeachBegin, block_begin,
.property("method", &sopv::node::ForeachBegin::method)                 \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Method"))    \
)                                                                      \
.property("blockpath", &sopv::node::ForeachBegin::block_path)          \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("BlockPath")) \
)                                                                      \
)
REGIST_NODE_RTTI(ForeachEnd, block_end,
.property("itermethod", &sopv::node::ForeachEnd::iter)                        \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("IterationMethod"))  \
)                                                                             \
.property("method", &sopv::node::ForeachEnd::gather)                          \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GatherMethod"))     \
)                                                                             \
.property("class", &sopv::node::ForeachEnd::element)                          \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("PieceElements"))    \
)                                                                             \
.property("useattrib", &sopv::node::ForeachEnd::piece_attrib_toggle)          \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("PieceAttribToggle"))\
)                                                                             \
.property("attrib", &sopv::node::ForeachEnd::piece_attrib)                    \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("PieceAttribute"))   \
)                                                                             \
.property("usemaxiter", &sopv::node::ForeachEnd::use_max_iter)                \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("UseMaxIter"))       \
)                                                                             \
.property("maxiter", &sopv::node::ForeachEnd::max_iter)                       \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("MaxIterations"))    \
)                                                                             \
.property("blockpath", &sopv::node::ForeachEnd::block_path)                   \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("DefaultBlockPath")) \
)                                                                             \
.property("templatepath", &sopv::node::ForeachEnd::template_path)             \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("PieceBlockPath"))   \
)                                                                             \
.property("dosinglepass", &sopv::node::ForeachEnd::do_single_pass)            \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SinglePass"))       \
)                                                                             \
.property("singlepass", &sopv::node::ForeachEnd::single_pass_offset)          \
(                                                                             \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("SinglePassOffset")) \
)                                                                             \
)
REGIST_NODE_RTTI_DEFAULT(Merge, merge)
REGIST_NODE_RTTI_DEFAULT(Null, null)
REGIST_NODE_RTTI(Output, output,
.property("outputidx", &sopv::node::Output::output_idx)                \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("OutputIdx")) \
)                                                                      \
)
REGIST_NODE_RTTI(Python, python,
.property("python", &sopv::node::Python::code)                            \
(                                                                         \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Python Code")), \
    rttr::metadata(ee0::PropLongStringTag(), true)                        \
)                                                                         \
)
REGIST_NODE_RTTI(Split, split,
.property("group", &sopv::node::Split::group_name)                     \
(                                                                      \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("GroupName")) \
)                                                                      \
)
REGIST_NODE_RTTI_DEFAULT(Subnetwork, subnetwork)
REGIST_NODE_RTTI(Switch, switch,
.property("input", &sopv::node::Switch::selected)                     \
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