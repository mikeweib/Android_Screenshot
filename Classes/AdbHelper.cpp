//
//  AdbHelper.cpp
//  AndroidScreenShot
//
//  Created by Wei Mike on 13-7-11.
//  Copyright (c) 2013年 Liqu. All rights reserved.
//

#include "AdbHelper.h"

#define REPLY_LENGTH 4

AdbHelper* gAdbHelper = NULL;

AdbHelper::AdbHelper() :
        m_host(NULL),
        m_device(NULL),
        m_callback(NULL),
        m_asyncThreadId(NULL) {
    m_mutex_callback = gg_mutex_create();
}

AdbHelper::~AdbHelper() {
    if (m_host) free((void*)m_host);
    
	if (m_device) free((void*)m_device);
    
    gg_mutex_lock(m_mutex_callback);
    LQ_SAFE_DELETE(m_callback);
    gg_mutex_unlock(m_mutex_callback);
    gg_mutex_destroy(m_mutex_callback);
    
}

AdbHelper* AdbHelper::getInstance() {
	if (gAdbHelper == NULL) {
		gAdbHelper = new AdbHelper();
	}
	return gAdbHelper;
}

#ifdef WIN32
unsigned __stdcall AdbHelper::threadGetFrameBuffer(void * arg) {
#else
void* AdbHelper::threadGetFrameBuffer(void* arg) {
#endif
    AdbHelper* thiz = (AdbHelper*)arg;
    RawImage rawImage;
    
    SCREENSHOT_RET_CODE ret = thiz->getFrameBuffer(thiz->m_host, thiz->m_port, thiz->m_device, rawImage);

    if (thiz->m_callback) {
        gg_mutex_lock(thiz->m_mutex_callback);
        thiz->m_callback->onGetFrameBuffer(ret, &rawImage);
        gg_mutex_unlock(thiz->m_mutex_callback);
    }
    return 0;
}

void AdbHelper::setCallback(RawImageCallback* callback) {
    if (m_callback != callback) {
        gg_mutex_lock(m_mutex_callback);
        LQ_SAFE_DELETE(m_callback);
        m_callback = callback;
        gg_mutex_unlock(m_mutex_callback);
    }
}

// convert image data
void AdbHelper::convertImageData(RawImage& rawImage, const char* data) {
    int bpp  = rawImage.bpp;
    int width= rawImage.width;
    int height = rawImage.height;
    
    int alpha_length = rawImage.alpha_length;
    int red_length = rawImage.red_length;
    int green_length = rawImage.green_length;
    int blue_length = rawImage.blue_length;
    
    int red_offset = rawImage.red_offset;
    int green_offset = rawImage.green_offset;
    int blue_offset = rawImage.blue_offset;
    int alpha_offset = rawImage.alpha_offset;
    
    char* newData = (char*)calloc(width*height*4 + 1, sizeof(char));
    
    int indexInc = bpp >> 3;
    int index = 0;
    int i = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int value;
            if (bpp == 16) {
                value  = data[index] & 0x00FF;
                value |= (data[index+1] << 8) & 0x0FF00;
            } else if (bpp == 32) {
                value = data[index] & 0x00FF;
                value |= (data[index+1] & 0x00FF) << 8;
                value |= (data[index+2] & 0x00FF) << 16;
                value |= (data[index+3] & 0x00FF) << 24;
            }
            index += indexInc;
            
            unsigned int v = (unsigned int)value;
            i += 4;
            newData[i] = ((v >> red_offset) & RawImage::getMask(red_length)) << (8 - red_length);
            newData[i+1] = ((v >> green_offset) & RawImage::getMask(green_length)) << (8 - green_length);
            newData[i+2] = ((v >> blue_offset) & RawImage::getMask(blue_length)) << (8 - blue_length);

            if (alpha_length == 0) {
                newData[i+3] = 0xFF; // force alpha to opaque if there's no alpha value in the framebuffer.
            } else {
                newData[i+3] = ((v >> alpha_offset) & RawImage::getMask(alpha_length)) << (8 - alpha_length);
            }
            
        }
    }
    rawImage.data = newData;
    
}

// set device
bool AdbHelper::setDevice(const int sock, const char* device) {
    bool ret = false;
    char buf[256];
    // send device
    sprintf(buf, "host:transport:%s", device);
    
    char* sendDevice = formAdbRequest(buf);
    gg_socket_send(sock, sendDevice, strlen((const char*)sendDevice));
    free((void*)sendDevice);
    
    AdbResponse* response = readAdbResponse(sock, false);
    ret = response->getRet();
    free(response);
    return ret;
}

