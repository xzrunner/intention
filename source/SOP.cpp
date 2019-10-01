#include "sopview/SOP.h"
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
#include <sop/node/Sort.h>
#include <sop/node/Measure.h>
// group
#include <sop/node/GroupCreate.h>
#include <sop/node/GroupExpression.h>
// manipulate
#include <sop/node/Delete.h>
#include <sop/node/Transform.h>
// nurbs
#include <sop/node/Carve.h>
// polygon
#include <sop/node/Add.h>
#include <sop/node/Boolean.h>
#include <sop/node/Fuse.h>
#include <sop/node/Knife.h>
#include <sop/node/Normal.h>
#include <sop/node/PolyExtrude.h>
// primitive
#include <sop/node/Box.h>
#include <sop/node/Curve.h>
#include <sop/node/Line.h>
#include <sop/node/Primitive.h>
#include <sop/node/Sphere.h>
// utility
#include <sop/node/Blast.h>
#include <sop/node/CopyToPoints.h>
#include <sop/node/ForeachPrimEnd.h>
#include <sop/node/Switch.h>

#include <boost/lexical_cast.hpp>

namespace
{

sop::GroupType TransGroupType(sopv::GroupType type)
{
    switch (type)
    {
    case sopv::GroupType::GuessFromGroup:
        return sop::GroupType::GuessFromGroup;
    case sopv::GroupType::Primitives:
        return sop::GroupType::Primitives;
    case sopv::GroupType::Points:
        return sop::GroupType::Points;
    case sopv::GroupType::Edges:
        return sop::GroupType::Edges;
    case sopv::GroupType::Vertices:
        return sop::GroupType::Vertices;
    default:
        assert(0);
        return sop::GroupType::Primitives;
    }
}

sop::GroupMerge TransGroupMerge(sopv::GroupMerge merge_op)
{

    switch (merge_op)
    {
    case sopv::GroupMerge::Replace:
        return sop::GroupMerge::Replace;
    case sopv::GroupMerge::Union:
        return sop::GroupMerge::Union;
    case sopv::GroupMerge::Intersect:
        return sop::GroupMerge::Intersect;
    case sopv::GroupMerge::Subtract:
        return sop::GroupMerge::Subtract;
    default:
        assert(0);
        return sop::GroupMerge::Replace;
    }
}

sop::node::GroupExpression::Instance TransGroupExprInst(const sopv::GroupExprInst& src)
{
    sop::node::GroupExpression::Instance dst;
    dst.group_name = src.group_name;
    dst.expr_str   = src.expr_str;
    dst.merge_op   = TransGroupMerge(src.merge_op);
    return dst;
}

}

