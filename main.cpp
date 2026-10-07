#include "box.hpp"
#include "box_data.hpp"
#include "calc/matrix.hpp"
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
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

namespace {

    /*! Parameters that are initialized by SDL
     */
    struct SDLParam {
        SDL_Window* window = nullptr;
        SDL_GLContext context = nullptr;
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
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

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
    std::vector<calc::Mat4f> build_grid(unsigned width, unsigned length)
    {
        const auto xinst
            = static_cast<int>(std::ceil(static_cast<float>(width) / 2.0F));
        const auto yinst
            = static_cast<int>(std::ceil(static_cast<float>(length) / 2.0F));

        calc::Mat4f model = calc::Mat4f::identity();
        float& x = model[3][0];
        float& y = model[3][1];

        std::vector<calc::Mat4f> grid;
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
    std::vector<calc::Mat4f> build_wall(unsigned width, unsigned length)
    {
        std::vector<calc::Mat4f> wall;

        const int lengthSpan = (length / 2) / 3;
        const int widthSpan = (width / 2) / 3;

        // West wall
        for (int i = 1 - lengthSpan; i != lengthSpan; ++i) {
            calc::Mat4f mat = calc::Mat4f::identity();
            mat[0][0] = 3;
            mat[1][1] = 3;

            mat[0][3] = (static_cast<float>(width) / 2.0F) - 1.0F;
            mat[1][3] = static_cast<float>(i) * 3.0F;
            mat[2][3] = -1.0;
            wall.push_back(calc::transpose(mat));
        }

        // East wall
        for (int i = 1 - lengthSpan; i != lengthSpan; ++i) {
            calc::Mat4f mat = calc::Mat4f::identity();
            mat[0][0] = 3;
            mat[1][1] = 3;

            mat[0][3] = 1 - (static_cast<float>(width) / 2.0F);
            mat[1][3] = static_cast<float>(i) * 3.0F;
            mat[2][3] = -1.0;
            wall.push_back(calc::transpose(mat));
        }

        // North wall
        for (int i = 1 - widthSpan; i != widthSpan; ++i) {
            calc::Mat4f mat = calc::Mat4f::identity();
            mat[0][0] = 3;
            mat[1][1] = 3;

            mat[0][3] = static_cast<float>(i) * 3.0F;
            mat[1][3] = (static_cast<float>(length) / 2.0F) - 1;
            mat[2][3] = -1.0;
            wall.push_back(calc::transpose(mat));
        }

        // South wall
        for (int i = 1 - widthSpan; i != widthSpan; ++i) {
            calc::Mat4f mat = calc::Mat4f::identity();
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
        const std::vector<calc::Mat4f>& mats)
    {
        std::vector<float> values(mats.size() * 16);
        for (std::size_t i = 0; i != mats.size(); ++i)
            std::memcpy(
                &values[i * 16], calc::data(mats[i]), sizeof(calc::Mat4f));
        return values;
    }
} // namespace

namespace {

    /*! Class Runner
     *! Encapsulates the main loop
     */
    class Runner {
    public:
        ~Runner()
        {
            for (unsigned tao: textureHandles_) {
                render::delete_texture(tao);
            }
        }

        /*! Ctor.
         */
        Runner(SDL_Window* window, const std::shared_ptr<Camera>& camera)
            : window_(window)
            , panel_(window)
            , camera_(camera)
        {
            static const unsigned kWidth = 30;
            static const unsigned kHeight = 30;

            unsigned tao1 = render::load_texture_from_data(
                kBrickWallPng, kBrickWallPngLen, false);
            if (tao1 == 0) {
                throw std::runtime_error("Invalid texture");
            }

            textureHandles_.push_back(tao1);

            unsigned tao2 = render::load_texture_from_data(
                kAwesomeFacePng, kAwesomeFacePngLen, true);
            if (tao2 == 0) {
                throw std::runtime_error("Invalid texture");
            }

            textureHandles_.push_back(tao2);

            unsigned tao3 = render::load_texture_from_data(
                kShockedFacePng, kShockedFacePngLen, true);
            if (tao3 == 0) {
                throw std::runtime_error("Invalid texture");
            }

            textureHandles_.push_back(tao3);

            unsigned tao4 = render::load_texture_from_data(
                kIncredulousFacePng, kIncredulousFacePngLen, true);
            if (tao4 == 0) {
                throw std::runtime_error("Invalid texture");
            }

            textureHandles_.push_back(tao4);

            unsigned tao5 = render::load_texture_from_data(
                kAwesomeFacePng, kAwesomeFacePngLen, false);
            if (tao5 == 0) {
                throw std::runtime_error("Invalid texture");
            }

            textureHandles_.push_back(tao5);

            unsigned tao6 = render::load_texture_from_data(
                kShockedFacePng, kShockedFacePngLen, false);
            if (tao6 == 0) {
                throw std::runtime_error("Invalid texture");
            }

            textureHandles_.push_back(tao6);

            unsigned tao7 = render::load_texture_from_data(
                kIncredulousFacePng, kIncredulousFacePngLen, false);
            if (tao7 == 0) {
                throw std::runtime_error("Invalid texture");
            }

            textureHandles_.push_back(tao7);

            unsigned tao8 = render::load_texture_from_data(
                kDryGrassPng, kDryGrassPngLen, false);
            if (tao8 == 0) {
                throw std::runtime_error("Invalid texture");
            }

            textureHandles_.push_back(tao8);

            unsigned tao9 = render::load_texture_from_data(
                kDarkGrassPng, kDarkGrassPngLen, false);
            if (tao9 == 0) {
                throw std::runtime_error("Invalid texture");
            }

            textureHandles_.push_back(tao9);

            // Load boxes
            unsigned boxTAO1[] = {tao1, tao2};
            unsigned boxTAO2[] = {tao1, tao3};
            unsigned boxTAO3[] = {tao1, tao4};
            wallTAO_ = {tao1, tao1};

            boxSkins_.push_back(tao5);
            boxSkins_.push_back(tao6);
            boxSkins_.push_back(tao7);

            ballObject_[0]
                = std::make_unique<render::Box>(boxTAO1, std::size(boxTAO1), 1);
            ballObject_[0]->push_back((calc::Mat4f::identity()).data());

            ballObject_[1]
                = std::make_unique<render::Box>(boxTAO2, std::size(boxTAO2), 1);
            ballObject_[1]->push_back((calc::Mat4f::identity()).data());

            ballObject_[2]
                = std::make_unique<render::Box>(boxTAO3, std::size(boxTAO3), 1);
            ballObject_[2]->push_back((calc::Mat4f::identity()).data());

            dryGrassTileTAO_ = {tao8, tao8};

            // Load map...
            ranges_ = Ranges(kWidth, kHeight);

            // Load tiles
            load_grid();

            // Load tiles
            load_wall();

            // Load tiles
            load_dry_grass();

            // Load tiles
            load_fresh_grass(tao9);
        }

        void load_grid()
        {
            const std::vector<float> grid = copy_matrix_data(
                build_grid(ranges_.gridWidth, ranges_.gridLength));
            gridTile_ = std::make_unique<render::GridSquare>(
                ranges_.gridWidth * ranges_.gridLength);
            gridTile_->reset(grid.data(),
                             static_cast<unsigned>(grid.size() / 16));
        }

        void load_wall()
        {
            // Load wall
            const std::vector<float> wall = copy_matrix_data(
                build_wall(ranges_.cageWidth, ranges_.cageLength));
            wallObject_ = std::make_unique<render::Box>(
                wallTAO_.value,
                static_cast<unsigned>(std::size(wallTAO_.value)),
                ranges_.cageWidth * ranges_.cageLength);
            wallObject_->reset(wall.data(),
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

            dryGrassTile_ = std::make_unique<render::Square>(
                dryGrassTileTAO_.value,
                static_cast<unsigned>(std::size(dryGrassTileTAO_.value)),
                ranges_.gridWidth * ranges_.gridLength);
            calc::Mat4f mat = calc::Mat4f::identity();

            // Top field
            for (int i = cageMaxLength; i <= gridMaxLength; ++i) {
                for (int j = gridMinWidth; j <= gridMaxWidth; ++j) {
                    mat[3][0] = static_cast<float>(j);
                    mat[3][1] = static_cast<float>(i);
                    dryGrassTile_->push_back(mat.data());
                }
            }

            // Right field
            for (int i = cageMinLength; i <= cageMaxLength; ++i) {
                for (int j = gridMinWidth; j <= (cageMinWidth + 1); ++j) {
                    mat[3][0] = static_cast<float>(j);
                    mat[3][1] = static_cast<float>(i);
                    dryGrassTile_->push_back(mat.data());
                }
            }

            // Left field
            for (int i = cageMinLength; i <= cageMaxLength; ++i) {
                for (int j = cageMaxWidth - 1; j <= gridMaxWidth; ++j) {
                    mat[3][0] = static_cast<float>(j);
                    mat[3][1] = static_cast<float>(i);
                    dryGrassTile_->push_back(mat.data());
                }
            }

            // Bottom field
            for (int i = gridMinLength; i <= cageMinLength; ++i) {
                for (int j = gridMinWidth; j <= gridMaxWidth; ++j) {
                    mat[3][0] = static_cast<float>(j);
                    mat[3][1] = static_cast<float>(i);
                    dryGrassTile_->push_back(mat.data());
                }
            }
        }

        void load_fresh_grass(unsigned tao9)
        {
            const unsigned cageWidth = ranges_.cageWidth;
            const unsigned cageLength = ranges_.cageLength;

            const int cageMinWidth = ranges_.cageMinWidth;
            const int cageMaxWidth = ranges_.cageMaxWidth;

            const int cageMinLength = ranges_.cageMinLength;
            const int cageMaxLength = ranges_.cageMaxLength;

            unsigned grassTileTAO[] = {tao9, tao9};

            grassTile_ = std::make_unique<render::Square>(
                grassTileTAO, std::size(grassTileTAO), cageWidth * cageLength);

            const int wallThickness = 2;

            // Load fresh grass coordinates
            int xStart = cageMinLength + wallThickness;
            int xEnd = cageMaxLength - wallThickness;

            int yStart = cageMinWidth + wallThickness;
            int yEnd = cageMaxWidth - wallThickness;

            for (int i = xStart; i <= xEnd; ++i) {
                for (int j = yStart; j <= yEnd; ++j) {
                    const calc::Mat4f transposed = calc::transpose([i, j]() {
                        calc::Mat4f mat = calc::Mat4f::identity();
                        mat[0][3] = static_cast<float>(j);
                        mat[1][3] = static_cast<float>(i);
                        mat[2][3] = 0;
                        return mat;
                    }());
                    grassTile_->push_back(transposed.data());
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
                            if ((e.window).event == SDL_WINDOWEVENT_CLOSE) {
                                return;
                            }

                            if ((e.window).event
                                == SDL_WINDOWEVENT_SIZE_CHANGED) {
                                on_window_resize();
                            }
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
        void on_window_resize()
        {
            int screenWidth = 0;
            int screenHeight = 0;
            SDL_GL_GetDrawableSize(window_, &screenWidth, &screenHeight);

            // Just in case
            screenWidth = std::max<int>(screenWidth, 1);
            screenHeight = std::max<int>(screenHeight, 1);

            glViewport(0, 0, screenWidth, screenHeight);

            // Update camera
            Camera& camera = *camera_;
            camera.resize(static_cast<unsigned>(screenWidth),
                          static_cast<unsigned>(screenHeight));
            camera.update();
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

            const calc::Mat4f& lookAt = camera_->get_device_look_at();
            const calc::Mat4f& projection = camera_->get_device_projection();

            // Maybe draw the grid
            if (panel_.enableGrid) {
                gridDraw_.use();
                gridDraw_.set_color(calc::Vec4f(panel_.gridColor[0],
                                                panel_.gridColor[1],
                                                panel_.gridColor[2],
                                                1.0));
                gridDraw_.set_scene(lookAt, projection);
                gridTile_->draw();
            }

            // Draw the wall
            mainDraw_.use();
            mainDraw_.set_scene(lookAt, projection);
            wallObject_->draw();

            // Draw the grass outside the cage
            dryGrassTile_->draw();
            // Draw the grass inside the cage
            grassTile_->draw();

            // Draw the box
            calc::Vec3f& direction = boxData_.direction;
            calc::Vec3f& speed = boxData_.speed;
            calc::Mat4f& translation = boxData_.translation;

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

            const calc::Vec3f turnRate
                = boxData_.turnRate
                  * calc::radians(static_cast<float>(SDL_GetTicks()) / 10.0F);
            const calc::Mat4f boxMat = calc::transpose(
                translation * calc::rotate_4x(turnRate[0])
                * calc::rotate_4y(turnRate[1]) * calc::rotate_4z(turnRate[2]));

            render::Box& ballObject = *ballObject_[boxData_.selectedSkin];
            ballObject.modify(calc::data(boxMat), 0);
            ballObject.draw();

            // Draw the control panel
            panel_.render(boxData_, *camera_, boxSkins_);
            // Update screen & return
            SDL_GL_SwapWindow(window_);
        }

        // Points to main SDL window
        SDL_Window* window_ = nullptr;

        // Control panel
        CtrlPanel panel_;

        // Camera / viewer
        std::shared_ptr<Camera> camera_;
        // Contains ball position and rotation information
        BoxData boxData_;

        // Program, uses instancing;
        // called to draw grid squares
        DrawInstancedNoTexture gridDraw_;
        // Program, uses instancing;
        // called to draw all textured objects
        DrawInstancedWithTexture mainDraw_;

        // Map item
        std::unique_ptr<render::Square> grassTile_;
        // Map item
        std::unique_ptr<render::Square> dryGrassTile_;
        // Map item
        std::unique_ptr<render::GridSquare> gridTile_;
        // Map item
        std::unique_ptr<render::Box> ballObject_[3];
        // Map item
        std::unique_ptr<render::Box> wallObject_;

        // Box skins
        std::vector<unsigned> boxSkins_;

        // All texture handles
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

        if (params.context != nullptr) {
            SDL_GL_DeleteContext(params.context);
        }

        if (params.window != nullptr) {
            SDL_DestroyWindow(params.window);
        }

        SDL_Quit();
        return 1;
    }

    // Load all OpenGL functions using the SDL loader function
    if (gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress))
        == 0) {
        printf("Error initializing OpenGL\n");
        SDL_GL_DeleteContext(params.context);
        SDL_DestroyWindow(params.window);
        SDL_Quit();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.BackendFlags &= ~ImGuiBackendFlags_HasMouseHoveredViewport;

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
    auto camera = std::make_shared<Camera>(calc::Vec3f(xPos, yPos, zPos),
                                           static_cast<float>(screenWidth),
                                           static_cast<float>(screenHeight),
                                           fov,
                                           zFar);
    camera->set_scene_rotation(0, 0, 0);
    camera->resize(screenWidth, screenHeight);

    try {
        // Enter run loop
        Runner runner(params.window, camera);
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

    catch (std::runtime_error& e) {
        printf("%s\n", e.what());
    }

    SDL_StopTextInput();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(params.context);
    SDL_DestroyWindow(params.window);
    SDL_Quit();

    return 0;
}
