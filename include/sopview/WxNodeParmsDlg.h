#pragma once

#include <wx/dialog.h>

class wxListCtrl;

namespace sopv
{

struct NodeParmArray;

class WxNodeParmsDlg : public wxDialog
{
public:
    WxNodeParmsDlg(wxWindow* parent, const NodeParmArray& parms);

    void StoreToProps(NodeParmArray& parms) const;

private:
    void InitLayout();
    void SetupList();

    void OnAddItem(wxCommandEvent& event);
    void OnClearItems(wxCommandEvent& event);

private:
    const NodeParmArray& m_parms;

    wxListCtrl* m_list = nullptr;

}; // WxNodeParmsDlg

}