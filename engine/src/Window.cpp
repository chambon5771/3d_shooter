#include "Window.hpp"
#include <iostream>
#include <stdexcept>

namespace Engine {

    Window::Window(int width, int height, const std::string& title)
        : m_width(width), m_height(height), m_title(title)
    {
        init();
    }

    Window::~Window() {
        if (m_window) {
            glfwDestroyWindow(m_window);
        }
        glfwTerminate();
    }

    void Window::init() {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        if (!m_window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize GLAD");
        }

        // Изначальная установка вьюпорта
        glViewport(0, 0, m_width, m_height);
        
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwSetWindowUserPointer(m_window, this);
        glfwSetCursorPosCallback(m_window, mouseCallback);
        
        // НОВОЕ: Регистрация коллбэка изменения размера окна
        glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    }

    void Window::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (!self) return;

        if (self->m_firstMouse) {
            self->m_lastX = static_cast<float>(xpos);
            self->m_lastY = static_cast<float>(ypos);
            self->m_firstMouse = false;
            return;
        }

        float x = static_cast<float>(xpos);
        float y = static_cast<float>(ypos);
        
        self->m_deltaX = x - self->m_lastX;
        self->m_deltaY = self->m_lastY - y; 

        self->m_lastX = x;
        self->m_lastY = y;
    }

    // НОВОЕ: Реализация коллбэка для изменения размера окна
    void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (self) {
            self->m_width = width;
            self->m_height = height;
        }
        
        // Обновляем область просмотра OpenGL
        glViewport(0, 0, width, height);
    }

    float Window::consumeDeltaX() {
        float dx = m_deltaX;
        m_deltaX = 0.0f;
        return dx;
    }

    float Window::consumeDeltaY() {
        float dy = m_deltaY;
        m_deltaY = 0.0f;
        return dy;
    }

    bool Window::shouldClose() const { return glfwWindowShouldClose(m_window); }
    void Window::swapBuffers() const { glfwSwapBuffers(m_window); }
    void Window::pollEvents() const { glfwPollEvents(); }

    bool Window::isKeyPressed(int key) const {
        int state = glfwGetKey(m_window, key);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Window::isMouseButtonPressed(int button) const {
        int state = glfwGetMouseButton(m_window, button);
        return state == GLFW_PRESS;
    }

} // namespace Engine