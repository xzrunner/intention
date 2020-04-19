#pragma once

#include "sopview/EditOpMode.h"

#include <ee0/Observer.h>

#include <wx/panel.h>

namespace ur2 { class Device; }
namespace ee0 { class WxStagePage; class WxNavigationBar; }
namespace bp { class WxNodeProperty; }

namespace sopv
{

class WxGeoProperty;
class SceneTree;

class WxToolbarPanel : public wxPanel, public ee0::Observer
{
public:
	WxToolbarPanel(const ur2::Device& dev, wxWindow* parent, ee0::WxStagePage* graph_stage,
        const std::shared_ptr<SceneTree>& stree);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    auto GetGeoPropView() const { return m_geo_prop; }

    void ChangeEditOpMode(EditOpMode mode);

private:
	void InitLayout(const ur2::Device& dev,
        const std::shared_ptr<SceneTree>& stree);

    void OnSelectionInsert(const ee0::VariantSet& variants);
    void OnSelectionClear(const ee0::VariantSet& variants);

    void OnRootToNextLevel(const ee0::VariantSet& variants);

    bp::WxNodeProperty* GetCurrNodeProp() const;

private:
    ee0::WxStagePage* m_graph_stage;

    WxGeoProperty* m_geo_prop;

    bp::WxNodeProperty* m_sop_node_prop;
    bp::WxNodeProperty* m_vop_node_prop;

    EditOpMode m_mode = EditOpMode::SOP;

    ee0::WxNavigationBar* m_nav_bar;

}; // WxToolbarPanel

}