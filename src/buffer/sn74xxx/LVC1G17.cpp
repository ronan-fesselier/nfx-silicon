#include "nfx/silicon/buffer/sn74xxx/LVC1G17.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::buffer::sn74xxx
{
    namespace
    {
        using Kind = internal::runtime::error::Kind;
    } // namespace

    LVC1G17::LVC1G17(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinNC{ signal::Pin::Descriptor{
              .name = "NC", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
          m_pinA{ signal::Pin::Descriptor{
              .name = "A", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } },
          m_pinGND{ signal::Pin::Descriptor{
              .name = "GND", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Input } },
          m_pinY{ signal::Pin::Descriptor{
              .name = "Y", .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Output } },
          m_pinVCC{ signal::Pin::Descriptor{
              .name = "VCC", .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Input } },
          m_pinPtrs{ &m_pinNC, &m_pinA, &m_pinGND, &m_pinY, &m_pinVCC }
    {
        m_pinA.connect<Level>([this](const Level) { compute(); });
        m_pinVCC.connect<float>([this](const Voltage v) { onVCC(v); });
    }

    const char* LVC1G17::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& LVC1G17::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "LVC1G17", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "LVC1G17: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& LVC1G17::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::NC:
                return m_pinNC;
            case Terminal::A:
                return m_pinA;
            case Terminal::GND:
                return m_pinGND;
            case Terminal::Y:
                return m_pinY;
            case Terminal::VCC:
                return m_pinVCC;
        }
        internal::runtime::error::log(
            "LVC1G17", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "LVC1G17: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> LVC1G17::pins() const
    {
        return m_pinPtrs;
    }

    void LVC1G17::onVCC(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vccMin && v.value() <= k_vccMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "LVC1G17",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VCC not powered: output held at HighZ");
            m_pinY.release();
            return;
        }

        compute();
    }

    void LVC1G17::compute()
    {
        if (!m_powered)
            return;

        const Level a = m_pinA.read<Level>();
        if (a == Level::HighZ)
        {
            m_pinY.release();
            return;
        }

        m_pinY.drive<Level>(a);
    }
} // namespace nfx::silicon::buffer::sn74xxx
