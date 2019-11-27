#pragma once

#include <ee0/WxStagePage.h>

namespace sopv
{

class SceneTree;

class WxGraphPage : public ee0::WxStagePage
{
public:
	WxGraphPage(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
        const ee0::GameObj& root);
    virtual ~WxGraphPage();

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
        const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

    auto GetSceneTree() const { return m_stree; }

    void SetPreviewCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas);

private:
    bool InsertSceneObj(const ee0::VariantSet& variants);
    bool DeleteSceneObj(const ee0::VariantSet& variants);

    bool AfterInsertNodeConn(const ee0::VariantSet& variants);
    bool BeforeDeleteNodeConn(const ee0::VariantSet& variants);
    bool UpdateNodeProp(const ee0::VariantSet& variants);

    bool ChangeSceneRoot(const ee0::VariantSet& variants);
    bool PathSeekToPrev(const ee0::VariantSet& variants);

private:
    n0::SceneNodePtr m_root = nullptr;

    std::shared_ptr<SceneTree> m_stree = nullptr;

    std::shared_ptr<ee0::WxStageCanvas> m_preview_canvas = nullptr;

}; // WxGraphPage

}