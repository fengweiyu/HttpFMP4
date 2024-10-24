/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :    HttpFMP4ServerCom.h
* Description           :    模块内部与外部调用者共同的依赖，放到对外的include里
* Created               :    2020.01.13.
* Author                :    Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HTTP_FMP4_SERVER_COM_H
#define HTTP_FMP4_SERVER_COM_H


#ifdef _WIN32
#include <Windows.h>
#define SleepMs(val) Sleep(val)
#else
#include <unistd.h>
#define SleepMs(val) usleep(val*1000)
#endif



#define  FMP4_LOGE2(val,fmt,...)      printf("[ERR]<%d>:"fmt,val,##__VA_ARGS__)
#define  FMP4_LOGW2(val,fmt,...)      printf("[WRN]<%d>:"fmt,val,##__VA_ARGS__)
#define  FMP4_LOGD2(val,fmt,...)      printf("[DBG]<%d>:"fmt,val,##__VA_ARGS__)
#define  FMP4_LOGI2(val,fmt,...)      printf("[INF]<%d>:"fmt,val,##__VA_ARGS__)
#define  FMP4_LOGW(...)     printf(__VA_ARGS__)
#define  FMP4_LOGE(...)     printf(__VA_ARGS__)
#define  FMP4_LOGD(...)     printf(__VA_ARGS__)
#define  FMP4_LOGI(...)     printf(__VA_ARGS__)

















#endif
