#include "intention/WxNodeProperty.h"
#include "intention/ReflectPropTypes.h"
#include "intention/PinType.h"
#include "intention/RegistNodes.h"
#include "intention/NodeHelper.h"
#include "intention/MessageID.h"
#include "intention/NodeProp.h"

#include <ee0/SubjectMgr.h>
#include <ee0/ReflectPropTypes.h>
#include <ee0/MessageID.h>
#include <ee0/WxPropHelper.h>
#include <blueprint/Node.h>
#include <blueprint/MessageID.h>
#include <blueprint/TreeHelper.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>

#include <cpputil/StringHelper.h>
#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>

#include <wx/sizer.h>
#include <wx/propgrid/propgrid.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace
{

const wxChar* PIN_IDX_TITLE[] = { wxT("Unknown"), wxT("Port"), wxT("Prim"), NULL };

const int PIN_IDX_TO_TYPE[] = {
    bp::PIN_ANY_VAR,
    bp::PIN_PORT,

    itt::PIN_PRIMITIVE,
};

int PinTypeToIdx(int type)
{
    const int num = sizeof(PIN_IDX_TO_TYPE) / sizeof(PIN_IDX_TO_TYPE[0]);
    for (int i = 0; i < num; ++i)
    {
        if (PIN_IDX_TO_TYPE[i] == type) {
            return i;
        }
    }
    assert(0);
    return -1;
}

const char* STR_GROUP_NULL = "null";

}

namespace itt
{

WxNodeProperty::WxNodeProperty(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr)
	: bp::WxNodeProperty(parent, sub_mgr)
{
}

void WxNodeProperty::LoadFromNode(const n0::SceneNodePtr& obj, const bp::NodePtr& node)
{
    bp::WxNodeProperty::LoadFromNode(obj, node);

    if (!node->get_type().is_derived_from<itt::Node>()) {
        return;
    }

    auto itt_node = std::static_pointer_cast<itt::Node>(node);
    auto& props = itt_node->GetProps();
    if (!props) {
        return;
    }

    for (auto& p : props->props) {
        m_pg->Append(new wxStringProperty(p.name, wxPG_LABEL, p.value));
    }
}

bool WxNodeProperty::InitView(const rttr::property& prop, const bp::NodePtr& node)
{
    bool ret = true;

    auto ui_info_obj = prop.get_metadata(ee0::UIMetaInfoTag());
    if (!ui_info_obj.is_valid()) {
        return false;
    }

    auto ui_info = ui_info_obj.get_value<ee0::UIMetaInfo>();
    auto node_type = node->get_type();
    if ((node_type == rttr::type::get<bp::node::Input>() && ui_info.desc == bp::node::Input::STR_TYPE) ||
        (node_type == rttr::type::get<bp::node::Output>() && ui_info.desc == bp::node::Output::STR_TYPE))
    {
		auto mode_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, PIN_IDX_TITLE);
		auto mode = prop.get_value(node).get_value<int>();
        if (mode < 0) {
            mode = bp::PIN_ANY_VAR;
        }
		mode_prop->SetValue(PinTypeToIdx(mode));
		m_pg->Append(mode_prop);
        return true;
    }

