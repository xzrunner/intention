#include "intention/WxNodeProperty.h"
#include "intention/ReflectPropTypes.h"
#include "intention/PinType.h"
#include "intention/RegistNodes.h"

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

}

namespace itt
{

WxNodeProperty::WxNodeProperty(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr)
	: bp::WxNodeProperty(parent, sub_mgr)
{
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
    if (prop_type == rttr::type::get<GroupName>())
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
                auto& name = static_cast<const node::GroupCreate*>(n)->name;
                if (name == group_name) {
                    idx = group_names.size();
                }
                group_names.push_back(name);
            }
            auto type_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, group_names);
            type_prop->SetValue(idx);
            m_pg->Append(type_prop);
        }
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

    auto prop_type = prop.get_type();
    if (prop_type == rttr::type::get<GroupName>() && key == ui_info.desc)
    {
        std::vector<const bp::Node*> group_nodes;
        NodeHelper::QueryPrevGroupCreateNodes(*m_node, group_nodes);
        if (!group_nodes.empty())
        {
            auto node = group_nodes[wxANY_AS(val, int)];
            auto& name = static_cast<const node::GroupCreate*>(node)->name;
            prop.set_value(m_node, GroupName({ name }));
        }
    }
    else
    {
        ret = false;
    }

    return ret;
}

}