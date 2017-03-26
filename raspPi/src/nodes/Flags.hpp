#pragma once
#include "../util/BitmaskEnumClass.h"

enum class FlagBitType :uint32_t { hide, readonly, logging, persist, query, numflags, invalidFlag = numflags };

#define MAKE_FLAG_VALUE(flagBit) flagBit = (1 << (uint32_t)FlagBitType::flagBit)
enum class FlagType :uint32_t
{ 
    none = 0,
    MAKE_FLAG_VALUE(hide),
    MAKE_FLAG_VALUE(logging),
    MAKE_FLAG_VALUE(readonly),
    MAKE_FLAG_VALUE(persist),
    MAKE_FLAG_VALUE(query),
    all = ((1U << (uint32_t)FlagBitType::numflags) - 1U)

};
#undef MAKE_FLAG_VALUE
ENABLE_BITMASK_OPERATORS(FlagType) // Supply overloaded bitwise operators for FlagType.
