#define BOOST_PYTHON_STATIC_MODULE
#define BOOST_PYTHON_STATIC_LIB

#include "sopview/PyParmValue.h"
#include "sopview/Node.h"
#include "sopview/Evaluator.h"
#include "sopview/ReflectPropTypes.h"
#include "sopview/RegistNodes.h"

using namespace boost::python;

namespace
{

void SetProp(rttr::property prop, float val, rttr::instance obj)
{
    assert(prop.is_valid());
    auto prop_type = prop.get_type();
    if (prop_type == rttr::type::get<float>())
    {
        auto succ = prop.set_value(obj, val);
        assert(succ);
    }
    else if (prop_type == rttr::type::get<int>())
    {
        auto succ = prop.set_value(obj, static_cast<int>(val));
        assert(succ);
    }
    else if (prop_type == rttr::type::get<unsigned int>())
    {
        auto succ = prop.set_value(obj, static_cast<unsigned int>(val));
        assert(succ);
    }
    else
    {
        assert(0);
    }
}

void SetProp(rttr::property prop, const std::string& val, rttr::instance obj)
{
    assert(prop.is_valid());
    auto prop_type = prop.get_type();
    if (prop_type == rttr::type::get<sopv::GroupName>())
    {
        auto name = prop.get_value(obj).get_value<sopv::GroupName>();
        name.str = val;
        auto succ = prop.set_value(obj, name);
        assert(succ);
    }
    else if (prop_type == rttr::type::get<sopv::AttributeName>())
    {
        auto name = prop.get_value(obj).get_value<sopv::AttributeName>();
        name.str = val;
        auto succ = prop.set_value(obj, name);
        assert(succ);
    }
    else if (prop_type == rttr::type::get<std::string>())
    {
        auto succ = prop.set_value(obj, val);
        assert(succ);
    }
    else if (prop_type.is_enumeration())
    {
        auto enum_val = prop_type.get_enumeration().name_to_value(val);
        if (enum_val.is_valid()) {
            auto succ = prop.set_value(obj, enum_val);
            assert(succ);
        } else {
            assert(0);
        }
    }
    else
    {
        assert(0);
    }
}

void SetProp(rttr::property prop, const boost::python::tuple& val, rttr::instance obj)
{
    assert(prop.is_valid());
    auto prop_type = prop.get_type();
    if (prop_type == rttr::type::get<sm::vec3>())
    {
        sm::vec3 v3;
        for (size_t i = 0; i < 3; ++i) {
            v3.xyz[i] = boost::python::extract<float>(val[i]);
        }

        auto succ = prop.set_value(obj, v3);
        assert(succ);
    }
    else if (prop_type == rttr::type::get<sm::vec4>())
    {
        sm::vec4 v4;
        for (size_t i = 0; i < 4; ++i) {
            v4.xyzw[i] = boost::python::extract<float>(val[i]);
        }

        auto succ = prop.set_value(obj, v4);
        assert(succ);
    }
    else
    {
        sopv::StrVec3 str_vec3(
            std::to_string(boost::python::extract<float>(val[0])),
            std::to_string(boost::python::extract<float>(val[1])),
            std::to_string(boost::python::extract<float>(val[2]))
        );

        auto succ = prop.set_value(obj, str_vec3);
        assert(succ);
    }
}

}

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
    auto node_type = m_node->get_type();
    if (node_type == rttr::type::get<node::AttributeCreate>() && m_name.back() >= '1' && m_name.back() <= '4')
    {
        const int i = std::stoi(&m_name.back()) - 1;
        auto prop = rttr::type::get(*m_node).get_property("item" + std::to_string(i));
        assert(prop.is_valid());
        auto sub_prop = prop.get_type().get_property(m_name.substr(0, m_name.size() - 1));
        assert(sub_prop.is_valid());
        SetProp(sub_prop, val, prop.get_value(*m_node));

        assert(m_eval);
        m_eval->OnNodePropChanged(m_node);

        return;
    }
    else if (node_type == rttr::type::get<node::PolyExtrude>() && 
             (m_name.find("thicknessramp") != std::string::npos || m_name.find("twistramp") != std::string::npos))
    {
        return;
    }
    else if (node_type == rttr::type::get<node::Color>() && 
             m_name.find("ramp2") != std::string::npos)
    {
        return;
    }

    auto prop = rttr::type::get(*m_node).get_property(m_name);
    assert(prop.is_valid());
    auto prop_type = prop.get_type();
    if (prop_type == rttr::type::get<bool>())
    {
        bool b = false;
        if (val == 0.0f) {
            b = false;
        } else if (val == 1.0f) {
            b = true;
        } else {
            assert(0);
        }

        auto succ = prop.set_value(*m_node, b);
        assert(succ);
    }
    else if (prop_type == rttr::type::get<std::string>())
    {
        //// to boolean
        //std::string b;
        //if (val == 0.0f) {
        //    b = "false";
        //} else if (val == 1.0f) {
        //    b = "true";
        //} else {
        //    assert(0);
        //}

        std::string str = std::to_string(val);
        auto succ = prop.set_value(*m_node, str);
        assert(succ);
    }
    else if (prop_type == rttr::type::get<int>())
    {
        int i = static_cast<int>(val);
        assert(static_cast<float>(i) == val);
        auto succ = prop.set_value(*m_node, i);
        assert(succ);
    }
    else if (prop_type == rttr::type::get<unsigned int>())
    {
        unsigned int i = static_cast<int>(val);
        assert(static_cast<float>(i) == val);
        auto succ = prop.set_value(*m_node, i);
        assert(succ);
    }
    else if (prop_type == rttr::type::get<sop::node::Fuse::Operator>())
    {
        sop::node::Fuse::Operator op;
        if (val == 0) {
            op = sop::node::Fuse::Operator::Consolidate;
        } else if (val == 1) {
            op = sop::node::Fuse::Operator::UniquePoints;
        } else if (val == 2) {
            op = sop::node::Fuse::Operator::Snap;
        } else {
            assert(0);
        }
        auto succ = prop.set_value(*m_node, op);
        assert(succ);
    }
    else
    {
        auto succ = prop.set_value(*m_node, val);
        assert(succ);
    }

    assert(m_eval);
    m_eval->OnNodePropChanged(m_node);
}

