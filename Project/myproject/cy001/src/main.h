#ifndef _MAIN_H_
#define _MAIN_H_

#include "common.h"

#define DEBUG_LOG

#ifdef DEBUG_LOG
#define LOG(format,args...)  printf(format,##args)
#else //DEBUG_LOG
#define LOG(format,args...)
#endif //DEBUG_LOG

#endif //_MAIN_H_
