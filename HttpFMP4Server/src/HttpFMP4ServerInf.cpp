/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvServerInf.cpp
* Description           : 	    接口层，防止曝露内部文件
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HttpFlvServerInf.h"
#include "HttpFlvServer.h"

/*****************************************************************************
-Fuction        : HttpFlvServerInf
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HttpFlvServerInf::HttpFlvServerInf()
{
    m_pHandle = NULL;
    m_pHandle = new HttpFlvServer();
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
HttpFlvServerInf::~HttpFlvServerInf()
{
    if(NULL != m_pHandle)
    {
        HttpFlvServer *pHlsServer = (HttpFlvServer *)m_pHandle;
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
int HttpFlvServerInf::HandleHttpReq(const char * i_strReq,char *o_strRes,int i_iResMaxLen)
{
    HttpFlvServer *pServer = (HttpFlvServer *)m_pHandle;
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
int HttpFlvServerInf::GetFLV(char *o_strRes,int i_iResMaxLen)
{
    HttpFlvServer *pServer = (HttpFlvServer *)m_pHandle;
    return pServer->GetFLV(o_strRes,i_iResMaxLen);
}

