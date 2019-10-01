#pragma once

#include <ee0/Observer.h>

#include <wx/panel.h>

namespace ee0 { class WxStagePage; class WxNavigationBar; }

namespace sopv
{

class WxGeoProperty;
class WxNodeProperty;
class SceneTree;

class WxToolbarPanel : public wxPanel, public ee0::Observer
{
public:
	WxToolbarPanel(wxWindow* parent, ee0::WxStagePage* stage_page,
        const std::shared_ptr<SceneTree>& stree);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

private:
	void InitLayout(const std::shared_ptr<SceneTree>& stree);

    void OnSelectionInsert(const ee0::VariantSet& variants);
    void OnSelectionClear(const ee0::VariantSet& variants);

    void OnRootToNextLevel(const ee0::VariantSet& variants);

private:
    ee0::WxStagePage* m_stage_page;

    WxGeoProperty* m_geo_prop;

    WxNodeProperty* m_node_prop;

    ee0::WxNavigationBar* m_nav_bar;

}; // WxToolbarPanel

}