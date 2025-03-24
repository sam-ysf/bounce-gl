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

        template <>
        struct M<double> {
            using T = __m128d;
        };

        template <typename U>
        inline typename detail::M<U>::T load(const U*);
        template <typename U>
        inline typename detail::M<U>::T loadu(const U*);

        template <>
        inline __m128 load(const float* dat)
        {
            return _mm_load_ps(dat);
        }

        template <>
        inline __m128d load(const double* dat)
        {
            return _mm_load_pd(dat);
        }

        template <>
        inline __m128 loadu(const float* dat)
        {
            return _mm_loadu_ps(dat);
        }

        template <>
        inline __m128d loadu(const double* dat)
        {
            return _mm_loadu_pd(dat);
        }

        template <typename U>
        void store(U*, typename detail::M<U>::T);

        template <>
        inline void store(float* dat, __m128 fill)
        {
            _mm_store_ps(dat, fill);
        }
        template <>
        inline void store(double* dat, __m128d fill)
        {
            _mm_store_pd(dat, fill);
        }
    } // namespace detail
} // namespace calc
