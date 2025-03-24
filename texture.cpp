#include "texture.hpp"
#include "glad/glad.h"
#include "stb/stb_image.h"
#include <cstdlib>
#include <cstring>
#include <new>

namespace {

    struct Texture {
        unsigned width = 0;
        unsigned height = 0;
        unsigned nchannels = 0;

        std::size_t buffsize = 0;
        unsigned char* data = nullptr;

        /*! Dtor.
         */
        ~Texture()
        {
            delete[] data;
        }

        /*! Ctor.
         */
        Texture(unsigned _width,
                unsigned _height,
                unsigned _nchannels,
                const unsigned char* _data)
            : width(_width)
            , height(_height)
            , nchannels(_nchannels)
        {
            unsigned datasize = _width * _height * _nchannels;
            // Copy data
            buffsize = datasize;
            data = new unsigned char[buffsize];

            std::memset(data, 0, buffsize);
            std::memcpy(data, _data, datasize);
        }

        Texture(Texture&&) = delete;
        Texture(const Texture&) = delete;

        Texture& operator=(Texture&&) = delete;
        Texture& operator=(const Texture&) = delete;
    };

    inline unsigned generate_texture(const Texture& t, GLenum format)
    {
        // Generate texture
        unsigned tao;
        glGenTextures(1, &tao);
        glBindTexture(GL_TEXTURE_2D, tao);

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE); // WebGL requirement
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     static_cast<GLint>(format),
                     static_cast<GLint>(t.width),
                     static_cast<GLint>(t.height),
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     t.data);

        glGenerateMipmap(GL_TEXTURE_2D);
        return (glBindTexture(GL_TEXTURE_2D, 0), tao);
    }
} // namespace

unsigned render::load_texture_from_data(const unsigned char* mem,
                                        unsigned memlen,
                                        bool alpha,
                                        bool flipVertically)
{
    int width = 0;
    int height = 0;
    int nchannels = 0;

    stbi_set_flip_vertically_on_load(static_cast<int>(flipVertically));
    unsigned char* data = stbi_load_from_memory(
        mem, static_cast<int>(memlen), &width, &height, &nchannels, 0);

    Texture t(static_cast<unsigned>(width),
              static_cast<unsigned>(height),
              static_cast<unsigned>(nchannels),
              data);
    return (stbi_image_free(data),
            generate_texture(t, alpha ? GL_RGBA : GL_RGB));
}

unsigned render::load_texture_from_file(const char* path,
                                        bool alpha,
                                        bool flipVertically)
{
    // Load image, create texture and generate mipmaps
    int width = 0;
    int height = 0;
    int nchannels = 0;

    // Load image data
    stbi_set_flip_vertically_on_load(static_cast<int>(flipVertically));
    unsigned char* data = stbi_load(path, &width, &height, &nchannels, 0);

    Texture t(static_cast<unsigned>(width),
              static_cast<unsigned>(height),
              static_cast<unsigned>(nchannels),
              data);
    return (stbi_image_free(data),
            generate_texture(t, alpha ? GL_RGBA : GL_RGB));
}
