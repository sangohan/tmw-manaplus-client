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

#include "sdlgraphics.h"

#include "main.h"

#include "configuration.h"
#include "graphicsmanager.h"
#include "graphicsvertexes.h"
#include "logger.h"

#include "resources/imagehelper.h"

#include <guichan/sdl/sdlpixel.hpp>

#include <SDL_gfxBlitFunc.h>

#include "debug.h"

int MSDL_gfxBlitRGBA(SDL_Surface *src, SDL_Rect *srcrect,
                     SDL_Surface *dst, SDL_Rect *dstrect)
{
    return SDL_gfxBlitRGBA(src, srcrect, dst, dstrect);
}

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
static unsigned int *cR = nullptr;
static unsigned int *cG = nullptr;
static unsigned int *cB = nullptr;
#endif

SDLGraphics::SDLGraphics() :
    Graphics(),
    mBlitMode(BLIT_NORMAL),
    mOldPixel(0),
    mOldAlpha(0)
{
}

SDLGraphics::~SDLGraphics()
{
}

bool SDLGraphics::drawRescaledImage(const Image *const image, int srcX, int srcY,
                                    int dstX, int dstY,
                                    const int width, const int height,
                                    const int desiredWidth,
                                    const int desiredHeight,
                                    const bool useColor A_UNUSED)
{
    FUNC_BLOCK("Graphics::drawRescaledImage", 1)
    // Check that preconditions for blitting are met.
    if (!mTarget || !image)
        return false;
    if (!image->mSDLSurface)
        return false;

    Image *const tmpImage = image->SDLgetScaledImage(
        desiredWidth, desiredHeight);

    if (!tmpImage)
        return false;
    if (!tmpImage->mSDLSurface)
        return false;

    dstX += mClipStack.top().xOffset;
    dstY += mClipStack.top().yOffset;

    srcX += image->mBounds.x;
    srcY += image->mBounds.y;

    SDL_Rect dstRect;
    SDL_Rect srcRect;
    dstRect.x = static_cast<int16_t>(dstX);
    dstRect.y = static_cast<int16_t>(dstY);
    srcRect.x = static_cast<int16_t>(srcX);
    srcRect.y = static_cast<int16_t>(srcY);
    srcRect.w = static_cast<uint16_t>(width);
    srcRect.h = static_cast<uint16_t>(height);

    const bool returnValue = !(SDL_BlitSurface(tmpImage->mSDLSurface,
        &srcRect, mTarget, &dstRect) < 0);

    delete tmpImage;

    return returnValue;
}

bool SDLGraphics::drawImage2(const Image *const image, int srcX, int srcY,
                          int dstX, int dstY, const int width,
                          const int height, const bool useColor A_UNUSED)
{
    FUNC_BLOCK("Graphics::drawImage2", 1)
    // Check that preconditions for blitting are met.
    if (!mTarget || !image || !image->mSDLSurface)
        return false;

    dstX += mClipStack.top().xOffset;
    dstY += mClipStack.top().yOffset;

    srcX += image->mBounds.x;
    srcY += image->mBounds.y;

    SDL_Rect dstRect;
    SDL_Rect srcRect;
    dstRect.x = static_cast<int16_t>(dstX);
    dstRect.y = static_cast<int16_t>(dstY);
    srcRect.x = static_cast<int16_t>(srcX);
    srcRect.y = static_cast<int16_t>(srcY);
    srcRect.w = static_cast<uint16_t>(width);
    srcRect.h = static_cast<uint16_t>(height);

    if (mBlitMode == BLIT_NORMAL)
    {
        return !(SDL_BlitSurface(image->mSDLSurface, &srcRect,
                                 mTarget, &dstRect) < 0);
    }
    else
    {
        return !(SDL_gfxBlitRGBA(image->mSDLSurface, &srcRect,
                                 mTarget, &dstRect) < 0);
    }
}

