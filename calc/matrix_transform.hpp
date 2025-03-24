#pragma once

#include "matrix_nxm.hpp"
#include <cmath>

namespace calc {

    //! 3.1415926...
    const float kPi = std::atanf(1.0) * 4.0F;

    //! @return angle in radians
    inline float radians(const float deg)
    {
        return kPi * deg / 180.0F;
    }

    //! @return angle in degrees
    inline float degrees(const float rad)
    {
        return rad * 180.0F / kPi;
    }

    //! @return rotation matrix
    inline mat4f rotate_4x(const float rad)
    {
        const float r[] = {1,
                           0,
                           0,
                           0,

                           0,
                           std::cos(rad),
                           -std::sin(rad),
                           0,

                           0,
                           std::sin(rad),
                           std::cos(rad),
                           0,

                           0,
                           0,
                           0,
                           1};

        return mat4f(r);
    }

    //! @return rotation matrix
    inline mat3f rotate_3x(const float rad)
    {
        const float r[] = {1,
                           0,
                           0,

                           0,
                           std::cos(rad),
                           -std::sin(rad),

                           0,
                           std::sin(rad),
                           std::cos(rad)};

        return mat3f(r);
    }

    //! @return rotation matrix
    inline mat4f rotate_4y(const float rad)
    {
        const float r[] = {std::cos(rad),
                           0,
                           std::sin(rad),
                           0,

                           0,
                           1,
                           0,
                           0,

                           -std::sin(rad),
                           0,
                           std::cos(rad),
                           0,

                           0,
                           0,
                           0,
                           1};

        return mat4f(r);
    }

    //! @return rotation matrix
    inline mat3f rotate_3y(float rad)
    {
        const float r[] = {std::cos(rad),
                           0,
                           std::sin(rad),

                           0,
                           1,
                           0,

                           -std::sin(rad),
                           0,
                           std::cos(rad)};

        return mat3f(r);
    }

    //! @return rotation matrix
    inline mat4f rotate_4z(const float rad)
    {
        const float r[] = {std::cos(rad),
                           -std::sin(rad),
                           0,
                           0,

                           std::sin(rad),
                           std::cos(rad),
                           0,
                           0,

                           0,
                           0,
                           1,
                           0,

                           0,
                           0,
                           0,
                           1};

        return mat4f(r);
    }

    //! @return rotation matrix
    inline mat3f rotate_3z(const float rad)
    {
        const float r[] = {std::cos(rad),
                           -std::sin(rad),
                           0,

                           std::sin(rad),
                           std::cos(rad),
                           0,

                           0,
                           0,
                           1};

        return mat3f(r);
    }

    //! @return scaling matrix
    inline mat4f scalar_xyz(const calc::vec3f& xyz)
    {
        mat4f scale = calc::mat4f::identity();

        scale[0][0] = xyz.x();
        scale[1][1] = xyz.y();
        scale[2][2] = xyz.z();

        return scale;
    }
} // namespace calc
