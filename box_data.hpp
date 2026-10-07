#pragma once

#include "calc/matrix.hpp"

//! struct BallData
/*! Defines a moving ball.
 */
struct BoxData {
    //! Ball sprite index.
    unsigned selectedSkin = 0;
    //! Ball direction.
    calc::Vec3f direction = calc::Vec3f(1.0, 1.0, 0);
    //! Ball speed.
    calc::Vec3f speed = calc::Vec3f(0, 0, 0);
    //! Ball turn rate.
    calc::Vec3f turnRate = calc::Vec3f(0, 0, 0);
    //! Current ball position.
    calc::Mat4f translation = calc::Mat4f::identity();
};
