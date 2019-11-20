#include "sopview/SOPAdapter.h"
#include "sopview/PinType.h"
#include "sopview/Evaluator.h"
#include "sopview/Node.h"
#include "sopview/RegistNodes.h"
#include "sopview/NodeProp.h"

#include <blueprint/Pin.h>
#include <blueprint/Node.h>
#include <blueprint/Connecting.h>
#include <blueprint/node/Function.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>

// attribute
#include <sop/node/AttributeCreate.h>
#include <sop/node/AttributePromote.h>
#include <sop/node/AttributeTransfer.h>
#include <sop/node/AttributeVOP.h>
#include <sop/node/AttributeWrangle.h>
#include <sop/node/Sort.h>
#include <sop/node/Measure.h>
// export
#include <sop/node/File.h>
#include <sop/node/ObjectMerge.h>
// group
#include <sop/node/GroupCreate.h>
#include <sop/node/GroupExpression.h>
#include <sop/node/GroupPromote.h>
// manipulate
#include <sop/node/Delete.h>
#include <sop/node/Peak.h>
#include <sop/node/Transform.h>
// material
#include <sop/node/Color.h>
#include <sop/node/UVQuickShade.h>
#include <sop/node/UVTransform.h>
// nurbs
#include <sop/node/Carve.h>
// polygon
#include <sop/node/Add.h>
#include <sop/node/Boolean.h>
#include <sop/node/Fuse.h>
#include <sop/node/Knife.h>
#include <sop/node/Normal.h>
#include <sop/node/PolyExtrude.h>
#include <sop/node/PolyFill.h>
#include <sop/node/PolyFrame.h>
// primitive
#include <sop/node/Box.h>
#include <sop/node/Curve.h>
#include <sop/node/Grid.h>
#include <sop/node/Line.h>
#include <sop/node/Primitive.h>
#include <sop/node/Sphere.h>
// primitive extern
#include <sop/node/Dungeon.h>
// utility
#include <sop/node/Blast.h>
#include <sop/node/CopyToPoints.h>
#include <sop/node/ForeachPrimEnd.h>
#include <sop/node/Python.h>
#include <sop/node/Split.h>
#include <sop/node/Switch.h>

#include <boost/lexical_cast.hpp>

namespace
{

sop::GroupType
TransGroupTypes(sopv::GroupTypes type)
{
    switch (type)
    {
    case sopv::GroupTypes::Auto:
        return sop::GroupType::GuessFromGroup;
    case sopv::GroupTypes::Primitives:
        return sop::GroupType::Primitives;
    case sopv::GroupTypes::Points:
        return sop::GroupType::Points;
    case sopv::GroupTypes::Edges:
        return sop::GroupType::Edges;
    case sopv::GroupTypes::Vertices:
        return sop::GroupType::Vertices;
    default:
        assert(0);
        return sop::GroupType::Primitives;
    }
}

sop::node::GroupExpression::Instance
TransGroupExprInst(const sopv::GroupExprInst& src)
{
    sop::node::GroupExpression::Instance dst;
    dst.group_name = src.group_name;
    dst.expr_str   = src.expr_str;
    dst.merge_op   = src.merge_op;
    return dst;
}

sop::VarValue TransAttrCreateVal(sop::node::AttributeCreate::ItemType type, const sm::vec4& value)
{
    sop::VarValue val;
    switch (type)
    {
    case sop::node::AttributeCreate::ItemType::Float:
        val = sop::VarValue(value.x);
        break;
    case sop::node::AttributeCreate::ItemType::Integer:
        val = sop::VarValue(static_cast<int>(value.x));
        break;
    default:
        assert(0);
        break;
    }
    return val;
}

sop::node::AttributeCreate::Item
TransAttrCreateItem(const sopv::AttrCreateItem& item)
{
    auto cls   = sopv::SOPAdapter::TransGeoAttrClass(item.cls);
    auto val   = TransAttrCreateVal(item.type, item.value);
    auto d_val = TransAttrCreateVal(item.type, item.default_val);

    return { item.name, item.type, cls, val, d_val };
}

sop::GeoAttrClass
TransGeoAttrClassType(sopv::NormalGeoAttrClass cls)
{
    switch (cls)
    {
    case sopv::NormalGeoAttrClass::Point:
        return sop::GeoAttrClass::Point;
    case sopv::NormalGeoAttrClass::Vertex:
        return sop::GeoAttrClass::Vertex;
    case sopv::NormalGeoAttrClass::Primitive:
        return sop::GeoAttrClass::Primitive;
    case sopv::NormalGeoAttrClass::Detail:
        return sop::GeoAttrClass::Detail;
    default:
        assert(0);
        return sop::GeoAttrClass::Point;
    }
}

}

