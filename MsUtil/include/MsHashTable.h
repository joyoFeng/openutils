#ifndef __MS_HASH_TABLE_H__
#define __MS_HASH_TABLE_H__


#include <MsAutoLocker.h>


#define   HASH_ELEM  2048


namespace matchvs
{
	template <typename T>
	class MsHashTable
	{
	public:
		struct tagLink
		{
			tagLink * next;
			unsigned int id;
			T * pObj;
			tagLink()
			{
				next = NULL;
				id = 0; 
				pObj = NULL;
			}
		};
	public:
		MsHashTable(unsigned int hashElem = HASH_ELEM, bool lock = false)
		{
			m_isLock = lock;
			m_hashElem = HASH_ELEM;
			m_hash = new tagLink *[m_hashElem];

			if (m_isLock)
			{
				m_tableLock = new MsMutexLocker[m_hashElem];
			}
			
			m_nElemCount = 0;
			memset(m_hash, 0, m_hashElem * sizeof(tagLink *));
		}

		virtual ~MsHashTable(void)
		{
			tagLink * pNode, *pDelNode;
			for (int i = 0; i < m_hashElem; ++i)
			{
				tableLock(i % m_hashElem);
				pNode = m_hash[i];
				while (pNode)
				{
					pDelNode = pNode;
					pNode = pNode->next;
					delete pDelNode->pObj;
					delete pDelNode;
				}
				m_hash[i] = NULL;
				tableUnLock(i % m_hashElem);
			}

			delete[]m_hash;
			if (m_isLock)
			{
				delete[]m_tableLock;
			}
			
		}

		virtual T * addHash(unsigned int id)
		{
		

			T * pObj = NULL;
			pObj = find(id);
			if (pObj)
			{
				return pObj;
			}

			unsigned int slot = id % m_hashElem;
			
			tableAutoLock(slot);

			addElem();

			tagLink *  pNode = new tagLink();
			pNode->id = id;
			pNode->pObj = new T();

			if (m_hash[slot])
			{
				pNode->next = m_hash[slot];
				m_hash[slot] = pNode;
			}
			else
			{
				m_hash[slot] = pNode;
			}
			return pNode->pObj;
		}

		virtual bool delHash(unsigned int id)
		{
			tagLink * pNode, *pNodePre;
			unsigned int slot = id % m_hashElem;

			tableAutoLock(slot);

			pNodePre = NULL;
			pNode = m_hash[slot];
			while (pNode)
			{
				if (id == pNode->id)
				{
					delElem();
					if (pNodePre == NULL)
					{
						m_hash[slot] = pNode->next;
					}
					else
					{
						pNodePre->next = pNode->next;
					}

					delete pNode->pObj; 
					pNode->pObj = NULL;
					delete pNode; 
					pNode = NULL;
					break;
				}
				pNodePre = pNode;
				pNode = pNode->next;
			}
			return true;
		}

		virtual T * find(unsigned int id)
		{
			tagLink * pNode;
			unsigned int slot = id % m_hashElem;

			tableAutoLock(slot);
			pNode = m_hash[slot];
			while (pNode)
			{
				if (id == pNode->id)
				{
					return pNode->pObj;
				}
				pNode = pNode->next;
			}
			return NULL;
		}

		int getElemCount()
		{
			return m_nElemCount;
		}

	private:

		void addElem()
		{
			if (m_isLock)
			{
				m_lockOnline.lock();
				++m_nElemCount;
				m_lockOnline.unlock();
			}
			else
			{
				m_lockOnline.lock();
				++m_nElemCount;
				m_lockOnline.unLock();
			}
		}

		void delElem()
		{
			if (m_isLock)
			{
				m_lockOnline.lock();
				--m_nElemCount;
				m_lockOnline.unLock();
			}
			else
			{
				--m_nElemCount;
			}
		}

		void tableAutoLock(unsigned int slot)
		{
			if(m_isLock)
				MsAutoLock MyLock(&m_tableLock[slot]);
		}

		void tableLock(unsigned int slot)
		{
			if (m_isLock)
				m_tableLock[slot].lock();
		}

		void tableUnLock(unsigned int slot)
		{
			if (m_isLock)
				m_tableLock[slot].unLock();
		}



	public:
		tagLink						**m_hash;
	private:
		volatile unsigned int 		m_nElemCount;
		unsigned int				m_hashElem;

		MsMutexLocker				*m_tableLock;
		MsMutexLocker				 m_lockOnline;
		bool						m_isLock;
	};
};



#endif