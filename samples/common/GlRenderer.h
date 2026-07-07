#pragma once

/**
 * \file GlRenderer.h
 * \brief Declares the GL scissor-based pixel renderer.
 */

namespace nfx::silicon::samples
{
    /**
     * \class GlRenderer
     * \brief Renders a flat bool pixel grid using glScissor/glClear.
     *
     * Each lit pixel is drawn as a filled rectangle scaled to fit the viewport.
     * No shader or texture required -- GL 1.x compatible.
     */
    class GlRenderer
    {
    public:
        /**
         * \brief Renders a pixel grid into the current GL viewport.
         * \param pixels Row-major pixel array, true = lit.
         * \param pixW Grid width in pixels.
         * \param pixH Grid height in pixels.
         * \param vpW Viewport width in screen pixels.
         * \param vpH Viewport height in screen pixels.
         */
        void render(const bool* pixels, int pixW, int pixH, int vpW, int vpH);
    };
} // namespace nfx::silicon::samples
