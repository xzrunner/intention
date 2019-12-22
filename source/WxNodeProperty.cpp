#include "sopview/WxNodeProperty.h"
#include "sopview/ReflectPropTypes.h"
#include "sopview/PinType.h"
#include "sopview/RegistNodes.h"
#include "sopview/MessageID.h"
#include "sopview/NodeParm.h"
#include "sopview/SceneTree.h"
#include "sopview/Evaluator.h"
#include "sopview/SOPAdapter.h"

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
#include <sop/GeometryImpl.h>
#include <sop/GeoAttrClass.h>
#include <sop/ParmList.h>
#include <sop/node/AttributeTransfer.h>

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

    sopv::PIN_PRIMITIVE,
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

sop::NodePtr GetGroupNameNode(const sopv::GroupName& name, const sop::NodePtr& self)
{
    if (name.idx == -1)
    {
        return self;
    }
    else
    {
        auto& imports = self->GetImports();
        assert(name.idx >= 0 && name.idx < static_cast<int>(imports.size()));
        auto& conns = imports[name.idx].conns;
        if (conns.empty()) {
            return false;
        }
        assert(conns.size() == 1);
        auto ret = conns[0].node.lock();
        if (!ret) {
            return nullptr;
        }
        assert(ret->get_type().is_derived_from<sop::Node>());
        if (!std::static_pointer_cast<sop::Node>(ret)->GetGeometry()) {
            return nullptr;
        }
        return std::static_pointer_cast<sop::Node>(ret);
    }
}

sop::NodePtr GetAttrNameNode(const bp::NodePtr& bp_node, const sopv::SceneTree& stree)
{
    auto get_input_geo = [](const bp::NodePtr& bp_node, const sopv::SceneTree& stree, size_t input_idx)->sop::NodePtr
    {
        auto sop_node = stree.GetCurrEval()->QueryBackNode(*bp_node);
        if (!sop_node || !sop_node->GetGeometry()) {
            return nullptr;
        }

        auto& conns = sop_node->GetImports()[input_idx].conns;
        if (conns.empty()) {
            return nullptr;
        }

        assert(conns.size() == 1);
        auto f_node = conns[0].node.lock();
        if (!f_node) {
            return nullptr;
        }
        assert(f_node->get_type().is_derived_from<sop::Node>());
        if (!std::static_pointer_cast<sop::Node>(f_node)->GetGeometry()) {
            return nullptr;
        }
        return std::static_pointer_cast<sop::Node>(f_node);
    };

    auto node_type = bp_node->get_type();
    assert(node_type.is_derived_from<sopv::Node>());
    if (node_type == rttr::type::get<sopv::node::AttributeTransfer>()) {
        return get_input_geo(bp_node, stree, sop::node::AttributeTransfer::IDX_FROM_GEO);
    } else {
        return get_input_geo(bp_node, stree, 0);
    }
}

const char* STR_GROUP_NULL = "null";

}

