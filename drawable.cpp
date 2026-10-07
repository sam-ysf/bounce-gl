#include "drawable.hpp"
#include "glad/glad.h"

void render::modify(const Vbo& refvbo, const float* mat, unsigned instanceIndex)
{
    static const unsigned kBytes = 16 * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, refvbo.vbo);

    unsigned off = instanceIndex * kBytes;
    glBufferSubData(GL_ARRAY_BUFFER, off, kBytes, mat);
}

void render::modify(const Vbo& refvbo,
                    const float* mat,
                    unsigned count,
                    const unsigned* instanceIndices)
{
    static const unsigned kBytes = 16 * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, refvbo.vbo);

    unsigned i = 0;
    for (; i != count; ++i) {
        unsigned off = instanceIndices[i] * kBytes;
        glBufferSubData(GL_ARRAY_BUFFER, off, kBytes, mat + (i * 16));
    }
}

void render::reset(Vbo& refvbo, const float* mat, unsigned count)
{
    static const unsigned kSize = 16 * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, refvbo.vbo);

    refvbo.instanceCount = count;
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * kSize, mat);
}

void render::push_back(Vbo& refvbo, const float* mat)
{
    static const unsigned kBytes = 16 * sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, refvbo.vbo);

    unsigned off = refvbo.instanceCount++ * kBytes;
    glBufferSubData(GL_ARRAY_BUFFER, off, kBytes, mat);
}

void render::push_back(Vbo& refvbo, const float* mat, unsigned count)
{
    static const unsigned kBytes = 16 * sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, refvbo.vbo);

    unsigned offset = refvbo.instanceCount * kBytes;
    glBufferSubData(GL_ARRAY_BUFFER, offset, count * kBytes, mat);
    refvbo.instanceCount += count;
}
