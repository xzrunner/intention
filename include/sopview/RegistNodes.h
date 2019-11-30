#pragma once

#include "sopview/Node.h"
#include "sopview/Compound.h"
#include "sopview/ReflectPropTypes.h"

#include <blueprint/Pin.h>

#include <sop/node/Sort.h>
#include <sop/node/Boolean.h>
#include <sop/node/Delete.h>
#include <sop/node/Measure.h>
#include <sop/node/PolyFrame.h>
#include <sop/node/PolyExtrude.h>
#include <sop/node/PolyFill.h>
#include <sop/node/Fuse.h>
#include <sop/node/ForeachPrimBegin.h>
#include <sop/node/ForeachPrimEnd.h>
#include <sop/node/UVUnwrap.h>
#include <sop/node/Knife.h>
#include <sop/node/AttributeCreate.h>
#include <sop/node/GroupCreate.h>
#include <sop/node/Normal.h>

namespace sopv
{

void nodes_regist_rttr();

namespace node
{

#define SOPV_DEFINE_NODE(type, name, parm)            \
class type : public Node                              \
{                                                     \
public:                                               \
	type()                                            \
		: Node(#type)                                 \
	{                                                 \
		InitPins(#name);                              \
	}                                                 \
                                                      \
	parm                                              \
	RTTR_ENABLE(Node)                                 \
};

#define SOPV_DEFINE_COMPOUND_NODE(type, name, parm)   \
class type : public Compound                          \
{                                                     \
public:                                               \
	type()                                            \
		: Compound(#type)                             \
	{                                                 \
		InitPins(#name);                              \
		InitChildren(#name);                          \
	}                                                 \
                                                      \
	parm                                              \
	RTTR_ENABLE(Compound)                             \
};

#define SOPV_DEFINE_PROPS_NODE(type, name, parm) \
class type : public Node                         \
{                                                \
public:                                          \
	type()                                       \
		: Node(#type, true)                      \
	{                                            \
		InitPins(#name);                         \
	}                                            \
                                                 \
	parm                                         \
	RTTR_ENABLE(Node)                            \
};

#define SOPV_DEFINE_IMPORT_EXT_NODE(type, name, parm)                \
class type : public Node                                             \
{                                                                    \
public:                                                              \
	type()                                                           \
		: Node(#type)                                                \
	{                                                                \
		InitPins(#name);                                             \
        SetExtensibleInputPorts(true);                               \
	}                                                                \
private:                                                             \
    virtual std::string GenInputPinName(size_t idx) const override { \
        return "in" + std::to_string(idx);                           \
    }                                                                \
                                                                     \
public:                                                              \
	parm                                                             \
	RTTR_ENABLE(Node)                                                \
};

#define SOPV_NODE_PROP

// attribute
SOPV_DEFINE_NODE(AttributeCreate, attribcreate::2.0,
    GroupName  group_name;                    \
    sop::GroupTypes group_type = sop::GroupTypes::Auto; \
    AttrCreateItem item0;                     \
    AttrCreateItem item1;                     \
    AttrCreateItem item2;                     \
    AttrCreateItem item3;                     \
)
SOPV_DEFINE_NODE(AttributePromote, attribpromote,
    AttributeName     attr_name = sop::GeoAttrClass::Point; \
    sop::GeoAttrClass  from_cls = sop::GeoAttrClass::Point; \
    sop::GeoAttrClass  to_cls   = sop::GeoAttrClass::Point; \
)
SOPV_DEFINE_NODE(AttributeTransfer, attribtransfer,
    bool points_attrs_toggle       = false;                   \
    bool vertices_attrs_toggle     = false;                   \
    bool prims_attrs_toggle        = false;                   \
    bool detail_attrs_toggle       = false;                   \
    AttributeName points_attrs     = sop::GeoAttrClass::Point;     \
    AttributeName vertices_attrs   = sop::GeoAttrClass::Vertex;    \
    AttributeName primitives_attrs = sop::GeoAttrClass::Primitive; \
    AttributeName detail_attrs     = sop::GeoAttrClass::Detail;    \
)
SOPV_DEFINE_COMPOUND_NODE(AttributeVOP, attribvop, SOPV_NODE_PROP)
SOPV_DEFINE_NODE(AttributeWrangle, attribwrangle,
    std::string vex_expr; \
)
SOPV_DEFINE_NODE(Measure, measure,
    sop::node::Measure::Type measure_type = sop::node::Measure::Type::Perimeter; \
    bool        override_name = false;                  \
    std::string attr_name;                              \
)
SOPV_DEFINE_NODE(Sort, sort,
    sop::node::Sort::Key key = sop::node::Sort::Key::NoChange; \
    bool        point_reverse = false;   \
    std::string point_offset = "0";      \
)

// group
SOPV_DEFINE_NODE(GroupCreate, groupcreate,
    std::string group_name;                         \
    sop::GroupType   group_type = sop::GroupType::Primitives; \
    sop::GroupMerge  merge_op   = sop::GroupMerge::Replace;   \
    bool        base_group = false;                 \
    std::string base_group_expr;                    \
    bool        keep_in_bounding = false;           \
    sop::node::GroupCreate::BoundingType bounding_type = sop::node::GroupCreate::BoundingType::Box;
    bool        keep_by_normals = false;            \
    sm::vec3    direction = sm::vec3(0, 0, 1);      \
    float       spread_angle = 180;                 \
)
SOPV_DEFINE_NODE(GroupExpression, groupexpression,
    sop::GroupType   group_type = sop::GroupType::Primitives; \
    size_t inst_num = 0;                            \
    GroupExprInst inst0;                            \
    GroupExprInst inst1;                            \
    GroupExprInst inst2;                            \
    GroupExprInst inst3;                            \
)
SOPV_DEFINE_NODE(GroupPromote, grouppromote,
    GroupName group_name;                      \
    sop::GroupTypes src_type1 = sop::GroupTypes::Auto;   \
    sop::GroupTypes dst_type1 = sop::GroupTypes::Points; \
)

// import
SOPV_DEFINE_NODE(File, file,
    std::string filepath; \
)
SOPV_DEFINE_NODE(ObjectMerge, object_merge,
    std::string obj0; \
    std::string obj1; \
    std::string obj2; \
    std::string obj3; \
)

// manipulate
SOPV_DEFINE_NODE(Delete, delete,
    sop::node::Delete::NegateSelected del_selected = sop::node::Delete::NegateSelected::Delete; \
    sop::node::Delete::EntityType     entity_type  = sop::node::Delete::EntityType::Points;     \
    sop::node::Delete::OpRule         op_rule      = sop::node::Delete::OpRule::Pattern;        \
    std::string                       filter_exp;                                               \
)
SOPV_DEFINE_NODE(Peak, peak,
    GroupName   group_name;                             \
    sop::GroupType   group_type = sop::GroupType::GuessFromGroup; \
    std::string distance = "0";                         \
    bool        update_norm = true;                     \
)
SOPV_DEFINE_NODE(Transform, xform,
    GroupName group_name;                             \
    sop::GroupType group_type = sop::GroupType::GuessFromGroup; \
    StrVec3 translate = StrVec3("0", "0", "0");       \
    StrVec3 rotate = StrVec3("0", "0", "0");          \
    StrVec3 scale = StrVec3("1", "1", "1");           \
    StrVec3 shear = StrVec3("0", "0", "0");           \
)

// material
SOPV_DEFINE_NODE(Color, color,
    sm::vec3 color = sm::vec3(1.0f, 1.0f, 1.0f); \
)
SOPV_DEFINE_NODE(UVLayout, uvlayout, SOPV_NODE_PROP)
SOPV_DEFINE_NODE(UVQuickShade, uvquickshade,
    std::string image_file; \
)
SOPV_DEFINE_NODE(UVTransform, uvtransform::2.0,
    GroupName group_name;                             \
    sop::GroupType group_type = sop::GroupType::GuessFromGroup; \
    StrVec3   translate = StrVec3("0", "0", "0");    \
    StrVec3   rotate = StrVec3("0", "0", "0");    \
    StrVec3   scale = StrVec3("1", "1", "1");    \
    StrVec3   shear = StrVec3("0", "0", "0");    \
)
SOPV_DEFINE_NODE(UVUnwrap, uvunwrap,
    sop::node::UVUnwrap::ScaleType scale = sop::node::UVUnwrap::ScaleType::Uniform;  \
    sm::vec3 rotate = sm::vec3(0, 0, 0); \
)

// NURBs
SOPV_DEFINE_NODE(Carve, carve,
    bool first_u_toggle = false; \
    bool second_u_toggle = false; \
    bool first_v_toggle = false; \
    bool second_v_toggle = false; \
    std::string first_u = "0";   \
    std::string second_u = "1";   \
    std::string first_v = "0";   \
    std::string second_v = "1";   \
)

// polygon
SOPV_DEFINE_NODE(Add, add,
    size_t top_switch = 0;  \
    size_t poly_switch = 0; \
    size_t pt_num = 0;      \
    bool p0_toggle = false; \
    bool p1_toggle = false; \
    bool p2_toggle = false; \
    bool p3_toggle = false; \
    sm::vec3 p0;            \
    sm::vec3 p1;            \
    sm::vec3 p2;            \
    sm::vec3 p3;            \
)
SOPV_DEFINE_NODE(Boolean, boolean::2.0,
    sop::node::Boolean::GeoType  a_surface = sop::node::Boolean::GeoType::Solid;      \
    sop::node::Boolean::GeoType  b_surface = sop::node::Boolean::GeoType::Solid;      \
    sop::node::Boolean::Operator op        = sop::node::Boolean::Operator::Intersect; \
    sop::node::Boolean::SubType  sub_type  = sop::node::Boolean::SubType::AMinusB;    \
    bool        ab_seam_edges_toggle = false;                                         \
    std::string ab_seam_edges;                                                        \
)
SOPV_DEFINE_NODE(Divide, divide,
    bool avoid_small_angles = false; \
)
SOPV_DEFINE_NODE(Fuse, fuse,
    sop::node::Fuse::Operator op = sop::node::Fuse::Operator::Consolidate; \
    float distance           = 0.001f;                    \
    bool  keep_unused_points = false;                     \
    bool  update_point_norm  = true;                      \
)
SOPV_DEFINE_NODE(Knife, knife,
    sop::node::Knife::KeepType knife_op = sop::node::Knife::KeepType::KeepAll;     \
    StrVec3   origin = StrVec3("0", "0", "0"); \
    sm::vec3  direction = sm::vec3(0, 1, 0);      \
)
SOPV_DEFINE_NODE(Normal, normal,
    sop::node::Normal::AddToType attr_add_norm_to = sop::node::Normal::AddToType::Vertices; \
)
SOPV_DEFINE_NODE(PolyExtrude, polyextrude::2.0,
    GroupName group_name;            \
    sop::node::PolyExtrude::Extrusion extrusion = sop::node::PolyExtrude::Extrusion::PrimEdgeNorm; \
    float distance = 0;              \
    bool output_front = true;        \
    bool output_back  = false;       \
    bool output_side  = true;        \
    bool front_group_toggle = false; \
    bool back_group_toggle  = false; \
    bool side_group_toggle  = false; \
    std::string front_group;         \
    std::string back_group;          \
    std::string side_group;          \
)
SOPV_DEFINE_NODE(PolyFill, polyfill,
    sop::node::PolyFill::Mode fill_mode       = sop::node::PolyFill::Mode::QuadrilateralGrid; \
    bool         smooth_toggle   = true;                            \
    float        smooth_strength = 50.0f;                           \
)
SOPV_DEFINE_NODE(PolyFrame, polyframe,
    sop::GroupType      entity_type = sop::GroupType::Primitives;     \
    sop::node::PolyFrame::FrameStyle frame_style = sop::node::PolyFrame::FrameStyle::TwoEdges;  \
    bool           normal_toggle    = true;                 \
    bool           tangent_toggle   = true;                 \
    bool           bitangent_toggle = false;                \
    std::string    normal_name;                             \
    std::string    tangent_name;                            \
    std::string    bitangent_name;                          \
)

// primitive
SOPV_DEFINE_NODE(Box, box,
    StrVec3   size   = StrVec3("1", "1", "1"); \
    StrVec3   center = StrVec3("0", "0", "0"); \
    float     scale  = 1.0f;                   \
    bool      divis_toggle = false;            \
    sm::ivec3 divis  = sm::ivec3(3, 3, 3);     \
)
SOPV_DEFINE_NODE(Curve, curve,
    std::vector<sm::vec3> vertices; \
)
SOPV_DEFINE_NODE(Grid, grid,
    sm::vec2 size    = sm::vec2(10, 10); \
    size_t   rows    = 10;               \
    size_t   columns = 10;               \
)
SOPV_DEFINE_NODE(Line, line,
    sm::vec3    origin    = sm::vec3(0, 0, 0); \
    sm::vec3    direction = sm::vec3(0, 1, 0); \
    std::string length    = "1";               \
    size_t      points    = 2;                 \
)
SOPV_DEFINE_NODE(Primitive, primitive,
    GroupName group_name;                      \
    bool trans_toggle     = false;             \
    sm::vec3 translate    = sm::vec3(0, 0, 0); \
    sm::vec3 rotate       = sm::vec3(0, 0, 0); \
    sm::vec3 scale        = sm::vec3(1, 1, 1); \
    sm::vec3 shear        = sm::vec3(0, 0, 0); \
    sm::vec3 pivot_trans  = sm::vec3(0, 0, 0); \
    sm::vec3 pivot_rotate = sm::vec3(0, 0, 0); \
    std::string vec_attrs_to_trans;
)
SOPV_DEFINE_NODE(Sphere, sphere, SOPV_NODE_PROP)
// primitive extern
SOPV_DEFINE_NODE(Dungeon, dungeon,
    sm::vec2 size     = sm::vec2(1, 1);         \
    sm::vec2 split_sz = sm::vec2(0.5f, 0.5f);   \
    sm::vec2 min_sz   = sm::vec2(0.25f, 0.25f); \
    uint32_t seed     = 0;                      \
)

// rigging
SOPV_DEFINE_NODE(Lattice, lattice, SOPV_NODE_PROP)

// utility
SOPV_DEFINE_NODE(Blast, blast,
    GroupName group_name;                             \
    sop::GroupType group_type = sop::GroupType::GuessFromGroup; \
    bool del_non_selected  = false;                   \
    bool del_unused_groups = false;                   \
)
SOPV_DEFINE_NODE(CopyToPoints, copytopoints,
    GroupName   src_group    = GroupName(0); \
    GroupName   target_group = GroupName(1); \
    std::string target_group_str;            \
    bool        show_guide   = true;         \
    bool        trans_dir    = true;         \
    bool        copy_attr    = true;         \
)
SOPV_DEFINE_NODE(ForeachBegin, block_begin,
    sop::node::ForeachPrimBegin::Method method = sop::node::ForeachPrimBegin::Method::Input; \
    std::string        block_path;                         \
)
SOPV_DEFINE_NODE(ForeachEnd, block_end,
    sop::node::ForeachPrimEnd::IterMethod    iter    = sop::node::ForeachPrimEnd::IterMethod::ByPiecesOrPoints; \
    sop::node::ForeachPrimEnd::GatherMethod  gather  = sop::node::ForeachPrimEnd::GatherMethod::Merge;          \
    sop::node::ForeachPrimEnd::PieceElements element = sop::node::ForeachPrimEnd::PieceElements::Primitives;    \
    bool        piece_attrib_toggle = false;                            \
    std::string piece_attrib;                                           \
    bool        use_max_iter = false;                                   \
    size_t      max_iter = 10;                                          \
    std::string block_path;                                             \
    std::string template_path;                                          \
    bool        do_single_pass     = false;                             \
    int         single_pass_offset = 0;                                 \
)
SOPV_DEFINE_IMPORT_EXT_NODE(Merge, merge, SOPV_NODE_PROP)
SOPV_DEFINE_PROPS_NODE(Null, null, SOPV_NODE_PROP)
SOPV_DEFINE_NODE(Output, output,
    int output_idx = -1; \
)
SOPV_DEFINE_NODE(Python, python,
    std::string code; \
)
SOPV_DEFINE_COMPOUND_NODE(Split, split,
    GroupName group_name; \
)
SOPV_DEFINE_COMPOUND_NODE(Subnetwork, subnetwork, SOPV_NODE_PROP)
SOPV_DEFINE_IMPORT_EXT_NODE(Switch, switch,
    std::string selected = "0"; \
)

}

}