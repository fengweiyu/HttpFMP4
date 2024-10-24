/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvServerInf.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FLV_SERVER_INF_H
#define HTTP_FLV_SERVER_INF_H




/*****************************************************************************
-Class          : HttpFlvServerInf
-Description    : HttpFlvServerInf
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class HttpFlvServerInf
{
public:
	HttpFlvServerInf();
	virtual ~HttpFlvServerInf();
    int HandleHttpReq(const char * i_strReq,char *o_strRes,int i_iResMaxLen);//return ResLen,<0 err
    int GetFLV(char *o_strRes,int i_iResMaxLen);
private:
    void * m_pHandle;
};










#endif
