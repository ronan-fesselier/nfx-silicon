#include "nfx/silicon/discrete/PowerSupply.h"

#include <cassert>
#include <cstring>

namespace nfx::silicon::discrete
{
    PowerSupply::PowerSupply(const Descriptor& descriptor)
        : m_descriptor{ descriptor }
    {
        for (const auto& rail : m_descriptor.rails)
        {
            for (const auto* existing : m_pinPtrs)
            {
                assert(std::strcmp(existing->descriptor().name, rail.name) != 0 && "PowerSupply: duplicate rail name");
            }

            auto pin = std::make_unique<signal::Pin>(signal::Pin::Descriptor{
                .name = rail.name, .kind = signal::Pin::Kind::Analog, .direction = signal::Pin::Direction::Output });

            pin->drive<float>(rail.voltage);

            m_pinPtrs.push_back(pin.get());
            m_pins.push_back(std::move(pin));
        }
    }

    const char* PowerSupply::name() const
    {
        return m_descriptor.name;
    }

    signal::Pin& PowerSupply::pin(const char* name)
    {
        for (auto* p : m_pinPtrs)
        {
            if (std::strcmp(p->descriptor().name, name) == 0)
            {
                return *p;
            }
        }
        assert(false && "PowerSupply: unknown pin name");
        return *m_pinPtrs[0];
    }

    std::span<signal::Pin* const> PowerSupply::pins() const
    {
        return m_pinPtrs;
    }
} // namespace nfx::silicon::discrete
