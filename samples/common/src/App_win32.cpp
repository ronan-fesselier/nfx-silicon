#if defined(_WIN32)
    #include "../App.h"

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif

    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    #include <windows.h>
    #include <GL/gl.h>
    #include <GL/wglext.h>

    #include <bit>
    #include <cstdint>
    #include <cstdio>
    #include <type_traits>
    #include <unordered_set>

namespace nfx::silicon::samples
{
    namespace
    {
        using nfx::silicon::host::input::KeyCode;
        using nfx::silicon::host::input::KeyEvent;

        struct WindowState
        {
            bool running = true;
            int width = 0;
            int height = 0;
            std::function<void(const KeyEvent&)> onKeyEvent;
            std::unordered_set<KeyCode> activeKeys{};
        };

        template <typename T>
        T loadWglProc(const char* name)
        {
            static_assert(std::is_pointer_v<T>, "T must be a pointer type");
            static_assert(sizeof(T) == sizeof(std::uintptr_t), "Unexpected pointer size mismatch");

            const PROC raw = wglGetProcAddress(name);
            if (!raw)
            {
                return nullptr;
            }

            const std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(raw);
            return std::bit_cast<T>(addr);
        }

        [[nodiscard]] KeyCode normalizeVirtualKey(const WPARAM wParam)
        {
            if (wParam >= 'A' && wParam <= 'Z')
            {
                return static_cast<KeyCode>(
                    static_cast<std::uint16_t>(KeyCode::A) + static_cast<std::uint16_t>(wParam - 'A'));
            }
            if (wParam >= '0' && wParam <= '9')
            {
                return static_cast<KeyCode>(
                    static_cast<std::uint16_t>(KeyCode::Num0) + static_cast<std::uint16_t>(wParam - '0'));
            }
            if (wParam == VK_SPACE)
            {
                return KeyCode::Space;
            }
            if (wParam == VK_ESCAPE)
            {
                return KeyCode::Escape;
            }
            if (wParam == VK_RETURN)
            {
                return KeyCode::Enter;
            }
            if (wParam == VK_BACK)
            {
                return KeyCode::Backspace;
            }
            if (wParam == VK_TAB)
            {
                return KeyCode::Tab;
            }
            if (wParam == VK_LEFT)
            {
                return KeyCode::Left;
            }
            if (wParam == VK_RIGHT)
            {
                return KeyCode::Right;
            }
            if (wParam == VK_UP)
            {
                return KeyCode::Up;
            }
            if (wParam == VK_DOWN)
            {
                return KeyCode::Down;
            }
            return KeyCode::Unknown;
        }

        void releaseActiveKeys(
            const std::function<void(const KeyEvent&)>& onKeyEvent, std::unordered_set<KeyCode>& activeKeys)
        {
            if (!onKeyEvent)
            {
                return;
            }

            for (const auto key : activeKeys)
            {
                onKeyEvent(KeyEvent{ .key = key, .pressed = false, .repeat = false });
            }
            activeKeys.clear();
        }

