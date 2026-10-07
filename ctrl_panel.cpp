#include "ctrl_panel.hpp"
#include "box_data.hpp"
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
void CtrlPanel::render(BoxData& boxData,
                       Camera& camera,
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
    render_ball_subpanel(boxData, textures);

    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 30));

    // Background...
    render_background_subpanel();

    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 30));

    // Scene...
    render_scene_subpanel(camera);

    ImGui::End();

    // Render imgui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/*! Renders the ball subpanel.
 */
void CtrlPanel::render_ball_subpanel(
    BoxData& boxData,
    const std::vector<unsigned>& textures) const
{
    ImGui::Text("Box Properties");
    ImGui::Separator();

    // Control group
    for (unsigned i = 0; i != textures.size(); ++i) {
        if (unsigned k = textures[i];
            ImGui::ImageButton(reinterpret_cast<void*>(k), ImVec2(100, 100)))
            boxData.selectedSkin = i;
        if (i != (textures.size() - 1)) {
            ImGui::SameLine();
        }
    }

    ImGui::Separator();

    // Control group
    ImGui::SliderFloat("Box x-speed", boxData.speed.data(), 0.0F, 0.2F);
    ImGui::SliderFloat("Box y-speed", boxData.speed.data() + 1, 0.0F, 0.2F);
    ImGui::Separator();

    // Control group
    ImGui::SliderFloat(
        "Box x-axis turn rate", boxData.turnRate.data(), 0.0F, 2.5F);
    ImGui::SliderFloat(
        "Box y-axis turn rate", boxData.turnRate.data() + 1, 0.0F, 2.5F);
    ImGui::SliderFloat(
        "Box z-axis turn rate", boxData.turnRate.data() + 2, 0.0F, 2.5F);
    ImGui::Separator();

    // Control group
    if (ImGui::Button("Stop Box"))
        stop(boxData);
    ImGui::SameLine();
    if (ImGui::Button("Reset Box"))
        reset(boxData);
}

/*! Stops the ball.
 */
void CtrlPanel::stop(BoxData& boxData) const
{
    boxData.speed[0] = 0;
    boxData.speed[1] = 0;
    boxData.speed[2] = 0;
}

/*! Stops the ball and resets it to its original position.
 */
void CtrlPanel::reset(BoxData& boxData) const
{
    stop(boxData);
    boxData.turnRate[0] = 0;
    boxData.turnRate[1] = 0;
    boxData.turnRate[2] = 0;

    boxData.translation[0][3] = 0;
    boxData.translation[1][3] = 0;
    boxData.direction[0] = 1.0;
    boxData.direction[1] = 1.0;
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
void CtrlPanel::render_scene_subpanel(Camera& camera)
{
    ImGui::Text("Scene Properties");
    ImGui::Separator();

    // Scene angle...
    bool update;
    update = render_scene_angle_subpanel(camera);
    ImGui::Separator();

    // Camera position...
    update |= render_scene_position_subpanel(camera);
    ImGui::Separator();

    // Reset
    if (ImGui::Button("Reset Scene")) {
        update = true;

        camera.reset();
        pitch = camera.get_pitch();
        yaw = camera.get_yaw();
        roll = camera.get_roll();
    }

    // Maybe update camera
    if (update)
        camera.update();

    // Exit...
    ImGui::Separator();
    ImGui::Dummy(ImVec2(540, 0));
    ImGui::SameLine();
    if (ImGui::Button("Exit"))
        run = false;
}

/*! Renders the scene angle subpanel.
 */
bool CtrlPanel::render_scene_angle_subpanel(Camera& camera)
{
    constexpr float kEpsilon = 1e-6F;

    pitch = camera.get_pitch();
    yaw = camera.get_yaw();
    roll = camera.get_roll();

    ImGui::SliderFloat("Scene pitch angle", &pitch, 0.0F, 45.0F);
    ImGui::SliderFloat("Scene yaw angle", &yaw, -45.0F, 45.0F);
    ImGui::SliderFloat("Scene roll angle", &roll, -180.0F, 180.0F);

    if (std::abs(pitch - camera.get_pitch()) > kEpsilon
        || std::abs(yaw - camera.get_yaw()) > kEpsilon
        || std::abs(roll - camera.get_roll()) > kEpsilon) {
        camera.set_scene_rotation(pitch, yaw, roll);
        return true;
    }

    // Nothing to do...
    return false;
}

/*! Renders the scene position subpanel.
 */
bool CtrlPanel::render_scene_position_subpanel(Camera& camera) const
{
    calc::Vec3f position = ([&camera]() {
        calc::Vec3f value = camera.get_position();
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

    calc::Vec3f correctedPosition = position;
    correctedPosition[0] = -position[0];
    correctedPosition[2] = -position[2];

    camera.translate_to(correctedPosition);
    return true;
}
