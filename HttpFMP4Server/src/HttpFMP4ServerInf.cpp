/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFMP4ServerInf.cpp
* Description           : 	    接口层，防止曝露内部文件
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HttpFMP4ServerInf.h"
#include "HttpFMP4Server.h"

/*****************************************************************************
-Fuction        : HttpFMP4ServerInf
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HttpFMP4ServerInf::HttpFMP4ServerInf()
{
    m_pHandle = NULL;
    m_pHandle = new HttpFMP4Server();
}
/*****************************************************************************
-Fuction        : ~WebRtcInterface
-Description    : ~WebRtcInterface
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HttpFMP4ServerInf::~HttpFMP4ServerInf()
{
    if(NULL != m_pHandle)
    {
        HttpFMP4Server *pHlsServer = (HttpFMP4Server *)m_pHandle;
        delete pHlsServer;
    }  
}

/*****************************************************************************
-Fuction        : Proc
-Description    : Proc
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFMP4ServerInf::HandleHttpReq(const char * i_strReq,char *o_strRes,int i_iResMaxLen)
{
    HttpFMP4Server *pServer = (HttpFMP4Server *)m_pHandle;
    return pServer->HandleHttpReq(i_strReq,o_strRes,i_iResMaxLen);
}
/*****************************************************************************
-Fuction        : Proc
-Description    : Proc
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFMP4ServerInf::GetFMP4(char *o_strRes,int i_iResMaxLen)
{
    HttpFMP4Server *pServer = (HttpFMP4Server *)m_pHandle;
    return pServer->GetFMP4(o_strRes,i_iResMaxLen);
}

