#include <MsMutexLocker.h>

namespace matchvs
{
	MsMutexLocker::MsMutexLocker()
	{
	#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
		::InitializeCriticalSection(&m_mutex);
	#else
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&m_mutex, &attr);//
		pthread_mutexattr_destroy(&attr);
	#endif//
	}

	MsMutexLocker::~MsMutexLocker()
	{
	#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
		::DeleteCriticalSection(&m_mutex);
	#else
		pthread_mutex_destroy(&m_mutex);
	#endif
	}

	void MsMutexLocker::lock()
	{
	#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
		::EnterCriticalSection(&m_mutex);
	#else
		pthread_mutex_lock(&m_mutex);
	#endif
	};
	void MsMutexLocker::unLock()
	{
	#ifdef _WIN32
		::LeaveCriticalSection(&m_mutex);
	#else
		pthread_mutex_unlock(&m_mutex);
	#endif
	};
};

