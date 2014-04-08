//
//  AdbHelper.h
//  AndroidScreenShot
//
//  Created by Wei Mike on 13-7-11.
//  Copyright (c) 2013年 Liqu. All rights reserved.
//

#ifndef __AndroidScreenShot__AdbHelper__
#define __AndroidScreenShot__AdbHelper__

#include "ggbase.h"

#include "AdbResponse.h"
#include "RawImage.h"

enum SCREENSHOT_RET_CODE {
    SCREENSHOT_GET_IMAGE_SUCCESS = 1,
    SCREENSHOT_ERROR_SOCKET_CREATE = 2,
    SCREENSHOT_ERROR_CONNECT = 3,
    SCREENSHOT_ERROR_SET_DEVICE = 4,
    SCREENSHOT_ERROR_GET_IMAGE = 5,
    SCREENSHOT_ERROR_UNSUPPORT_VERSION = 6
    };


class RawImageCallback {
public:
	virtual void onGetFrameBuffer(SCREENSHOT_RET_CODE ret, RawImage* rawImage) {}
};

class AdbHelper {
private:
    const char* m_host;
    
	int32_t m_port;
    
    const char* m_device;
    
    gg_thread_t m_asyncThreadId;
    
private:
    AdbHelper();
    
    gg_mutex_t m_mutex_callback;
    
    RawImageCallback* m_callback;
    
    bool setDevice(const int sock, const char* device);
    
    SCREENSHOT_RET_CODE getFrameBuffer(const int sock, RawImage& outRawImage);
    
    void convertImageData(RawImage& rawImage, const char* data);
    
    AdbResponse* readAdbResponse(const int sock, bool readDiagString);
    
    char* formAdbRequest(const char* req);
    
    bool write(const int sock, const char* req);
    
#ifdef WIN32
	static unsigned __stdcall threadGetFrameBuffer(void * arg);
#else
	static void* threadGetFrameBuffer(void* arg);
#endif
    
public:
    static AdbHelper* getInstance();
    
	virtual ~AdbHelper();
    
    void asyncGetFrameBuffer(const char* host, int port, const char* device);
    
    SCREENSHOT_RET_CODE getFrameBuffer(const char* host, int port, const char* device, RawImage& outRawImage);
    
    void setCallback(RawImageCallback* callback);
    
};

#endif /* defined(__AndroidScreenShot__AdbHelper__) */
