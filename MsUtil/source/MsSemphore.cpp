#include <MsSemphore.h>



namespace matchvs
{
	MsSemaphore::MsSemaphore()
	{
#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
		m_handle = NULL;
		m_handle = ::CreateSemaphore(NULL, 0, 1, NULL);
		assert(m_handle);
#else
		pthread_cond_init(&m_cond, NULL);
		pthread_mutex_init(&m_lock, NULL);
#endif
	}

	MsSemaphore::~MsSemaphore()
	{
		close();
	}

	void    MsSemaphore::wait() {
#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
		assert(m_handle);
		::WaitForSingleObject(m_handle, INFINITE);
#else
		pthread_cond_wait(&m_cond, &m_lock);
#endif
	}

	void    MsSemaphore::notify() {
#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
		assert(m_handle);
		::ReleaseSemaphore(m_handle, 1, NULL);
#else
		pthread_cond_signal(&m_cond);
#endif
	}

	void    MsSemaphore::close() {
#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
		if (m_handle != NULL) {
			::CloseHandle(m_handle);
			m_handle = NULL;
		}
#else
		pthread_cond_destroy(&m_cond);
		pthread_mutex_destroy(&m_lock);
#endif
	}
};