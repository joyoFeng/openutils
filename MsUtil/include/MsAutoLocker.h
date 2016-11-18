#ifndef __MS_AUTO_LOCKER_H__
#define __MS_AUTO_LOCKER_H__

#include <MsMutexLocker.h>

namespace matchvs
{


	class MsAutoLock {
	public:
		MsAutoLock(MsMutexLocker &m)
			:m_mutex(m)
		{
			m_mutex.lock();
		};

		~MsAutoLock()
		{
			m_mutex.unLock();
		};

	private:
		MsMutexLocker &m_mutex;
	};

};


#endif