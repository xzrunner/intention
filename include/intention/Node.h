#pragma once

#include <blueprint/Node.h>

namespace itt
{

struct NodePropArray;

class Node : public bp::Node
{
public:
    Node(const std::string& title, bool props = false);
    Node(const Node& node);
    Node& operator = (const Node& node);
    virtual ~Node();

    virtual void Draw(const n2::RenderParams& rp) const override;

    virtual void StoreToJson(const std::string& dir, rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) override;
    virtual void LoadFromJson(const std::string& dir, const rapidjson::Value& val) override;

    auto& GetName() const { return m_name; }
    void  SetName(const std::string& name) { m_name = name; }

    bool GetTemplate() const { return m_template; }
    void SetTemplate(bool temp) { m_template = temp; }

    bool GetDisplay() const { return m_display; }
    void SetDisplay(bool display) { m_display = display; }

    auto& GetProps() const { return m_props; }

protected:
    struct PinDesc
    {
        bool operator == (const PinDesc& desc) const {
            return type == desc.type && name == desc.name;
        }

        int         type;
        std::string name;
    };

public:
    static const char* STR_PROP_DISPLAY;

protected:
    void InitPins(const std::vector<PinDesc>& input,
        const std::vector<PinDesc>& output);
    void InitPins(const std::string& name);

private:
    void InitPinsImpl(const std::vector<PinDesc>& pins,
        bool is_input);

private:
    std::string m_name;

    bool m_bypass = false;
    bool m_lock   = false;

    bool m_template = false;
    bool m_display  = false;

    std::unique_ptr<NodePropArray> m_props = nullptr;

    RTTR_ENABLE(bp::Node)

}; // Node

}