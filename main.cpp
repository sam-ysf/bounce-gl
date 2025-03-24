#include "ball_data.hpp"
#include "box.hpp"
#include "calc/matrix_nxm.hpp"
#include "calc/matrix_operation.hpp"
#include "calc/matrix_transform.hpp"
#include "camera.hpp"
#include "ctrl_panel.hpp"
#include "dear_imgui/imgui.h"
#include "dear_imgui_backends/imgui_impl_opengl3.h"
#include "dear_imgui_backends/imgui_impl_sdl.h"
#include "draw_instanced_no_texture.hpp"
#include "draw_instanced_with_texture.hpp"
#include "glad/glad.h"
#include "grid_square.hpp"
#include "images/awesome_face.h"
#include "images/brick_wall.h"
#include "images/incredulous_face.h"
#include "images/shocked_face.h"
#include "images/tiles/dark_grass.h"
#include "images/tiles/dry_grass.h"
#include "program.hpp"
#include "square.hpp"
#include "texture.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

namespace {

    /*! Parameters that are initialized by SDL
     */
    struct SDLParam {
        SDL_Window* window;
        SDL_GLContext context;
    };

    /*! Initializes SDL
     */
    bool init_sdl(SDLParam& params, unsigned widthHint, unsigned heightHint)
    {
        // Initialize sdl
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
            printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
            return false;
        }

        // GL 3.0 + GLSL 130
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        // Create window with graphics context
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        auto flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_OPENGL
                                                  | SDL_WINDOW_RESIZABLE
                                                  | SDL_WINDOW_ALLOW_HIGHDPI);
        params.window = SDL_CreateWindow("Bounce",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         static_cast<int>(widthHint),
                                         static_cast<int>(heightHint),
                                         flags);
        if (params.window == nullptr) {
            printf("Window could not be created! SDL Error: %s\n",
                   SDL_GetError());
            return false;
        }

        // Create context
        params.context = SDL_GL_CreateContext(params.window);
        if (params.context == nullptr) {
            printf("OpenGL context could not be created! SDL Error: %s\n",
                   SDL_GetError());
            return false;
        }

        SDL_GL_MakeCurrent(params.window, params.context);

        return true;
    }
} // namespace

namespace {

    /*! Helper
     *! Builds the vertices for the map grid
     */
    std::vector<calc::mat4f> build_grid(unsigned width, unsigned length)
    {
        const auto xinst
            = static_cast<int>(std::ceil(static_cast<float>(width) / 2.0F));
        const auto yinst
            = static_cast<int>(std::ceil(static_cast<float>(length) / 2.0F));

        calc::mat4f model = calc::mat4f::identity();
        float& x = model[3][0];
        float& y = model[3][1];

        std::vector<calc::mat4f> grid;
        for (int i = -xinst; i != xinst; ++i) {
            for (int j = -yinst; j != yinst; ++j) {
                x = static_cast<float>(i) + 0.5F;
                y = static_cast<float>(j) + 0.5F;
                grid.push_back(model);
            }
        }

        return grid;
    }

