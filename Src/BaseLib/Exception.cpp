#include "stdafx.h"
// ..
#include "Exception.hpp"


namespace BaseLib {

Exception::Exception(const std::string& message)
	: std::exception(message.c_str())
{ }

Exception::~Exception()
{ }

//								ExceptionHolder							//

ExceptionHolder::ExceptionHolder()
	: mMode(None)
{ }

void ExceptionHolder::operator() (std::exception& ex)
{
	mMode = Std;
	mStdEx = ex;
}

void ExceptionHolder::operator() (Exception& ex)
{
	mMode = Eny;
	mEx = ex;
}

}