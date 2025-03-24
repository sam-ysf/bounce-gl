#include "texture.hpp"
#include "glad/glad.h"
#include "stb/stb_image.h"
#include <cstdlib>
#include <cstring>
#include <new>

namespace {

    struct texture {
        int width;
        int height;
        int nchannels;

        std::size_t buffsize;
        unsigned char* data;

        /*! Dtor.
         */
        ~texture()
        {
            std::free(data);
        }

        /*! Ctor.
         */
        texture(int width, int height, int nchannels, const unsigned char* data)
            : width(width)
            , height(height)
            , nchannels(nchannels)
        {
            unsigned datasize = width * height * nchannels;
            // Copy data
            buffsize = datasize;
            this->data = static_cast<unsigned char*>(::malloc(buffsize));
            if (this->data == nullptr) {
                throw std::bad_alloc();
            }
            std::memset(this->data, 0, buffsize);
            std::memcpy(this->data, data, datasize);
        }
    };

    unsigned generate_texture(const texture& t, int format)
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
                     format,
                     t.width,
                     t.height,
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     t.data);

        glGenerateMipmap(GL_TEXTURE_2D);
        return (glBindTexture(GL_TEXTURE_2D, 0), tao);
    }
} // namespace

unsigned render::load_texture_from_data(const unsigned char* mem,
                                        int memlen,
                                        bool alpha,
                                        bool flipVertically)
{
    int width = 0;
    int height = 0;
    int nchannels = 0;

    stbi_set_flip_vertically_on_load(static_cast<int>(flipVertically));
    unsigned char* data
        = stbi_load_from_memory(mem, memlen, &width, &height, &nchannels, 0);

    texture t(width, height, nchannels, data);
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

    texture t(width, height, nchannels, data);
    return (stbi_image_free(data),
            generate_texture(t, alpha ? GL_RGBA : GL_RGB));
}
