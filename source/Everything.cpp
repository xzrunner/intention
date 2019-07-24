#include "intention/Everything.h"
#include "intention/PinType.h"
#include "intention/Evaluator.h"
#include "intention/Node.h"
#include "intention/RegistNodes.h"

#include <blueprint/Pin.h>
#include <blueprint/Node.h>
#include <blueprint/Connecting.h>
#include <blueprint/node/Function.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>

#include <everything/node/Box.h>

namespace itt
{

evt::NodePtr Everything::CreateGraphNode(Evaluator& eval, const bp::Node* node)
{
    auto cached = eval.QueryEvtNode(node);
    if (cached) {
        return cached;
    }

    auto type = node->get_type();
    auto src_type = type.get_name().to_string();
    std::string dst_type;
    std::string lib_str = "evt";
    auto find_lib = src_type.find("itt::");
    if (find_lib != std::string::npos) {
        dst_type = lib_str + "::" + src_type.substr(find_lib + strlen("itt::"));
    }

    evt::NodePtr dst = nullptr;

    // create evt node
    if (!dst_type.empty())
    {
	    rttr::type t = rttr::type::get_by_name(dst_type);
        // fixme: specify node type
	    if (!t.is_valid())
        {
            dst = std::make_shared<evt::Node>();
            InitPortsBackFromFront(*dst, *node);
	    }
        else
        {
            rttr::variant var = t.create();
            assert(var.is_valid());

            dst = var.get_value<std::shared_ptr<evt::Node>>();
            assert(dst);
        }
    }

    if (type == rttr::type::get<node::Box>())
    {
        auto src = static_cast<const node::Box*>(node);
        auto box = std::static_pointer_cast<evt::node::Box>(dst);

        box->SetSize(src->size);
        box->SetCenter(src->center);
        box->SetScale(sm::vec3(src->scale, src->scale, src->scale));
    }

    // insert to cache
    if (dst) {
        eval.AddNodeMap(node, dst);
    }

    // connect input
    if (dst)
    {
        for (int i = 0, n = node->GetAllInput().size(); i < n; ++i)
        {
            auto& imports = dst->GetImports();
            if (node->IsExtensibleInputPorts() && i >= static_cast<int>(imports.size())) {
                continue;
            }
            evt::Node::PortAddr from_port;
            auto& conns = node->GetAllInput()[i]->GetConnecting();
            if (conns.empty()) {
                continue;
            }
            assert(conns.size() == 1);
            auto& bp_from_port = conns[0]->GetFrom();
            assert(bp_from_port);
            if (CreateFromNode(eval, bp_from_port, from_port)) {
                evt::make_connecting(from_port, { dst, i });
            }
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

bool Everything::CreateFromNode(Evaluator& eval, const std::shared_ptr<bp::Pin>& bp_from_port,
                                evt::Node::PortAddr& from_port)
{
    auto& parent = bp_from_port->GetParent();
    auto p_type = parent.get_type();
    if (p_type == rttr::type::get<bp::node::Input>())
    {
        auto& input = static_cast<const bp::node::Input&>(parent);
        auto func_node = input.GetParent();
        if (!func_node) {
            return false;
        }
        auto& func_inputs = func_node->GetAllInput();
        for (int i = 0, n = func_inputs.size(); i < n; ++i)
        {
            if (func_inputs[i]->GetName() != input.GetName()) {
                continue;
            }
            auto& conns = func_node->GetAllInput()[i]->GetConnecting();
            if (conns.empty()) {
                return false;
            }
            assert(conns.size() == 1);
            auto bp_from_port = conns[0]->GetFrom();
            assert(bp_from_port);

            return CreateFromNode(eval, bp_from_port, from_port);
        }
        return false;
    }
    from_port.node = CreateGraphNode(eval, &bp_from_port->GetParent());
    from_port.idx  = bp_from_port->GetPosIdx();

    return true;
}

void Everything::InitPortsBackFromFront(evt::Node& back, const bp::Node& front)
{
    auto& inputs  = front.GetAllInput();
    auto& outputs = front.GetAllOutput();

    std::vector<evt::Node::Port> imports, exports;
    imports.reserve(inputs.size());
    for (auto i : inputs)
    {
        evt::Variable var;
        var.type = TypeFrontToBack(i->GetType());
//        var.name = std::string("in") + std::to_string(i);
        var.name = i->GetName();
        imports.push_back(var);
    }
    exports.reserve(outputs.size());
    for (auto o : outputs)
    {
        evt::Variable var;
        var.type = TypeFrontToBack(o->GetType());
//        var.name = std::string("in") + std::to_string(i);
        var.name = o->GetName();
        exports.push_back(var);
    }
    back.SetImports(imports);
    back.SetExports(exports);
}

}