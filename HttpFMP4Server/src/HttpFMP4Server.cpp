/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvServer.cpp
* Description           : 	    Ϊ��Ӧ�Զ����ӣ�server��Ƴɳ�פ����
ע�⣬��Ա����Ҫ�����߳̾�������
* Created               :       2022.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HttpFlvServer.h"
#include "HttpFlvServerCom.h"
#include <regex>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "cJSON.h"

using std::string;
using std::smatch;
using std::regex;


#define HTTP_FLV_CLIENT_VLC         "VLC"
#define HTTP_FLV_CLIENT_FFPLAY      "Lavf"
#define HTTP_FLV_CLIENT_CHROME      "Chrome"

#define HTTP_FLV_MAX_MATCH_NUM       8



#define HTTP_FLV_UN_SUPPORT_FUN (-2)
#ifndef _WIN32
#include <regex.h> //C++ regexҪ��gcc 4.9���ϰ汾������linux������c��
/*****************************************************************************
-Fuction		: Regex
-Description	: ������ʽ
.��				ƥ�����\r\n��֮����κε����ַ�
*				ƥ��ǰ����ӱ��ʽ����Ρ����磬zo*��ƥ�䡰z����Ҳ��ƥ�䡰zo���Լ���zoo����*�ȼ���o{0,}
				����.*��ƥ��������洢�����������
(pattern)		ƥ��ģʽ��pattern����ȡ��һƥ�䡣����ȡ��ƥ����ԴӲ�����Matches���ϵõ�
[xyz]			�ַ����ϡ�ƥ��������������һ���ַ������磬��[abc]������ƥ�䡰plain���еġ�a����
+				ƥ��ǰ����ӱ��ʽһ�λ���(���ڵ���1�Σ������磬��zo+����ƥ�䡰zo���Լ���zoo����������ƥ�䡰z����+�ȼ���{1,}��
				//���������в���+��Ĭ����һ�Σ���ֻ��ƥ�䵽һ������6
				
[A-Za-z0-9] 	26����д��ĸ��26��Сд��ĸ��0��9����
[A-Za-z0-9+/=]	26����д��ĸ��26��Сд��ĸ0��9�����Լ�+/= �����ַ�


-Input			: i_strPattern ģʽ��,i_strBuf��ƥ���ַ���,
-Output 		: o_ptMatch �洢ƥ�䴮λ�õ�����,���ڴ洢ƥ�����ڴ�ƥ�䴮�е��±귶Χ
//����0��Ԫ�����������ʽƥ������λ��,�������������������ƥ��������ߵĵ�Ԫ���δ����������ʽƥ������λ��
-Return 		: -1 err,other cnt
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/11/01	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int CRegex(const char *i_strPattern,char *i_strBuf,regmatch_t *o_ptMatch,int i_iMatchMaxNum)
{
    char acErrBuf[256];
    int iRet=-1;
    regex_t tReg;    //����һ������ʵ��
    //const size_t dwMatch = 6;    //����ƥ�������������       //��ʾ������ƥ��


    //REG_ICASE ƥ����ĸʱ���Դ�Сд��
    iRet =regcomp(&tReg, i_strPattern, REG_EXTENDED);    //��������ģʽ��
    if(iRet != 0) 
    {
        regerror(iRet, &tReg, acErrBuf, sizeof(acErrBuf));
        FLV_LOGE("Regex Error:\r\n");
        return -1;
    }
    
    iRet = regexec(&tReg, i_strBuf, i_iMatchMaxNum, o_ptMatch, 0); //ƥ����
    if (iRet == REG_NOMATCH)
    { //���ûƥ����
        FLV_LOGE("Regex No Match!\r\n");
        iRet = 0;
    }
    else if (iRet == REG_NOERROR)
    { //���ƥ������
        FLV_LOGD("Match\r\n");
        int i=0,j=0;
        for(i=0;i<i_iMatchMaxNum && o_ptMatch[i].rm_so != -1;i++)
        {
            for (j= o_ptMatch[i].rm_so; j < o_ptMatch[i].rm_eo;j++)
            { //�������ƥ�䷶Χ���ַ���
                //printf("%c", i_strBuf[j]);
            }
            //printf("\n");
        }
        iRet = i;
    }
    else
    {
        FLV_LOGE("Regex Unknow err%d:\r\n",iRet);
        iRet = -1;
    }
    regfree(&tReg);  //�ͷ�������ʽ
    
    return iRet;
}
/*****************************************************************************
-Fuction		: HttpParseReqHeader
-Description	: Send
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int LinuxCRegex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt)
{
    int iRet = -1;
	string strBuf;
	regmatch_t atMatch[HTTP_FLV_MAX_MATCH_NUM];
	
    if(NULL == i_strPattern || NULL == i_strBuf || NULL == o_aMatch || i_iMatchMaxCnt <= 0 || i_iMatchMaxCnt <= 0)
    {
        FLV_LOGE("LinuxCRegex NULL \r\n");
        return iRet;
    }
    strBuf.assign(i_strBuf);
    
    memset(atMatch,0,sizeof(atMatch));
    iRet= CRegex(i_strPattern,i_strBuf,atMatch,HTTP_FLV_MAX_MATCH_NUM);
    if(iRet <= 0 || i_iMatchMaxCnt < iRet)//ȥ������������-1
    {
        FLV_LOGE("LinuxCRegex %d,%d, iRet <= 0 || i_iMatchMaxCnt < iRet err \r\n",i_iMatchMaxCnt,iRet);
        return iRet;
    }
    // �������ƥ�䵽���Ӵ�
    for (int i = 0; i < iRet; ++i) 
    {
        o_aMatch[i].assign(strBuf,atMatch[i].rm_so,atMatch[i].rm_eo-atMatch[i].rm_so);//0������
    }
    return iRet;
} 
#else
/*****************************************************************************
-Fuction		: HttpParseReqHeader
-Description	: Send
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int LinuxCRegex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt)
{
    return HTTP_FLV_UN_SUPPORT_FUN;
} 
#endif







#define HTTP_FLV_MAX_LEN       (1*1024*1024)

/*****************************************************************************
-Fuction        : HlsServer
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HttpFlvServer::HttpFlvServer()
{
    m_pHttpFlvServerSession=NULL;
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
HttpFlvServer::~HttpFlvServer()
{
    if (NULL != m_pHttpFlvServerSession)
    {
        delete m_pHttpFlvServerSession;
        m_pHttpFlvServerSession=NULL;
    }
}
/*****************************************************************************
-Fuction        : HandleHttpReq
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFlvServer::HandleHttpReq(const char * i_strReq,char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;
    T_HttpReqPacket tHttpReqPacket;

    if(NULL == i_strReq || NULL == o_strRes|| i_iResMaxLen <= 0)
    {
        FLV_LOGW("HandleHttpReq NULL \r\n");
        return iRet;
    }
    memset(&tHttpReqPacket,0,sizeof(T_HttpReqPacket));
    iRet=HttpServer::ParseRequest((char *)i_strReq,strlen(i_strReq),&tHttpReqPacket);
    if(iRet < 0)
    {
        FLV_LOGE("HttpServer::ParseRequest err%d\r\n",iRet);
        return iRet;
    }
    if(0 == strcmp(tHttpReqPacket.strMethod,HTTP_METHOD_OPTIONS))
    {
        FLV_LOGW("HandleHttpReq HTTP_METHOD_OPTIONS\r\n");
        HttpServer *pHttpServer=new HttpServer();
        iRet=pHttpServer->CreateResponse();
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Method", "POST, GET, OPTIONS, DELETE, PUT");
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Max-Age", "600");
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Headers", "access-control-allow-headers,accessol-allow-origin,content-type");//����������������
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Origin","*");
        iRet|=pHttpServer->SetResHeaderValue("Connection", "Keep-Alive");
        iRet=pHttpServer->FormatResToStream(NULL,0,o_strRes,i_iResMaxLen);
        delete pHttpServer;
        return iRet;
    }
    if(0 == strcmp(tHttpReqPacket.strMethod,HTTP_METHOD_GET))
    {
        string astrRegex[HTTP_FLV_MAX_MATCH_NUM];
        const char * strPattern = "/([A-Za-z_]+)/([A-Za-z0-9_]+).mp4";//http://localhost:9214/file/H264AAC.flv
        iRet=this->Regex(strPattern,tHttpReqPacket.strURL,astrRegex,HTTP_FLV_MAX_MATCH_NUM);//http://localhost:9214/file_enhanced/H265AAC.flv
        if (iRet>2) //0������
        {
            int iEnhancedFlag=0;
            string strStreamType(astrRegex[1].c_str());//file
            string strFileName(astrRegex[2].c_str());
            strFileName.append(".flv");
            FLV_LOGW("%d,file m_pFileName %s\r\n",iRet,strFileName.c_str());
            if(0 != strlen(tHttpReqPacket.strUserAgent)&&(NULL != strstr(tHttpReqPacket.strUserAgent,HTTP_FLV_CLIENT_VLC) 
            ||NULL != strstr(tHttpReqPacket.strUserAgent,HTTP_FLV_CLIENT_FFPLAY) ||NULL != strstr(tHttpReqPacket.strUserAgent,HTTP_FLV_CLIENT_CHROME)))
            {
                FLV_LOGW("iEnhancedFlag tHttpReqPacket.strUserAgent %s\r\n",tHttpReqPacket.strUserAgent);
                iEnhancedFlag=1;
            }
            if(string::npos!= strStreamType.find("_enhanced"))
            {
                FLV_LOGW("find EnhancedFlag tHttpReqPacket.strUserAgent %s\r\n",tHttpReqPacket.strUserAgent);
                iEnhancedFlag=1;
            }
            iRet = HandleReqGetFlv(iEnhancedFlag,&strFileName,o_strRes,i_iResMaxLen);
            return iRet;
        } 
    }
    FLV_LOGE("unsupport HTTP_METHOD_ %s,url %s\r\n",tHttpReqPacket.strMethod,tHttpReqPacket.strURL);

    HttpServer *pHttpServer=new HttpServer();
    iRet=pHttpServer->CreateResponse(400,"bad request");
    iRet|=pHttpServer->SetResHeaderValue("Connection", "Keep-Alive");
    iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Origin", "*");
    iRet=pHttpServer->FormatResToStream(NULL,0,o_strRes,i_iResMaxLen);
    delete pHttpServer;
    
    return iRet;
}
/*****************************************************************************
-Fuction        : HandleReqGetFlv
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFlvServer::HandleReqGetFlv(int i_iEnhancedFlag,string *i_pPlaySrc,char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;
    char *pcFlv = NULL;
    int iFlvLen = -1;

    if(NULL == i_pPlaySrc || NULL == o_strRes|| i_iResMaxLen <= 0)
    {
        FLV_LOGE("HandleReqGetM3U8 NULL \r\n");
        return iRet;
    }
    
    if(NULL == m_pHttpFlvServerSession)
    {
        m_pHttpFlvServerSession = new HttpFlvServerSession((char *)i_pPlaySrc->c_str(),i_iEnhancedFlag);
    }
    pcFlv = new char [HTTP_FLV_MAX_LEN];//�����Ż�
    iFlvLen = m_pHttpFlvServerSession->GetFlv(pcFlv,HTTP_FLV_MAX_LEN);
    if(iFlvLen <= 0)
    {
        FLV_LOGE("HandleReq GetFlv err \r\n");
        HttpServer *pHttpServer=new HttpServer();
        iRet=pHttpServer->CreateResponse(400,"bad request");
        iRet|=pHttpServer->SetResHeaderValue("Connection", "Keep-Alive");
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Origin", "*");
        iRet=pHttpServer->FormatResToStream(NULL,0,o_strRes,i_iResMaxLen);
        delete pHttpServer;
        return iRet;
    }
    HttpServer *pHttpServer=new HttpServer();
    iRet=pHttpServer->CreateResponse();
    iRet|=pHttpServer->SetResHeaderValue("Connection", "Keep-Alive");
    iRet|=pHttpServer->SetResHeaderValue("Content-Type", "video/mp4");//x-flv
    iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Origin", "*");
    iRet=pHttpServer->FormatResToStream(pcFlv,iFlvLen,o_strRes,i_iResMaxLen);
    delete pHttpServer;
    delete [] pcFlv;
    return iRet;
}
/*****************************************************************************
-Fuction        : GetFLV
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFlvServer::GetFLV(char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;

    if(NULL == o_strRes|| i_iResMaxLen <= 0)
    {
        FLV_LOGE("GetFLV NULL%p \r\n",m_pHttpFlvServerSession);
        return iRet;
    }
    if(NULL == m_pHttpFlvServerSession)
    {
        //FLV_LOGE("NULL == m_pHttpFlvServerSession %p \r\n",m_pHttpFlvServerSession);
        return 0;
    }
    return m_pHttpFlvServerSession->GetFlv(o_strRes,i_iResMaxLen);
}

/*****************************************************************************
-Fuction        : HlsRegex
-Description    : 
-Input          : 
-Output         : 
-Return         : -1 err,>0 cnt
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int HttpFlvServer::Regex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt)
{
    int iRet = -1;

    if(NULL == i_strPattern || NULL == i_strBuf || NULL == o_aMatch || i_iMatchMaxCnt <= 0)
    {
        FLV_LOGE("HlsRegex NULL \r\n");
        return iRet;
    }
    iRet = LinuxCRegex(i_strPattern,i_strBuf,o_aMatch,i_iMatchMaxCnt);
    if(HTTP_FLV_UN_SUPPORT_FUN!=iRet)
    {
        return iRet;
    }
    string strBuf(i_strBuf);
    regex Pattern(i_strPattern);//http://localhost:9212/file/H264AAC.flv/test.m3u8
    smatch Match;
    if (std::regex_search(strBuf,Match,Pattern)) 
    {
        if((int)Match.size()<=0 || i_iMatchMaxCnt < (int)Match.size())
        {
            FLV_LOGE("HlsRegex err i_iMatchMaxCnt %d<%d Match.size()-1 \r\n",i_iMatchMaxCnt,Match.size()-1);
            return iRet;
        }
        iRet = (int)Match.size();
        // �������ƥ�䵽���Ӵ�
        for (int i = 0; i < iRet; ++i) 
        {
            o_aMatch[i].assign( Match[i].str());
        }
    } 
    return iRet;
}


