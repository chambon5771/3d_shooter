#pragma once

#include <glm/glm.hpp>

namespace Engine {

    class Projectile {
    public:
        Projectile(const glm::vec3& startPos, const glm::vec3& direction, float speed);

        void update(float deltaTime);
        bool isAlive() const { return m_lifeTime > 0.0f; }

        glm::vec3 getPosition() const { return m_position; }
        
        // Проверка: находится ли снаряд внутри сферы (для попадания по врагу)
        bool intersectsSphere(const glm::vec3& center, float radius) const;

    private:
        glm::vec3 m_position;
        glm::vec3 m_velocity;
        float m_lifeTime = 5.0f;
    };

} // namespace Engine