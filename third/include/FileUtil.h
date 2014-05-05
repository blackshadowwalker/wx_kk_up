// FileUtil.h

/**
* Auth: Karl
* Date: 2014/2/20
* LastUpdate: 2014/2/24
*/

#pragma once

#ifndef HEADER_FILEUTIL
#define HEADER_FILEUTIL

//#include <list>
//#include <string>
//using namespace std;

#include "afxwin.h"
#include <windows.h>  
#include <ShlObj.h> 
#include <stdio.h>
#include <time.h>   
#include <string>
#include <list>

using namespace std;

#ifndef MAX_PATH
	#define MAX_PATH 512
#endif

class FileUtil
{
public:
	FileUtil(void);
	~FileUtil(void);

	/**
	* 列出目录下的所有文件
	* @baseDir : 文件根目录
	* @curDir ：相对目录，完整路径为 baseDir + "\\" + curDir + "\\" + wfd.cFileName
	* @listResult: 返回的文件列表
	* @fileExt: 文件扩展名称，如 "*.jpg;*.bmp;*.png"， 如果为空 NULL | "" 则不过滤文件
	* @PathNameType: 返回的文件名称方式, 0：全路径  1:相对路径(curDir + "\\" + wfd.cFileName) 2:文件名(wfd.cFileName)
	* @bAddDirectory: 文件夹名称是否加入文件列表
	* @bListSub: 是否列出子文件夹中的文件
	*
	* @Return : &listResult
	*
	* e.g:
	*	 list<char *> list; 
	*	 FileUtil::ListFiles( "D:\\upload", "", list, "*.jpg;*.bmp;*.png", true, false, true);
	*
	*/
	static list<char*>* ListFiles(char *baseDir, char *curDir, list<char*> &listResult, char *fileExt = 0, int PathNameType=0, bool bAddDirectory=false, bool bListSub=false);

	
	//弹出选择文件夹对话框
	static char* SelectFolder(HWND hwnd, char* title="Select Folder", char* dir=0);

	//判断文件/文件夹是否存在
	//判断文件：	FindFirstFileExists(lpPath, FALSE);  
	//判断文件夹：	FindFirstFileExists(lpPath, FILE_ATTRIBUTE_DIRECTORY);  
	static BOOL FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter);

	//创建多级目录
	static bool CreateFolders(const char* folderPath);

	//复制文件，会自动创建目录
	static long	CopyFileEx(const char* src, const char* dest, BOOL bFailIfExists ); 

	//删除多级目录（包含文件）
	static bool RemoveDir(const char* szFileDir);

	static char *FormatFileName(const char *path, int indexIn=0, bool fileTime=true, const char *destDir=0);

private:
	


};

char *GetDateTime(char *timeString=0);
void __cdecl debug(const char *format, ...);
void __cdecl release(const char *format, ...);

long __cdecl getCurrentTime();

#endif