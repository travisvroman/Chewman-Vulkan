#include "SVE/Engine.h"
#include "SVE/SceneManager.h"
#include "SVE/CameraNode.h"
#include "SVE/TextEntity.h"
#include "SVE/ResourceManager.h"
#include "SVE/LightManager.h"
#include "SVE/PostEffectManager.h"
#include "SVE/PipelineCacheManager.h"
#include "SVE/VulkanException.h"
#include "SVE/FontManager.h"

#include "Game/Game.h"
#include "Game/Controls/ControlDocument.h"
#include "Game/Level/GameUtils.h"
#include "DesktopFS.h"

#include <SDL2/SDL.h>
#include "VulkanHeaders.h"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

// Thanks to:
// Karl "ThinMatrix" for his video blogs on OpenGL techniques
// Niko Kauppi for his Vulkan video tutorials
// Alexander Overvoorde for his Vulkan tutorial website (https://vulkan-tutorial.com)
// Sascha Willems for his Vulkan examples git repository
// Joey de Vries for his OpenGL tutorials (learnopengl.com)
// Eray Meiri for his OGL dev tutorials (ogldev.org)
// Pawel Lapinski for his Vulkan Cookbook and compute shaders receipts

void moveCamera(const Uint8* keystates, float deltaTime, std::shared_ptr<SVE::CameraNode>& camera)
{
    if (keystates[SDL_SCANCODE_A])
        camera->movePosition(glm::vec3(-12.0f*deltaTime,0,0));
    if (keystates[SDL_SCANCODE_D])
        camera->movePosition(glm::vec3(12.0f*deltaTime,0,0));
    if (keystates[SDL_SCANCODE_W])
        camera->movePosition(glm::vec3(0,0,-12.0f*deltaTime));
    if (keystates[SDL_SCANCODE_S])
        camera->movePosition(glm::vec3(0,0,12.0f*deltaTime));
}

void rotateCamera(SDL_MouseMotionEvent& event, std::shared_ptr<SVE::CameraNode>& camera)
{
    auto yawPitchRoll = camera->getYawPitchRoll();
    yawPitchRoll.x += glm::radians(-event.xrel * 0.4f);
    yawPitchRoll.y += glm::radians(-event.yrel * 0.4f);
    camera->setYawPitchRoll(yawPitchRoll);
}

