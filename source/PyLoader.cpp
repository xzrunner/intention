#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sopview/PyLoader.h"
#include "sopview/PyParmValue.h"
#include "sopview/PyNodeProxy.h"
#include "sopview/SceneTree.h"
#include "sopview/Evaluator.h"
#include "sopview/Node.h"
#include "sopview/RegistNodes.h"

#include <blueprint/CompNode.h>
#include <blueprint/Node.h>

#include <cpputil/StringHelper.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/CompBoundingBox.h>
#include <ns/NodeFactory.h>
#include <sop/PyCommonTypes.h>
#include <sop/PyParmTemplate.h>

#include <fstream>

#include <boost/python.hpp>

using namespace boost::python;
using namespace sopv::py;

namespace
{

std::shared_ptr<sopv::SceneTree> STREE = nullptr;

std::shared_ptr<NodeProxy> hou_get_node(const std::string& path)
{
    std::vector<std::string> tokens;
    cpputil::StringHelper::Split(path, "/", tokens);
    assert(tokens.size() >= 2 && tokens[0] == "obj");
    STREE->SetDepth(0);
    std::vector<n0::SceneNodePtr> paths;
    for (size_t i = 1, n = tokens.size(); i < n; ++i)
    {
        auto& name = tokens[i];
        assert(STREE->GetCurrNode()->HasSharedComp<n0::CompComplex>());
        auto& ccomplex = STREE->GetCurrNode()->GetSharedComp<n0::CompComplex>();
        for (auto& c : ccomplex.GetAllChildren())
        {
            assert(c->HasUniqueComp<bp::CompNode>());
            auto bp_node = c->GetUniqueComp<bp::CompNode>().GetNode();
            if (!bp_node->get_type().is_derived_from<sopv::Node>()) {
                continue;
            }

            auto sopv_node = std::static_pointer_cast<sopv::Node>(bp_node);
            if (sopv_node->GetName() == name)
            {
                if (i == n - 1) {
                    paths.push_back(c);
                    return std::make_shared<NodeProxy>(STREE, paths);
                } else {
                    assert(bp_node->get_type() == rttr::type::get<sopv::node::Geometry>());
                    STREE->ToNextLevel(c);
                }
            }
        }

        auto scene_node = ns::NodeFactory::Create2D();

        scene_node->AddSharedComp<n0::CompComplex>();

        auto sopv_node = std::make_shared<sopv::node::Geometry>();
        sopv_node->SetName(name);
        auto& cnode = scene_node->AddUniqueComp<bp::CompNode>();
        cnode.SetNode(sopv_node);

        auto& caabb = scene_node->GetUniqueComp<n2::CompBoundingBox>();
        auto& style = sopv_node->GetStyle();
        caabb.SetSize(*scene_node, sm::rect(style.width, style.height));

        STREE->Add(scene_node);

        STREE->ToNextLevel(scene_node);

        paths.push_back(scene_node);
    }

    return std::make_shared<NodeProxy>(STREE, paths);
}

BOOST_PYTHON_MODULE(hou)
{
    sop::py::BindCommonTypes();
    sop::py::BindParmTemplate();

    BindParmValue();
    BindNodeProxy();

    register_ptr_to_python<std::shared_ptr<NodeProxy>>();
    def("node", hou_get_node);
}

}

namespace sopv
{

PyLoader::PyLoader(std::shared_ptr<SceneTree> stree)
    : m_stree(stree)
{
    static bool inited = false;
    PyImport_AppendInittab("hou", &inithou);

    if (!inited)
    {
        Py_Initialize();

        inited = true;
    }
}

void PyLoader::RunFile(const std::string& filepath)
{
    m_stree->SetDepth(0);

    PyRun_SimpleString(R"(
import hou
hou_parent = None
hou_node = None
)");

    PrepareContext();

    std::ifstream fin(filepath);
    if (fin.fail()) {
        fin.close();
        return;
    }

    std::stringstream buffer;
    buffer << fin.rdbuf();

    try {
        PyRun_SimpleString(buffer.str().c_str());
    } catch (...) {
        boost::python::handle_exception();
    }

    fin.close();

    PyRun_SimpleString(R"(
del hou
)");

    m_stree->SetDepth(0);
}

void PyLoader::PrepareContext()
{
    STREE = m_stree;
}

}