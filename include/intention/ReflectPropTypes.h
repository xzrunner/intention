#pragma once

#include <string>

namespace itt
{

struct StrVec3
{
    StrVec3(const std::string& x,
            const std::string& y,
            const std::string& z)
        : x(x), y(y), z(z) {}

    std::string x;
    std::string y;
    std::string z;

}; // StrVec3

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