void SDLGraphics::drawImagePattern(const Image *const image,
                                   const int x, const int y,
                                   const int w, const int h)
{
    FUNC_BLOCK("Graphics::drawImagePattern", 1)
    // Check that preconditions for blitting are met.
    if (!mTarget || !image)
        return;
    if (!image->mSDLSurface)
        return;

    const int iw = image->mBounds.w;
    const int ih = image->mBounds.h;

    if (iw == 0 || ih == 0)
        return;

    for (int py = 0; py < h; py += ih)  // Y position on pattern plane
    {
        const int dh = (py + ih >= h) ? h - py : ih;
        const int srcY = image->mBounds.y;
        const int dstY = y + py + mClipStack.top().yOffset;

        for (int px = 0; px < w; px += iw)  // X position on pattern plane
        {
            const int dw = (px + iw >= w) ? w - px : iw;
            const int srcX = image->mBounds.x;
            const int dstX = x + px + mClipStack.top().xOffset;

            SDL_Rect dstRect;
            SDL_Rect srcRect;
            dstRect.x = static_cast<int16_t>(dstX);
            dstRect.y = static_cast<int16_t>(dstY);
            srcRect.x = static_cast<int16_t>(srcX);
            srcRect.y = static_cast<int16_t>(srcY);
            srcRect.w = static_cast<uint16_t>(dw);
            srcRect.h = static_cast<uint16_t>(dh);

            SDL_BlitSurface(image->mSDLSurface, &srcRect, mTarget, &dstRect);
        }
    }
}

void SDLGraphics::drawRescaledImagePattern(const Image *const image,
                                           const int x, const int y,
                                           const int w, const int h,
                                           const int scaledWidth,
                                           const int scaledHeight)
{
    // Check that preconditions for blitting are met.
    if (!mTarget || !image)
        return;
    if (!image->mSDLSurface)
        return;

    if (scaledHeight == 0 || scaledWidth == 0)
        return;

    Image *const tmpImage = image->SDLgetScaledImage(
        scaledWidth, scaledHeight);
    if (!tmpImage)
        return;

    const int iw = tmpImage->mBounds.w;
    const int ih = tmpImage->mBounds.h;

    if (iw == 0 || ih == 0)
        return;

    for (int py = 0; py < h; py += ih)  // Y position on pattern plane
    {
        const int dh = (py + ih >= h) ? h - py : ih;
        const int srcY = tmpImage->mBounds.y;
        const int dstY = y + py + mClipStack.top().yOffset;

        for (int px = 0; px < w; px += iw)  // X position on pattern plane
        {
            const int dw = (px + iw >= w) ? w - px : iw;
            const int srcX = tmpImage->mBounds.x;
            const int dstX = x + px + mClipStack.top().xOffset;

            SDL_Rect dstRect;
            SDL_Rect srcRect;
            dstRect.x = static_cast<int16_t>(dstX);
            dstRect.y = static_cast<int16_t>(dstY);
            srcRect.x = static_cast<int16_t>(srcX);
            srcRect.y = static_cast<int16_t>(srcY);
            srcRect.w = static_cast<uint16_t>(dw);
            srcRect.h = static_cast<uint16_t>(dh);

            SDL_BlitSurface(tmpImage->mSDLSurface, &srcRect,
                            mTarget, &dstRect);
        }
    }

    delete tmpImage;
}

void SDLGraphics::calcImagePattern(ImageVertexes* const vert,
                                   const Image *const image,
                                   const int x, const int y,
                                   const int w, const int h) const
{
    // Check that preconditions for blitting are met.
    if (!vert || !mTarget || !image || !image->mSDLSurface)
        return;

    const int iw = image->mBounds.w;
    const int ih = image->mBounds.h;

    if (iw == 0 || ih == 0)
        return;

    for (int py = 0; py < h; py += ih)  // Y position on pattern plane
    {
        const int dh = (py + ih >= h) ? h - py : ih;
        const int srcY = image->mBounds.y;
        const int dstY = y + py + mClipStack.top().yOffset;

        for (int px = 0; px < w; px += iw)  // X position on pattern plane
        {
            const int dw = (px + iw >= w) ? w - px : iw;
            const int srcX = image->mBounds.x;
            const int dstX = x + px + mClipStack.top().xOffset;

            DoubleRect *const r = new DoubleRect();
            SDL_Rect &dstRect = r->dst;
            SDL_Rect &srcRect = r->src;
            dstRect.x = static_cast<int16_t>(dstX);
            dstRect.y = static_cast<int16_t>(dstY);
            srcRect.x = static_cast<int16_t>(srcX);
            srcRect.y = static_cast<int16_t>(srcY);
            srcRect.w = static_cast<uint16_t>(dw);
            srcRect.h = static_cast<uint16_t>(dh);

            if (SDL_FakeUpperBlit(image->mSDLSurface, &srcRect,
                mTarget, &dstRect) == 1)
            {
                vert->sdl.push_back(r);
            }
            else
            {
                delete r;
            }
        }
    }
}