    /*! Helper
     *! Build the vertices for the map wall
     */
    std::vector<calc::mat4f> build_wall(unsigned width, unsigned length)
    {
        std::vector<calc::mat4f> wall;

        const int lengthSpan = (length / 2) / 3;
        const int widthSpan = (width / 2) / 3;

        // West wall
        for (int i = 1 - lengthSpan; i != lengthSpan; ++i) {
            calc::mat4f mat = calc::mat4f::identity();
            mat[0][0] = 3;
            mat[1][1] = 3;

            mat[0][3] = (static_cast<float>(width) / 2.0F) - 1.0F;
            mat[1][3] = static_cast<float>(i) * 3.0F;
            mat[2][3] = -1.0;
            wall.push_back(calc::transpose(mat));
        }

        // East wall
        for (int i = 1 - lengthSpan; i != lengthSpan; ++i) {
            calc::mat4f mat = calc::mat4f::identity();
            mat[0][0] = 3;
            mat[1][1] = 3;

            mat[0][3] = 1 - (static_cast<float>(width) / 2.0F);
            mat[1][3] = static_cast<float>(i) * 3.0F;
            mat[2][3] = -1.0;
            wall.push_back(calc::transpose(mat));
        }

        // North wall
        for (int i = 1 - widthSpan; i != widthSpan; ++i) {
            calc::mat4f mat = calc::mat4f::identity();
            mat[0][0] = 3;
            mat[1][1] = 3;

            mat[0][3] = static_cast<float>(i) * 3.0F;
            mat[1][3] = (static_cast<float>(length) / 2.0F) - 1;
            mat[2][3] = -1.0;
            wall.push_back(calc::transpose(mat));
        }

        // South wall
        for (int i = 1 - widthSpan; i != widthSpan; ++i) {
            calc::mat4f mat = calc::mat4f::identity();
            mat[0][0] = 3;
            mat[1][1] = 3;

            mat[0][3] = static_cast<float>(i) * 3.0F;
            mat[1][3] = 1 - (static_cast<float>(length) / 2.0F);
            mat[2][3] = -1.0;
            wall.push_back(calc::transpose(mat));
        }

        return wall;
    }

    /*! Helper
     *! Converts matrix to float data
     */
    inline std::vector<float> copy_matrix_data(
        const std::vector<calc::mat4f>& mats)
    {
        std::vector<float> values(mats.size() * 16);
        for (::size_t i = 0; i != mats.size(); ++i)
            std::memcpy(
                &values[i * 16], calc::data(mats[i]), sizeof(calc::mat4f));
        return values;
    }
} // namespace

namespace {

    /*! Class Runner
     *! Encapsulates the main loop
     */
    class Runner {
    public:
        /*! ctor.
         */
        Runner(SDL_Window* window, Camera* camera)
            : window_(window)
            , panel_(window)
            , camera_(camera)
        {
            static const unsigned kWidth = 30;
            static const unsigned kHeight = 30;

            // Load boxes
            unsigned boxTAO1[]
                = {render::load_texture_from_data(
                       kBrickWallPng, kBrickWallPngLen, false),
                   render::load_texture_from_data(
                       kAwesomeFacePng, kAwesomeFacePngLen, true)};

            unsigned boxTAO2[]
                = {boxTAO1[0],
                   render::load_texture_from_data(
                       kShockedFacePng, kShockedFacePngLen, true)};

            unsigned boxTAO3[]
                = {boxTAO1[0],
                   render::load_texture_from_data(
                       kIncredulousFacePng, kIncredulousFacePngLen, true)};

            wallTAO_ = {
                boxTAO1[0],
                boxTAO1[0],
            };

            textureHandles_.push_back(render::load_texture_from_data(
                kAwesomeFacePng, kAwesomeFacePngLen, true, false));
            textureHandles_.push_back(render::load_texture_from_data(
                kShockedFacePng, kShockedFacePngLen, true, false));
            textureHandles_.push_back(render::load_texture_from_data(
                kIncredulousFacePng, kIncredulousFacePngLen, true, false));

            ballObject_[0] = render::Box(boxTAO1, std::size(boxTAO1), 1);
            ballObject_[0].push_back((calc::mat4f::identity()).data());

            ballObject_[1] = render::Box(boxTAO2, std::size(boxTAO2), 1);
            ballObject_[1].push_back((calc::mat4f::identity()).data());

            ballObject_[2] = render::Box(boxTAO3, std::size(boxTAO3), 1);
            ballObject_[2].push_back((calc::mat4f::identity()).data());

            // Load map...
            ranges_ = Ranges(kWidth, kHeight);

            // Load tiles
            load_grid();

            // Load tiles
            load_wall();

            unsigned dryGrassTextureTAO = render::load_texture_from_data(
                kDryGrassPng, kDryGrassPngLen, false);
            dryGrassTileTAO_ = {dryGrassTextureTAO, dryGrassTextureTAO};

            // Load tiles
            load_dry_grass();

            // Load tiles
            load_fresh_grass();
        }

