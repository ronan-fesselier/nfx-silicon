#include "nfx/silicon/gate/cd4xxx/CD4011B.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::gate::cd4xxx
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
    } // namespace

    CD4011B::CD4011B(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_gates{ Nand{ Nand::Descriptor{ .name = "NAND1" } },
                   Nand{ Nand::Descriptor{ .name = "NAND2" } },
                   Nand{ Nand::Descriptor{ .name = "NAND3" } },
                   Nand{ Nand::Descriptor{ .name = "NAND4" } } },
          m_pinA{ makeInput("A") },
          m_pinB{ makeInput("B") },
          m_pinJ{ makeOutput("J") },
          m_pinK{ makeOutput("K") },
          m_pinC{ makeInput("C") },
          m_pinD{ makeInput("D") },
          m_pinVSS{ makePower("VSS") },
          m_pinE{ makeInput("E") },
          m_pinF{ makeInput("F") },
          m_pinL{ makeOutput("L") },
          m_pinM{ makeOutput("M") },
          m_pinG{ makeInput("G") },
          m_pinH{ makeInput("H") },
          m_pinVDD{ makePower("VDD") },
          m_pinPtrs{ &m_pinA, &m_pinB, &m_pinJ, &m_pinK, &m_pinC, &m_pinD, &m_pinVSS,
                     &m_pinE, &m_pinF, &m_pinL, &m_pinM, &m_pinG, &m_pinH, &m_pinVDD }
    {
        auto wireGate = [this](const std::size_t idx, signal::Pin& pinIn0, signal::Pin& pinIn1, signal::Pin& pinOut) {
            pinIn0.connect<Level>([this, idx](const Level l) {
                if (!m_powered)
                {
                    return;
                }
                m_gates[idx].pin(Nand::Terminal::A).drive<Level>(l);
            });

            pinIn1.connect<Level>([this, idx](const Level l) {
                if (!m_powered)
                {
                    return;
                }
                m_gates[idx].pin(Nand::Terminal::B).drive<Level>(l);
            });

            m_gates[idx].pin(Nand::Terminal::Y).connect<Level>([this, &pinOut](const Level l) {
                if (!m_powered)
                {
                    return;
                }
                pinOut.drive<Level>(l);
            });
        };

        wireGate(0, m_pinA, m_pinB, m_pinJ);
        wireGate(1, m_pinC, m_pinD, m_pinK);
        wireGate(2, m_pinE, m_pinF, m_pinL);
        wireGate(3, m_pinG, m_pinH, m_pinM);

        m_pinVDD.connect<float>([this](const Voltage v) { onVDD(v); });
    }

    const char* CD4011B::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& CD4011B::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "CD4011B", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "CD4011B: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& CD4011B::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::A:
                return m_pinA;
            case Terminal::B:
                return m_pinB;
            case Terminal::J:
                return m_pinJ;
            case Terminal::K:
                return m_pinK;
            case Terminal::C:
                return m_pinC;
            case Terminal::D:
                return m_pinD;
            case Terminal::VSS:
                return m_pinVSS;
            case Terminal::E:
                return m_pinE;
            case Terminal::F:
                return m_pinF;
            case Terminal::L:
                return m_pinL;
            case Terminal::M:
                return m_pinM;
            case Terminal::G:
                return m_pinG;
            case Terminal::H:
                return m_pinH;
            case Terminal::VDD:
                return m_pinVDD;
        }
        internal::runtime::error::log(
            "CD4011B", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "CD4011B: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> CD4011B::pins() const
    {
        return m_pinPtrs;
    }

    void CD4011B::onVDD(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vddMin && v.value() <= k_vddMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "CD4011B",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VDD not powered: outputs held at HighZ");
            m_pinJ.release();
            m_pinK.release();
            m_pinL.release();
            m_pinM.release();
        }
    }
} // namespace nfx::silicon::gate::cd4xxx
