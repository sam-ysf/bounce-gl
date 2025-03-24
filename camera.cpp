#include "camera.hpp"
#include "calc/matrix_operation.hpp"
#include "calc/matrix_transform.hpp"
#include <cmath>

Camera::vector_pair::vector_pair(const calc::vec3f& value)
    : value(value)
    , defaultValue(value)
{}

Camera::Camera(const calc::vec3f& eye,
               float screenWidth,
               float screenHeight,
               float fov,
               float zfar,
               float znear)
    : screenWidth_(screenWidth)
    , screenHeight_(screenHeight)
    , viewOrientation_(0, 0, 0)
    , viewerOrientation_(0, 0, 0)
    , fov_(fov, znear, zfar)
    , E_(eye)
    , F_(calc::vec3f(0, 0, 1))
    , U_(calc::vec3f(0, 1, 0))
{
    update();
}

void Camera::translate_to(const calc::vec3f& position)
{
    const calc::mat3f rot
        = calc::rotate_3x(calc::radians(viewerOrientation_.x())) // Pitch
          * calc::rotate_3y(calc::radians(viewerOrientation_.y())) // Yaw
          * calc::rotate_3z(calc::radians(viewerOrientation_.z())); // Roll

    E_.value = rot * position;
}

void Camera::reset()
{
    viewOrientation_ = calc::vec3f(0, 0, 0);
    viewerOrientation_ = calc::vec3f(0, 0, 0);

    E_.value = E_.defaultValue;
    F_.value = F_.defaultValue;
    U_.value = U_.defaultValue;
}

void Camera::resize(unsigned screenWidth, unsigned screenHeight)
{
    screenWidth_ = static_cast<float>(screenWidth);
    screenHeight_ = static_cast<float>(screenHeight);
}

void Camera::set_scene_rotation(const float pitch,
                                const float yaw,
                                const float roll)
{
    viewOrientation_[0] = pitch;
    viewOrientation_[1] = yaw;
    viewOrientation_[2] = roll;
}

void Camera::calc_look_at()
{
    // Forward
    const calc::vec3f f = calc::normal(F_.value);
    // Left
    const calc::vec3f s = calc::normal(calc::cross(f, U_.value));
    // Up (recalculated and normalized)
    const calc::vec3f u = calc::cross(s, f);

    // Calculate the transposed rotation matrix
    calc::mat4f lookAt = calc::mat4f::identity();

    lookAt(0, 0) = s[0];
    lookAt(0, 1) = s[1];
    lookAt(0, 2) = s[2];

    lookAt(1, 0) = u[0];
    lookAt(1, 1) = u[1];
    lookAt(1, 2) = u[2];

    lookAt(2, 0) = -f[0];
    lookAt(2, 1) = -f[1];
    lookAt(2, 2) = -f[2];

    lookAt(0, 3) = -calc::dot(s, E_.value);
    lookAt(1, 3) = -calc::dot(u, E_.value);
    lookAt(2, 3) = +calc::dot(f, E_.value);

    lookAt_.value = lookAt
                    * calc::rotate_4x(calc::radians(viewOrientation_.x()))
                    * calc::rotate_4y(calc::radians(viewOrientation_.y()))
                    * calc::rotate_4z(calc::radians(viewOrientation_.z()));
    lookAt_.deviceValue = calc::transpose(lookAt_.value);
}

void Camera::calc_projection()
{
    const float aspect = screenWidth_ / screenHeight_;
    const float tanHalfFOV = std::tan(calc::radians(fov_.half));

    const float zfar = fov_.zfar;
    const float znear = fov_.znear;

    projection_.value(0, 0) = 1.0F / (tanHalfFOV * aspect);
    projection_.value(1, 1) = 1.0F / tanHalfFOV;
    projection_.value(2, 2) = -(zfar + znear) / (zfar - znear);
    projection_.value(2, 3) = (-2.0F * zfar * znear) / (zfar - znear);
    projection_.value(3, 2) = -1.0F;
    projection_.value(3, 3) = 0.0F;

    projection_.deviceValue = calc::transpose(projection_.value);
}

namespace {

