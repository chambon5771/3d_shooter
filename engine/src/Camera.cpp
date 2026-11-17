#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace Engine {

    Camera::Camera(glm::vec3 position, float yaw, float pitch)
        : m_position(position), m_yaw(yaw), m_pitch(pitch)
    {
        // При создании камеры сразу вычисляем векторы направления,
        // иначе m_front, m_right и m_up будут содержать мусор.
        updateVectors();
    }

    glm::mat4 Camera::getViewMatrix() const {
        // lookAt(откуда смотрим, куда смотрим, где верх)
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

    glm::mat4 Camera::getProjection(float aspect) const {
        return glm::perspective(glm::radians(m_fov), aspect, m_near, m_far);
    }

    void Camera::processMouse(float xoffset, float yoffset) {
        xoffset *= m_sensitivity;
        yoffset *= m_sensitivity;

        m_yaw   += xoffset;
        m_pitch += yoffset;

        // Защита от Gimbal Lock (чтобы камера не перевернулась через голову)
        if (m_pitch >  89.0f) m_pitch =  89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;

        // После изменения углов ОБЯЗАТЕЛЬНО пересчитываем векторы
        updateVectors();
    }

    void Camera::move(glm::vec3 wishDir, float deltaTime) {
        // 1. Вычисляем скорость с учетом времени (чтобы на 144Гц и 60Гц движение было одинаковым)
        float velocity = m_speed * deltaTime;

        // 2. Если игрок жмет W и A одновременно, вектор будет длиннее.
        // Нормализуем его, чтобы диагональ не была быстрее прямой.
        if (glm::length(wishDir) > 0.0f) {
            wishDir = glm::normalize(wishDir);
        }

        // 3. Двигаем камеру.
        // wishDir.x — вправо (m_right)
        // wishDir.y — вперед (используем forwardFloor, чтобы не лететь вверх при взгляде в небо)
        // wishDir.z — вверх (используем мировой m_worldUp, чтобы Space всегда поднимал вверх)
        
        // Считаем "горизонтальный" forward (обнуляем Y)
        glm::vec3 forwardFloor = glm::vec3(m_front.x, 0.0f, m_front.z);
        if (glm::length(forwardFloor) > 0.0f) {
            forwardFloor = glm::normalize(forwardFloor);
        }

        m_position += m_right * (wishDir.x * velocity);
        m_position += forwardFloor * (wishDir.y * velocity);
        m_position += m_worldUp * (wishDir.z * velocity);
    }

    void Camera::updateVectors() {
        // Сферические координаты -> Декартовы
        glm::vec3 front;
        front.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
        front.y = std::sin(glm::radians(m_pitch));
        front.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
        m_front = glm::normalize(front);

        // Вектор "вправо" всегда горизонтален (Y=0), поэтому используем мировой верх
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));
        
        // Вектор "вверх" камеры перпендикулярен front и right
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }

} // namespace Engine