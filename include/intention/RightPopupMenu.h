#pragma once

#include <ee0/RightPopupMenu.h>

namespace ee0 { class WxStagePage; }

namespace itt
{

class RightPopupMenu : public ee0::RightPopupMenu
{
public:
    RightPopupMenu(ee0::WxStagePage* stage);

    virtual void SetRightPopupMenu(wxMenu& menu, int x, int y) override;
    virtual void OnRightPopupMenu(int id) override;

private:
    ee0::WxStagePage* m_stage = nullptr;

}; // RightPopupMenu

}