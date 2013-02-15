// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#ifndef stdafx_h
#define stdafx_h

// std
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

#ifndef _MSC_VER

// gcc add suport of nullptr in 4.6.0, retaken from http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2431.pdf
const class 
{
public:
  template<class T>
    operator T*() const  { return 0; }
  template<class C, class T> operator T C::*() const { return 0; }
private:
  void operator&() const;
} nullptr = {};

namespace std 
{
	// std::begin	
	template<class T> typename T::iterator begin(T& data) { return data.begin(); };
	template<class T> typename T::const_iterator begin(const T& data) { return data.cbegin(); };
	// std::end
	template<class T> typename T::iterator end(T& data) { return data.end(); };
	template<class T> typename T::const_iterator end(const T& data) { return data.cend(); };
}

#endif

#endif 
