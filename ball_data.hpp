#pragma once

#include "calc/matrix.hpp"

//! struct BallData
/*! Defines a moving ball.
 */
struct BallData {
    //! Ball sprite index.
    unsigned selectedSkin = 0;
    //! Ball direction.
    calc::vec3f direction = calc::vec3f(1.0, 1.0, 0);
    //! Ball speed.
    calc::vec3f speed = calc::vec3f(0, 0, 0);
    //! Ball turn rate.
    calc::vec3f turnRate = calc::vec3f(0, 0, 0);
    //! Current ball position.
    calc::mat4f translation = calc::mat4f::identity();
};
