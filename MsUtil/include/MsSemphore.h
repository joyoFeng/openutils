#ifndef __MS_SEMPHORE_H__
#define __MS_SEMPHORE_H__

#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <assert.h>

namespace matchvs
{
	class MsSemaphore 
	{
	public:
		MsSemaphore(void);
		~MsSemaphore();
	public:
		void    wait();
		void    notify();
		void    close();
	private:
#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
		HANDLE		m_handle;
#else
		pthread_cond_t		 m_cond;
		pthread_mutex_t		 m_lock;
#endif
	};
};



#endif