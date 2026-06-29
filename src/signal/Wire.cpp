#include "nfx/silicon/signal/Wire.h"

#include <cassert>

namespace nfx::silicon::signal
{
    Wire::Wire(const Descriptor& descriptor)
        : m_descriptor{ descriptor }
    {
        assert(
            (m_descriptor.kind != Pin::Kind::Analog || m_descriptor.resolution == Resolution::PushPull) &&
            "Analog wire must not specify a digital resolution model");
    }

    void Wire::attach(Pin& pin)
    {
        assert(pin.descriptor().kind == m_descriptor.kind && "Pin kind does not match Wire kind");
        const auto direction = pin.descriptor().direction;

        if (direction == Pin::Direction::Output || direction == Pin::Direction::Bidirectional ||
            direction == Pin::Direction::OpenDrain || direction == Pin::Direction::OpenCollector)
        {
            m_connections.drivers.push_back(&pin);
            if (pin.descriptor().kind == Pin::Kind::Digital)
            {
                pin.connect<Level>([this](Level) { resolve(); });
            }
            else
            {
                pin.connect<float>([this](Voltage) { resolve(); });
            }
        }

        if (direction == Pin::Direction::Input || direction == Pin::Direction::Bidirectional)
        {
            m_connections.receivers.push_back(&pin);
        }
    }

    template <>
    typename SignalReadType<Level>::type Wire::read<Level>() const
    {
        return m_state.level;
    }

    template <>
    typename SignalReadType<float>::type Wire::read<float>() const
    {
        return m_state.voltage;
    }

    void Wire::resolve()
    {
        if (m_descriptor.kind == Pin::Kind::Analog)
        {
            Voltage resolved{ std::nullopt };
            for (auto* pin : m_connections.drivers)
            {
                auto v = pin->read<float>();
                if (v.has_value())
                {
                    resolved = v;
                }
            }
            m_state.voltage = resolved;
            for (auto* pin : m_connections.receivers)
            {
                if (resolved.has_value())
                {
                    pin->drive<float>(resolved.value());
                }
                else
                {
                    pin->release();
                }
            }
            return;
        }

        // Digital resolution
        Level resolved = Level::HighZ;

        switch (m_descriptor.resolution)
        {
            case Resolution::PushPull:
            {
                int activeDrivers = 0;
                Level firstActive = Level::HighZ;
                for (auto* pin : m_connections.drivers)
                {
                    Level l = pin->read<Level>();
                    if (l != Level::HighZ)
                    {
                        if (activeDrivers > 0)
                        {
                            assert(l == firstActive && "PushPull bus conflict: multiple drivers with different levels");
                        }
                        firstActive = l;
                        resolved = l;
                        activeDrivers++;
                    }
                }
                break;
            }
            case Resolution::WiredAnd:
            {
                bool hasDriver = false;
                resolved = Level::High;
                for (auto* pin : m_connections.drivers)
                {
                    Level l = pin->read<Level>();
                    if (l == Level::Low)
                    {
                        resolved = Level::Low;
                        hasDriver = true;
                        break;
                    }
                    if (l == Level::High)
                    {
                        hasDriver = true;
                    }
                }
                if (!hasDriver)
                {
                    resolved = Level::HighZ;
                }
                break;
            }
            case Resolution::WiredOr:
            {
                bool hasDriver = false;
                resolved = Level::Low;
                for (auto* pin : m_connections.drivers)
                {
                    Level l = pin->read<Level>();
                    if (l == Level::High)
                    {
                        resolved = Level::High;
                        hasDriver = true;
                        break;
                    }
                    if (l == Level::Low)
                    {
                        hasDriver = true;
                    }
                }
                if (!hasDriver)
                {
                    resolved = Level::HighZ;
                }
                break;
            }
        }

        m_state.level = resolved;
        for (auto* pin : m_connections.receivers)
        {
            pin->drive<Level>(resolved);
        }
    }
} // namespace nfx::silicon::signal
