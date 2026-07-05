#include "nfx/silicon/latch/cd4xxx/CD4508B.h"

#include "internal/runtime/Error.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::latch::cd4xxx
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

    CD4508B::CD4508B(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_pinRESA{ makeInput("RESA") },
          m_pinSTRBA{ makeInput("STRBA") },
          m_pinDISA{ makeInput("DISA") },
          m_pinD0A{ makeInput("D0A") },
          m_pinQ0A{ makeOutput("Q0A") },
          m_pinD1A{ makeInput("D1A") },
          m_pinQ1A{ makeOutput("Q1A") },
          m_pinD2A{ makeInput("D2A") },
          m_pinQ2A{ makeOutput("Q2A") },
          m_pinD3A{ makeInput("D3A") },
          m_pinQ3A{ makeOutput("Q3A") },
          m_pinVSS{ makePower("VSS") },
          m_pinRESB{ makeInput("RESB") },
          m_pinSTRBB{ makeInput("STRBB") },
          m_pinDISB{ makeInput("DISB") },
          m_pinD0B{ makeInput("D0B") },
          m_pinQ0B{ makeOutput("Q0B") },
          m_pinD1B{ makeInput("D1B") },
          m_pinQ1B{ makeOutput("Q1B") },
          m_pinD2B{ makeInput("D2B") },
          m_pinQ2B{ makeOutput("Q2B") },
          m_pinD3B{ makeInput("D3B") },
          m_pinQ3B{ makeOutput("Q3B") },
          m_pinVDD{ makePower("VDD") },
          m_pinPtrs{ &m_pinRESA, &m_pinSTRBA, &m_pinDISA, &m_pinD0A, &m_pinQ0A,  &m_pinD1A,   &m_pinQ1A,  &m_pinD2A,
                     &m_pinQ2A,  &m_pinD3A,   &m_pinQ3A,  &m_pinVSS, &m_pinRESB, &m_pinSTRBB, &m_pinDISB, &m_pinD0B,
                     &m_pinQ0B,  &m_pinD1B,   &m_pinQ1B,  &m_pinD2B, &m_pinQ2B,  &m_pinD3B,   &m_pinQ3B,  &m_pinVDD },
          m_qAPtrs{ &m_pinQ0A, &m_pinQ1A, &m_pinQ2A, &m_pinQ3A },
          m_dAPtrs{ &m_pinD0A, &m_pinD1A, &m_pinD2A, &m_pinD3A },
          m_qBPtrs{ &m_pinQ0B, &m_pinQ1B, &m_pinQ2B, &m_pinQ3B },
          m_dBPtrs{ &m_pinD0B, &m_pinD1B, &m_pinD2B, &m_pinD3B }
    {
        auto onChangeA = [this](const Level) { compute(false); };
        auto onChangeB = [this](const Level) { compute(true); };

        m_pinRESA.connect<Level>(onChangeA);
        m_pinSTRBA.connect<Level>(onChangeA);
        m_pinDISA.connect<Level>(onChangeA);
        m_pinD0A.connect<Level>(onChangeA);
        m_pinD1A.connect<Level>(onChangeA);
        m_pinD2A.connect<Level>(onChangeA);
        m_pinD3A.connect<Level>(onChangeA);

        m_pinRESB.connect<Level>(onChangeB);
        m_pinSTRBB.connect<Level>(onChangeB);
        m_pinDISB.connect<Level>(onChangeB);
        m_pinD0B.connect<Level>(onChangeB);
        m_pinD1B.connect<Level>(onChangeB);
        m_pinD2B.connect<Level>(onChangeB);
        m_pinD3B.connect<Level>(onChangeB);

        m_pinVDD.connect<float>([this](const Voltage v) { onVDD(v); });
    }

    const char* CD4508B::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& CD4508B::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "CD4508B", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "CD4508B: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& CD4508B::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::RESA:
                return m_pinRESA;
            case Terminal::STRBA:
                return m_pinSTRBA;
            case Terminal::DISA:
                return m_pinDISA;
            case Terminal::D0A:
                return m_pinD0A;
            case Terminal::Q0A:
                return m_pinQ0A;
            case Terminal::D1A:
                return m_pinD1A;
            case Terminal::Q1A:
                return m_pinQ1A;
            case Terminal::D2A:
                return m_pinD2A;
            case Terminal::Q2A:
                return m_pinQ2A;
            case Terminal::D3A:
                return m_pinD3A;
            case Terminal::Q3A:
                return m_pinQ3A;
            case Terminal::VSS:
                return m_pinVSS;
            case Terminal::RESB:
                return m_pinRESB;
            case Terminal::STRBB:
                return m_pinSTRBB;
            case Terminal::DISB:
                return m_pinDISB;
            case Terminal::D0B:
                return m_pinD0B;
            case Terminal::Q0B:
                return m_pinQ0B;
            case Terminal::D1B:
                return m_pinD1B;
            case Terminal::Q1B:
                return m_pinQ1B;
            case Terminal::D2B:
                return m_pinD2B;
            case Terminal::Q2B:
                return m_pinQ2B;
            case Terminal::D3B:
                return m_pinD3B;
            case Terminal::Q3B:
                return m_pinQ3B;
            case Terminal::VDD:
                return m_pinVDD;
        }
        internal::runtime::error::log(
            "CD4508B", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "CD4508B: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> CD4508B::pins() const
    {
        return m_pinPtrs;
    }

    void CD4508B::reset()
    {
        for (auto* p : m_qAPtrs)
        {
            p->drive<Level>(Level::Low);
        }
        for (auto* p : m_qBPtrs)
        {
            p->drive<Level>(Level::Low);
        }
        m_latchA = 0;
        m_latchB = 0;
    }

    void CD4508B::onVDD(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vddMin && v.value() <= k_vddMax;

        if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "CD4508B",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VDD not powered: outputs held at HighZ");
            for (auto* p : m_qAPtrs)
            {
                p->release();
            }
            for (auto* p : m_qBPtrs)
            {
                p->release();
            }
        }
        else if (!wasPowered && m_powered)
        {
            compute(false);
            compute(true);
        }
    }

    void CD4508B::compute(const bool latchB)
    {
        if (!m_powered)
        {
            return;
        }

        auto& pinDIS = latchB ? m_pinDISB : m_pinDISA;
        auto& pinRES = latchB ? m_pinRESB : m_pinRESA;
        auto& pinSTRB = latchB ? m_pinSTRBB : m_pinSTRBA;
        auto& qPtrs = latchB ? m_qBPtrs : m_qAPtrs;
        auto& dPtrs = latchB ? m_dBPtrs : m_dAPtrs;
        auto& latchVal = latchB ? m_latchB : m_latchA;

        const Level dis = pinDIS.read<Level>();
        const Level res = pinRES.read<Level>();
        const Level strb = pinSTRB.read<Level>();

        if (dis == Level::High)
        {
            for (auto* p : qPtrs)
            {
                p->release();
            }
            return;
        }

        if (res == Level::High)
        {
            for (auto* p : qPtrs)
            {
                p->drive<Level>(Level::Low);
            }
            latchVal = 0;
            return;
        }

        if (strb == Level::High)
        {
            for (std::uint8_t i = 0; i < 4; ++i)
            {
                const Level d = dPtrs[i]->read<Level>();
                if (d == Level::HighZ)
                {
                    for (auto* p : qPtrs)
                    {
                        p->release();
                    }
                    return;
                }
                qPtrs[i]->drive<Level>(d);
                latchVal = static_cast<std::uint8_t>((latchVal & ~(1u << i)) | (d == Level::High ? (1u << i) : 0u));
            }
            return;
        }

        // STROBE=Low: latched, outputs hold -> nothing to do
    }
} // namespace nfx::silicon::latch::cd4xxx
