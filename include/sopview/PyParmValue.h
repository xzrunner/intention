#pragma once

#include <sop/PyCommonTypes.h>

#include <string>
#include <memory>

#include <boost/python.hpp>

namespace sopv
{

class Node;
class Evaluator;

namespace py
{

class Parm
{
public:
    Parm(const std::string& name, const std::shared_ptr<Node>& node,
        const std::shared_ptr<Evaluator>& eval);

    void SetKeyframe(const sop::py::Keyframe& k) {}
    void SetKeyframe(const sop::py::StringKeyframe& k) {}

    void Set(float val);
    void Set(const std::string& val);
    void Set(int val);

private:
    std::string m_name;

    std::shared_ptr<Node> m_node = nullptr;

    std::shared_ptr<Evaluator> m_eval = nullptr;

}; // Parm

class ParmTuple
{
public:
    ParmTuple(const std::string& name, const std::shared_ptr<Node>& node,
        const std::shared_ptr<Evaluator>& eval);

    void SetAutoscope(const boost::python::tuple& bool_values) {}
    void Set(const boost::python::tuple& val);
    Parm GetItem(size_t i);

private:
    std::string m_name;

    std::shared_ptr<Node> m_node = nullptr;

    std::shared_ptr<Evaluator> m_eval = nullptr;

}; // ParmTuple

void BindParmValue();

}
}