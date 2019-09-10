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

enum class GroupType
{
    GuessFromGroup,
    Primitives,
    Points,
    Edges,
    Vertices,
};

enum class MergeOP
{
    Replace,
    Union,
    Intersect,
    Subtract,
};

struct GroupExprInst
{
    std::string group_name;
    std::string expr_str;
    MergeOP     merge_op = MergeOP::Union;

}; // GroupExprInst

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