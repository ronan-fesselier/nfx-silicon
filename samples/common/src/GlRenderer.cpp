#include "../GlRenderer.h"

#ifdef __linux__
    #include <GL/gl.h>
#elif defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #define NOMINMAX
    #include <windows.h>
    #include <GL/gl.h>
#endif

#include <algorithm>

namespace nfx::silicon::samples
{
    void GlRenderer::render(const bool* pixels, const int pixW, const int pixH, const int vpW, const int vpH)
    {
        const int cellW = std::max(1, vpW / pixW);
        const int cellH = std::max(1, vpH / pixH);
        const int displayW = cellW * pixW;
        const int displayH = cellH * pixH;
        const int x0 = (vpW - displayW) / 2;
        const int y0 = (vpH - displayH) / 2;

        glViewport(0, 0, vpW, vpH);
        glDisable(GL_SCISSOR_TEST);
        glClearColor(0.01f, 0.08f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_SCISSOR_TEST);
        glClearColor(0.96f, 0.96f, 0.98f, 1.0f);

        for (int y = 0; y < pixH; ++y)
        {
            const int py = y0 + (pixH - 1 - y) * cellH;
            for (int x = 0; x < pixW; ++x)
            {
                if (!pixels[y * pixW + x])
                {
                    continue;
                }
                glScissor(x0 + x * cellW, py, cellW, cellH);
                glClear(GL_COLOR_BUFFER_BIT);
            }
        }

        glDisable(GL_SCISSOR_TEST);
    }
} // namespace nfx::silicon::samples
