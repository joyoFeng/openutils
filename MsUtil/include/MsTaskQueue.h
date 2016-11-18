#ifndef __MS_TASK_QUEUE_H__
#define __MS_TASK_QUEUE_H__

#include <deque>

#include <MsMutexLocker.h>
#include <MsSemphore.h>


namespace matchvs
{
	template<typename T>
	class MsTaskQueue
	{
	public:
		MsTaskQueue() {}
		virtual ~MsTaskQueue() {}
	public:
		void push(T &data)
		{
			m_lock.lock();
			m_que.push_back(data);
			m_lock.unlock();
			m_sem.notify();
		}

		T pop()
		{
			m_sem.wait();
			m_lock.lock();
			T data;
			if (!m_que.empty())
			{
				data = m_que.front();
				m_que.pop_front();
			}
			else
				assert(0);
			m_lock.unlock();
			return data;
		}

		int size() const 
		{
			int nRet = m_que.size();
			return nRet;
		}

		void clear()
		{
			m_que.clear();
		}
	private:
		std::deque<T>		m_que;
		MsSemaphore			m_sem;
		MsMutexLocker		m_lock;
	};
};


#endif