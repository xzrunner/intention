#pragma once

#include "sopview/PyParmValue.h"

#include <sop/PyCommonTypes.h>

#include <node0/typedef.h>
#include <SM_Vector.h>

#include <memory>
#include <string>
#include <vector>

#include <boost/python.hpp>

namespace ur2 { class Device; }

namespace sopv
{

class Node;

namespace py
{

struct PyLoaderCtx;

class NodeProxy
{
public:
    NodeProxy(PyLoaderCtx& ctx, const std::vector<n0::SceneNodePtr>& paths);

    std::shared_ptr<NodeProxy> CreateNode(const ur2::Device& dev, const std::string& type, const std::string& name,
        bool run_init_scripts = false, bool load_contents = false, bool exact_type_name = false);
    std::shared_ptr<NodeProxy> CreateNetworkBox(const std::string& name);
    std::shared_ptr<NodeProxy> FindNetworkBox(const std::string& name);

    std::shared_ptr<NodeProxy> GetParent();
    std::shared_ptr<NodeProxy> GetChild(const std::string& name);

    void AddNode(const std::shared_ptr<NodeProxy>& node) {}

    void Move(const sm::vec2& v);
    void SetSelectableInViewport(bool b) {}
    void ShowOrigin(bool b) {}
    void UseXray(bool b) {}
    void Hide(bool b) {}
    void SetSelected(bool b) {}
    void SetParmTemplateGroup(const sop::py::ParmTemplateGroup& parm) {}
    ParmTuple GetParmTuple(const std::string& name);
    void SetExpressionLanguage(sop::py::ExprLanguage lang) {}
    void SetUserData(const std::string& name, const std::string& value) {}
    void Bypass(bool on) {}
    void SetDisplayFlag(bool on) {}
    void SetHighlightFlag(bool on) {}
    void SetHardLocked(bool on) {}
    void SetSoftLocked(bool on) {}
    void SetSelectableTemplateFlag(bool on) {}
    void SetRenderFlag(bool on) {}
    void SetTemplateFlag(bool on) {}
    void SetUnloadFlag(bool on) {}
    void SetInput(int input_index, const std::shared_ptr<NodeProxy>& item_to_become_input, int output_index = 0);
    void SetPosition(const sm::vec2& pos);
    void SetSize(const sm::vec2& size);
    void SetMinimized(bool on) {}
    void SetAutoFit(bool on) {}
    void SetComment(const std::string& comment) {}
    void SetColor(const sop::py::Color& col) {}
    Parm GetParm(const std::string& name);
    boost::python::list IndirectInputs() { return boost::python::list(); }

private:
    std::shared_ptr<Node> CreateNodeByName(const std::string& name) const;

    std::shared_ptr<Node> GetNode() const;

private:
    PyLoaderCtx& m_ctx;

    std::vector<n0::SceneNodePtr> m_paths;

}; // NodeProxy

void BindNodeProxy();

}
}