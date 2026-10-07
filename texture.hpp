#pragma once

namespace render {
    /*! @brief Loads a texture from a raw data buffer
     */
    //! @return
    //!     Texture object
    unsigned load_texture_from_data(const unsigned char* mem,
                                    unsigned memlen,
                                    bool flipVertically = true);

    /*! @brief Loads a texture from a texture file
     */
    //! @return
    //!     Texture object
    unsigned load_texture_from_file(const char* path,
                                    bool flipVertically = true);

    /*! @brief Cleans up texture associated with handle
     */
    void delete_texture(unsigned tao);
} // namespace render
