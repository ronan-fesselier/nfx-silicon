#include "nfx/silicon/latch/sn74xxx/LS373.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::latch::sn74xxx
{
    namespace
    {
        using Kind = internal::runtime::error::Kind;

        signal::Pin makeInput(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{
                .name = name, .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Input } };
        }

        signal::Pin makeOutput(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{
                .name = name, .kind = signal::Pin::Kind::Digital, .direction = signal::Pin::Direction::Output } };
        }

        signal::Pin makePower(const char* name)
        {
            return signal::Pin{ signal::Pin::Descriptor{
                .name = name, .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Input } };
        }

        void gateOutput(signal::Pin& pin, bool powered, bool outputsEnabled, const Level level)
        {
            if (!powered || !outputsEnabled)
            {
                pin.release();
            }
            else
            {
                pin.drive<Level>(level);
            }
        }
    } // namespace

    LS373::LS373(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_latches{ primitive::DLatch{ primitive::DLatch::Descriptor{ .name = "L0" } },
                     primitive::DLatch{ primitive::DLatch::Descriptor{ .name = "L1" } },
                     primitive::DLatch{ primitive::DLatch::Descriptor{ .name = "L2" } },
                     primitive::DLatch{ primitive::DLatch::Descriptor{ .name = "L3" } },
                     primitive::DLatch{ primitive::DLatch::Descriptor{ .name = "L4" } },
                     primitive::DLatch{ primitive::DLatch::Descriptor{ .name = "L5" } },
                     primitive::DLatch{ primitive::DLatch::Descriptor{ .name = "L6" } },
                     primitive::DLatch{ primitive::DLatch::Descriptor{ .name = "L7" } } },
          m_pinOC{ makeInput("OC") },
          m_pinQ1{ makeOutput("1Q") },
          m_pinD1{ makeInput("1D") },
          m_pinD2{ makeInput("2D") },
          m_pinQ2{ makeOutput("2Q") },
          m_pinQ3{ makeOutput("3Q") },
          m_pinD3{ makeInput("3D") },
          m_pinD4{ makeInput("4D") },
          m_pinQ4{ makeOutput("4Q") },
          m_pinGND{ makePower("GND") },
          m_pinC{ makeInput("C") },
          m_pinQ5{ makeOutput("5Q") },
          m_pinD5{ makeInput("5D") },
          m_pinD6{ makeInput("6D") },
          m_pinQ6{ makeOutput("6Q") },
          m_pinQ7{ makeOutput("7Q") },
          m_pinD7{ makeInput("7D") },
          m_pinD8{ makeInput("8D") },
          m_pinQ8{ makeOutput("8Q") },
          m_pinVCC{ makePower("VCC") },
          m_pinPtrs{ &m_pinOC, &m_pinQ1, &m_pinD1,  &m_pinD2, &m_pinQ2, &m_pinQ3, &m_pinD3,
                     &m_pinD4, &m_pinQ4, &m_pinGND, &m_pinC,  &m_pinQ5, &m_pinD5, &m_pinD6,
                     &m_pinQ6, &m_pinQ7, &m_pinD7,  &m_pinD8, &m_pinQ8, &m_pinVCC }
    {
        // OC callback
        m_pinOC.connect<Level>([this](const Level l) {
            m_outputsEnabled = (l == Level::Low);
            if (!m_powered)
            {
                return;
            }
            onOC(l);
        });

        // D inputs and C -> internal latch wiring
        auto wireLatch = [this](const std::size_t idx, signal::Pin& pinD, signal::Pin& pinQ) {
            pinD.connect<Level>([this, idx](const Level l) {
                if (!m_powered)
                {
                    return;
                }
                m_latches[idx].pin(primitive::DLatch::Terminal::D).drive<Level>(l);
            });

            m_latches[idx].pin(primitive::DLatch::Terminal::Q).connect<Level>([this, &pinQ](const Level l) {
                gateOutput(pinQ, m_powered, m_outputsEnabled, l);
            });
        };

        wireLatch(0, m_pinD1, m_pinQ1);
        wireLatch(1, m_pinD2, m_pinQ2);
        wireLatch(2, m_pinD3, m_pinQ3);
        wireLatch(3, m_pinD4, m_pinQ4);
        wireLatch(4, m_pinD5, m_pinQ5);
        wireLatch(5, m_pinD6, m_pinQ6);
        wireLatch(6, m_pinD7, m_pinQ7);
        wireLatch(7, m_pinD8, m_pinQ8);

        // C -> all internal latch EN pins
        m_pinC.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            for (auto& latch : m_latches)
            {
                latch.pin(primitive::DLatch::Terminal::EN).drive<Level>(l);
            }
        });

        // VCC monitoring
        m_pinVCC.connect<float>([this](const Voltage v) { onVCC(v); });
    }

    const char* LS373::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& LS373::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "LS373", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "LS373: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& LS373::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::OC:
                return m_pinOC;
            case Terminal::Q1:
                return m_pinQ1;
            case Terminal::D1:
                return m_pinD1;
            case Terminal::D2:
                return m_pinD2;
            case Terminal::Q2:
                return m_pinQ2;
            case Terminal::Q3:
                return m_pinQ3;
            case Terminal::D3:
                return m_pinD3;
            case Terminal::D4:
                return m_pinD4;
            case Terminal::Q4:
                return m_pinQ4;
            case Terminal::GND:
                return m_pinGND;
            case Terminal::C:
                return m_pinC;
            case Terminal::Q5:
                return m_pinQ5;
            case Terminal::D5:
                return m_pinD5;
            case Terminal::D6:
                return m_pinD6;
            case Terminal::Q6:
                return m_pinQ6;
            case Terminal::Q7:
                return m_pinQ7;
            case Terminal::D7:
                return m_pinD7;
            case Terminal::D8:
                return m_pinD8;
            case Terminal::Q8:
                return m_pinQ8;
            case Terminal::VCC:
                return m_pinVCC;
        }
        internal::runtime::error::log(
            "LS373", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "LS373: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> LS373::pins() const
    {
        return m_pinPtrs;
    }

    void LS373::reset()
    {
        for (auto& latch : m_latches)
        {
            latch.reset();
        }
    }

    void LS373::onVCC(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vccMin && v.value() <= k_vccMax;

        if (!wasPowered && m_powered)
        {
            for (auto& latch : m_latches)
            {
                latch.reset();
                latch.pin(primitive::DLatch::Terminal::EN).drive<Level>(m_pinC.read<Level>());
            }
            onOC(m_pinOC.read<Level>());
        }
        else if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "LS373",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VCC not powered: outputs held at HighZ");
            m_pinQ1.release();
            m_pinQ2.release();
            m_pinQ3.release();
            m_pinQ4.release();
            m_pinQ5.release();
            m_pinQ6.release();
            m_pinQ7.release();
            m_pinQ8.release();
        }
    }

    void LS373::onOC(const Level /*level*/)
    {
        const bool oeActive = m_outputsEnabled;

        signal::Pin* qPins[8] = { &m_pinQ1, &m_pinQ2, &m_pinQ3, &m_pinQ4, &m_pinQ5, &m_pinQ6, &m_pinQ7, &m_pinQ8 };

        for (std::size_t i = 0; i < 8; ++i)
        {
            const Level latchQ = m_latches[i].pin(primitive::DLatch::Terminal::Q).read<Level>();
            gateOutput(*qPins[i], m_powered, oeActive, latchQ);
        }
    }
} // namespace nfx::silicon::latch::sn74xxx
