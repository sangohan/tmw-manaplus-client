/*
 *  The ManaPlus Client
 *  Copyright (C) 2004-2009  The Mana World Development Team
 *  Copyright (C) 2009-2010  The Mana Developers
 *  Copyright (C) 2011-2013  The ManaPlus Developers
 *
 *  This file is part of The ManaPlus Client.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RESOURCES_SURFACEIMAGEHELPER_H
#define RESOURCES_SURFACEIMAGEHELPER_H

#ifndef USE_SDL2
#include "resources/sdlimagehelper.h"

#else

#include "localconsts.h"

#include "resources/imagehelper.h"

#include <SDL.h>

class Dye;
class Image;

/**
 * Defines a class for loading and storing images.
 */
class SurfaceImageHelper final : public ImageHelper
{
    friend class Image;

    public:
        SurfaceImageHelper()
        { }

        A_DELETE_COPY(SurfaceImageHelper)

        ~SurfaceImageHelper()
        { }

        /**
         * Loads an image from an SDL_RWops structure and recolors it.
         *
         * @param rw         The SDL_RWops to load the image from.
         * @param dye        The dye used to recolor the image.
         *
         * @return <code>NULL</code> if an error occurred, a valid pointer
         *         otherwise.
         */
        Image *load(SDL_RWops *const rw,
                    Dye const &dye) const override final A_WARN_UNUSED;

        /**
         * Loads an image from an SDL surface.
         */
        Image *load(SDL_Surface *const tmpImage) const
                    override final A_WARN_UNUSED;

        Image *createTextSurface(SDL_Surface *const tmpImage,
                                 const int width, const int height,
                                 const float alpha)
                                 const override final A_WARN_UNUSED;

        static void SDLSetEnableAlphaCache(const bool n)
        { mEnableAlphaCache = n; }

        static bool SDLGetEnableAlphaCache() A_WARN_UNUSED
        { return mEnableAlphaCache; }

         /**
         * Tells if the image was loaded using OpenGL or SDL
         * @return true if OpenGL, false if SDL.
         */
        RenderType useOpenGL() const override final A_WARN_UNUSED;

        static SDL_Surface* SDLDuplicateSurface(SDL_Surface *const tmpImage)
                                                A_WARN_UNUSED;

        SDL_Surface *create32BitSurface(int width,
                                        int height) const override final;

        static int combineSurface(SDL_Surface *const src,
                                  SDL_Rect *const srcrect,
                                  SDL_Surface *const dst,
                                  SDL_Rect *const dstrect);

    protected:
        /** SDL_Surface to SDL_Surface Image loader */
        Image *_SDLload(SDL_Surface *tmpImage) const A_WARN_UNUSED;

        static bool mEnableAlphaCache;
};

#endif  // USE_SDL2
#endif  // RESOURCES_SURFACEIMAGEHELPER_H
