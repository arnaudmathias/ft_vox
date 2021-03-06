#include "env.hpp"

Env::Env() : Env(0, 0) {}

Env::Env(unsigned short width, unsigned short height)
    : width(width),
      height(height),
      _frame(0),
      _window_width(1280),
      _window_height(720) {
  if (!glfwInit()) return;
  setupWindowHint();
  GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = glfwGetVideoMode(primary_monitor);
  if (width == 0 && height == 0) {
    this->width = mode->width;
    this->height = mode->height;
    window = glfwCreateWindow(this->width, this->height, "ft_vox",
                              primary_monitor, NULL);
    glfwSetWindowMonitor(window, primary_monitor, 0, 0, mode->width,
                         mode->height, mode->refreshRate);
  } else {
    _window_width = width;
    _window_height = height;
    window = glfwCreateWindow(width, height, "ft_vox", NULL, NULL);
    glfwSetWindowMonitor(window, NULL, (mode->width / 2) - (_window_width / 2),
                         (mode->height / 2) - (_window_height / 2),
                         _window_width, _window_height, 0);
    inputHandler.mousex = _window_width / 2;
    inputHandler.mousey = _window_height / 2;
  }
  if (!window) {
    std::cout << "Could not create window\n";
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
  }
  std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  inputHandler.mouseDisabled = false;

  setupWindow();
  setupContext();
}

Env::~Env() { glfwTerminate(); }

void Env::toggleFullscreen() {
  _fullscreen = !_fullscreen;
  GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = glfwGetVideoMode(primary_monitor);
  if (_fullscreen) {
    glfwSetWindowMonitor(window, primary_monitor, 0, 0, mode->width,
                         mode->height, mode->refreshRate);
    this->width = mode->width;
    this->height = mode->height;
    // The mouse virtual position reported by curposPosCallback promptly jump
    // after a window -> fullscreen transition
    // Doesn't happen on fullscreen -> window
    // GLFW bug ?
    inputHandler.mousex -= (this->_window_width / 2);
    inputHandler.mousey -= (this->_window_height / 2);
    this->has_resized = true;
  } else {
    glfwSetWindowMonitor(window, NULL, (mode->width / 2) - (_window_width / 2),
                         (mode->height / 2) - (_window_height / 2),
                         _window_width, _window_height, 0);
    this->width = _window_width;
    this->height = _window_height;
  }
  // Query and update framebuffer size
  int wframe, hframe;
  glfwGetFramebufferSize(window, &wframe, &hframe);
  glViewport(0, 0, wframe, hframe);
}

void Env::setupWindowHint() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
}

void Env::setupWindow() {
  if (window != nullptr) {
    glfwSetWindowUserPointer(window, &inputHandler);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseKeyCallback);
  }
}

void Env::setupContext() {
  glfwSwapInterval(0);
  glEnable(GL_DEBUG_OUTPUT);
  while (glGetError() != GL_NO_ERROR)
    ;  // Flush gl_error
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
  // glEnable(GL_FRAMEBUFFER_SRGB);
}

void Env::update() {
  updateFpsCounter();
  static double previousTime = glfwGetTime();
  static int frame_count;
  double currentTime = glfwGetTime();
  double deltaTime = currentTime - previousTime;
  previousTime = currentTime;
  this->_deltaTime = static_cast<float>(deltaTime);
  this->_absoluteTime = static_cast<float>(currentTime);
  this->_frame++;
}

void Env::updateFpsCounter() {
  static double previous_seconds = glfwGetTime();
  static int frame_count;
  double current_seconds = glfwGetTime();
  double elapsed_seconds = current_seconds - previous_seconds;
  if (elapsed_seconds > 1.0) {
    previous_seconds = current_seconds;
    this->_fps =
        static_cast<float>(frame_count) / static_cast<float>(elapsed_seconds);
    std::ostringstream tmp;
    tmp << _fps << " fps";
    glfwSetWindowTitle(window, tmp.str().c_str());
    frame_count = 0;
  }
  frame_count++;
}

float Env::getDeltaTime() const { return (this->_deltaTime); }

float Env::getAbsoluteTime() const { return (this->_absoluteTime); }

float Env::getFrame() const { return (this->_frame); }

float Env::getFPS() const { return (this->_fps); }

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mode) {
  (void)scancode;
  (void)mode;
  InputHandler *inputHandler =
      reinterpret_cast<InputHandler *>(glfwGetWindowUserPointer(window));
  if (action == GLFW_PRESS) {
    inputHandler->keys[key] = true;
  } else if (action == GLFW_RELEASE) {
    inputHandler->keys[key] = false;
  }
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
  InputHandler *inputHandler =
      reinterpret_cast<InputHandler *>(glfwGetWindowUserPointer(window));
  inputHandler->mousex = static_cast<float>(xpos);
  inputHandler->mousey = static_cast<float>(ypos);
}

void mouseKeyCallback(GLFWwindow *window, int button, int action, int mods) {
  (void)mods;
  InputHandler *inputHandler =
      reinterpret_cast<InputHandler *>(glfwGetWindowUserPointer(window));
  if (action == GLFW_PRESS) {
    inputHandler->mouse_keys[button] = true;
  } else if (action == GLFW_RELEASE) {
    inputHandler->mouse_keys[button] = false;
  }
}
