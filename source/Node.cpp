#include "sopview/Node.h"
#include "sopview/SOPAdapter.h"
#include "sopview/NodeParm.h"

#include <blueprint/Pin.h>
#include <blueprint/RenderSystem.h>

#include <sop/Node.h>
#include <node2/RenderSystem.h>
#include <painting2/RenderSystem.h>

namespace sopv
{

const char* Node::STR_PROP_DISPLAY = "Display";

Node::Node(const std::string& title, bool parms)
    : bp::Node(title)
{
    if (parms) {
        m_parms = std::make_unique<NodeParmArray>();
    }

    m_style.hori = false;
}

Node::Node(const Node& node)
    : bp::Node(node)
{
    operator = (node);
}

Node& Node::operator = (const Node& node)
{
    m_name = node.m_name;

    m_bypass = node.m_bypass;
    m_lock   = node.m_lock;

    m_template = node.m_template;
    m_display  = node.m_display;

    if (node.m_parms) {
        m_parms = std::make_unique<NodeParmArray>(*node.m_parms);
    }

    return *this;
}

Node::~Node()
{
}

void Node::Draw(const ur2::Device& dev, ur2::Context& ctx,
                const n2::RenderParams& rp) const
{
    bp::Node::Draw(dev, ctx, rp);

    // draw name
    sm::Matrix2D mat;
    auto s = bp::RenderSystem::Instance()->GetTextTitleScale();
    mat.Scale(s, s);
    auto pos = rp.GetMatrix() * sm::vec2(0, 0);
    mat.Translate(pos.x + m_style.width * 0.8f, pos.y);
    pt2::RenderSystem::DrawText(m_name, bp::RenderSystem::Instance()->GetTitleTB(),
        mat, bp::RenderSystem::COL_TEXT, bp::RenderSystem::COL_ZERO);

    //if (m_preview) {
    //    auto eval = Blackboard::Instance()->GetEval();
    //    if (eval) {
    //        NodePreview::Draw(*eval, *this, rp);
    //    }
    //}
}

void Node::StoreToJson(const std::string& dir, rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc)
{
    bp::Node::StoreToJson(dir, val, alloc);

    if (m_parms && !m_parms->parms.empty())
    {
        rapidjson::Value val_parms;
        val_parms.SetArray();

        for (auto& p : m_parms->parms)
        {
            rapidjson::Value val_p;
            val_p.SetObject();
            val_p.AddMember("name", rapidjson::Value(p.name.c_str(), alloc), alloc);
#ifdef EnableNodeParmType
            auto type_str = NodeParmStrings[static_cast<int>(p.type)];
            val_p.AddMember("type", rapidjson::Value(type_str, alloc), alloc);
#endif // EnableNodeParmType
            val_p.AddMember("value", rapidjson::Value(p.value.c_str(), alloc), alloc);
            val_parms.PushBack(val_p, alloc);
        }

        val.AddMember("node_parms", val_parms, alloc);
    }
}

void Node::LoadFromJson(const std::string& dir, const rapidjson::Value& val)
{
    bp::Node::LoadFromJson(dir, val);

    if (val.HasMember("node_parms"))
    {
        if (!m_parms) {
            m_parms = std::make_unique<NodeParmArray>();
        }

        auto array = val["node_parms"].GetArray();
        m_parms->parms.clear();
        m_parms->parms.resize(array.Size());
        for (int i = 0, n = array.Size(); i < n; ++i)
        {
            auto& src = array[i];
            auto& dst = m_parms->parms[i];

            dst.name = src["name"].GetString();

#ifdef EnableNodeParmType
            auto type = src["type"].GetString();
            bool finded = false;
            for (int j = 0, m = (int)NodeParmType::MaxNum; j < m; ++j) {
                if (strcmp(type, NodeParmStrings[j]) == 0) {
                    dst.type = static_cast<NodeParmType>(j);
                    finded = true;
                    break;
                }
            }
            assert(finded);
#endif // EnableNodeParmType

            dst.value = src["value"].GetString();
        }
    }
}

void Node::InitPins(const std::vector<PinDesc>& input,
                    const std::vector<PinDesc>& output)
{
    if (input.size() <= 1 && output.size() <= 1)
    {
        if ((input.empty() || input[0].name == "in") &&
            (output.empty() || output[0].name == "out")) {
            m_style.draw_pin_label = false;
        }
    }

    InitPinsImpl(input, true);
    InitPinsImpl(output, false);

    Layout();
}

void Node::InitPins(const std::string& name)
{
	rttr::type t = rttr::type::get_by_name("sop::" + name);
    if (!t.is_valid()) {
        return;
    }

	rttr::variant var = t.create();
	assert(var.is_valid());

	auto method_imports = t.get_method("GetImports");
	assert(method_imports.is_valid());
	auto var_imports = method_imports.invoke(var);
	assert(var_imports.is_valid()
		&& var_imports.is_type<std::vector<dag::Node<sop::NodeVarType>::Port>>());
	auto& imports = var_imports.get_value<std::vector<dag::Node<sop::NodeVarType>::Port>>();

	auto method_exports = t.get_method("GetExports");
	assert(method_exports.is_valid());
	auto var_exports = method_exports.invoke(var);
	assert(var_exports.is_valid()
		&& var_exports.is_type<std::vector<dag::Node<sop::NodeVarType>::Port>>());
	auto& exports = var_exports.get_value<std::vector<dag::Node<sop::NodeVarType>::Port>>();

	auto port_back2front = [](std::vector<PinDesc>& dst, const std::vector<dag::Node<sop::NodeVarType>::Port>& src)
	{
		dst.reserve(dst.size() + src.size());
		for (int i = 0, n = src.size(); i < n; ++i)
		{
            PinDesc d;

			auto& s = src[i];
            d.type = SOPAdapter::TypeBackToFront(s.var.type);
            d.name = s.var.full_name;

            dst.push_back(d);
		}
	};

	std::vector<PinDesc> input, output;
	port_back2front(input, imports);
	port_back2front(output, exports);

	InitPins(input, output);
}

void Node::InitPinsImpl(const std::vector<PinDesc>& pins, bool is_input)
{
    auto& dst = is_input ? m_all_input : m_all_output;
	dst.clear();
	dst.reserve(pins.size());
	int idx = 0;
	for (auto& d : pins)
	{
		auto p = std::make_shared<bp::Pin>(is_input, idx++, d.type, d.name, *this);
		if (!CheckPinName(*p, dst)) {
			assert(0);
			return;
		}
		dst.push_back(p);
	}
}

}