void SDLGraphics::calcImagePattern(ImageCollection* const vertCol,
                                   const Image *const image,
                                   const int x, const int y,
                                   const int w, const int h) const
{
    ImageVertexes *vert = nullptr;
    if (vertCol->currentImage != image)
    {
        vert = new ImageVertexes();
        vertCol->currentImage = image;
        vertCol->currentVert = vert;
        vert->image = image;
        vertCol->draws.push_back(vert);
    }
    else
    {
        vert = vertCol->currentVert;
    }

    calcImagePattern(vert, image, x, y, w, h);
}

void SDLGraphics::calcTile(ImageVertexes *const vert,
                           const Image *const image,
                           int x, int y) const
{
    vert->image = image;
    calcTileSDL(vert, x, y);
}

void SDLGraphics::calcTileSDL(ImageVertexes *const vert, int x, int y) const
{
    // Check that preconditions for blitting are met.
    if (!vert || !vert->image || !vert->image->mSDLSurface)
        return;

    const Image *const image = vert->image;

    x += mClipStack.top().xOffset;
    y += mClipStack.top().yOffset;

    DoubleRect *rect = new DoubleRect();

    rect->dst.x = static_cast<int16_t>(x);
    rect->dst.y = static_cast<int16_t>(y);
    rect->src.x = static_cast<int16_t>(image->mBounds.x);
    rect->src.y = static_cast<int16_t>(image->mBounds.y);
    rect->src.w = static_cast<uint16_t>(image->mBounds.w);
    rect->src.h = static_cast<uint16_t>(image->mBounds.h);
    if (SDL_FakeUpperBlit(image->mSDLSurface, &rect->src,
        mTarget, &rect->dst) == 1)
    {
        vert->sdl.push_back(rect);
    }
    else
    {
        delete rect;
    }
}

void SDLGraphics::calcTile(ImageCollection *const vertCol,
                           const Image *const image,
                           int x, int y)
{
    if (vertCol->currentImage != image)
    {
        ImageVertexes *const vert = new ImageVertexes();
        vertCol->currentImage = image;
        vertCol->currentVert = vert;
        vert->image = image;
        vertCol->draws.push_back(vert);
        calcTileSDL(vert, x, y);
    }
    else
    {
        calcTileSDL(vertCol->currentVert, x, y);
    }
}

void SDLGraphics::drawTile(const ImageCollection *const vertCol)
{
    const ImageVertexesVector &draws = vertCol->draws;
    const ImageCollectionCIter it_end = draws.end();
    for (ImageCollectionCIter it = draws.begin(); it != it_end; ++ it)
    {
        const ImageVertexes *const vert = *it;
        const Image *const img = vert->image;
        const DoubleRects *const rects = &vert->sdl;
        DoubleRects::const_iterator it2 = rects->begin();
        const DoubleRects::const_iterator it2_end = rects->end();
        while (it2 != it2_end)
        {
            SDL_LowerBlit(img->mSDLSurface, &(*it2)->src,
                mTarget, &(*it2)->dst);
            ++ it2;
        }
    }
}

void SDLGraphics::drawTile(const ImageVertexes *const vert)
{
    // vert and img must be != 0
    const Image *const img = vert->image;
    const DoubleRects *const rects = &vert->sdl;
    DoubleRects::const_iterator it = rects->begin();
    const DoubleRects::const_iterator it_end = rects->end();
    while (it != it_end)
    {
        SDL_LowerBlit(img->mSDLSurface, &(*it)->src, mTarget, &(*it)->dst);
        ++ it;
    }
}

void SDLGraphics::updateScreen()
{
    BLOCK_START("Graphics::updateScreen")
    if (mDoubleBuffer)
    {
        SDL_Flip(mTarget);
    }
    else
    {
        SDL_UpdateRects(mTarget, 1, &mRect);
//        SDL_UpdateRect(mTarget, 0, 0, 0, 0);
    }
    BLOCK_END("Graphics::updateScreen")
}

