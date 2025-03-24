#pragma once

#include <cassert>
#include <cstring>
#include <new>
#include <type_traits>

#ifndef __NO_USE_SIMD__
#include "simd/matrix_add.hpp"
#include "simd/matrix_mul.hpp"
#include "simd/matrix_sub.hpp"
#include "simd/scalar_div.hpp"
#include "simd/scalar_mul.hpp"
#endif

// Padds to alignment size
#define __padd__(a) (((a) == 0) || ((a) % 16) ? ((a) + 16 - ((a) % 16)) : (a))

namespace calc {

    //! class Matrix
    /*! Defines a row-major matrix
     */
    template <typename Type, int N, int M>
    class Matrix {
        // Row-major ordered matrix data
        Type buffer_[__padd__(N * M)] __attribute__((aligned(16)));
    public:
        static Matrix<Type, N, M> identity(const Type eigenout = 1)
        {
            Matrix<Type, N, M> out(Type(0));
            for (int i = 0; i != N; ++i)
                out(i, i) = eigenout;
            return out;
        }

        explicit operator Type*()
        {
            return buffer_;
        }

        explicit operator const Type*() const
        {
            return buffer_;
        }

        //! Ctor.
        Matrix()
        {
            std::memset(buffer_, 0, sizeof(buffer_));
        }

        //! Ctor.
        template <typename T = Type>
        explicit Matrix(
            const typename std::enable_if<std::is_floating_point_v<T>,
                                          Type>::type fill)
        {
            for (int i = 0; i != N * M; ++i) {
                buffer_[i] = fill;
            }
        }

        //! Ctor.
        explicit Matrix(const Type* fill, std::size_t size = N * M)
        {
            std::memset(buffer_, 0, sizeof(buffer_));

            if (size > N * M) {
                throw std::bad_alloc();
            }

            std::memcpy(buffer_, fill, size * sizeof(Type));
        }

        //! Ctor.
        template <int N1, int M1, int N0 = N, int M0 = M>
        Matrix(const Matrix<Type, N1, M1>& v,
               std::enable_if_t<N0 * M0 == 4 && (N0 == 1 || M0 == 1)
                                    && (N1 * M1 == 2),
                                Type> x2,
               Type x3)
        {
            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = v[0];
            buffer_[1] = v[1];
            buffer_[2] = x2;
            buffer_[3] = x3;
        }

        //! Ctor.
        template <int N1, int M1, int N0 = N, int M0 = M>
        Matrix(const Matrix<Type, N1, M1>& v,
               std::enable_if_t<N0 * M0 == 4 && (N0 == 1 || M0 == 1)
                                    && (N1 * M1 == 3),
                                Type> x3)
        {
            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = v[0];
            buffer_[1] = v[1];
            buffer_[2] = v[2];
            buffer_[3] = x3;
        }

        //! Ctor.
        template <int N0 = N, int M0 = M>
        explicit Matrix(std::enable_if_t<N0 * M0 == 1, Type> x0)
        {
            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = x0;
        }

        //! Ctor.
        template <int N0 = N, int M0 = M>
        Matrix(std::enable_if_t<N0 * M0 == 2, Type> x0, Type x1)
        {
            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = x0;
            buffer_[1] = x1;
        }

        //! Ctor.
        template <int N0 = N, int M0 = M>
        Matrix(std::enable_if_t<N0 * M0 == 3, Type> x0, Type x1, Type x2)
        {
            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = x0;
            buffer_[1] = x1;
            buffer_[2] = x2;
        }

        //! Ctor.
        template <int N0 = N, int M0 = M>
        Matrix(std::enable_if_t<N0 * M0 == 4, Type> x0,
               Type x1,
               Type x2,
               Type x3)
        {
            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = x0;
            buffer_[1] = x1;
            buffer_[2] = x2;
            buffer_[3] = x3;
        }

        //! Ctor.
        template <int N0 = N, int M0 = M>
        Matrix(std::enable_if_t<N0 * M0 == 9, Type> x0,
               Type x1,
               Type x2,
               Type x3,
               Type x4,
               Type x5,
               Type x6,
               Type x7,
               Type x8)
        {
            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = x0;
            buffer_[1] = x1;
            buffer_[2] = x2;

            buffer_[3] = x3;
            buffer_[4] = x4;
            buffer_[5] = x5;

            buffer_[6] = x6;
            buffer_[7] = x7;
            buffer_[8] = x8;
        }

        //! Ctor.
        template <int N0 = N, int M0 = M>
        Matrix(typename std::enable_if_t<N0 * M0 == 16, Type> x0,
               Type x1,
               Type x2,
               Type x3,
               Type x4,
               Type x5,
               Type x6,
               Type x7,
               Type x8,
               Type x9,
               Type x10,
               Type x11,
               Type x12,
               Type x13,
               Type x14,
               Type x15)
        {
            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = x0;
            buffer_[1] = x1;
            buffer_[2] = x2;
            buffer_[3] = x3;

            buffer_[4] = x4;
            buffer_[5] = x5;
            buffer_[6] = x6;
            buffer_[7] = x7;

            buffer_[8] = x8;
            buffer_[9] = x9;
            buffer_[10] = x10;
            buffer_[11] = x11;

            buffer_[12] = x12;
            buffer_[13] = x13;
            buffer_[14] = x14;
            buffer_[15] = x15;
        }

