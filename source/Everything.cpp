#include "intention/Everything.h"
#include "intention/PinType.h"
#include "intention/Evaluator.h"
#include "intention/Node.h"
#include "intention/RegistNodes.h"
#include "intention/NodeHelper.h"

#include <blueprint/Pin.h>
#include <blueprint/Node.h>
#include <blueprint/Connecting.h>
#include <blueprint/node/Function.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>

// attribute
#include <everything/node/Sort.h>
// manipulate
#include <everything/node/Delete.h>
#include <everything/node/Transform.h>
// nurbs
#include <everything/node/Carve.h>
// polygon
#include <everything/node/Add.h>
#include <everything/node/Boolean.h>
#include <everything/node/Knife.h>
#include <everything/node/PolyExtrude.h>
// primitive
#include <everything/node/Box.h>
#include <everything/node/Curve.h>
#include <everything/node/Line.h>
#include <everything/node/Sphere.h>
// utility
#include <everything/node/Blast.h>
#include <everything/node/CopyToPoints.h>
#include <everything/node/GroupCreate.h>
#include <everything/node/Switch.h>

#include <boost/lexical_cast.hpp>

namespace itt
{

void Everything::UpdatePropBackFromFront(const bp::Node& front, evt::Node& back)
{
    auto type = front.get_type();
    // attribute
    if (type == rttr::type::get<node::Sort>())
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

        dst.SetTranslate(src.translate);
        dst.SetRotate(src.rotate);
        dst.SetScale(src.scale);
        dst.SetShear(src.shear);
    }
    // NURBs
    else if (type == rttr::type::get<node::Carve>())
    {
        auto& src = static_cast<const node::Carve&>(front);
        auto& dst = static_cast<evt::node::Carve&>(back);

        dst.SetFirstU(src.first_u);
        dst.SetSecondU(src.second_u);
        dst.SetFirstV(src.first_v);
        dst.SetSecondV(src.second_v);
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

        sm::vec3 size;
        try {
            size.x = boost::lexical_cast<float>(src.size.x);
        } catch (boost::bad_lexical_cast&) {
            dst_props.SetExpr(evt::node::Box::SIZE_X, src.size.x);
        }
        try {
            size.y = boost::lexical_cast<float>(src.size.y);
        } catch (boost::bad_lexical_cast&) {
            dst_props.SetExpr(evt::node::Box::SIZE_Y, src.size.y);
        }
        try {
            size.z = boost::lexical_cast<float>(src.size.z);
        } catch (boost::bad_lexical_cast&) {
            dst_props.SetExpr(evt::node::Box::SIZE_Z, src.size.z);
        }
        dst.SetSize(size);

        sm::vec3 pos;
        try {
            pos.x = boost::lexical_cast<float>(src.center.x);
        } catch (boost::bad_lexical_cast&) {
            dst_props.SetExpr(evt::node::Box::POS_X, src.center.x);
        }
        try {
            pos.y = boost::lexical_cast<float>(src.center.y);
        } catch (boost::bad_lexical_cast&) {
            dst_props.SetExpr(evt::node::Box::POS_Y, src.center.y);
        }
        try {
            pos.z = boost::lexical_cast<float>(src.center.z);
        } catch (boost::bad_lexical_cast&) {
            dst_props.SetExpr(evt::node::Box::POS_Z, src.center.z);
        }
        dst.SetCenter(pos);

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
        dst.SetGroupType(src.group_type);
        dst.SetDeleteNonSelected(src.delete_non_selected);
    }
    else if (type == rttr::type::get<node::GroupCreate>())
    {
        auto& src = static_cast<const node::GroupCreate&>(front);
        auto& dst = static_cast<evt::node::GroupCreate&>(back);

        dst.SetGroupName(src.group_name);
        dst.SetGroupType(src.group_type);

        if (src.keep_by_normals) {
            dst.EnableKeepByNormals(src.direction, src.spread_angle);
        } else {
            dst.DisableKeepByNormals();
        }
    }
    else if (type == rttr::type::get<node::Switch>())
    {
        auto& src = static_cast<const node::Switch&>(front);
        auto& dst = static_cast<evt::node::Switch&>(back);
        dst.SetSelected(src.selected);
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

}