SDL_Surface *SDLGraphics::getScreenshot()
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    const int rmask = 0xff000000;
    const int gmask = 0x00ff0000;
    const int bmask = 0x0000ff00;
#else
    const int rmask = 0x000000ff;
    const int gmask = 0x0000ff00;
    const int bmask = 0x00ff0000;
#endif
    const int amask = 0x00000000;

    SDL_Surface *const screenshot = SDL_CreateRGBSurface(SDL_SWSURFACE,
        mTarget->w, mTarget->h, 24, rmask, gmask, bmask, amask);

    if (screenshot)
        SDL_BlitSurface(mTarget, nullptr, screenshot, nullptr);

    return screenshot;
}

bool SDLGraphics::drawNet(const int x1, const int y1,
                          const int x2, const int y2,
                          const int width, const int height)
{
    for (int y = y1; y < y2; y += height)
        drawLine(x1, y, x2, y);

    for (int x = x1; x < x2; x += width)
        drawLine(x, y1, x, y2);

    return true;
}

bool SDLGraphics::calcWindow(ImageCollection *const vertCol,
                             const int x, const int y,
                             const int w, const int h,
                             const ImageRect &imgRect)
{
    ImageVertexes *vert = nullptr;
    Image *const image = imgRect.grid[4];
    if (vertCol->currentImage != image)
    {
        vert = new ImageVertexes();
        vertCol->currentImage = image;
        vertCol->currentVert = vert;
        vert->image = image;
        vertCol->draws.push_back(vert);
    }
    else
    {
        vert = vertCol->currentVert;
    }

    return calcImageRect(vert, x, y, w, h,
        imgRect.grid[0], imgRect.grid[2], imgRect.grid[6], imgRect.grid[8],
        imgRect.grid[1], imgRect.grid[5], imgRect.grid[7], imgRect.grid[3],
        imgRect.grid[4]);
}

int SDLGraphics::SDL_FakeUpperBlit(const SDL_Surface *const src,
                                SDL_Rect *const srcrect,
                                const SDL_Surface *const dst,
                                SDL_Rect *dstrect) const
{
    SDL_Rect fulldst;
    int srcx, srcy, w, h;

    // Make sure the surfaces aren't locked
    if (!src || !dst)
        return -1;

    if (src->locked || dst->locked)
        return -1;

    // If the destination rectangle is nullptr, use the entire dest surface
    if (!dstrect)
    {
        fulldst.x = 0;
        fulldst.y = 0;
        dstrect = &fulldst;
    }

    // clip the source rectangle to the source surface
    if (srcrect)
    {
        srcx = srcrect->x;
        w = srcrect->w;
        if (srcx < 0)
        {
            w += srcx;
            dstrect->x -= static_cast<int16_t>(srcx);
            srcx = 0;
        }
        int maxw = src->w - srcx;
        if (maxw < w)
            w = maxw;

        srcy = srcrect->y;
        h = srcrect->h;
        if (srcy < 0)
        {
            h += srcy;
            dstrect->y -= static_cast<int16_t>(srcy);
            srcy = 0;
        }
        int maxh = src->h - srcy;
        if (maxh < h)
            h = maxh;
    }
    else
    {
        srcx = 0;
        srcy = 0;
        w = src->w;
        h = src->h;
    }

    // clip the destination rectangle against the clip rectangle
    {
        const SDL_Rect *const clip = &dst->clip_rect;
        int dx = clip->x - dstrect->x;
        if (dx > 0)
        {
            w -= dx;
            dstrect->x += static_cast<int16_t>(dx);
            srcx += dx;
        }
        dx = dstrect->x + w - clip->x - clip->w;
        if (dx > 0)
            w -= dx;

        int dy = clip->y - dstrect->y;
        if (dy > 0)
        {
            h -= dy;
            dstrect->y += static_cast<int16_t>(dy);
            srcy += dy;
        }
        dy = dstrect->y + h - clip->y - clip->h;
        if (dy > 0)
            h -= dy;
    }

    if (w > 0 && h > 0)
    {
        if (srcrect)
        {
            srcrect->x = static_cast<int16_t>(srcx);
            srcrect->y = static_cast<int16_t>(srcy);
            srcrect->w = static_cast<int16_t>(w);
            srcrect->h = static_cast<int16_t>(h);
        }
        dstrect->w = static_cast<int16_t>(w);
        dstrect->h = static_cast<int16_t>(h);

        return 1;
//        return SDL_LowerBlit(src, &sr, dst, dstrect);
    }
    dstrect->w = dstrect->h = 0;
    return 0;
}

