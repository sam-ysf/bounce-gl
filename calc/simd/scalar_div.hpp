#pragma once

#include "common.hpp"
#include <algorithm>

namespace calc {

    namespace detail {

        template <typename>
        struct scalar_div_impl;

        template <>
        struct scalar_div_impl<float> {
            static void div(const float* dat1, const __m128 v2, float* out)
            {
                __m128 v1 = _mm_load_ps(dat1);
                __m128 v3 = _mm_div_ps(v1, v2);
                _mm_store_ps(out, v3);
            }
        };
    } // namespace detail

    //! functor scalar_div
    /*! SIMD scalar division
     */
    template <typename, unsigned>
    struct ScalarDiv;

    template <>
    struct ScalarDiv<float, 0> {
        static void div(const float* dat1,
                        const float dat2,
                        float* out,
                        std::size_t size)
        {
            const typename detail::M<float>::T v2 = _mm_set1_ps(dat2);

            size = std::max(size / sizeof(float), sizeof(float));

            std::size_t i = 0;
            for (; i < size; ++i) {
                std::size_t ii = i * 16 / sizeof(float);
                detail::scalar_div_impl<float>::div(dat1 + ii, v2, out + ii);
            }
        }
    };

    /*! SIMD scalar division / matrix size 2
     */
    template <>
    struct ScalarDiv<float, 2> {
        static void div(const float* dat1,
                        const float dat2,
                        float* out,
                        std::size_t)
        {
            const __m128 v2 = _mm_set1_ps(dat2);
            detail::scalar_div_impl<float>::div(dat1, v2, out); //> 1 pass
        }
    };

    /*! SIMD scalar division / matrix size 3
     */
    template <>
    struct ScalarDiv<float, 3> {
        static void div(const float* dat1,
                        const float dat2,
                        float* out,
                        std::size_t)
        {
            const __m128 v2 = _mm_set1_ps(dat2);
            detail::scalar_div_impl<float>::div(dat1, v2, out); //> 1 pass
        }
    };

    /*! SIMD scalar division / matrix size 4
     */
    template <>
    struct ScalarDiv<float, 4> {
        static void div(const float* dat1,
                        const float dat2,
                        float* out,
                        std::size_t)
        {
            const __m128 v2 = _mm_set1_ps(dat2);
            detail::scalar_div_impl<float>::div(dat1, v2, out); //> 1 pass
        }
    };

    /*! SIMD scalar division / matrix size 9
     */
    template <>
    struct ScalarDiv<float, 9> {
        static void div(const float* dat1,
                        const float dat2,
                        float* out,
                        std::size_t)
        {
            unsigned i1 = 4;
            unsigned i2 = 8;

            const __m128 v2 = _mm_set1_ps(dat2);
            // 3 passes
            detail::scalar_div_impl<float>::div(dat1, v2, out);
            detail::scalar_div_impl<float>::div(dat1 + i1, v2, out + i1);
            detail::scalar_div_impl<float>::div(dat1 + i2, v2, out + i2);
        }
    };

    /*! SIMD scalar division / matrix size 16
     */
    template <>
    struct ScalarDiv<float, 16> {
        static void div(const float* dat1,
                        const float dat2,
                        float* out,
                        std::size_t)
        {
            unsigned i1 = 4;
            unsigned i2 = 8;
            unsigned i3 = 12;

            const __m128 v2 = _mm_set1_ps(dat2);
            // 4 passes
            detail::scalar_div_impl<float>::div(dat1, v2, out);
            detail::scalar_div_impl<float>::div(dat1 + i1, v2, out + i1);
            detail::scalar_div_impl<float>::div(dat1 + i2, v2, out + i2);
            detail::scalar_div_impl<float>::div(dat1 + i3, v2, out + i3);
        }
    };
} // namespace calc
