#include "grid_square.hpp"
#include "glad/glad.h"
#include "texture.hpp"
#include <cstring>

namespace {

    // Square
    static const float kVertices[] = {
        -0.5f, -0.5f, -0.5f,
        +0.5f, -0.5f, -0.5f,
        +0.5f,  0.5f, -0.5f,
        +0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
    };
} // namespace

render::GridSquare::GridSquare(unsigned instanceSizeMax)
{
    std::memset(&vbo_, 0, sizeof(vbo_));

    // Initialize OpenGL buffers
    glGenVertexArrays(1, &vbo_.mesh);
    glBindVertexArray(vbo_.mesh);

    glGenBuffers(1, &vbo_.vertex);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_.vertex);

    // Add vertices
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(kVertices), kVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));

    // Instancing
    glGenBuffers(1, &vbo_.instance);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_.instance);

    // Null buffer
    glBufferData(GL_ARRAY_BUFFER,
                 instanceSizeMax * 16 * sizeof(float),
                 nullptr,
                 GL_STREAM_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(0));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          16 * sizeof(float),
                          (void*)(4 * sizeof(float)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          16 * sizeof(float),
                          (void*)(8 * sizeof(float)));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          16 * sizeof(float),
                          (void*)(12 * sizeof(float)));

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void render::GridSquare::draw() const
{
    static const unsigned kVertexSize = sizeof(kVertices) / sizeof(float) / 3;
    glBindVertexArray(vbo_.mesh);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Draw
    glDrawArraysInstanced(
        GL_LINE_STRIP_ADJACENCY, 0, kVertexSize, vbo_.instanceCount);
}

void render::GridSquare::modify(const float* mat, unsigned instanceIndex)
{
    render::modify(vbo_, mat, instanceIndex);
}

void render::GridSquare::modify(const float* mat,
                                const unsigned* instanceIndices,
                                unsigned size)
{
    render::modify(vbo_, mat, instanceIndices, size);
}

void render::GridSquare::reset(const float* mat, unsigned size)
{
    render::reset(vbo_, mat, size);
}

void render::GridSquare::push_back(const float* mat)
{
    render::push_back(vbo_, mat);
}

void render::GridSquare::push_back(const float* mat, unsigned size)
{
    render::push_back(vbo_, mat, size);
}
