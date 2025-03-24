#pragma once

#include "drawable.hpp"

namespace render {

    //! class GridSquare
    /*! Defines a square grid element
     */
    class GridSquare : public Drawable {
    public:
        GridSquare() = default;

        //! Ctor.
        //! @param instanceSizeMax
        //!     The maximum # of instances to allocate
        explicit GridSquare(unsigned instanceSizeMax);

        void draw() const override;

        void modify(const float* mat, unsigned instanceIndex) override;

        void modify(const float* mat,
                    const unsigned* instanceIndices,
                    unsigned size) override;

        void reset(const float* mat, unsigned size) override;

        void push_back(const float* mat) override;

        void push_back(const float* mat, unsigned size) override;
    private:
        // Vertex handles
        vbo vbo_;
    };
} // namespace render
