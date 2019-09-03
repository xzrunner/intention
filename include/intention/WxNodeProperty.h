#pragma once

#include <blueprint/WxNodeProperty.h>

namespace itt
{

class WxNodeProperty : public bp::WxNodeProperty
{
public:
	WxNodeProperty(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr);

    virtual void LoadFromNode(const n0::SceneNodePtr& obj, const bp::NodePtr& node) override;

private:
    virtual bool InitView(const rttr::property& prop, const bp::NodePtr& node) override;
    virtual bool UpdateView(const rttr::property& prop, const wxPGProperty& wx_prop) override;
    virtual void UpdateView(wxPropertyGridEvent& event) override;

}; // WxNodeProperty

}