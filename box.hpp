#pragma once

#include "drawable.hpp"

namespace render {

    //! class box
    /*! Implements a textured box
     */
    class Box : public Drawable {
    public:
        //! Dtor.
        ~Box() override;

        //! Ctor.
        Box() = default;

        //! Ctor.
        //! @param taoSrc
        //!     Texture handle array
        //! @param taoCount
        //!     Size of texture handle array
        //! @param instanceSizeMax
        //!     The maximum # of instances to allocate
        Box(const unsigned* taoSrc,
            unsigned taoCount,
            unsigned instanceSizeMax);

        void draw() const override;

        void modify(const float* mat, unsigned instanceIndex) override;

        void modify(const float* mat,
                    unsigned count,
                    const unsigned* instanceIndices) override;

        void reset(const float* mat, unsigned count) override;

        void push_back(const float* mat) override;

        void push_back(const float* mat, unsigned count) override;
    private:
        // Texture handles
        Tao tao_;
        // Vertex handles
        Vbo vbo_;
    };
} // namespace render
