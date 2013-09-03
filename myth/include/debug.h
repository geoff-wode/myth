#if ! defined(__DEBUG_H__)
#define __DEBUG_H__

#include <stdio.h>
#include <stdarg.h>

namespace debug
{
  extern FILE* logFile;
}

#define LOG(msg, ...) \
	do \
	{ \
		fprintf(debug::logFile, msg, __VA_ARGS__); \
	} while (0)

#define ASSERT(pred) ASSERTM(pred, "")

#define ASSERTM(pred, msg, ...) \
  do \
  { \
    if (!(pred)) \
    { \
      LOG("%s:%d:%s - %s", #pred, msg, __VA_ARGS__); \
      assert(0); \
    } \
  } while (0)

#endif // __DEBUG_H__

