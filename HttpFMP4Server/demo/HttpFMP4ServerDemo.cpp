/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvServerDemo.c
* Description           : 	
* Created               :       2023.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HttpFlvServerDemo.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <utility>

using std::make_pair;

/*****************************************************************************
-Fuction		: HttpFlvServerDemo
-Description	: HttpFlvServerDemo
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2023/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HttpFlvServerDemo :: HttpFlvServerDemo(int i_iServerPort)
{
    TcpServer::Init(NULL,i_iServerPort);
}

/*****************************************************************************
-Fuction		: ~HttpFlvServerDemo
-Description	: ~HttpFlvServerDemo
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2023/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HttpFlvServerDemo :: ~HttpFlvServerDemo()
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
int HttpFlvServerDemo :: Proc()
{
    int iClientSocketFd=-1;
    HttpFlvServerIO *pHttpFlvServerIO = NULL;
    while(1)
    {
        iClientSocketFd=TcpServer::Accept();
        if(iClientSocketFd<0)  
        {  
            SleepMs(10);
            CheckMapServerIO();
            continue;
        } 
        pHttpFlvServerIO = new HttpFlvServerIO(iClientSocketFd);
        AddMapServerIO(pHttpFlvServerIO,iClientSocketFd);
        FLV_LOGD("m_HttpFlvServerIOMap size %d\r\n",m_HttpFlvServerIOMap.size());
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
int HttpFlvServerDemo::CheckMapServerIO()
{
    int iRet = -1;
    HttpFlvServerIO *pHttpFlvServerIO=NULL;

    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard对象会在其作用域结束时自动释放互斥量
    for (map<int, HttpFlvServerIO *>::iterator iter = m_HttpFlvServerIOMap.begin(); iter != m_HttpFlvServerIOMap.end(); )
    {
        pHttpFlvServerIO=iter->second;
        if(0 == pHttpFlvServerIO->GetProcFlag())
        {
            delete pHttpFlvServerIO;
            iter=m_HttpFlvServerIOMap.erase(iter);// 擦除元素并返回下一个元素的迭代器
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
int HttpFlvServerDemo::AddMapServerIO(HttpFlvServerIO * i_pHttpFlvServerIO,int i_iClientSocketFd)
{
    int iRet = -1;

    if(NULL == i_pHttpFlvServerIO)
    {
        FLV_LOGE("AddMapServerIO NULL!!!%p\r\n",i_pHttpFlvServerIO);
        return -1;
    }
    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard对象会在其作用域结束时自动释放互斥量
    m_HttpFlvServerIOMap.insert(make_pair(i_iClientSocketFd,i_pHttpFlvServerIO));
    return 0;
}


