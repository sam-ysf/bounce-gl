#pragma once

#include <array>

namespace render {

    //! struct Tao
    /*! OpenGL textures
     */
    struct Tao {
        static constexpr unsigned kTaoMax = 1024;
        // Buffer of texture handles
        std::size_t size = 0;
        std::array<unsigned, kTaoMax> tao = {};
    };

    //! struct Vbo
    /*! OpenGL VBOs
     */
    struct Vbo {
        unsigned mesh = 0;
        unsigned vbo = 0;
        unsigned vertex = 0;
        unsigned instanceCount = 0;
    };

    //! class Drawable
    /*! Abstract interface for instance-based drawing of a single object type,
     *! implemented by instanced objects that are passed to the render pipeline.
     */
    class Drawable {
    public:
        //! Dtor.
        virtual ~Drawable() = default;

        //! Called by renderer to draw all stored object instances.
        virtual void draw() const = 0;

        //! @param mat
        //!     Model matrix.
        virtual void modify(const float* mat, unsigned instanceIndex) = 0;

        //! @param mat
        //!     Array of model matrices.
        //! @param count
        //!     Size of matrices array.
        virtual void modify(const float* mat,
                            unsigned count,
                            const unsigned* instanceIndices) = 0;

        //! Reinitializes all drawable instances of this class to the passed
        //! matrix array
        //! @param mat
        //!     Array of model matrices.
        //! @param count
        //!     Size of matrices array.
        virtual void reset(const float* mat, unsigned count) = 0;

        //! @param mat
        //!     Model matrix.
        virtual void push_back(const float* mat) = 0;

        //! @param mat
        //!     Array of model matrices.
        //! @param count
        //!     Size of matrices array.
        virtual void push_back(const float* mat, unsigned count) = 0;
    };

    /*! @brief Impl.
     */
    void modify(const Vbo& refvbo, const float* mat, unsigned instanceIndex);

    /*! @brief Impl.
     */
    void modify(const Vbo& refvbo,
                const float* mat,
                unsigned count,
                const unsigned* instanceIndices);

    /*! @brief Implementation.
     */
    void reset(Vbo& refvbo, const float* mat, unsigned count);

    /*! @brief Implementation.
     */
    void push_back(Vbo& refvbo, const float* mat);

    /*! @brief Implementation.
     */
    void push_back(Vbo& refvbo, const float* mat, unsigned count);
} // namespace render
