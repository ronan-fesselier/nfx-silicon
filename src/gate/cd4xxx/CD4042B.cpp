#include "nfx/silicon/gate/cd4xxx/CD4042B.h"

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

        // CL = CLOCK XNOR POLARITY: High when both agree, enabling transparency.
        Level computeCL(const Level clock, const Level polarity)
        {
            if (clock == Level::HighZ || polarity == Level::HighZ)
                return Level::HighZ;
            return (clock == polarity) ? Level::High : Level::Low;
        }
    } // namespace

    CD4042B::CD4042B(const Descriptor& descriptor)
        : m_descriptor{ descriptor },
          m_latch1{ DLatch::Descriptor{ .name = "L1" } },
          m_latch2{ DLatch::Descriptor{ .name = "L2" } },
          m_latch3{ DLatch::Descriptor{ .name = "L3" } },
          m_latch4{ DLatch::Descriptor{ .name = "L4" } },
          m_pinQ4{ makeOutput("Q4") },
          m_pinQ1{ makeOutput("Q1") },
          m_pinNQ1{ makeOutput("/Q1") },
          m_pinD1{ makeInput("D1") },
          m_pinCLOCK{ makeInput("CLOCK") },
          m_pinPOLARITY{ makeInput("POLARITY") },
          m_pinD2{ makeInput("D2") },
          m_pinVSS{ makePower("VSS") },
          m_pinNQ2{ makeOutput("/Q2") },
          m_pinQ2{ makeOutput("Q2") },
          m_pinQ3{ makeOutput("Q3") },
          m_pinNQ3{ makeOutput("/Q3") },
          m_pinD3{ makeInput("D3") },
          m_pinD4{ makeInput("D4") },
          m_pinNQ4{ makeOutput("/Q4") },
          m_pinVDD{ makePower("VDD") },
          m_pinPtrs{ &m_pinQ4,  &m_pinQ1, &m_pinNQ1, &m_pinD1,  &m_pinCLOCK, &m_pinPOLARITY, &m_pinD2,  &m_pinVSS,
                     &m_pinNQ2, &m_pinQ2, &m_pinQ3,  &m_pinNQ3, &m_pinD3,    &m_pinD4,       &m_pinNQ4, &m_pinVDD }
    {
        m_pinCLOCK.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_clock = l;
            onClockOrPolarity();
        });
        m_pinPOLARITY.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_polarity = l;
            onClockOrPolarity();
        });

        m_pinD1.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_latch1.pin(DLatch::Terminal::D).drive<Level>(l);
        });
        m_pinD2.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_latch2.pin(DLatch::Terminal::D).drive<Level>(l);
        });
        m_pinD3.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_latch3.pin(DLatch::Terminal::D).drive<Level>(l);
        });
        m_pinD4.connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_latch4.pin(DLatch::Terminal::D).drive<Level>(l);
        });

        m_latch1.pin(DLatch::Terminal::Q).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pinQ1.drive<Level>(l);
        });
        m_latch1.pin(DLatch::Terminal::NQ).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pinNQ1.drive<Level>(l);
        });
        m_latch2.pin(DLatch::Terminal::Q).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pinQ2.drive<Level>(l);
        });
        m_latch2.pin(DLatch::Terminal::NQ).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pinNQ2.drive<Level>(l);
        });
        m_latch3.pin(DLatch::Terminal::Q).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pinQ3.drive<Level>(l);
        });
        m_latch3.pin(DLatch::Terminal::NQ).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pinNQ3.drive<Level>(l);
        });
        m_latch4.pin(DLatch::Terminal::Q).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pinQ4.drive<Level>(l);
        });
        m_latch4.pin(DLatch::Terminal::NQ).connect<Level>([this](const Level l) {
            if (!m_powered)
            {
                return;
            }
            m_pinNQ4.drive<Level>(l);
        });

        m_pinVDD.connect<float>([this](const Voltage v) { onVDD(v); });
    }

    const char* CD4042B::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& CD4042B::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        internal::runtime::error::log(
            "CD4042B", internal::runtime::error::Level::Critical, Kind::Contract, "unknown pin name");
        assert(false && "CD4042B: unknown pin name");
        return *m_pinPtrs[0];
    }

    signal::Pin& CD4042B::pin(const Terminal terminal)
    {
        switch (terminal)
        {
            case Terminal::Q4:
                return m_pinQ4;
            case Terminal::Q1:
                return m_pinQ1;
            case Terminal::NQ1:
                return m_pinNQ1;
            case Terminal::D1:
                return m_pinD1;
            case Terminal::CLOCK:
                return m_pinCLOCK;
            case Terminal::POLARITY:
                return m_pinPOLARITY;
            case Terminal::D2:
                return m_pinD2;
            case Terminal::VSS:
                return m_pinVSS;
            case Terminal::NQ2:
                return m_pinNQ2;
            case Terminal::Q2:
                return m_pinQ2;
            case Terminal::Q3:
                return m_pinQ3;
            case Terminal::NQ3:
                return m_pinNQ3;
            case Terminal::D3:
                return m_pinD3;
            case Terminal::D4:
                return m_pinD4;
            case Terminal::NQ4:
                return m_pinNQ4;
            case Terminal::VDD:
                return m_pinVDD;
        }
        internal::runtime::error::log(
            "CD4042B", internal::runtime::error::Level::Critical, Kind::Contract, "invalid terminal");
        assert(false && "CD4042B: invalid terminal");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> CD4042B::pins() const
    {
        return m_pinPtrs;
    }

    void CD4042B::reset()
    {
        m_latch1.reset();
        m_latch2.reset();
        m_latch3.reset();
        m_latch4.reset();
    }

    void CD4042B::onClockOrPolarity()
    {
        const Level cl = computeCL(m_clock, m_polarity);
        m_latch1.pin(DLatch::Terminal::EN).drive<Level>(cl);
        m_latch2.pin(DLatch::Terminal::EN).drive<Level>(cl);
        m_latch3.pin(DLatch::Terminal::EN).drive<Level>(cl);
        m_latch4.pin(DLatch::Terminal::EN).drive<Level>(cl);
    }

    void CD4042B::onVDD(const Voltage v)
    {
        const bool wasPowered = m_powered;
        m_powered = v.has_value() && v.value() >= k_vddMin && v.value() <= k_vddMax;

        if (!wasPowered && m_powered)
        {
            m_latch1.reset();
            m_latch2.reset();
            m_latch3.reset();
            m_latch4.reset();
        }
        else if (wasPowered && !m_powered)
        {
            internal::runtime::error::log(
                "CD4042B",
                internal::runtime::error::Level::Warning,
                Kind::Configuration,
                "VDD not powered: outputs held at HighZ");
            m_pinQ1.release();
            m_pinNQ1.release();
            m_pinQ2.release();
            m_pinNQ2.release();
            m_pinQ3.release();
            m_pinNQ3.release();
            m_pinQ4.release();
            m_pinNQ4.release();
        }
    }
} // namespace nfx::silicon::gate::cd4xxx
