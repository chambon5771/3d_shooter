#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine {

    class Window {
    public:
        Window(int width, int height, const std::string& title);
        ~Window();

        Window(const Window&)            = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&)                 = delete;
        Window& operator=(Window&&)      = delete;

        bool shouldClose() const;
        void swapBuffers() const;
        void pollEvents() const;
        
        bool isKeyPressed(int key) const;
        bool isMouseButtonPressed(int button) const;

        GLFWwindow* getNativeWindow() const { return m_window; }

        float consumeDeltaX();
        float consumeDeltaY();
        float getSensitivity() const { return m_sensitivity; }

        // НОВОЕ: Геттеры для получения актуального размера окна
        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }

    private:
        GLFWwindow* m_window = nullptr;
        int         m_width;
        int         m_height;
        std::string m_title;

        bool  m_firstMouse = true;
        float m_lastX = 400.0f;
        float m_lastY = 300.0f;
        float m_deltaX = 0.0f;
        float m_deltaY = 0.0f;
        float m_sensitivity = 0.1f;

        void init();
        static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
        
        // НОВОЕ: Коллбэк для изменения размера окна
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    };

} // namespace Engine