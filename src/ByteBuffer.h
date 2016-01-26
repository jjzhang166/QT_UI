/* 
* Copyright (C) 2005,2006,2007 MaNGOS <http://www.mangosproject.org/>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _BYTEBUFFER_H
#define _BYTEBUFFER_H

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <assert.h>
#include <string>
#include <stdio.h>
#include <stdint.h>

#define WPAssert( assertion ) { if( !(assertion) ) { fprintf( stderr, "\n%s:%i ASSERTION FAILED:\n  %s\n", __FILE__, __LINE__, #assertion ); assert( #assertion &&0 ); } }
#define WPError( assertion, errmsg ) if( ! (assertion) ) { sLog.outError( "%s:%i ERROR:\n  %s\n", __FILE__, __LINE__, (char *)errmsg ); assert( false ); }
#define WPWarning( assertion, errmsg ) if( ! (assertion) ) { sLog.outError( "%s:%i WARNING:\n  %s\n", __FILE__, __LINE__, (char *)errmsg ); }

#define WPFatal( assertion, errmsg ) if( ! (assertion) ) { sLog.outError( "%s:%i FATAL ERROR:\n  %s\n", __FILE__, __LINE__, (char *)errmsg ); assert( #assertion &&0 ); abort(); }

#define ASSERT WPAssert

#define PRINTF printf
#define TRACE0_L0 printf
#define TRACE1_L0 printf
#define TRACE2_L0 printf
#define TRACE3_L0 printf
#define TRACE4_L0 printf
#define TRACE5_L0 printf

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
class ByteBuffer
{
public:
	const static size_t DEFAULT_SIZE = 0x2000;

	// constructor
	ByteBuffer(): _rpos(0), _wpos(0)
	{
		_storage.reserve(DEFAULT_SIZE);
	}
	// constructor
	ByteBuffer(size_t res): _rpos(0), _wpos(0)
	{
		_storage.reserve(res);
	}
	// constructor
	ByteBuffer(uint8* pBuf, size_t len, size_t res): _rpos(0), _wpos(0)
	{
		_storage.reserve(res);
		append(pBuf, len);
	}
	// copy constructor
	ByteBuffer(const ByteBuffer &buf): _rpos(buf._rpos), _wpos(buf._wpos), _storage(buf._storage) { }

	void clear()
	{
		_storage.clear();
		_rpos = _wpos = 0;
	}
	inline void reset()
	{
		_rpos = _wpos = 0;
	}

	template <typename T> void append(T value)
	{
		append((uint8 *)&value, sizeof(value));
	}
	template <typename T> void put(size_t pos,T value)
	{
		put(pos,(uint8 *)&value,sizeof(value));
	}

	ByteBuffer &operator<<(bool value)
	{
		append<char>((char)value);
		return *this;
	}
	ByteBuffer &operator<<(uint8 value)
	{
		append<uint8>(value);
		return *this;
	}
	ByteBuffer &operator<<(uint16 value)
	{
		append<uint16>(value);
		return *this;
	}
	ByteBuffer &operator<<(uint32 value)
	{
		append<uint32>(value);
		return *this;
	}
	ByteBuffer &operator<<(uint64 value)
	{
		append<uint64>(value);
		return *this;
	}

	// signed as in 2e complement
	ByteBuffer &operator<<(int8 value)
	{
		append<int8>(value);
		return *this;
	}
	ByteBuffer &operator<<(int16 value)
	{
		append<int16>(value);
		return *this;
	}
	ByteBuffer &operator<<(int32 value)
	{
		append<int32>(value);
		return *this;
	}
	ByteBuffer &operator<<(int64 value)
	{
		append<int64>(value);
		return *this;
	}
	ByteBuffer &operator<<(long value)
	{
		append<long>(value);
		return *this;
	}
	ByteBuffer &operator<<(void* value)
	{
		append<void*>(value);
		return *this;
	}
	// floating points
	ByteBuffer &operator<<(float value)
	{
		append<float>(value);
		return *this;
	}
	ByteBuffer &operator<<(double value)
	{
		append<double>(value);
		return *this;
	}
	ByteBuffer &operator<<(const std::string &value)
	{
		append<ushort>(value.length());
		append((uint8 *)value.c_str(), value.length());
		return *this;
	}
	ByteBuffer &operator<<(const char *str)
	{
		if (str)	
		{
			append((short)strlen(str));
			append((uint8 *)str, strlen(str));
		}
		else
		{
			append((short)0);
		}
		return *this;
	}
/*
	const ByteBuffer &operator>>(char** value) const
	{
		if (*value)
		{
			delete[] (*value);
		}
		size_t len=read<size_t>();
		size_t count = 0;
		char* str = new char[len + 1];
		while (count < len)
		{
			char c=read<char>();
			if (c==0)
				break;
			str[count] = c;
			++count;
		}
		str[len] = 0;
		*value = str;
		return *this;
	}
*/
	const ByteBuffer &operator>>(bool &value) const
	{
		value = read<char>() > 0 ? true : false;
		return *this;
	}
	const ByteBuffer &operator>>(uint8 &value) const
	{
		value = read<uint8>();
		return *this;
	}
	const ByteBuffer &operator>>(uint16 &value) const
	{
		value = read<uint16>();
		return *this;
	}
	const ByteBuffer &operator>>(uint32 &value) const
	{
		value = read<uint32>();
		return *this;
	}
	const ByteBuffer &operator>>(uint64 &value) const
	{
		value = read<uint64>();
		return *this;
	}

	//signed as in 2e complement
	const ByteBuffer &operator>>(int8 &value) const
	{
		value = read<int8>();
		return *this;
	}
	const ByteBuffer &operator>>(int16 &value) const
	{
		value = read<int16>();
		return *this;
	}
	const ByteBuffer &operator>>(int32 &value) const
	{
		value = read<int32>();
		return *this;
	}
	const ByteBuffer &operator>>(int64 &value) const
	{
		value = read<int64>();
		return *this;
	}
	const ByteBuffer &operator>>(long &value) const
	{
		value = read<long>();
		return *this;
	}
	const ByteBuffer &operator>>(void* &value) const
	{
		value = read<void*>();
		return *this;
	}
	const ByteBuffer &operator>>(float &value) const
	{
		value = read<float>();
		return *this;
	}
	const ByteBuffer &operator>>(double &value) const
	{
		value = read<double>();
		return *this;
	}
	const ByteBuffer &operator>>(std::string& value) const
	{
		value.clear();
		unsigned short len = read<short>();
		if (len > 0)
		{
			if (len > size() - _rpos)
			{
				value = "error";
#ifdef WIN32
				TRACE0_L0("[ByteBuffer::operator>>(std::string& value)] fatal error: stream buffer out of size\n");
#endif // WIN32
			}		
			else
			{
				if (len < 512)
				{
					char buff[512];
					read((uint8*)buff, len);
					value.append(buff, len);
				}
				else if (len < 2048)
				{
					char buff[2048];
					read((uint8*)buff, len);
					value.append(buff, len);
				}
				else
				{
					char buff[65535];
					read((uint8*)buff, len);
					value.append(buff, len);
				}
			}		
		}		
		return *this;
	}

	uint8 operator[](size_t pos) const
	{
		return read<uint8>(pos);
	}

	size_t rpos() const
	{
		return _rpos;
	};


	int getWrong() const
	{
		return wrong;
	};

	size_t rpos(size_t rpos)
	{
		_rpos = rpos;
		return _rpos;
	};

	size_t wpos() const
	{
		return _wpos;
	}

	size_t wpos(size_t wpos)
	{
		_wpos = wpos;
		return _wpos;
	}

	template <typename T> T read() const
	{
		T r=read<T>(_rpos);
		_rpos += sizeof(T);
		return r;
	};
	template <typename T> T read(size_t pos) const
	{
		//ASSERT(pos + sizeof(T) <= size() || PrintPosError(false,pos,sizeof(T)));
		if (pos + sizeof(T) > size() )
		{
#ifdef WIN32
			TRACE0_L0("[ByteBuffer::read] fatal error: stream buffer our of size\n");
#endif // WIN32
			return (T)0;
		}
		return *((T*)&_storage[pos]);
	}

	void read(uint8 *dest, size_t len) const
	{
		ASSERT(_rpos  + len  <= size() || PrintPosError(false,_rpos,len));
		memcpy(dest, &_storage[_rpos], len);
		_rpos += len;
	}

    void read(ByteBuffer& buffer, size_t len) const
    {
        ASSERT(_rpos  + len  <= size() || PrintPosError(false,_rpos,len));
        buffer.append(&_storage[_rpos], len);
        _rpos += len;
    }
	const uint8 *contents() const { return &_storage[0]; };

	inline size_t size() const { return _storage.size(); };

	void resize(size_t newsize)
	{
		_storage.resize(newsize);
		_rpos = 0;
		_wpos = size();
	};
	void reserve(size_t ressize)
	{
		if (ressize > size()) _storage.reserve(ressize);
	};
	void append(const std::string& str)
	{
		append((uint8 *)str.c_str(),str.size() + 1);
	}
	void append(const char *src, size_t cnt)
	{
		return append((const uint8 *)src, cnt);
	}
	void append(const uint8 *src, size_t cnt)
	{
		if (!cnt) return;
		if (_storage.size() < _wpos + cnt)
			_storage.resize(_wpos + cnt);
		memcpy(&_storage[_wpos], src, cnt);
		_wpos += cnt;
	}
	void append(const ByteBuffer& buffer)
	{
		if(buffer.size()) append(buffer.contents(),buffer.size());
	}
	void append(const ByteBuffer& buffer, size_t pos)
	{
		if(buffer.size() && (buffer.size() - pos) > 0) append(&(buffer._storage[pos]),buffer.size()-pos);
	}
	void put(size_t pos, const uint8 *src, size_t cnt)
	{
		ASSERT(pos + cnt <= size() || PrintPosError(true,pos,cnt));
		memcpy(&_storage[pos], src, cnt);
	}
	void print_storage() const
	{
		PRINTF("STORAGE_SIZE: %u\n", size() );
		for(uint32 i = 0; i < size(); i++)
			PRINTF("%u - ", read<uint8>(i) );
		PRINTF("\n");
	}

	void textlike() const
	{
		PRINTF("STORAGE_SIZE: %u\n", size() );
		for(uint32 i = 0; i < size(); i++)
			PRINTF("%c", read<uint8>(i) );
		PRINTF("\n");
	}

	void hexlike() const
	{
		uint32 j = 1, k = 1;
		PRINTF("STORAGE_SIZE: %u\n", size() );
		for(uint32 i = 0; i < size(); i++)
		{
			if ((i == (j*8)) && ((i != (k*16))))
			{
				if (read<uint8>(i) <= 0x0F)
				{
					PRINTF("| 0%X ", read<uint8>(i) );
				}
				else
				{
					PRINTF("| %X ", read<uint8>(i) );
				}
				j++;
			}
			else if (i == (k*16))
			{
				if (read<uint8>(i) <= 0x0F)
				{
					PRINTF("\n0%X ", read<uint8>(i) );
				}
				else
				{
					PRINTF("\n%X ", read<uint8>(i) );
				}

				k++;
				j++;
			}
			else
			{
				if (read<uint8>(i) <= 0x0F)
				{
					PRINTF("0%X ", read<uint8>(i) );
				}
				else
				{
					PRINTF("%X ", read<uint8>(i) );
				}
			}
		}
		PRINTF("\n");
	}

