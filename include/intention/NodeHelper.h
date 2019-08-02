#pragma once

#include <blueprint/typedef.h>

#include <vector>

namespace itt
{

class NodeHelper
{
public:
    static void QueryPrevGroupCreateNodes(const bp::Node& root,
        std::vector<const bp::Node*>& result);

}; // NodeHelper

}