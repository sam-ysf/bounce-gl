#pragma once

#include <immintrin.h>

namespace calc {

    namespace detail {

        template <typename>
        struct MatrixSubImpl;

        template <>
        struct MatrixSubImpl<float> {
            static void sub(const float* dat1, const float* dat2, float* out)
            {
                __m128 v1 = _mm_load_ps(dat1);
                __m128 v2 = _mm_load_ps(dat2);

                __m128 v3 = _mm_sub_ps(v1, v2);
                _mm_storeu_ps(out, v3);
            }
        };
    } // namespace detail

    //! struct matrix_sub
    /*! SIMD matrix subtraction
     */
    template <typename, unsigned>
    struct MatrixSub;

    /*! SIMD matrix subtraction / matrix size 2
     */
    template <>
    struct MatrixSub<float, 2> {
        static void sub(const float* dat1,
                        const float* dat2,
                        float* out,
                        std::size_t)
        {
            detail::MatrixSubImpl<float>::sub(dat1, dat2, out); //> 1 pass
        }
    };

    /*! SIMD matrix subtraction / matrix size 3
     */
    template <>
    struct MatrixSub<float, 3> {
        static void sub(const float* dat1,
                        const float* dat2,
                        float* out,
                        std::size_t)
        {
            detail::MatrixSubImpl<float>::sub(dat1, dat2, out); //> 1 pass
        }
    };

    /*! SIMD matrix subtraction / matrix size 4
     */
    template <>
    struct MatrixSub<float, 4> {
        static void sub(const float* dat1,
                        const float* dat2,
                        float* out,
                        std::size_t)
        {
            detail::MatrixSubImpl<float>::sub(dat1, dat2, out); //> 1 pass
        }
    };

    /*! SIMD matrix subtraction / matrix size 9
     */
    template <>
    struct MatrixSub<float, 9> {
        static void sub(const float* dat1,
                        const float* dat2,
                        float* out,
                        std::size_t)
        {
            unsigned i1 = 4;
            unsigned i2 = 8;

            // 3 passes
            detail::MatrixSubImpl<float>::sub(dat1, dat2, out);
            detail::MatrixSubImpl<float>::sub(dat1 + i1, dat2 + i1, out + i1);
            detail::MatrixSubImpl<float>::sub(dat1 + i2, dat2 + i2, out + i2);
        }
    };

    /*! SIMD matrix subtraction / matrix size 16
     */
    template <>
    struct MatrixSub<float, 16> {
        static void sub(const float* dat1,
                        const float* dat2,
                        float* out,
                        std::size_t)
        {
            unsigned i1 = 4;
            unsigned i2 = 8;
            unsigned i3 = 12;

            // 4 passes
            detail::MatrixSubImpl<float>::sub(dat1, dat2, out);
            detail::MatrixSubImpl<float>::sub(dat1 + i1, dat2 + i1, out + i1);
            detail::MatrixSubImpl<float>::sub(dat1 + i2, dat2 + i2, out + i2);
            detail::MatrixSubImpl<float>::sub(dat1 + i3, dat2 + i3, out + i3);
        }
    };
} // namespace calc
