//
//  AdbResponse.h
//  AndroidScreenShot
//
//  Created by Wei Mike on 13-7-11.
//  Copyright (c) 2013年 Liqu. All rights reserved.
//

#ifndef __AndroidScreenShot__AdbResponse__
#define __AndroidScreenShot__AdbResponse__

class AdbResponse {
private:
    bool m_okay;
    
    const char* m_message;
    
public:
    
    AdbResponse();
    
    virtual ~AdbResponse();
    
    const char* getMessage() { return m_message; }
    
    bool getRet() { return m_okay; }
    
    void setRet(bool okay) { m_okay = okay; }

    void setMessage(const char* message);
};

#endif /* defined(__AndroidScreenShot__AdbResponse__) */