    /*! @brief Helper, returns inverse of input matrix
     */
    calc::mat4f inverse(const calc::mat4f& m)
    {
        float coeff00 = (m[2][2] * m[3][3]) - (m[3][2] * m[2][3]);
        float coeff01 = (m[2][1] * m[3][3]) - (m[3][1] * m[2][3]);
        float coeff02 = (m[2][1] * m[3][2]) - (m[3][1] * m[2][2]);
        float coeff03 = (m[2][0] * m[3][3]) - (m[3][0] * m[2][3]);
        float coeff04 = (m[2][0] * m[3][2]) - (m[3][0] * m[2][2]);
        float coeff05 = (m[2][0] * m[3][1]) - (m[3][0] * m[2][1]);
        float coeff06 = (m[1][2] * m[3][3]) - (m[3][2] * m[1][3]);
        float coeff07 = (m[1][1] * m[3][3]) - (m[3][1] * m[1][3]);
        float coeff08 = (m[1][1] * m[3][2]) - (m[3][1] * m[1][2]);
        float coeff09 = (m[1][0] * m[3][3]) - (m[3][0] * m[1][3]);
        float coeff10 = (m[1][0] * m[3][2]) - (m[3][0] * m[1][2]);
        float coeff11 = (m[1][0] * m[3][1]) - (m[3][0] * m[1][1]);
        float coeff12 = (m[1][2] * m[2][3]) - (m[2][2] * m[1][3]);
        float coeff13 = (m[1][1] * m[2][3]) - (m[2][1] * m[1][3]);
        float coeff14 = (m[1][1] * m[2][2]) - (m[2][1] * m[1][2]);
        float coeff15 = (m[1][0] * m[2][3]) - (m[2][0] * m[1][3]);
        float coeff16 = (m[1][0] * m[2][2]) - (m[2][0] * m[1][2]);
        float coeff17 = (m[1][0] * m[2][1]) - (m[2][0] * m[1][1]);

        calc::mat4f inverse;
        inverse[0][0] = +(((m[1][1] * coeff00) - (m[1][2] * coeff01))
                          + (m[1][3] * coeff02));
        inverse[0][1] = -(((m[1][0] * coeff00) - (m[1][2] * coeff03))
                          + (m[1][3] * coeff04));
        inverse[0][2] = +(((m[1][0] * coeff01) - (m[1][1] * coeff03))
                          + (m[1][3] * coeff05));
        inverse[0][3] = -(((m[1][0] * coeff02) - (m[1][1] * coeff04))
                          + (m[1][2] * coeff05));

        inverse[1][0] = -(((m[0][1] * coeff00) - (m[0][2] * coeff01))
                          + (m[0][3] * coeff02));
        inverse[1][1] = +(((m[0][0] * coeff00) - (m[0][2] * coeff03))
                          + (m[0][3] * coeff04));
        inverse[1][2] = -(((m[0][0] * coeff01) - (m[0][1] * coeff03))
                          + (m[0][3] * coeff05));
        inverse[1][3] = +(((m[0][0] * coeff02) - (m[0][1] * coeff04))
                          + (m[0][2] * coeff05));

        inverse[2][0] = +(((m[0][1] * coeff06) - (m[0][2] * coeff07))
                          + (m[0][3] * coeff08));
        inverse[2][1] = -(((m[0][0] * coeff06) - (m[0][2] * coeff09))
                          + (m[0][3] * coeff10));
        inverse[2][2] = +(((m[0][0] * coeff07) - (m[0][1] * coeff09))
                          + (m[0][3] * coeff11));
        inverse[2][3] = -(((m[0][0] * coeff08) - (m[0][1] * coeff10))
                          + (m[0][2] * coeff11));

        inverse[3][0] = -(((m[0][1] * coeff12) - (m[0][2] * coeff13))
                          + (m[0][3] * coeff14));
        inverse[3][1] = +(((m[0][0] * coeff12) - (m[0][2] * coeff15))
                          + (m[0][3] * coeff16));
        inverse[3][2] = -(((m[0][0] * coeff13) - (m[0][1] * coeff15))
                          + (m[0][3] * coeff17));
        inverse[3][3] = +(((m[0][0] * coeff14) - (m[0][1] * coeff16))
                          + (m[0][2] * coeff17));

        float determinant
            = +(m[0][0] * inverse[0][0]) + (m[0][1] * inverse[0][1])
              + (m[0][2] * inverse[0][2]) + (m[0][3] * inverse[0][3]);
        if (determinant < 1e-6F)
            determinant = 1e-6F * (determinant / std::abs(determinant));

        inverse /= determinant;

        return calc::transpose(inverse);
    }

    // Helper
    ray unproject_impl(float x,
                       float y,
                       float screenWidth,
                       float screenHeight,
                       const calc::mat4f& scene)
    {
        y = screenHeight - y - 1;

        x = ((2 * x) / screenWidth) - 1;
        y = ((2 * y) / screenHeight) - 1;

        const calc::mat4f inv = inverse(scene);

        ray r;
        r.x = x;
        r.y = y;

        r.fRay = inv * calc::vec4f(x, y, 1, 1);
        r.nRay = inv * calc::vec4f(x, y, 0, 1);

        r.fRay = r.fRay / r.fRay[3];
        r.nRay = r.nRay / r.nRay[3];

        const calc::vec4f dir = calc::normal(r.fRay - r.nRay);

        r.origin = calc::vec3f(r.nRay[0], r.nRay[1], r.nRay[2]);
        r.direction = calc::vec3f(dir[0], dir[1], dir[2]);
        return r;
    }
} // namespace

ray Camera::unproject(float x, float y) const
{
    const calc::mat4f& scene = scene_.value;
    return unproject_impl(x, y, screenWidth_, screenHeight_, scene);
}

ray Camera::unproject(float x,
                      float y,
                      const calc::mat4f& lookAt,
                      const calc::mat4f& projection) const
{
    const calc::mat4f scene = projection * lookAt;
    return unproject_impl(x, y, screenWidth_, screenHeight_, scene);
}

void Camera::update()
{
    calc_look_at();
    calc_projection();

    scene_.value = projection_.value * lookAt_.value;
    scene_.deviceValue = calc::transpose(scene_.value);
}

float Camera::get_screen_width() const
{
    return screenWidth_;
}

float Camera::get_screen_height() const
{
    return screenHeight_;
}

const calc::vec3f& Camera::get_position() const
{
    return E_.value;
}

const calc::mat4f& Camera::get_scene() const
{
    return scene_.value;
}

const calc::mat4f& Camera::get_device_scene() const
{
    return scene_.deviceValue;
}

const calc::mat4f& Camera::get_look_at() const
{
    return lookAt_.value;
}

const calc::mat4f& Camera::get_device_look_at() const
{
    return lookAt_.deviceValue;
}

const calc::mat4f& Camera::get_projection() const
{
    return projection_.value;
}

const calc::mat4f& Camera::get_device_projection() const
{
    return projection_.deviceValue;
}
