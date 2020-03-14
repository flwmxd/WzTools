//////////////////////////////////////////////////////////////////////////////
// This file is part of the PharaohStroy MMORPG client                      // 
// Copyright ?2016-2017 Prime Zeng                                          // 
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

#include "WzKey.h"
#include "SimdChecker.h"

namespace WzKey{

	auto init() -> void 
	{
        if(isSimdAvailable)
        {
            #ifdef __SSE__
                    __m128i amask = _mm_setr_epi8(
                        0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9
                    );
                    __m128i aplus = _mm_set1_epi8(0x10);

                    __m128i wmask = _mm_setr_epi16(
                        0xAAAA, 0xAAAB, 0xAAAC, 0xAAAD, 0xAAAE, 0xAAAF, 0xAAB0, 0xAAB1
                    );
                    __m128i wplus = _mm_set1_epi16(0x0008);

                    __m128i *wk0 = (__m128i *)emsWzWideKey;
                    __m128i *ak0 = (__m128i *)emsWzNormalKey;
                    __m128i *k0 = (__m128i *)emsWzKey;

                    for (int32_t i = 0x1000; i > 0; --i)
                    {
                        _mm_storeu_si128(ak0, _mm_xor_si128(amask, _mm_loadu_si128(k0)));
                        _mm_storeu_si128(wk0, _mm_xor_si128(wmask, _mm_loadu_si128(k0)));
                        ++ak0;
                        ++wk0;
                        ++k0;
                        amask = _mm_add_epi8(amask, aplus);
                        wmask = _mm_add_epi8(wmask, wplus);
                    }
            #elif defined(__ARM_NEON__)
                    
                    uint8_t __attribute__((aligned(16))) magic1[16] =
                      {
                        0xAA, 0xAB, 0xAC, 0xAD,
                        0xAE, 0xAF, 0xB0, 0xB1,
                        0xB2, 0xB3, 0xB4, 0xB5,
                        0xB6, 0xB7, 0xB8, 0xB9
                     };

                    uint16_t __attribute__((aligned(16))) magic2[8] = {
                        0xAAAA, 0xAAAB, 0xAAAC, 0xAAAD,
                        0xAAAE, 0xAAAF, 0xAAB0, 0xAAB1
                    };


                    auto amask = (__m128i) vld1q_s8((int8_t*)magic1);
                    auto aplus = vreinterpretq_m128i_s8(vdupq_n_s8(0x10));


                    auto wmask = vreinterpretq_m128i_s16(vld1q_s16((int16_t *)magic2));
                    auto wplus = vreinterpretq_m128i_s16(vdupq_n_s16(0x0008));


                    auto wk0 = (__m128i *)emsWzWideKey;
                    auto ak0 = (__m128i *)emsWzNormalKey;
                    auto k0 = (__m128i *)emsWzKey;


            #  if defined(__arm64__) || defined(__aarch64__) // NEON6

                    //FORCE_INLINE __m128i _mm_xor_si128(__m128i a, __m128i b)
                    //{
                    //    return vreinterpretq_m128i_s32(veorq_s32(vreinterpretq_s32_m128i(a), vreinterpretq_s32_m128i(b)));
                    //}
                    for (int32_t i = 0x1000; i > 0; --i)
                    {
                          // veorq_u64(uint64x2_t __p0, uint64x2_t __p1)
                      auto v = vreinterpretq_m128i_s64(vld1q_s64((const int64_t *)k0));

                      vst1q_s64((int64_t *)ak0, veorq_s64(amask, v));
                      vst1q_s64((int64_t *)wk0, veorq_s64(wmask, v));
                      ++ak0;
                      ++wk0;
                      ++k0;
                      amask = vreinterpretq_m128i_s8(vaddq_s8(vreinterpretq_s8_m128i(amask), vreinterpretq_s8_m128i(aplus)));
                      wmask = vreinterpretq_m128i_s8(vaddq_s8(vreinterpretq_s8_m128i(wmask), vreinterpretq_s8_m128i(wplus)));
                    }

            #else

                  for (int32_t i = 0x1000; i > 0; --i)
                  {
                      // veorq_u64(uint64x2_t __p0, uint64x2_t __p1)
                      auto v = vreinterpretq_m128i_s32(vld1q_s32((const int32_t *)k0));

                      vst1q_s32((int32_t *)ak0, veorq_s32(amask, v));
                      vst1q_s32((int32_t *)wk0, veorq_s32(wmask, v));
                      ++ak0;
                      ++wk0;
                      ++k0;
                      amask = vreinterpretq_m128i_s8(vaddq_s8(vreinterpretq_s8_m128i(amask), vreinterpretq_s8_m128i(aplus)));
                      wmask = vreinterpretq_m128i_s8(vaddq_s8(vreinterpretq_s8_m128i(wmask), vreinterpretq_s8_m128i(wplus)));
                  }
            #endif
                    
            #endif
        }
	};

};
