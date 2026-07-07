#pragma once

/**
 * \file App.h
 * \brief Shared sample runtime entry point.
 */

#include <nfx/silicon/host/input/KeyEvent.h>

#include <functional>

namespace nfx::silicon::samples
{
    class App
    {
    public:
        /**
         * \struct Config
         * \brief Shared sample runtime configuration.
         */
        struct Config
        {
            const char* title = "nfx-silicon-app";
            int width = 640;
            int height = 320;
            int glMajor = 3;
            int glMinor = 3;
        };

        /**
         * \brief Constructs the shared sample runtime with default configuration.
         */
        App() = default;

        /**
         * \brief Constructs the shared sample runtime.
         * \param config Window and GL configuration.
         */
        explicit App(Config config);

        /**
         * \brief Runs the shared sample runtime with a windowed GL event loop.
         *
         * Initializes an OpenGL 3.3+ core context window and runs an event loop that:
         * - calls `onInit()` once before the loop,
         * - calls `onRender(width, height)` each frame to render the display,
         * - calls `onKeyEvent(event)` for each keyboard event (press and release),
         * - calls `onShutdown()` once before exiting.
         *
         * Keyboard events are normalized to `KeyCode` and `KeyEvent` types from the library,
         * with automatic layout adaptation (e.g., AZERTY symbol variants normalized to digit keys).
         * Window close terminates the loop.
         *
         * \param onInit Callback invoked once before the event loop starts. Optional.
         * \param onShutdown Callback invoked once after the event loop stops. Optional.
         * \param onRender Callback invoked each frame with current window dimensions. Required.
         *                 Responsible for all rendering (GL context is already current).
         * \param onKeyEvent Callback invoked for each keyboard event (press/release).
         *                   Events have normalized `KeyCode` and `repeat = false`. Optional.
         * \return 0 on success, non-zero on initialization or runtime failure.
         */
        int run(
            std::function<void()> onInit,
            std::function<void()> onShutdown,
            std::function<void(int width, int height)> onRender,
            std::function<void(const silicon::host::input::KeyEvent&)> onKeyEvent = {});

        /**
         * \brief Returns the runtime configuration.
         * \return The shared sample runtime configuration.
         */
        [[nodiscard]] const Config& config() const { return m_config; }

    private:
        Config m_config{};
    };
} // namespace nfx::silicon::samples
