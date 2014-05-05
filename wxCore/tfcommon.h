
// tfcommon.h
#pragma once

#ifndef _TF_COMMMON_HEADER_
#define _TF_COMMMON_HEADER_


//=========== define errors  ================

#define		OK			1  //成功
#define		FALIED		-1 //失败
#define		REFUSED		0 // Refuse
#define		CONNECTED	2  //连接成功
#define		CONNECT_ERROR	-2 //连接失败
#define		CONNECT_TIMEOUT -1 //通讯超时

//===========================================



#define TF_MAX_PATH_LEN  512

//系统环境设置
typedef struct SystemConfig{
	char	updateurl[TF_MAX_PATH_LEN];//软件更新地址
	int		autorun;//软件开启后自动运行
	char	host[TF_MAX_PATH_LEN];//车辆信息上传服务器地址
	char	runMode[64];//#运行模式,{"无锡测试":"wx", "":""}
}SystemConfig;

//卡口配置
typedef struct KKConfig{
	char	id[TF_MAX_PATH_LEN];//卡口编号
	char	direction[TF_MAX_PATH_LEN];//方向类型
	long	cdid;//车道号
	long	cdSpeedLimit;//车道限速
	long	cdSpeedLimitDefalut;//车道限速默认值
	void *  hDir; //文件监控句柄
	char	ftpPath[TF_MAX_PATH_LEN];//对应的监控FTP目录
	char	httpPath[TF_MAX_PATH_LEN];//对应的HTTP目录
	char	http[TF_MAX_PATH_LEN];//http访问
	char	fileExt[128];//FTP目录符合要求的文件扩展名:e.g:  "*.jpg;*.bmp;*.png"
}KKConfig;


//车辆信息,上传稽查布控系统
typedef struct VehicleInfo
{
	long	id;//for db

	char	kkbh[64];//卡口编号  不能为空
	char	fxlx[64];//	方向类型	不能为空
	long	cdh;//	车道号	不能为空
	char	hphm[64];//	号牌号码	无牌、未识别、无法识别均用半角“-”表示，其中无号牌要注意hpzl填41
	char	hpzl[64];//	号牌种类	参考GA24.7（如01-大型汽车，02-小型汽车，25-农机号牌，41-无号牌，42-假号牌，99-其他号牌），不能为空；
	char	gcsj[64];//	过车时间	日期格式字符串“yyyy-mm-dd hh24:mi:ss”,形如“2003-09-11 11:07:23”，不能为空
	long	clsd;//	车辆速度	最长3位，单位：公里/小时
	long	clxs;//	车辆限速	最长3位，单位：公里/小时
	char	wfdm[16];//	违章行为编码	参考GA408.1
	long	cwkc;//车外廓长,最长5位，以厘米为单位
	char	hpys[16];//	号牌颜色	0-白色，1-黄色，2-蓝色，3-黑色，4-绿色，9-其它颜色，不能为空；
	char	cllx[64];//	车辆类型	参考GA24.4（K11-大型普通客车，K21-中型普通客车，K31-小型普通客车，K33-轿车，H11-重型普通客车，H21-中型普通客车，M22-轻便二轮摩托车）
	char	fzhpzl[64];//	辅助号牌种类	参考GA24.7
	char	fzhphm[16];//	辅助号牌号码	无牌、未识别、无法识别均用半角“-”表示，其中无号牌要注意hpzl填41
	char	fzhpys[16];//	辅助号牌颜色	0-白色，1-黄色，2-蓝色，3-黑色，4-绿色，9-其它颜色
	char	clpp[64];//	车辆品牌	
	char	clwx[64];//	车辆外形	
	char	csys[16];//	车身颜色	参考GA24.8（A-白，B-灰，C-黄，D-粉，E-红，F-紫，G-绿，H-蓝，I-棕，J-黑，Z-其他）
	char	tplj[256];//通行图片路径	图片访问路径中固定部分（长度限定不能超过100个字节），如：FTP访问路径：ftp://user:666@192.168.1.1/001.jpg，其中图片路径为ftp://user:666@192.168.1.1/  
	char	tp3[256];
	char	tp2[256];
	char	tp1[256];//	通行图片1	图片访问路径中变化的部分（长度限定不能超过60个字节），如：FTP访问路径：ftp://user:666@192.168.1.1/001.jpg，其中图片名称为001.jpg。
	
	char	tztp[256];//	特征图片	扩展预留，为空
}VehicleInfo;

#endif