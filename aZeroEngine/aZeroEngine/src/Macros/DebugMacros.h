#pragma once

#ifdef _DEBUG
#include <iostream>

namespace aZero
{
	namespace Debug
	{
		// TODO - Change these all so that they can be compiled with in release mode but then removed. AKA move the #ifdef clause into the macro.
		#define FATAL_LOG(msg) std::cout << "FATAL ERROR: " + msg + "\n";
		#define WARNING_LOG(msg) std::cout << "WARNING: " + msg + "\n";
	}
}

#endif