
//TCode.h

#ifndef __TCode_H 
#define __TCode_H 

#include <WinSock2.h>
#include <string> 
 
 
using namespace std; 

//UTF-8到GB2312的转换
char* U2G(const char* utf8);

//GB2312到UTF-8的转换
char* G2U(const char* gb2312);

//字符串反向查找，与 strstr相反
char* strrstr(char*source ,const char* match);
 
#endif 