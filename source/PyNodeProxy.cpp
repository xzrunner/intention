#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sopview/PyNodeProxy.h"
#include "sopview/RegistNodes.h"
#include "sopview/Evaluator.h"
#include "sopview/SceneTree.h"

#include <blueprint/Connecting.h>
#include <blueprint/CompNode.h>

#include <blueprint/NSCompNode.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/CompBoundingBox.h>
#include <node2/CompTransform.h>
#include <ns/NodeFactory.h>
#include <sop/node/Geometry.h>

using namespace boost::python;

namespace
{

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(hou_node_create_node_overloads, NodeProxy::CreateNode, 2, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(hou_node_set_input_overloads, NodeProxy::SetInput, 2, 3)

}

namespace sopv
{
namespace py
{

NodeProxy::NodeProxy(std::shared_ptr<sopv::SceneTree>& stree,
                     const std::vector<n0::SceneNodePtr>& paths)
    : m_stree(stree)
    , m_paths(paths)
{
    assert(!m_paths.empty());
}

std::shared_ptr<NodeProxy>
NodeProxy::CreateNode(const std::string& type, const std::string& name,
                      bool run_init_scripts, bool load_contents, bool exact_type_name)
{
    auto child = CreateNodeByName(type);
    assert(child);

    auto scene_node = ns::NodeFactory::Create2D();

    auto& cnode = scene_node->AddUniqueComp<bp::CompNode>();
    cnode.SetNode(child);

    if ("sopv::" + type == rttr::type::get<node::Geometry>().get_name().to_string()) {
        scene_node->AddSharedComp<n0::CompComplex>();
    }

    auto& caabb = scene_node->GetUniqueComp<n2::CompBoundingBox>();
    auto& style = child->GetStyle();
    caabb.SetSize(*scene_node, sm::rect(style.width, style.height));

    m_stree->SetDepth(0);
    for (auto& n : m_paths) {
        m_stree->ToNextLevel(n);
    }
    m_stree->Add(scene_node);

    child->SetName(name);

    auto paths = m_paths;
    paths.push_back(scene_node);
    return std::make_shared<NodeProxy>(m_stree, paths);
}

std::shared_ptr<NodeProxy> NodeProxy::CreateNetworkBox(const std::string& name)
{
    return std::make_shared<NodeProxy>(m_stree, m_paths);
}

std::shared_ptr<NodeProxy> NodeProxy::FindNetworkBox(const std::string& name)
{
    assert(0);
    return nullptr;
}

std::shared_ptr<NodeProxy> NodeProxy::GetParent()
{
    auto paths = m_paths;
    paths.pop_back();
    return std::make_shared<NodeProxy>(m_stree, paths);
}

std::shared_ptr<NodeProxy>
NodeProxy::GetChild(const std::string& name)
{
    assert(!m_paths.empty());

    auto node = m_paths.back();
    assert(node->HasUniqueComp<bp::CompNode>());
    auto& cnode = node->GetUniqueComp<bp::CompNode>();
    auto bp_node = cnode.GetNode();
    assert(bp_node->get_type().is_derived_from<Node>());

    if (bp_node->get_type() == rttr::type::get<node::Geometry>())
    {
        assert(node->HasSharedComp<n0::CompComplex>());
        for (auto& c : node->GetSharedComp<n0::CompComplex>().GetAllChildren())
        {
            assert(c->HasUniqueComp<bp::CompNode>());
            auto& cnode = c->GetUniqueComp<bp::CompNode>();
            auto bp_node = cnode.GetNode();
            if (bp_node->get_type().is_derived_from<Node>()) {
                auto sopv_node = std::static_pointer_cast<Node>(bp_node);
                if (sopv_node->GetName() == name) {
                    auto paths = m_paths;
                    paths.push_back(c);
                    return std::make_shared<NodeProxy>(m_stree, paths);
                }
            }

        }
    }

    return nullptr;
}

void NodeProxy::Move(const sm::vec2& v)
{
    assert(!m_paths.empty());
    auto node = m_paths.back();
    assert(node->HasUniqueComp<n2::CompTransform>());
    auto& ctrans = node->GetUniqueComp<n2::CompTransform>();
    ctrans.SetPosition(*node, v * 100);
}

ParmTuple NodeProxy::GetParmTuple(const std::string& name)
{
    assert(m_paths.size() > 1);
    auto eval = m_stree->QueryEval(m_paths[m_paths.size() - 2]);
    assert(eval);
    return ParmTuple(name, GetNode(), eval);
}

void NodeProxy::SetInput(int input_index, const std::shared_ptr<NodeProxy>& item_to_become_input, int output_index)
{
    assert(!m_paths.empty() && !item_to_become_input->m_paths.empty());
    auto from = item_to_become_input->GetNode()->GetAllOutput()[output_index];
    auto to = GetNode()->GetAllInput()[input_index];
    auto conn = bp::make_connecting(from, to);
    m_stree->GetCurrEval()->OnConnected(*conn);
}

Parm NodeProxy::GetParm(const std::string& name)
{
    assert(m_paths.size() > 1);
    auto eval = m_stree->QueryEval(m_paths[m_paths.size() - 2]);
    assert(eval);
    return Parm(name, GetNode(), eval);
}

std::shared_ptr<Node>
NodeProxy::CreateNodeByName(const std::string& name) const
{
    rttr::type type = rttr::type::get_by_name("sopv::" + name);
    if (!type.is_valid()) {
        assert(0);
        return nullptr;
    }

    rttr::variant var = type.create();
    assert(var.is_valid());
    return var.get_value<std::shared_ptr<Node>>();
}

std::shared_ptr<Node> NodeProxy::GetNode() const
{
    if (m_paths.empty()) {
        return nullptr;
    }

    auto node = m_paths.back();
    assert(node->HasUniqueComp<bp::CompNode>());
    auto& cnode = node->GetUniqueComp<bp::CompNode>();
    auto bp_node = cnode.GetNode();
    assert(bp_node->get_type().is_derived_from<Node>());
    return std::static_pointer_cast<Node>(bp_node);
}

void BindNodeProxy()
{
    class_<NodeProxy>("NodeProxy", init<std::shared_ptr<SceneTree>&, const std::vector<n0::SceneNodePtr>&>())
        .def("createNode", &NodeProxy::CreateNode, hou_node_create_node_overloads(
            args("type", "name", "run_init_scripts", "load_contents", "exact_type_name")
        ))
        .def("createNetworkBox", &NodeProxy::CreateNetworkBox)
        .def("findNetworkBox", &NodeProxy::FindNetworkBox)
        .def("parent", &NodeProxy::GetParent)
        .def("node", &NodeProxy::GetChild)
        .def("move", &NodeProxy::Move)
        .def("setSelectableInViewport", &NodeProxy::SetSelectableInViewport)
        .def("showOrigin", &NodeProxy::ShowOrigin)
        .def("useXray", &NodeProxy::UseXray)
        .def("hide", &NodeProxy::Hide)
        .def("setSelected", &NodeProxy::SetSelected)
        .def("setParmTemplateGroup", &NodeProxy::SetParmTemplateGroup)
        .def("parmTuple", &NodeProxy::GetParmTuple)
        .def("setExpressionLanguage", &NodeProxy::SetExpressionLanguage)
        .def("setUserData", &NodeProxy::SetUserData)
        .def("bypass", &NodeProxy::Bypass)
        .def("setDisplayFlag", &NodeProxy::SetDisplayFlag)
        .def("setHighlightFlag", &NodeProxy::SetHighlightFlag)
        .def("setHardLocked", &NodeProxy::SetHardLocked)
        .def("setSoftLocked", &NodeProxy::SetSoftLocked)
        .def("setSelectableTemplateFlag", &NodeProxy::SetSelectableTemplateFlag)
        .def("setRenderFlag", &NodeProxy::SetRenderFlag)
        .def("setTemplateFlag", &NodeProxy::SetTemplateFlag)
        .def("setUnloadFlag", &NodeProxy::SetUnloadFlag)
        .def("setInput", &NodeProxy::SetInput, hou_node_set_input_overloads(
            args("input_index", "item_to_become_input", "output_index")
        ))
        .def("setPosition", &NodeProxy::SetPosition)
        .def("setSize", &NodeProxy::SetSize)
        .def("setMinimized", &NodeProxy::SetMinimized)
        .def("setAutoFit", &NodeProxy::SetAutoFit)
        .def("setComment", &NodeProxy::SetComment)
        .def("setColor", &NodeProxy::SetColor)
        .def("parm", &NodeProxy::GetParm)
        .def("indirectInputs", &NodeProxy::IndirectInputs)
    ;
}

}
}