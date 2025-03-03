// #define BENCHMARKING

#include <glad/glad.h>
// Preserve include order
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include <typeinfo>

#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"
#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../vmlib/mat33.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/vec4.hpp"
#include "button.hpp"
#include "defaults.hpp"
#include "mesh.hpp"
#include "performance.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "texture.hpp"

using namespace std::chrono;

namespace {
constexpr char const *kWindowTitle = "COMP3811 - CW2";

void glfw_callback_error_(int, char const *);

void glfw_callback_key_(GLFWwindow *, int, int, int, int);
void glfw_callback_motion_(GLFWwindow *, double, double);
void glfw_callback_button_(GLFWwindow *, int, int, int);

struct GLFWCleanupHelper {
  ~GLFWCleanupHelper();
};
struct GLFWWindowDeleter {
  ~GLFWWindowDeleter();
  GLFWwindow *window;
};

}  // namespace

int main() try {
  // Initialize GLFW
  if (GLFW_TRUE != glfwInit()) {
    char const *msg = nullptr;
    int ecode = glfwGetError(&msg);
    throw Error("glfwInit() failed with '%s' (%d)", msg, ecode);
  }

  // Ensure that we call glfwTerminate() at the end of the program.
  GLFWCleanupHelper cleanupHelper;

  // Configure GLFW and create window
  glfwSetErrorCallback(&glfw_callback_error_);

  glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

  // glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_DEPTH_BITS, 24);

#if !defined(NDEBUG)
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif  // ~ !NDEBUG

  GLFWwindow *window =
      glfwCreateWindow(1280, 720, kWindowTitle, nullptr, nullptr);

  if (!window) {
    char const *msg = nullptr;
    int ecode = glfwGetError(&msg);
    throw Error("glfwCreateWindow() failed with '%s' (%d)", msg, ecode);
  }

  GLFWWindowDeleter windowDeleter{window};

  // Set up event handling
  State state{};
  glfwSetWindowUserPointer(window, &state);

  glfwSetKeyCallback(window, &glfw_callback_key_);
  glfwSetCursorPosCallback(window, &glfw_callback_motion_);
  glfwSetMouseButtonCallback(window, &glfw_callback_button_);

  // Set up drawing stuff
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // V-Sync is on.

  if (!gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress))
    throw Error("gladLoaDGLLoader() failed - cannot load GL API!");

  std::printf("RENDERER %s\n", glGetString(GL_RENDERER));
  std::printf("VENDOR %s\n", glGetString(GL_VENDOR));
  std::printf("VERSION %s\n", glGetString(GL_VERSION));
  std::printf("SHADING_LANGUAGE_VERSION %s\n",
              glGetString(GL_SHADING_LANGUAGE_VERSION));

// Ddebug output
#if !defined(NDEBUG)
  setup_gl_debug_output();
#endif  // ~ !NDEBUG

  OGL_CHECKPOINT_ALWAYS();

  // #### Global GL Setup ####//

  glEnable(GL_FRAMEBUFFER_SRGB);
  glEnable(GL_CULL_FACE);
  glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  // Standard Blending equation
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  OGL_CHECKPOINT_ALWAYS();

  int iwidth, iheight;
  glfwGetFramebufferSize(window, &iwidth,
                         &iheight);  // alter this to change viewports

  // Other initialization & loading
  ShaderProgram normalsProg(
      {{GL_VERTEX_SHADER, "assets/cw2/normalsColor.vert"},
       {GL_FRAGMENT_SHADER, "assets/cw2/normalsColor.frag"}});
  ShaderProgram textureBlinnPhong(
      {{GL_VERTEX_SHADER, "assets/cw2/textureBlinnPhong.vert"},
       {GL_FRAGMENT_SHADER, "assets/cw2/textureBlinnPhong.frag"}});
  ShaderProgram colorBlinnPhong(
      {{GL_VERTEX_SHADER, "assets/cw2/colorBlinnPhong.vert"},
       {GL_FRAGMENT_SHADER, "assets/cw2/colorBlinnPhong.frag"}});
  ShaderProgram uiProg({{GL_VERTEX_SHADER, "assets/cw2/ui.vert"},
                        {GL_FRAGMENT_SHADER, "assets/cw2/ui.frag"}});

  OGL_CHECKPOINT_ALWAYS();

  // Benchmarking
#if defined(BENCHMARKING)
  QueryTimer fullRendering("fullRendering");
  QueryTimer onePointtwo("onePointTwo");
  QueryTimer onePointfour("onePointfour");
  QueryTimer onePointfive("onePointfive");
