#pragma once

#include <cstring>
#include <immintrin.h>

namespace calc {

    namespace detail {

        template <typename>
        struct MatrixAddImpl;

        template <>
        struct MatrixAddImpl<float> {
            static void add(const float* dat1, const float* dat2, float* out)
            {
                __m128 v1 = _mm_load_ps(dat1);
                __m128 v2 = _mm_load_ps(dat2);

                __m128 v3 = _mm_add_ps(v1, v2);
                _mm_store_ps(out, v3);
            }
        };
    } // namespace detail

    //! struct matrix_add
    /*! SIMD matrix addition
     */
    template <typename T, unsigned>
    struct MatrixAdd;

    /*! SIMD matrix addition / matrix size 2
     */
    template <>
    struct MatrixAdd<float, 2> {
        static void add(const float* dat1,
                        const float* dat2,
                        float* out,
                        std::size_t)
        {
            detail::MatrixAddImpl<float>::add(dat1, dat2, out); //> 1 pass
        }
    };

    /*! SIMD matrix addition / matrix size 3
     */
    template <>
    struct MatrixAdd<float, 3> {
        static void add(const float* dat1,
                        const float* dat2,
                        float* out,
                        std::size_t)
        {
            detail::MatrixAddImpl<float>::add(dat1, dat2, out); //> 1 pass
        }
    };

    /*! SIMD matrix addition / matrix size 4
     */
    template <>
    struct MatrixAdd<float, 4> {
        static void add(const float* dat1,
                        const float* dat2,
                        float* out,
                        std::size_t)
        {
            detail::MatrixAddImpl<float>::add(dat1, dat2, out); //> 1 pass
        }
    };

    /*! SIMD matrix addition / matrix size 9
     */
    template <>
    struct MatrixAdd<float, 9> {
        static void add(const float* dat1,
                        const float* dat2,
                        float* out,
                        std::size_t)
        {
            unsigned i1 = 4;
            unsigned i2 = 8;

            // 3 passes
            detail::MatrixAddImpl<float>::add(dat1, dat2, out);
            detail::MatrixAddImpl<float>::add(dat1 + i1, dat2 + i1, out + i1);
            detail::MatrixAddImpl<float>::add(dat1 + i2, dat2 + i2, out + i2);
        }
    };

    /*! SIMD matrix addition / matrix size 16
     */
    template <>
    struct MatrixAdd<float, 16> {
        static void add(const float* dat1,
                        const float* dat2,
                        float* out,
                        std::size_t)
        {
            unsigned i1 = 4;
            unsigned i2 = 8;
            unsigned i3 = 12;

            // 4 passes
            detail::MatrixAddImpl<float>::add(dat1, dat2, out);
            detail::MatrixAddImpl<float>::add(dat1 + i1, dat2 + i1, out + i1);
            detail::MatrixAddImpl<float>::add(dat1 + i2, dat2 + i2, out + i2);
            detail::MatrixAddImpl<float>::add(dat1 + i3, dat2 + i3, out + i3);
        }
    };
}; // namespace calc
