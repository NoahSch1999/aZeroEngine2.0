#pragma once

#ifdef _DEBUG
#define DEBUG_CHECK(statement) if(!statement) DebugBreak(); 
#else 
DEBUG_CHECK(statement)
#endif

namespace aZeroECS
{

#define MAXCOMPONENTS 32

}