    auto prop_type = prop.get_type();
    if (prop_type == rttr::type::get<StrVec3>())
    {
        auto v = prop.get_value(node).get_value<StrVec3>();
        wxPGProperty* pos_prop = m_pg->Append(new wxStringProperty(ui_info.desc, wxPG_LABEL, wxT("<composed>")));
        pos_prop->SetExpanded(false);
        m_pg->AppendIn(pos_prop, new wxStringProperty(wxT("X"), wxPG_LABEL, v.x));
        m_pg->AppendIn(pos_prop, new wxStringProperty(wxT("Y"), wxPG_LABEL, v.y));
        m_pg->AppendIn(pos_prop, new wxStringProperty(wxT("Z"), wxPG_LABEL, v.z));
    }
    else if (prop_type == rttr::type::get<GroupName>())
    {
        std::vector<const bp::Node*> group_nodes;
        NodeHelper::QueryPrevGroupCreateNodes(*node, group_nodes);
        if (!group_nodes.empty())
        {
            int idx = -1;
            auto group_name = prop.get_value(node).get_value<GroupName>().str;

            wxArrayString group_names;
            for (auto& n : group_nodes)
            {
                auto& name = static_cast<const node::GroupCreate*>(n)->group_name;
                if (name == group_name) {
                    idx = group_names.size();
                }
                group_names.push_back(name);
            }
            group_names.push_back(STR_GROUP_NULL);

            auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, group_names);
            type_prop->SetValue(idx);
            m_pg->Append(type_prop);
        }
    }
    else if (prop_type == rttr::type::get<GroupType>())
    {
        const wxChar* TYPES[] = { wxT("GuessFromGroup"), wxT("Primitives"), wxT("Points"), wxT("Edges"), wxT("Vertices"), NULL };
        auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, TYPES);
        auto type = prop.get_value(node).get_value<GroupType>();
        type_prop->SetValue(static_cast<int>(type));
        m_pg->Append(type_prop);
    }
    else if (prop_type == rttr::type::get<GroupMerge>())
    {
        const wxChar* OPs[] = { wxT("Replace"), wxT("Union"), wxT("Intersect"), wxT("Subtract"), NULL };
        auto op_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, OPs);
        auto op = prop.get_value(node).get_value<GroupMerge>();
        op_prop->SetValue(static_cast<int>(op));
        m_pg->Append(op_prop);
    }
    else if (prop_type == rttr::type::get<GroupExprInst>())
    {
        auto v = prop.get_value(node).get_value<GroupExprInst>();

        wxPGProperty* prop = m_pg->Append(new wxStringProperty(ui_info.desc, wxPG_LABEL, wxT("<composed>")));
        prop->SetExpanded(false);

        m_pg->AppendIn(prop, new wxStringProperty(wxT("GroupName"), wxPG_LABEL, v.group_name));
        m_pg->AppendIn(prop, new wxStringProperty(wxT("ExprStr"),   wxPG_LABEL, v.expr_str));

        const wxChar* OPS[] = { wxT("Replace"), wxT("Union"), wxT("Intersect"), wxT("Subtract"), NULL };
        auto op_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, OPS);
        op_prop->SetValue(static_cast<int>(v.merge_op));
        m_pg->AppendIn(prop, op_prop);
    }
    else if (prop_type == rttr::type::get<BooleanOperator>())
    {
        const wxChar* OPS[] = { wxT("Union"), wxT("Intersect"), wxT("Subtract"), NULL };
        auto op_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, OPS);
        auto op = prop.get_value(node).get_value<BooleanOperator>();
        op_prop->SetValue(static_cast<int>(op));
        m_pg->Append(op_prop);
    }
    else if (prop_type == rttr::type::get<KnifeKeep>())
    {
        const wxChar* KEEPS[] = { wxT("KeepAbove"), wxT("KeepBelow"), wxT("KeepAll"), NULL };
        auto keep_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, KEEPS);
        auto keep = prop.get_value(node).get_value<KnifeKeep>();
        keep_prop->SetValue(static_cast<int>(keep));
        m_pg->Append(keep_prop);
    }
    else
    {
        ret = false;
    }

    return ret;
}

