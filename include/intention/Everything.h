#pragma once

#include <everything/typedef.h>
#include <everything/Node.h>

namespace bp { class Node; class Pin; }

namespace itt
{

class Evaluator;

class Everything
{
public:
    static evt::NodePtr CreateGraphNode(Evaluator& eval,
        const bp::Node* node);

    static int TypeBackToFront(evt::VariableType type);
    static evt::VariableType TypeFrontToBack(int pin_type);

private:
    static bool CreateFromNode(Evaluator& eval,
        const std::shared_ptr<bp::Pin>& bp_from_port, evt::Node::PortAddr& from_port);

    static void InitPortsBackFromFront(evt::Node& back, const bp::Node& front);;

}; // Everything

}