#pragma once

namespace render {
    /*! @brief Loads a texture from a raw data buffer
     */
    //! @return
    //!     Texture object
    unsigned load_texture_from_data(const unsigned char* mem,
                                    int memlen,
                                    bool alpha,
                                    bool flipVertically = true);

    /*! @brief Loads a texture from a texture file
     */
    //! @return
    //!     Texture object
    unsigned load_texture_from_file(const char* path,
                                    bool alpha,
                                    bool flipVertically = true);
} // namespace render
