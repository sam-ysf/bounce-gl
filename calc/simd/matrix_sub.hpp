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

        template <>
        struct MatrixSubImpl<double> {
            static void sub(const double* dat1, const double* dat2, double* out)
            {
                __m128d v1 = _mm_load_pd(dat1);
                __m128d v2 = _mm_load_pd(dat2);

                __m128d v3 = _mm_sub_pd(v1, v2);
                _mm_store_pd(out, v3);
            }
        };
    } // namespace detail

    //! struct matrix_sub
    /*! SIMD matrix subtraction
     */
    template <typename T, int>
    struct MatrixSub;

    /*! SIMD matrix subtraction / matrix size 2
     */
    template <typename T>
    struct MatrixSub<T, 2> {
        static void sub(const T* dat1, const T* dat2, T* out, std::size_t)
        {
            detail::MatrixSubImpl<T>::sub(dat1, dat2, out); //> 1 pass
        }
    };

    /*! SIMD matrix subtraction / matrix size 3
     */
    template <typename T>
    struct MatrixSub<T, 3> {
        static void sub(const T* dat1, const T* dat2, T* out, std::size_t)
        {
            detail::MatrixSubImpl<T>::sub(dat1, dat2, out); //> 1 pass
        }
    };

    /*! SIMD matrix subtraction / matrix size 4
     */
    template <typename T>
    struct MatrixSub<T, 4> {
        static void sub(const T* dat1, const T* dat2, T* out, std::size_t)
        {
            detail::MatrixSubImpl<T>::sub(dat1, dat2, out); //> 1 pass
        }
    };

    /*! SIMD matrix subtraction / matrix size 9
     */
    template <typename T>
    struct MatrixSub<T, 9> {
        static void sub(const T* dat1, const T* dat2, T* out, std::size_t)
        {
            const int i1 = 4;
            const int i2 = 8;

            // 3 passes
            detail::MatrixSubImpl<T>::sub(dat1, dat2, out);
            detail::MatrixSubImpl<T>::sub(dat1 + i1, dat2 + i1, out + i1);
            detail::MatrixSubImpl<T>::sub(dat1 + i2, dat2 + i2, out + i2);
        }
    };

    /*! SIMD matrix subtraction / matrix size 16
     */
    template <typename T>
    struct MatrixSub<T, 16> {
        static void sub(const T* dat1, const T* dat2, T* out, std::size_t)
        {
            const int i1 = 4;
            const int i2 = 8;
            const int i3 = 12;

            // 4 passes
            detail::MatrixSubImpl<T>::sub(dat1, dat2, out);
            detail::MatrixSubImpl<T>::sub(dat1 + i1, dat2 + i1, out + i1);
            detail::MatrixSubImpl<T>::sub(dat1 + i2, dat2 + i2, out + i2);
            detail::MatrixSubImpl<T>::sub(dat1 + i3, dat2 + i3, out + i3);
        }
    };
} // namespace calc