void SDLGraphics::fillRectangle(const gcn::Rectangle& rectangle)
{
    FUNC_BLOCK("Graphics::fillRectangle", 1)
    if (mClipStack.empty())
        return;

    const gcn::ClipRectangle& top = mClipStack.top();

    gcn::Rectangle area = rectangle;
    area.x += top.xOffset;
    area.y += top.yOffset;

    if (!area.isIntersecting(top))
        return;

    if (mAlpha)
    {
        const int x1 = area.x > top.x ? area.x : top.x;
        const int y1 = area.y > top.y ? area.y : top.y;
        const int x2 = area.x + area.width < top.x + top.width ?
            area.x + area.width : top.x + top.width;
        const int y2 = area.y + area.height < top.y + top.height ?
            area.y + area.height : top.y + top.height;
        int x, y;

        SDL_LockSurface(mTarget);

        const int bpp = mTarget->format->BytesPerPixel;
        const uint32_t pixel = SDL_MapRGB(mTarget->format,
            static_cast<uint8_t>(mColor.r), static_cast<uint8_t>(mColor.g),
            static_cast<uint8_t>(mColor.b));

        switch (bpp)
        {
            case 1:
                for (y = y1; y < y2; y++)
                {
                    uint8_t *const p = static_cast<uint8_t *>(mTarget->pixels)
                        + y * mTarget->pitch;
                    for (x = x1; x < x2; x++)
                        *(p + x) = static_cast<uint8_t>(pixel);
                }
                break;
            case 2:
                for (y = y1; y < y2; y++)
                {
                    uint8_t *const p0 = static_cast<uint8_t *>(mTarget->pixels)
                        + y * mTarget->pitch;
                    for (x = x1; x < x2; x++)
                    {
                        uint8_t *const p = p0 + x * 2;
                        *reinterpret_cast<uint16_t *>(p) = gcn::SDLAlpha16(
                            static_cast<uint16_t>(pixel),
                            *reinterpret_cast<uint16_t *>(p),
                            static_cast<uint8_t>(mColor.a), mTarget->format);
                    }
                }
                break;
            case 3:
            {
                const int ca = 255 - mColor.a;
                const int cr = mColor.r * mColor.a;
                const int cg = mColor.g * mColor.a;
                const int cb = mColor.b * mColor.a;

                for (y = y1; y < y2; y++)
                {
                    uint8_t *const p0 = static_cast<uint8_t *>(mTarget->pixels)
                        + y * mTarget->pitch;
                    for (x = x1; x < x2; x++)
                    {
                        uint8_t *const p = p0 + x * 3;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
                        p[2] = static_cast<uint8_t>((p[2] * ca + cb) >> 8);
                        p[1] = static_cast<uint8_t>((p[1] * ca + cg) >> 8);
                        p[0] = static_cast<uint8_t>((p[0] * ca + cr) >> 8);
#else
                        p[0] = static_cast<uint8_t>((p[0] * ca + cb) >> 8);
                        p[1] = static_cast<uint8_t>((p[1] * ca + cg) >> 8);
                        p[2] = static_cast<uint8_t>((p[2] * ca + cr) >> 8);
#endif
                    }
                }
                break;
            }
            case 4:
            {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
                const unsigned pb = (pixel & 0xff) * mColor.a;
                const unsigned pg = (pixel & 0xff00) * mColor.a;
                const unsigned pr = (pixel & 0xff0000) * mColor.a;
                const unsigned a1 = (255 - mColor.a);

                for (y = y1; y < y2; y++)
                {
                    uint8_t *const p0 = static_cast<uint8_t *>(mTarget->pixels)
                        + y * mTarget->pitch;
                    for (x = x1; x < x2; x++)
                    {
                        uint8_t *p = p0 + x * 4;
                        uint32_t dst = *reinterpret_cast<uint32_t *>(p);
                        const unsigned int b = (pb + (dst & 0xff) * a1) >> 8;
                        const unsigned int g = (pg + (dst & 0xff00) * a1) >> 8;
                        const unsigned int r = (pr
                            + (dst & 0xff0000) * a1) >> 8;

                        *reinterpret_cast<uint32_t *>(p) = ((b & 0xff)
                            | (g & 0xff00) | (r & 0xff0000));
                    }
                }
#else
                if (!cR)
                {
                    cR = new unsigned int[0x100];
                    cG = new unsigned int[0x100];
                    cB = new unsigned int[0x100];
                    mOldPixel = 0;
                    mOldAlpha = mColor.a;
                }

                const SDL_PixelFormat * const format = mTarget->format;
                const unsigned rMask = format->Rmask;
                const unsigned gMask = format->Gmask;
                const unsigned bMask = format->Bmask;
//                const unsigned aMask = format->Amask;
                unsigned rShift = rMask / 0xff;
                unsigned gShift = gMask / 0xff;
                unsigned bShift = bMask / 0xff;
                if (!rShift)
                    rShift = 1;
                if (!gShift)
                    gShift = 1;
                if (!bShift)
                    bShift = 1;
                if (pixel != mOldPixel || mColor.a != mOldAlpha)
                {
                    const unsigned pb = (pixel & bMask) * mColor.a;
                    const unsigned pg = (pixel & gMask) * mColor.a;
                    const unsigned pr = (pixel & rMask) * mColor.a;
                    const unsigned a0 = (255 - mColor.a);

                    const unsigned int a1 = a0 * bShift;
                    const unsigned int a2 = a0 * gShift;
                    const unsigned int a3 = a0 * rShift;

                    for (int f = 0; f <= 0xff; f ++)
                    {
                        cB[f] = ((pb + f * a1) >> 8) & bMask;
                        cG[f] = ((pg + f * a2) >> 8) & gMask;
                        cR[f] = ((pr + f * a3) >> 8) & rMask;
                    }

                    mOldPixel = pixel;
                    mOldAlpha = mColor.a;
                }

                for (y = y1; y < y2; y++)
                {
                    uint32_t *const p0 = reinterpret_cast<uint32_t*>(
                        static_cast<uint8_t*>(mTarget->pixels)
                        + y * mTarget->pitch);
                    for (x = x1; x < x2; x++)
                    {
                        uint32_t *const p = p0 + x;
                        const uint32_t dst = *p;
                        *p = cB[dst & bMask / bShift]
                            | cG[(dst & gMask) / gShift]
                            | cR[(dst & rMask) / rShift];
                    }
                }
#endif
                break;
            }
            default:
                break;
        }

        SDL_UnlockSurface(mTarget);
    }
    else
    {
        SDL_Rect rect;
        rect.x = static_cast<int16_t>(area.x);
        rect.y = static_cast<int16_t>(area.y);
        rect.w = static_cast<uint16_t>(area.width);
        rect.h = static_cast<uint16_t>(area.height);

        const uint32_t color = SDL_MapRGBA(mTarget->format,
            static_cast<int8_t>(mColor.r),
            static_cast<int8_t>(mColor.g),
            static_cast<int8_t>(mColor.b),
            static_cast<int8_t>(mColor.a));
        SDL_FillRect(mTarget, &rect, color);
    }
}

