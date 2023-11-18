#pragma once

#include <cstdint>
#if defined(__STDCPP_BFLOAT16_T__)
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
#if defined(__STDCPP_BFLOAT16_T__)
    using f16 = std::float16_t;
    using bf16 = std::bfloat16_t;
#define BF16_LIT(x) x ## bf16
#else
    using f16 = float;
    using bf16 = float;
#define BF16_LIT(x) x ## f
#endif

}


