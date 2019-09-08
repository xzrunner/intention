#pragma once

#include <SM_Vector.h>
#include <everything/typedef.h>
#include <everything/NodeVar.h>

namespace evt { class Node; }
namespace bp { class Node; class Pin; }

namespace itt
{

class Evaluator;
struct StrVec3;

class Everything
{
public:
    static void UpdatePropBackFromFront(const bp::Node& front,
        evt::Node& back, const Evaluator& eval);

    static evt::NodePtr CreateBackFromFront(const bp::Node& front);

    static int TypeBackToFront(evt::NodeVarType type);
    static evt::NodeVarType TypeFrontToBack(int pin_type);

private:
    static sm::vec3 ParseExprFloat3(const StrVec3& src, const evt::Node& dst,
        const sm::ivec3& idx, const sm::vec3& expect, const Evaluator& eval);

}; // Everything

}