void SDLGraphics::_beginDraw()
{
    pushClipArea(gcn::Rectangle(0, 0, mTarget->w, mTarget->h));
}

void SDLGraphics::_endDraw()
{
    popClipArea();
}

bool SDLGraphics::pushClipArea(gcn::Rectangle area)
{
    SDL_Rect rect;
    const bool result = gcn::Graphics::pushClipArea(area);

    const gcn::ClipRectangle &carea = mClipStack.top();
    rect.x = static_cast<int16_t>(carea.x);
    rect.y = static_cast<int16_t>(carea.y);
    rect.w = static_cast<int16_t>(carea.width);
    rect.h = static_cast<int16_t>(carea.height);
    SDL_SetClipRect(mTarget, &rect);

    return result;
}

void SDLGraphics::popClipArea()
{
    gcn::Graphics::popClipArea();

    if (mClipStack.empty())
        return;

    const gcn::ClipRectangle &carea = mClipStack.top();
    SDL_Rect rect;
    rect.x = static_cast<int16_t>(carea.x);
    rect.y = static_cast<int16_t>(carea.y);
    rect.w = static_cast<int16_t>(carea.width);
    rect.h = static_cast<int16_t>(carea.height);

    SDL_SetClipRect(mTarget, &rect);
}

void SDLGraphics::drawPoint(int x, int y)
{
    if (mClipStack.empty())
        return;

    const gcn::ClipRectangle& top = mClipStack.top();

    x += top.xOffset;
    y += top.yOffset;

    if (!top.isPointInRect(x, y))
        return;

    if (mAlpha)
        SDLputPixelAlpha(mTarget, x, y, mColor);
    else
        SDLputPixel(mTarget, x, y, mColor);
}

