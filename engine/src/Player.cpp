#include "Player.hpp"
#include <cmath>

namespace Engine {

    Player::Player(glm::vec3 startPos) : m_position(startPos) {}

    void Player::update(float deltaTime, const Map& map, const glm::vec3& wishDir, bool jumpPressed) {
        // 1. Гравитация
        m_velocity.y -= m_gravity * deltaTime;

        // ОГРАНИЧЕНИЕ СКОРОСТИ ПАДЕНИЯ (чтобы не проваливаться сквозь пол на огромной скорости)
        if (m_velocity.y < -50.0f) {
            m_velocity.y = -50.0f;
        }

        // 2. Прыжок
        if (jumpPressed && m_isGrounded) {
            m_velocity.y = m_jumpForce;
            m_isGrounded = false;
        }

        // 3. Горизонтальное движение (с инерцией)
        glm::vec3 moveDir = wishDir;
        moveDir.y = 0.0f; // Отсекаем вертикальное движение
        if (glm::length(moveDir) > 0.0f) {
            moveDir = glm::normalize(moveDir);
        }
        
        float accel = 15.0f; // Чем больше, тем резче движение
        m_velocity.x += (moveDir.x * m_speed - m_velocity.x) * accel * deltaTime;
        m_velocity.z += (moveDir.z * m_speed - m_velocity.z) * accel * deltaTime;

        // 4. Движение по осям с коллизиями
        // Ось X
        m_position.x += m_velocity.x * deltaTime;
        glm::vec3 minBox = m_position - glm::vec3(m_width / 2.0f, 0.0f, m_width / 2.0f);
        glm::vec3 maxBox = m_position + glm::vec3(m_width / 2.0f, m_height, m_width / 2.0f);
        if (map.checkCollision(minBox, maxBox)) {
            m_position.x -= m_velocity.x * deltaTime;
            m_velocity.x = 0.0f;
        }

        // Ось Z
        m_position.z += m_velocity.z * deltaTime;
        minBox = m_position - glm::vec3(m_width / 2.0f, 0.0f, m_width / 2.0f);
        maxBox = m_position + glm::vec3(m_width / 2.0f, m_height, m_width / 2.0f);
        if (map.checkCollision(minBox, maxBox)) {
            m_position.z -= m_velocity.z * deltaTime;
            m_velocity.z = 0.0f;
        }

        // Ось Y (Падение и прыжок)
        m_position.y += m_velocity.y * deltaTime;
        minBox = m_position - glm::vec3(m_width / 2.0f, 0.0f, m_width / 2.0f);
        maxBox = m_position + glm::vec3(m_width / 2.0f, m_height, m_width / 2.0f);
        
        m_isGrounded = false;
        if (map.checkCollision(minBox, maxBox)) {
            m_position.y -= m_velocity.y * deltaTime;
            if (m_velocity.y < 0.0f) {
                m_isGrounded = true; // Приземлились
            }
            m_velocity.y = 0.0f;
        }
    }

} // namespace Engine