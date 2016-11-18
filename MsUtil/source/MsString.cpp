
#include <MsString.h>
namespace matchvs
{ 
	MsString::MsString(void)
		: m_length(0)
	{
		m_capacity = ROUND_UP(0);
		m_capacity *= 2;
		m_ptr = (char *)malloc(m_capacity);
		assert(m_ptr);
		memset(m_ptr, 0, m_capacity);
	}


	MsString::MsString(const int pre)
		: m_length(0), m_capacity(0), m_ptr(0)
	{
		m_capacity = ROUND_UP(pre);
		m_capacity *= 2;
		m_ptr = (char *)malloc(m_capacity);
		assert(m_ptr);
		memset(m_ptr, 0, m_capacity);
	}

	MsString::MsString(const char* str)
		: m_length(0), m_capacity(0), m_ptr(0)
	{
		if (!str)
			return;

		m_length = strlen(str);

		m_capacity = ROUND_UP(m_length + 1);
		m_capacity *= 2;
		m_ptr = (char *)malloc(m_capacity);
		assert(m_ptr);
		memset(m_ptr, 0, m_capacity);
		memcpy(m_ptr, str, m_length);
		m_ptr[m_length] = '\0';

	}
	MsString::MsString(const char* str, int size)
	{
		if (!str || size < 0) {
			m_capacity = sizeof(char);
			m_length = 0;
			m_ptr = (char*)malloc(sizeof(char));
			m_ptr[0] = '\0';
			return;
		}
		else {
			m_length = size;
			m_capacity = ROUND_UP(m_length + 1);
			m_capacity *= 2;
			m_ptr = (char*)malloc(m_capacity);
			memset(m_ptr, 0, m_capacity);
			memcpy(m_ptr, str, m_length);
		}
	}

	MsString::MsString(const MsString& obj)
	{
		m_capacity = obj.m_capacity;
		m_length = obj.m_length;
		m_ptr = (char *)malloc(m_capacity);
		assert(m_ptr);
		memset(m_ptr, 0, m_capacity);
		memcpy(m_ptr, obj.m_ptr, m_length);
		m_ptr[m_length] = '\0';
	}


	MsString::~MsString(void)
	{
		if (m_ptr)
		{
			free(m_ptr);
			m_ptr = NULL;
		}

		m_length = 0;
		m_capacity = 0;
	}

	MsString& MsString::operator=(const MsString& obj)
	{
		if (this != &obj)
		{
			if (0 == m_capacity)
			{
				m_capacity = ROUND_UP(obj.m_length + 1);
				m_capacity *= 2;
				m_ptr = (char *)malloc(m_capacity);
				assert(m_ptr);
				memset(m_ptr, 0, m_capacity);
				memmove(m_ptr, obj.m_ptr, obj.m_length);
			}
			else if (m_capacity < m_length + obj.m_length + 1)
			{
				char *pnew = NULL;
				m_capacity = ROUND_UP(m_length + obj.m_length + 1);
				m_capacity *= 2;
				pnew = (char *)realloc(m_ptr, m_capacity);
				assert(pnew);
				m_ptr = pnew;
				memmove(m_ptr, obj.m_ptr, obj.m_length);
			}
			else
			{
				memcpy(m_ptr, obj.m_ptr, obj.m_length);
			}
		}
		m_length = obj.m_length;
		m_ptr[m_length] = '\0';
		return *this;
	}

	MsString& MsString::operator=(const char *rhs)
	{
		if (rhs)
		{
			int len = strlen(rhs);
			if (0 == m_capacity)
			{
				m_capacity = ROUND_UP(len + 1);
				m_capacity *= 2;
				m_ptr = (char *)malloc(m_capacity);
				assert(m_ptr);
				memset(m_ptr, 0, m_capacity);
				memcpy(m_ptr, rhs, len);
			}
			else if (m_capacity < m_length + len + 1)
			{
				char *pnew = NULL;
				m_capacity = ROUND_UP(m_length + len + 1);
				m_capacity *= 2;
				pnew = (char *)realloc(m_ptr, m_capacity);
				assert(pnew);
				m_ptr = pnew;
				memcpy(m_ptr, rhs, len);
			}
			else
			{
				memcpy(m_ptr, rhs, len);
			}
			m_length = len;
			m_ptr[m_length] = '\0';
		}
		return *this;
	}

