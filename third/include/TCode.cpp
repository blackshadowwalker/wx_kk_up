
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

//convert wchar_t to char
// e.g: wcharTochar(wchar, chr, sizeof(chr));  
void wcharTochar(const wchar_t *wchar, char *chr )
{  
	int length = WideCharToMultiByte(CP_ACP, 0, wchar, -1, NULL, 0, NULL, NULL);
    WideCharToMultiByte( CP_ACP, 0, wchar, -1,  
        chr, length, NULL, NULL );  
}  

//convert char to wchar_t 
// e.g : char = (wchar_t *)malloc(sizeof(wchar_t) * charlength);   \
		 charTowchar(chr, wchar, sizeof(wchar_t) * charlength);  
void charTowchar(const char *chr, wchar_t *wchar)
{     
	int length = MultiByteToWideChar(CP_ACP, 0, chr, -1, NULL, 0);
    MultiByteToWideChar( CP_ACP, 0, chr,  
        strlen(chr)+1, wchar, length/sizeof(wchar[0]) );  
}  

/*********************************************************************   
**   从系统中取最后一次错误代码，并转换成字符串返回   
*********************************************************************/   
LPTSTR   GetLastErrorText( long lErrorCode,  LPTSTR   lpszBuf,   DWORD   dwSize   )     
{   
	DWORD   dwRet;   
	LPTSTR   lpszTemp   =   NULL;   

	dwRet   =   FormatMessage(   FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ARGUMENT_ARRAY,   
		NULL,   
		lErrorCode,   
		LANG_NEUTRAL,   
		(LPTSTR)&lpszTemp,   
		0,   
		NULL   );   

	//   supplied   buffer   is   not   long   enough   
	if(!dwRet||((long)dwSize<(long)dwRet+14))   
		lpszBuf[0]   =   TEXT('\0');   
	else{   
		lpszTemp[lstrlen(lpszTemp)-2]   =   TEXT('\0');     //remove   cr   and   newline   character   
		sprintf(   lpszBuf,   TEXT("%s   (%ld)"),   lpszTemp,   GetLastError()   );   
	}   

	if   (   lpszTemp   )   
		LocalFree((HLOCAL)   lpszTemp   );   

	return   lpszBuf;   
}  
