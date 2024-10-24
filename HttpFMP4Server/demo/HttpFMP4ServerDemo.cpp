/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFMP4ServerDemo.c
* Description           : 	
* Created               :       2023.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HttpFMP4ServerDemo.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <utility>

using std::make_pair;

/*****************************************************************************
-Fuction		: HttpFMP4ServerDemo
-Description	: HttpFMP4ServerDemo
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2023/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HttpFMP4ServerDemo :: HttpFMP4ServerDemo(int i_iServerPort)
{
    TcpServer::Init(NULL,i_iServerPort);
}

/*****************************************************************************
-Fuction		: ~HttpFMP4ServerDemo
-Description	: ~HttpFMP4ServerDemo
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2023/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HttpFMP4ServerDemo :: ~HttpFMP4ServerDemo()
{
}

/*****************************************************************************
-Fuction		: Proc
-Description	: 阻塞
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2023/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HttpFMP4ServerDemo :: Proc()
{
    int iClientSocketFd=-1;
    HttpFMP4ServerIO *pHttpFMP4ServerIO = NULL;
    while(1)
    {
        iClientSocketFd=TcpServer::Accept();
        if(iClientSocketFd<0)  
        {  
            SleepMs(10);
            CheckMapServerIO();
            continue;
        } 
        pHttpFMP4ServerIO = new HttpFMP4ServerIO(iClientSocketFd);
        AddMapServerIO(pHttpFMP4ServerIO,iClientSocketFd);
        FMP4_LOGD("m_HttpFMP4ServerIOMap size %d\r\n",m_HttpFMP4ServerIOMap.size());
    }
    return 0;
}

/*****************************************************************************
-Fuction        : CheckMapServerIO
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int HttpFMP4ServerDemo::CheckMapServerIO()
{
    int iRet = -1;
    HttpFMP4ServerIO *pHttpFMP4ServerIO=NULL;

    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard对象会在其作用域结束时自动释放互斥量
    for (map<int, HttpFMP4ServerIO *>::iterator iter = m_HttpFMP4ServerIOMap.begin(); iter != m_HttpFMP4ServerIOMap.end(); )
    {
        pHttpFMP4ServerIO=iter->second;
        if(0 == pHttpFMP4ServerIO->GetProcFlag())
        {
            delete pHttpFMP4ServerIO;
            iter=m_HttpFMP4ServerIOMap.erase(iter);// 擦除元素并返回下一个元素的迭代器
        }
        else
        {
            iter++;// 继续遍历下一个元素
        }
    }
    return 0;
}

/*****************************************************************************
-Fuction        : AddMapHttpSession
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int HttpFMP4ServerDemo::AddMapServerIO(HttpFMP4ServerIO * i_pHttpFMP4ServerIO,int i_iClientSocketFd)
{
    int iRet = -1;

    if(NULL == i_pHttpFMP4ServerIO)
    {
        FMP4_LOGE("AddMapServerIO NULL!!!%p\r\n",i_pHttpFMP4ServerIO);
        return -1;
    }
    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard对象会在其作用域结束时自动释放互斥量
    m_HttpFMP4ServerIOMap.insert(make_pair(i_iClientSocketFd,i_pHttpFMP4ServerIO));
    return 0;
}


