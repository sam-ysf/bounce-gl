#pragma once

#include "calc/matrix.hpp"
#include "program.hpp"

//! class DrawInstancedNoTexture
/*! Program for drawing untextured objects to screen.
 */
class DrawInstancedNoTexture : public Program {
public:
    /*! @brief Ctor.
     */
    explicit DrawInstancedNoTexture();

    void set_color(const calc::vec4f& v);

    void set_scene(const calc::mat4f& lookAt, const calc::mat4f& projection);
};