void SDLGraphics::drawHLine(int x1, int y, int x2)
{
    if (mClipStack.empty())
        return;

    const gcn::ClipRectangle& top = mClipStack.top();

    const int xOffset = top.xOffset;
    x1 += xOffset;
    y += top.yOffset;
    x2 += xOffset;

    const int topY = top.y;
    if (y < topY || y >= topY + top.height)
        return;

    if (x1 > x2)
    {
        x1 ^= x2;
        x2 ^= x1;
        x1 ^= x2;
    }

    const int topX = top.x;
    if (topX > x1)
    {
        if (topX > x2)
            return;

        x1 = topX;
    }

    const int sumX = topX + top.width;
    if (sumX <= x2)
    {
        if (sumX <= x1)
            return;

        x2 = sumX -1;
    }

    const int bpp = mTarget->format->BytesPerPixel;

    SDL_LockSurface(mTarget);

    uint8_t *p = static_cast<uint8_t*>(mTarget->pixels)
        + y * mTarget->pitch + x1 * bpp;

    const uint32_t pixel = SDL_MapRGB(mTarget->format,
        static_cast<uint8_t>(mColor.r),
        static_cast<uint8_t>(mColor.g),
        static_cast<uint8_t>(mColor.b));
    switch (bpp)
    {
        case 1:
            for (; x1 <= x2; ++x1)
                *(p++) = static_cast<uint8_t>(pixel);
            break;

        case 2:
        {
            uint16_t* q = reinterpret_cast<uint16_t*>(p);
            for (; x1 <= x2; ++x1)
                *(q++) = pixel;
            break;
        }

        case 3:
        {
            const uint8_t b0 = static_cast<uint8_t>((pixel >> 16) & 0xff);
            const uint8_t b1 = static_cast<uint8_t>((pixel >> 8) & 0xff);
            const uint8_t b2 = static_cast<uint8_t>(pixel & 0xff);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            for (; x1 <= x2; ++x1)
            {
                p[0] = b0;
                p[1] = b1;
                p[2] = b2;
                p += 3;
            }
#else
            for (; x1 <= x2; ++x1)
            {
                p[0] = b2;
                p[1] = b1;
                p[2] = b0;
                p += 3;
            }
#endif
            break;
        }

        case 4:
        {
            uint32_t *q = reinterpret_cast<uint32_t*>(p);
            if (mAlpha)
            {
                unsigned char a = static_cast<unsigned char>(mColor.a);
                unsigned char a1 = 255 - a;
                const int b0 = (pixel & 0xff) * a;
                const int g0 = (pixel & 0xff00) * a;
                const int r0 = (pixel & 0xff0000) * a;
                for (; x1 <= x2; ++x1)
                {
                    const unsigned int b = (b0 + (*q & 0xff) * a1) >> 8;
                    const unsigned int g = (g0 + (*q & 0xff00) * a1) >> 8;
                    const unsigned int r = (r0 + (*q & 0xff0000) * a1) >> 8;
                    *q = (b & 0xff) | (g & 0xff00) | (r & 0xff0000);

                    q++;
                }
            }
            else
            {
                for (; x1 <= x2; ++x1)
                    *(q++) = pixel;
            }
            break;
        }
        default:
            break;
    }  // end switch

    SDL_UnlockSurface(mTarget);
}

