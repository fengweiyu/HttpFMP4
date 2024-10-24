/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFMP4ServerDemo.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FMP4_SERVER_DEMO_H
#define HTTP_FMP4_SERVER_DEMO_H

#include <mutex>
#include <string>
#include <list>
#include <map>
#include "HttpFMP4ServerIO.h"

using std::map;
using std::string;
using std::list;
using std::mutex;

/*****************************************************************************
-Class			: HttpFMP4ServerDemo
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2019/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class HttpFMP4ServerDemo : public TcpServer
{
public:
	HttpFMP4ServerDemo(int i_iServerPort);
	virtual ~HttpFMP4ServerDemo();
    int Proc();
    
private:
    int CheckMapServerIO();
    int AddMapServerIO(HttpFMP4ServerIO * i_pHttpFMP4ServerIO,int i_iClientSocketFd);
    
    map<int, HttpFMP4ServerIO *>  m_HttpFMP4ServerIOMap;
    mutex m_MapMtx;
};

#endif
