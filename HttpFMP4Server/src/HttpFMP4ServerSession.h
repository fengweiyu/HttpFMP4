/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFMP4ServerSession.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FMP4_SERVER_SESSION_H
#define HTTP_FMP4_SERVER_SESSION_H

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
-Class          : HttpFMP4ServerSession
-Description    : HttpFMP4ServerSession
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2022/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class HttpFMP4ServerSession
{
public:
	HttpFMP4ServerSession(char * i_strPlaySrc);
	virtual ~HttpFMP4ServerSession();
    int GetFMP4(char *o_strRes,int i_iResMaxLen);//return ResLen,<0 err
private:
    unsigned int GetTickCount();

    string *m_pPlaySrc;
    MediaHandle *m_pMediaHandle;
    T_MediaFrameInfo m_tFileFrameInfo;
	unsigned int m_dwFileLastTimeStamp;
	unsigned int m_dwFileLastTick;
};













#endif
