#pragma once

#include "drawable.hpp"

namespace render {

    //! class GridSquare
    /*! Defines a square grid element
     */
    class GridSquare : public Drawable {
    public:
        ~GridSquare() override;

        GridSquare() = default;

        //! Ctor.
        //! @param instanceSizeMax
        //!     The maximum # of instances to allocate
        explicit GridSquare(unsigned instanceSizeMax);

        void draw() const override;

        void modify(const float* mat, unsigned instanceIndex) override;

        void modify(const float* mat,
                    unsigned count,
                    const unsigned* instanceIndices) override;

        void reset(const float* mat, unsigned count) override;

        void push_back(const float* mat) override;

        void push_back(const float* mat, unsigned count) override;
    private:
        // Vertex handles
        Vbo vbo_;
    };
} // namespace render
