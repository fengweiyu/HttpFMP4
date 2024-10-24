/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFMP4ServerIO.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FMP4_SERVER_IO_H
#define HTTP_FMP4_SERVER_IO_H

#include "TcpSocket.h"
#include "HttpFMP4ServerInf.h"
#include "HttpFMP4ServerCom.h"
#include <thread>
#include <mutex>

using std::thread;
using std::mutex;

/*****************************************************************************
-Class			: HttpFMP4ServerIO
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2019/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class HttpFMP4ServerIO : TcpServer
{
public:
	HttpFMP4ServerIO(int i_iClientSocketFd);
	virtual ~HttpFMP4ServerIO();
    int Proc();
    int GetProcFlag();
private:
	int m_iClientSocketFd;
	
    HttpFMP4ServerInf m_HttpFMP4ServerInf;

    thread * m_pHttpFMP4ServerIOProc;
	int m_iHttpFMP4ServerIOFlag;
};

#endif
