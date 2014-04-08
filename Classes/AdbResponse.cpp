//
//  AdbResponse.cpp
//  AndroidScreenShot
//
//  Created by Wei Mike on 13-7-11.
//  Copyright (c) 2013年 Liqu. All rights reserved.
//

#include "AdbResponse.h"
#include "ggbase.h"

AdbResponse::AdbResponse():
    m_okay(false),
    m_message(NULL) {

}

AdbResponse::~AdbResponse() {
    if (m_message) {
        free((void*)m_message);
        m_message = NULL;
    }
}

void AdbResponse::setMessage(const char* message) {
    if(m_message)
		free((void*)m_message);
    
	m_message = gg_copy_char_length(message, (int)strlen(message));
}