#pragma once

#include <wx/dialog.h>

class wxListCtrl;

namespace itt
{

struct NodePropArray;

class WxNodePropsDlg : public wxDialog
{
public:
    WxNodePropsDlg(wxWindow* parent, const NodePropArray& props);

    void StoreToProps(NodePropArray& props) const;

private:
    void InitLayout();
    void SetupList();

    void OnAddItem(wxCommandEvent& event);
    void OnClearItems(wxCommandEvent& event);

private:
    const NodePropArray& m_props;

    wxListCtrl* m_list = nullptr;

}; // WxNodePropsDlg

}