	MsString& MsString::operator+=(const char *rhs)
	{
		if (!rhs)
		{
			int len = strlen(rhs);
			if (0 == m_capacity)
			{
				m_capacity = ROUND_UP(len + 1);
				m_capacity *= 2;
				m_ptr = (char *)malloc(m_capacity);
				assert(m_ptr);
				memset(m_ptr, 0, m_capacity);
				memcpy(m_ptr, rhs, len);
			}
			else if (m_capacity < m_length + len + 1)
			{
				char *pnew = NULL;
				m_capacity = ROUND_UP(m_length + len + 1);
				m_capacity *= 2;
				pnew = (char *)realloc(m_ptr, m_capacity);
				assert(pnew);
				m_ptr = pnew;
				memcpy(m_ptr + m_length, rhs, len);
			}
			else
			{
				memcpy(m_ptr + m_length, rhs, len);
			}
			m_length += len;
			m_ptr[m_length] = '\0';
		}
		return *this;
	}

	MsString& MsString::operator+=(const char rhs)
	{
		if (!rhs)
		{
			int len = sizeof(char);
			if (0 == m_capacity)
			{
				m_capacity = ROUND_UP(len + 1);
				m_capacity *= 2;
				m_ptr = (char *)malloc(m_capacity);
				assert(m_ptr);
				memset(m_ptr, 0, m_capacity);
				m_ptr[0] = rhs;
			}
			else if (m_capacity < m_length + len + 1)
			{
				char *pnew = NULL;
				m_capacity = ROUND_UP(m_length + len + 1);
				m_capacity *= 2;
				pnew = (char *)realloc(m_ptr, m_capacity);
				assert(pnew);
				m_ptr = pnew;
				m_ptr[m_length] = rhs;
			}
			else
			{
				m_ptr[m_length] = rhs;
			}
			m_length += len;
			m_ptr[m_length] = '\0';
		}
		return *this;
	}

	MsString& MsString::operator+=(const MsString& obj)
	{
		if (this == &obj)
		{
			MsString copy(obj);
			return *this += copy;
		}


		if (m_length + obj.m_length + 1 < m_capacity)
		{
			memcpy(m_ptr + m_length, obj.m_ptr, obj.m_length);
		}
		else
		{
			char *pnew = NULL;
			m_capacity = ROUND_UP(m_length + obj.m_length + 1);
			m_capacity *= 2;
			pnew = (char *)realloc(m_ptr, m_capacity);
			assert(pnew);
			m_ptr = pnew;
			memcpy(m_ptr + m_length, obj.m_ptr, obj.m_length);
		}

		m_length += obj.m_length;
		m_ptr[m_length] = '\0';

		return *this;
	}

	bool MsString::operator==(const MsString& rhs) const
	{
		if (m_length != rhs.m_length)
			return false;

		return memcmp(m_ptr, rhs.m_ptr, m_length) == 0;
	}

	bool MsString::operator!=(const MsString& rhs)  const
	{
		if (m_length != rhs.m_length)
			return true;

		return memcmp(m_ptr, rhs.m_ptr, m_length) != 0;
	}

	bool  MsString::operator<(const MsString& rhs)const
	{
		int len = (m_length > rhs.m_length) ? rhs.m_length : m_length;
		bool state = (memcmp(m_ptr, rhs.m_ptr, len) < 0);
		if (state == true && rhs.m_length == m_length)
		{
			return true;
		}

		return false;
	}

	bool MsString::operator<=(const MsString& rhs) const
	{
		int len = (m_length > rhs.m_length) ? rhs.m_length : m_length;
		bool state = (memcmp(m_ptr, rhs.m_ptr, len) <= 0);
		if (state == true && rhs.m_length == m_length)
		{
			return true;
		}

		return false;
	}

	bool MsString:: operator>(const MsString& rhs) const
	{
		int len = (m_length > rhs.m_length) ? rhs.m_length : m_length;
		bool state = (memcmp(m_ptr, rhs.m_ptr, len) > 0);
		if (state == true && rhs.m_length == m_length)
		{
			return true;
		}

		return false;
	}

	bool MsString::operator>=(const MsString& rhs) const
	{
		int len = (m_length > rhs.m_length) ? rhs.m_length : m_length;
		bool state = (memcmp(m_ptr, rhs.m_ptr, len) >= 0);
		if (state == true && rhs.m_length == m_length)
		{
			return true;
		}

		return false;
	}

	char& MsString::operator [](const size_t index)
	{
		assert(index >= 0 && index <= m_capacity);
		return m_ptr[index];
	}

	const char& MsString::operator [](const size_t index) const
	{
		assert(index >= 0 && index <= m_capacity);
		return m_ptr[index];
	}