// get framebuffer
SCREENSHOT_RET_CODE AdbHelper::getFrameBuffer(const int sock, RawImage& outRawImage) {
    SCREENSHOT_RET_CODE ret = SCREENSHOT_GET_IMAGE_SUCCESS;
    AdbResponse* response = NULL;
    char* request = NULL;
    do {
        char buf[256] = "framebuffer:";
        // send framebuffer
    
        request = formAdbRequest(buf);
        gg_socket_send(sock, request, strlen((const char*)request));
        free((void*)request);
        request = NULL;
    
        response = readAdbResponse(sock, false);
        if (!response->getRet()) {
            ret = SCREENSHOT_ERROR_GET_IMAGE;
            ggLogD("Screen capture error:%s\n", response->getMessage());
            break;
        }
        free(response);
        response = NULL;
        
        
        // read version
        const char* versionStr = gg_socket_recv(sock, sizeof(int));
        int version = *versionStr;
        free((void*)versionStr);
        
        
        // read header
        int headerSize = RawImage::getHeaderSize(version);
        if (headerSize == 0) {
            ret = SCREENSHOT_ERROR_UNSUPPORT_VERSION;
            ggLogD("Unsupport image version:%d\n", version);
            break;
        }
        
        const char* headerStr = gg_socket_recv(sock, sizeof(int) * headerSize);
        outRawImage.readHeader(version, headerStr);
        free((void*)headerStr);
        
        
        const char* data = gg_socket_recv(sock, outRawImage.size);
        convertImageData(outRawImage, data);
        free((void*)data);
        
    } while(0);
    
    if (response) {
        free(response);
        response = NULL;
    }
    if (request) {
        free(request);
        request = NULL;
    }
    ggLogD("Screen capture. bpp=%d, size=%d, width=%d, height=%d\n", outRawImage.bpp, outRawImage.size, outRawImage.width, outRawImage.height);
    return ret;
}


// read response from socket
AdbResponse* AdbHelper::readAdbResponse(const int sock, bool readDiagString) {
    AdbResponse* rep = new AdbResponse();
    
    const char* reply = gg_socket_recv(sock, REPLY_LENGTH);
    
    if (strcmp(reply, "OKAY") == 0) {
        rep->setRet(true);
    } else {
        readDiagString = true;
        rep->setRet(false);
    }
    free((void*)reply);
    
    // read message
    if (readDiagString) {
        const char* lenstr = gg_socket_recv(sock, REPLY_LENGTH);
        int len;
        sscanf(lenstr,"%x",&len);
        free((void*)lenstr);
        
        const char* message = gg_socket_recv(sock, len);
        rep->setMessage(message);
        free((void*)message);
    }
    
    return rep;
}

char* AdbHelper::formAdbRequest(const char* req) {
    int length = (int)strlen(req);
    char* buf = (char*)calloc(length + 4 + 1, sizeof(char));
    sprintf(buf, "%04X%s", length, req);
    return buf;
}

bool AdbHelper::write(const int sock, const char* req) {
    bool ret = false;;
    char* buf = formAdbRequest(req);
    ret = gg_socket_send(sock, buf, strlen(buf));
    free((void*)buf);
    return ret;
}

SCREENSHOT_RET_CODE AdbHelper::getFrameBuffer(const char* host, int port, const char* device, RawImage& outRawImage) {
    SCREENSHOT_RET_CODE ret = SCREENSHOT_GET_IMAGE_SUCCESS;
    GG_SOCKET sock = 0;
    do {
        sock = gg_socket_create(PF_INET, SOCK_STREAM, 0);
        if (!sock) {
            ggLogD("fail create socket.");
            ret = SCREENSHOT_ERROR_SOCKET_CREATE;
            break;
        }
        
        if (!gg_socket_connect(sock, host, port)) {
            ggLogD("fail connect %s, %d.", host, port);
            ret = SCREENSHOT_ERROR_CONNECT;
            break;
        }

        if (!setDevice(sock, device)) {
            ggLogD("Screen capture can not set device\n");
            ret = SCREENSHOT_ERROR_SET_DEVICE;
            break;
        }
        
        // get framebuffer
        ret = getFrameBuffer(sock, outRawImage);
    } while(0);
    
    if (sock) {
        gg_socket_shutdown(sock);
        gg_socket_close(sock);
		sock = 0;
    }
    
    return ret;
}

void AdbHelper::asyncGetFrameBuffer(const char* host, int port, const char* device) {
    if (m_host) free((void*)m_host);
    if (m_device) free((void*)m_device);
    
    if (m_asyncThreadId) {
        gg_thread_wait(m_asyncThreadId);
        gg_thread_destroy(m_asyncThreadId);
    }
        
    m_host = gg_copy_char(host);
    m_port = port;
    m_device = gg_copy_char(device);

    m_asyncThreadId = gg_thread_create(threadGetFrameBuffer, this);
}
