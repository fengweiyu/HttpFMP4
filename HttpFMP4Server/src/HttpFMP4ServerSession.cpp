/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFMP4ServerSession.cpp
* Description           : 	
* Created               :       2022.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HttpFMP4ServerSession.h"
#include "HttpFMP4ServerCom.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <functional>

//#include <sys/time.h>
//#include "cJSON.h"

#define HTTP_FMP4_FRAME_BUF_MAX_LEN	(2*1024*1024) 

/*****************************************************************************
-Fuction        : HlsServerSession
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HttpFMP4ServerSession::HttpFMP4ServerSession(char * i_strPlaySrc)
{
    m_pPlaySrc = new string(i_strPlaySrc);
    m_pMediaHandle = new MediaHandle();
    m_dwFileLastTimeStamp = 0;

    FMP4_LOGW("HttpFMP4ServerSession start, %s\r\n",m_pPlaySrc->c_str());
    memset(&m_tFileFrameInfo,0,sizeof(T_MediaFrameInfo));
    m_tFileFrameInfo.pbFrameBuf = new unsigned char [HTTP_FMP4_FRAME_BUF_MAX_LEN];
    m_tFileFrameInfo.iFrameBufMaxLen = HTTP_FMP4_FRAME_BUF_MAX_LEN;
    m_pMediaHandle->Init(i_strPlaySrc);//默认取文件流
}
/*****************************************************************************
-Fuction        : ~HlsServer
-Description    : ~HlsServer
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HttpFMP4ServerSession::~HttpFMP4ServerSession()
{
    delete m_pPlaySrc;
    delete m_pMediaHandle;
    if(NULL!= m_tFileFrameInfo.pbFrameBuf)
    {
        FMP4_LOGW("~HttpFMP4ServerSession start exit\r\n");
        delete [] m_tFileFrameInfo.pbFrameBuf;
        m_tFileFrameInfo.pbFrameBuf = NULL;
    }
}
/*****************************************************************************
-Fuction        : GetFMP4
-Description    : 如果是实时流，可以改为通过传参传入m_tFileFrameInfo
-Input          : 
-Output         : 
-Return         : //return ResLen,<0 err
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFMP4ServerSession::GetFMP4(char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;
	unsigned int dwLastSegTimeStamp=0;
	int iSleepTimeMS=0;
    unsigned char * pbContainerBuf=NULL;
	int iContainerHeaderLen=0;
	int iDiffTimeMS=0;
	
    if(NULL == o_strRes)
    {
        FMP4_LOGE("NULL == o_strRes %d[%s]\r\n",iRet,m_pPlaySrc->c_str());
        return iRet;
    }
    do
    {
        iRet=m_pMediaHandle->GetFrame(&m_tFileFrameInfo);//非文件流可直接调用此接口
        if(iRet<0)
        {
            FMP4_LOGE("GetFrame err exit %d[%s]\r\n",iRet,m_pPlaySrc->c_str());
            break;
        }
        if(0==m_dwFileLastTimeStamp)
        {
            m_dwFileLastTimeStamp=m_tFileFrameInfo.dwTimeStamp;
            m_dwFileLastTick = GetTickCount();
        }
        if(m_tFileFrameInfo.dwTimeStamp<m_dwFileLastTimeStamp)
        {
            FMP4_LOGE("dwTimeStamp err exit %d,%d\r\n",m_tFileFrameInfo.dwTimeStamp,m_dwFileLastTimeStamp);
            break;
        }
        iSleepTimeMS=(int)(m_tFileFrameInfo.dwTimeStamp-m_dwFileLastTimeStamp);
        unsigned int dwFileCurTick=GetTickCount();
        iDiffTimeMS=(int)(dwFileCurTick-m_dwFileLastTick);
        if(iDiffTimeMS < iSleepTimeMS)
        {
            //SleepMs((iSleepTimeMS-iDiffTimeMS));//模拟实时流(直播)，点播和当前的处理机制不匹配，需要后续再开发
        }
        m_dwFileLastTick = GetTickCount();
        if(MEDIA_FRAME_TYPE_VIDEO_I_FRAME== m_tFileFrameInfo.eFrameType)
        {
            FMP4_LOGD("MEDIA_FRAME_TYPE_VIDEO_I_FRAME %d,%d\r\n",m_tFileFrameInfo.dwTimeStamp,m_dwFileLastTimeStamp);
        }
        iContainerHeaderLen = 0;
        iRet=m_pMediaHandle->FrameToContainer(&m_tFileFrameInfo,STREAM_TYPE_FMP4_STREAM,(unsigned char *)o_strRes,i_iResMaxLen,&iContainerHeaderLen);
        if(iRet<0 || i_iResMaxLen<iRet)
        {
            FMP4_LOGE("FrameToContainer err exit %d,%d[%s]\r\n",iRet,i_iResMaxLen,m_pPlaySrc->c_str());
            iRet=-1;
            break;
        }
        if(0 == iRet)
        {
            FMP4_LOGW("FrameToContainer need more data %d,%d[%s]\r\n",iRet,i_iResMaxLen,m_pPlaySrc->c_str());
            continue;
        }
        m_dwFileLastTimeStamp = m_tFileFrameInfo.dwTimeStamp;
        break;
    }while(1);
    return iRet;
}
/*****************************************************************************
-Fuction        : SendErrorCode
-Description    : // 返回自系统开机以来的毫秒数
-Input          : 
-Output         : 
-Return         : len
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
unsigned int HttpFMP4ServerSession::GetTickCount()	// milliseconds
{
#ifdef _WIN32
	return ::GetTickCount64() & 0xFFFFFFFF;
#else  
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint64_t cnt = ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
	return cnt & 0xFFFFFFFF;
#endif
}

