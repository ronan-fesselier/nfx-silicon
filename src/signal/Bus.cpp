#include "nfx/silicon/signal/Bus.h"

#include "internal/runtime/Error.h"

#include <cassert>

namespace nfx::silicon::signal
{
    Bus::Bus(const Descriptor& descriptor)
        : m_descriptor{ descriptor }
    {
        for (auto* w : m_descriptor.wires)
        {
            if (w->descriptor().kind != m_descriptor.kind)
            {
                internal::runtime::error::log(
                    "Bus",
                    internal::runtime::error::Level::Critical,
                    internal::runtime::error::Kind::Contract,
                    "wire kind does not match bus kind");
                assert(false && "Bus: wire kind does not match bus kind");
            }
        }
    }

    std::size_t Bus::width() const
    {
        return m_descriptor.wires.size();
    }

    Wire& Bus::wire(std::size_t index) const
    {
        if (index >= m_descriptor.wires.size())
        {
            internal::runtime::error::log(
                "Bus",
                internal::runtime::error::Level::Critical,
                internal::runtime::error::Kind::Programming,
                "wire index out of range");
            assert(false && "Bus: wire index out of range");
        }
        return *m_descriptor.wires[index];
    }

    template <>
    void Bus::read<Level>(std::span<Level> out) const
    {
        if (out.size() != m_descriptor.wires.size())
        {
            internal::runtime::error::log(
                "Bus",
                internal::runtime::error::Level::Critical,
                internal::runtime::error::Kind::Contract,
                "output span size does not match bus width");
            assert(false && "Bus: output span size does not match bus width");
        }
        for (std::size_t i = 0; i < m_descriptor.wires.size(); ++i)
        {
            out[i] = m_descriptor.wires[i]->read<Level>();
        }
    }

    template <>
    void Bus::read<float>(std::span<Voltage> out) const
    {
        if (out.size() != m_descriptor.wires.size())
        {
            internal::runtime::error::log(
                "Bus",
                internal::runtime::error::Level::Critical,
                internal::runtime::error::Kind::Contract,
                "output span size does not match bus width");
            assert(false && "Bus: output span size does not match bus width");
        }
        for (std::size_t i = 0; i < m_descriptor.wires.size(); ++i)
        {
            out[i] = m_descriptor.wires[i]->read<float>();
        }
    }
} // namespace nfx::silicon::signal
