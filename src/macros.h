#ifdef __LINUX__
#include <cstring>
#endif

#define __STRINGIZE__(...) #__VA_ARGS__

#ifdef __WINDOWS__
#define __RUNNING_PLATFORM__ "Windows"
#elif defined __LINUX__
#define __RUNNING_PLATFORM__ "Linux"
#endif

#if defined __DEBUG__
#define __BUILD_MODE__ "Debug"
#else
#define __BUILD_MODE__ "Release"
#ifndef __RELEASE__
#define __RELEASE__
#endif
#endif

#ifdef __WINDOWS__
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif


#pragma warning(disable:4250)
