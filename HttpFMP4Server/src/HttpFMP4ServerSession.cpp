/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsServerSession.cpp
* Description           : 	
* Created               :       2022.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HttpFlvServerSession.h"
#include "HttpFlvServerCom.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <functional>

//#include <sys/time.h>
//#include "cJSON.h"

#define HTTP_FLV_FRAME_BUF_MAX_LEN	(2*1024*1024) 

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
HttpFlvServerSession::HttpFlvServerSession(char * i_strPlaySrc,int i_iEnhancedFlag)
{
    m_pPlaySrc = new string(i_strPlaySrc);
    m_pMediaHandle = new MediaHandle();
    m_dwFileLastTimeStamp = 0;
    m_iEnhancedFlag=i_iEnhancedFlag;

    FLV_LOGW("HttpFlvServerSession start%d, %s\r\n",m_iEnhancedFlag,m_pPlaySrc->c_str());
    memset(&m_tFileFrameInfo,0,sizeof(T_MediaFrameInfo));
    m_tFileFrameInfo.pbFrameBuf = new unsigned char [HTTP_FLV_FRAME_BUF_MAX_LEN];
    m_tFileFrameInfo.iFrameBufMaxLen = HTTP_FLV_FRAME_BUF_MAX_LEN;
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
HttpFlvServerSession::~HttpFlvServerSession()
{
    delete m_pPlaySrc;
    delete m_pMediaHandle;
    if(NULL!= m_tFileFrameInfo.pbFrameBuf)
    {
        FLV_LOGW("~HttpFlvServerSession start exit\r\n");
        delete [] m_tFileFrameInfo.pbFrameBuf;
        m_tFileFrameInfo.pbFrameBuf = NULL;
    }
}
/*****************************************************************************
-Fuction        : GetFlv
-Description    : 如果是实时流，可以改为通过传参传入m_tFileFrameInfo
-Input          : 
-Output         : 
-Return         : //return ResLen,<0 err
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFlvServerSession::GetFlv(char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;
	unsigned int dwLastSegTimeStamp=0;
	int iSleepTimeMS=0;
    unsigned char * pbContainerBuf=NULL;
	int iContainerHeaderLen=0;
	int iDiffTimeMS=0;
	
    if(NULL == o_strRes)
    {
        FLV_LOGE("NULL == o_strRes %d[%s]\r\n",iRet,m_pPlaySrc->c_str());
        return iRet;
    }
    do
    {
        iRet=m_pMediaHandle->GetFrame(&m_tFileFrameInfo);//非文件流可直接调用此接口
        if(iRet<0)
        {
            FLV_LOGE("GetFrame err exit %d[%s]\r\n",iRet,m_pPlaySrc->c_str());
            break;
        }
        if(0==m_dwFileLastTimeStamp)
        {
            m_dwFileLastTimeStamp=m_tFileFrameInfo.dwTimeStamp;
            m_dwFileLastTick = GetTickCount();
        }
        if(m_tFileFrameInfo.dwTimeStamp<m_dwFileLastTimeStamp)
        {
            FLV_LOGE("dwTimeStamp err exit %d,%d\r\n",m_tFileFrameInfo.dwTimeStamp,m_dwFileLastTimeStamp);
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
            FLV_LOGD("MEDIA_FRAME_TYPE_VIDEO_I_FRAME %d,%d\r\n",m_tFileFrameInfo.dwTimeStamp,m_dwFileLastTimeStamp);
        }
        iContainerHeaderLen = 0;
        if(0 != m_iEnhancedFlag)
        {
            iRet=m_pMediaHandle->FrameToContainer(&m_tFileFrameInfo,STREAM_TYPE_FMP4_STREAM,(unsigned char *)o_strRes,i_iResMaxLen,&iContainerHeaderLen);
        }
        else
        {
            iRet=m_pMediaHandle->FrameToContainer(&m_tFileFrameInfo,STREAM_TYPE_FMP4_STREAM,(unsigned char *)o_strRes,i_iResMaxLen,&iContainerHeaderLen);
        }
        if(iRet<0 || i_iResMaxLen<iRet)
        {
            FLV_LOGE("FrameToContainer err exit %d,%d[%s]\r\n",iRet,i_iResMaxLen,m_pPlaySrc->c_str());
            iRet=-1;
            break;
        }
        if(0 == iRet)
        {
            FLV_LOGW("FrameToContainer need more data %d,%d[%s]\r\n",iRet,i_iResMaxLen,m_pPlaySrc->c_str());
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
unsigned int HttpFlvServerSession::GetTickCount()	// milliseconds
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

