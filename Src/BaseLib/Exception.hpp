#ifndef baseLib_exception_h
#define baseLib_exception_h

/**
 * Contains class for exception handling. 
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standart library includes
///
#include <string>
#include <stdexcept>

namespace BaseLib {

/**
 * Library specific exception.
 */
class Exception : public std::exception
{
public:
	/**
	 * @param[in] message Exception message.
	 */
	Exception(const std::string& message = "");
	~Exception() throw();
};

/**
 * Class for exception holding. Used to 
 * transfer exception between modules.
 */
class ExceptionHolder
{
public:
	ExceptionHolder();
public:
	/**
	 * Capture exception.
	 * @param[in] ex Standart exception.
	 */
	void operator() (std::exception& ex);
	/**
	 * Capture exception.
	 * @param[in] ex Library specific exception.
	 */
	void operator() (Exception& ex);
public:
	/**
	 * Throw stored exception. If no exception is stored
	 * do nothing.
	 */
	void Rethrow();
	/**
	 * Check whether the holder contains an exception.
	 * @return true if contains
	 */
	bool ContainsException() const;
private:
	enum eMode {
		None	/*!< No exception. */
		,Std	/*!< Standart exception. */ 
		,Eny	/*!< Library specific exception. */
	};
private:
	/**
	 * Active stored exception.
	 */
	eMode mMode;
	/**
	 * Any exception.
	 */
	Exception mEx;
	/**
	 * Std exception.
	 */
	std::exception mStdEx;	
};

// Because of inlining
inline void ExceptionHolder::Rethrow()
{
	switch(mMode)
	{
	case Std:
		throw mStdEx;
	case Eny:
		throw mEx;
	case None:
	default:
		return;
	}
}

// Inline funcions
///

inline bool ExceptionHolder::ContainsException() const
{	// if mMode == None we dont have an exception
	return !(mMode == None);
}


} // baseLib_exception_h

#endif