        void load_grid()
        {
            const std::vector<float> grid = copy_matrix_data(
                build_grid(ranges_.gridWidth, ranges_.gridLength));
            gridTile_
                = render::GridSquare(ranges_.gridWidth * ranges_.gridLength);
            gridTile_.reset(grid.data(),
                            static_cast<unsigned>(grid.size() / 16));
        }

        void load_wall()
        {
            // Load wall
            const std::vector<float> wall = copy_matrix_data(
                build_wall(ranges_.cageWidth, ranges_.cageLength));
            wallObject_
                = render::Box(wallTAO_.value,
                              static_cast<unsigned>(std::size(wallTAO_.value)),
                              ranges_.cageWidth * ranges_.cageLength);
            wallObject_.reset(wall.data(),
                              static_cast<unsigned>(wall.size() / 16));
        }

        void load_dry_grass()
        {
            const int cageMinWidth = ranges_.cageMinWidth;
            const int cageMaxWidth = ranges_.cageMaxWidth;

            const int cageMinLength = ranges_.cageMinLength;
            const int cageMaxLength = ranges_.cageMaxLength;

            const int gridMinWidth = ranges_.gridMinWidth;
            const int gridMaxWidth = ranges_.gridMaxWidth;

            const int gridMinLength = ranges_.gridMinLength;
            const int gridMaxLength = ranges_.gridMaxLength;

            dryGrassTile_ = render::Square(
                dryGrassTileTAO_.value,
                static_cast<unsigned>(std::size(dryGrassTileTAO_.value)),
                ranges_.gridWidth * ranges_.gridLength);
            calc::mat4f mat = calc::mat4f::identity();

            // Top field
            for (int i = cageMaxLength; i <= gridMaxLength; ++i) {
                for (int j = gridMinWidth; j <= gridMaxWidth; ++j) {
                    mat[3][0] = static_cast<float>(j);
                    mat[3][1] = static_cast<float>(i);
                    dryGrassTile_.push_back(mat.data());
                }
            }

            // Right field
            for (int i = cageMinLength; i <= cageMaxLength; ++i) {
                for (int j = gridMinWidth; j <= (cageMinWidth + 1); ++j) {
                    mat[3][0] = static_cast<float>(j);
                    mat[3][1] = static_cast<float>(i);
                    dryGrassTile_.push_back(mat.data());
                }
            }

            // Left field
            for (int i = cageMinLength; i <= cageMaxLength; ++i) {
                for (int j = cageMaxWidth - 1; j <= gridMaxWidth; ++j) {
                    mat[3][0] = static_cast<float>(j);
                    mat[3][1] = static_cast<float>(i);
                    dryGrassTile_.push_back(mat.data());
                }
            }

            // Bottom field
            for (int i = gridMinLength; i <= cageMinLength; ++i) {
                for (int j = gridMinWidth; j <= gridMaxWidth; ++j) {
                    mat[3][0] = static_cast<float>(j);
                    mat[3][1] = static_cast<float>(i);
                    dryGrassTile_.push_back(mat.data());
                }
            }
        }

        void load_fresh_grass()
        {
            const unsigned cageWidth = ranges_.cageWidth;
            const unsigned cageLength = ranges_.cageLength;

            const int cageMinWidth = ranges_.cageMinWidth;
            const int cageMaxWidth = ranges_.cageMaxWidth;

            const int cageMinLength = ranges_.cageMinLength;
            const int cageMaxLength = ranges_.cageMaxLength;

            unsigned grassTextureTAO = render::load_texture_from_data(
                kDarkGrassPng, kDarkGrassPngLen, false);
            unsigned grassTileTAO[] = {grassTextureTAO, grassTextureTAO};

            grassTile_ = render::Square(
                grassTileTAO, std::size(grassTileTAO), cageWidth * cageLength);

            const int wallThickness = 2;

            // Load fresh grass coordinates
            int xStart = cageMinLength + wallThickness;
            int xEnd = cageMaxLength - wallThickness;

            int yStart = cageMinWidth + wallThickness;
            int yEnd = cageMaxWidth - wallThickness;

            for (int i = xStart; i <= xEnd; ++i) {
                for (int j = yStart; j <= yEnd; ++j) {
                    const calc::mat4f transposed = calc::transpose([i, j]() {
                        calc::mat4f mat = calc::mat4f::identity();
                        mat[0][3] = static_cast<float>(j);
                        mat[1][3] = static_cast<float>(i);
                        mat[2][3] = 0;
                        return mat;
                    }());
                    grassTile_.push_back(transposed.data());
                }
            }
        }

