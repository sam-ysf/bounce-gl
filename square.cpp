#include "square.hpp"
#include "glad/glad.h"
#include <algorithm>
#include <cstring>

namespace {
    // Render::Square shape and texture vertices
    const std::array<float, 30> kVertices = {
        -0.5F, -0.5F, 0.0F, 0.0F, 0.0F,

        +0.5F, -0.5F, 0.0F, 1.0F, 0.0F,

        +0.5F, 0.5F,  0.0F, 1.0F, 1.0F,

        +0.5F, 0.5F,  0.0F, 1.0F, 1.0F,

        -0.5F, 0.5F,  0.0F, 0.0F, 1.0F,

        -0.5F, -0.5F, 0.0F, 0.0F, 0.0F,
    };
} // namespace

render::Square::Square(const unsigned* taoSrc,
                       unsigned taoCount,
                       unsigned instanceSizeMax)
{
    // Safely copy texture handles
    if (taoSrc != nullptr) {
        tao_.size = std::min<std::size_t>(tao::kTaoMax, taoCount);
        for (unsigned i = 0; i != tao_.size; ++i) {
            (tao_.tao)[i] = taoSrc[i];
        }
    }

    glGenVertexArrays(1, &vbo_.mesh);
    glBindVertexArray(vbo_.mesh);

    glGenBuffers(1, &vbo_.vertex);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_.vertex);

    // Add vertices
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(kVertices), kVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(float),
                          reinterpret_cast<void*>(3 * sizeof(float)));

    // Instancing
    glGenBuffers(1, &vbo_.instance);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_.instance);

    // Null buffer
    glBufferData(GL_ARRAY_BUFFER,
                 instanceSizeMax * 16 * sizeof(float),
                 nullptr,
                 GL_STREAM_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), nullptr);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          16 * sizeof(float),
                          reinterpret_cast<void*>(4 * sizeof(float)));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          16 * sizeof(float),
                          reinterpret_cast<void*>(8 * sizeof(float)));

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          16 * sizeof(float),
                          reinterpret_cast<void*>(12 * sizeof(float)));

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void render::Square::draw() const
{
    static const unsigned kVertexSize = std::size(kVertices) / 5;

    // Load textures...
    glBindVertexArray(0);
    glBindVertexArray(vbo_.mesh);
    glBindTexture(GL_TEXTURE_2D, 0);

    for (unsigned i = 0; i != tao_.size; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);

        unsigned tao = tao_.tao[i];
        glBindTexture(GL_TEXTURE_2D, tao);
    }

    // Draw...
    glDrawArraysInstanced(
        GL_TRIANGLES, 0, kVertexSize, static_cast<GLint>(vbo_.instanceCount));
}

void render::Square::modify(const float* mat, unsigned instanceIndex)
{
    render::modify(vbo_, mat, instanceIndex);
}

void render::Square::modify(const float* mat,
                            const unsigned* instanceIndices,
                            unsigned count)
{
    render::modify(vbo_, mat, instanceIndices, count);
}

void render::Square::reset(const float* mat, unsigned count)
{
    render::reset(vbo_, mat, count);
}

void render::Square::push_back(const float* mat)
{
    render::push_back(vbo_, mat);
}

void render::Square::push_back(const float* mat, unsigned count)
{
    render::push_back(vbo_, mat, count);
}