namespace sopv
{

WxNodeProperty::WxNodeProperty(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
                               const std::shared_ptr<SceneTree>& stree)
	: bp::WxNodeProperty(parent, sub_mgr)
    , m_stree(stree)
{
}

void WxNodeProperty::LoadFromNode(const n0::SceneNodePtr& obj, const bp::NodePtr& node)
{
    bp::WxNodeProperty::LoadFromNode(obj, node);

    if (!node->get_type().is_derived_from<sopv::Node>()) {
        return;
    }

    auto sopv_node = std::static_pointer_cast<sopv::Node>(node);
    auto& props = sopv_node->GetParms();
    if (!props) {
        return;
    }

    for (auto& p : props->parms) {
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
        assert(node_type.is_derived_from<Node>());
        auto sop_node = m_stree->GetCurrEval()->QueryBackNode(*node);
        if (!sop_node || !sop_node->GetGeometry()) {
            return false;
        }
        auto group_name = prop.get_value(node).get_value<GroupName>();
        sop_node = GetGroupNameNode(group_name, sop_node);
        if (!sop_node) {
            return false;
        }
        auto& groups = sop_node->GetGeometry()->GetGroup();

        int idx = -1;

        wxArrayString group_names;
        group_names.push_back("");
        groups.Traverse([&](const sop::Group& group)->bool
        {
            if (group.GetName() == group_name.str) {
                idx = group_names.size();
            }
            group_names.push_back(group.GetName());
            return true;
        });

        auto name_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, group_names);
        if (idx < 0) {
            idx = 0;
        }
        name_prop->SetValue(idx);
        m_pg->Append(name_prop);
    }
    else if (prop_type == rttr::type::get<GroupExprInst>())
    {
        auto v = prop.get_value(node).get_value<GroupExprInst>();

        wxPGProperty* prop = m_pg->Append(new wxStringProperty(ui_info.desc, wxPG_LABEL, wxT("<composed>")));
        prop->SetExpanded(false);

        m_pg->AppendIn(prop, new wxStringProperty(wxT("GroupName"), wxPG_LABEL, v.group_name));
        m_pg->AppendIn(prop, new wxStringProperty(wxT("ExprStr"),   wxPG_LABEL, v.expr_str));

        auto op_prop = ee0::WxPropHelper::CreateEnumProp(ui_info.desc, rttr::type::get<sop::GroupMerge>(), static_cast<int>(v.merge_op));
        m_pg->AppendIn(prop, op_prop);
    }
    else if (prop_type == rttr::type::get<AttrCreateItem>())
    {
        auto item = prop.get_value(node).get_value<AttrCreateItem>();

        wxPGProperty* prop = m_pg->Append(new wxStringProperty(ui_info.desc, wxPG_LABEL, wxT("<composed>")));
        prop->SetExpanded(false);

        m_pg->AppendIn(prop, new wxStringProperty(wxT("Name"),  wxPG_LABEL, item.name));
        m_pg->AppendIn(prop, ee0::WxPropHelper::CreateEnumProp("Class", rttr::type::get<sop::GeoAttrClass>(), static_cast<int>(item.cls)));
        m_pg->AppendIn(prop, ee0::WxPropHelper::CreateEnumProp("Type", rttr::type::get<sop::node::AttributeCreate::ItemType>(), static_cast<int>(item.type)));
        m_pg->AppendIn(prop, new wxFloatProperty("Val X", wxPG_LABEL, item.value.x));
        m_pg->AppendIn(prop, new wxFloatProperty("Val Y", wxPG_LABEL, item.value.y));
        m_pg->AppendIn(prop, new wxFloatProperty("Val Z", wxPG_LABEL, item.value.z));
        m_pg->AppendIn(prop, new wxFloatProperty("Val W", wxPG_LABEL, item.value.w));
        m_pg->AppendIn(prop, new wxFloatProperty("Default X", wxPG_LABEL, item.default_val.x));
        m_pg->AppendIn(prop, new wxFloatProperty("Default Y", wxPG_LABEL, item.default_val.y));
        m_pg->AppendIn(prop, new wxFloatProperty("Default Z", wxPG_LABEL, item.default_val.z));
        m_pg->AppendIn(prop, new wxFloatProperty("Default W", wxPG_LABEL, item.default_val.w));
    }
    else if (prop_type == rttr::type::get<AttributeName>())
    {
        auto sop_node = GetAttrNameNode(node, *m_stree);
        if (!sop_node) {
            return false;
        }

        auto attr_name = prop.get_value(node).get_value<AttributeName>();
        auto& lists = sop_node->GetGeometry()->GetAttr().GetAllParmLists()[static_cast<int>(attr_name.cls)];

        int idx = -1;

        wxArrayString attr_names;
        attr_names.push_back("");
        for (auto& list : lists)
        {
            if (list->GetName() == attr_name.str) {
                idx = attr_names.size();
            }
            attr_names.push_back(list->GetName());
        }

        auto attr_prop = new wxEnumProperty(ui_info.desc, wxPG_LABEL, attr_names);
        if (idx < 0) {
            idx = 0;
        }
        attr_prop->SetValue(idx);
        m_pg->Append(attr_prop);
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
        const int idx = wxANY_AS(val, int);

        assert(node_type.is_derived_from<Node>());
        auto sop_node = m_stree->GetCurrEval()->QueryBackNode(*m_node);
        assert(sop_node && sop_node->GetGeometry());
        auto group_name = prop.get_value(m_node).get_value<GroupName>();
        sop_node = GetGroupNameNode(group_name, sop_node);
        if (!sop_node) {
            return false;
        }

        auto& groups = sop_node->GetGeometry()->GetGroup();

        std::string name;
        if (idx > 0)
        {
            int i = 1;
            groups.Traverse([&](const sop::Group& group)->bool
            {
                if (i++ == idx) {
                    name = group.GetName();
                    return false;
                } else {
                    return true;
                }
            });
        }

        if (name.empty()) {
            group_name.str.clear();
        } else {
            group_name.str = name;
        }
        prop.set_value(m_node, group_name);
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

        auto op_str = tokens[2];
        v.merge_op = ee0::WxPropHelper::QueryEnumPropByLabel(op_str, rttr::type::get<sop::GroupMerge>()).get_value<sop::GroupMerge>();

        prop.set_value(m_node, v);
    }
    else if (prop_type == rttr::type::get<sop::GeoAttrClass>() && key == ui_info.desc)
    {
        prop.set_value(m_node, sop::GeoAttrClass(wxANY_AS(val, int)));

        if (node_type == rttr::type::get<node::AttributePromote>()) {
            static_cast<node::AttributePromote&>(*m_node).attr_name.cls = sop::GeoAttrClass(wxANY_AS(val, int));
        }
    }
    else if (prop_type == rttr::type::get<AttrCreateItem>() && key == ui_info.desc)
    {
        std::vector<std::string> tokens;
        auto str = wxANY_AS(val, wxString).ToStdString();
        boost::split(tokens, str, boost::is_any_of(";"));
        assert(tokens.size() == 11);

        for (auto& t : tokens) {
            boost::algorithm::trim(t);
        }

        auto item = prop.get_value(m_node).get_value<AttrCreateItem>();

        size_t idx = 0;
        item.name = tokens[idx++];

        auto cls_str = tokens[idx++];
        item.cls = ee0::WxPropHelper::QueryEnumPropByLabel(cls_str, rttr::type::get<sop::GeoAttrClass>()).get_value<sop::GeoAttrClass>();

        auto type_str = tokens[idx++];
        item.type = ee0::WxPropHelper::QueryEnumPropByLabel(type_str, rttr::type::get<sop::node::AttributeCreate::ItemType>()).get_value<sop::node::AttributeCreate::ItemType>();

        for (int i = 0; i < 4; ++i) {
            item.value.xyzw[i] = std::atof(tokens[idx++].c_str());
        }
        for (int i = 0; i < 4; ++i) {
            item.default_val.xyzw[i] = std::atof(tokens[idx++].c_str());
        }

        prop.set_value(m_node, item);
    }
    else if (prop_type == rttr::type::get<AttributeName>() && key == ui_info.desc)
    {
        const int idx = wxANY_AS(val, int);
        auto attr_name = prop.get_value(m_node).get_value<AttributeName>();
        if (idx == 0)
        {
            attr_name.str.clear();
        }
        else
        {
            assert(idx > 0);

            assert(node_type.is_derived_from<Node>());
            auto sop_node = GetAttrNameNode(m_node, *m_stree);
            assert(sop_node && sop_node->GetGeometry());

            auto& lists = sop_node->GetGeometry()->GetAttr().GetAllParmLists()[static_cast<int>(attr_name.cls)];
            attr_name.str = lists[idx - 1]->GetName();
        }
        prop.set_value(m_node, attr_name);
    }
    else
    {
        ret = false;
    }

    return ret;
}

bool WxNodeProperty::UpdateView(wxPropertyGridEvent& event)
{
    if (!m_node || !m_node->get_type().is_derived_from<sopv::Node>()) {
        return false;
    }

    auto sopv_node = std::static_pointer_cast<sopv::Node>(m_node);
    auto& props = sopv_node->GetParms();
    if (!props) {
        return false;
    }

    bool dirty = false;

    wxPGProperty* property = event.GetProperty();
    auto key = property->GetName();
    wxAny val = property->GetValue();
    for (auto& p : props->parms)
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