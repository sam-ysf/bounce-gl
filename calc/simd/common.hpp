#pragma once

#include <immintrin.h>

namespace calc {

    namespace detail {

        template <typename>
        struct M;

        template <>
        struct M<float> {
            using T = __m128;
        };

        template <typename U>
        inline typename detail::M<U>::T load(const U*);

        template <>
        inline __m128 load(const float* dat)
        {
            return _mm_load_ps(dat);
        }

        template <typename U>
        void store(U*, typename detail::M<U>::T);

        template <>
        inline void store(float* dat, __m128 fill)
        {
            _mm_store_ps(dat, fill);
        }
    } // namespace detail
} // namespace calc
