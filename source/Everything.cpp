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

// manipulate
#include <everything/node/Transform.h>
// polygon
#include <everything/node/Boolean.h>
#include <everything/node/Knife.h>
#include <everything/node/PolyExtrude.h>
// primitive
#include <everything/node/Box.h>
// utility
#include <everything/node/Blast.h>
#include <everything/node/CopyToPoints.h>
#include <everything/node/GroupCreate.h>
#include <everything/node/Merge.h>

namespace itt
{

void Everything::UpdatePropBackFromFront(const bp::Node& front, evt::Node& back)
{
    auto type = front.get_type();
    // manipulate
    if (type == rttr::type::get<node::Transform>())
    {
        auto& src = static_cast<const node::Transform&>(front);
        auto& dst = static_cast<evt::node::Transform&>(back);

        dst.SetTranslate(src.translate);
        dst.SetRotate(src.rotate);
        dst.SetScale(src.scale);
        dst.SetShear(src.shear);
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

        dst.SetSize(src.size);
        dst.SetCenter(src.center);
        dst.SetScale(sm::vec3(src.scale, src.scale, src.scale));
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

        dst.SetName(src.name);
        dst.SetType(src.type);

        if (src.keep_by_normals) {
            dst.EnableKeepByNormals(src.direction, src.spread_angle);
        } else {
            dst.DisableKeepByNormals();
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

    return dst;
}

int Everything::TypeBackToFront(evt::VariableType type)
{
    int ret = -1;

    switch (type)
    {
    case evt::VariableType::Any:
        ret = bp::PIN_ANY_VAR;
        break;
    case evt::VariableType::Port:
        ret = bp::PIN_PORT;
        break;
    case evt::VariableType::Primitive:
        ret = PIN_PRIMITIVE;
        break;
    }

    return ret;
}

evt::VariableType Everything::TypeFrontToBack(int pin_type)
{
    evt::VariableType ret = evt::VariableType::Any;

    switch (pin_type)
    {
    case bp::PIN_ANY_VAR:
        ret = evt::VariableType::Any;
        break;
    case bp::PIN_PORT:
        ret = evt::VariableType::Port;
        break;
    case PIN_PRIMITIVE:
        ret = evt::VariableType::Primitive;
        break;
    }

    return ret;
}

}