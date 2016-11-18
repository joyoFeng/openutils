#ifndef __MS_TIME_OPS_H__
#define __MS_TIME_OPS_H__

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
#include <WinSock2.h>
#include <windows.h>
#else
#include <sys/time.h>
#include <sys/time.h>

#endif

namespace matchvs 
{
	void sleepOps(unsigned long msec);
	void gettimeofdayOps(struct timeval *tp, void *tzp);

};



#endif