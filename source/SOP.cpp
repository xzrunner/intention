#include "sopview/SOP.h"
#include "sopview/PinType.h"
#include "sopview/Evaluator.h"
#include "sopview/Node.h"
#include "sopview/RegistNodes.h"
#include "sopview/NodeProp.h"

#include <blueprint/Pin.h>
#include <blueprint/Node.h>
#include <blueprint/Connecting.h>
#include <blueprint/node/Function.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>

// attribute
#include <sop/node/AttributeCreate.h>
#include <sop/node/AttributeTransfer.h>
#include <sop/node/AttributeWrangle.h>
#include <sop/node/Sort.h>
#include <sop/node/Measure.h>
// export
#include <sop/node/File.h>
// group
#include <sop/node/GroupCreate.h>
#include <sop/node/GroupExpression.h>
#include <sop/node/GroupPromote.h>
// manipulate
#include <sop/node/Delete.h>
#include <sop/node/Peak.h>
#include <sop/node/Transform.h>
// material
#include <sop/node/Color.h>
// nurbs
#include <sop/node/Carve.h>
// polygon
#include <sop/node/Add.h>
#include <sop/node/Boolean.h>
#include <sop/node/Fuse.h>
#include <sop/node/Knife.h>
#include <sop/node/Normal.h>
#include <sop/node/PolyExtrude.h>
#include <sop/node/PolyFrame.h>
// primitive
#include <sop/node/Box.h>
#include <sop/node/Curve.h>
#include <sop/node/Grid.h>
#include <sop/node/Line.h>
#include <sop/node/Primitive.h>
#include <sop/node/Sphere.h>
// utility
#include <sop/node/Blast.h>
#include <sop/node/CopyToPoints.h>
#include <sop/node/ForeachPrimEnd.h>
#include <sop/node/Split.h>
#include <sop/node/Switch.h>

#include <boost/lexical_cast.hpp>

