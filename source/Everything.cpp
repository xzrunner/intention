#include "intention/Everything.h"
#include "intention/PinType.h"
#include "intention/Evaluator.h"
#include "intention/Node.h"
#include "intention/RegistNodes.h"
#include "intention/NodeProp.h"

#include <blueprint/Pin.h>
#include <blueprint/Node.h>
#include <blueprint/Connecting.h>
#include <blueprint/node/Function.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>

// attribute
#include <everything/node/Sort.h>
#include <everything/node/Measure.h>
// group
#include <everything/node/GroupCreate.h>
#include <everything/node/GroupExpression.h>
// manipulate
#include <everything/node/Delete.h>
#include <everything/node/Transform.h>
// nurbs
#include <everything/node/Carve.h>
// polygon
#include <everything/node/Add.h>
#include <everything/node/Boolean.h>
#include <everything/node/Fuse.h>
#include <everything/node/Knife.h>
#include <everything/node/Normal.h>
#include <everything/node/PolyExtrude.h>
// primitive
#include <everything/node/Box.h>
#include <everything/node/Curve.h>
#include <everything/node/Line.h>
#include <everything/node/Sphere.h>
// utility
#include <everything/node/Blast.h>
#include <everything/node/CopyToPoints.h>
#include <everything/node/ForeachPrimEnd.h>
#include <everything/node/Switch.h>

#include <boost/lexical_cast.hpp>

namespace
{

evt::GroupType TransGroupType(itt::GroupType type)
{
    switch (type)
    {
    case itt::GroupType::GuessFromGroup:
        return evt::GroupType::GuessFromGroup;
    case itt::GroupType::Primitives:
        return evt::GroupType::Primitives;
    case itt::GroupType::Points:
        return evt::GroupType::Points;
    case itt::GroupType::Edges:
        return evt::GroupType::Edges;
    case itt::GroupType::Vertices:
        return evt::GroupType::Vertices;
    default:
        assert(0);
        return evt::GroupType::Primitives;
    }
}

evt::GroupMerge TransGroupMerge(itt::GroupMerge merge_op)
{

    switch (merge_op)
    {
    case itt::GroupMerge::Replace:
        return evt::GroupMerge::Replace;
    case itt::GroupMerge::Union:
        return evt::GroupMerge::Union;
    case itt::GroupMerge::Intersect:
        return evt::GroupMerge::Intersect;
    case itt::GroupMerge::Subtract:
        return evt::GroupMerge::Subtract;
    default:
        assert(0);
        return evt::GroupMerge::Replace;
    }
}

evt::node::GroupExpression::Instance TransGroupExprInst(const itt::GroupExprInst& src)
{
    evt::node::GroupExpression::Instance dst;
    dst.group_name = src.group_name;
    dst.expr_str   = src.expr_str;
    dst.merge_op   = TransGroupMerge(src.merge_op);
    return dst;
}

}