bool WxNodeProperty::UpdateView(const rttr::property& prop, const wxPGProperty& wx_prop)
{
    bool ret = true;

    auto ui_info_obj = prop.get_metadata(ee0::UIMetaInfoTag());
    if (!ui_info_obj.is_valid()) {
        return false;
    }

    auto key  = wx_prop.GetName();
    wxAny val = wx_prop.GetValue();

    auto ui_info = ui_info_obj.get_value<ee0::UIMetaInfo>();
    auto node_type = m_node->get_type();
    if (key == ui_info.desc &&
        ((node_type == rttr::type::get<bp::node::Input>() && ui_info.desc == bp::node::Input::STR_TYPE) ||
            (node_type == rttr::type::get<bp::node::Output>() && ui_info.desc == bp::node::Output::STR_TYPE)))
    {
        prop.set_value(m_node, PIN_IDX_TO_TYPE[wxANY_AS(val, int)]);
        return true;
    }

    if (key == Node::STR_PROP_DISPLAY && key == ui_info.desc)
    {
        const bool display = wxANY_AS(val, bool);
        if (display)
        {
            m_sub_mgr->NotifyObservers(MSG_CLEAR_NODE_DISPLAY_TAG);
            prop.set_value(m_node, true);
        }
        return true;
    }

    auto prop_type = prop.get_type();
    if (prop_type == rttr::type::get<StrVec3>() && key == ui_info.desc)
    {
        std::vector<std::string> tokens;
        auto str = wxANY_AS(val, wxString).ToStdString();
        cpputil::StringHelper::Split(str, ";", tokens);
        assert(tokens.size() == 3);

        for (auto& t : tokens) {
            boost::algorithm::trim(t);
        }

        auto v = prop.get_value(m_node).get_value<StrVec3>();
        v.x = tokens[0];
        v.y = tokens[1];
        v.z = tokens[2];

        prop.set_value(m_node, v);
    }
    else if (prop_type == rttr::type::get<GroupName>() && key == ui_info.desc)
    {
        std::vector<const bp::Node*> group_nodes;
        NodeHelper::QueryPrevGroupCreateNodes(*m_node, group_nodes);
        if (!group_nodes.empty())
        {
            const int idx = wxANY_AS(val, int);
            if (idx >= 0 && idx < static_cast<int>(group_nodes.size()))
            {
                auto node = group_nodes[idx];
                auto& name = static_cast<const node::GroupCreate*>(node)->group_name;
                prop.set_value(m_node, GroupName({ name }));
            }
            else
            {
                prop.set_value(m_node, GroupName());
            }
        }
    }
    else if (prop_type == rttr::type::get<GroupType>() && key == ui_info.desc)
    {
        prop.set_value(m_node, GroupType(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<GroupMerge>() && key == ui_info.desc)
    {
        prop.set_value(m_node, GroupMerge(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<GroupExprInst>() && key == ui_info.desc)
    {
        std::vector<std::string> tokens;
        auto str = wxANY_AS(val, wxString).ToStdString();
        boost::split(tokens, str, boost::is_any_of(";"));
        assert(tokens.size() == 3);

        for (auto& t : tokens) {
            boost::algorithm::trim(t);
        }

        auto v = prop.get_value(m_node).get_value<GroupExprInst>();
        v.group_name = tokens[0];
        v.expr_str   = tokens[1];
        auto& op_str = tokens[2];
        if (op_str == "Replace") {
            v.merge_op = GroupMerge::Replace;
        } else if (op_str == "Union") {
            v.merge_op = GroupMerge::Union;
        } else if (op_str == "Intersect") {
            v.merge_op = GroupMerge::Intersect;
        } else if (op_str == "Subtract") {
            v.merge_op = GroupMerge::Subtract;
        } else {
            assert(0);
        }

        prop.set_value(m_node, v);
    }
    else if (prop_type == rttr::type::get<BooleanOperator>() && key == ui_info.desc)
    {
        prop.set_value(m_node, BooleanOperator(wxANY_AS(val, int)));
    }
    else if (prop_type == rttr::type::get<KnifeKeep>() && key == ui_info.desc)
    {
        prop.set_value(m_node, KnifeKeep(wxANY_AS(val, int)));
    }
    else
    {
        ret = false;
    }

    return ret;
}

bool WxNodeProperty::UpdateView(wxPropertyGridEvent& event)
{
    if (!m_node || !m_node->get_type().is_derived_from<itt::Node>()) {
        return false;
    }

    auto itt_node = std::static_pointer_cast<itt::Node>(m_node);
    auto& props = itt_node->GetProps();
    if (!props) {
        return false;
    }

    bool dirty = false;

    wxPGProperty* property = event.GetProperty();
    auto key = property->GetName();
    wxAny val = property->GetValue();
    for (auto& p : props->props)
    {
        if (p.name != key) {
            continue;
        }
        std::string str = wxANY_AS(val, wxString).ToStdString();
        if (p.value != str) {
            p.value = str;
            dirty = true;
        }
    }

    return dirty;
}

}