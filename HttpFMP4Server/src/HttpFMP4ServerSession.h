/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvServerSession.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FLV_SERVER_SESSION_H
#define HTTP_FLV_SERVER_SESSION_H

#include "HttpServer.h"
#include "MediaHandle.h"
#include <thread>
#include <mutex>
#include <string>
#include <list>
#include <map>

using std::map;
using std::string;
using std::list;
using std::mutex;
using std::thread;



/*****************************************************************************
-Class          : HttpFlvServerSession
-Description    : HttpFlvServerSession
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2022/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class HttpFlvServerSession
{
public:
	HttpFlvServerSession(char * i_strPlaySrc,int i_iEnhancedFlag=0);
	virtual ~HttpFlvServerSession();
    int GetFlv(char *o_strRes,int i_iResMaxLen);//return ResLen,<0 err
private:
    unsigned int GetTickCount();

    int m_iEnhancedFlag;
    string *m_pPlaySrc;
    MediaHandle *m_pMediaHandle;
    T_MediaFrameInfo m_tFileFrameInfo;
	unsigned int m_dwFileLastTimeStamp;
	unsigned int m_dwFileLastTick;
};













#endif
