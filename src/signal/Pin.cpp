#include "nfx/silicon/signal/Pin.h"

namespace nfx::silicon::signal
{
    Pin::Pin(const Descriptor& descriptor)
        : m_descriptor{ descriptor }
    {}

    template <>
    void Pin::drive<Level>(Level value)
    {
        if (m_state.level == value)
        {
            return;
        }
        m_state.level = value;
        for (auto& cb : m_observers.digitalCallbacks)
        {
            cb(value);
        }
    }

    template <>
    void Pin::drive<float>(float value)
    {
        m_state.voltage = value;
        for (auto& cb : m_observers.analogCallbacks)
        {
            cb(Voltage{ value });
        }
    }

    template <>
    typename SignalReadType<Level>::type Pin::read<Level>() const
    {
        return m_state.level;
    }

    template <>
    typename SignalReadType<float>::type Pin::read<float>() const
    {
        return m_state.voltage;
    }

    template <>
    void Pin::connect<Level>(std::function<void(Level)> callback)
    {
        m_observers.digitalCallbacks.push_back(std::move(callback));
    }

    template <>
    void Pin::connect<float>(std::function<void(Voltage)> callback)
    {
        m_observers.analogCallbacks.push_back(std::move(callback));
    }

    void Pin::release()
    {
        if (m_descriptor.kind == Kind::Digital)
        {
            drive<Level>(Level::HighZ);
        }
        else
        {
            m_state.voltage = std::nullopt;
            for (auto& cb : m_observers.analogCallbacks)
            {
                cb(std::nullopt);
            }
        }
    }
} // namespace nfx::silicon::signal
