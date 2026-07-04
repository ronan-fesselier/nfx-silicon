#include "nfx/silicon/signal/Wire.h"

#include "internal/runtime/Error.h"

#include <cassert>

namespace nfx::silicon::signal
{
    Wire::Wire(const Descriptor& descriptor)
        : m_descriptor{ descriptor }
    {
        if (m_descriptor.kind == Pin::Kind::Analog && m_descriptor.resolution != Resolution::PushPull)
        {
            internal::runtime::error::log(
                "Wire",
                internal::runtime::error::Level::Critical,
                internal::runtime::error::Kind::Contract,
                "analog wire must not specify a digital resolution model");
            assert(false && "Wire: analog wire must not specify a digital resolution model");
        }
    }

    void Wire::attach(Pin& pin)
    {
        if (pin.descriptor().kind != m_descriptor.kind)
        {
            internal::runtime::error::log(
                "Wire",
                internal::runtime::error::Level::Critical,
                internal::runtime::error::Kind::Contract,
                "pin kind does not match wire kind");
            assert(false && "Wire: pin kind does not match wire kind");
        }
        const auto direction = pin.descriptor().direction;

        if (direction == Pin::Direction::Output || direction == Pin::Direction::Bidirectional ||
            direction == Pin::Direction::OpenDrain || direction == Pin::Direction::OpenCollector)
        {
            m_connections.drivers.push_back(&pin);
            if (pin.descriptor().kind == Pin::Kind::Digital)
            {
                if (direction == Pin::Direction::Bidirectional)
                {
                    pin.connect<Level>([this, &pin](Level) {
                        m_bidirTrigger = &pin;
                        resolve();
                        m_bidirTrigger = nullptr;
                    });
                }
                else
                {
                    pin.connect<Level>([this](Level) { resolve(); });
                }
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

        auto activeLevel = [this](Pin* pin) -> Level {
            if (pin->descriptor().direction == Pin::Direction::Bidirectional && pin != m_bidirTrigger)
            {
                return Level::HighZ;
            }
            return pin->read<Level>();
        };

        switch (m_descriptor.resolution)
        {
            case Resolution::PushPull:
            {
                int activeDrivers = 0;
                Level firstActive = Level::HighZ;
                for (auto* pin : m_connections.drivers)
                {
                    Level l = activeLevel(pin);
                    if (l != Level::HighZ)
                    {
                        if (activeDrivers > 0)
                        {
                            if (l != firstActive)
                            {
                                internal::runtime::error::log(
                                    "Wire",
                                    internal::runtime::error::Level::Critical,
                                    internal::runtime::error::Kind::Signal,
                                    "PushPull conflict: multiple drivers with different levels");
                                assert(false && "Wire: PushPull conflict: multiple drivers with different levels");
                            }
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
                    Level l = activeLevel(pin);
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
                    Level l = activeLevel(pin);
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
