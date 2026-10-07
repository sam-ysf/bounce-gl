#include "texture.hpp"
#include "glad/glad.h"
#include "stb/stb_image.h"
#include <cstdlib>
#include <cstring>

namespace {

    inline unsigned generate_texture(const unsigned char* data,
                                     int width,
                                     int height,
                                     GLenum format)
    {
        // Generate texture
        unsigned tao = 0;
        glGenTextures(1, &tao);
        glBindTexture(GL_TEXTURE_2D, tao);

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     static_cast<GLint>(format),
                     static_cast<GLint>(width),
                     static_cast<GLint>(height),
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     data);

        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        return tao;
    }
} // namespace

unsigned render::load_texture_from_data(const unsigned char* mem,
                                        unsigned memlen,
                                        bool flipVertically)
{
    int width = 0;
    int height = 0;
    int nchannels = 0;

    stbi_set_flip_vertically_on_load(static_cast<int>(flipVertically));
    unsigned char* data = stbi_load_from_memory(
        mem, static_cast<int>(memlen), &width, &height, &nchannels, 0);

    // Just in case
    if (!data) {
        return 0;
    }

    unsigned tao = 0;
    if (nchannels == 3) {
        tao = generate_texture(data, width, height, GL_RGB);
    } else if (nchannels == 4) {
        tao = generate_texture(data, width, height, GL_RGBA);
    }

    stbi_image_free(data);
    return tao;
}

unsigned render::load_texture_from_file(const char* path, bool flipVertically)
{
    // Load image, create texture and generate mipmaps
    int width = 0;
    int height = 0;
    int nchannels = 0;

    // Load image data
    stbi_set_flip_vertically_on_load(static_cast<int>(flipVertically));
    unsigned char* data = stbi_load(path, &width, &height, &nchannels, 0);

    if (!data) {
        return 0;
    }

    unsigned tao = 0;
    if (nchannels == 3) {
        tao = generate_texture(data, width, height, GL_RGB);
    } else if (nchannels == 4) {
        tao = generate_texture(data, width, height, GL_RGBA);
    }

    stbi_image_free(data);
    return tao;
}

void render::delete_texture(unsigned tao)
{
    glDeleteTextures(1, &tao);
}