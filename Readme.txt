无锡卡口测试--数据上传稽查布控系统模块

一、配置说明：
1.各个依赖项目设置
	常规--配置类型：静态库(.lib)
	常规--MFC的使用：使用标准Windows库
	输出目录都为: ..\lib  即 bin\lib
	中间目录都为：..\bin\mid\debug\xxProjectName
	
2.主项目
	常规--MFC的使用：在静态库中使用MFC
	C/C++--代码生成--运行库: 多线程调试(/MTd)
	输出目录都为: ..\lib  即 bin\lib
	中间目录都为：..\bin\mid\debug\xxProjectName
	将third\include以及子目录加入include目录
	连接器--常规：附近库目录  ..\bin\lib，最后加入 ./ 否则CrashHandler找不到stdafx.h
	连接器--输入，加入必要的lib
			mysqllib.lib
			unixport.lib
			zlog.lib
			pthreadVC2.lib
			xxxOther.lib
	stdafx.h最后加入包含文件	
			#include <signal.h>
			#include <exception>
			#include <sys/stat.h>
			#include <psapi.h>
			#include <rtcapi.h>
			#include <Shellapi.h>
			#include <dbghelp.h>
			
	
3.其他
	lib库都在  bin\lib下
	dll和exe在  bin下
	将多线程运行库pthreadVC2.lib拷贝到bin\lib下，pthreadVC2.dll拷贝到bin下


二、源码框架使用说明：

1.日志zlog
  zlog依赖与unixport
  使用方法:
  1)配置文件 logcfg.conf
  2)初始化: xxApp::InitInstance()
		dzlog_init("logcfg.conf", "my_cat");
  3)销毁 xxApp::ExitInstance() 
		zlog_fini();

		
2.崩溃堆栈dmp
  使用方法：
  1)项目添加 CrashHandler.h & CrashHandler.cpp
  2)在xxApp.cpp :: InitInstance()添加
		CCrashHandler ch;//后面用来捕获一个异常
		ch.SetProcessExceptionHandlers();//设置一个对整个程序有效的异常处理函数（如SEH)
		ch.SetThreadExceptionHandlers();//设置仅对当前个线程有效的异常处理函数（如预料外异常或终止处理函数）
  3)使用vs调试运行

  
3.内存泄漏检查CMemLeakDetect
  使用方法：
  1)项目添加 mem
  2)添加包含头文件mem/MemLake.h
  3)定义使用，如在xxApp.cpp
     #ifdef _DEBUG
		CMemLeakDetect memLeakDetect;
	 #endif
   4)debugoutput区域会打印内存泄漏
   
4. ini文件读写
	third\include\minIni.h
	
	
5. mysql使用
	third\include\mysql\mysql.h

6. sqlite使用
	third\include\sqlite\sqlite3.h

7. md5使用
	third\include\md5.h

三、本程序的说明
1. 设计说明
   运用主要的库 zlog + mysql + sqlite + pthread
   
   1) 监控目录线程监控 FTP上传目录，将监控到的数据库放入list;
   2) 处理线程获取必要的信息，GetInfoFromPicture()
		检查数据库中是否存在该图片记录 ret = CheckInfo();
		if(ret>=1)//若存在
			continue; 
		else //否则{
			CopyFileFromFTP2HTTP() 复制图片到http目录
			Insert2DB()写入数据库
		}
   3) 上传线程
         读取数据库 QueryDB() where status=1;
		 上传 Upload();
		 更改状态  status=0;
   
   
2. 使用说明
   详见操作手册
	
  
 