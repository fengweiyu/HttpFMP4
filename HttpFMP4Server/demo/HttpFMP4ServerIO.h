/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HttpFlvServerIO.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HLS_SERVER_IO_H
#define HLS_SERVER_IO_H

#include "TcpSocket.h"
#include "HttpFlvServerInf.h"
#include "HttpFlvServerCom.h"
#include <thread>
#include <mutex>

using std::thread;
using std::mutex;

/*****************************************************************************
-Class			: HttpFlvServerIO
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2019/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class HttpFlvServerIO : TcpServer
{
public:
	HttpFlvServerIO(int i_iClientSocketFd);
	virtual ~HttpFlvServerIO();
    int Proc();
    int GetProcFlag();
private:
	int m_iClientSocketFd;
	
    HttpFlvServerInf m_HttpFlvServerInf;

    thread * m_pHttpFlvServerIOProc;
	int m_iHttpFlvServerIOFlag;
};

#endif
