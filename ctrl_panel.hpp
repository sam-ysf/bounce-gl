#pragma once

#include <SDL2/SDL.h>
#include <vector>

// Fwd. decl.
struct BallData;
// Fwd. decl.
class Camera;

struct CtrlPanel {
    SDL_Window* window;

    float pitch = 0;
    float yaw = 0;
    float roll = 0;

    bool enableGrid = true;

    bool run = true;
    bool firstCall = true;

    float gridColor[3] = {};
    float backgroundColor[3] = {};

    /*! Ctor.
     */
    explicit CtrlPanel(SDL_Window* window);

    /*! @brief Renders entire panel
     */
    void render(BallData& refballData,
                Camera& refcamera,
                const std::vector<unsigned>& textures);

    /*! @brief Stops ball movement
     */
    void stop(BallData& refballData) const;

    /*! @brief Resets ball and scene to their original poses
     */
    void reset(BallData& refballData) const;

    /*! @brief Renders subpanel segment
     */
    void render_ball_subpanel(BallData& refballData,
                              const std::vector<unsigned>& textures) const;

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
    bool render_scene_position_subpanel(Camera& refcamera) const;
};
