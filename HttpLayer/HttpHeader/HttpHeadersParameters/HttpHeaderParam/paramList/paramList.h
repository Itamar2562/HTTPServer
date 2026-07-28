#pragma once
#include <stddef.h>
#include "../parameter/parameter.h"

#define STARTING_MAX_PARAM_SIZE 3

typedef struct
{
    parameter *p;

    int param_count;
    int param_max_size;
  
} paramList;


