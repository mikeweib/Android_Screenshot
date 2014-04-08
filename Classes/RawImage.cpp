//
//  RawImage.cpp
//  AndroidScreenShot
//
//  Created by Wei Mike on 13-7-12.
//  Copyright (c) 2013年 Liqu. All rights reserved.
//

#include "RawImage.h"

RawImage::RawImage():data(NULL) {
    version = 0;
    bpp = 0;
    size = 0;
    width = 0;
    height = 0;
    red_offset = 0;
    red_length = 0;
    blue_offset = 0;
    blue_length = 0;
    green_offset = 0;
    green_length = 0;
    alpha_offset = 0;
    alpha_length = 0;

}

RawImage::~RawImage() {
    if (data)
        free((void*)data);
}

int RawImage::getHeaderSize(int version) {
    switch (version) {
        case 16: // compatibility mode
            return 3; // size, width, height
        case 1:
            return 12; // bpp, size, width, height, 4*(length, offset)
    }
    
    return 0;
}

bool RawImage::readHeader(int version, const char* buf) {
    this->version = version;
    
    if (version == 16) {
        // compatibility mode with original protocol
        this->bpp = 16;
        
        _packet16* packet = (_packet16*)buf;
        // read actual values.
        this->size = (packet->size);
        this->width = (packet->width);
        this->height = (packet->height);
        
        // create default values for the rest. Format is 565
        this->red_offset = 11;
        this->red_length = 5;
        this->green_offset = 5;
        this->green_length = 6;
        this->blue_offset = 0;
        this->blue_length = 5;
        this->alpha_offset = 0;
        this->alpha_length = 0;
        
    } else if (version == 1) {
        _packet1* packet = (_packet1*)buf;
        
        this->bpp = (packet->bpp);
        this->size = (packet->size);
        this->width = (packet->width);
        this->height = (packet->height);
        this->red_offset = (packet->red_offset);
        this->red_length = (packet->red_length);
        this->blue_offset = (packet->blue_offset);
        this->blue_length = (packet->blue_length);
        this->green_offset = (packet->green_offset);
        this->green_length = (packet->green_length);
        this->alpha_offset = (packet->alpha_offset);
        this->alpha_length = (packet->alpha_length);
    } else {
        // unsupported protocol!
        return false;
    }
    
    return true;
}
