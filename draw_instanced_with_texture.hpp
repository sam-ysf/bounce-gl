#pragma once

#include "calc/matrix.hpp"
#include "program.hpp"

//! class DrawInstancedWithTexture
/*! Program for drawing textured objects to screen.
 */
class DrawInstancedWithTexture : public Program {
public:
    /*! @brief Ctor.
     */
    DrawInstancedWithTexture();

    void set_scene(const calc::mat4f& lookAt, const calc::mat4f& projection);
};
