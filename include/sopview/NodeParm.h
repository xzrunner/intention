#pragma once

#include <string>
#include <vector>

namespace sopv
{

//#define EnableNodeParmType

#ifdef EnableNodeParmType
enum class NodeParmType
{
    Bool,
    Int,
    Float,
    Float3,
    Double,
    String,

    MaxNum
};
#endif // EnableNodeParmType

static char* NodeParmStrings[] = {
    "Bool",
    "Int",
    "Float",
    "Float3",
    "Double",
    "String",

    "",
};

struct NodeParm
{
    std::string  name;
#ifdef EnableNodeParmType
    NodeParmType type;
#endif // EnableNodeParmType
    std::string  value;

}; // NodeParm

struct NodeParmArray
{
    std::vector<NodeParm> parms;

}; // NodeParmArray

}