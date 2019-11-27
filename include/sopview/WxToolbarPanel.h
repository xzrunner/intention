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
	WxToolbarPanel(wxWindow* parent, ee0::WxStagePage* graph_stage,
        const std::shared_ptr<SceneTree>& stree);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    auto GetGeoPropView() const { return m_geo_prop; }

private:
	void InitLayout(const std::shared_ptr<SceneTree>& stree);

    void OnSelectionInsert(const ee0::VariantSet& variants);
    void OnSelectionClear(const ee0::VariantSet& variants);

    void OnRootToNextLevel(const ee0::VariantSet& variants);

private:
    ee0::WxStagePage* m_graph_stage;

    WxGeoProperty* m_geo_prop;

    WxNodeProperty* m_node_prop;

    ee0::WxNavigationBar* m_nav_bar;

}; // WxToolbarPanel

}