        /*! Run loop
         */
        void run()
        {
            while (panel_.run) {
                // Handle events in queue
                SDL_Event e;
                while (SDL_PollEvent(&e) != 0) {
                    if (e.type == SDL_QUIT) {
                        return;
                    }

                    ImGui_ImplSDL2_ProcessEvent(&e);
                    switch (e.type) {
                        // Handle window-related events
                        case SDL_WINDOWEVENT:
                        {
                            on_window_event(e);
                            break;
                        }

                        // Handle keypress...
                        case SDL_TEXTINPUT:
                        {
                            if (e.text.text[0] == SDLK_q) {
                                return;
                            }

                            on_text_input(e);
                            break;
                        }

                        default:
                        {
                            break;
                        }
                    }
                }

                // Render the scene
                render();
            }
        }
    private:
        /*! Helper
         *! Evt. handler
         */
        void on_window_event(const SDL_Event& e)
        {
            if ((e.window).event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                int screenWidth = e.window.data1;
                int screenHeight = e.window.data2;
                glViewport(0, 0, screenWidth, screenHeight);

                // Update camera
                Camera& refcamera = *camera_;
                refcamera.resize(static_cast<unsigned>(screenWidth),
                                 static_cast<unsigned>(screenHeight));
                refcamera.update();
            }
        }

        /*! Helper
         *! Evt. handler
         */
        static void on_text_input(const SDL_Event&)
        {
            if ((ImGui::GetIO()).WantCaptureKeyboard) {
                return;
            }
        }