namespace sopv
{

void SOP::UpdatePropBackFromFront(const bp::Node& front, sop::Node& back,
                                         const Evaluator& eval)
{
    auto type = front.get_type();
    // attribute
    if (type == rttr::type::get<node::Measure>())
    {
        auto& src = static_cast<const node::Measure&>(front);
        auto& dst = static_cast<sop::node::Measure&>(back);

        sop::node::Measure::Type type;
        switch (src.ms_type)
        {
        case MeasureType::Perimeter:
            type = sop::node::Measure::Type::Perimeter;
            break;
        case MeasureType::Area:
            type = sop::node::Measure::Type::Area;
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
        auto& dst = static_cast<sop::node::Sort&>(back);

        sop::node::Sort::Key key;
        switch (src.key)
        {
        case SortKey::NoChange:
            key = sop::node::Sort::Key::NoChange;
            break;
        case SortKey::X:
            key = sop::node::Sort::Key::X;
            break;
        case SortKey::Y:
            key = sop::node::Sort::Key::Y;
            break;
        case SortKey::Z:
            key = sop::node::Sort::Key::Z;
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
        auto& dst = static_cast<sop::node::GroupCreate&>(back);

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
        auto& dst = static_cast<sop::node::GroupExpression&>(back);

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
        auto& dst = static_cast<sop::node::Delete&>(back);

        dst.SetDelNonSelected(src.delete_non_selected);

        sop::node::Delete::EntityType type;
        switch (src.entity_type)
        {
        case DeleteEntityType::Points:
            type = sop::node::Delete::EntityType::Points;
            break;
        case DeleteEntityType::Edges:
            type = sop::node::Delete::EntityType::Edges;
            break;
        case DeleteEntityType::Faces:
            type = sop::node::Delete::EntityType::Faces;
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
        auto& dst = static_cast<sop::node::Transform&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetGroupType(TransGroupType(src.group_type));

        sm::ivec3 trans_idx(sop::node::Transform::TRANS_X, sop::node::Transform::TRANS_Y, sop::node::Transform::TRANS_Z);
        dst.SetTranslate(ParseExprFloat3(src.translate, back, trans_idx, sm::vec3(0, 0, 0), eval));

        sm::ivec3 rot_idx(sop::node::Transform::ROT_X, sop::node::Transform::ROT_Y, sop::node::Transform::ROT_Z);
        dst.SetRotate(ParseExprFloat3(src.rotate, back, rot_idx, sm::vec3(0, 0, 0), eval));

        sm::ivec3 scale_idx(sop::node::Transform::SCALE_X, sop::node::Transform::SCALE_Y, sop::node::Transform::SCALE_Z);
        dst.SetScale(ParseExprFloat3(src.scale, back, scale_idx, sm::vec3(1, 1, 1), eval));

        sm::ivec3 shear_idx(sop::node::Transform::SHEAR_X, sop::node::Transform::SHEAR_Y, sop::node::Transform::SHEAR_Z);
        dst.SetShear(ParseExprFloat3(src.shear, back, shear_idx, sm::vec3(0, 0, 0), eval));
    }
    // NURBs
    else if (type == rttr::type::get<node::Carve>())
    {
        auto& src = static_cast<const node::Carve&>(front);
        auto& dst = static_cast<sop::node::Carve&>(back);

        dst.SetFirstU(ParseExprFloat(src.first_u, back,
            sop::node::Carve::FIRST_U, 0, eval));
        dst.SetSecondU(ParseExprFloat(src.second_u, back,
            sop::node::Carve::SECOND_U, 1, eval));
        dst.SetFirstV(ParseExprFloat(src.first_v, back,
            sop::node::Carve::FIRST_V, 0, eval));
        dst.SetSecondV(ParseExprFloat(src.second_v, back,
            sop::node::Carve::SECOND_V, 1, eval));
    }
    else if (type == rttr::type::get<node::Add>())
    {
        auto& src = static_cast<const node::Add&>(front);
        auto& dst = static_cast<sop::node::Add&>(back);
        dst.SetPoints(src.points);
    }
    // polygon
    else if (type == rttr::type::get<node::Boolean>())
    {
        auto& src = static_cast<const node::Boolean&>(front);
        auto& dst = static_cast<sop::node::Boolean&>(back);

        sop::node::Boolean::Operator op;
        switch (src.op)
        {
        case BooleanOperator::Union:
            op = sop::node::Boolean::Operator::Union;
            break;
        case BooleanOperator::Intersect:
            op = sop::node::Boolean::Operator::Intersect;
            break;
        case BooleanOperator::Subtract:
            op = sop::node::Boolean::Operator::Subtract;
            break;
        default:
            assert(0);
        }
        dst.SetOperator(op);
    }
    else if (type == rttr::type::get<node::Fuse>())
    {
        auto& src = static_cast<const node::Fuse&>(front);
        auto& dst = static_cast<sop::node::Fuse&>(back);

        dst.SetDistance(src.distance);
    }
    else if (type == rttr::type::get<node::Knife>())
    {
        auto& src = static_cast<const node::Knife&>(front);
        auto& dst = static_cast<sop::node::Knife&>(back);

        dst.SetOrigin(src.origin);
        dst.SetDirection(src.direction);

        sop::node::Knife::KeepType keep;
        switch (src.keep)
        {
        case KnifeKeep::KeepAbove:
            keep = sop::node::Knife::KeepType::KeepAbove;
            break;
        case KnifeKeep::KeepBelow:
            keep = sop::node::Knife::KeepType::KeepBelow;
            break;
        case KnifeKeep::KeepAll:
            keep = sop::node::Knife::KeepType::KeepAll;
            break;
        default:
            assert(0);
        }
        dst.SetKeepType(keep);
    }
    else if (type == rttr::type::get<node::Normal>())
    {
        auto& src = static_cast<const node::Normal&>(front);
        auto& dst = static_cast<sop::node::Normal&>(back);

        sop::GeoAttrType type;
        switch (src.attr_add_norm_to)
        {
        case GeoAttrType::Point:
            type = sop::GeoAttrType::Point;
            break;
        case GeoAttrType::Vertex:
            type = sop::GeoAttrType::Vertex;
            break;
        case GeoAttrType::Primitive:
            type = sop::GeoAttrType::Primitive;
            break;
        case GeoAttrType::Detail:
            type = sop::GeoAttrType::Detail;
            break;
        default:
            assert(0);
        }

        dst.SetAttrAddTo(type);
    }
    else if (type == rttr::type::get<node::PolyExtrude>())
    {
        auto& src = static_cast<const node::PolyExtrude&>(front);
        auto& dst = static_cast<sop::node::PolyExtrude&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetDistance(src.distance);
    }
    // primitive
    else if (type == rttr::type::get<node::Box>())
    {
        auto& src = static_cast<const node::Box&>(front);
        auto& dst = static_cast<sop::node::Box&>(back);
        auto& dst_props = const_cast<sop::NodePropsMgr&>(dst.GetProps());

        sm::ivec3 size_idx(sop::node::Box::SIZE_X, sop::node::Box::SIZE_Y, sop::node::Box::SIZE_Z);
        dst.SetSize(ParseExprFloat3(src.size, back, size_idx, sm::vec3(1, 1, 1), eval));

        sm::ivec3 pos_idx(sop::node::Box::POS_X, sop::node::Box::POS_Y, sop::node::Box::POS_Z);
        dst.SetCenter(ParseExprFloat3(src.center, back, pos_idx, sm::vec3(0, 0, 0), eval));

        dst.SetScale(sm::vec3(src.scale, src.scale, src.scale));
    }
    else if (type == rttr::type::get<node::Curve>())
    {
        auto& src = static_cast<const node::Curve&>(front);
        auto& dst = static_cast<sop::node::Curve&>(back);
        dst.SetVertices(src.vertices);
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

        dst.SetTranslate(src.translate);
        dst.SetRotate(src.rotate);
        dst.SetScale(src.scale);
        dst.SetShear(src.shear);
    }
    // utility
    else if (type == rttr::type::get<node::Blast>())
    {
        auto& src = static_cast<const node::Blast&>(front);
        auto& dst = static_cast<sop::node::Blast&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetGroupType(TransGroupType(src.group_type));
        dst.SetDeleteNonSelected(src.delete_non_selected);
    }
    else if (type == rttr::type::get<node::CopyToPoints>())
    {
        auto& src = static_cast<const node::CopyToPoints&>(front);
        auto& dst = static_cast<sop::node::CopyToPoints&>(back);

        dst.SetSrcGroup(src.src_group.str);
        dst.SetTargetGroup(src.target_group.str);
        dst.EnableUsePointDir(src.use_pt_dir);
    }
    else if (type == rttr::type::get<node::ForeachPrimEnd>())
    {
        auto& src = static_cast<const node::ForeachPrimEnd&>(front);
        auto& dst = static_cast<sop::node::ForeachPrimEnd&>(back);

        dst.EnableSinglePass(src.do_single_pass);
        dst.SetSinglePassOffset(src.single_pass_offset);
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

sop::NodePtr SOP::CreateBackFromFront(const bp::Node& node)
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

int SOP::TypeBackToFront(sop::NodeVarType type)
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

sop::NodeVarType SOP::TypeFrontToBack(int pin_type)
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

int SOP::ParseExprInt(const std::string& src, const sop::Node& dst,
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

float SOP::ParseExprFloat(const std::string& src, const sop::Node& dst,
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

sm::vec3 SOP::ParseExprFloat3(const StrVec3& src, const sop::Node& dst,
                                     const sm::ivec3& idx, const sm::vec3& expect,
                                     const Evaluator& eval)
{
    sm::vec3 ret = expect;

    auto& dst_props = const_cast<sop::NodePropsMgr&>(dst.GetProps());
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