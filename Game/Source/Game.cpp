// Include necessary headers for functionality
#include "CharacterController.h"
#include "Level.h"
#include "FullScreenQuad.h"
#include "RenderTarget.h"

// Define constants for window dimensions
constexpr unsigned int WINDOW_WIDTH = 1280;
constexpr unsigned int WINDOW_HEIGHT = 720;

/**
 * @brief Load all required shaders and textures into memory.
 *
 * @param driver Pointer to the DXCore driver for shader and texture management.
 */
static void InitializeShadersAndTextures(DXCore* driver) {
    // Initialize shader manager
    ShaderManager::Init(driver);

    // Add vertex and pixel shaders to the manager
    ShaderManager::Add("Gizmos", "Resources/Shaders/Vertex/DefaultVertex.hlsl", "Resources/Shaders/Pixel/GizmosPixel.hlsl");
    ShaderManager::Add("Default", "Resources/Shaders/Vertex/DefaultVertex.hlsl", "Resources/Shaders/Pixel/DefaultPixel.hlsl");
    ShaderManager::Add("Grass", "Resources/Shaders/Vertex/GrassVertex.hlsl", "Resources/Shaders/Pixel/DefaultPixel.hlsl", ShaderType::Instancing);
    ShaderManager::Add("Leaves", "Resources/Shaders/Vertex/LeavesVertex.hlsl", "Resources/Shaders/Pixel/DefaultPixel.hlsl", ShaderType::Instancing);
    ShaderManager::Add("NormalMap", "Resources/Shaders/Vertex/DefaultVertex.hlsl", "Resources/Shaders/Pixel/NormalMapPixel.hlsl");
    ShaderManager::Add("DefaultTiling", "Resources/Shaders/Vertex/DefaultVertex.hlsl", "Resources/Shaders/Pixel/DefaultTilingPixel.hlsl");
    ShaderManager::Add("Tree", "Resources/Shaders/Vertex/InstancingVertex.hlsl", "Resources/Shaders/Pixel/NormalMapPixel.hlsl", ShaderType::Instancing);
    ShaderManager::Add("Leaf", "Resources/Shaders/Vertex/AnimatedVertex.hlsl", "Resources/Shaders/Pixel/NormalMapPixel.hlsl", ShaderType::Instancing);
    ShaderManager::Add("TRex", "Resources/Shaders/Vertex/BoneAnimatedVertex.hlsl", "Resources/Shaders/Pixel/NormalMapPixel.hlsl", ShaderType::Animated);

    // Initialize texture manager
    TextureManager::Init(driver);

    // Load textures into the texture manager
    TextureManager::load("Resources/Textures/Wall.png");
    TextureManager::load("Resources/Textures/Wall_normal.png");
    TextureManager::load("Resources/Textures/Grass.png");
    TextureManager::load("Resources/Textures/Leaf.png");
    TextureManager::load("Resources/Textures/Ground.jpg");
    TextureManager::load("Resources/Textures/Sky.jpg");
}


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    // Ensure the cache directory exists
    CreateDirectory(L"Cache", NULL);

    // Set up the camera with default parameters
    Camera camera(Vec2(WINDOW_WIDTH, WINDOW_HEIGHT), Vec3(0, 5, 10), Vec3(0, 0, 0), 0.1f, 1000.f);

    // Create the application window
    Window appWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GTA-Rex");
    DXCore* driver = &appWindow.GetDevice();

    // Initialize a full-screen quad for deferred rendering
    FullScreenQuad screenQuad("Resources/Shaders/Vertex/FullScreenQuadVertex.hlsl", "Resources/Shaders/Pixel/FullScreenQuadPixel.hlsl", driver);

    // Initialize collision detection module
    Collisions::Init(driver);

    // Load all shaders and textures
    InitializeShadersAndTextures(driver);

    // Initialize the texture sampler
    Sampler textureSampler(*driver);
    textureSampler.Bind(*driver);

    // Create the game level and character controller
    Level gameLevel(driver);
    CharacterController playerCharacter(Vec3(0, 5, 0), Vec3::zero, Vec3::one);

    // Set up a render target for rendering the scene
    RenderTarget renderTarget(WINDOW_WIDTH, WINDOW_HEIGHT, driver);

    // Prompt the user for operational modes
    bool isFreeLook = (MessageBoxA(NULL, "Free Look?", "Mode", MB_YESNO) == IDYES);
    bool isDebugMode = (MessageBoxA(NULL, "Debug?", "Sub Mode", MB_YESNO) == IDYES);

    // Initialize input settings
    appWindow.inputs.SetCursorLock(true);

    // Timer for frame management
    Timer frameTimer;
    float deltaTime, frameCount = 0, elapsedTime = 0;

    // Configure ambient lighting
    float ambientLight = 0.2f, lightIntensity = 2.0f;
    ShaderManager::UpdateConstantForAll(ShaderStage::PixelShader, "ConstBuffer", "Amb", &ambientLight);
    ShaderManager::UpdateConstantForAll(ShaderStage::PixelShader, "ConstBuffer", "Int", &lightIntensity);

    // Main game loop
    while (true) {
        // Calculate delta time
        deltaTime = frameTimer.dt();

        // Avoid extremely large delta times
        if (deltaTime > 2.0f) deltaTime = 0.00001f;

        // Cap frame rate at 120 FPS
        if (1.0f / deltaTime <= 120.0f) {
            frameTimer.reset();
        }
        else {
            continue;
        }

        // Update window inputs
        appWindow.Update();

        // Update camera or character based on mode
        if (isFreeLook) {
            camera.Update(deltaTime);
        }
        else {
            playerCharacter.Update(deltaTime);
        }

        // Update the game level
        gameLevel.Update(deltaTime);

        // Update collisions after all scene updates
        Collisions::Update();

        // Update the view-projection matrix for shaders
        Matrix viewProjectionMatrix = camera.GetViewProjMat();
        ShaderManager::UpdateConstantForAll(ShaderStage::VertexShader, "ConstBuffer", "VP", &viewProjectionMatrix);

        // Clear the render target and apply it
        driver->Clear();
        renderTarget.Clear(driver);
        renderTarget.Apply(driver);

        // Draw the game level and character
        gameLevel.Draw();
        if (!isFreeLook) playerCharacter.Draw();

        // Draw debug visuals if in debug mode
        if (isDebugMode) {
            Collisions::DrawGizmos();
        }

        // Perform deferred shading
        driver->ApplyBackbufferView();
        screenQuad.DrawTexture("tex", renderTarget.srv, driver);
        driver->Present();

        // Handle user input for exiting or toggling cursor lock
        if (appWindow.inputs.ButtonDown(2) || appWindow.inputs.KeyDown(VK_ESCAPE)) {
            appWindow.inputs.ToggleCursorLock();
        }
        if (appWindow.inputs.Exit()) {
            break;
        }

        // Track frames and time for FPS evaluation
        frameCount++;
        elapsedTime += deltaTime;
    }

    // Release all resources
    ShaderManager::Free();
    TextureManager::Free();

    // Display average FPS to the user
    std::string averageFPS = "Average FPS: " + std::to_string(frameCount / elapsedTime);
    MessageBoxA(NULL, averageFPS.c_str(), "Evaluation", MB_OK);

    return 0;
}
