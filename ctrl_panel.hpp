#pragma once

#include <SDL2/SDL.h>

// Fwd. decl.
struct BallData;
// Fwd. decl.
struct Camera;

struct CtrlPanel {
    SDL_Window* window;

    float pitch;
    float yaw;
    float roll;

    bool enableGrid;

    bool run;
    bool firstCall;

    float gridColor[3];
    float backgroundColor[3];

    /*! Ctor.
     */
    explicit CtrlPanel(SDL_Window* window);

    /*! @brief Renders entire panel
     */
    void render(BallData& refballData,
                Camera& refcamera,
                const unsigned* skinHandles,
                unsigned skinHandlesCount);

    void stop(BallData& refballData) const;

    void reset(BallData& refballData) const;

    /*! @brief Renders subpanel segment
     */
    void render_ball_subpanel(BallData& refballData,
                              const unsigned* textures,
                              unsigned textureCount) const;
    /*! @brief Renders subpanel segment
     */
    void render_background_subpanel();

    /*! @brief Renders subpanel segment
     */
    void render_scene_subpanel(Camera& refcamera);

    /*! @brief Renders subpanel segment
     */
    bool render_scene_angle_subpanel(Camera& refcamera);

    /*! @brief Renders subpanel segment
     */
    bool render_scene_position_subpanel(Camera& refcamera);
};
