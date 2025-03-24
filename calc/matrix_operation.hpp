#pragma once

#include "matrix_nxm.hpp"

namespace calc {

    //! @return
    //!     Pointer to matrix raw data
    template <typename T, unsigned N, unsigned M>
    inline T* data(Matrix<T, N, M>& m)
    {
        return static_cast<T*>(m);
    }

    //! @return
    //!     Pointer to matrix data
    template <typename T, unsigned N, unsigned M>
    inline const T* data(const Matrix<T, N, M>& m)
    {
        return static_cast<const T*>(m);
    }

    //! @return
    //!     Cross product
    template <typename T>
    inline Matrix<T, 4, 1> cross(const Matrix<T, 4, 1>& lhs,
                                 const Matrix<T, 4, 1>& rhs)
    {
        Matrix<T, 4, 1> out;

        out[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1]; // AyBz - AzBy
        out[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2]; // AzBx - AxBz
        out[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0]; // AxBy - AyBx

        return out;
    }

    //! @return
    //!     Cross product
    template <typename T>
    inline Matrix<T, 3, 1> cross(const Matrix<T, 3, 1>& lhs,
                                 const Matrix<T, 3, 1>& rhs)
    {
        Matrix<T, 3, 1> out;

        out[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1]; // AyBz - AzBy
        out[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2]; // AzBx - AxBz
        out[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0]; // AxBy - AyBx

        return out;
    }

    //! @return
    //!     Cross product
    template <typename T>
    inline Matrix<T, 3, 1> cross(const Matrix<T, 1, 3>& lhs,
                                 const Matrix<T, 1, 3>& rhs)
    {
        Matrix<T, 3, 1> out;

        out[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1]; // AyBz - AzBy
        out[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2]; // AzBx - AxBz
        out[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0]; // AxBy - AyBx

        return out;
    }

    //! @return
    //!     Normalized vector
    template <typename T, unsigned N>
    inline Matrix<T, N, 1> normal(const Matrix<T, N, 1>& in)
    {
        const float* d = data(in);

        float mag = 0;
        for (unsigned i = 0; i != N; ++i)
            mag += (d[i] * d[i]);
        return in / std::sqrt(mag);
    }

    //! @return
    //!     Transposed matrix
    template <typename T, unsigned N, unsigned M>
    inline Matrix<T, M, N> transpose(const Matrix<T, N, M>& in)
    {
        Matrix<T, M, N> out;

        std::size_t i = 0;
        for (; i != in.size(); ++i) {
            std::size_t r = i / N;
            std::size_t c = i % N;
            out(c, r) = in(r, c);
        }

        return out;
    }

    //! @return
    //!     Absolute-valued matrix
    template <typename T, unsigned N, unsigned M>
    inline Matrix<T, N, M> abs(const Matrix<T, N, M>& inp)
    {
        Matrix<T, N, M> out;
        for (unsigned r = 0; r != N; ++r) {
            for (unsigned c = 0; c != M; ++c) {
                out(r, c) = std::abs(inp(r, c));
            }
        }

        return out;
    }

    template <unsigned N>
    inline Matrix<float, N, 1> max(const Matrix<float, N, 1>& lhs,
                                   const Matrix<float, N, 1>& rhs)
    {
        Matrix<float, N, 1> out;

        unsigned i = 0;
        for (; i != N; ++i)
            out[i] = (lhs[i] >= rhs[i]) ? lhs[i] : rhs[i];
        return out;
    }

    template <unsigned N>
    inline Matrix<float, N, 1> min(const Matrix<float, N, 1>& lhs,
                                   const Matrix<float, N, 1>& rhs)
    {
        Matrix<float, N, 1> out;

        unsigned i = 0;
        for (; i != N; ++i)
            out[i] = (lhs[i] <= rhs[i]) ? lhs[i] : rhs[i];
        return out;
    }

    //! @return
    //!     Dot product: lhs * rhs
    template <unsigned N>
    inline float dot(const Matrix<float, N, 1>& lhs,
                     const Matrix<float, N, 1>& rhs)
    {
        float sum = 0;

        unsigned i = 0;
        for (; i != lhs.size(); ++i)
            sum += lhs[i] * rhs[i];
        return sum;
    }

    //! @return
    //!     Dot product: lhs * rhs
    template <unsigned N>
    inline float dot(const Matrix<float, 0, 0>& lhs,
                     const Matrix<float, N, 1>& rhs)
    {
        /*ASSERT*/ assert(lhs.rows() == 1);
        /*ASSERT*/ assert(lhs.size() == rhs.size());

        float sum = 0;

        unsigned i = 0;
        for (; i != lhs.size(); ++i)
            sum += lhs[i] * rhs[i];
        return sum;
    }

    //! @return
    //!     Dot product: lhs * rhs
    inline float dot(const Matrix<float, 0, 0>& lhs,
                     const Matrix<float, 0, 0>& rhs)
    {
        /*ASSERT*/ assert(lhs.rows() == 1);
        /*ASSERT*/ assert(lhs.size() == rhs.size());

        float sum = 0;

        unsigned i = 0;
        for (; i != lhs.size(); ++i)
            sum += lhs[i] * rhs[i];
        return sum;
    }
} // namespace calc