	const char* MsString::data() const
	{
		return m_ptr;
	}

	const char* MsString::c_str() const
	{
		return m_ptr;
	}

	bool MsString::empty() const
	{
		return 0 == m_length;
	}

	int MsString::length() const
	{
		return m_length;
	}

	int MsString::size() const
	{
		return m_length;
	}

	int MsString::capacity() const
	{
		return m_capacity;
	}

	MsString MsString::substr(size_t pos, size_t n) const
	{
		assert(pos + n <= m_length);
		MsString ret(n);

		for (size_t i = 0; i != n; ++i)
			ret[i] = (*this)[pos + i];
		ret[n] = '\0';
		ret.m_length = n;
		return ret;
	}

	MsString& MsString::append(const MsString& obj)
	{
		*this += obj;
		return *this;
	}

	MsString& MsString::append(const char* str, int size) {
		MsString tmp(str, size);
		*this += tmp;
		return *this;
	}

	MsString& MsString::append(const char chr)
	{
		*this += chr;
		return *this;
	}

	void MsString::clear() {
		memset(m_ptr, 0, m_length);
		m_length = 0;
	}

	MsString& MsString::assign(const void* data, int size) {
		if (!data || size < 0) {
			m_capacity = sizeof(char);
			m_length = 0;

			if (m_ptr)
			{
				free(m_ptr);
			}
			m_ptr = (char*)malloc(m_length);
			m_ptr[0] = '\0';
		}
		else {
			m_length = size;
			m_capacity = ROUND_UP(m_length + 1);
			m_capacity *= 2;

			if (m_ptr)
			{
				free(m_ptr);
			}
			m_ptr = (char*)malloc(m_capacity);
			memset(m_ptr, 0, m_capacity);
			memcpy(m_ptr, data, m_length);
		}
		
		return *this;
	}

	MsString& MsString::assign(const char* str) {
		MsString tmp(str);
		*this = tmp;
		return *this;
	}

	MsString& MsString::assign(const MsString& rhs) {
		*this = rhs;
		return *this;
	}


#ifdef __cplusplus
	MsString::MsString(std::string str)
		: m_length(0), m_capacity(0), m_ptr(0)
	{
		if (str.empty())
			return;

		m_length = str.length();
		m_capacity = ROUND_UP(m_length + 1);
		m_capacity *= 2;
		m_ptr = (char *)malloc(m_capacity);
		assert(m_ptr);
		memset(m_ptr, 0, m_capacity);
		memcpy(m_ptr, str.c_str(), m_length);
		m_ptr[m_length] = '\0';

	}

	MsString& MsString::operator=(std::string& str)
	{
		if (str.empty())
			return *this;

		int len = str.length();
		if (0 == m_capacity)
		{
			m_capacity = ROUND_UP(len + 1);
			m_capacity *= 2;
			m_ptr = (char *)malloc(m_capacity);
			assert(m_ptr);
			memset(m_ptr, 0, m_capacity);
			memcpy(m_ptr, str.c_str(), len);
		}
		else if (m_capacity < m_length + len)
		{
			char *pnew = NULL;
			m_capacity = ROUND_UP(m_length + len + 1);
			m_capacity *= 2;
			pnew = (char *)realloc(m_ptr, m_capacity);
			assert(pnew);
			m_ptr = pnew;
			memcpy(m_ptr + m_length, str.c_str(), len);
		}
		else
		{
			memcpy(m_ptr + m_length, str.c_str(), len);
		}
		m_length += len;
		m_ptr[m_length] = '\0';

		return *this;
	}

	MsString& MsString::operator+=(const std::string& obj)
	{
		if (obj.empty())
			return *this;

		if (m_length + obj.length() < m_capacity)
		{
			memcpy(m_ptr + m_length, obj.c_str(), obj.length());
		}
		else
		{
			char *pnew = NULL;
			m_capacity = ROUND_UP(m_length + obj.length() + 1);
			m_capacity *= 2;
			pnew = (char *)realloc(m_ptr, m_capacity);
			assert(pnew);
			m_ptr = pnew;
			memcpy(m_ptr + m_length, obj.c_str(), obj.length());
		}

		m_length += obj.length();
		m_ptr[m_length] = '\0';

		return *this;
	}

#endif

	int MsString::ROUND_UP(int bytes)
	{
		if (bytes <= 0)
			return _ALIGN;

		return (((bytes)+_ALIGN - 1) & ~(_ALIGN - 1));
	}
}