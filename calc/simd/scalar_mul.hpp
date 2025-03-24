#pragma once

#include "common.hpp"
#include <algorithm>

namespace calc {

    namespace detail {

        template <typename>
        struct scalar_mul_impl;

        template <>
        struct scalar_mul_impl<float> {
            static void mul(const float* dat1, const __m128 v2, float* out)
            {
                __m128 v1 = _mm_load_ps(dat1);
                __m128 v3 = _mm_mul_ps(v1, v2);
                _mm_store_ps(out, v3);
            }
        };

        template <>
        struct scalar_mul_impl<double> {
            static void mul(const double* dat1, const __m128d v2, double* out)
            {
                __m128d v1 = _mm_load_pd(dat1);
                __m128d v3 = _mm_mul_pd(v1, v2);
                _mm_store_pd(out, v3);
            }
        };
    } // namespace detail

    //! struct scalar_mul
    /*! SIMD scalar multiplication
     */
    template <typename T, unsigned = 0>
    struct ScalarMul {
        static void mul(const T* dat1, const T dat2, T* out, std::size_t size)
        {
            const typename detail::M<T>::T v2 = _mm_set1_ps(dat2);

            size = std::max(size / sizeof(T), sizeof(T));

            std::size_t i = 0;
            for (; i < size; ++i) {
                std::size_t ii = i * 16 / sizeof(T);
                detail::scalar_mul_impl<T>::mul(dat1 + ii, v2, out + ii);
            }
        }
    };

    /*! SIMD scalar multiplication / matrix size 2
     */
    template <>
    struct ScalarMul<float, 2> {
        static void mul(const float* dat1,
                        const float dat2,
                        float* out,
                        std::size_t = 0)
        {
            const __m128 v2 = _mm_set1_ps(dat2);
            detail::scalar_mul_impl<float>::mul(dat1, v2, out); //> 1 pass
        }
    };

    /*! SIMD scalar multiplication / matrix size 3
     */
    template <>
    struct ScalarMul<float, 3> {
        static void mul(const float* dat1,
                        const float dat2,
                        float* out,
                        std::size_t = 0)
        {
            const __m128 v2 = _mm_set1_ps(dat2);
            detail::scalar_mul_impl<float>::mul(dat1, v2, out); //> 1 pass
        }
    };

    /*! SIMD scalar multiplication / matrix size 4
     */
    template <>
    struct ScalarMul<float, 4> {
        static void mul(const float* dat1,
                        const float dat2,
                        float* out,
                        std::size_t = 0)
        {
            const __m128 v2 = _mm_set1_ps(dat2);
            detail::scalar_mul_impl<float>::mul(dat1, v2, out); //> 1 pass
        }
    };

    /*! SIMD scalar multiplication / matrix size 9
     */
    template <>
    struct ScalarMul<float, 9> {
        static void mul(const float* dat1,
                        const float dat2,
                        float* out,
                        std::size_t = 0)
        {
            const unsigned i1 = 4;
            const unsigned i2 = 8;

            const __m128 v2 = _mm_set1_ps(dat2);
            // 3 passes
            detail::scalar_mul_impl<float>::mul(dat1, v2, out);
            detail::scalar_mul_impl<float>::mul(dat1 + i1, v2, out + i1);
            detail::scalar_mul_impl<float>::mul(dat1 + i2, v2, out + i2);
        }
    };

    /*! SIMD scalar multiplication / matrix size 16
     */
    template <>
    struct ScalarMul<float, 16> {
        static void mul(const float* dat1,
                        const float dat2,
                        float* out,
                        std::size_t = 0)
        {
            const unsigned i1 = 4;
            const unsigned i2 = 8;
            const unsigned i3 = 12;

            const __m128 v2 = _mm_set1_ps(dat2);
            // 4 passes
            detail::scalar_mul_impl<float>::mul(dat1, v2, out);
            detail::scalar_mul_impl<float>::mul(dat1 + i1, v2, out + i1);
            detail::scalar_mul_impl<float>::mul(dat1 + i2, v2, out + i2);
            detail::scalar_mul_impl<float>::mul(dat1 + i3, v2, out + i3);
        }
    };
} // namespace calc