namespace sopv
{

void SOPAdapter::UpdatePropBackFromFront(const bp::Node& front, sop::Node& back,
                                         const Evaluator& eval)
{
    auto type = front.get_type();
    // attribute
    if (type == rttr::type::get<node::AttributeCreate>())
    {
        auto& src = static_cast<const node::AttributeCreate&>(front);
        auto& dst = static_cast<sop::node::AttributeCreate&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetGroupType(TransGroupTypes(src.group_type));

        std::vector<sop::node::AttributeCreate::Item> items;
        if (!src.item0.name.empty()) {
            items.push_back(TransAttrCreateItem(src.item0));
        }
        if (!src.item1.name.empty()) {
            items.push_back(TransAttrCreateItem(src.item1));
        }
        if (!src.item2.name.empty()) {
            items.push_back(TransAttrCreateItem(src.item2));
        }
        if (!src.item3.name.empty()) {
            items.push_back(TransAttrCreateItem(src.item3));
        }
        dst.SetAttrItems(items);
    }
    else if (type == rttr::type::get<node::AttributePromote>())
    {
        auto& src = static_cast<const node::AttributePromote&>(front);
        auto& dst = static_cast<sop::node::AttributePromote&>(back);

        dst.SetAttrName(src.attr_name.str);

        dst.SetPromoteType(
            SOPAdapter::TransGeoAttrClass(src.from_cls),
            SOPAdapter::TransGeoAttrClass(src.to_cls)
        );
    }
    else if (type == rttr::type::get<node::AttributeTransfer>())
    {
        auto& src = static_cast<const node::AttributeTransfer&>(front);
        auto& dst = static_cast<sop::node::AttributeTransfer&>(back);

        dst.ClearCopyAttrs();
        if (src.points_attrs_toggle && !src.points_attrs.str.empty()) {
            dst.SetCopyAttrs(sop::GeoAttrClass::Point, { src.points_attrs.str });
        }
        if (src.vertices_attrs_toggle && !src.vertices_attrs.str.empty()) {
            dst.SetCopyAttrs(sop::GeoAttrClass::Vertex, { src.vertices_attrs.str });
        }
        if (src.prims_attrs_toggle && !src.primitives_attrs.str.empty()) {
            dst.SetCopyAttrs(sop::GeoAttrClass::Primitive, { src.primitives_attrs.str });
        }
        if (src.detail_attrs_toggle && !src.detail_attrs.str.empty()) {
            dst.SetCopyAttrs(sop::GeoAttrClass::Detail, { src.detail_attrs.str });
        }
    }
    else if (type == rttr::type::get<node::AttributeWrangle>())
    {
        auto& src = static_cast<const node::AttributeWrangle&>(front);
        auto& dst = static_cast<sop::node::AttributeWrangle&>(back);
        dst.SetVexExpr(src.vex_expr);
    }
    else if (type == rttr::type::get<node::Measure>())
    {
        auto& src = static_cast<const node::Measure&>(front);
        auto& dst = static_cast<sop::node::Measure&>(back);

        dst.SetMesureType(src.measure_type);
        if (src.override_name) {
            dst.SetMesureName(src.attr_name);
        }
    }
    else if (type == rttr::type::get<node::Sort>())
    {
        auto& src = static_cast<const node::Sort&>(front);
        auto& dst = static_cast<sop::node::Sort&>(back);

        dst.SetKey(src.key);
        dst.SetPointReverse(src.point_reverse);
        dst.SetPointOffset(ParseExprInt(src.point_offset, back,
            sop::node::Sort::POINT_OFFSET, 0, eval));
    }
    // export
    else if (type == rttr::type::get<node::File>())
    {
        auto& src = static_cast<const node::File&>(front);
        auto& dst = static_cast<sop::node::File&>(back);
        dst.SetFilepath(src.filepath);
    }
    else if (type == rttr::type::get<node::ObjectMerge>())
    {
        auto& src = static_cast<const node::ObjectMerge&>(front);
        auto& dst = static_cast<sop::node::ObjectMerge&>(back);
//        dst.SetObjects();
    }
    // group
    else if (type == rttr::type::get<node::GroupCreate>())
    {
        auto& src = static_cast<const node::GroupCreate&>(front);
        auto& dst = static_cast<sop::node::GroupCreate&>(back);

        dst.SetGroupName(src.group_name);
        dst.SetGroupType(src.group_type);
        dst.SetGroupMerge(src.merge_op);

        if (src.base_group) {
            dst.EnableBaseGroup(src.base_group_expr);
        } else {
            dst.DisableBaseGroup();
        }

        if (src.keep_in_bounding) {
            dst.EnableKeepInBounding();
        } else {
            dst.DisableKeepInBounding();
        }

        if (src.keep_by_normals) {
            dst.EnableKeepByNormals(src.direction, src.spread_angle);
        } else {
            dst.DisableKeepByNormals();
        }
    }
    else if (type == rttr::type::get<node::GroupExpression>())
    {
        auto& src = static_cast<const node::GroupExpression&>(front);
        auto& dst = static_cast<sop::node::GroupExpression&>(back);

        dst.SetGroupType(src.group_type);
        dst.ClearInstances();
        if (!src.inst0.expr_str.empty()) {
            dst.AddInstance(TransGroupExprInst(src.inst0));
        }
        if (!src.inst1.expr_str.empty()) {
            dst.AddInstance(TransGroupExprInst(src.inst1));
        }
        if (!src.inst2.expr_str.empty()) {
            dst.AddInstance(TransGroupExprInst(src.inst2));
        }
        if (!src.inst3.expr_str.empty()) {
            dst.AddInstance(TransGroupExprInst(src.inst3));
        }
    }
    else if (type == rttr::type::get<node::GroupPromote>())
    {
        auto& src = static_cast<const node::GroupPromote&>(front);
        auto& dst = static_cast<sop::node::GroupPromote&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetSrcGroupType(TransGroupTypes(src.src_type1));
        dst.SetDstGroupType(TransGroupTypes(src.dst_type1));
    }
    // manipulate
    else if (type == rttr::type::get<node::Delete>())
    {
        auto& src = static_cast<const node::Delete&>(front);
        auto& dst = static_cast<sop::node::Delete&>(back);

        dst.SetDelNonSelected(src.delete_non_selected);
        dst.SetEntityType(src.entity_type);
        dst.SetFilterExpr(src.filter_exp);
    }
    else if (type == rttr::type::get<node::Peak>())
    {
        auto& src = static_cast<const node::Peak&>(front);
        auto& dst = static_cast<sop::node::Peak&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetGroupType(src.group_type);

        dst.SetDistance(ParseExprFloat(src.distance, back,
            sop::node::Peak::DIST, 0, eval));

        dst.SetUpdateNorm(src.update_norm);
    }
    else if (type == rttr::type::get<node::Transform>())
    {
        auto& src = static_cast<const node::Transform&>(front);
        auto& dst = static_cast<sop::node::Transform&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetGroupType(src.group_type);

        dst.SetTranslate(ParseExprFloat3(src.translate, back, sop::node::Transform::TRANS, sm::vec3(0, 0, 0), eval));
        dst.SetRotate(ParseExprFloat3(src.rotate, back, sop::node::Transform::ROT, sm::vec3(0, 0, 0), eval));
        dst.SetScale(ParseExprFloat3(src.scale, back, sop::node::Transform::SCALE, sm::vec3(1, 1, 1), eval));
        dst.SetShear(ParseExprFloat3(src.shear, back, sop::node::Transform::SHEAR, sm::vec3(0, 0, 0), eval));
    }
    // material
    else if (type == rttr::type::get<node::Color>())
    {
        auto& src = static_cast<const node::Color&>(front);
        auto& dst = static_cast<sop::node::Color&>(back);
        dst.SetColor(src.color);
    }
    else if (type == rttr::type::get<node::UVQuickShade>())
    {
        auto& src = static_cast<const node::UVQuickShade&>(front);
        auto& dst = static_cast<sop::node::UVQuickShade&>(back);
        dst.SetImageFile(src.image_file);
    }
    else if (type == rttr::type::get<node::UVTransform>())
    {
        auto& src = static_cast<const node::UVTransform&>(front);
        auto& dst = static_cast<sop::node::UVTransform&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetGroupType(src.group_type);

        dst.SetTranslate(ParseExprFloat3(src.translate, back, sop::node::UVTransform::TRANS, sm::vec3(0, 0, 0), eval));
        dst.SetRotate(ParseExprFloat3(src.rotate, back, sop::node::UVTransform::ROT, sm::vec3(0, 0, 0), eval));
        dst.SetScale(ParseExprFloat3(src.scale, back, sop::node::UVTransform::SCALE, sm::vec3(1, 1, 1), eval));
        dst.SetShear(ParseExprFloat3(src.shear, back, sop::node::UVTransform::SHEAR, sm::vec3(0, 0, 0), eval));
    }
    // NURBs
    else if (type == rttr::type::get<node::Carve>())
    {
        auto& src = static_cast<const node::Carve&>(front);
        auto& dst = static_cast<sop::node::Carve&>(back);

        if (src.first_u_toggle) {
            dst.SetFirstU(ParseExprFloat(src.first_u, back,
                sop::node::Carve::FIRST_U, 0, eval));
        }
        if (src.second_u_toggle) {
            dst.SetSecondU(ParseExprFloat(src.second_u, back,
                sop::node::Carve::SECOND_U, 1, eval));
        }
        if (src.first_v_toggle) {
            dst.SetFirstV(ParseExprFloat(src.first_v, back,
                sop::node::Carve::FIRST_V, 0, eval));
        }
        if (src.second_v_toggle) {
            dst.SetSecondV(ParseExprFloat(src.second_v, back,
                sop::node::Carve::SECOND_V, 1, eval));
        }
    }
    else if (type == rttr::type::get<node::Add>())
    {
        auto& src = static_cast<const node::Add&>(front);
        auto& dst = static_cast<sop::node::Add&>(back);
        std::vector<sm::vec3> points;
        if (src.p0_toggle) {
            points.push_back(src.p0);
        }
        if (src.p1_toggle) {
            points.push_back(src.p1);
        }
        if (src.p2_toggle) {
            points.push_back(src.p2);
        }
        if (src.p3_toggle) {
            points.push_back(src.p3);
        }
        dst.SetPoints(points);
    }
    // polygon
    else if (type == rttr::type::get<node::Boolean>())
    {
        auto& src = static_cast<const node::Boolean&>(front);
        auto& dst = static_cast<sop::node::Boolean&>(back);
        dst.SetOperator(src.op);
    }
    else if (type == rttr::type::get<node::Fuse>())
    {
        auto& src = static_cast<const node::Fuse&>(front);
        auto& dst = static_cast<sop::node::Fuse&>(back);

        dst.SetFuseOP(src.op);
        dst.SetDistance(src.distance);
    }
    else if (type == rttr::type::get<node::Knife>())
    {
        auto& src = static_cast<const node::Knife&>(front);
        auto& dst = static_cast<sop::node::Knife&>(back);

        dst.SetOrigin(ParseExprFloat3(src.origin, back, sop::node::Knife::ORIGINX, sm::vec3(0, 0, 0), eval));
        dst.SetDirection(src.direction);
        dst.SetKeepType(src.knife_op);
    }
    else if (type == rttr::type::get<node::Normal>())
    {
        auto& src = static_cast<const node::Normal&>(front);
        auto& dst = static_cast<sop::node::Normal&>(back);
        dst.SetAttrAddTo(TransGeoAttrClassType(src.attr_add_norm_to));
    }
    else if (type == rttr::type::get<node::PolyExtrude>())
    {
        auto& src = static_cast<const node::PolyExtrude&>(front);
        auto& dst = static_cast<sop::node::PolyExtrude&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetDistance(src.distance);

        dst.SetOutputFront(src.output_front);
        dst.SetOutputBack(src.output_back);
        dst.SetOutputSide(src.output_side);

        if (src.front_group_toggle) {
            dst.SetFrontGroupName(src.front_group);
        }
        if (src.back_group_toggle) {
            dst.SetBackGroupName(src.back_group);
        }
        if (src.side_group_toggle) {
            dst.SetSideGroupName(src.side_group);
        }
    }
    else if (type == rttr::type::get<node::PolyFill>())
    {
        auto& src = static_cast<const node::PolyFill&>(front);
        auto& dst = static_cast<sop::node::PolyFill&>(back);
    }
    else if (type == rttr::type::get<node::PolyFrame>())
    {
        auto& src = static_cast<const node::PolyFrame&>(front);
        auto& dst = static_cast<sop::node::PolyFrame&>(back);

        dst.SetEntityType(src.entity_type);

        dst.SetFrameStyle(src.frame_style);

        dst.SetNormalName(src.normal_name);
        dst.SetTangentName(src.tangent_name);
        dst.SetBitangentName(src.bitangent_name);
    }
    // primitive
    else if (type == rttr::type::get<node::Box>())
    {
        auto& src = static_cast<const node::Box&>(front);
        auto& dst = static_cast<sop::node::Box&>(back);
        auto& dst_props = const_cast<sop::NodePropsMgr&>(dst.GetProps());

        dst.SetSize(ParseExprFloat3(src.size, back, sop::node::Box::SIZE, sm::vec3(1, 1, 1), eval));
        dst.SetCenter(ParseExprFloat3(src.center, back, sop::node::Box::POS, sm::vec3(0, 0, 0), eval));
        dst.SetScale(sm::vec3(src.scale, src.scale, src.scale));
    }
    else if (type == rttr::type::get<node::Curve>())
    {
        auto& src = static_cast<const node::Curve&>(front);
        auto& dst = static_cast<sop::node::Curve&>(back);
        dst.SetVertices(src.vertices);
    }
    else if (type == rttr::type::get<node::Grid>())
    {
        auto& src = static_cast<const node::Grid&>(front);
        auto& dst = static_cast<sop::node::Grid&>(back);

        sm::ivec2 sz;
        sz.x = static_cast<int>(src.size.x);
        sz.y = static_cast<int>(src.size.y);
        dst.SetSize(sz);

        dst.SetRows(src.rows);
        dst.SetColumns(src.columns);
    }
    else if (type == rttr::type::get<node::Line>())
    {
        auto& src = static_cast<const node::Line&>(front);
        auto& dst = static_cast<sop::node::Line&>(back);

        dst.SetOrigin(src.origin);
        dst.SetDirection(src.direction);
        dst.SetLength(ParseExprFloat(src.length, back,
            sop::node::Line::LENGTH, 1.0f, eval));
        dst.SetPoints(src.points);
    }
    else if (type == rttr::type::get<node::Primitive>())
    {
        auto& src = static_cast<const node::Primitive&>(front);
        auto& dst = static_cast<sop::node::Primitive&>(back);

        dst.SetGroupName(src.group_name.str);

        dst.SetTranslate(src.translate);
        dst.SetRotate(src.rotate);
        dst.SetScale(src.scale);
        dst.SetShear(src.shear);
    }
    // primitive extern
    else if (type == rttr::type::get<node::Dungeon>())
    {
        auto& src = static_cast<const node::Dungeon&>(front);
        auto& dst = static_cast<sop::node::Dungeon&>(back);

        dst.SetSize(src.size);
        dst.SetSplitSize(src.split_sz);
        dst.SetMinSize(src.min_sz);

        dst.SetSeed(src.seed);
    }
    // utility
    else if (type == rttr::type::get<node::Blast>())
    {
        auto& src = static_cast<const node::Blast&>(front);
        auto& dst = static_cast<sop::node::Blast&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetGroupType(src.group_type);
        dst.SetDeleteNonSelected(src.del_non_selected);
    }
    else if (type == rttr::type::get<node::CopyToPoints>())
    {
        auto& src = static_cast<const node::CopyToPoints&>(front);
        auto& dst = static_cast<sop::node::CopyToPoints&>(back);

        dst.SetSrcGroup(src.src_group.str);

        if (!src.target_group.str.empty()) {
            dst.SetTargetGroup(src.target_group.str);
        } else {
            dst.SetTargetGroup(src.target_group_str);
        }

        dst.EnableUsePointDir(src.trans_dir);
    }
    else if (type == rttr::type::get<node::ForeachEnd>())
    {
        auto& src = static_cast<const node::ForeachEnd&>(front);
        auto& dst = static_cast<sop::node::ForeachPrimEnd&>(back);

        dst.EnableSinglePass(src.do_single_pass);
        dst.SetSinglePassOffset(src.single_pass_offset);
    }
    else if (type == rttr::type::get<node::Python>())
    {
        auto& src = static_cast<const node::Python&>(front);
        auto& dst = static_cast<sop::node::Python&>(back);
        dst.SetCode(src.code);
    }
    else if (type == rttr::type::get<node::Split>())
    {
        auto& src = static_cast<const node::Split&>(front);
        auto& dst = static_cast<sop::node::Split&>(back);
        dst.SetGroupName(src.group_name.str);
    }
    else if (type == rttr::type::get<node::Switch>())
    {
        auto& src = static_cast<const node::Switch&>(front);
        auto& dst = static_cast<sop::node::Switch&>(back);
        dst.SetSelected(ParseExprInt(src.selected, back,
            sop::node::Switch::SELECTED, 0, eval));
    }

    // update props
    if (type.is_derived_from<Node>())
    {
        auto& src = static_cast<const Node&>(front);
        auto& src_props = src.GetProps();
        if (src_props)
        {
            auto& dst_props = const_cast<sop::NodePropsMgr&>(back.GetProps());
            dst_props.Clear();
            for (auto& sp : src_props->props)
            {
                sop::Variable d_val;
                try {
                    d_val.type = sop::VarType::Float;
                    d_val.f = boost::lexical_cast<float>(sp.value);
                } catch (boost::bad_lexical_cast&) {
                    continue;
                }
                dst_props.Add(sp.name, d_val);
            }
        }
    }
}

sop::NodePtr SOPAdapter::CreateBackFromFront(const bp::Node& node)
{
    auto type = node.get_type();
    auto src_type = type.get_name().to_string();
    std::string dst_type;
    std::string lib_str = "sop";
    auto find_lib = src_type.find("sopv::");
    if (find_lib != std::string::npos) {
        dst_type = lib_str + "::" + src_type.substr(find_lib + strlen("sopv::"));
    }

    sop::NodePtr dst = nullptr;

    if (!dst_type.empty())
    {
	    rttr::type t = rttr::type::get_by_name(dst_type);
        // fixme: specify node type
	    if (!t.is_valid())
        {
            assert(0);

            //dst = std::make_shared<sop::Node>();
            //InitPortsBackFromFront(*dst, *node);
	    }
        else
        {
            rttr::variant var = t.create();
            assert(var.is_valid());

            dst = var.get_value<std::shared_ptr<sop::Node>>();
            assert(dst);
        }
    }

    if (node.get_type().is_derived_from<Node>()) {
        dst->SetName(static_cast<const Node&>(node).GetName());
    }

    return dst;
}

int SOPAdapter::TypeBackToFront(sop::NodeVarType type)
{
    int ret = -1;

    switch (type)
    {
    case sop::NodeVarType::Any:
        ret = bp::PIN_ANY_VAR;
        break;
    case sop::NodeVarType::Port:
        ret = bp::PIN_PORT;
        break;
    case sop::NodeVarType::Primitive:
        ret = PIN_PRIMITIVE;
        break;
    }

    return ret;
}

sop::NodeVarType SOPAdapter::TypeFrontToBack(int pin_type)
{
    sop::NodeVarType ret = sop::NodeVarType::Any;

    switch (pin_type)
    {
    case bp::PIN_ANY_VAR:
        ret = sop::NodeVarType::Any;
        break;
    case bp::PIN_PORT:
        ret = sop::NodeVarType::Port;
        break;
    case PIN_PRIMITIVE:
        ret = sop::NodeVarType::Primitive;
        break;
    }

    return ret;
}

sop::GeoAttrClass
SOPAdapter::TransGeoAttrClass(sop::GeoAttrClass cls)
{
    switch (cls)
    {
    case sop::GeoAttrClass::Point:
        return sop::GeoAttrClass::Point;
    case sop::GeoAttrClass::Vertex:
        return sop::GeoAttrClass::Vertex;
    case sop::GeoAttrClass::Primitive:
        return sop::GeoAttrClass::Primitive;
    case sop::GeoAttrClass::Detail:
        return sop::GeoAttrClass::Detail;
    default:
        assert(0);
        return sop::GeoAttrClass::Point;
    }
}

int SOPAdapter::ParseExprInt(const std::string& src, const sop::Node& dst,
                      size_t idx, int expect, const Evaluator& eval)
{
    int ret = expect;

    auto& dst_props = const_cast<sop::NodePropsMgr&>(dst.GetProps());
    try {
        ret = boost::lexical_cast<int>(src);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx, src);
        ret = eval.CalcInt(src, dst, 0);
    }