void SDLGraphics::drawVLine(int x, int y1, int y2)
{
    if (mClipStack.empty())
        return;

    const gcn::ClipRectangle& top = mClipStack.top();

    const int yOffset = top.yOffset;
    x += top.xOffset;
    y1 += yOffset;
    y2 += yOffset;

    if (x < top.x || x >= top.x + top.width)
        return;

    if (y1 > y2)
    {
        y1 ^= y2;
        y2 ^= y1;
        y1 ^= y2;
    }

    if (top.y > y1)
    {
        if (top.y > y2)
            return;

        y1 = top.y;
    }

    const int sumY = top.y + top.height;
    if (sumY <= y2)
    {
        if (sumY <= y1)
            return;

        y2 = sumY - 1;
    }

    const int bpp = mTarget->format->BytesPerPixel;

    SDL_LockSurface(mTarget);

    uint8_t *p = static_cast<uint8_t*>(mTarget->pixels)
        + y1 * mTarget->pitch + x * bpp;

    const uint32_t pixel = SDL_MapRGB(mTarget->format,
        static_cast<uint8_t>(mColor.r),
        static_cast<uint8_t>(mColor.g),
        static_cast<uint8_t>(mColor.b));

    const int pitch = mTarget->pitch;
    switch (bpp)
    {
        case 1:
            for (; y1 <= y2; ++y1)
            {
                *p = static_cast<uint8_t>(pixel);
                p += pitch;
            }
            break;

        case 2:
            for (; y1 <= y2; ++ y1)
            {
                *reinterpret_cast<uint16_t*>(p)
                    = static_cast<uint16_t>(pixel);
                p += pitch;
            }
            break;

        case 3:
        {
            const uint8_t b0 = static_cast<uint8_t>((pixel >> 16) & 0xff);
            const uint8_t b1 = static_cast<uint8_t>((pixel >> 8) & 0xff);
            const uint8_t b2 = static_cast<uint8_t>(pixel & 0xff);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            for (; y1 <= y2; ++y1)
            {
                p[0] = b0;
                p[1] = b1;
                p[2] = b2;
                p += pitch;
            }
#else
            for (; y1 <= y2; ++y1)
            {
                p[0] = b2;
                p[1] = b1;
                p[2] = b0;
                p += pitch;
            }
#endif
            break;
        }

        case 4:
        {
            if (mAlpha)
            {
                unsigned char a = static_cast<unsigned char>(mColor.a);
                unsigned char a1 = 255 - a;
                const int b0 = (pixel & 0xff) * a;
                const int g0 = (pixel & 0xff00) * a;
                const int r0 = (pixel & 0xff0000) * a;
                for (; y1 <= y2; ++y1)
                {
                    const unsigned int dst = *reinterpret_cast<uint32_t*>(p);
                    const unsigned int b = (b0 + (dst & 0xff) * a1) >> 8;
                    const unsigned int g = (g0 + (dst & 0xff00) * a1) >> 8;
                    const unsigned int r = (r0 + (dst & 0xff0000) * a1) >> 8;
                    *reinterpret_cast<uint32_t*>(p) =
                        (b & 0xff) | (g & 0xff00) | (r & 0xff0000);

                    p += pitch;
                }
            }
            else
            {
                for (; y1 <= y2; ++y1)
                {
                    *reinterpret_cast<uint32_t*>(p) = pixel;
                    p += pitch;
                }
            }
            break;
        }

        default:
            break;
    }  // end switch

    SDL_UnlockSurface(mTarget);
}

void SDLGraphics::drawRectangle(const gcn::Rectangle &rectangle)
{
    const int x1 = rectangle.x;
    const int x2 = x1 + rectangle.width - 1;
    const int y1 = rectangle.y;
    const int y2 = y1 + rectangle.height - 1;

    drawHLine(x1, y1, x2);
    drawHLine(x1, y2, x2);

    drawVLine(x1, y1, y2);
    drawVLine(x2, y1, y2);
}

void SDLGraphics::drawLine(int x1, int y1, int x2, int y2)
{
    if (x1 == x2)
    {
        drawVLine(x1, y1, y2);
        return;
    }
    if (y1 == y2)
    {
        drawHLine(x1, y1, x2);
        return;
    }

    //  other cases not implimented
}