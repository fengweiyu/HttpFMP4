/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvServerIO.c
* Description           : 	
* Created               :       2023.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HttpFlvServerIO.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>
#include <thread>

using std::thread;

#define HTTP_FLV_IO_RECV_MAX_LEN (10240)
#define HTTP_FLV_IO_SEND_MAX_LEN (3*1024*1024)

/*****************************************************************************
-Fuction		: HttpFlvServerIO
-Description	: HttpFlvServerIO
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HttpFlvServerIO :: HttpFlvServerIO(int i_iClientSocketFd)
{
    m_iClientSocketFd=i_iClientSocketFd;
    
    m_iHttpFlvServerIOFlag = 0;
    m_pHttpFlvServerIOProc = new thread(&HttpFlvServerIO::Proc, this);
    //m_pHttpSessionProc->detach();//注意线程回收
}

/*****************************************************************************
-Fuction		: ~HttpFlvServerIO
-Description	: ~HttpFlvServerIO
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HttpFlvServerIO :: ~HttpFlvServerIO()
{
    if(NULL!= m_pHttpFlvServerIOProc)
    {
        FLV_LOGW("~HttpFlvServerIO start exit\r\n");
        m_iHttpFlvServerIOFlag = 0;
        //while(0 == m_iExitProcFlag){usleep(10);};
        m_pHttpFlvServerIOProc->join();//
        delete m_pHttpFlvServerIOProc;
        m_pHttpFlvServerIOProc = NULL;
    }
    FLV_LOGW("~~HttpFlvServerIO exit\r\n");
}

/*****************************************************************************
-Fuction		: Proc
-Description	: 阻塞
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HttpFlvServerIO :: Proc()
{
    int iRet=-1;
    char *pcRecvBuf=NULL;
    char *pcSendBuf=NULL;
    int iRecvLen=-1;
    timeval tTimeValue;
    
    if(m_iClientSocketFd < 0)
    {
        FLV_LOGE("HttpFlvServerIO m_iClientSocketFd < 0 err\r\n");
        return -1;
    }
    pcRecvBuf = new char[HTTP_FLV_IO_RECV_MAX_LEN];
    if(NULL == pcRecvBuf)
    {
        FLV_LOGE("HttpFlvServerIO NULL == pcRecvBuf err\r\n");
        return -1;
    }
    pcSendBuf = new char[HTTP_FLV_IO_SEND_MAX_LEN];
    if(NULL == pcSendBuf)
    {
        FLV_LOGE("HttpFlvServerIO NULL == pcSendBuf err\r\n");
        delete[] pcRecvBuf;
        return -1;
    }
    m_iHttpFlvServerIOFlag = 1;
    FLV_LOGW("HlsServerIO start Proc\r\n");
    while(m_iHttpFlvServerIOFlag)
    {
        iRecvLen = 0;
        memset(pcRecvBuf,0,HTTP_FLV_IO_RECV_MAX_LEN);
        milliseconds timeMS(10);// 表示10毫秒
        iRet=TcpServer::Recv(pcRecvBuf,&iRecvLen,HTTP_FLV_IO_RECV_MAX_LEN,m_iClientSocketFd,&timeMS);
        if(iRet < 0)
        {
            FLV_LOGE("TcpServer::Recv err exit %d\r\n",iRecvLen);
            break;
        }
        if(iRecvLen<=0)
        {
            iRet=m_HttpFlvServerInf.GetFLV(pcSendBuf,HTTP_FLV_IO_SEND_MAX_LEN);
            if(iRet > 0)
            {
                TcpServer::Send(pcSendBuf,iRet,m_iClientSocketFd);
            }
            else if(iRet < 0)
            {
                FLV_LOGE("m_HttpFlvServerInf.GetFLV err exit %d\r\n",iRecvLen);
                break;
            }
            continue;
        }
        memset(pcSendBuf,0,HTTP_FLV_IO_SEND_MAX_LEN);
        iRet=m_HttpFlvServerInf.HandleHttpReq(pcRecvBuf,pcSendBuf,HTTP_FLV_IO_SEND_MAX_LEN);
        if(iRet > 0)
        {
            TcpServer::Send(pcSendBuf,iRet,m_iClientSocketFd);
        }
    }
    
    if(m_iClientSocketFd>=0)
    {
        TcpServer::Close(m_iClientSocketFd);//主动退出,
        FLV_LOGW("HttpFlvServerIO::Close m_iClientSocketFd Exit%d\r\n",m_iClientSocketFd);
    }
    if(NULL != pcSendBuf)
    {
        delete[] pcSendBuf;
    }
    if(NULL != pcRecvBuf)
    {
        delete[] pcRecvBuf;
    }
    
    m_iHttpFlvServerIOFlag=0;
    return 0;
}

/*****************************************************************************
-Fuction		: GetProcFlag
-Description	: HttpFlvServerIO
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HttpFlvServerIO :: GetProcFlag()
{
    return m_iHttpFlvServerIOFlag;//多线程竞争注意优化
}

