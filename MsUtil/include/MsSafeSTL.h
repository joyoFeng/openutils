#ifndef __MS_SAFE_STL_H__
#define __MS_SAFE_STL_H__


#include <vector>
#include <list>
#include <set>
#include <map>
#include <deque>



#include <MsMutexLocker.h>


namespace mathvs
{
	using namespace std;

	template <class T>
	class SafeVector : public vector<T>
	{
	public:
		SafeVector()
		{

		}
		~SafeVector()
		{

		}
		void lock()
		{
			m_locker.lock();
		}
		void unLock()
		{
			m_locker.unLock();
		}
	private:
		MsMutexLocker m_locker;
	};

	template <class T>
	class SafeList : public list<T>
	{
	public:
		SafeList()
		{

		}
		~SafeList()
		{

		}
		void lock()
		{
			m_locker.lock();
		}
		void unLock()
		{
			m_locker.unLock();
		}
	private:
		MsMutexLocker			m_locker;
	};

	template <class T>
	class CSafeList
	{
	public:
		CSafeList() {

		}

		~CSafeList(void) {
			clear();
		}

		void add(T* pObject)
		{
			m_locker.lock();
			m_list.push_back(pObject);
			m_locker.unLock();
		}

		T* get()
		{
			T* pObject = NULL;
			m_locker.lock();

			if (!m_list.empty())
			{
				pObject = (T*)m_list.front();
				m_list.pop_front();
			}

			m_locker.unLock();
			return pObject;
		}

		void clear()
		{
			T* pObject = NULL;
			m_locker.lock();

			while (!m_list.empty())
			{
				pObject = (T*)m_list.front();
				m_list.pop_front();

				if (pObject)
				{
					delete pObject;
					pObject = NULL;
				}
			}

			m_locker.unLock();
		}

		int size()
		{
			int nRet = 0;

			m_locker.lock();
			nRet = (int)m_list.size();
			m_locker.unLock();

			return nRet;
		}

		T* peek()
		{
			T* pObject = NULL;
			m_locker.lock();
			if (!m_list.empty())
			{
				pObject = (T*)m_list.front();
			}
			m_locker.unLock();
			return pObject;
		}

		void pop_front()
		{
			m_locker.lock();
			if (!m_list.empty())
			{
				m_list.pop_front();
			}
			m_locker.unLock();
		}

		void add_front(T * pObject)
		{
			m_locker.lock();
			m_list.push_front(pObject);
			m_locker.unLock();
		}

	private:
		MsMutexLocker			m_locker;
		std::list<T*>			m_list;
	};


	template <class K, class T>
	class SafeMap : public map<K, T>
	{
	public:
		SafeMap()
		{

		}
		~SafeMap()
		{

		}
		void lock()
		{
			m_locker.unLock();
		}
		void unLock()
		{
			m_locker.unLock();
		}
	private:
		MsMutexLocker m_locker;
	};


	template <class T>
	class SafeQueue : public deque<T>
	{
	public:
		SafeQueue()
		{

		}
		~SafeQueue()
		{

		}
		void lock()
		{
			m_locker.lock();
		}
		void unLock()
		{
			m_locker.unLock();
		}

	private:
		MsMutexLocker m_locker;
	};


	template <class T>
	class SafeSet : public set<T>
	{
	public:
		SafeSet()
		{

		}
		~SafeSet()
		{

		}
		void lock()
		{
			m_locker.lock();
		}
		void unLock()
		{
			m_locker.unLock();
		}
	private:
		MsMutexLocker m_locker;
	};
};

#endif