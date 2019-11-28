#pragma once

#include <ee0/WxStagePage.h>

namespace vopv { class WxGraphPage; }
namespace pt0 { class Camera; }

namespace sopv
{

class SceneTree;
class WxToolbarPanel;

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

    void SetToolbarPanel(WxToolbarPanel* toolbar) { m_toolbar = toolbar; }

    void SetupCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas,
        const std::shared_ptr<ee0::WxStageCanvas>& preview_canvas);

private:
    ee0::EditOPPtr CreateSopEditOP(const std::shared_ptr<pt0::Camera>& camera);
    ee0::EditOPPtr CreateVopEditOP(const std::shared_ptr<pt0::Camera>& camera);

    bool InsertSceneObj(const ee0::VariantSet& variants);
    bool DeleteSceneObj(const ee0::VariantSet& variants);

    bool AfterInsertNodeConn(const ee0::VariantSet& variants);
    bool BeforeDeleteNodeConn(const ee0::VariantSet& variants);
    bool UpdateNodeProp(const ee0::VariantSet& variants);

    bool PathPushToNext(const ee0::VariantSet& variants);
    bool PathPopToPrev(const ee0::VariantSet& variants);

private:
    enum class ModeType
    {
        SOP,
        VOP
    };

    void ChangeMode(ModeType mode);

private:
    n0::SceneNodePtr m_root = nullptr;

    std::shared_ptr<SceneTree> m_stree = nullptr;

    std::shared_ptr<ee0::WxStageCanvas> m_preview_canvas = nullptr;

    std::shared_ptr<vopv::WxGraphPage> m_vop_view = nullptr;

    WxToolbarPanel* m_toolbar = nullptr;

    ee0::EditOPPtr m_sop_op = nullptr;
    ee0::EditOPPtr m_vop_op = nullptr;

    ModeType m_mode = ModeType::SOP;

}; // WxGraphPage

}