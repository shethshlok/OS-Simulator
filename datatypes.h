// protect from multiple compiling
#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdio.h>
#include <stdbool.h>
#include "StandardConstants.h"

// GLOBAL CONSTANTS - may be used in other files

typedef enum { CMD_STR_LEN = 5,
               IO_ARG_STR_LEN = 5,
               STR_ARG_LEN = 15 } OpCodeArrayCapacity;

// five state data structure process
typedef enum { NEW_STATE,
               READY_STATE,
               RUNNING_STATE,
               BLOCKED_STATE,
               EXIT_STATE } ProcessState;

typedef enum { MEM_INIT,
               MEM_CLEAR,
               MEM_ALLOC_SUCC,
               MEM_CLEAR_ALL } MemoryDisplayCodes;


#endif

