#if defined(__linux__)
    #include "../App.h"

    #include <X11/XKBlib.h>
    #include <X11/keysym.h>
    #include <GL/glx.h>

    #include <cstdint>
    #include <cstdio>
    #include <unordered_set>

namespace nfx::silicon::samples
{
    namespace
    {
        using PFNGLXCREATECONTEXTATTRIBSARBPROC = GLXContext (*)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
        using silicon::host::input::KeyCode;
        using silicon::host::input::KeyEvent;

        [[nodiscard]] KeyCode normalizeKeySym(const KeySym key)
        {
            if (key >= XK_A && key <= XK_Z)
            {
                return static_cast<KeyCode>(
                    static_cast<std::uint16_t>(KeyCode::A) + static_cast<std::uint16_t>(key - XK_A));
            }
            if (key >= XK_a && key <= XK_z)
            {
                return static_cast<KeyCode>(
                    static_cast<std::uint16_t>(KeyCode::A) + static_cast<std::uint16_t>(key - XK_a));
            }
            if (key >= XK_0 && key <= XK_9)
            {
                return static_cast<KeyCode>(
                    static_cast<std::uint16_t>(KeyCode::Num0) + static_cast<std::uint16_t>(key - XK_0));
            }
            if (key >= XK_KP_0 && key <= XK_KP_9)
            {
                return static_cast<KeyCode>(
                    static_cast<std::uint16_t>(KeyCode::Num0) + static_cast<std::uint16_t>(key - XK_KP_0));
            }

            switch (key)
            {
                case XK_ampersand:
                case XK_exclam:
                    return KeyCode::Num1;
                case XK_eacute:
                case XK_at:
                    return KeyCode::Num2;
                case XK_quotedbl:
                case XK_numbersign:
                    return KeyCode::Num3;
                case XK_apostrophe:
                case XK_dollar:
                    return KeyCode::Num4;
                case XK_parenleft:
                case XK_percent:
                    return KeyCode::Num5;
                case XK_minus:
                case XK_asciicircum:
                    return KeyCode::Num6;
                case XK_egrave:
                    return KeyCode::Num7;
                case XK_underscore:
                    return KeyCode::Num8;
                case XK_ccedilla:
                    return KeyCode::Num9;
                case XK_agrave:
                    return KeyCode::Num0;
                default:
                    break;
            }
            if (key == XK_space)
            {
                return KeyCode::Space;
            }
            if (key == XK_Escape)
            {
                return KeyCode::Escape;
            }
            if (key == XK_Return)
            {
                return KeyCode::Enter;
            }
            if (key == XK_KP_Enter)
            {
                return KeyCode::Enter;
            }
            if (key == XK_BackSpace)
            {
                return KeyCode::Backspace;
            }
            if (key == XK_Tab)
            {
                return KeyCode::Tab;
            }
            if (key == XK_Left)
            {
                return KeyCode::Left;
            }
            if (key == XK_Right)
            {
                return KeyCode::Right;
            }
            if (key == XK_Up)
            {
                return KeyCode::Up;
            }
            if (key == XK_Down)
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

        Display* display = XOpenDisplay(nullptr);
        if (!display)
        {
            std::fprintf(stderr, "Cannot open X display\n");
            return 1;
        }

        GLXContext ctx = nullptr;
        Colormap colormap = 0;
        Window win = 0;

        auto cleanup = [&] {
            if (ctx)
            {
                glXMakeCurrent(display, None, nullptr);
                glXDestroyContext(display, ctx);
                ctx = nullptr;
            }

            if (win)
            {
                XDestroyWindow(display, win);
                win = 0;
            }

            if (colormap)
            {
                XFreeColormap(display, colormap);
                colormap = 0;
            }

            XCloseDisplay(display);
        };

        int screen = DefaultScreen(display);

        // clang-format off
        static const int fbAttribs[] = {
            GLX_X_RENDERABLE,  true,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_RENDER_TYPE,   GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
            GLX_RED_SIZE,      8,
            GLX_GREEN_SIZE,    8,
            GLX_BLUE_SIZE,     8,
            GLX_ALPHA_SIZE,    8,
            GLX_DEPTH_SIZE,    24,
            GLX_STENCIL_SIZE,  8,
            GLX_DOUBLEBUFFER,  true,
            None
        };
        // clang-format on

        int fbCount = 0;
        GLXFBConfig* fbc = glXChooseFBConfig(display, screen, fbAttribs, &fbCount);
        if (!fbc || fbCount == 0)
        {
            std::fprintf(stderr, "No matching GLX framebuffer config found\n");
            if (fbc)
            {
                XFree(fbc);
            }
            cleanup();
            return 1;
        }
        GLXFBConfig bestFbc = fbc[0];
        XFree(fbc);

        XVisualInfo* vi = glXGetVisualFromFBConfig(display, bestFbc);
        if (!vi)
        {
            std::fprintf(stderr, "glXGetVisualFromFBConfig failed\n");
            cleanup();
            return 1;
        }

        Window root = DefaultRootWindow(display);
        XSetWindowAttributes swa{};
        colormap = XCreateColormap(display, root, vi->visual, AllocNone);
        if (!colormap)
        {
            std::fprintf(stderr, "XCreateColormap failed\n");
            XFree(vi);
            cleanup();
            return 1;
        }

        swa.colormap = colormap;
        swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask | PointerMotionMask |
                         ButtonPressMask | ButtonReleaseMask | FocusChangeMask;

        win = XCreateWindow(
            display,
            root,
            0,
            0,
            static_cast<unsigned>(config.width),
            static_cast<unsigned>(config.height),
            0,
            vi->depth,
            InputOutput,
            vi->visual,
            CWColormap | CWEventMask,
            &swa);
        XFree(vi);

        if (!win)
        {
            std::fprintf(stderr, "XCreateWindow failed\n");
            cleanup();
            return 1;
        }

        XStoreName(display, win, title);
        XMapWindow(display, win);
        XFlush(display);

        Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", False);
        if (wmDelete == None)
        {
            std::fprintf(stderr, "XInternAtom(WM_DELETE_WINDOW) failed\n");
            cleanup();
            return 1;
        }

        XSetWMProtocols(display, win, &wmDelete, 1);

        auto glXCreateContextAttribsARB = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(
            glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB")));

        if (!glXCreateContextAttribsARB)
        {
            std::fprintf(stderr, "glXCreateContextAttribsARB not available\n");
            cleanup();
            return 1;
        }

        // clang-format off
        static const int ctxAttribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, config.glMajor,
            GLX_CONTEXT_MINOR_VERSION_ARB, config.glMinor,
            GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
            None
        };
        // clang-format on

        ctx = glXCreateContextAttribsARB(display, bestFbc, nullptr, True, ctxAttribs);
        if (!ctx)
        {
            std::fprintf(stderr, "Failed to create OpenGL %d.%d core context\n", config.glMajor, config.glMinor);
            cleanup();
            return 1;
        }

        if (!glXMakeCurrent(display, win, ctx))
        {
            std::fprintf(stderr, "glXMakeCurrent failed\n");
            cleanup();
            return 1;
        }

        // Enable VSync
        auto glXSwapIntervalEXT_ = reinterpret_cast<void (*)(Display*, GLXDrawable, int)>(
            glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glXSwapIntervalEXT")));
        if (glXSwapIntervalEXT_)
        {
            glXSwapIntervalEXT_(display, win, 1);
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

        int winWidth = config.width, winHeight = config.height;
        std::unordered_set<KeyCode> activeKeys{};
        bool running = true;
        while (running)
        {
            while (XPending(display))
            {
                XEvent ev;
                XNextEvent(display, &ev);

                if (ev.type == ClientMessage && static_cast<Atom>(ev.xclient.data.l[0]) == wmDelete)
                {
                    running = false;
                }
                if (ev.type == KeyPress)
                {
                    const KeySym key = XkbKeycodeToKeysym(display, ev.xkey.keycode, 0, 0);
                    const auto normalized = normalizeKeySym(key);
                    if (onKeyEvent && normalized != KeyCode::Unknown)
                    {
                        activeKeys.insert(normalized);
                        onKeyEvent(KeyEvent{ .key = normalized, .pressed = true, .repeat = false });
                    }
                    if (normalized == KeyCode::Escape)
                    {
                        running = false;
                    }
                }
                if (ev.type == KeyRelease && onKeyEvent)
                {
                    if (XPending(display))
                    {
                        XEvent nextEv;
                        XPeekEvent(display, &nextEv);
                        if (nextEv.type == KeyPress && nextEv.xkey.time == ev.xkey.time &&
                            nextEv.xkey.keycode == ev.xkey.keycode)
                        {
                            continue;
                        }
                    }

                    const KeySym key = XkbKeycodeToKeysym(display, ev.xkey.keycode, 0, 0);
                    const auto normalized = normalizeKeySym(key);
                    if (normalized != KeyCode::Unknown)
                    {
                        activeKeys.erase(normalized);
                        onKeyEvent(KeyEvent{ .key = normalized, .pressed = false, .repeat = false });
                    }
                }
                if (ev.type == FocusOut && onKeyEvent)
                {
                    releaseActiveKeys(onKeyEvent, activeKeys);
                }
                if (ev.type == ConfigureNotify)
                {
                    winWidth = ev.xconfigure.width;
                    winHeight = ev.xconfigure.height;
                }
            }

            onRender(winWidth, winHeight);

            glXSwapBuffers(display, win);
        }

        if (onShutdown)
        {
            onShutdown();
        }

        cleanup();

        return 0;
    }
} // namespace nfx::silicon::samples
#endif
