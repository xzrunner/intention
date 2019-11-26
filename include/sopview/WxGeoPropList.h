#pragma once

#include <sop/GeoAttrClass.h>
#include <sop/GeoAttribute.h>

#include <wx/listctrl.h>

namespace sop { class GeometryImpl; }

namespace sopv
{

class WxGeoPropList : public wxListCtrl
{
public:
    WxGeoPropList(wxWindow* parent, sop::GeoAttrClass type);

    std::vector<size_t> GetSelectedIndices() const;

    void ClearAll();

    void SetGeoData(const std::shared_ptr<sop::GeometryImpl>& geo);

protected:
    virtual wxString OnGetItemText(long item, long column) const override;

private:
    void InitColumns();

    wxString GetPointItemText(long item, long column) const;
    wxString GetVertexItemText(long item, long column) const;
    wxString GetPrimitiveItemText(long item, long column) const;
    wxString GetDetailItemText(long item, long column) const;

    wxString GetOthersPropText(long item, long column, const std::vector<std::shared_ptr<sop::ParmList>>& lists) const;

    void QueryVertexIndex(const sop::GeoAttribute& attr, const std::shared_ptr<sop::GeoAttribute::Vertex>& vert,
        int& prim_idx, int& vert_idx) const;
    std::string GetAttrName(sop::GeoAttr attr, int index = -1) const;

    std::string TopoIDToString(const he::TopoID& id) const;
    std::string IDToString(const he::TopoID& topo_id, size_t group_id) const;

    std::string VarToString(sop::ParmType type, const sop::ParmList& list, size_t item, int comp_idx = -1) const;

private:
    sop::GeoAttrClass m_type = sop::GeoAttrClass::Point;

    std::shared_ptr<sop::GeometryImpl> m_geo = nullptr;

}; // WxGeoPropList

}