        Type* data()
        {
            return buffer_;
        }

        const Type* data() const
        {
            return buffer_;
        }

        int rows() const
        {
            return N;
        }

        int cols() const
        {
            return M;
        }

        std::size_t size() const
        {
            return static_cast<std::size_t>(N * M);
        }

        template <typename T = Type, int N0 = N, int M0 = M>
        bool operator==(std::enable_if_t<std::is_floating_point_v<T>
                                             && (N0 == 1 || M0 == 1),
                                         const Matrix<T, N0, M0>&> rhs) const
        {
            constexpr Type kEps = 1e-8;

            if (rows() != rhs.rows()) {
                return false;
            }

            if (cols() != rhs.cols()) {
                return false;
            }

            for (int k = 0; k != N0 * M0; ++k) {
                if (((*this)[k] - rhs[k]) > kEps) {
                    return false;
                }
            }

            return true;
        }

        template <typename T = Type, int N0 = N, int M0 = M>
        bool operator==(std::enable_if_t<std::is_floating_point_v<T> && (N0 > 1)
                                             && (M0 > 1),
                                         const Matrix<T, N0, M0>&> rhs) const
        {
            constexpr Type kEps = 1e-8;

            if (rows() != rhs.rows()) {
                return false;
            }

            if (cols() != rhs.cols()) {
                return false;
            }

            for (int r = 0; r != N0; ++r) {
                for (int c = 0; c != M0; ++c) {
                    if (((*this)[r][c] - rhs[r][c]) > kEps) {
                        return false;
                    }
                }
            }

            return true;
        }

        template <typename T = Type, int N0 = N, int M0 = M>
        bool operator==(std::enable_if_t<!std::is_floating_point_v<T>
                                             && (N0 == 1 || M0 == 1),
                                         const Matrix<T, N0, M0>&> rhs) const
        {
            if (rows() != rhs.rows()) {
                return false;
            }

            if (cols() != rhs.cols()) {
                return false;
            }

            for (int k = 0; k != N0 * M0; ++k) {
                if ((*this)[k] != rhs[k]) {
                    return false;
                }
            }

            return true;
        }

        template <typename T = Type, int N0 = N, int M0 = M>
        bool operator==(std::enable_if_t<!std::is_floating_point_v<T>
                                             && (N0 > 1) && (M0 > 1),
                                         const Matrix<T, N0, M0>&> rhs) const
        {
            if (rows() != rhs.rows()) {
                return false;
            }

            if (cols() != rhs.cols()) {
                return false;
            }

            for (int r = 0; r != N0; ++r) {
                for (int c = 0; c != M0; ++c) {
                    if ((*this)[r][c] != rhs[r][c]) {
                        return false;
                    }
                }
            }

            return true;
        }

        template <int N0 = N, int M0 = M>
        std::enable_if_t<N0 == 1 || M0 == 1, Type&> operator[](int i)
        {
            return buffer_[i];
        }

        template <int N0 = N, int M0 = M>
        std::enable_if_t<N0 == 1 || M0 == 1, const Type&> operator[](
            int i) const
        {
            return buffer_[i];
        }

        template <int N0 = N, int M0 = M>
        std::enable_if_t<(N0 > 1 && M0 > 1), Type*> operator[](int r)
        {
            return &buffer_[r * M0];
        }

        template <int N0 = N, int M0 = M>
        std::enable_if_t<(N0 > 1 && M0 > 1), const Type*> operator[](
            int r) const
        {
            return &buffer_[r * M0];
        }

        Type& operator()(std::size_t r, std::size_t c)
        {
            return buffer_[(r * M) + c];
        }

        const Type& operator()(std::size_t r, std::size_t c) const
        {
            return buffer_[(r * M) + c];
        }

        Type& operator()(int r, int c)
        {
            return (*this)(static_cast<std::size_t>(r),
                           static_cast<std::size_t>(c));
        }

        const Type& operator()(int r, int c) const
        {
            return (*this)(static_cast<std::size_t>(r),
                           static_cast<std::size_t>(c));
        }

        template <int M1>
        Matrix<Type, N, M1> operator*(const Matrix<Type, M, M1>& rhs) const
        {
#ifdef __NO_USE_SIMD__
            Matrix<Type, N, M1> out;

            for (int r = 0; r != N; ++r) {
                for (int c = 0; c != M1; ++c) {
                    Type sum = 0;
                    for (int i = 0; i != M; ++i) {
                        Type a = (*this)(r, i);
                        Type b = rhs(i, c);
                        sum += (a * b);
                    }

                    out(r, c) = sum;
                }
            }
#else
            Matrix<Type, N, M1> out;
            matrix_mul<Type, N, M, M1>::mul(buffer_, rhs.data(), out.data());
#endif
            return out;
        }

