#ifndef __MS_THREAD_BASE_H__
#define __MS_THREAD_BASE_H__


#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
#include <process.h>
#include <Windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#include <stdio.h>

enum MsThreadStatus
{
	MS_THREAD_INIT,
	MS_THREAD_STARTING,
	MS_THREAD_RUNNING,
	MS_THREAD_STOPPED
};

class MsThreadBase
{
public:
	MsThreadBase()
	{
		m_eStatus = MS_THREAD_INIT;
		m_bRun = false;
		m_threadId = 0;
	}
	virtual ~MsThreadBase()
	{
		stopThread();
	}

	virtual int startThread()
	{
		if (m_bRun == true || m_eStatus == MS_THREAD_RUNNING)
		{
			printf("thread has been running ..");
			return 0;
		}

		m_eStatus = MS_THREAD_STARTING;
#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
		m_threadId = (HANDLE)_beginthreadex(0, 0, MsThreadBase::dummyProcess, this, 0, 0);
		if(NULL == m_threadId)
		{
			return -1;
		}
#else
		if (0 != pthread_create(&m_threadId, NULL, MsThreadEx::dummyProcess, this))
		{
			return -1;
		}
#endif
		m_bRun = true;

		return 0;
	}

	virtual void setDetach()
	{
#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
		CloseHandle(m_threadId);
		return;
#else
		pthread_detach(m_threadId);
		return;
#endif
	}

	const MsThreadStatus getThreadStatus() { return m_eStatus; }
	void stopThread()
	{
		m_bRun = false;
		m_threadId = 0;
		m_eStatus = MS_THREAD_STOPPED;
	}

#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
	static unsigned _stdcall	dummyProcess(void * pParam)
#else
	static void*				dummyProcess(void* pParam)
#endif
	{
		MsThreadBase * pThread = (MsThreadBase *)pParam;
		pThread->setDetach();
		pThread->m_eStatus = MS_THREAD_RUNNING;
		pThread->process();
		pThread->m_bRun = false;
		pThread->m_eStatus = MS_THREAD_STOPPED;
		return 0;
	}

protected:
	virtual void process() = 0;
protected:
	volatile MsThreadStatus     m_eStatus;
	bool						m_bRun;
#if defined(WIN32)  || defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__)
	HANDLE						m_threadId;
#else
	pthread_t					m_threadId;
#endif
};



#endif