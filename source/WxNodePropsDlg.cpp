#include "sopview/WxNodePropsDlg.h"
#include "sopview/NodeProp.h"

#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/button.h>

namespace sopv
{

WxNodePropsDlg::WxNodePropsDlg(wxWindow* parent, const NodePropArray& props)
    : wxDialog(parent, wxID_ANY, "Props")
    , m_props(props)
{
    InitLayout();
}

void WxNodePropsDlg::StoreToProps(NodePropArray& props) const
{
    const int n = m_list->GetItemCount();
    props.props.clear();
    props.props.resize(n);
    for (int i = 0; i < n; ++i)
    {
        auto& p(props.props[i]);

        wxListItem info;
        info.m_itemId = i;
        info.m_col = 0;
        info.m_mask = wxLIST_MASK_TEXT;
        m_list->GetItem(info);
        p.name = info.m_text;

        info.m_col = 1;
        m_list->GetItem(info);
#ifdef EnableNodePropType
        bool finded = false;
        for (int j = 0, m = (int)NodePropType::MaxNum; j < m; ++j) {
            if (info.m_text == NodePropStrings[j]) {
                p.type = static_cast<NodePropType>(j);
                finded = true;
                break;
            }
        }
        assert(finded);
#endif // EnableNodePropType
        p.value = info.m_text;
    }
}

void WxNodePropsDlg::InitLayout()
{
    wxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    {
        m_list = new wxListCtrl(this, wxID_ANY, wxDefaultPosition,
            wxDefaultSize, wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL);
        top_sizer->Add(m_list, 1, wxEXPAND);

        m_list->InsertColumn(0, "name", wxLIST_FORMAT_LEFT, 100);
#ifdef EnableNodePropType
        m_list->InsertColumn(1, "type", wxLIST_FORMAT_LEFT, 100);
#endif // EnableNodePropType
        m_list->InsertColumn(1, "value", wxLIST_FORMAT_LEFT, 400);

        SetupList();
    }
    {
        wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

        auto add_btn = new wxButton(this, wxID_ANY, "Add");
        Connect(add_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(WxNodePropsDlg::OnAddItem));
        sizer->Add(add_btn, wxALL, 5);

        auto clear_btn = new wxButton(this, wxID_ANY, "Clear");
        Connect(clear_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(WxNodePropsDlg::OnClearItems));
        sizer->Add(clear_btn, wxALL, 5);

        top_sizer->Add(sizer, 0, wxALIGN_CENTER);
    }
    {
        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
        sizer->Add(new wxButton(this, wxID_OK), wxALL, 5);
        sizer->Add(new wxButton(this, wxID_CANCEL), wxALL, 5);
        top_sizer->Add(sizer, 0, wxALIGN_CENTER);
    }
    SetSizer(top_sizer);
}

void WxNodePropsDlg::SetupList()
{
    for (int i = 0, n = m_props.props.size(); i < n; ++i)
    {
        auto& p = m_props.props[i];
        long item = m_list->InsertItem(i, "");
        m_list->SetItem(item, 0, p.name);
#ifdef EnableNodePropType
        m_list->SetItem(item, 1, NodePropStrings[static_cast<int>(p.type)]);
#endif // EnableNodePropType
        m_list->SetItem(item, 1, p.value);
    }
}

void WxNodePropsDlg::OnAddItem(wxCommandEvent& event)
{
    auto num = m_list->GetItemCount();
    long item = m_list->InsertItem(num, "");
    m_list->SetItem(item, 0, "item" + std::to_string(num));
#ifdef EnableNodePropType
    m_list->SetItem(item, 1, NodePropStrings[static_cast<int>(NodePropType::Float)]);
#endif // EnableNodePropType
    m_list->SetItem(item, 1, "");
}

void WxNodePropsDlg::OnClearItems(wxCommandEvent& event)
{
    m_list->DeleteAllItems();
}

}