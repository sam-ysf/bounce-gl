#pragma once

#include "drawable.hpp"

namespace render {
    //! class Square
    /*! Implements a textured square
     */
    class Square : public Drawable {
    public:
        Square() = default;

        //! @param taoSrc
        //!     Texture handle array
        //! @param taoCount
        //!     taoSrc size
        //! @param instanceSizeMax
        //!     The maximum # of instances to allocate
        Square(const unsigned* taoSrc,
               unsigned taoCount,
               unsigned instanceSizeMax);

        void draw() const override;

        void modify(const float* mat, unsigned instanceIndex) override;

        void modify(const float* mat,
                    const unsigned* instanceIndices,
                    unsigned count) override;

        void reset(const float* mat, unsigned count) override;

        void push_back(const float* mat) override;

        void push_back(const float* mat, unsigned count) override;
    private:
        // Texture handles
        tao tao_;
        // Vertex handles
        vbo vbo_;
    };
} // namespace render
