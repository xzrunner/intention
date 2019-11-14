#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sopview/PyParmValue.h"
#include "sopview/Node.h"
#include "sopview/Evaluator.h"
#include "sopview/ReflectPropTypes.h"

using namespace boost::python;

namespace sopv
{
namespace py
{

//////////////////////////////////////////////////////////////////////////
// class Parm
//////////////////////////////////////////////////////////////////////////

Parm::Parm(const std::string& name, const std::shared_ptr<Node>& node,
           const std::shared_ptr<Evaluator>& eval)
    : m_name(name)
    , m_node(node)
    , m_eval(eval)
{
}

void Parm::Set(float val)
{
    auto prop = rttr::type::get(*m_node).get_property(m_name);
    assert(prop.is_valid());
    auto succ = prop.set_value(*m_node, val);
    assert(succ);

    assert(m_eval);
    m_eval->OnNodePropChanged(m_node);
}

void Parm::Set(const std::string& val)
{
    auto prop = rttr::type::get(*m_node).get_property(m_name);
    assert(prop.is_valid());
    auto succ = prop.set_value(*m_node, val);
    assert(succ);

    assert(m_eval);
    m_eval->OnNodePropChanged(m_node);
}

void Parm::Set(int val)
{
    auto prop = rttr::type::get(*m_node).get_property(m_name);
    assert(prop.is_valid());
    auto succ = prop.set_value(*m_node, val);
    assert(succ);

    assert(m_eval);
    m_eval->OnNodePropChanged(m_node);
}

//////////////////////////////////////////////////////////////////////////
// class ParmTuple
//////////////////////////////////////////////////////////////////////////

ParmTuple::ParmTuple(const std::string& name, const std::shared_ptr<Node>& node,
                     const std::shared_ptr<Evaluator>& eval)
    : m_name(name)
    , m_node(node)
    , m_eval(eval)
{
}

void ParmTuple::Set(const boost::python::tuple& val)
{
    StrVec3 str_vec3(
        std::to_string(boost::python::extract<float>(val[0])),
        std::to_string(boost::python::extract<float>(val[1])),
        std::to_string(boost::python::extract<float>(val[2]))
    );

    auto prop = rttr::type::get(*m_node).get_property(m_name);
    assert(prop.is_valid());
    auto succ = prop.set_value(*m_node, str_vec3);
    assert(succ);

    assert(m_eval);
    m_eval->OnNodePropChanged(m_node);
}

Parm ParmTuple::GetItem(size_t i)
{
    switch (i)
    {
    case 0:
        return Parm(m_name + "x", m_node, m_eval);
    case 1:
        return Parm(m_name + "y", m_node, m_eval);
    case 2:
        return Parm(m_name + "z", m_node, m_eval);
    default:
        assert(0);
        return Parm("", nullptr, nullptr);
    }
}

void BindParmValue()
{
    void(Parm::*ParmSet0)(float)              = &Parm::Set;
    void(Parm::*ParmSet1)(const std::string&) = &Parm::Set;
    void(Parm::*ParmSet2)(int)                = &Parm::Set;
    void(Parm::*ParmSetKeyframe0)(const sop::py::Keyframe&)       = &Parm::SetKeyframe;
    void(Parm::*ParmSetKeyframe1)(const sop::py::StringKeyframe&) = &Parm::SetKeyframe;
    class_<Parm>("Parm", init<const std::string&, const std::shared_ptr<Node>&, const std::shared_ptr<Evaluator>&>())
        .def("setKeyframe", ParmSetKeyframe0)
        .def("setKeyframe", ParmSetKeyframe1)
        .def("set", ParmSet0)
        .def("set", ParmSet1)
    ;
    class_<ParmTuple>("ParmTuple", init<const std::string&, const std::shared_ptr<Node>&, const std::shared_ptr<Evaluator>&>())
        .def("setAutoscope", &ParmTuple::SetAutoscope)
        .def("set", &ParmTuple::Set)
        .def("__getitem__", &ParmTuple::GetItem, return_value_policy<return_by_value>())
    ;
}

}
}