namespace
{

sop::GroupType
TransGroupType(sopv::GroupType type)
{
    switch (type)
    {
    case sopv::GroupType::GuessFromGroup:
        return sop::GroupType::GuessFromGroup;
    case sopv::GroupType::Primitives:
        return sop::GroupType::Primitives;
    case sopv::GroupType::Points:
        return sop::GroupType::Points;
    case sopv::GroupType::Edges:
        return sop::GroupType::Edges;
    case sopv::GroupType::Vertices:
        return sop::GroupType::Vertices;
    default:
        assert(0);
        return sop::GroupType::Primitives;
    }
}

sop::GeoAttrType
TransGeoAttrType(sopv::GeoAttrType type)
{
    switch (type)
    {
    case sopv::GeoAttrType::Int:
        return sop::GeoAttrType::Int;
    case sopv::GeoAttrType::Bool:
        return sop::GeoAttrType::Bool;
    case sopv::GeoAttrType::Double:
        return sop::GeoAttrType::Double;
    case sopv::GeoAttrType::Float:
        return sop::GeoAttrType::Float;
    case sopv::GeoAttrType::Float2:
        return sop::GeoAttrType::Float2;
    case sopv::GeoAttrType::Float3:
        return sop::GeoAttrType::Float3;
    case sopv::GeoAttrType::Float4:
        return sop::GeoAttrType::Float4;
    case sopv::GeoAttrType::String:
        return sop::GeoAttrType::String;
    case sopv::GeoAttrType::Vector:
        return sop::GeoAttrType::Vector;
    case sopv::GeoAttrType::Vector4:
        return sop::GeoAttrType::Vector4;
    case sopv::GeoAttrType::Matrix2:
        return sop::GeoAttrType::Matrix2;
    case sopv::GeoAttrType::Matrix3:
        return sop::GeoAttrType::Matrix3;
    case sopv::GeoAttrType::Matrix4:
        return sop::GeoAttrType::Matrix4;
    default:
        assert(0);
        return sop::GeoAttrType::Int;
    }
}

sop::GroupMerge
TransGroupMerge(sopv::GroupMerge merge_op)
{
    switch (merge_op)
    {
    case sopv::GroupMerge::Replace:
        return sop::GroupMerge::Replace;
    case sopv::GroupMerge::Union:
        return sop::GroupMerge::Union;
    case sopv::GroupMerge::Intersect:
        return sop::GroupMerge::Intersect;
    case sopv::GroupMerge::Subtract:
        return sop::GroupMerge::Subtract;
    default:
        assert(0);
        return sop::GroupMerge::Replace;
    }
}

sop::node::GroupExpression::Instance
TransGroupExprInst(const sopv::GroupExprInst& src)
{
    sop::node::GroupExpression::Instance dst;
    dst.group_name = src.group_name;
    dst.expr_str   = src.expr_str;
    dst.merge_op   = TransGroupMerge(src.merge_op);
    return dst;
}

sop::VarValue TransAttrCreateVal(sopv::GeoAttrType type, const sm::vec4& value)
{
    sop::VarValue val;
    switch (type)
    {
    case sopv::GeoAttrType::Int:
        val = sop::VarValue(static_cast<int>(value.x));
        break;

    case sopv::GeoAttrType::Bool:
        val = sop::VarValue(value.x == 0 ? false : true);
        break;
    case sopv::GeoAttrType::Double:
        val = sop::VarValue(static_cast<double>(value.x));
        break;

    case sopv::GeoAttrType::Float:
        val = sop::VarValue(value.x);
        break;
    case sopv::GeoAttrType::Float2:
        assert(0);
        break;
    case sopv::GeoAttrType::Float3:
        val = sop::VarValue(sm::vec3(value.x, value.y, value.z));
        break;
    case sopv::GeoAttrType::Float4:
        assert(0);
        break;

    case sopv::GeoAttrType::Vector:
        val = sop::VarValue(sm::vec3(value.x, value.y, value.z));
        break;
    case sopv::GeoAttrType::Vector4:
        assert(0);
        break;

    case sopv::GeoAttrType::Matrix2:
    case sopv::GeoAttrType::Matrix3:
    case sopv::GeoAttrType::Matrix4:
        assert(0);
        break;
    }
    return val;
}

sop::node::AttributeCreate::Item
TransAttrCreateItem(const sopv::AttrCreateItem& item)
{
    auto cls   = sopv::SOP::TransGeoAttrClass(item.cls);
    auto type  = TransGeoAttrType(item.type);
    auto val   = TransAttrCreateVal(item.type, item.value);
    auto d_val = TransAttrCreateVal(item.type, item.default_val);

    return { item.name, type, cls, val, d_val };
}

sop::node::PolyFrame::FrameStyle
TransGroupType(sopv::PolyFrameStyle style)
{
    switch (style)
    {
    case sopv::PolyFrameStyle::FirstEdge:
        return sop::node::PolyFrame::FrameStyle::FirstEdge;
    case sopv::PolyFrameStyle::TwoEdges:
        return sop::node::PolyFrame::FrameStyle::TwoEdges;
    case sopv::PolyFrameStyle::PrimitiveCentroid:
        return sop::node::PolyFrame::FrameStyle::PrimitiveCentroid;
    case sopv::PolyFrameStyle::TextureUV:
        return sop::node::PolyFrame::FrameStyle::TextureUV;
    case sopv::PolyFrameStyle::TextureUVGradient:
        return sop::node::PolyFrame::FrameStyle::TextureUVGradient;
    case sopv::PolyFrameStyle::AttributeGradient:
        return sop::node::PolyFrame::FrameStyle::AttributeGradient;
    case sopv::PolyFrameStyle::MikkT:
        return sop::node::PolyFrame::FrameStyle::MikkT;
    default:
        assert(0);
        return sop::node::PolyFrame::FrameStyle::FirstEdge;
    }
}

}