        Matrix<Type, N, M> operator*(const Type scalar) const
        {
#ifdef __NO_USE_SIMD__
            Matrix<Type, N, M> out;
            for (int r = 0; r != N; ++r) {
                for (int c = 0; c != M; ++c) {
                    out(r, c) = (*this)(r, c) * scalar;
                }
            }
#else
            Matrix<Type, N, M> out;
            ScalarMul<Type, N * M>::mul(buffer_, scalar, out.data(), size());
#endif
            return out;
        }

        Matrix<Type, N, M> operator/(const Type scalar) const
        {
#ifdef __NO_USE_SIMD__
            Matrix<Type, N, M> out;
            for (int r = 0; r != N; ++r) {
                for (int c = 0; c != M; ++c) {
                    out(r, c) = (*this)(r, c) / scalar;
                }
            }
#else
            Matrix<Type, N, M> out;
            ScalarDiv<Type, N * M>::div(
                buffer_, scalar, out.data(), static_cast<std::size_t>(size()));
#endif
            return out;
        }

        template <int M1>
        Matrix<Type, N, M1>& operator*=(const Matrix<Type, M, M1>& rhs)
        {
            return (*this = (*this * rhs));
        }

        Matrix<Type, N, M>& operator*=(const Type scalar)
        {
            return (*this = (*this * scalar));
        }

        Matrix<Type, N, M>& operator/=(const Type scalar)
        {
            return (*this = (*this / scalar));
        }

        Matrix<Type, N, M> operator+(const Matrix<Type, N, M>& rhs) const
        {
#ifdef __NO_USE_SIMD__
            Matrix<Type, N, M> out;
            for (int r = 0; r != N; ++r) {
                for (int c = 0; c != M; ++c) {
                    out(r, c) = (*this)(r, c) + rhs(r, c);
                }
            }
#else
            Matrix<Type, N, M> out;
            MatrixAdd<Type, sizeof(Type)>::add(
                buffer_, rhs.data(), out.data(), size());
#endif
            return out;
        }

        Matrix<Type, N, M>& operator+=(const Matrix<Type, N, M>& rhs)
        {
#ifdef __NO_USE_SIMD__
            return (*this = (*this + rhs));
#else
            MatrixAdd<Type, sizeof(Type)>::add(
                buffer_, rhs.data(), buffer_, size());
            return *this;
#endif
        }

        Matrix<Type, N, M> operator-(const Matrix<Type, N, M>& rhs) const
        {
#ifdef __NO_USE_SIMD__
            Matrix<Type, N, M> out;
            for (int r = 0; r != N; ++r) {
                for (int c = 0; c != M; ++c) {
                    out(r, c) = (*this)(r, c) - rhs(r, c);
                }
            }
#else
            Matrix<Type, N, M> out;
            MatrixSub<Type, sizeof(Type)>::sub(
                buffer_,
                rhs.data(),
                out.data(),
                static_cast<std::size_t>(size()));
#endif
            return out;
        }

        Matrix<Type, N, M>& operator-=(const Matrix<Type, N, M>& rhs)
        {
#ifdef __NO_USE_SIMD__
            return (*this = (*this - rhs));
#else
            MatrixSub<Type, sizeof(Type)>::sub(
                buffer_, rhs.data(), buffer_, static_cast<std::size_t>(size()));
            return *this;
#endif
        }

        template <int N0 = N, int M0 = M>
        std::enable_if_t<(N0 == 1 || M0 == 1) && (N0 * M0 >= 1), Type> x() const
        {
            return buffer_[0];
        }

        template <int N0 = N, int M0 = M>
        std::enable_if_t<(N0 == 1 || M0 == 1) && (N0 * M0 >= 2), Type> y() const
        {
            return buffer_[1];
        }

        template <int N0 = N, int M0 = M>
        std::enable_if_t<(N0 == 1 || M0 == 1) && (N0 * M0 >= 3), Type> z() const
        {
            return buffer_[2];
        }

        template <int N0 = N, int M0 = M>
        std::enable_if_t<(N0 == 1 || M0 == 1) && (N0 * M0 >= 4), Type> w() const
        {
            return buffer_[3];
        }
    };

    template <typename Type, int N = 0, int M = 0>
    Matrix<Type, N, M> operator-(const Matrix<Type, N, M>& m)
    {
        return m * -1;
    }

    template <typename Type, int N = 0, int M = 0>
    Matrix<Type, N, M> operator*(const Type scalar, const Matrix<Type, N, M>& m)
    {
        static_assert(std::is_standard_layout_v<Type>
                          && std::is_trivial_v<Type>,
                      "Type must be a standard layout and trivial type "
                      "for scalar "
                      "multiplication");
        return m * scalar;
    }

    // 2x2
    using mat2f = Matrix<float, 2, 2>;
    // 3x3
    using mat3f = Matrix<float, 3, 3>;
    // 4x4
    using mat4f = Matrix<float, 4, 4>;

    // 2x1
    using vec2f = Matrix<float, 2, 1>;
    // 3x1
    using vec3f = Matrix<float, 3, 1>;
    // 4x1
    using vec4f = Matrix<float, 4, 1>;
} // namespace calc
