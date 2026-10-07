#pragma once

#include <SDL2/SDL.h>
#include <vector>

// Fwd. decl.
struct BoxData;
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
    void render(BoxData& boxData,
                Camera& camera,
                const std::vector<unsigned>& textures);

    /*! @brief Stops ball movement
     */
    void stop(BoxData& boxData) const;

    /*! @brief Resets ball and scene to their original poses
     */
    void reset(BoxData& boxData) const;

    /*! @brief Renders subpanel segment
     */
    void render_ball_subpanel(BoxData& boxData,
                              const std::vector<unsigned>& textures) const;

    /*! @brief Renders subpanel segment
     */
    void render_background_subpanel();

    /*! @brief Renders subpanel segment
     */
    void render_scene_subpanel(Camera& camera);

    /*! @brief Renders subpanel segment
     */
    bool render_scene_angle_subpanel(Camera& camera);

    /*! @brief Renders subpanel segment
     */
    bool render_scene_position_subpanel(Camera& camera) const;
};