namespace sopv
{

void SOP::UpdatePropBackFromFront(const bp::Node& front, sop::Node& back,
                                         const Evaluator& eval)
{
    auto type = front.get_type();
    // attribute
    if (type == rttr::type::get<node::AttributeCreate>())
    {
        auto& src = static_cast<const node::AttributeCreate&>(front);
        auto& dst = static_cast<sop::node::AttributeCreate&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetGroupType(TransGroupType(src.group_type));

        std::vector<sop::node::AttributeCreate::Item> items;
        if (!src.item0.name.empty()) {
            items.push_back(TransAttrCreateItem(src.item0));
        }
        if (!src.item1.name.empty()) {
            items.push_back(TransAttrCreateItem(src.item1));
        }
        if (!src.item2.name.empty()) {
            items.push_back(TransAttrCreateItem(src.item2));
        }
        if (!src.item3.name.empty()) {
            items.push_back(TransAttrCreateItem(src.item3));
        }
        dst.SetAttrItems(items);
    }
    else if (type == rttr::type::get<node::AttributeTransfer>())
    {
        auto& src = static_cast<const node::AttributeTransfer&>(front);
        auto& dst = static_cast<sop::node::AttributeTransfer&>(back);

        dst.ClearCopyAttrs();
        if (!src.points_attrs.str.empty()) {
            dst.SetCopyAttrs(sop::GeoAttrClass::Point, { src.points_attrs.str });
        }
        if (!src.vertices_attrs.str.empty()) {
            dst.SetCopyAttrs(sop::GeoAttrClass::Vertex, { src.vertices_attrs.str });
        }
        if (!src.primitives_attrs.str.empty()) {
            dst.SetCopyAttrs(sop::GeoAttrClass::Primitive, { src.primitives_attrs.str });
        }
        if (!src.detail_attrs.str.empty()) {
            dst.SetCopyAttrs(sop::GeoAttrClass::Detail, { src.detail_attrs.str });
        }
    }
    else if (type == rttr::type::get<node::AttributeWrangle>())
    {
        auto& src = static_cast<const node::AttributeWrangle&>(front);
        auto& dst = static_cast<sop::node::AttributeWrangle&>(back);
        dst.SetVexExpr(src.vex_expr);
    }
    else if (type == rttr::type::get<node::Measure>())
    {
        auto& src = static_cast<const node::Measure&>(front);
        auto& dst = static_cast<sop::node::Measure&>(back);

        sop::node::Measure::Type type;
        switch (src.ms_type)
        {
        case MeasureType::Perimeter:
            type = sop::node::Measure::Type::Perimeter;
            break;
        case MeasureType::Area:
            type = sop::node::Measure::Type::Area;
            break;
        default:
            assert(0);
        }
        dst.SetMesureType(type);

        dst.SetMesureName(src.ms_name);
    }
    else if (type == rttr::type::get<node::Sort>())
    {
        auto& src = static_cast<const node::Sort&>(front);
        auto& dst = static_cast<sop::node::Sort&>(back);

        sop::node::Sort::Key key;
        switch (src.key)
        {
        case SortKey::NoChange:
            key = sop::node::Sort::Key::NoChange;
            break;
        case SortKey::X:
            key = sop::node::Sort::Key::X;
            break;
        case SortKey::Y:
            key = sop::node::Sort::Key::Y;
            break;
        case SortKey::Z:
            key = sop::node::Sort::Key::Z;
            break;
        case SortKey::Shift:
            key = sop::node::Sort::Key::Shift;
            break;
        default:
            assert(0);
        }
        dst.SetKey(key);

        dst.SetPointOffset(ParseExprInt(src.point_offset, back,
            sop::node::Sort::POINT_OFFSET, 0, eval));
    }
    // export
    else if (type == rttr::type::get<node::File>())
    {
        auto& src = static_cast<const node::File&>(front);
        auto& dst = static_cast<sop::node::File&>(back);
        dst.SetFilepath(src.filepath);
    }
    // group
    else if (type == rttr::type::get<node::GroupCreate>())
    {
        auto& src = static_cast<const node::GroupCreate&>(front);
        auto& dst = static_cast<sop::node::GroupCreate&>(back);

        dst.SetGroupName(src.group_name);
        dst.SetGroupType(TransGroupType(src.group_type));
        dst.SetGroupMerge(TransGroupMerge(src.merge_op));

        if (src.base_group) {
            dst.EnableBaseGroup(src.base_group_expr);
        } else {
            dst.DisableBaseGroup();
        }

        if (src.keep_in_bounding) {
            dst.EnableKeepInBounding();
        } else {
            dst.DisableKeepInBounding();
        }

        if (src.keep_by_normals) {
            dst.EnableKeepByNormals(src.direction, src.spread_angle);
        } else {
            dst.DisableKeepByNormals();
        }
    }
    else if (type == rttr::type::get<node::GroupExpression>())
    {
        auto& src = static_cast<const node::GroupExpression&>(front);
        auto& dst = static_cast<sop::node::GroupExpression&>(back);

        dst.SetGroupType(TransGroupType(src.group_type));
        dst.ClearInstances();
        if (!src.inst0.expr_str.empty()) {
            dst.AddInstance(TransGroupExprInst(src.inst0));
        }
        if (!src.inst1.expr_str.empty()) {
            dst.AddInstance(TransGroupExprInst(src.inst1));
        }
        if (!src.inst2.expr_str.empty()) {
            dst.AddInstance(TransGroupExprInst(src.inst2));
        }
        if (!src.inst3.expr_str.empty()) {
            dst.AddInstance(TransGroupExprInst(src.inst3));
        }
    }
    else if (type == rttr::type::get<node::GroupPromote>())
    {
        auto& src = static_cast<const node::GroupPromote&>(front);
        auto& dst = static_cast<sop::node::GroupPromote&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetSrcGroupType(TransGroupType(src.src_type));
        dst.SetDstGroupType(TransGroupType(src.dst_type));
    }
    // manipulate
    else if (type == rttr::type::get<node::Delete>())
    {
        auto& src = static_cast<const node::Delete&>(front);
        auto& dst = static_cast<sop::node::Delete&>(back);

        dst.SetDelNonSelected(src.delete_non_selected);

        sop::node::Delete::EntityType type;
        switch (src.entity_type)
        {
        case DeleteEntityType::Points:
            type = sop::node::Delete::EntityType::Points;
            break;
        case DeleteEntityType::Edges:
            type = sop::node::Delete::EntityType::Edges;
            break;
        case DeleteEntityType::Faces:
            type = sop::node::Delete::EntityType::Faces;
            break;
        default:
            assert(0);
        }
        dst.SetEntityType(type);

        dst.SetFilterExpr(src.filter_exp);
    }
    else if (type == rttr::type::get<node::Peak>())
    {
        auto& src = static_cast<const node::Peak&>(front);
        auto& dst = static_cast<sop::node::Peak&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetGroupType(TransGroupType(src.group_type));

        dst.SetDistance(ParseExprFloat(src.distance, back,
            sop::node::Peak::DIST, 0, eval));
    }
    else if (type == rttr::type::get<node::Transform>())
    {
        auto& src = static_cast<const node::Transform&>(front);
        auto& dst = static_cast<sop::node::Transform&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetGroupType(TransGroupType(src.group_type));

        sm::ivec3 trans_idx(sop::node::Transform::TRANS_X, sop::node::Transform::TRANS_Y, sop::node::Transform::TRANS_Z);
        dst.SetTranslate(ParseExprFloat3(src.translate, back, trans_idx, sm::vec3(0, 0, 0), eval));

        sm::ivec3 rot_idx(sop::node::Transform::ROT_X, sop::node::Transform::ROT_Y, sop::node::Transform::ROT_Z);
        dst.SetRotate(ParseExprFloat3(src.rotate, back, rot_idx, sm::vec3(0, 0, 0), eval));

        sm::ivec3 scale_idx(sop::node::Transform::SCALE_X, sop::node::Transform::SCALE_Y, sop::node::Transform::SCALE_Z);
        dst.SetScale(ParseExprFloat3(src.scale, back, scale_idx, sm::vec3(1, 1, 1), eval));

        sm::ivec3 shear_idx(sop::node::Transform::SHEAR_X, sop::node::Transform::SHEAR_Y, sop::node::Transform::SHEAR_Z);
        dst.SetShear(ParseExprFloat3(src.shear, back, shear_idx, sm::vec3(0, 0, 0), eval));
    }
    // material
    else if (type == rttr::type::get<node::Color>())
    {
        auto& src = static_cast<const node::Color&>(front);
        auto& dst = static_cast<sop::node::Color&>(back);
        dst.SetColor(src.color);
    }
    // NURBs
    else if (type == rttr::type::get<node::Carve>())
    {
        auto& src = static_cast<const node::Carve&>(front);
        auto& dst = static_cast<sop::node::Carve&>(back);

        dst.SetFirstU(ParseExprFloat(src.first_u, back,
            sop::node::Carve::FIRST_U, 0, eval));
        dst.SetSecondU(ParseExprFloat(src.second_u, back,
            sop::node::Carve::SECOND_U, 1, eval));
        dst.SetFirstV(ParseExprFloat(src.first_v, back,
            sop::node::Carve::FIRST_V, 0, eval));
        dst.SetSecondV(ParseExprFloat(src.second_v, back,
            sop::node::Carve::SECOND_V, 1, eval));
    }
    else if (type == rttr::type::get<node::Add>())
    {
        auto& src = static_cast<const node::Add&>(front);
        auto& dst = static_cast<sop::node::Add&>(back);
        std::vector<sm::vec3> points;
        if (src.use_p0) {
            points.push_back(src.p0);
        }
        if (src.use_p1) {
            points.push_back(src.p1);
        }
        if (src.use_p2) {
            points.push_back(src.p2);
        }
        if (src.use_p3) {
            points.push_back(src.p3);
        }
        dst.SetPoints(points);
    }
    // polygon
    else if (type == rttr::type::get<node::Boolean>())
    {
        auto& src = static_cast<const node::Boolean&>(front);
        auto& dst = static_cast<sop::node::Boolean&>(back);

        sop::node::Boolean::Operator op;
        switch (src.op)
        {
        case BooleanOperator::Union:
            op = sop::node::Boolean::Operator::Union;
            break;
        case BooleanOperator::Intersect:
            op = sop::node::Boolean::Operator::Intersect;
            break;
        case BooleanOperator::Subtract:
            op = sop::node::Boolean::Operator::Subtract;
            break;
        default:
            assert(0);
        }
        dst.SetOperator(op);
    }
    else if (type == rttr::type::get<node::Fuse>())
    {
        auto& src = static_cast<const node::Fuse&>(front);
        auto& dst = static_cast<sop::node::Fuse&>(back);

        sop::node::Fuse::Operator op;
        switch (src.op)
        {
        case sopv::FuseOperator::Consolidate:
            op = sop::node::Fuse::Operator::Consolidate;
            break;
        case sopv::FuseOperator::UniquePoints:
            op = sop::node::Fuse::Operator::UniquePoints;
            break;
        case sopv::FuseOperator::Snap:
            op = sop::node::Fuse::Operator::Snap;
            break;
        default:
            assert(0);
            op = sop::node::Fuse::Operator::Consolidate;
        }
        dst.SetFuseOP(op);

        dst.SetDistance(src.distance);
    }
    else if (type == rttr::type::get<node::Knife>())
    {
        auto& src = static_cast<const node::Knife&>(front);
        auto& dst = static_cast<sop::node::Knife&>(back);

        sm::ivec3 origin_idx(sop::node::Knife::ORIGINX_X, sop::node::Knife::ORIGINX_Y, sop::node::Knife::ORIGINX_Z);
        dst.SetOrigin(ParseExprFloat3(src.origin, back, origin_idx, sm::vec3(0, 0, 0), eval));

        dst.SetDirection(src.direction);

        sop::node::Knife::KeepType keep;
        switch (src.keep)
        {
        case KnifeKeep::KeepAbove:
            keep = sop::node::Knife::KeepType::KeepAbove;
            break;
        case KnifeKeep::KeepBelow:
            keep = sop::node::Knife::KeepType::KeepBelow;
            break;
        case KnifeKeep::KeepAll:
            keep = sop::node::Knife::KeepType::KeepAll;
            break;
        default:
            assert(0);
        }
        dst.SetKeepType(keep);
    }
    else if (type == rttr::type::get<node::Normal>())
    {
        auto& src = static_cast<const node::Normal&>(front);
        auto& dst = static_cast<sop::node::Normal&>(back);
        dst.SetAttrAddTo(TransGeoAttrClass(src.attr_add_norm_to));
    }
    else if (type == rttr::type::get<node::PolyExtrude>())
    {
        auto& src = static_cast<const node::PolyExtrude&>(front);
        auto& dst = static_cast<sop::node::PolyExtrude&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetDistance(src.distance);

        dst.SetOutputFront(src.output_front);
        dst.SetOutputBack(src.output_back);
        dst.SetOutputSide(src.output_side);

        dst.SetFrontGroupName(src.front_group);
        dst.SetBackGroupName(src.back_group);
        dst.SetSideGroupName(src.side_group);
    }
    else if (type == rttr::type::get<node::PolyFrame>())
    {
        auto& src = static_cast<const node::PolyFrame&>(front);
        auto& dst = static_cast<sop::node::PolyFrame&>(back);

        dst.SetEntityType(TransGroupType(src.entity_type));

        dst.SetFrameStyle(TransGroupType(src.frame_style));

        dst.SetNormalName(src.normal_name);
        dst.SetTangentName(src.tangent_name);
        dst.SetBitangentName(src.bitangent_name);
    }
    // primitive
    else if (type == rttr::type::get<node::Box>())
    {
        auto& src = static_cast<const node::Box&>(front);
        auto& dst = static_cast<sop::node::Box&>(back);
        auto& dst_props = const_cast<sop::NodePropsMgr&>(dst.GetProps());

        sm::ivec3 size_idx(sop::node::Box::SIZE_X, sop::node::Box::SIZE_Y, sop::node::Box::SIZE_Z);
        dst.SetSize(ParseExprFloat3(src.size, back, size_idx, sm::vec3(1, 1, 1), eval));

        sm::ivec3 pos_idx(sop::node::Box::POS_X, sop::node::Box::POS_Y, sop::node::Box::POS_Z);
        dst.SetCenter(ParseExprFloat3(src.center, back, pos_idx, sm::vec3(0, 0, 0), eval));

        dst.SetScale(sm::vec3(src.scale, src.scale, src.scale));
    }
    else if (type == rttr::type::get<node::Curve>())
    {
        auto& src = static_cast<const node::Curve&>(front);
        auto& dst = static_cast<sop::node::Curve&>(back);
        dst.SetVertices(src.vertices);
    }
    else if (type == rttr::type::get<node::Grid>())
    {
        auto& src = static_cast<const node::Grid&>(front);
        auto& dst = static_cast<sop::node::Grid&>(back);

        dst.SetSize(sm::ivec2(src.size_x, src.size_y));

        dst.SetRows(src.rows);
        dst.SetColumns(src.columns);
    }
    else if (type == rttr::type::get<node::Line>())
    {
        auto& src = static_cast<const node::Line&>(front);
        auto& dst = static_cast<sop::node::Line&>(back);

        dst.SetOrigin(src.origin);
        dst.SetDirection(src.direction);
        dst.SetLength(ParseExprFloat(src.length, back,
            sop::node::Line::LENGTH, 1.0f, eval));
        dst.SetPoints(src.points);
    }
    else if (type == rttr::type::get<node::Primitive>())
    {
        auto& src = static_cast<const node::Primitive&>(front);
        auto& dst = static_cast<sop::node::Primitive&>(back);

        dst.SetGroupName(src.group_name.str);

        dst.SetTranslate(src.translate);
        dst.SetRotate(src.rotate);
        dst.SetScale(src.scale);
        dst.SetShear(src.shear);
    }
    // utility
    else if (type == rttr::type::get<node::Blast>())
    {
        auto& src = static_cast<const node::Blast&>(front);
        auto& dst = static_cast<sop::node::Blast&>(back);

        dst.SetGroupName(src.group_name.str);
        dst.SetGroupType(TransGroupType(src.group_type));
        dst.SetDeleteNonSelected(src.delete_non_selected);
    }
    else if (type == rttr::type::get<node::CopyToPoints>())
    {
        auto& src = static_cast<const node::CopyToPoints&>(front);
        auto& dst = static_cast<sop::node::CopyToPoints&>(back);

        dst.SetSrcGroup(src.src_group.str);

        if (!src.target_group.str.empty()) {
            dst.SetTargetGroup(src.target_group.str);
        } else {
            dst.SetTargetGroup(src.target_group_str);
        }

        dst.EnableUsePointDir(src.use_pt_dir);
    }
    else if (type == rttr::type::get<node::ForeachPrimEnd>())
    {
        auto& src = static_cast<const node::ForeachPrimEnd&>(front);
        auto& dst = static_cast<sop::node::ForeachPrimEnd&>(back);

        dst.EnableSinglePass(src.do_single_pass);
        dst.SetSinglePassOffset(src.single_pass_offset);
    }
    else if (type == rttr::type::get<node::Split>())
    {
        auto& src = static_cast<const node::Split&>(front);
        auto& dst = static_cast<sop::node::Split&>(back);
        dst.SetGroupName(src.group_name.str);
    }
    else if (type == rttr::type::get<node::Switch>())
    {
        auto& src = static_cast<const node::Switch&>(front);
        auto& dst = static_cast<sop::node::Switch&>(back);
        dst.SetSelected(ParseExprInt(src.selected, back,
            sop::node::Switch::SELECTED, 0, eval));
    }

    // update props
    if (type.is_derived_from<Node>())
    {
        auto& src = static_cast<const Node&>(front);
        auto& src_props = src.GetProps();
        if (src_props)
        {
            auto& dst_props = const_cast<sop::NodePropsMgr&>(back.GetProps());
            dst_props.Clear();
            for (auto& sp : src_props->props)
            {
                sop::Variable d_val;
                try {
                    d_val.type = sop::VarType::Float;
                    d_val.f = boost::lexical_cast<float>(sp.value);
                } catch (boost::bad_lexical_cast&) {
                    continue;
                }
                dst_props.Add(sp.name, d_val);
            }
        }
    }
}

sop::NodePtr SOP::CreateBackFromFront(const bp::Node& node)
{
    auto type = node.get_type();
    auto src_type = type.get_name().to_string();
    std::string dst_type;
    std::string lib_str = "sop";
    auto find_lib = src_type.find("sopv::");
    if (find_lib != std::string::npos) {
        dst_type = lib_str + "::" + src_type.substr(find_lib + strlen("sopv::"));
    }

    sop::NodePtr dst = nullptr;

    if (!dst_type.empty())
    {
	    rttr::type t = rttr::type::get_by_name(dst_type);
        // fixme: specify node type
	    if (!t.is_valid())
        {
            assert(0);

            //dst = std::make_shared<sop::Node>();
            //InitPortsBackFromFront(*dst, *node);
	    }
        else
        {
            rttr::variant var = t.create();
            assert(var.is_valid());

            dst = var.get_value<std::shared_ptr<sop::Node>>();
            assert(dst);
        }
    }

    if (node.get_type().is_derived_from<Node>()) {
        dst->SetName(static_cast<const Node&>(node).GetName());
    }

    return dst;
}

int SOP::TypeBackToFront(sop::NodeVarType type)
{
    int ret = -1;

    switch (type)
    {
    case sop::NodeVarType::Any:
        ret = bp::PIN_ANY_VAR;
        break;
    case sop::NodeVarType::Port:
        ret = bp::PIN_PORT;
        break;
    case sop::NodeVarType::Primitive:
        ret = PIN_PRIMITIVE;
        break;
    }

    return ret;
}

sop::NodeVarType SOP::TypeFrontToBack(int pin_type)
{
    sop::NodeVarType ret = sop::NodeVarType::Any;

    switch (pin_type)
    {
    case bp::PIN_ANY_VAR:
        ret = sop::NodeVarType::Any;
        break;
    case bp::PIN_PORT:
        ret = sop::NodeVarType::Port;
        break;
    case PIN_PRIMITIVE:
        ret = sop::NodeVarType::Primitive;
        break;
    }

    return ret;
}

sop::GeoAttrClass
SOP::TransGeoAttrClass(sopv::GeoAttrClass cls)
{
    switch (cls)
    {
    case sopv::GeoAttrClass::Point:
        return sop::GeoAttrClass::Point;
    case sopv::GeoAttrClass::Vertex:
        return sop::GeoAttrClass::Vertex;
    case sopv::GeoAttrClass::Primitive:
        return sop::GeoAttrClass::Primitive;
    case sopv::GeoAttrClass::Detail:
        return sop::GeoAttrClass::Detail;
    default:
        assert(0);
        return sop::GeoAttrClass::Point;
    }
}

int SOP::ParseExprInt(const std::string& src, const sop::Node& dst,
                             size_t idx, int expect, const Evaluator& eval)
{
    int ret = expect;

    auto& dst_props = const_cast<sop::NodePropsMgr&>(dst.GetProps());
    try {
        ret = boost::lexical_cast<int>(src);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx, src);
        ret = eval.CalcInt(src, dst, 0);
    }