        /*! Helper
         *! Renders the scene
         */
        void render()
        {
            glClearColor(panel_.backgroundColor[0],
                         panel_.backgroundColor[1],
                         panel_.backgroundColor[2],
                         1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            const calc::mat4f& lookAt = camera_->get_device_look_at();
            const calc::mat4f& projection = camera_->get_device_projection();

            // Maybe draw the grid
            if (panel_.enableGrid) {
                gridDraw_.use();
                gridDraw_.set_color(calc::vec4f(panel_.gridColor[0],
                                                panel_.gridColor[1],
                                                panel_.gridColor[2],
                                                1.0));
                gridDraw_.set_scene(lookAt, projection);
                gridTile_.draw();
            }

            // Draw the wall
            mainDraw_.use();
            mainDraw_.set_scene(lookAt, projection);
            wallObject_.draw();

            // Draw the grass outside the cage
            dryGrassTile_.draw();
            // Draw the grass inside the cage
            grassTile_.draw();

            // Draw the box
            calc::vec3f& direction = ballData_.direction;
            calc::vec3f& speed = ballData_.speed;
            calc::mat4f& translation = ballData_.translation;

            translation[0][3] += speed[0] * direction[0];
            translation[1][3] += speed[1] * direction[1];
            translation[2][3] = -1.0;

            float x = translation[0][3];
            float y = translation[1][3];

            const auto cageWidth = static_cast<float>(ranges_.cageWidth);
            const auto cageLength = static_cast<float>(ranges_.cageLength);

            // Bounce back on wall hit
            float hitOffset = 3.0;
            if (x < (hitOffset - (cageWidth / 2))
                || x > (-hitOffset + (cageWidth / 2))) {
                direction[0] *= -1;
            }

            if (y < (hitOffset - (cageLength / 2))
                || y > (-hitOffset + (cageLength / 2))) {
                direction[1] *= -1;
            }

            const calc::vec3f turnRate
                = ballData_.turnRate
                  * calc::radians(static_cast<float>(SDL_GetTicks()) / 10.0F);
            const calc::mat4f boxMat = calc::transpose(
                translation * calc::rotate_4x(turnRate[0])
                * calc::rotate_4y(turnRate[1]) * calc::rotate_4z(turnRate[2]));

            render::Box& refobject = ballObject_[ballData_.selectedSkin];
            refobject.modify(calc::data(boxMat), 0);
            refobject.draw();

            // Draw the control panel
            panel_.render(ballData_, *camera_, textureHandles_);
            // Update screen & return
            SDL_GL_SwapWindow(window_);
        }

        // Points to main SDL window
        SDL_Window* window_;

        // Control panel
        CtrlPanel panel_;

        // Camera / viewer
        Camera* camera_;
        // Contains ball position and rotation information
        BallData ballData_;

        // Program, uses instancing;
        // called to draw grid squares
        DrawInstancedNoTexture gridDraw_;
        // Program, uses instancing;
        // called to draw all textured objects
        DrawInstancedWithTexture mainDraw_;

        // Map item
        render::Square grassTile_;
        // Map item
        render::Square dryGrassTile_;
        // Map item
        render::GridSquare gridTile_;
        // Map item
        render::Box ballObject_[3];
        // Map item
        render::Box wallObject_;

        // Box skins
        std::vector<unsigned> textureHandles_;

        struct WallTAO {
            unsigned value[2];
        } wallTAO_;

        struct DryGrassTileTAO {
            unsigned value[2];
        } dryGrassTileTAO_;

        struct Ranges {
            Ranges() = default;

            Ranges(unsigned width, unsigned height)
                : cageWidth(width + (width % 2))
                , cageLength(height + (height % 2))
                , gridWidth(2 * cageWidth)
                , gridLength(2 * cageLength)
                , gridMaxWidth(gridWidth / 2)
                , gridMinWidth(-gridMaxWidth)
                , gridMaxLength(gridLength / 2)
                , gridMinLength(-gridMaxLength)
                , cageMaxWidth(cageWidth / 2)
                , cageMinWidth(-cageMaxWidth)
                , cageMaxLength(cageLength / 2)
                , cageMinLength(-cageMaxLength)
            {}

            unsigned cageWidth = 0;
            unsigned cageLength = 0;

            unsigned gridWidth = 0;
            unsigned gridLength = 0;

            int gridMaxWidth = 0;
            int gridMinWidth = 0;

            int gridMaxLength = 0;
            int gridMinLength = 0;

            int cageMaxWidth = 0;
            int cageMinWidth = 0;

            int cageMaxLength = 0;
            int cageMinLength = 0;
        } ranges_;
    };
} // namespace

/*! Entry point
 */
int main()
{
    unsigned screenWidth = 1000;
    unsigned screenHeight = 1000;

    // Initialize SDL
    SDLParam params;
    if (!init_sdl(params, screenWidth, screenHeight)) {
        printf("Error initializing SDL\n");
        return 1;
    }

    // Load all OpenGL functions using the SDL loader function
    if (gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
        glEnable(GL_DEPTH_TEST);
    else {
        printf("Error initializing OpenGL\n");
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.BackendFlags |= ~ImGuiBackendFlags_HasMouseHoveredViewport;

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(params.window, params.context);
    ImGui_ImplOpenGL3_Init();

    // Init camera defaults
    static float xPos = 0.0F;
    static float yPos = 0.0F;
    static float zPos = -20.0F;
    static float fov = 30.0F;
    static float zFar = 1000.0F;

    // Init camera
    auto camera = std::make_unique<Camera>(calc::vec3f(xPos, yPos, zPos),
                                           static_cast<float>(screenWidth),
                                           static_cast<float>(screenHeight),
                                           fov,
                                           zFar);
    camera->set_scene_rotation(0, 0, 0);
    camera->resize(screenWidth, screenHeight);

    try {
        // Enter run loop
        Runner runner(params.window, camera.get());
        runner.run();
    }

    catch (Program::ProgramBuildException&) {
        printf("Error: Make sure that your implementation of OpenGL supports "
               "version 3.3 or above\n");
    }

    catch (Program::ShaderBuildException&) {
        printf("Error: Make sure that your implementation of OpenGL supports "
               "version 3.3 or above\n");
    }

    SDL_StopTextInput();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
