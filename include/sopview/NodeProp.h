#pragma once

#include <string>
#include <vector>

namespace sopv
{

//#define EnableNodePropType

#ifdef EnableNodePropType
enum class NodePropType
{
    Bool,
    Int,
    Float,
    Float3,
    Double,
    String,

    MaxNum
};
#endif // EnableNodePropType

static char* NodePropStrings[] = {
    "Bool",
    "Int",
    "Float",
    "Float3",
    "Double",
    "String",

    "",
};

struct NodeProp
{
    std::string  name;
#ifdef EnableNodePropType
    NodePropType type;
#endif // EnableNodePropType
    std::string  value;

}; // NodeProp

struct NodePropArray
{
    std::vector<NodeProp> props;

}; // NodePropArray

}