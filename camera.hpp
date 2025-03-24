#pragma once

#include "calc/matrix.hpp"

//! struct ray
/*! Unnormalized ray.
 */
struct ray {
    float x, y;
    calc::vec3f origin;
    calc::vec3f direction;

    calc::vec4f fRay; //> Far ray.
    calc::vec4f nRay; //> Near ray.
};

//! class Camera
/*! Translatable and rotatable camera that calculates perspective projection and
 *! look-at transformations.
 */
class Camera {
public:
    //! Ctor.
    Camera(const calc::vec3f& eye,
           float fov,
           float zfar,
           float znear = 0.1,
           unsigned screenWidth = 0,
           unsigned screenHeight = 0);

    //! Transform
    void move(const calc::vec3f& direction);

    //! Transform
    void set_position(const calc::vec3f& direction);

    //! Transform
    void resize(int screenWidth, int screenHeight);

    //! Transform
    void set_scene_rotation(float pitch, float yaw, float roll);

    //! Transform
    void rotate_viewer(float pitch, float yaw, float roll);

    //! Undo all transforms
    void reset();

    //! @param
    //!     x, y Mouse coordinates
    //! @return
    //      Ray
    ray unproject(float x, float y) const;

    //! @param x, y Mouse coordinates.
    //! @return Ray.
    ray unproject(float x,
                  float y,
                  const calc::mat4f& lookAt,
                  const calc::mat4f& projection) const;

    //! Recalculates look-at and projection matrices;
    //! should always be called after moving or rotating the camera.
    void update();

    float get_pitch() const
    {
        return viewOrientation_.pitch;
    }

    float get_yaw() const
    {
        return viewOrientation_.yaw;
    }

    float get_roll() const
    {
        return viewOrientation_.roll;
    }

    float get_screen_width() const; //!> @return The stored screen width.
    float get_screen_height() const; //!> @return The stored screen height.

    const calc::vec3f& get_position() const; //!> @return The eye coordinates.

    const calc::mat4f& get_scene()
        const; //!> @return The projection x view matrix.
    const calc::mat4f& get_device_scene()
        const; //!> @return The projection x view matrix in column-major
               //! ordering.

    const calc::mat4f& get_look_at() const; //!> @return The view matrix.
    const calc::mat4f& get_device_look_at() const; //!> @return The view matrix.

    const calc::mat4f& get_projection()
        const; //!> @return The projection matrix.
    const calc::mat4f& get_device_projection()
        const; //!> @return The projection matrix.
private:
    void calc_look_at(); //> Helper.
    void calc_projection(); //> Helper.

    float screenWidth_; //> Screen width.
    float screenHeight_; //> Screen height.

    //! struct orientation
    /*! 3D space orientation.
     */
    struct orientation {
        float pitch = 0, yaw = 0, roll = 0;
    };

    orientation viewOrientation_; //> Current 3D orientation of the scene.
    orientation viewerOrientation_; //> Current 3D orientation of the
                                    // first-person viewer.

    //! struct fov
    /*! Field of view.
     */
    struct fov {
        float half, znear, zfar;
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
        calc::vec3f value;
        const calc::vec3f defaultValue;
        /*! Ctor.
         */
        explicit vector_pair(const calc::vec3f& value);
    };

    vector_pair E_; //> Eye position vector.
    vector_pair F_; //> Forward direction vector.
    vector_pair U_; //> Up direction vector.

    //! struct matrix_pair
    /*! Matrix and its (transposed) GPU device-friendly pair.
     */
    struct matrix_pair {
        calc::mat4f value, deviceValue;
    };

    matrix_pair lookAt_; //> View matrix.
    matrix_pair projection_; //> Perspective projection matrix.
    matrix_pair scene_; //> Perspective x view.
};
