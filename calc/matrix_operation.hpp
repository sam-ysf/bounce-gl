#pragma once

#include "matrix_nxm.hpp"
#include <cmath>

namespace calc {

    //! @return
    //!     Pointer to matrix raw data
    template <typename T, int N, int M>
    inline T* data(Matrix<T, N, M>& m)
    {
        return static_cast<T*>(m);
    }

    //! @return
    //!     Pointer to matrix data
    template <typename T, int N, int M>
    inline const T* data(const Matrix<T, N, M>& m)
    {
        return static_cast<const T*>(m);
    }

    //! @return
    //!     Schur product
    template <typename T, int N, int M>
    inline Matrix<T, N, M> schur(const Matrix<T, N, M>& lhs,
                                 const Matrix<T, N, M>& rhs)
    {
        Matrix<T, N, M> out;

        for (int r = 0; r != N; ++r) {
            for (int c = 0; c != M; ++c) {
                out[r][c] = lhs[r][c] * rhs[r][c];
            }
        }

        return out;
    }

    //! @return
    //!     Schur product
    template <typename T, int N>
    inline Matrix<T, 1, N> schur(const Matrix<T, 1, N>& lhs,
                                 const Matrix<T, 1, N>& rhs)
    {
        Matrix<T, 1, N> out;

        for (int i = 0; i != N; ++i) {
            out[i] = lhs[i] * rhs[i];
        }

        return out;
    }

    //! @return
    //!     Schur product
    template <typename T, int N>
    inline Matrix<T, N, 1> schur(const Matrix<T, N, 1>& lhs,
                                 const Matrix<T, N, 1>& rhs)
    {
        Matrix<T, N, 1> out;

        for (int i = 0; i != N; ++i) {
            out[i] = lhs[i] * rhs[i];
        }

        return out;
    }

    //! @return
    //!     Cross product
    template <typename T>
    inline Matrix<T, 3, 1> cross(const Matrix<T, 3, 1>& lhs,
                                 const Matrix<T, 3, 1>& rhs)
    {
        Matrix<T, 3, 1> out;

        out[0] = lhs.y() * rhs.z() - lhs.z() * rhs.y(); // AyBz - AzBy
        out[1] = lhs.z() * rhs.x() - lhs.x() * rhs.z(); // AzBx - AxBz
        out[2] = lhs.x() * rhs.y() - lhs.y() * rhs.x(); // AxBy - AyBx

        return out;
    }

    //! @return
    //!     Cross product
    template <typename T>
    inline Matrix<T, 3, 1> cross(const Matrix<T, 1, 3>& lhs,
                                 const Matrix<T, 1, 3>& rhs)
    {
        Matrix<T, 3, 1> out;

        out[0] = lhs.y() * rhs.z() - lhs.z() * rhs.y(); // AyBz - AzBy
        out[1] = lhs.z() * rhs.x() - lhs.x() * rhs.z(); // AzBx - AxBz
        out[2] = lhs.x() * rhs.y() - lhs.y() * rhs.x(); // AxBy - AyBx

        return out;
    }

    //! @return
    //!     Normalized vector
    template <typename T, int N>
    inline Matrix<T, N, 1> normal(const Matrix<T, N, 1>& in)
    {
        const T* d = data(in);

        T mag = 0;
        for (int i = 0; i != N; ++i)
            mag += d[i] * d[i];
        return in / std::sqrt(mag);
    }

    //! @return
    //!     Normalized vector
    template <typename T, int N>
    inline Matrix<T, 1, N> normal(const Matrix<T, 1, N>& in)
    {
        const T* d = data(in);

        T mag = 0;
        for (int i = 0; i != N; ++i)
            mag += d[i] * d[i];
        return in / std::sqrt(mag);
    }

    //! @return
    //!     Transposed matrix
    template <typename T, int N, int M>
    inline Matrix<T, M, N> transpose(const Matrix<T, N, M>& in)
    {
        Matrix<T, M, N> out;

        for (int r = 0; r != N; ++r) {
            for (int c = 0; c != M; ++c) {
                out(c, r) = in(r, c);
            }
        }

        return out;
    }

    //! @return
    //!     Absolute-valued matrix
    template <typename T, int N, int M>
    inline Matrix<T, N, M> abs(const Matrix<T, N, M>& in)
    {
        Matrix<T, N, M> out;
        for (int r = 0; r != N; ++r) {
            for (int c = 0; c != M; ++c) {
                out[r][c] = std::abs(in[r][c]);
            }
        }

        return out;
    }