namespace itt
{

void Everything::UpdatePropBackFromFront(const bp::Node& front, evt::Node& back,
                                         const Evaluator& eval)
{
    auto type = front.get_type();
    // attribute
    if (type == rttr::type::get<node::Measure>())
    {
        auto& src = static_cast<const node::Measure&>(front);
        auto& dst = static_cast<evt::node::Measure&>(back);

        evt::node::Measure::Type type;
        switch (src.ms_type)
        {
        case MeasureType::Perimeter:
            type = evt::node::Measure::Type::Perimeter;
            break;
        case MeasureType::Area:
            type = evt::node::Measure::Type::Area;
            break;
        default:
            assert(0);
        }
        dst.SetMesureType(type);

        dst.SetMesureName(src.ms_name);
    }
    else if (type == rttr::type::get<node::Sort>())
    {
        auto& src = static_cast<const node::Sort&>(front);
        auto& dst = static_cast<evt::node::Sort&>(back);

        evt::node::Sort::Key key;
        switch (src.key)
        {
        case SortKey::NoChange:
            key = evt::node::Sort::Key::NoChange;
            break;
        case SortKey::X:
            key = evt::node::Sort::Key::X;
            break;
        case SortKey::Y:
            key = evt::node::Sort::Key::Y;
            break;
        case SortKey::Z:
            key = evt::node::Sort::Key::Z;
            break;
        default:
            assert(0);
        }
        dst.SetKey(key);
    }
    // group
    else if (type == rttr::type::get<node::GroupCreate>())
    {
        auto& src = static_cast<const node::GroupCreate&>(front);
        auto& dst = static_cast<evt::node::GroupCreate&>(back);

        dst.SetGroupName(src.group_name);
        dst.SetGroupType(TransGroupType(src.group_type));
        dst.SetGroupMerge(TransGroupMerge(src.merge_op));

        if (src.base_group) {
            dst.EnableBaseGroup(src.base_group_expr);
        } else {
            dst.DisableBaseGroup();
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
        auto& dst = static_cast<evt::node::GroupExpression&>(back);

        dst.SetGroupType(TransGroupType(src.group_type));
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
    // manipulate
    else if (type == rttr::type::get<node::Delete>())
    {
        auto& src = static_cast<const node::Delete&>(front);
        auto& dst = static_cast<evt::node::Delete&>(back);

        dst.SetDelNonSelected(src.delete_non_selected);

        evt::node::Delete::EntityType type;
        switch (src.entity_type)
        {
        case DeleteEntityType::Points:
            type = evt::node::Delete::EntityType::Points;
            break;
        case DeleteEntityType::Edges:
            type = evt::node::Delete::EntityType::Edges;
            break;
        case DeleteEntityType::Faces:
            type = evt::node::Delete::EntityType::Faces;
            break;
        default:
            assert(0);
        }
        dst.SetEntityType(type);

        dst.SetFilterExpr(src.filter_exp);
    }
    else if (type == rttr::type::get<node::Transform>())
    {
        auto& src = static_cast<const node::Transform&>(front);
        auto& dst = static_cast<evt::node::Transform&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetGroupType(TransGroupType(src.group_type));

        sm::ivec3 trans_idx(evt::node::Transform::TRANS_X, evt::node::Transform::TRANS_Y, evt::node::Transform::TRANS_Z);
        dst.SetTranslate(ParseExprFloat3(src.translate, back, trans_idx, sm::vec3(0, 0, 0), eval));

        sm::ivec3 rot_idx(evt::node::Transform::ROT_X, evt::node::Transform::ROT_Y, evt::node::Transform::ROT_Z);
        dst.SetRotate(ParseExprFloat3(src.rotate, back, rot_idx, sm::vec3(0, 0, 0), eval));

        sm::ivec3 scale_idx(evt::node::Transform::SCALE_X, evt::node::Transform::SCALE_Y, evt::node::Transform::SCALE_Z);
        dst.SetScale(ParseExprFloat3(src.scale, back, scale_idx, sm::vec3(1, 1, 1), eval));

        sm::ivec3 shear_idx(evt::node::Transform::SHEAR_X, evt::node::Transform::SHEAR_Y, evt::node::Transform::SHEAR_Z);
        dst.SetShear(ParseExprFloat3(src.shear, back, shear_idx, sm::vec3(0, 0, 0), eval));
    }
    // NURBs
    else if (type == rttr::type::get<node::Carve>())
    {
        auto& src = static_cast<const node::Carve&>(front);
        auto& dst = static_cast<evt::node::Carve&>(back);

        dst.SetFirstU(ParseExprFloat(src.first_u, back,
            evt::node::Carve::FIRST_U, 0, eval));
        dst.SetSecondU(ParseExprFloat(src.second_u, back,
            evt::node::Carve::SECOND_U, 1, eval));
        dst.SetFirstV(ParseExprFloat(src.first_v, back,
            evt::node::Carve::FIRST_V, 0, eval));
        dst.SetSecondV(ParseExprFloat(src.second_v, back,
            evt::node::Carve::SECOND_V, 1, eval));
    }
    else if (type == rttr::type::get<node::Add>())
    {
        auto& src = static_cast<const node::Add&>(front);
        auto& dst = static_cast<evt::node::Add&>(back);
        dst.SetPoints(src.points);
    }
    // polygon
    else if (type == rttr::type::get<node::Boolean>())
    {
        auto& src = static_cast<const node::Boolean&>(front);
        auto& dst = static_cast<evt::node::Boolean&>(back);

        evt::node::Boolean::Operator op;
        switch (src.op)
        {
        case BooleanOperator::Union:
            op = evt::node::Boolean::Operator::Union;
            break;
        case BooleanOperator::Intersect:
            op = evt::node::Boolean::Operator::Intersect;
            break;
        case BooleanOperator::Subtract:
            op = evt::node::Boolean::Operator::Subtract;
            break;
        default:
            assert(0);
        }
        dst.SetOperator(op);
    }
    else if (type == rttr::type::get<node::Fuse>())
    {
        auto& src = static_cast<const node::Fuse&>(front);
        auto& dst = static_cast<evt::node::Fuse&>(back);

        dst.SetDistance(src.distance);
    }
    else if (type == rttr::type::get<node::Knife>())
    {
        auto& src = static_cast<const node::Knife&>(front);
        auto& dst = static_cast<evt::node::Knife&>(back);

        dst.SetOrigin(src.origin);
        dst.SetDirection(src.direction);

        evt::node::Knife::KeepType keep;
        switch (src.keep)
        {
        case KnifeKeep::KeepAbove:
            keep = evt::node::Knife::KeepType::KeepAbove;
            break;
        case KnifeKeep::KeepBelow:
            keep = evt::node::Knife::KeepType::KeepBelow;
            break;
        case KnifeKeep::KeepAll:
            keep = evt::node::Knife::KeepType::KeepAll;
            break;
        default:
            assert(0);
        }
        dst.SetKeepType(keep);
    }
    else if (type == rttr::type::get<node::Normal>())
    {
        auto& src = static_cast<const node::Normal&>(front);
        auto& dst = static_cast<evt::node::Normal&>(back);

        evt::GeoAttrType type;
        switch (src.attr_add_norm_to)
        {
        case GeoAttrType::Point:
            type = evt::GeoAttrType::Point;
            break;
        case GeoAttrType::Vertex:
            type = evt::GeoAttrType::Vertex;
            break;
        case GeoAttrType::Primitive:
            type = evt::GeoAttrType::Primitive;
            break;
        case GeoAttrType::Detail:
            type = evt::GeoAttrType::Detail;
            break;
        default:
            assert(0);
        }

        dst.SetAttrAddTo(type);
    }
    else if (type == rttr::type::get<node::PolyExtrude>())
    {
        auto& src = static_cast<const node::PolyExtrude&>(front);
        auto& dst = static_cast<evt::node::PolyExtrude&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetDistance(src.distance);
    }
    // primitive
    else if (type == rttr::type::get<node::Box>())
    {
        auto& src = static_cast<const node::Box&>(front);
        auto& dst = static_cast<evt::node::Box&>(back);
        auto& dst_props = const_cast<evt::NodePropsMgr&>(dst.GetProps());

        sm::ivec3 size_idx(evt::node::Box::SIZE_X, evt::node::Box::SIZE_Y, evt::node::Box::SIZE_Z);
        dst.SetSize(ParseExprFloat3(src.size, back, size_idx, sm::vec3(1, 1, 1), eval));

        sm::ivec3 pos_idx(evt::node::Box::POS_X, evt::node::Box::POS_Y, evt::node::Box::POS_Z);
        dst.SetCenter(ParseExprFloat3(src.center, back, pos_idx, sm::vec3(0, 0, 0), eval));

        dst.SetScale(sm::vec3(src.scale, src.scale, src.scale));
    }
    else if (type == rttr::type::get<node::Curve>())
    {
        auto& src = static_cast<const node::Curve&>(front);
        auto& dst = static_cast<evt::node::Curve&>(back);
        dst.SetVertices(src.vertices);
    }
    else if (type == rttr::type::get<node::Line>())
    {
        auto& src = static_cast<const node::Line&>(front);
        auto& dst = static_cast<evt::node::Line&>(back);

        dst.SetOrigin(src.origin);
        dst.SetDirection(src.direction);
        dst.SetLength(src.length);
        dst.SetPoints(src.points);
    }
    // utility
    else if (type == rttr::type::get<node::Blast>())
    {
        auto& src = static_cast<const node::Blast&>(front);
        auto& dst = static_cast<evt::node::Blast&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetGroupType(TransGroupType(src.group_type));
        dst.SetDeleteNonSelected(src.delete_non_selected);
    }
    else if (type == rttr::type::get<node::CopyToPoints>())
    {
        auto& src = static_cast<const node::CopyToPoints&>(front);
        auto& dst = static_cast<evt::node::CopyToPoints&>(back);

        dst.EnableUsePointDir(src.use_pt_dir);
    }
    else if (type == rttr::type::get<node::ForeachPrimEnd>())
    {
        auto& src = static_cast<const node::ForeachPrimEnd&>(front);
        auto& dst = static_cast<evt::node::ForeachPrimEnd&>(back);

        dst.EnableSinglePass(src.do_single_pass);
        dst.SetSinglePassOffset(src.single_pass_offset);
    }
    else if (type == rttr::type::get<node::Switch>())
    {
        auto& src = static_cast<const node::Switch&>(front);
        auto& dst = static_cast<evt::node::Switch&>(back);
        dst.SetSelected(ParseExprInt(src.selected, back,
            evt::node::Switch::SELECTED, 0, eval));
    }

    // update props
    if (type.is_derived_from<Node>())
    {
        auto& src = static_cast<const Node&>(front);
        auto& src_props = src.GetProps();
        if (src_props)
        {
            auto& dst_props = const_cast<evt::NodePropsMgr&>(back.GetProps());
            dst_props.Clear();
            for (auto& sp : src_props->props)
            {
                evt::Variable d_val;
                try {
                    d_val.type = evt::VarType::Float;
                    d_val.f = boost::lexical_cast<float>(sp.value);
                } catch (boost::bad_lexical_cast&) {
                    continue;
                }
                dst_props.Add(sp.name, d_val);
            }
        }
    }
}

evt::NodePtr Everything::CreateBackFromFront(const bp::Node& node)
{
    auto type = node.get_type();
    auto src_type = type.get_name().to_string();
    std::string dst_type;
    std::string lib_str = "evt";
    auto find_lib = src_type.find("itt::");
    if (find_lib != std::string::npos) {
        dst_type = lib_str + "::" + src_type.substr(find_lib + strlen("itt::"));
    }

    evt::NodePtr dst = nullptr;

    if (!dst_type.empty())
    {
	    rttr::type t = rttr::type::get_by_name(dst_type);
        // fixme: specify node type
	    if (!t.is_valid())
        {
            assert(0);

            //dst = std::make_shared<evt::Node>();
            //InitPortsBackFromFront(*dst, *node);
	    }
        else
        {
            rttr::variant var = t.create();
            assert(var.is_valid());

            dst = var.get_value<std::shared_ptr<evt::Node>>();
            assert(dst);
        }
    }

    if (node.get_type().is_derived_from<Node>()) {
        dst->SetName(static_cast<const Node&>(node).GetName());
    }

    return dst;
}

int Everything::TypeBackToFront(evt::NodeVarType type)
{
    int ret = -1;

    switch (type)
    {
    case evt::NodeVarType::Any:
        ret = bp::PIN_ANY_VAR;
        break;
    case evt::NodeVarType::Port:
        ret = bp::PIN_PORT;
        break;
    case evt::NodeVarType::Primitive:
        ret = PIN_PRIMITIVE;
        break;
    }

    return ret;
}

evt::NodeVarType Everything::TypeFrontToBack(int pin_type)
{
    evt::NodeVarType ret = evt::NodeVarType::Any;

    switch (pin_type)
    {
    case bp::PIN_ANY_VAR:
        ret = evt::NodeVarType::Any;
        break;
    case bp::PIN_PORT:
        ret = evt::NodeVarType::Port;
        break;
    case PIN_PRIMITIVE:
        ret = evt::NodeVarType::Primitive;
        break;
    }

    return ret;
}

int Everything::ParseExprInt(const std::string& src, const evt::Node& dst,
                             size_t idx, int expect, const Evaluator& eval)
{
    int ret = expect;

    auto& dst_props = const_cast<evt::NodePropsMgr&>(dst.GetProps());
    try {
        ret = boost::lexical_cast<int>(src);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx, src);
        ret = eval.CalcInt(src, dst, 0);
    }

    return ret;
}

float Everything::ParseExprFloat(const std::string& src, const evt::Node& dst,
                                 size_t idx, float expect, const Evaluator& eval)
{
    float ret = expect;

    auto& dst_props = const_cast<evt::NodePropsMgr&>(dst.GetProps());
    try {
        ret = boost::lexical_cast<float>(src);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx, src);
        ret = eval.CalcFloat(src, dst, expect);
    }

    return ret;
}

sm::vec3 Everything::ParseExprFloat3(const StrVec3& src, const evt::Node& dst,
                                     const sm::ivec3& idx, const sm::vec3& expect,
                                     const Evaluator& eval)
{
    sm::vec3 ret = expect;

    auto& dst_props = const_cast<evt::NodePropsMgr&>(dst.GetProps());
    try {
        ret.x = boost::lexical_cast<float>(src.x);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx.x, src.x);
        ret.x = eval.CalcFloat(src.x, dst, 1.0f);
    }
    try {
        ret.y = boost::lexical_cast<float>(src.y);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx.y, src.y);
        ret.y = eval.CalcFloat(src.y, dst, 1.0f);
    }
    try {
        ret.z = boost::lexical_cast<float>(src.z);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx.z, src.z);
        ret.z = eval.CalcFloat(src.z, dst, 1.0f);
    }

    return ret;
}

}