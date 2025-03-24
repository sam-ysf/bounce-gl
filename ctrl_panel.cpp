#include "ctrl_panel.hpp"
#include "ball_data.hpp"
#include "calc/matrix_nxm.hpp"
#include "camera.hpp"
#include "dear_imgui/imgui.h"
#include "dear_imgui_backends/imgui_impl_opengl3.h"
#include "dear_imgui_backends/imgui_impl_sdl.h"
#include <cstdlib>

/*! Ctor.
 */
CtrlPanel::CtrlPanel(SDL_Window* window)
    : window(window)
{
    backgroundColor[0] = 0.63F;
    backgroundColor[1] = 0.58F;
    backgroundColor[2] = 0.10F;

    gridColor[0] = 0.75F;
    gridColor[1] = 0.75F;
    gridColor[2] = 0.75F;
}

/*! Renders the entire control panel.
 */
void CtrlPanel::render(BallData& refballData,
                       Camera& refcamera,
                       const std::vector<unsigned>& textures)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);

    ImGui::NewFrame();
    ImGui::Begin("Control Panel");

    if (firstCall) {
        firstCall = false;
        ImGui::SetWindowSize("Control Panel", ImVec2(600, 700));
    }

    // Box...
    render_ball_subpanel(refballData, textures);

    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 30));

    // Background...
    render_background_subpanel();

    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 30));

    // Scene...
    render_scene_subpanel(refcamera);

    ImGui::End();

    // Render imgui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/*! Renders the ball subpanel.
 */
void CtrlPanel::render_ball_subpanel(
    BallData& refballData,
    const std::vector<unsigned>& textures) const
{
    ImGui::Text("Box Properties");
    ImGui::Separator();

    // Control group
    for (unsigned i = 0; i != textures.size(); ++i) {
        if (unsigned k = textures[i];
            ImGui::ImageButton(reinterpret_cast<void*>(k), ImVec2(100, 100)))
            refballData.selectedSkin = i;
        if (i != (textures.size() - 1)) {
            ImGui::SameLine();
        }
    }

    ImGui::Separator();

    // Control group
    ImGui::SliderFloat("Box x-speed", refballData.speed.data(), 0.0F, 0.2F);
    ImGui::SliderFloat("Box y-speed", refballData.speed.data() + 1, 0.0F, 0.2F);
    ImGui::Separator();

    // Control group
    ImGui::SliderFloat(
        "Box x-axis turn rate", refballData.turnRate.data(), 0.0F, 2.5F);
    ImGui::SliderFloat(
        "Box y-axis turn rate", refballData.turnRate.data() + 1, 0.0F, 2.5F);
    ImGui::SliderFloat(
        "Box z-axis turn rate", refballData.turnRate.data() + 2, 0.0F, 2.5F);
    ImGui::Separator();

    // Control group
    if (ImGui::Button("Stop Box"))
        stop(refballData);
    ImGui::SameLine();
    if (ImGui::Button("Reset Box"))
        reset(refballData);
}

/*! Stops the ball.
 */
void CtrlPanel::stop(BallData& refballData) const
{
    refballData.speed[0] = 0;
    refballData.speed[1] = 0;
    refballData.speed[2] = 0;
}

/*! Stops the ball and resets it to its original position.
 */
void CtrlPanel::reset(BallData& refballData) const
{
    stop(refballData);
    refballData.turnRate[0] = 0;
    refballData.turnRate[1] = 0;
    refballData.turnRate[2] = 0;

    refballData.translation[0][3] = 0;
    refballData.translation[1][3] = 0;
    refballData.direction[0] = 1.0;
    refballData.direction[1] = 1.0;
}

/*! Renders the background subpanel.
 */
void CtrlPanel::render_background_subpanel()
{
    ImGui::Text("Background Properties");
    ImGui::Separator();

    // Background color control
    ImGui::ColorEdit3("Background color", backgroundColor);
    ImGui::Separator();

    // Grid color control
    ImGui::ColorEdit3("Grid color", gridColor);
    ImGui::Checkbox("Enable Grid", &enableGrid);
}

/*! Renders the scene subpanel.
 */
void CtrlPanel::render_scene_subpanel(Camera& refcamera)
{
    ImGui::Text("Scene Properties");
    ImGui::Separator();

    // Scene angle...
    bool update;
    update = render_scene_angle_subpanel(refcamera);
    ImGui::Separator();

    // Camera position...
    update |= render_scene_position_subpanel(refcamera);
    ImGui::Separator();

    // Reset
    if (ImGui::Button("Reset Scene")) {
        update = true;

        refcamera.reset();
        pitch = refcamera.get_pitch();
        yaw = refcamera.get_yaw();
        roll = refcamera.get_roll();
    }

    // Maybe update camera
    if (update)
        refcamera.update();

    // Exit...
    ImGui::Separator();
    ImGui::Dummy(ImVec2(540, 0));
    ImGui::SameLine();
    if (ImGui::Button("Exit"))
        run = false;
}

/*! Renders the scene angle subpanel.
 */
bool CtrlPanel::render_scene_angle_subpanel(Camera& refcamera)
{
    constexpr float kEpsilon = 1e-6F;

    pitch = refcamera.get_pitch();
    yaw = refcamera.get_yaw();
    roll = refcamera.get_roll();

    ImGui::SliderFloat("Scene pitch angle", &pitch, 0.0F, 45.0F);
    ImGui::SliderFloat("Scene yaw angle", &yaw, -45.0F, 45.0F);
    ImGui::SliderFloat("Scene roll angle", &roll, -180.0F, 180.0F);

    if (std::abs(pitch - refcamera.get_pitch()) > kEpsilon
        || std::abs(yaw - refcamera.get_yaw()) > kEpsilon
        || std::abs(roll - refcamera.get_roll()) > kEpsilon) {
        refcamera.set_scene_rotation(pitch, yaw, roll);
        return true;
    }

    // Nothing to do...
    return false;
}

/*! Renders the scene position subpanel.
 */
bool CtrlPanel::render_scene_position_subpanel(Camera& refcamera) const
{
    calc::vec3f position = ([&refcamera]() {
        calc::vec3f value = refcamera.get_position();
        value[0] = -value[0]; // Invert
        value[2] = -value[2]; // Invert
        if (value[0] == -0)
            value[0] = 0;
        if (value[1] == -0)
            value[1] = 0;
        if (value[2] == -0)
            value[2] = 0;
        return value;
    }());

    ImGui::SliderFloat("Viewer x-position", position.data(), -10, 10);
    ImGui::SliderFloat("Viewer y-position", position.data() + 1, -10, 10);
    ImGui::SliderFloat("Viewer z-position", position.data() + 2, 10, 40);

    calc::vec3f correctedPosition = position;
    correctedPosition[0] = -position[0];
    correctedPosition[2] = -position[2];

    refcamera.translate_to(correctedPosition);
    return true;
}
