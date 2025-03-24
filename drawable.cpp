#include "drawable.hpp"
#include "glad/glad.h"

void render::modify(vbo& refvbo, const float* mat, unsigned instanceIndex)
{
    static const unsigned kBytes = 16 * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, refvbo.instance);

    unsigned off = instanceIndex * kBytes;
    glBufferSubData(GL_ARRAY_BUFFER, off, kBytes, mat);
}

void render::modify(vbo& refvbo,
                    const float* mat,
                    const unsigned* instanceIndices,
                    unsigned count)
{
    static const unsigned kBytes = 16 * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, refvbo.instance);

    unsigned i = 0;
    for (; i != count; ++i) {
        unsigned off = instanceIndices[i] * kBytes;
        glBufferSubData(GL_ARRAY_BUFFER, off, kBytes, mat);
    }
}

void render::reset(vbo& refvbo, const float* mat, unsigned count)
{
    static const unsigned kSize = 16 * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, refvbo.instance);

    refvbo.instanceCount = count;
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * kSize, mat);
}

void render::push_back(vbo& refvbo, const float* mat)
{
    static const unsigned kBytes = 16 * sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, refvbo.instance);

    unsigned off = refvbo.instanceCount++ * kBytes;
    glBufferSubData(GL_ARRAY_BUFFER, off, kBytes, mat);
}

void render::push_back(vbo& refvbo, const float* mat, unsigned count)
{
    static const unsigned kBytes = 16 * sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, refvbo.instance);

    unsigned offset = refvbo.instanceCount * kBytes;
    glBufferSubData(GL_ARRAY_BUFFER, offset, count * kBytes, mat);
    refvbo.instanceCount += count;
}