    return ret;
}

float SOP::ParseExprFloat(const std::string& src, const sop::Node& dst,
                                 size_t idx, float expect, const Evaluator& eval)
{
    float ret = expect;

    auto& dst_props = const_cast<sop::NodePropsMgr&>(dst.GetProps());
    try {
        ret = boost::lexical_cast<float>(src);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx, src);
        ret = eval.CalcFloat(src, dst, expect);
    }

    return ret;
}

sm::vec3 SOP::ParseExprFloat3(const StrVec3& src, const sop::Node& dst,
                                     const sm::ivec3& idx, const sm::vec3& expect,
                                     const Evaluator& eval)
{
    sm::vec3 ret = expect;

    auto& dst_props = const_cast<sop::NodePropsMgr&>(dst.GetProps());
    try {
        ret.x = boost::lexical_cast<float>(src.x);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx.x, src.x);
        ret.x = eval.CalcFloat(src.x, dst, 1.0f);
    }
    try {
        ret.y = boost::lexical_cast<float>(src.y);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx.y, src.y);
        ret.y = eval.CalcFloat(src.y, dst, 1.0f);
    }
    try {
        ret.z = boost::lexical_cast<float>(src.z);
    } catch (boost::bad_lexical_cast&) {
        dst_props.SetExpr(idx.z, src.z);
        ret.z = eval.CalcFloat(src.z, dst, 1.0f);
    }

    return ret;
}

}