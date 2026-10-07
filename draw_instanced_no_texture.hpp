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

    void set_color(const calc::Vec4f& v) const;

    void set_scene(const calc::Mat4f& lookAt,
                   const calc::Mat4f& projection) const;
};
