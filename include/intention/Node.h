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
    bool m_display = false;

    RTTR_ENABLE(bp::Node)

}; // Node

}