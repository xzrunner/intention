#pragma once

#include <blueprint/Node.h>

namespace itt
{

class Node : public bp::Node
{
public:
    Node(const std::string& title);
    virtual ~Node();

    virtual void Draw(const n2::RenderParams& rp) const override;

    bool GetDisplay() const { return m_display; }
    void SetDisplay(bool display) { m_display = display; }

    struct PinDesc
    {
        bool operator == (const PinDesc& desc) const {
            return type == desc.type && name == desc.name;
        }

        int         type;
        std::string name;
    };

protected:
    void InitPins(const std::vector<PinDesc>& input,
        const std::vector<PinDesc>& output);
    void InitPins(const std::string& name);

private:
    void InitPinsImpl(const std::vector<PinDesc>& pins,
        bool is_input);

private:
    bool m_bypass = false;
    bool m_lock   = false;

    bool m_toggle  = false;
    bool m_display = true;

    RTTR_ENABLE(bp::Node)

}; // Node

}