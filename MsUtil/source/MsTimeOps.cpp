#include <MsTimeOps.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

namespace matchvs
{
	void sleepOps(unsigned long msec)
	{
#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
		Sleep(msec);
#else
		usleep(msec * 1000);
#endif
	}


	void gettimeofdayOps(struct timeval *tp, void *tzp)
	{
#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
		FILETIME ft;
		unsigned __int64 tmpres = 0;
		static int tzflag;

		if (NULL != tp)
		{
			GetSystemTimeAsFileTime(&ft);

			tmpres |= ft.dwHighDateTime;
			tmpres <<= 32;
			tmpres |= ft.dwLowDateTime;

			tmpres /= 10;
			tmpres -= DELTA_EPOCH_IN_MICROSECS;
			tp->tv_sec = (long)(tmpres / 1000000UL);
			tp->tv_usec = (long)(tmpres % 1000000UL);
		}
#else
		gettimeofday(tp, tzp);
#endif
	}


};