        [[nodiscard]] WindowState* getWindowState(HWND hwnd)
        {
            return reinterpret_cast<WindowState*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));
        }

        LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            if (msg == WM_NCCREATE)
            {
                const auto* createStruct = reinterpret_cast<const CREATESTRUCTA*>(lParam);
                auto* state = static_cast<WindowState*>(createStruct->lpCreateParams);
                if (!state)
                {
                    return FALSE;
                }

                SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
                return TRUE;
            }

            auto* state = getWindowState(hwnd);

            switch (msg)
            {
                case WM_CLOSE:
                case WM_DESTROY:
                    if (state)
                    {
                        state->running = false;
                    }
                    PostQuitMessage(0);
                    return 0;
                case WM_KEYDOWN:
                {
                    if (!state)
                    {
                        return DefWindowProcA(hwnd, msg, wParam, lParam);
                    }

                    const auto normalized = normalizeVirtualKey(wParam);
                    if (state->onKeyEvent && normalized != KeyCode::Unknown)
                    {
                        state->activeKeys.insert(normalized);
                        state->onKeyEvent(KeyEvent{ .key = normalized, .pressed = true, .repeat = false });
                    }
                    if (normalized == KeyCode::Escape)
                    {
                        state->running = false;
                        PostQuitMessage(0);
                    }
                    return 0;
                }
                case WM_KEYUP:
                    if (!state)
                    {
                        return DefWindowProcA(hwnd, msg, wParam, lParam);
                    }

                    if (state->onKeyEvent)
                    {
                        const auto normalized = normalizeVirtualKey(wParam);
                        if (normalized != KeyCode::Unknown)
                        {
                            state->activeKeys.erase(normalized);
                            state->onKeyEvent(KeyEvent{ .key = normalized, .pressed = false, .repeat = false });
                        }
                    }
                    return 0;
                case WM_KILLFOCUS:
                    if (state)
                    {
                        releaseActiveKeys(state->onKeyEvent, state->activeKeys);
                    }
                    return 0;
                case WM_SIZE:
                    if (state)
                    {
                        state->width = LOWORD(lParam);
                        state->height = HIWORD(lParam);
                    }
                    return 0;
                default:
                    return DefWindowProcA(hwnd, msg, wParam, lParam);
            }
        }
    } // namespace

    App::App(Config config)
        : m_config{ std::move(config) }
    {}

    int App::run(
        std::function<void()> onInit,
        std::function<void()> onShutdown,
        std::function<void(int width, int height)> onRender,
        std::function<void(const KeyEvent&)> onKeyEvent)
    {
        if (!onRender)
        {
            std::fprintf(stderr, "onRender callback is required\n");
            return 1;
        }

        const Config& config = m_config;
        const char* title = config.title ? config.title : "nfx-graphics";
        WindowState windowState{};
        windowState.running = true;
        windowState.width = config.width;
        windowState.height = config.height;
        windowState.onKeyEvent = std::move(onKeyEvent);

        HINSTANCE hInstance = GetModuleHandle(nullptr);
        if (!hInstance)
        {
            std::fprintf(stderr, "GetModuleHandle failed\n");
            return 1;
        }

        constexpr const char* kTmpClassName = "nfx_tmp";
        constexpr const char* kAppClassName = "nfx_app";

        ATOM tmpClassAtom = 0;
        HWND hwndTmp = nullptr;
        HDC hdcTmp = nullptr;
        HGLRC hrcTmp = nullptr;

        auto cleanupTmp = [&] {
            if (hrcTmp)
            {
                wglMakeCurrent(nullptr, nullptr);
                wglDeleteContext(hrcTmp);
                hrcTmp = nullptr;
            }

            if (hdcTmp && hwndTmp)
            {
                ReleaseDC(hwndTmp, hdcTmp);
                hdcTmp = nullptr;
            }

            if (hwndTmp)
            {
                DestroyWindow(hwndTmp);
                hwndTmp = nullptr;
            }

            if (tmpClassAtom)
            {
                UnregisterClassA(kTmpClassName, hInstance);
                tmpClassAtom = 0;
            }
        };

        // Temporary window to load WGL extensions
        WNDCLASSEXA wcTmp{};
        wcTmp.cbSize = sizeof(wcTmp);
        wcTmp.style = CS_OWNDC;
        wcTmp.lpfnWndProc = DefWindowProcA;
        wcTmp.hInstance = hInstance;
        wcTmp.lpszClassName = kTmpClassName;
        tmpClassAtom = RegisterClassExA(&wcTmp);
        if (!tmpClassAtom)
        {
            std::fprintf(stderr, "RegisterClassExA(nfx_tmp) failed\n");
            return 1;
        }

        hwndTmp = CreateWindowExA(
            0, kTmpClassName, "", WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, nullptr, nullptr, hInstance, nullptr);
        if (!hwndTmp)
        {
            std::fprintf(stderr, "CreateWindowExA(nfx_tmp) failed\n");
            cleanupTmp();
            return 1;
        }

        hdcTmp = GetDC(hwndTmp);
        if (!hdcTmp)
        {
            std::fprintf(stderr, "GetDC(nfx_tmp) failed\n");
            cleanupTmp();
            return 1;
        }

        PIXELFORMATDESCRIPTOR pfdTmp{};
        pfdTmp.nSize = sizeof(pfdTmp);
        pfdTmp.nVersion = 1;
        pfdTmp.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfdTmp.iPixelType = PFD_TYPE_RGBA;
        pfdTmp.cColorBits = 32;
        pfdTmp.cDepthBits = 24;
        int pixelFormatTmp = ChoosePixelFormat(hdcTmp, &pfdTmp);
        if (pixelFormatTmp <= 0 || !SetPixelFormat(hdcTmp, pixelFormatTmp, &pfdTmp))
        {
            std::fprintf(stderr, "SetPixelFormat(nfx_tmp) failed\n");
            cleanupTmp();
            return 1;
        }

        hrcTmp = wglCreateContext(hdcTmp);
        if (!hrcTmp || !wglMakeCurrent(hdcTmp, hrcTmp))
        {
            std::fprintf(stderr, "Temporary WGL context creation failed\n");
            cleanupTmp();
            return 1;
        }

        auto wglCreateContextAttribsARB = loadWglProc<PFNWGLCREATECONTEXTATTRIBSARBPROC>("wglCreateContextAttribsARB");
        auto wglChoosePixelFormatARB = loadWglProc<PFNWGLCHOOSEPIXELFORMATARBPROC>("wglChoosePixelFormatARB");

        cleanupTmp();

        if (!wglCreateContextAttribsARB || !wglChoosePixelFormatARB)
        {
            MessageBoxA(nullptr, "WGL extensions not available", "Error", MB_ICONERROR);
            return 1;
        }

        ATOM appClassAtom = 0;
        HWND hwnd = nullptr;
        HDC hdc = nullptr;
        HGLRC hrc = nullptr;

        auto cleanupApp = [&] {
            if (hrc)
            {
                wglMakeCurrent(nullptr, nullptr);
                wglDeleteContext(hrc);
                hrc = nullptr;
            }

            if (hdc && hwnd)
            {
                ReleaseDC(hwnd, hdc);
                hdc = nullptr;
            }

            if (hwnd)
            {
                DestroyWindow(hwnd);
                hwnd = nullptr;
            }

            if (appClassAtom)
            {
                UnregisterClassA(kAppClassName, hInstance);
                appClassAtom = 0;
            }
        };

        // Real window
        WNDCLASSEXA wc{};
        wc.cbSize = sizeof(wc);
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = wndProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.lpszClassName = kAppClassName;
        appClassAtom = RegisterClassExA(&wc);
        if (!appClassAtom)
        {
            std::fprintf(stderr, "RegisterClassExA(nfx_app) failed\n");
            return 1;
        }

        RECT windowRect{ 0, 0, config.width, config.height };
        if (!AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0))
        {
            std::fprintf(stderr, "AdjustWindowRectEx failed\n");
            cleanupApp();
            return 1;
        }

        hwnd = CreateWindowExA(
            0,
            kAppClassName,
            title,
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr,
            nullptr,
            hInstance,
            &windowState);
        if (!hwnd)
        {
            std::fprintf(stderr, "CreateWindowExA(nfx_app) failed\n");
            cleanupApp();
            return 1;
        }

        hdc = GetDC(hwnd);
        if (!hdc)
        {
            std::fprintf(stderr, "GetDC(nfx_app) failed\n");
            cleanupApp();
            return 1;
        }

        // clang-format off
        const int pfAttribs[] = {
            WGL_DRAW_TO_WINDOW_ARB, true,
            WGL_SUPPORT_OPENGL_ARB, true,
            WGL_DOUBLE_BUFFER_ARB,  true,
            WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB,     32,
            WGL_DEPTH_BITS_ARB,     24,
            WGL_STENCIL_BITS_ARB,   8,
            0
        };
        // clang-format on

        int pixelFormat = 0;
        UINT numFormats = 0;
        if (!wglChoosePixelFormatARB(hdc, pfAttribs, nullptr, 1, &pixelFormat, &numFormats) || numFormats == 0)
        {
            std::fprintf(stderr, "wglChoosePixelFormatARB failed\n");
            cleanupApp();
            return 1;
        }

        PIXELFORMATDESCRIPTOR pfd{};
        if (DescribePixelFormat(hdc, pixelFormat, sizeof(pfd), &pfd) == 0 || !SetPixelFormat(hdc, pixelFormat, &pfd))
        {
            std::fprintf(stderr, "SetPixelFormat(nfx_app) failed\n");
            cleanupApp();
            return 1;
        }

        // clang-format off
        int ctxAttribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, config.glMajor,
            WGL_CONTEXT_MINOR_VERSION_ARB, config.glMinor,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };
        // clang-format on

        hrc = wglCreateContextAttribsARB(hdc, nullptr, ctxAttribs);
        if (!hrc)
        {
            MessageBoxA(nullptr, "Failed to create OpenGL core context", "Error", MB_ICONERROR);
            cleanupApp();
            return 1;
        }

        if (!wglMakeCurrent(hdc, hrc))
        {
            std::fprintf(stderr, "wglMakeCurrent failed\n");
            cleanupApp();
            return 1;
        }

        // Enable VSync
        auto wglSwapIntervalEXT_ = loadWglProc<PFNWGLSWAPINTERVALEXTPROC>("wglSwapIntervalEXT");
        if (wglSwapIntervalEXT_)
        {
            wglSwapIntervalEXT_(1);
        }

        const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        std::fprintf(stdout, "OpenGL vendor  : %s\n", vendor ? vendor : "<unavailable>");
        std::fprintf(stdout, "OpenGL renderer: %s\n", renderer ? renderer : "<unavailable>");
        std::fprintf(stdout, "OpenGL version : %s\n", version ? version : "<unavailable>");

        if (onInit)
        {
            onInit();
        }

        while (windowState.running)
        {
            MSG msg{};
            while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    windowState.running = false;
                }

                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }

            onRender(windowState.width, windowState.height);

            if (!SwapBuffers(hdc))
            {
                std::fprintf(stderr, "SwapBuffers failed\n");
                break;
            }
        }

        if (onShutdown)
        {
            onShutdown();
        }

        cleanupApp();

        return 0;
    }
} // namespace nfx::silicon::samples
#endif
