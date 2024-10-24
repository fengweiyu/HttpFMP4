/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvServerDemo.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FLV_SERVER_DEMO_H
#define HTTP_FLV_SERVER_DEMO_H

#include <mutex>
#include <string>
#include <list>
#include <map>
#include "HttpFlvServerIO.h"

using std::map;
using std::string;
using std::list;
using std::mutex;

/*****************************************************************************
-Class			: HttpFlvServerDemo
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2019/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class HttpFlvServerDemo : public TcpServer
{
public:
	HttpFlvServerDemo(int i_iServerPort);
	virtual ~HttpFlvServerDemo();
    int Proc();
    
private:
    int CheckMapServerIO();
    int AddMapServerIO(HttpFlvServerIO * i_pHttpFlvServerIO,int i_iClientSocketFd);
    
    map<int, HttpFlvServerIO *>  m_HttpFlvServerIOMap;
    mutex m_MapMtx;
};

#endif
