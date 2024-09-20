#ifndef COMMON_HH
#define COMMON_HH

#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <iomanip>
#include <iterator>
#include <stdexcept>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cassert>
#include <string>
#include <string.h>
#include <stdlib.h>

#ifndef WIN32
#include <unistd.h>
#endif

// TODO deuglify
#ifdef _WIN32
#define snprintf _snprintf
#endif

namespace std
{
	template <class T>
		std::ostream& operator<< (std::ostream& os, const std::vector<T>& container)
		{
			std::copy( container.begin(), container.end(), std::ostream_iterator<T>( os, "; " ) );
			return os;
		}

	template <class T, class U>
		std::ostream& operator<< (std::ostream& os, const std::pair<T,U>& pair)
		{
			os << pair.first << ":" << pair.second;
			return os;
		}


	template <class T, class U>
		std::ostream& operator<< (std::ostream& os, const std::map<T,U>& container)
		{
			std::copy( container.begin(), container.end(), std::ostream_iterator<typename std::map<T,U>::value_type>( os, "; " ) );
			return os;
		}

}

namespace xigen
{

		template <class T>
		std::string quickFromStream (const T& t)
		{
			std::ostringstream oss;
			oss << t;
			return oss.str();
		}

#define XI_ASSERT(Expr1) xigen::assert_true((Expr1), __FILE__, __LINE__)
template <class T>
void assert_true (const T& left, const char* file, unsigned line)
{
	if (!(bool)left)
	{
		std::ostringstream oss;
		oss << "assertTrue at " << file << ":" << line << ": " << (left ? "true" : "false") << " - " << left;
		throw std::runtime_error(oss.str());
	}
}

inline void stream_copy (std::istream& is, std::ostream& os)
{
		std::copy(
				std::istreambuf_iterator<char>(is),
				std::istreambuf_iterator<char>(),
				std::ostreambuf_iterator<char>(os)
		);
}

class Noncopyable
{
public:
	Noncopyable() { }
private:
	Noncopyable(const Noncopyable&);
	Noncopyable& operator=(const Noncopyable&);
};

class Printable
{
public:
	virtual std::string toString() const = 0;
};

template <class Cont>
void clear_ptr_container(Cont& cont)
{
	for (typename Cont::const_iterator it = cont.begin(); it != cont.end(); ++it)
		delete *it;
	cont.clear();
}

inline std::ostream& operator<< (std::ostream& os, const Printable& printable)
{
	os << printable.toString() << std::endl;
	return os;
}

template <class T>
std::string toString (const T& t)
{
	std::ostringstream oss;
	oss << t;
	return oss.str();
}

inline std::string unquote(const std::string& s)
{
	return s.size() >= 2 ? s.substr(1,s.size()-2) : s;
}

	inline bool withHead (const std::string& s, const std::string& head)
	{
		return head.size() < s.size() && s.substr( 0, std::min(s.size(), head.size()) ) == head;
	}

	inline bool withTail (const std::string& s, const std::string& tail)
	{
		return s.size() >= tail.size() && s.substr( s.size() - tail.size() ) == tail;
	}

	inline std::string dropHead (const std::string& s, const std::string& head)
	{
		return withHead( s, head )
			? s.substr( head.size() )
			: s;
	}

	inline std::string dropTail (const std::string& s, const std::string& tail)
	{
		return withTail( s, tail )
			? s.substr( 0, s.size() - tail.size() )
			: s;
	}

	inline std::string toupper (const std::string& s)
	{
		std::string result(s.size(), ' ');
		std::transform(s.begin(), s.end(), result.begin(), ::toupper);
		return result;
	}

	inline std::string tolower (const std::string& s)
	{
		std::string result(s.size(), ' ');
		std::transform(s.begin(), s.end(), result.begin(), ::tolower);
		return result;
	}

	inline std::string join (const std::vector<std::string>& slist, const std::string& delim)
	{
		if (slist.empty())
			return "";
		std::string result;
		for (size_t i = 0; i < slist.size()-1; ++i)
			result += slist[i] + delim;
		result += slist.back();
		return result;
	}

	inline void trim (std::string& s, const std::string& trails)
	{
		size_t ind = s.find_first_not_of(trails);
		if (ind != std::string::npos)
			s.erase(0, ind);
		ind = s.find_last_not_of(trails);
		if (ind != std::string::npos)
			s.erase(ind+1);
		else
			s.clear();
	}

	inline std::string trim_copy (const std::string& s, const std::string& trails)
	{
		std::string result = s;
		trim(result, trails);
		return result;
	}

	inline void replace_all(std::string& s, const std::string& from, const std::string& to)
	{
		if (from.empty())
			return;
		for (size_t start_pos = 0;
				(start_pos = s.find(from, start_pos)) != std::string::npos;
				start_pos += to.length())
			s.replace(start_pos, from.length(), to);
	}


	inline void parseToList (const std::string& source, const std::string& delims,
			const std::string& trails, std::vector<std::string>& strings)
	{
		strings.clear();
		for (int pos = 0, lastPos = 0; pos != -1; )
		{
			pos = source.find_first_of( delims, pos+1 );
			std::string next = source.substr( lastPos, pos == -1 ? -1 : pos-lastPos );
			trim( next, trails );
			if (!next.empty())
				strings.push_back( next );
			lastPos = pos ? pos + 1 : 0;
		}
	}



}

#endif


/* vim: set ts=2 sw=2: */
