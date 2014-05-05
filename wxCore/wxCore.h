#pragma once

#ifndef _WX_CORE_HEADER_
#define _WX_CORE_HEADER_

#include "MemLeak.h"
#include "zlog.h"
#include "pthread.h"

#include "sqlite3.h"
#include "mysql.h"
#include "md5.h"
#include "tfcommon.h"
#include "DB.h"

class wxCore
{
public:
	wxCore(void);
	~wxCore(void);
};

#endif