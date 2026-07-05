#include "nfx/silicon/gate/cd4xxx/CD4049UB.h"

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

    CD4049UB::CD4049UB(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_gates{ Not{ Not::Descriptor{ .name = "NOT1" } }, Not{ Not::Descriptor{ .name = "NOT2" } },
                   Not{ Not::Descriptor{ .name = "NOT3" } }, Not{ Not::Descriptor{ .name = "NOT4" } },
                   Not{ Not::Descriptor{ .name = "NOT5" } }, Not{ Not::Descriptor{ .name = "NOT6" } } },
          m_pinVCC{ makePower("VCC") },
          m_pinG{ makeOutput("G") },
          m_pinA{ makeInput("A") },
          m_pinH{ makeOutput("H") },
          m_pinB{ makeInput("B") },
          m_pinI{ makeOutput("I") },
          m_pinC{ makeInput("C") },
          m_pinVSS{ makePower("VSS") },
          m_pinD{ makeInput("D") },
          m_pinJ{ makeOutput("J") },
          m_pinE{ makeInput("E") },
          m_pinK{ makeOutput("K") },
          m_pinF{ makeInput("F") },
          m_pinL{ makeOutput("L") },
          m_pinPtrs{ &m_pinVCC, &m_pinG, &m_pinA, &m_pinH, &m_pinB, &m_pinI, &m_pinC,
                     &m_pinVSS, &m_pinD, &m_pinJ, &m_pinE, &m_pinK, &m_pinF, &m_pinL }
    {
        auto wireGate = [this](const std::size_t idx, signal::Pin& pinIn, signal::Pin& pinOut) {
            pinIn.connect<Level>([this, idx](const Level l) {
                if (!m_powered)
                {
                    return;
                }
                m_gates[idx].pin(Not::Terminal::A).drive<Level>(l);
            });

            m_gates[idx].pin(Not::Terminal::Y).connect<Level>([this, &pinOut](const Level l) {
                if (!m_powered)
                {
                    return;
                }
                pinOut.drive<Level>(l);
            });
        };

        wireGate(0, m_pinA, m_pinG);
        wireGate(1, m_pinB, m_pinH);
        wireGate(2, m_pinC, m_pinI);
        wireGate(3, m_pinD, m_pinJ);
        wireGate(4, m_pinE, m_pinK);
        wireGate(5, m_pinF, m_pinL);

        m_pinVCC.connect<float>([this](const Voltage v) { onVCC(v); });
    }

    const char* CD4049UB::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& CD4049UB::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "CD4049UB", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "CD4049UB: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& CD4049UB::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::VCC:
                return m_pinVCC;
            case Terminal::G:
                return m_pinG;
            case Terminal::A:
                return m_pinA;
            case Terminal::H:
                return m_pinH;
            case Terminal::B:
                return m_pinB;
            case Terminal::I:
                return m_pinI;
            case Terminal::C:
                return m_pinC;
            case Terminal::VSS:
                return m_pinVSS;
            case Terminal::D:
                return m_pinD;
            case Terminal::J:
                return m_pinJ;
            case Terminal::E:
                return m_pinE;
            case Terminal::K:
                return m_pinK;
            case Terminal::F:
                return m_pinF;
            case Terminal::L:
                return m_pinL;
        }
        internal::runtime::error::log(
            "CD4049UB", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "CD4049UB: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> CD4049UB::pins() const
    {
        return m_pinPtrs;
    }

    void CD4049UB::onVCC(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vccMin && v.value() <= k_vccMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "CD4049UB",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VCC not powered: outputs held at HighZ");
            m_pinG.release();
            m_pinH.release();
            m_pinI.release();
            m_pinJ.release();
            m_pinK.release();
            m_pinL.release();
        }
    }
} // namespace nfx::silicon::gate::cd4xxx
