#pragma once

namespace render {

    //! struct tao
    /*! OpenGL textures
     */
    struct tao {
        unsigned tao[1024], size;
    };

    //! struct vbo
    /*! OpenGL VBOs
     */
    struct vbo {
        unsigned mesh, instance, vertex, instanceCount;
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
        //! @param size
        //!     Size of array.
        virtual void modify(const float* mat,
                            const unsigned* instanceIndices,
                            unsigned size)
            = 0;

        //! Reinitializes all drawable instances of this class to the passed
        //! matrix array
        //! @param mat
        //!     Array of model matrices.
        //! @param size
        //!     Size of array.
        virtual void reset(const float* mat, unsigned size) = 0;

        //! @param mat
        //!     Model matrix.
        virtual void push_back(const float* mat) = 0;

        //! @param mat
        //!     Array of model matrices.
        //! @param size
        //!     Size of array.
        virtual void push_back(const float* mat, unsigned size) = 0;
    };

    /*! @brief Impl.
     */
    void modify(vbo& refvbo, const float* mat, unsigned instanceIndex);

    /*! @brief Impl.
     */
    void modify(vbo& refvbo,
                const float* mat,
                const unsigned* instanceIndices,
                unsigned count);

    /*! @brief Implementation.
     */
    void reset(vbo& refvbo, const float* mat, unsigned count);

    /*! @brief Implementation.
     */
    void push_back(vbo& refvbo, const float* mat);

    /*! @brief Implementation.
     */
    void push_back(vbo& refvbo, const float* mat, unsigned count);
} // namespace render
