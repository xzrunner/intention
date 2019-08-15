#pragma once

#include <string>

namespace itt
{

struct GroupName
{
    std::string str;

}; // GroupName

enum class BooleanOperator
{
    Union,
    Intersect,
    Subtract,
};

enum class KnifeKeep
{
    KeepAbove,
    KeepBelow,
    KeepAll,
};

enum class SortKey
{
    NoChange,
    X,
    Y,
    Z
};

enum class DeleteEntityType
{
    Points,
    Edges,
    Faces,
};

void prop_types_regist_rttr();

}