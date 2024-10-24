/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFMP4ServerInf.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FMP4_SERVER_INF_H
#define HTTP_FMP4_SERVER_INF_H




/*****************************************************************************
-Class          : HttpFMP4ServerInf
-Description    : HttpFMP4ServerInf
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class HttpFMP4ServerInf
{
public:
	HttpFMP4ServerInf();
	virtual ~HttpFMP4ServerInf();
    int HandleHttpReq(const char * i_strReq,char *o_strRes,int i_iResMaxLen);//return ResLen,<0 err
    int GetFMP4(char *o_strRes,int i_iResMaxLen);
private:
    void * m_pHandle;
};










#endif
