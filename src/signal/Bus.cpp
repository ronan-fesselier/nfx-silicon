#include "nfx/silicon/signal/Bus.h"

#include <cassert>

namespace nfx::silicon::signal
{
    Bus::Bus(const Descriptor& descriptor)
        : m_descriptor{ descriptor }
    {
        for (auto* w : m_descriptor.wires)
        {
            assert(w->descriptor().kind == m_descriptor.kind && "Wire kind does not match Bus kind");
        }
    }

    std::size_t Bus::width() const
    {
        return m_descriptor.wires.size();
    }

    Wire& Bus::wire(std::size_t index) const
    {
        assert(index < m_descriptor.wires.size() && "Bus wire index out of range");
        return *m_descriptor.wires[index];
    }

    template <>
    void Bus::read<Level>(std::span<Level> out) const
    {
        assert(out.size() == m_descriptor.wires.size() && "Output span size does not match bus width");
        for (std::size_t i = 0; i < m_descriptor.wires.size(); ++i)
        {
            out[i] = m_descriptor.wires[i]->read<Level>();
        }
    }

    template <>
    void Bus::read<float>(std::span<Voltage> out) const
    {
        assert(out.size() == m_descriptor.wires.size() && "Output span size does not match bus width");
        for (std::size_t i = 0; i < m_descriptor.wires.size(); ++i)
        {
            out[i] = m_descriptor.wires[i]->read<float>();
        }
    }
} // namespace nfx::silicon::signal
