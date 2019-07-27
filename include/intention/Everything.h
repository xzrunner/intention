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
    static void UpdatePropBackFromFront(const bp::Node& front, evt::Node& back);

    static evt::NodePtr CreateBackFromFront(const bp::Node& front);

    static int TypeBackToFront(evt::VariableType type);
    static evt::VariableType TypeFrontToBack(int pin_type);

}; // Everything

}