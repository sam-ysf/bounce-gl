#include "draw_instanced_no_texture.hpp"
#include "calc/matrix.hpp"

DrawInstancedNoTexture::DrawInstancedNoTexture()
{
    const VertexShader sh1 = {
#include "shaders/instanced_no_texture.vs"
    };

    const FragmentShader sh2 = {
#include "shaders/instanced_no_texture.fs"
    };

    Program::add_shader(sh1);
    Program::add_shader(sh2);

    // Link program
    Program::link();
    Program::use();
}

void DrawInstancedNoTexture::set_color(const calc::Vec4f& v) const
{
    // Set projection matrix
    Program::set_value_vec4("color", calc::data(v));
}

void DrawInstancedNoTexture::set_scene(const calc::Mat4f& lookAt,
                                       const calc::Mat4f& projection) const
{
    // Set projection matrix
    Program::set_value_mat4x4("view", calc::data(lookAt));
    // Set view matrix
    Program::set_value_mat4x4("projection", calc::data(projection));
}
