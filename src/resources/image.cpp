/*
 *  The Mana World
 *  Copyright 2004 The Mana World Development Team
 *
 *  This file is part of The Mana World.
 *
 *  The Mana World is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  The Mana World is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Mana World; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  $Id$
 */

#include "../log.h"
#include "image.h"
#include <iostream>

Image::Image(BITMAP *image):
    image(image)
{
}

Image::~Image()
{
    unload();
}

Image* Image::load(const std::string &filePath)
{
    // Attempt to use SDL_Image to load the file.
    //image = IMG_Load(filePath.c_str());
    std::cout << "Attempting to load image from " << filePath << std::endl;
    BITMAP *image = load_bitmap(filePath.c_str(), NULL);

    // Check if the file was opened and return the appropriate value.
    if (!image) {
        //log("Error", "Image load failed : %s", IMG_GetError());
        log("Error", "Image load failed : %s", filePath.c_str());
        return NULL;
    }

    return new Image(image);
}

void Image::unload()
{
    // Free the image surface.
    if (image != NULL) {
        //SDL_FreeSurface(image);
        destroy_bitmap(image);
        image = NULL;
        loaded = false;
    }
}

Image* Image::createSubImage(int x, int y, int width, int height)
{
    // Create a new clipped sub-image
    return new SubImage(this, image, x, y, width, height);
}

bool Image::draw(BITMAP *screen, int x, int y)
{
    // Check that preconditions for blitting are met.
    if (screen == NULL || image == NULL) return false;

    // Draw the image onto the screen.
    draw_sprite(screen, image, x, y);
    //if (SDL_BlitSurface(image, NULL, screen, &screenRect) < 0) {
    //    return false;
    //}

    return true;
}

SubImage::SubImage(Image *parent, BITMAP *image,
        int x, int y, int width, int height):
    Image(create_sub_bitmap(image, x, y, width, height)),
    parent(parent)
{
    //this->image = create_sub_bitmap(image, x, y, width, height);
    parent->incRef();
    // Set up the clipping rectangle.
    //clipRect.x = x;
    //clipRect.y = y;
    //clipRect.w = width;
    //clipRect.h = height;
}

SubImage::~SubImage()
{
    if (image) {
        destroy_bitmap(image);
    }
    parent->decRef();
}

void SubImage::unload()
{
}

bool SubImage::draw(BITMAP *screen, int x, int y)
{
    // Check that drawing preconditions are satisfied.
    if (screen == NULL || image == NULL) return false;

    // Draw the image onto the screen.
    draw_sprite(screen, image, x, y);
    //if (SDL_BlitSurface(image, &clipRect, screen, &screenRect) < 0) {
    //    return false;
    //}

    return true;
}
