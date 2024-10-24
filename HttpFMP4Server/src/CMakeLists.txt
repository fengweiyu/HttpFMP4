cmake_minimum_required (VERSION 2.8)

SET(CMAKE_TOOLCHAIN_FILE "build/ToolChain.cmake")
#SET(CMAKE_SYSTEM_NAME "Linux")
#SET(CMAKE_C_COMPILER "gcc")
#SET(CMAKE_CXX_COMPILER "g++")


project (HttpFMP4Server_static)
 
#SET(CMAKE_C_FLAGS "-g")
SET(CMAKE_CXX_FLAGS "-g -std=c++11 -pthread ")

#add_definitions(-DMEDIA_SEVER_TYPE_WEBRTC)
# 设置C++标准为C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(src_list
    HttpFMP4ServerInf.cpp
	HttpFMP4Server.cpp
    HttpFMP4ServerSession.cpp
	)		
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/ThirdSrc/http/include
	${PROJECT_SOURCE_DIR}/ThirdSrc/media/src/include
	)
ADD_LIBRARY(HttpFMP4Server_static STATIC ${src_list})
SET_TARGET_PROPERTIES(HttpFMP4Server_static PROPERTIES OUTPUT_NAME HttpFMP4Server)


SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/lib)





#TARGET_LINK_LIBRARIES(HttpFMP4Server pthread -lm)

#TARGET_LINK_LIBRARIES(crnode log4cpp thrift)










