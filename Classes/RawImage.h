//
//  RawImage.h
//  AndroidScreenShot
//
//  Created by Wei Mike on 13-7-12.
//  Copyright (c) 2013年 Liqu. All rights reserved.
//

#ifndef __AndroidScreenShot__RawImage__
#define __AndroidScreenShot__RawImage__

#include "ggbase.h"

#ifdef WIN32
typedef int int32_t;
#endif

typedef struct _packet16 {
	int32_t size;
	int32_t width;
	int32_t height;
	char value[];
} _packet16;

typedef struct _packet1 {
	int32_t bpp;
    int32_t size;
    int32_t width;
    int32_t height;
    int32_t red_offset;
    int32_t red_length;
    int32_t blue_offset;
    int32_t blue_length;
    int32_t green_offset;
    int32_t green_length;
    int32_t alpha_offset;
    int32_t alpha_length;
	char value[];
} _packet1;

class RawImage {
    
public:
    
    int32_t version;
    int32_t bpp;
    int32_t size;
    int32_t width;
    int32_t height;
    int32_t red_offset;
    int32_t red_length;
    int32_t blue_offset;
    int32_t blue_length;
    int32_t green_offset;
    int32_t green_length;
    int32_t alpha_offset;
    int32_t alpha_length;
    
    char* data;
    
    RawImage();
    
    virtual ~RawImage();
    
    static int32_t getMask(int32_t length) { return (1 << length) - 1; }
    
    bool readHeader(int32_t version, const char* buf);
    
    static int getHeaderSize(int version);
};

#endif /* defined(__AndroidScreenShot__RawImage__) */