int runGame()
{
    SDL_Window *window;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    srand((unsigned)time(0));

    window = SDL_CreateWindow(
            "Chewman Vulkan",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1440, 720,
            SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI);

    if(!window)
    {
        std::cout << "Could not create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    SVE::Engine* engine = SVE::Engine::createInstance(window, "resources/main.engine", std::make_shared<SVE::DesktopFS>());
    {
        auto windowSize = engine->getRenderWindowSize();
        auto camera = engine->getSceneManager()->createMainCamera();
        std::cout << "Start loading resources..." << std::endl;
        // show loading screen
        engine->getResourceManager()->loadFolder("resources/loadingScreen");
        std::unique_ptr<Chewman::ControlDocument> loadingScreen;
        if ((float)windowSize.x / windowSize.y < 1.4)
        {
            loadingScreen = std::make_unique<Chewman::ControlDocument>("resources/game/GUI/loading.xml");
        } else {
            loadingScreen = std::make_unique<Chewman::ControlDocument>("resources/game/GUI/loadingWide.xml");
        }
        engine->renderFrame(0.0f);

        // load resources
        engine->getPipelineCacheManager()->load();
#ifdef FLATTEN_FS
        engine->getResourceManager()->loadFolder("resflat");
#else
        engine->getResourceManager()->loadFolder("resources/shaders");
        engine->getResourceManager()->loadFolder("resources/materials");
        engine->getResourceManager()->loadFolder("resources/materials/skins");
        engine->getResourceManager()->loadFolder("resources/models");
        engine->getResourceManager()->loadFolder("resources/fonts");
        engine->getResourceManager()->loadFolder("resources");
#endif
        std::cout << "Resources loading finished." << std::endl;
        loadingScreen->hide();

        // Create game controller
        auto* game = Chewman::Game::getInstance();

        // Store all cache
        if (engine->getPipelineCacheManager()->isNew())
        {
            std::cout << "Storing pipeline cache." << std::endl;
            engine->getPipelineCacheManager()->store();
        }

        if (game->getGraphicsManager().getSettings().effectSettings == Chewman::EffectSettings::High)
        {
            engine->getPostEffectManager()->addPostEffect("OnlyBrightEffect", "OnlyBrightEffect",windowSize.x / 4, windowSize.y / 4);
            engine->getPostEffectManager()->addPostEffect("VBlurEffect", "VBlurEffect",windowSize.x / 8, windowSize.y / 8);
            engine->getPostEffectManager()->addPostEffect("HBlurEffect", "HBlurEffect",windowSize.x / 8, windowSize.y / 8);
            engine->getPostEffectManager()->addPostEffect("BloomEffect", "BloomEffect");
        }

        // configure light
        if (game->getGraphicsManager().getSettings().dynamicLights == Chewman::LightSettings::Off)
            Chewman::setSunLight(Chewman::SunLightType::Day);
        else
            Chewman::setSunLight(Chewman::SunLightType::Night);

        // create camera
        camera->setNearFarPlane(0.1f, 100.0f);

        // create skybox
        engine->getSceneManager()->setSkybox("Skybox4");

        bool quit = false;
        bool skipRendering = false;
        bool lockControl = true;
        bool isMusicEnabled = game->getSoundsManager().isMusicEnabled();

        auto startTime = std::chrono::high_resolution_clock::now();
        auto prevTime = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - startTime).count();
        while (!quit)
        {
            auto curTime = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - startTime).count();

            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                game->processInput(event);
                if (event.type == SDL_QUIT)
                {
                    quit = true;
                }
                if (event.type == SDL_WINDOWEVENT)
                {
                    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    {
                        if (!skipRendering)
                            engine->resizeWindow();
                    }
                    if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
                    {
                        skipRendering = true;
                        isMusicEnabled = game->getSoundsManager().isMusicEnabled();
                        game->getSoundsManager().setMusicEnabled(false);
                        std::cout << "skip rendering" << std::endl;
                    }
                    if (event.window.event == SDL_WINDOWEVENT_RESTORED)
                    {
                        game->getSoundsManager().setMusicEnabled(isMusicEnabled);
                        skipRendering = false;
                    }
                }
                if (event.type == SDL_KEYUP)
                {
                    if (event.key.keysym.sym == SDLK_f)
                    {
                        lockControl = !lockControl;
                    }
                }
                if (event.type == SDL_MOUSEMOTION && !lockControl)
                {
                    if (event.motion.state && SDL_BUTTON(1))
                        rotateCamera(event.motion, camera);
                }
                if (event.type == SDL_MOUSEWHEEL && !lockControl)
                {
                    camera->movePosition(glm::vec3(0,0,-event.wheel.y*100.0f*(curTime - prevTime)));
                }
            }

            const Uint8* keystates = SDL_GetKeyboardState(nullptr);
            if (!lockControl)
                moveCamera(keystates, curTime - prevTime, camera);
            SDL_Delay(1);
            if (!skipRendering)
            {
                game->update(curTime - prevTime);
                engine->renderFrame(curTime - prevTime);
            }

            prevTime = curTime;
        }

        engine->finishRendering();

        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    engine->destroyInstance();

    return 0;
}

int main(int argv, char** args)
{
    try
    {
        return runGame();
    }
    catch (const SVE::VulkanException& ex)
    {
        std::cerr << "Application exception: " << ex.what() << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Unhandled exception: " << ex.what() << std::endl;
        throw;
    }
}