#pragma once
#include "defines.h"
#if USE_SSE
#include "vec3_SSE.h"
#else 
#include "vec3_scalar.h"
#endif
