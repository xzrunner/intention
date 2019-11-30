#pragma once

#include "sopview/Node.h"

namespace sopv
{
namespace node
{

class Compound : public Node
{
public:
    Compound(const std::string& title, bool parms = false);

    auto& GetAllChildren() const { return m_children; }

    void AddChild(const bp::NodePtr& child);
    void RemoveChild(const bp::NodePtr& child);
    void ClearAllChildren();

protected:
    void InitChildren(const std::string& name);

private:
    std::vector<bp::NodePtr> m_children;

    RTTR_ENABLE(Node)
};

}
}
