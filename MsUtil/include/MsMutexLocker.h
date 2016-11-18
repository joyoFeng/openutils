#ifndef __MS_MUTEX_LOCKER_H__
#define __MS_MUTEX_LOCKER_H__


#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace matchvs
{
	class MsMutexLocker
	{
	public:
		MsMutexLocker();
		~MsMutexLocker();
		void lock();
		void unLock();
	private:
	#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
		CRITICAL_SECTION m_mutex;
	#else
		pthread_mutex_t  m_mutex;
	#endif
	};
};


#endif