#endif

  // Objects
  Camera firstPersonCamera({-5.0f, 0.2f, 4.f});
  Camera trackingCamera({0.f, 0.f, 0.f});
  Camera groundedCamera({-3.0f, 0.2f, 5.f});

  Scene scene;
  Lighting light;
  Spaceship spaceship;

  // UI
  Button altitudeLabel("Altitude: ", topLeft, {0.f, 0.f, 1.f, 0.2f},
                       {0.f, 0.f, 1.f, 0.2f}, {0.f, 0.f, 1.f, 0.2f}, 1.f,
                       {0.f, 0.f, 0.f, 1.f}, [](State *) {});

  Button launchButton("Launch", bottomCentreLeft, {0.f, 1.f, 0.f, 0.5f},
                      {0.f, 1.f, 0.f, 1.f}, {1.f, 1.f, 1.f, 1.f}, 3.f,
                      {0.f, 0.f, 0.f, 1.f},
                      [](State *state) { state->spaceship->launch(); });

  Button resetButton("Reset", bottomCentreRight, {1.f, 0.f, 0.f, 0.5f},
                     {1.f, 0.f, 0.f, 1.f}, {1.f, 1.f, 1.f, 1.f}, 3.f,
                     {0.f, 0.f, 0.f, 1.f},
                     [](State *state) { state->spaceship->resetState(); });

  // State
  state.firstPersonCamera = &firstPersonCamera;
  state.trackingCamera = &trackingCamera;
  state.groundedCamera = &groundedCamera;
  state.leftScreenCamera = state.firstPersonCamera;
  state.rightScreenCamera = state.firstPersonCamera;
  state.splitScreen = false;

  state.spaceship = &spaceship;

  state.buttons.push_back(&altitudeLabel);
  state.buttons.push_back(&launchButton);
  state.buttons.push_back(&resetButton);

  OGL_CHECKPOINT_ALWAYS();

  // Clock
  auto lastClock = Clock::now();

#if defined(BENCHMARKING)
  auto lastChrono = high_resolution_clock::now();
#endif

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // Let GLFW process events
    glfwPollEvents();

    // Check if window was resized.
    float fbwidth, fbheight;
    {
      int nwidth, nheight;
      glfwGetFramebufferSize(window, &nwidth, &nheight);

      fbwidth = float(nwidth);
      fbheight = float(nheight);

      if (0 == nwidth || 0 == nheight) {
        // Window minimized? Pause until it is unminimized.
        // This is a bit of a hack.
        do {
          glfwWaitEvents();
          glfwGetFramebufferSize(window, &nwidth, &nheight);
        } while (0 == nwidth || 0 == nheight);
      }
    }
    // #### Update State ####//
    auto const now = Clock::now();
    float dt = std::chrono::duration_cast<Secondsf>(now - lastClock).count();
    lastClock = now;

    // Benchmarking
#if defined(BENCHMARKING)
    auto newChrono = high_resolution_clock::now();
    duration<double> frameTime =
        duration_cast<duration<double>>(newChrono - lastChrono);
    std::cout << "Frame time: " << frameTime.count() << " seconds" << std::endl;
#endif

    // Update state
    spaceship.animate(dt);
    firstPersonCamera.updateState(dt);
    trackingCamera.track(spaceship.getPosition());
    groundedCamera.pointAt(spaceship.getPosition());

    // Update UI
    for (Button *b : state.buttons) {
      b->updateSize(fbwidth, fbheight);
    }

    // Draw
    OGL_CHECKPOINT_DEBUG();
#if defined(BENCHMARKING)
    auto startRenderChrono = high_resolution_clock::now();

    fullRendering.startQuery();  ///---------------------------------start query
#endif

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set Left Viewport
    float aspect;
    if (state.splitScreen) {
      aspect = fbwidth / (2.f * fbheight);
      glViewport(0, 0, GLsizei(fbwidth / 2), GLsizei(fbheight));
    } else {
      aspect = fbwidth / fbheight;
      glViewport(0, 0, GLsizei(fbwidth), GLsizei(fbheight));
    }

    // Draw Left Screen
    light.updateLighting(state.leftScreenCamera->getCamWorldPosition(),
                         spaceship.getLightPos(), spaceship.getLightAmbient(),
                         spaceship.getLightDiffuse());
    Mat44f leftCamProjection = state.leftScreenCamera->getProjection(aspect);

    // Draw Ground
    glUseProgram(textureBlinnPhong.programId());
    light.setLighting();

#if defined(BENCHMARKING)
    onePointtwo.startQuery();  ///--------------------------start query
#endif
    scene.drawGround(leftCamProjection);
#if defined(BENCHMARKING)
    onePointtwo.stopQuery();  ///------------------------------stop query
#endif

    // Draw Launchpads and Spaceship
    glUseProgram(colorBlinnPhong.programId());
    light.setLighting();
#if defined(BENCHMARKING)
    onePointfour.startQuery();  ///-----------------------start query
#endif
    scene.drawLaunchpads(leftCamProjection);
#if defined(BENCHMARKING)
    onePointfour.stopQuery();  ///----------------------stop query
#endif

#if defined(BENCHMARKING)
    onePointfive.startQuery();  ///------------------------start query
#endif
    spaceship.draw(leftCamProjection);
#if defined(BENCHMARKING)
    onePointfive.stopQuery();  ///----------------------stop query