    return ret;
}

float SOPAdapter::ParseExprFloat(const std::string& src, const sop::Node& dst,
                          size_t idx, float expect, const Evaluator& eval)
{
    float ret = expect;

    auto& dst_props = const_cast<sop::NodePropsMgr&>(dst.GetProps());
    try {
        ret = boost::lexical_cast<float>(src);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx, src);
        ret = eval.CalcFloat(src, dst, expect);
    }

    return ret;
}

sm::vec3 SOPAdapter::ParseExprFloat3(const StrVec3& src, const sop::Node& dst,
                              size_t idx, const sm::vec3& expect, const Evaluator& eval)
{
    sm::vec3 ret = expect;

    auto& dst_props = const_cast<sop::NodePropsMgr&>(dst.GetProps());
    try {
        ret.x = boost::lexical_cast<float>(src.x);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx, src.x, 0);
        ret.x = eval.CalcFloat(src.x, dst, 1.0f);
    }
    try {
        ret.y = boost::lexical_cast<float>(src.y);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx, src.y, 1);
        ret.y = eval.CalcFloat(src.y, dst, 1.0f);
    }
    try {
        ret.z = boost::lexical_cast<float>(src.z);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx, src.z, 2);
        ret.z = eval.CalcFloat(src.z, dst, 1.0f);
    }

    return ret;
}

}