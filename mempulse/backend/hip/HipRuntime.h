#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4201)
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#include <hip/hip_runtime.h>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif
