#include "Projectile.hpp"
#include <cmath>

namespace Engine {

    Projectile::Projectile(const glm::vec3& startPos, const glm::vec3& direction, float speed)
        : m_position(startPos) 
    {
        m_velocity = direction * speed;
    }

    void Projectile::update(float deltaTime) {
        m_position += m_velocity * deltaTime;
        m_lifeTime -= deltaTime;
    }

    bool Projectile::intersectsSphere(const glm::vec3& center, float radius) const {
        // Вычисляем расстояние от центра сферы до снаряда
        glm::vec3 diff = m_position - center;
        float distSq = glm::dot(diff, diff); // Квадрат расстояния (быстрее, чем корень)
        return distSq < (radius * radius);
    }

} // namespace Engine