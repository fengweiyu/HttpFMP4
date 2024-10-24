/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFMP4ServerIO.c
* Description           : 	
* Created               :       2023.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HttpFMP4ServerIO.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>
#include <thread>

using std::thread;

#define HTTP_FMP4_IO_RECV_MAX_LEN (10240)
#define HTTP_FMP4_IO_SEND_MAX_LEN (3*1024*1024)

/*****************************************************************************
-Fuction		: HttpFMP4ServerIO
-Description	: HttpFMP4ServerIO
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HttpFMP4ServerIO :: HttpFMP4ServerIO(int i_iClientSocketFd)
{
    m_iClientSocketFd=i_iClientSocketFd;
    
    m_iHttpFMP4ServerIOFlag = 0;
    m_pHttpFMP4ServerIOProc = new thread(&HttpFMP4ServerIO::Proc, this);
    //m_pHttpSessionProc->detach();//注意线程回收
}

/*****************************************************************************
-Fuction		: ~HttpFMP4ServerIO
-Description	: ~HttpFMP4ServerIO
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HttpFMP4ServerIO :: ~HttpFMP4ServerIO()
{
    if(NULL!= m_pHttpFMP4ServerIOProc)
    {
        FMP4_LOGW("~HttpFMP4ServerIO start exit\r\n");
        m_iHttpFMP4ServerIOFlag = 0;
        //while(0 == m_iExitProcFlag){usleep(10);};
        m_pHttpFMP4ServerIOProc->join();//
        delete m_pHttpFMP4ServerIOProc;
        m_pHttpFMP4ServerIOProc = NULL;
    }
    FMP4_LOGW("~~HttpFMP4ServerIO exit\r\n");
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
int HttpFMP4ServerIO :: Proc()
{
    int iRet=-1;
    char *pcRecvBuf=NULL;
    char *pcSendBuf=NULL;
    int iRecvLen=-1;
    timeval tTimeValue;
    
    if(m_iClientSocketFd < 0)
    {
        FMP4_LOGE("HttpFMP4ServerIO m_iClientSocketFd < 0 err\r\n");
        return -1;
    }
    pcRecvBuf = new char[HTTP_FMP4_IO_RECV_MAX_LEN];
    if(NULL == pcRecvBuf)
    {
        FMP4_LOGE("HttpFMP4ServerIO NULL == pcRecvBuf err\r\n");
        return -1;
    }
    pcSendBuf = new char[HTTP_FMP4_IO_SEND_MAX_LEN];
    if(NULL == pcSendBuf)
    {
        FMP4_LOGE("HttpFMP4ServerIO NULL == pcSendBuf err\r\n");
        delete[] pcRecvBuf;
        return -1;
    }
    m_iHttpFMP4ServerIOFlag = 1;
    FMP4_LOGW("HlsServerIO start Proc\r\n");
    while(m_iHttpFMP4ServerIOFlag)
    {
        iRecvLen = 0;
        memset(pcRecvBuf,0,HTTP_FMP4_IO_RECV_MAX_LEN);
        milliseconds timeMS(10);// 表示10毫秒
        iRet=TcpServer::Recv(pcRecvBuf,&iRecvLen,HTTP_FMP4_IO_RECV_MAX_LEN,m_iClientSocketFd,&timeMS);
        if(iRet < 0)
        {
            FMP4_LOGE("TcpServer::Recv err exit %d\r\n",iRecvLen);
            break;
        }
        if(iRecvLen<=0)
        {
            iRet=m_HttpFMP4ServerInf.GetFMP4(pcSendBuf,HTTP_FMP4_IO_SEND_MAX_LEN);
            if(iRet > 0)
            {
                TcpServer::Send(pcSendBuf,iRet,m_iClientSocketFd);
            }
            else if(iRet < 0)
            {
                FMP4_LOGE("m_HttpFMP4ServerInf.GetFMP4 err exit %d\r\n",iRecvLen);
                break;
            }
            continue;
        }
        memset(pcSendBuf,0,HTTP_FMP4_IO_SEND_MAX_LEN);
        iRet=m_HttpFMP4ServerInf.HandleHttpReq(pcRecvBuf,pcSendBuf,HTTP_FMP4_IO_SEND_MAX_LEN);
        if(iRet > 0)
        {
            TcpServer::Send(pcSendBuf,iRet,m_iClientSocketFd);
        }
    }
    
    if(m_iClientSocketFd>=0)
    {
        TcpServer::Close(m_iClientSocketFd);//主动退出,
        FMP4_LOGW("HttpFMP4ServerIO::Close m_iClientSocketFd Exit%d\r\n",m_iClientSocketFd);
    }
    if(NULL != pcSendBuf)
    {
        delete[] pcSendBuf;
    }
    if(NULL != pcRecvBuf)
    {
        delete[] pcRecvBuf;
    }
    
    m_iHttpFMP4ServerIOFlag=0;
    return 0;
}

/*****************************************************************************
-Fuction		: GetProcFlag
-Description	: HttpFMP4ServerIO
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HttpFMP4ServerIO :: GetProcFlag()
{
    return m_iHttpFMP4ServerIOFlag;//多线程竞争注意优化
}

