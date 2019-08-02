#include "intention/NodeHelper.h"
#include "intention/RegistNodes.h"

#include <blueprint/TreeHelper.h>
#include <blueprint/Node.h>

#include <set>

namespace itt
{

void NodeHelper::QueryPrevGroupCreateNodes(const bp::Node& root,
                                           std::vector<const bp::Node*>& result)
{
    std::set<const bp::Node*> prevs;
    bp::TreeHelper::GetPrecursorNodes(root, prevs);
    auto type = rttr::type::get<node::GroupCreate>();
    for (auto& p : prevs) {
        if (p->get_type() == type) {
            result.push_back(p);
        }
    }
}

}