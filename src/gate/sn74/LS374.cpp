#include "nfx/silicon/gate/sn74xxx/LS374.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::gate::sn74
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

        void gateOutput(signal::Pin& pin, bool powered, bool outputsEnabled, Level level)
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

    LS374::LS374(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_flipflops{
              DFlipFlop{ DFlipFlop::Descriptor{ .name = "FF0" } }, DFlipFlop{ DFlipFlop::Descriptor{ .name = "FF1" } },
              DFlipFlop{ DFlipFlop::Descriptor{ .name = "FF2" } }, DFlipFlop{ DFlipFlop::Descriptor{ .name = "FF3" } },
              DFlipFlop{ DFlipFlop::Descriptor{ .name = "FF4" } }, DFlipFlop{ DFlipFlop::Descriptor{ .name = "FF5" } },
              DFlipFlop{ DFlipFlop::Descriptor{ .name = "FF6" } }, DFlipFlop{ DFlipFlop::Descriptor{ .name = "FF7" } }
          },
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
          m_pinCLK{ makeInput("CLK") },
          m_pinQ5{ makeOutput("5Q") },
          m_pinD5{ makeInput("5D") },
          m_pinD6{ makeInput("6D") },
          m_pinQ6{ makeOutput("6Q") },
          m_pinQ7{ makeOutput("7Q") },
          m_pinD7{ makeInput("7D") },
          m_pinD8{ makeInput("8D") },
          m_pinQ8{ makeOutput("8Q") },
          m_pinVCC{ makePower("VCC") },
          m_pinPtrs{ &m_pinOC, &m_pinQ1, &m_pinD1,  &m_pinD2,  &m_pinQ2, &m_pinQ3, &m_pinD3,
                     &m_pinD4, &m_pinQ4, &m_pinGND, &m_pinCLK, &m_pinQ5, &m_pinD5, &m_pinD6,
                     &m_pinQ6, &m_pinQ7, &m_pinD7,  &m_pinD8,  &m_pinQ8, &m_pinVCC }
    {
        // OC callback
        m_pinOC.connect<Level>([this](Level l) {
            m_outputsEnabled = (l == Level::Low);
            if (!m_powered)
                return;
            onOC(l);
        });

        // CLK -> all internal FF CLK pins
        m_pinCLK.connect<Level>([this](Level l) {
            if (!m_powered)
                return;
            for (auto& ff : m_flipflops)
            {
                ff.pin(DFlipFlop::Terminal::CLK).drive<Level>(l);
            }
        });

        // D inputs + Q outputs wiring for each FF
        auto wireFF = [this](std::size_t idx, signal::Pin& pinD, signal::Pin& pinQ) {
            pinD.connect<Level>([this, idx](Level l) {
                if (!m_powered)
                    return;
                m_flipflops[idx].pin(DFlipFlop::Terminal::D).drive<Level>(l);
            });

            // internal Q -> package output pin
            m_flipflops[idx].pin(DFlipFlop::Terminal::Q).connect<Level>([this, &pinQ](Level l) {
                gateOutput(pinQ, m_powered, m_outputsEnabled, l);
            });
        };

        wireFF(0, m_pinD1, m_pinQ1);
        wireFF(1, m_pinD2, m_pinQ2);
        wireFF(2, m_pinD3, m_pinQ3);
        wireFF(3, m_pinD4, m_pinQ4);
        wireFF(4, m_pinD5, m_pinQ5);
        wireFF(5, m_pinD6, m_pinQ6);
        wireFF(6, m_pinD7, m_pinQ7);
        wireFF(7, m_pinD8, m_pinQ8);

        // VCC monitoring
        m_pinVCC.connect<float>([this](Voltage v) { onVCC(v); });
    }

    const char* LS374::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& LS374::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "LS374", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "LS374: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& LS374::pin(Terminal terminal)
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
            case Terminal::CLK:
                return m_pinCLK;
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
            "LS374", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "LS374: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> LS374::pins() const
    {
        return m_pinPtrs;
    }

    void LS374::reset()
    {
        for (auto& ff : m_flipflops)
        {
            ff.reset();
        }
    }

    void LS374::onVCC(Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vccMin && v.value() <= k_vccMax;

        if (!wasPowered && m_powered)
        {
            // reset all FFs on power-up
            for (auto& ff : m_flipflops)
                ff.reset();
            // re-evaluate OC
            onOC(m_pinOC.read<Level>());
        }
        else if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "LS374",
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

    void LS374::onOC(Level /*level*/)
    {
        signal::Pin* qPins[8] = { &m_pinQ1, &m_pinQ2, &m_pinQ3, &m_pinQ4, &m_pinQ5, &m_pinQ6, &m_pinQ7, &m_pinQ8 };

        for (std::size_t i = 0; i < 8; ++i)
        {
            const Level ffQ = m_flipflops[i].pin(DFlipFlop::Terminal::Q).read<Level>();
            gateOutput(*qPins[i], m_powered, m_outputsEnabled, ffQ);
        }
    }
} // namespace nfx::silicon::gate::sn74
