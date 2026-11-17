#pragma once

#include <glm/glm.hpp>
#include "Map.hpp"

namespace Engine {

    class Player {
    public:
        Player(glm::vec3 startPos = glm::vec3(0.0f, 1.0f, 5.0f));

        // Обновляет физику (гравитацию, коллизии)
        void update(float deltaTime, const Map& map, const glm::vec3& wishDir, bool jumpPressed);
        void setPosition(const glm::vec3& pos) { m_position = pos; }
        glm::vec3 getPosition() const { return m_position; }
        bool isGrounded() const { return m_isGrounded; }

    private:
        glm::vec3 m_position;
        glm::vec3 m_velocity;

        // Габариты игрока
        const float m_width = 0.6f;
        const float m_height = 1.8f;
        
        // Параметры физики
        float m_gravity = 25.0f;
        float m_jumpForce = 8.0f;
        float m_speed = 6.0f;
        
        bool m_isGrounded = false;

        // Вспомогательный метод для движения по одной оси с проверкой коллизии
        void moveAxis(glm::vec3& pos, float delta, int axis, const Map& map);
    };

} // namespace Engine