protected:
	bool PrintPosError(bool add, size_t pos, size_t esize) const
	{
#ifdef WIN32
		TRACE4_L0("ERROR: Attempt %s in ByteBuffer (pos: %u size: %u) value with size: %u",(add ? "put" : "get"),pos, size(), esize);
#endif // WIN32
		// assert must fail after function call
		return false;
	}

	mutable size_t _rpos, _wpos;
	mutable int wrong;
	std::vector<uint8> _storage;
};


template <typename T> ByteBuffer &operator<<(ByteBuffer &b, std::vector<T> v)
{
    b << ((uint32)(v.size()));
	for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); i++)
	{
		b << *i;
	}
	return b;
}

template <typename T> ByteBuffer &operator>>(ByteBuffer &b, std::vector<T> &v)
{
	uint32 vsize;
	b >> vsize;
	v.clear();
	while(vsize--)
	{
		T t;
		b >> t;
		v.push_back(t);
	}
	return b;
}

template <typename T> ByteBuffer &operator<<(ByteBuffer &b, std::list<T> v)
{
	b << (uint32)v.size();
	for (typename std::list<T>::iterator i = v.begin(); i != v.end(); i++)
	{
		b << *i;
	}
	return b;
}

template <typename T> ByteBuffer &operator>>(ByteBuffer &b, std::list<T> &v)
{
	uint32 vsize;
	b >> vsize;
	v.clear();
	while(vsize--)
	{
		T t;
		b >> t;
		v.push_back(t);
	}
	return b;
}

template <typename K, typename V> ByteBuffer &operator<<(ByteBuffer &b, std::map<K, V> &m)
{
	b << (uint32)m.size();
	for (typename std::map<K, V>::iterator i = m.begin(); i != m.end(); i++)
	{
		b << i->first << i->second;
	}
	return b;
}

template <typename K, typename V> ByteBuffer &operator>>(ByteBuffer &b, std::map<K, V> &m)
{
	uint32 msize;
	b >> msize;
	m.clear();
	while(msize--)
	{
		K k;
		V v;
		b >> k >> v;
		m.insert(make_pair(k, v));
	}
	return b;
}

#endif
