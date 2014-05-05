
#include "TCode.h"   


//UTF-8到GB2312的转换
char* U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len+1];
	memset(str, 0, len+1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return str;
}

//GB2312到UTF-8的转换
char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len+1];
	memset(str, 0, len+1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return str;
}

/*
*  字符串反向查找，与 strstr相反
*  字符反向查找函数: strrchr
*/
char* strrstr(char* source ,const char* match)
{
	if(source==0 || match==0)
		return 0;
	int lenSource = strlen(source);
	int lenMatch = strlen(match);
	if( lenSource < lenMatch)
		return 0;
	char *p =   source + lenSource - lenMatch ;
	while( p >= source)
	{
		if( p[0] == match[0] && strncmp( p, match,lenMatch)==0) 
			break;
		p -- ;
	}
	if(p<=source)
		p = 0;

    return p;        
}