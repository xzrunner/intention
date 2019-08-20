#pragma once

#include <everything/typedef.h>
#include <everything/NodeVar.h>

namespace bp { class Node; class Pin; }

namespace itt
{

class Evaluator;

class Everything
{
public:
    static void UpdatePropBackFromFront(const bp::Node& front, evt::Node& back);

    static evt::NodePtr CreateBackFromFront(const bp::Node& front);

    static int TypeBackToFront(evt::NodeVarType type);
    static evt::NodeVarType TypeFrontToBack(int pin_type);

}; // Everything

}