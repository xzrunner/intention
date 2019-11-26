#include "sopview/WxNodeParmsDlg.h"
#include "sopview/NodeParm.h"

#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/button.h>

namespace sopv
{

WxNodeParmsDlg::WxNodeParmsDlg(wxWindow* parent, const NodeParmArray& parms)
    : wxDialog(parent, wxID_ANY, "Props")
    , m_parms(parms)
{
    InitLayout();
}

void WxNodeParmsDlg::StoreToProps(NodeParmArray& parms) const
{
    const int n = m_list->GetItemCount();
    parms.parms.clear();
    parms.parms.resize(n);
    for (int i = 0; i < n; ++i)
    {
        auto& p(parms.parms[i]);

        wxListItem info;
        info.m_itemId = i;
        info.m_col = 0;
        info.m_mask = wxLIST_MASK_TEXT;
        m_list->GetItem(info);
        p.name = info.m_text;

        info.m_col = 1;
        m_list->GetItem(info);
#ifdef EnableNodeParmType
        bool finded = false;
        for (int j = 0, m = (int)NodeParmType::MaxNum; j < m; ++j) {
            if (info.m_text == NodeParmStrings[j]) {
                p.type = static_cast<NodeParmType>(j);
                finded = true;
                break;
            }
        }
        assert(finded);
#endif // EnableNodeParmType
        p.value = info.m_text;
    }
}

void WxNodeParmsDlg::InitLayout()
{
    wxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    {
        m_list = new wxListCtrl(this, wxID_ANY, wxDefaultPosition,
            wxDefaultSize, wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL);
        top_sizer->Add(m_list, 1, wxEXPAND);

        m_list->InsertColumn(0, "name", wxLIST_FORMAT_LEFT, 100);
#ifdef EnableNodeParmType
        m_list->InsertColumn(1, "type", wxLIST_FORMAT_LEFT, 100);
#endif // EnableNodeParmType
        m_list->InsertColumn(1, "value", wxLIST_FORMAT_LEFT, 400);

        SetupList();
    }
    {
        wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

        auto add_btn = new wxButton(this, wxID_ANY, "Add");
        Connect(add_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(WxNodeParmsDlg::OnAddItem));
        sizer->Add(add_btn, wxALL, 5);

        auto clear_btn = new wxButton(this, wxID_ANY, "Clear");
        Connect(clear_btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(WxNodeParmsDlg::OnClearItems));
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

void WxNodeParmsDlg::SetupList()
{
    for (int i = 0, n = m_parms.parms.size(); i < n; ++i)
    {
        auto& p = m_parms.parms[i];
        long item = m_list->InsertItem(i, "");
        m_list->SetItem(item, 0, p.name);
#ifdef EnableNodeParmType
        m_list->SetItem(item, 1, NodeParmStrings[static_cast<int>(p.type)]);
#endif // EnableNodeParmType
        m_list->SetItem(item, 1, p.value);
    }
}

void WxNodeParmsDlg::OnAddItem(wxCommandEvent& event)
{
    auto num = m_list->GetItemCount();
    long item = m_list->InsertItem(num, "");
    m_list->SetItem(item, 0, "item" + std::to_string(num));
#ifdef EnableNodeParmType
    m_list->SetItem(item, 1, NodeParmStrings[static_cast<int>(NodeParmType::Float)]);
#endif // EnableNodeParmType
    m_list->SetItem(item, 1, "");
}

void WxNodeParmsDlg::OnClearItems(wxCommandEvent& event)
{
    m_list->DeleteAllItems();
}

}