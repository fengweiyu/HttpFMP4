/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFMP4Server.cpp
* Description           : 	    为了应对短连接，server设计成常驻对象
注意，成员变量要做多线程竞争保护
* Created               :       2022.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HttpFMP4Server.h"
#include "HttpFMP4ServerCom.h"
#include <regex>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "cJSON.h"

using std::string;
using std::smatch;
using std::regex;


#define HTTP_FMP4_CLIENT_VLC         "VLC"
#define HTTP_FMP4_CLIENT_FFPLAY      "Lavf"
#define HTTP_FMP4_CLIENT_CHROME      "Chrome"

#define HTTP_FMP4_MAX_MATCH_NUM       8



#define HTTP_FMP4_UN_SUPPORT_FUN (-2)
#ifndef _WIN32
#include <regex.h> //C++ regex要求gcc 4.9以上版本，所以linux还是用c的
/*****************************************************************************
-Fuction		: Regex
-Description	: 正则表达式
.点				匹配除“\r\n”之外的任何单个字符
*				匹配前面的子表达式任意次。例如，zo*能匹配“z”，也能匹配“zo”以及“zoo”。*等价于o{0,}
				其中.*的匹配结果不会存储到结果数组里
(pattern)		匹配模式串pattern并获取这一匹配。所获取的匹配可以从产生的Matches集合得到
[xyz]			字符集合。匹配所包含的任意一个字符。例如，“[abc]”可以匹配“plain”中的“a”。
+				匹配前面的子表达式一次或多次(大于等于1次）。例如，“zo+”能匹配“zo”以及“zoo”，但不能匹配“z”。+等价于{1,}。
				//如下例子中不用+，默认是一次，即只能匹配到一个数字6
				
[A-Za-z0-9] 	26个大写字母、26个小写字母和0至9数字
[A-Za-z0-9+/=]	26个大写字母、26个小写字母0至9数字以及+/= 三个字符


-Input			: i_strPattern 模式串,i_strBuf待匹配字符串,
-Output 		: o_ptMatch 存储匹配串位置的数组,用于存储匹配结果在待匹配串中的下标范围
//数组0单元存放主正则表达式匹配结果的位置,即所有正则组合起来的匹配结果，后边的单元依次存放子正则表达式匹配结果的位置
-Return 		: -1 err,other cnt
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/11/01	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int CRegex(const char *i_strPattern,char *i_strBuf,regmatch_t *o_ptMatch,int i_iMatchMaxNum)
{
    char acErrBuf[256];
    int iRet=-1;
    regex_t tReg;    //定义一个正则实例
    //const size_t dwMatch = 6;    //定义匹配结果最大允许数       //表示允许几个匹配


    //REG_ICASE 匹配字母时忽略大小写。
    iRet =regcomp(&tReg, i_strPattern, REG_EXTENDED);    //编译正则模式串
    if(iRet != 0) 
    {
        regerror(iRet, &tReg, acErrBuf, sizeof(acErrBuf));
        FMP4_LOGE("Regex Error:\r\n");
        return -1;
    }
    
    iRet = regexec(&tReg, i_strBuf, i_iMatchMaxNum, o_ptMatch, 0); //匹配他
    if (iRet == REG_NOMATCH)
    { //如果没匹配上
        FMP4_LOGE("Regex No Match!\r\n");
        iRet = 0;
    }
    else if (iRet == REG_NOERROR)
    { //如果匹配上了
        FMP4_LOGD("Match\r\n");
        int i=0,j=0;
        for(i=0;i<i_iMatchMaxNum && o_ptMatch[i].rm_so != -1;i++)
        {
            for (j= o_ptMatch[i].rm_so; j < o_ptMatch[i].rm_eo;j++)
            { //遍历输出匹配范围的字符串
                //printf("%c", i_strBuf[j]);
            }
            //printf("\n");
        }
        iRet = i;
    }
    else
    {
        FMP4_LOGE("Regex Unknow err%d:\r\n",iRet);
        iRet = -1;
    }
    regfree(&tReg);  //释放正则表达式
    
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
	regmatch_t atMatch[HTTP_FMP4_MAX_MATCH_NUM];
	
    if(NULL == i_strPattern || NULL == i_strBuf || NULL == o_aMatch || i_iMatchMaxCnt <= 0 || i_iMatchMaxCnt <= 0)
    {
        FMP4_LOGE("LinuxCRegex NULL \r\n");
        return iRet;
    }
    strBuf.assign(i_strBuf);
    
    memset(atMatch,0,sizeof(atMatch));
    iRet= CRegex(i_strPattern,i_strBuf,atMatch,HTTP_FMP4_MAX_MATCH_NUM);
    if(iRet <= 0 || i_iMatchMaxCnt < iRet)//去掉整串，所以-1
    {
        FMP4_LOGE("LinuxCRegex %d,%d, iRet <= 0 || i_iMatchMaxCnt < iRet err \r\n",i_iMatchMaxCnt,iRet);
        return iRet;
    }
    // 输出所有匹配到的子串
    for (int i = 0; i < iRet; ++i) 
    {
        o_aMatch[i].assign(strBuf,atMatch[i].rm_so,atMatch[i].rm_eo-atMatch[i].rm_so);//0是整行
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
    return HTTP_FMP4_UN_SUPPORT_FUN;
} 
#endif







#define HTTP_FMP4_MAX_LEN       (1*1024*1024)

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
HttpFMP4Server::HttpFMP4Server()
{
    m_pHttpFMP4ServerSession=NULL;
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
HttpFMP4Server::~HttpFMP4Server()
{
    if (NULL != m_pHttpFMP4ServerSession)
    {
        delete m_pHttpFMP4ServerSession;
        m_pHttpFMP4ServerSession=NULL;
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
int HttpFMP4Server::HandleHttpReq(const char * i_strReq,char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;
    T_HttpReqPacket tHttpReqPacket;

    if(NULL == i_strReq || NULL == o_strRes|| i_iResMaxLen <= 0)
    {
        FMP4_LOGW("HandleHttpReq NULL \r\n");
        return iRet;
    }
    memset(&tHttpReqPacket,0,sizeof(T_HttpReqPacket));
    iRet=HttpServer::ParseRequest((char *)i_strReq,strlen(i_strReq),&tHttpReqPacket);
    if(iRet < 0)
    {
        FMP4_LOGE("HttpServer::ParseRequest err%d\r\n",iRet);
        return iRet;
    }
    if(0 == strcmp(tHttpReqPacket.strMethod,HTTP_METHOD_OPTIONS))
    {
        FMP4_LOGW("HandleHttpReq HTTP_METHOD_OPTIONS\r\n");
        HttpServer *pHttpServer=new HttpServer();
        iRet=pHttpServer->CreateResponse();
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Method", "POST, GET, OPTIONS, DELETE, PUT");
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Max-Age", "600");
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Headers", "access-control-allow-headers,accessol-allow-origin,content-type");//解决浏览器跨域问题
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Origin","*");
        iRet|=pHttpServer->SetResHeaderValue("Connection", "Keep-Alive");
        iRet=pHttpServer->FormatResToStream(NULL,0,o_strRes,i_iResMaxLen);
        delete pHttpServer;
        return iRet;
    }
    if(0 == strcmp(tHttpReqPacket.strMethod,HTTP_METHOD_GET))
    {
        string astrRegex[HTTP_FMP4_MAX_MATCH_NUM];
        const char * strPattern = "/([A-Za-z_]+)/([A-Za-z0-9_.]+)/([A-Za-z0-9_.]+).mp4";//http://localhost:9214/file/H264AAC.flv/test.mp4
        iRet=this->Regex(strPattern,tHttpReqPacket.strURL,astrRegex,HTTP_FMP4_MAX_MATCH_NUM);//http://localhost:9208/file/H264AAC.flv/test.mp4
        if (iRet>3) //0是整行
        {
            string strStreamType(astrRegex[1].c_str());//file
            string strFileName(astrRegex[2].c_str());
            string strStreamArgs(astrRegex[3].c_str());
            FMP4_LOGW("%d,strStreamType %s,m_pFileName %s,strStreamArgs %s\r\n",iRet,strStreamType.c_str(),strFileName.c_str(),strStreamArgs.c_str());
            iRet = HandleReqGetFMP4(&strFileName,o_strRes,i_iResMaxLen);
            return iRet;
        } 
    }
    FMP4_LOGE("unsupport HTTP_METHOD_ %s,url %s\r\n",tHttpReqPacket.strMethod,tHttpReqPacket.strURL);

    HttpServer *pHttpServer=new HttpServer();
    iRet=pHttpServer->CreateResponse(400,"bad request");
    iRet|=pHttpServer->SetResHeaderValue("Connection", "Keep-Alive");
    iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Origin", "*");
    iRet=pHttpServer->FormatResToStream(NULL,0,o_strRes,i_iResMaxLen);
    delete pHttpServer;
    
    return iRet;
}
/*****************************************************************************
-Fuction        : HandleReqGetFMP4
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFMP4Server::HandleReqGetFMP4(string *i_pPlaySrc,char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;
    char *pcFMP4 = NULL;
    int iFMP4Len = -1;

    if(NULL == i_pPlaySrc || NULL == o_strRes|| i_iResMaxLen <= 0)
    {
        FMP4_LOGE("HandleReqGetM3U8 NULL \r\n");
        return iRet;
    }
    
    if(NULL == m_pHttpFMP4ServerSession)
    {
        m_pHttpFMP4ServerSession = new HttpFMP4ServerSession((char *)i_pPlaySrc->c_str());
    }
    pcFMP4 = new char [HTTP_FMP4_MAX_LEN];//后续优化
    iFMP4Len = m_pHttpFMP4ServerSession->GetFMP4(pcFMP4,HTTP_FMP4_MAX_LEN);
    if(iFMP4Len <= 0)
    {
        FMP4_LOGE("HandleReq GetFMP4 err \r\n");
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
    iRet=pHttpServer->FormatResToStream(pcFMP4,iFMP4Len,o_strRes,i_iResMaxLen);
    delete pHttpServer;
    delete [] pcFMP4;
    return iRet;
}
/*****************************************************************************
-Fuction        : GetFMP4
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HttpFMP4Server::GetFMP4(char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;

    if(NULL == o_strRes|| i_iResMaxLen <= 0)
    {
        FMP4_LOGE("GetFMP4 NULL%p \r\n",m_pHttpFMP4ServerSession);
        return iRet;
    }
    if(NULL == m_pHttpFMP4ServerSession)
    {
        //FMP4_LOGE("NULL == m_pHttpFMP4ServerSession %p \r\n",m_pHttpFMP4ServerSession);
        return 0;
    }
    return m_pHttpFMP4ServerSession->GetFMP4(o_strRes,i_iResMaxLen);
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
int HttpFMP4Server::Regex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt)
{
    int iRet = -1;

    if(NULL == i_strPattern || NULL == i_strBuf || NULL == o_aMatch || i_iMatchMaxCnt <= 0)
    {
        FMP4_LOGE("HttpFMP4Server Regex NULL \r\n");
        return iRet;
    }
    iRet = LinuxCRegex(i_strPattern,i_strBuf,o_aMatch,i_iMatchMaxCnt);
    if(HTTP_FMP4_UN_SUPPORT_FUN!=iRet)
    {
        return iRet;
    }
    string strBuf(i_strBuf);
    regex Pattern(i_strPattern);//http://localhost:9212/file/H264AAC.mp4/test.m3u8
    smatch Match;
    if (std::regex_search(strBuf,Match,Pattern)) 
    {
        if((int)Match.size()<=0 || i_iMatchMaxCnt < (int)Match.size())
        {
            FMP4_LOGE("HlsRegex err i_iMatchMaxCnt %d<%d Match.size()-1 \r\n",i_iMatchMaxCnt,Match.size()-1);
            return iRet;
        }
        iRet = (int)Match.size();
        // 输出所有匹配到的子串
        for (int i = 0; i < iRet; ++i) 
        {
            o_aMatch[i].assign( Match[i].str());
        }
    } 
    return iRet;
}