#endif

    // Conditionally Draw Right Screen
    if (state.splitScreen) {
      glViewport(GLsizei(fbwidth / 2), 0, GLsizei(fbwidth / 2),
                 GLsizei(fbheight));

      light.updateLighting(state.rightScreenCamera->getCamWorldPosition(),
                           spaceship.getLightPos(), spaceship.getLightAmbient(),
                           spaceship.getLightDiffuse());
      Mat44f rightCamProjection =
          state.rightScreenCamera->getProjection(aspect);

      // Draw Ground
      glUseProgram(textureBlinnPhong.programId());
      light.setLighting();
      scene.drawGround(rightCamProjection);

      // Draw Launchpads and Spaceship
      glUseProgram(colorBlinnPhong.programId());
      light.setLighting();
      scene.drawLaunchpads(rightCamProjection);
      spaceship.draw(rightCamProjection);
    }

    // UI Drawing
    glViewport(0, 0, GLsizei(fbwidth), GLsizei(fbheight));
    glUseProgram(uiProg.programId());
    // GL Setup
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    for (Button *b : state.buttons) {
      b->draw();
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // Clean up state
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);

#if defined(BENCHMARKING)
    fullRendering.stopQuery();  ///-----------------------stop query
#endif

    OGL_CHECKPOINT_DEBUG();

#if defined(BENCHMARKING)
    auto stopRenderChrono = high_resolution_clock::now();
    duration<double> renderTime =
        duration_cast<duration<double>>(stopRenderChrono - startRenderChrono);
    std::cout << "Render time: " << renderTime.count() << " seconds"
              << std::endl;
    fullRendering.printResult();
    onePointtwo.printResult();
    onePointfour.printResult();
    onePointfive.printResult();
    std::cout << std::endl;
#endif

    // Display results
    glfwSwapBuffers(window);
  }
  state.prog = nullptr;
  return 0;
} catch (std::exception const &eErr) {
  std::fprintf(stderr, "Top-level Exception (%s):\n", typeid(eErr).name());
  std::fprintf(stderr, "%s\n", eErr.what());
  std::fprintf(stderr, "Bye.\n");
  return 1;
}

namespace {
void glfw_callback_error_(int aErrNum, char const *aErrDesc) {
  std::fprintf(stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum);
}

void glfw_callback_key_(GLFWwindow *aWindow, int aKey, int, int aAction,
                        int mods) {
  if (GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction) {
    glfwSetWindowShouldClose(aWindow, GLFW_TRUE);
    return;
  }

  if (auto *state = static_cast<State *>(glfwGetWindowUserPointer(aWindow))) {
    // Split Screen Toggle
    if (GLFW_KEY_V == aKey && GLFW_PRESS == aAction) {
      state->splitScreen = !state->splitScreen;
    }

    // Toggle Active Cameras
    if (GLFW_KEY_C == aKey && GLFW_PRESS == aAction) {
      if (mods & GLFW_MOD_SHIFT) {
        if (state->rightScreenCamera == state->firstPersonCamera) {
          state->rightScreenCamera = state->trackingCamera;
        } else if (state->rightScreenCamera == state->trackingCamera) {
          state->rightScreenCamera = state->groundedCamera;
        } else {
          state->rightScreenCamera = state->firstPersonCamera;
        }
      } else {
        if (state->leftScreenCamera == state->firstPersonCamera) {
          state->leftScreenCamera = state->trackingCamera;
        } else if (state->leftScreenCamera == state->trackingCamera) {
          state->leftScreenCamera = state->groundedCamera;
        } else {
          state->leftScreenCamera = state->firstPersonCamera;
        }
      }
      // Reset First Person Camera State
      if (state->leftScreenCamera != state->firstPersonCamera &&
          (state->rightScreenCamera != state->firstPersonCamera ||
           !state->splitScreen)) {
        state->firstPersonCamera->resetState(aWindow);
      }
    }

    // Update first person camera if active
    if (state->leftScreenCamera == state->firstPersonCamera ||
        (state->rightScreenCamera == state->firstPersonCamera &&
         state->splitScreen)) {
      state->firstPersonCamera->updateKeyActions(aKey, aAction);
    }
    // Update spaceship animation
    state->spaceship->updateKeyActions(aKey, aAction);
  }
}

void glfw_callback_motion_(GLFWwindow *aWindow, double aX, double aY) {
  if (auto *state = static_cast<State *>(glfwGetWindowUserPointer(aWindow))) {
    state->firstPersonCamera->updateMouseMovement(aX, aY);

    // UI
    for (Button *b : state->buttons) {
      b->updateMouseMove(aX, aY);
    }
  }
}

void glfw_callback_button_(GLFWwindow *aWindow, int aButton, int aAction, int) {
  if (auto *state = static_cast<State *>(glfwGetWindowUserPointer(aWindow))) {
    if (state->leftScreenCamera == state->firstPersonCamera ||
        (state->rightScreenCamera == state->firstPersonCamera &&
         state->splitScreen)) {
      state->firstPersonCamera->updateMouseClicked(aWindow, aButton, aAction);
    }
    // UI
    for (Button *b : state->buttons) {
      b->updateMousePress(aButton, aAction, state);
    }
  }
}
}  // namespace
namespace {
GLFWCleanupHelper::~GLFWCleanupHelper() { glfwTerminate(); }

GLFWWindowDeleter::~GLFWWindowDeleter() {
  if (window) glfwDestroyWindow(window);
}
}  // namespace