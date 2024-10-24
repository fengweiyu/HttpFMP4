/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFMP4Server.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FMP4_SERVER_H
#define HTTP_FMP4_SERVER_H

#include "HttpServer.h"
#include "HttpFMP4ServerSession.h"


/*****************************************************************************
-Class          : HttpFMP4Server
-Description    : HttpFMP4Server
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class HttpFMP4Server : public HttpServer
{
public:
	HttpFMP4Server();
	virtual ~HttpFMP4Server();
    int HandleHttpReq(const char * i_strReq,char *o_strRes,int i_iResMaxLen);//return ResLen,<0 err
    int GetFMP4(char *o_strRes,int i_iResMaxLen);
private:
    int HandleReqGetFMP4(string *i_pPlaySrc,char *o_strRes,int i_iResMaxLen);
    int Regex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt);
    
    HttpFMP4ServerSession * m_pHttpFMP4ServerSession;
};













#endif
