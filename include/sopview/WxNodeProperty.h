#pragma once

#include <blueprint/WxNodeProperty.h>

namespace sopv
{

class SceneTree;

class WxNodeProperty : public bp::WxNodeProperty
{
public:
	WxNodeProperty(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
        const std::shared_ptr<SceneTree>& stree);

    virtual void LoadFromNode(const n0::SceneNodePtr& obj, const bp::NodePtr& node) override;

private:
    virtual bool InitView(const rttr::property& prop, const bp::NodePtr& node) override;
    virtual bool UpdateView(const rttr::property& prop, const wxPGProperty& wx_prop) override;
    virtual bool UpdateView(wxPropertyGridEvent& event) override;

private:
    std::shared_ptr<SceneTree> m_stree = nullptr;

}; // WxNodeProperty

}