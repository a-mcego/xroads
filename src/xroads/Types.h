#pragma once

#if defined(__STDCPP_BFLOAT16_T__)
//#define XROADS_BFLOAT16
#endif // defined

#include <cstdint>
#if defined(XROADS_BFLOAT16)
#include <stdfloat>
#endif

namespace Xroads
{
    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;
    using f32 = float;
    using f64 = double;
#if defined(XROADS_BFLOAT16)
    using bf16 = std::bfloat16_t;
#define BF16_LIT(x) x ## bf16
#else
    using bf16 = float;
#define BF16_LIT(x) x ## f
#endif

}