void Parm::Set(const std::string& val)
{
    auto node_type = m_node->get_type();
    if (node_type == rttr::type::get<node::AttributeCreate>() && m_name.back() >= '1' && m_name.back() <= '4')
    {
        const int i = std::stoi(&m_name.back()) - 1;
        auto prop = rttr::type::get(*m_node).get_property("item" + std::to_string(i));
        assert(prop.is_valid());
        auto sub_prop = prop.get_type().get_property(m_name.substr(0, m_name.size() - 1));
        assert(sub_prop.is_valid());
        SetProp(sub_prop, val, prop.get_value(*m_node));

        assert(m_eval);
        m_eval->OnNodePropChanged(m_node);

        return;
    }
    else if (node_type == rttr::type::get<node::GroupExpression>() && m_name.back() >= '1' && m_name.back() <= '4')
    {
        const int i = std::stoi(&m_name.back()) - 1;
        auto prop = rttr::type::get(*m_node).get_property("inst" + std::to_string(i));
        assert(prop.is_valid());
        auto sub_prop = prop.get_type().get_property(m_name.substr(0, m_name.size() - 1));
        assert(sub_prop.is_valid());
        SetProp(sub_prop, val, prop.get_value(*m_node));

        assert(m_eval);
        m_eval->OnNodePropChanged(m_node);

        return;
    }
    else if (node_type == rttr::type::get<node::PolyExtrude>() &&
             (m_name.find("thicknessramp") != std::string::npos || m_name.find("twistramp") != std::string::npos))
    {
        return;
    }

    auto prop = rttr::type::get(*m_node).get_property(m_name);
    assert(prop.is_valid());

    if (node_type == rttr::type::get<node::Delete>() && m_name == "negate")
    {
        bool del_no_sel = false;
        if (val == "keep") {
            del_no_sel = false;
        } else if (val == "dele") {
            del_no_sel = true;
        } else {
            assert(0);
        }

        auto succ = prop.set_value(*m_node, del_no_sel);
        assert(succ);

        assert(m_eval);
        m_eval->OnNodePropChanged(m_node);

        return;
    }

    SetProp(prop, val, *m_node);

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
    auto node_type = m_node->get_type();
    if (node_type == rttr::type::get<node::AttributeCreate>() && m_name.back() == 'v' &&
        m_name.size() > 1 && m_name[m_name.size() - 2] >= '1' && m_name[m_name.size() - 2] <= '4')
    {
        const int i = std::stoi(&m_name[m_name.size() - 2]) - 1;
        auto prop = rttr::type::get(*m_node).get_property("item" + std::to_string(i));
        assert(prop.is_valid());
        auto sub_prop = prop.get_type().get_property(m_name.substr(0, m_name.size() - 2));
        assert(sub_prop.is_valid());
        SetProp(sub_prop, val, prop.get_value(*m_node));

        assert(m_eval);
        m_eval->OnNodePropChanged(m_node);

        return;
    }
    else if (node_type == rttr::type::get<node::Color>() && 
             m_name.find("ramp2") != std::string::npos)
    {
        return;
    }

    auto prop = rttr::type::get(*m_node).get_property(m_name);
    assert(prop.is_valid());
    SetProp(prop, val, *m_node);

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