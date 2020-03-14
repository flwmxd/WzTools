//////////////////////////////////////////////////////////////////////////////
// This file is part of the PharaohStroy MMORPG client                      //
// Copyright ?2020-2022 Prime Zeng                                          //
//                                                                          //
// This program is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU Affero General Public License as           //
// published by the Free Software Foundation, either version 3 of the       //
// License, or (at your option) any later version.                          //
//                                                                          //
// This program is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
// GNU Affero General Public License for more details.                      //
//                                                                          //
// You should have received a copy of the GNU Affero General Public License //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef _WIN32
#	define __SSE__
# include <emmintrin.h>
#endif

#if defined(__ANDROID__)
#  include <cpu-features.h>
#endif

#if defined(__ARM_NEON__)
#  include <arm_neon.h>

typedef int64x2_t __m128i;
#define vreinterpretq_m128_f16(x) vreinterpretq_f32_f16(x)
#define vreinterpretq_m128_f32(x) (x)
#define vreinterpretq_m128_f64(x) vreinterpretq_f32_f64(x)

#define vreinterpretq_m128_u8(x) vreinterpretq_f32_u8(x)
#define vreinterpretq_m128_u16(x) vreinterpretq_f32_u16(x)
#define vreinterpretq_m128_u32(x) vreinterpretq_f32_u32(x)
#define vreinterpretq_m128_u64(x) vreinterpretq_f32_u64(x)

#define vreinterpretq_m128_s8(x) vreinterpretq_f32_s8(x)
#define vreinterpretq_m128_s16(x) vreinterpretq_f32_s16(x)
#define vreinterpretq_m128_s32(x) vreinterpretq_f32_s32(x)
#define vreinterpretq_m128_s64(x) vreinterpretq_f32_s64(x)

#define vreinterpretq_f16_m128(x) vreinterpretq_f16_f32(x)
#define vreinterpretq_f32_m128(x) (x)
#define vreinterpretq_f64_m128(x) vreinterpretq_f64_f32(x)

#define vreinterpretq_u8_m128(x) vreinterpretq_u8_f32(x)
#define vreinterpretq_u16_m128(x) vreinterpretq_u16_f32(x)
#define vreinterpretq_u32_m128(x) vreinterpretq_u32_f32(x)
#define vreinterpretq_u64_m128(x) vreinterpretq_u64_f32(x)

#define vreinterpretq_s8_m128(x) vreinterpretq_s8_f32(x)
#define vreinterpretq_s16_m128(x) vreinterpretq_s16_f32(x)
#define vreinterpretq_s32_m128(x) vreinterpretq_s32_f32(x)
#define vreinterpretq_s64_m128(x) vreinterpretq_s64_f32(x)

#define vreinterpretq_m128i_s8(x) vreinterpretq_s64_s8(x)
#define vreinterpretq_m128i_s16(x) vreinterpretq_s64_s16(x)
#define vreinterpretq_m128i_s32(x) vreinterpretq_s64_s32(x)
#define vreinterpretq_m128i_s64(x) (x)

#define vreinterpretq_m128i_u8(x) vreinterpretq_s64_u8(x)
#define vreinterpretq_m128i_u16(x) vreinterpretq_s64_u16(x)
#define vreinterpretq_m128i_u32(x) vreinterpretq_s64_u32(x)
#define vreinterpretq_m128i_u64(x) vreinterpretq_s64_u64(x)

#define vreinterpretq_s8_m128i(x) vreinterpretq_s8_s64(x)
#define vreinterpretq_s16_m128i(x) vreinterpretq_s16_s64(x)
#define vreinterpretq_s32_m128i(x) vreinterpretq_s32_s64(x)
#define vreinterpretq_s64_m128i(x) (x)

#define vreinterpretq_u8_m128i(x) vreinterpretq_u8_s64(x)
#define vreinterpretq_u16_m128i(x) vreinterpretq_u16_s64(x)
#define vreinterpretq_u32_m128i(x) vreinterpretq_u32_s64(x)
#define vreinterpretq_u64_m128i(x) vreinterpretq_u64_s64(x)



#  if defined(__ANDROID__) && defined(__arm__)
    // NEON support must be checked at runtime on 32-bit Android
    class AndroidNeonChecker final
    {
    public:
        AndroidNeonChecker() noexcept:
            neonAvailable(android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM &&
                          (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0)
        {
        }

        operator bool() const noexcept { return neonAvailable; }

    private:
        bool neonAvailable;
    };

    extern AndroidNeonChecker isSimdAvailable;
#  else
    constexpr auto isSimdAvailable = true;
#  endif
#elif defined(__SSE__)
    constexpr auto isSimdAvailable = true;
#else
    constexpr auto isSimdAvailable = false;
#endif