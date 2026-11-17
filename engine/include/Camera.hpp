#pragma once

#include <glm/glm.hpp>

namespace Engine {

    class Camera {
    public:
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
            float yaw = -90.0f, 
            float pitch = 0.0f);

        glm::mat4 getViewMatrix() const;
        glm::mat4 getProjection(float aspect) const;

        void processMouse(float xoffset, float yoffset);
        void move(glm::vec3 wishDir, float deltaTime);

        // Геттеры для дебага и стрельбы
        glm::vec3 getPosition() const { return m_position; }
        float getYaw() const { return m_yaw; }
        float getPitch() const { return m_pitch; }
        glm::vec3 getFront() const { return m_front; } // <--- ДОБАВЛЕНО
        
        void setPosition(const glm::vec3& pos) { m_position = pos; }

    private:
        glm::vec3 m_position;
        float m_yaw;
        float m_pitch;

        float m_fov = 45.0f;
        float m_near = 0.5f;
        float m_far = 100.0f;
        float m_speed = 6.0f;       // Чуть ускорил для теста
        float m_sensitivity = 0.1f;

        glm::vec3 m_front;
        glm::vec3 m_right;
        glm::vec3 m_up;
        glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        void updateVectors();
    };

} // namespace Engine