    //! @return
    //!     Absolute-valued matrix
    template <typename T, int N>
    inline Matrix<T, N, 1> abs(const Matrix<T, N, 1>& in)
    {
        Matrix<T, N, 1> out;

        int i = 0;
        for (; i != N; ++i)
            out[i] = std::abs(in[i]);
        return out;
    }

    //! @return
    //!     Absolute-valued matrix
    template <typename T, int N>
    inline Matrix<T, 1, N> abs(const Matrix<T, 1, N>& in)
    {
        Matrix<T, 1, N> out;

        int i = 0;
        for (; i != N; ++i)
            out[i] = std::abs(in[i]);
        return out;
    }

    //! @return
    //!     Dot product: lhs * rhs
    template <typename T, int N>
    inline T dot(const Matrix<T, N, 1>& lhs, const Matrix<T, N, 1>& rhs)
    {
        T sum = 0;

        int i = 0;
        for (; i != static_cast<int>(lhs.size()); ++i)
            sum += lhs[i] * rhs[i];
        return sum;
    }

    //! @return
    //!     Dot product: lhs * rhs
    template <typename T, int N>
    inline T dot(const Matrix<T, 1, N>& lhs, const Matrix<T, 1, N>& rhs)
    {
        T sum = 0;

        int i = 0;
        for (; i != static_cast<int>(lhs.size()); ++i)
            sum += lhs[i] * rhs[i];
        return sum;
    }

    template <typename T, int N, int M>
    inline Matrix<T, N, M> max(const Matrix<T, N, M>& lhs,
                               const Matrix<T, N, M>& rhs)
    {
        Matrix<T, N, M> out;

        for (int r = 0; r != N; ++r) {
            for (int c = 0; c != M; ++c) {
                out[r][c] = lhs[r][c] > rhs[r][c] ? lhs[r][c] : rhs[r][c];
            }
        }

        return out;
    }

    template <typename T, int N>
    inline Matrix<T, N, 1> max(const Matrix<T, N, 1>& lhs,
                               const Matrix<T, N, 1>& rhs)
    {
        Matrix<T, N, 1> out;

        int i = 0;
        for (; i != N; ++i)
            out[i] = lhs[i] > rhs[i] ? lhs[i] : rhs[i];
        return out;
    }

    template <typename T, int N>
    inline Matrix<T, 1, N> max(const Matrix<T, 1, N>& lhs,
                               const Matrix<T, 1, N>& rhs)
    {
        Matrix<T, 1, N> out;

        int i = 0;
        for (; i != N; ++i)
            out[i] = lhs[i] > rhs[i] ? lhs[i] : rhs[i];
        return out;
    }

    template <typename T, int N, int M>
    inline Matrix<T, N, M> min(const Matrix<T, N, M>& lhs,
                               const Matrix<T, N, M>& rhs)
    {
        Matrix<T, N, M> out;

        for (int r = 0; r != N; ++r) {
            for (int c = 0; c != M; ++c) {
                out[r][c] = lhs[r][c] < rhs[r][c] ? lhs[r][c] : rhs[r][c];
            }
        }

        return out;
    }

    template <typename T, int N>
    inline Matrix<T, N, 1> min(const Matrix<T, N, 1>& lhs,
                               const Matrix<T, N, 1>& rhs)
    {
        Matrix<T, N, 1> out;

        int i = 0;
        for (; i != N; ++i)
            out[i] = lhs[i] < rhs[i] ? lhs[i] : rhs[i];
        return out;
    }

    template <typename T, int N>
    inline Matrix<T, 1, N> min(const Matrix<T, 1, N>& lhs,
                               const Matrix<T, 1, N>& rhs)
    {
        Matrix<T, 1, N> out;

        int i = 0;
        for (; i != N; ++i)
            out[i] = lhs[i] < rhs[i] ? lhs[i] : rhs[i];
        return out;
    }

    template <typename T, int N>
    inline T norm(const Matrix<T, 1, N>& value)
    {
        Matrix<T, 1, N> out;

        T sum_of_squares = 0;

        int i = 0;
        for (; i != N; ++i)
            sum_of_squares += value[i] * value[i];
        return std::sqrt(sum_of_squares);
    }

    template <typename T, int N>
    inline T norm(const Matrix<T, N, 1>& value)
    {
        Matrix<T, 1, N> out;

        T sum_of_squares = 0;

        int i = 0;
        for (; i != N; ++i)
            sum_of_squares += value[i] * value[i];
        return std::sqrt(sum_of_squares);
    }
} // namespace calc
