/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvServer.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FLV_SERVER_H
#define HTTP_FLV_SERVER_H

#include "HttpServer.h"
#include "HttpFlvServerSession.h"


/*****************************************************************************
-Class          : HttpFlvServer
-Description    : HttpFlvServer
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class HttpFlvServer : public HttpServer
{
public:
	HttpFlvServer();
	virtual ~HttpFlvServer();
    int HandleHttpReq(const char * i_strReq,char *o_strRes,int i_iResMaxLen);//return ResLen,<0 err
    int GetFLV(char *o_strRes,int i_iResMaxLen);
private:
    int HandleReqGetFlv(int i_iEnhancedFlag,string *i_pPlaySrc,char *o_strRes,int i_iResMaxLen);
    int Regex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt);
    
    HttpFlvServerSession * m_pHttpFlvServerSession;
};













#endif
