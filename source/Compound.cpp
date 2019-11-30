#include "sopview/Compound.h"
#include "sopview/SOPAdapter.h"

#include <blueprint/Connecting.h>
#include <vopview/Node.h>
#include <vopview/VOPAdapter.h>

#include <sop/node/Subnetwork.h>
#include <sop/node/AttributeVOP.h>

namespace sopv
{
namespace node
{

Compound::Compound(const std::string& title, bool parms)
    : Node(title, parms)
{
}

void Compound::InitChildren(const std::string& name)
{
    if (!m_children.empty()) {
        return;
    }

    rttr::type t = rttr::type::get_by_name("sop::" + name);
    if (!t.is_valid()) {
        return;
    }

    rttr::variant var = t.create();
    assert(var.is_valid());

    if (t.is_derived_from<sop::node::Subnetwork>())
    {
        std::map<const sop::NodePtr, const std::shared_ptr<Node>> back2front;

        auto sub_nw = var.get_value<std::shared_ptr<sop::node::Subnetwork>>();
        for (auto& c : sub_nw->GetAllChildren())
        {
            assert(c);
            auto node = SOPAdapter::CreateFrontFromBack(*c);
            if (node)
            {
                AddChild(node);
                back2front.insert({ c, node });
            }
        }

        for (auto& c : sub_nw->GetAllChildren())
        {
            auto itr_f = back2front.find(std::static_pointer_cast<sop::Node>(c));
            assert(itr_f != back2front.end());

            auto& out_ports = c->GetExports();
            for (size_t i = 0, n = out_ports.size(); i < n; ++i)
            {
                auto& port = out_ports[i];
                for (auto& conn : port.conns)
                {
                    auto back = conn.node.lock();
                    assert(back && back->get_type().is_derived_from<sop::Node>());
                    auto itr_t = back2front.find(std::static_pointer_cast<sop::Node>(back));
                    assert(itr_t != back2front.end());
                    bp::make_connecting(itr_f->second->GetAllOutput()[i], itr_t->second->GetAllInput()[conn.idx]);
                }
            }
        }
    }
    else if (t == rttr::type::get<sop::node::AttributeVOP>())
    {
        auto attr_vop = var.get_value<std::shared_ptr<sop::node::AttributeVOP>>();
        for (auto& c : attr_vop->GetAllChildren())
        {
            assert(c);
            auto node = vopv::VOPAdapter::CreateFrontFromBack(*c);
            if (node) {
                AddChild(node);
            }
        }
    }
}

void Compound::AddChild(const bp::NodePtr& child)
{
    m_children.push_back(child);
}

void Compound::RemoveChild(const bp::NodePtr& child)
{
    for (auto itr = m_children.begin(); itr != m_children.end(); )
    {
        if (*itr == child) {
            itr = m_children.erase(itr);
        } else {
            ++itr;
        }
    }
}

void Compound::ClearAllChildren()
{
    m_children.clear();
}

}
}