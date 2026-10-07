#pragma once

#include "calc/matrix.hpp"

//! struct Ray
/*! Unnormalized ray.
 */
struct Ray {
    float x = 0;
    float y = 0;
    calc::Vec3f origin;
    calc::Vec3f direction;

    calc::Vec4f fRay; //> Far ray.
    calc::Vec4f nRay; //> Near ray.
};

//! class Camera
/*! Translatable and rotatable camera that calculates perspective projection and
 *! look-at transformations.
 */
class Camera {
public:
    //! Ctor.
    Camera(const calc::Vec3f& eye,
           float screenWidth,
           float screenHeight,
           float fov,
           float zfar,
           float znear = 0.1F);

    //! Applies transformation
    void translate_to(const calc::Vec3f& position);

    //! Applies transformation
    void resize(unsigned screenWidth, unsigned screenHeight);

    //! Applies transformation
    void set_scene_rotation(float pitch, float yaw, float roll);

    //! Undoes all transforms
    void reset();

    //! @param
    //!     x, y Mouse coordinates
    //! @return
    //      Ray
    Ray unproject(float x, float y) const;

    //! @param
    //!     x, y Mouse coordinates
    //! @return
    //!     Ray
    Ray unproject(float x,
                  float y,
                  const calc::Mat4f& lookAt,
                  const calc::Mat4f& projection) const;

    //! Recalculates look-at and projection matrices;
    //! should always be called after moving or rotating the camera.
    void update();

    float get_pitch() const
    {
        return viewOrientation_.x();
    }

    float get_yaw() const
    {
        return viewOrientation_.y();
    }

    float get_roll() const
    {
        return viewOrientation_.z();
    }

    float get_screen_width() const; //!> @return The stored screen width.
    float get_screen_height() const; //!> @return The stored screen height.

    const calc::Vec3f& get_position() const; //!> @return The eye coordinates

    const calc::Mat4f& get_scene()
        const; //!> @return The projection x view matrix
    const calc::Mat4f& get_device_scene()
        const; //!> @return The projection x view matrix in column-major order

    const calc::Mat4f& get_look_at() const; //!> @return The view matrix
    const calc::Mat4f& get_device_look_at() const; //!> @return The view matrix

    const calc::Mat4f& get_projection()
        const; //!> @return The projection matrix
    const calc::Mat4f& get_device_projection()
        const; //!> @return The projection matrix
private:
    void calc_look_at(); //> Helper
    void calc_projection(); //> Helper

    float screenWidth_; //> Screen width
    float screenHeight_; //> Screen height

    calc::Vec3f viewOrientation_; //> Current 3D orientation of the scene.
    calc::Vec3f viewerOrientation_; //> Current 3D orientation of the
                                    // first-person viewer.

    //! struct fov
    /*! Field of view
     */
    struct fov {
        float half = 0;
        float znear = 0;
        float zfar = 0;
        fov(float half, float znear, float zfar)
            : half(half)
            , znear(znear)
            , zfar(zfar)
        {}
    };

    fov fov_; //> Current field of view.

    //! struct vector_pair
    /*! Vector and its default (initial) value.
     */
    struct vector_pair {
        calc::Vec3f value;
        const calc::Vec3f defaultValue;
        /*! Ctor.
         */
        explicit vector_pair(const calc::Vec3f& value);
    };

    vector_pair E_; //> Eye position vector.
    vector_pair F_; //> Forward direction vector.
    vector_pair U_; //> Up direction vector.

    //! struct matrix_pair
    /*! Matrix and its (transposed) GPU device-friendly pair.
     */
    struct matrix_pair {
        calc::Mat4f value;
        calc::Mat4f deviceValue;
    };

    matrix_pair lookAt_; //> View matrix.
    matrix_pair projection_; //> Perspective projection matrix.
    matrix_pair scene_; //> Perspective x view.
};
