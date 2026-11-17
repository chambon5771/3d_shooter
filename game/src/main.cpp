#include "Window.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Map.hpp"
#include "Projectile.hpp"
#include "Player.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>

// ==========================================
// ШЕЙДЕРЫ ДЛЯ 3D МИРА (С ИНСТАНСНЫМИ ЦВЕТАМИ)
// ==========================================
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aInstancePos;
layout (location = 2) in vec3 aInstanceColor; // НОВОЕ
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 vColor; // Передаем цвет во фрагментный шейдер
void main() {
    vec3 worldPos = aPos + aInstancePos;
    gl_Position = projection * view * model * vec4(worldPos, 1.0);
    vColor = aInstanceColor;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(vColor, 1.0);
}
)";

// ==========================================
// ШЕЙДЕРЫ ДЛЯ UI (ПРИЦЕЛ)
// ==========================================
const char* uiVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

const char* uiFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0); // Белый цвет
}
)";

// Шейдер для обычных объектов (враги, снаряды)
const char* objectVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* objectFragmentShaderSource = R"(
#version 330 core
uniform vec3 objectColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(objectColor, 1.0);
}
)";

int main() {
    try {
        Engine::Window window(1280, 720, "3D Shooter | Arena");
        glEnable(GL_DEPTH_TEST);
        
        Engine::Shader shader(vertexShaderSource, fragmentShaderSource); // Для карты
        Engine::Shader objectShader(objectVertexShaderSource, objectFragmentShaderSource); // Для врагов и снарядов
        Engine::Shader uiShader(uiVertexShaderSource, uiFragmentShaderSource); // Для прицела   

        Engine::Map map;
        map.generateArenaMap();

        // Вершины куба (36 вершин)
        float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f, -0.5f,
             0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
        };

        // === ИНИЦИАЛИЗАЦИЯ КАРТЫ (ИНСТАНСИНГ) ===
        unsigned int mapVAO, mapVBO, instancePosVBO, instanceColorVBO;
        glGenVertexArrays(1, &mapVAO);
        glGenBuffers(1, &mapVBO);
        glGenBuffers(1, &instancePosVBO);
        glGenBuffers(1, &instanceColorVBO);
        glBindVertexArray(mapVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mapVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        const auto& blocks = map.getBlocks();
        
        // Готовим массивы для позиций и цветов
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> colors;
        for (const auto& b : blocks) {
            positions.push_back(b.position);
            colors.push_back(b.color);
        }

        // Настройка позиций (атрибут 1)
        glBindBuffer(GL_ARRAY_BUFFER, instancePosVBO);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1);

        // Настройка цветов (атрибут 2)
        glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribDivisor(2, 1);

        glBindVertexArray(0);

        // === ИНИЦИАЛИЗАЦИЯ VAO ДЛЯ ВРАГОВ И СНАРЯДОВ (БЕЗ ИНСТАНСОВ) ===
        unsigned int projVAO, projVBO;
        glGenVertexArrays(1, &projVAO);
        glGenBuffers(1, &projVBO);
        glBindVertexArray(projVAO);
        glBindBuffer(GL_ARRAY_BUFFER, projVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        // === ИНИЦИАЛИЗАЦИЯ ПРИЦЕЛА ===
        float crosshairVertices[] = {
            // Вертикальная линия
            -0.005f, -0.05f, 0.0f,  0.005f, -0.05f, 0.0f,  0.005f,  0.05f, 0.0f,
             0.005f,  0.05f, 0.0f, -0.005f,  0.05f, 0.0f, -0.005f, -0.05f, 0.0f,
            // Горизонтальная линия
            -0.05f, -0.005f, 0.0f,  0.05f, -0.005f, 0.0f,  0.05f,  0.005f, 0.0f,
             0.05f,  0.005f, 0.0f, -0.05f,  0.005f, 0.0f, -0.05f, -0.005f, 0.0f
        };
        unsigned int crosshairVAO, crosshairVBO;
        glGenVertexArrays(1, &crosshairVAO);
        glGenBuffers(1, &crosshairVBO);
        glBindVertexArray(crosshairVAO);
        glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        // === ИГРОВЫЕ ОБЪЕКТЫ ===
        Engine::Player player(glm::vec3(0.0f, 2.0f, 8.0f)); // Спавн на южной стороне
        Engine::Camera camera; 

        std::vector<Engine::Projectile> projectiles;
        
        // Враги (Y = 1.0f для возвышенностей, Y = 0.0f для земли)
        std::vector<glm::vec3> enemies = {
            glm::vec3(0.0f, 0.0f, -8.0f), 
            glm::vec3(-2.0f, 0.0f, -6.0f),
            glm::vec3(2.0f, 0.0f, -6.0f),
            glm::vec3(-6.0f, 1.0f, -7.0f), // На возвышенности
            glm::vec3(6.0f, 1.0f, -7.0f)   // На возвышенности
        };

        auto lastTime = std::chrono::high_resolution_clock::now();
        bool mousePressedLastFrame = false;

        while (!window.shouldClose()) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;

            window.pollEvents();
            if (window.isKeyPressed(GLFW_KEY_ESCAPE)) break;

            // Управление камерой
            camera.processMouse(window.consumeDeltaX(), window.consumeDeltaY());

            // Ввод WASD
            glm::vec3 wishDir(0.0f);
            glm::vec3 frontFlat = camera.getFront();
            frontFlat.y = 0.0f;
            if (glm::length(frontFlat) > 0.001f) frontFlat = glm::normalize(frontFlat);
            glm::vec3 right = glm::normalize(glm::cross(camera.getFront(), glm::vec3(0.0f, 1.0f, 0.0f)));

            if (window.isKeyPressed(GLFW_KEY_W)) wishDir += frontFlat;
            if (window.isKeyPressed(GLFW_KEY_S)) wishDir -= frontFlat;
            if (window.isKeyPressed(GLFW_KEY_D)) wishDir += right;
            if (window.isKeyPressed(GLFW_KEY_A)) wishDir -= right;
            
            bool jumpPressed = window.isKeyPressed(GLFW_KEY_SPACE);

            // Физика
            player.update(deltaTime, map, wishDir, jumpPressed);

            // Страховка от падения
            if (player.getPosition().y < -20.0f) {
                player.setPosition(glm::vec3(0.0f, 2.0f, 8.0f));
            }

            camera.setPosition(player.getPosition() + glm::vec3(0.0f, 1.6f, 0.0f));

            // Стрельба
            bool mousePressed = window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
            if (mousePressed && !mousePressedLastFrame) {
                glm::vec3 spawnPos = camera.getPosition() + camera.getFront() * 0.5f;
                projectiles.emplace_back(spawnPos, camera.getFront(), 30.0f);
            }
            mousePressedLastFrame = mousePressed;

            // Обновление снарядов
            for (auto it = projectiles.begin(); it != projectiles.end(); ) {
                it->update(deltaTime);
                bool destroyProjectile = false;

                glm::vec3 roundedPos = glm::round(it->getPosition());
                if (map.hasBlock(roundedPos)) destroyProjectile = true;

                for (auto enemyIt = enemies.begin(); enemyIt != enemies.end(); ) {
                    if (it->intersectsSphere(*enemyIt, 0.8f)) {
                        destroyProjectile = true;
                        enemyIt = enemies.erase(enemyIt);
                    } else {
                        ++enemyIt;
                    }
                }

                if (destroyProjectile || !it->isAlive()) {
                    it = projectiles.erase(it);
                } else {
                    ++it;
                }
            }

            // === РЕНДЕР 3D МИРА ===
            glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.use();
            shader.setMat4("view", camera.getViewMatrix());
            float aspect = static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight());
            shader.setMat4("projection", camera.getProjection(aspect));

            // Рендер карты (цвета берутся из инстансов)
            shader.setMat4("model", glm::mat4(1.0f));
            glBindVertexArray(mapVAO);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 36, static_cast<GLsizei>(blocks.size()));

            // Рендер врагов (используем старый шейдер с uniform для цвета)
            // ВНИМАНИЕ: Для врагов мы используем тот же шейдер, но без инстансов.
            // Атрибут aInstanceColor не задан, но он нам и не нужен, так как мы не рисуем инстансами.
            // Однако шейдер ожидает vColor. Чтобы не переписывать шейдер, мы можем просто
            // временно отвязать атрибут 2 или использовать отдельный шейдер.
            // Для простоты, давайте используем uniform objectColor, но так как мы убрали его из фрагментного шейдера,
            // это не сработает. 
            
            // ВМЕСТО ЭТОГО: Используем простой подход. Враги и снаряды - это кубы.
            // Мы можем нарисовать их, используя тот же шейдер, но передав цвет через
            // дополнительный uniform. Но так как фрагментный шейдер теперь жестко берет vColor,
            // нам нужно либо изменить шейдер, чтобы он поддерживал и uniform, либо
            // использовать отдельный шейдер для врагов.
            
            // САМЫЙ ПРОСТОЙ ПУТЬ: Используем uiShader для врагов? Нет, он без матриц.
            // Давайте просто создадим еще один шейдер для не-инстансных объектов.
            // Но чтобы не усложнять код, я временно верну objectColor в шейдер.
            // (В реальном проекте лучше сделать два шейдера: один для инстансов, другой для обычных объектов).
            
            // Поскольку мы уже скомпилировали shader с vColor, давайте просто
            // для врагов и снарядов использовать отдельный простой шейдер.
            
            // ... Для простоты, я оставлю отрисовку врагов и снарядов как есть, но
            // предупрежу, что они не будут видны с текущим шейдером, так как vColor не задан.
            // В следующем сообщении я предложу решение с двумя шейдерами.
            
            // ВРЕМЕННО: Пропускаем отрисовку врагов и снарядов, чтобы код скомпилировался.
            // (В следующем шаге мы это исправим).
            
            // Рендер врагов
            objectShader.use(); // <--- ИСПОЛЬЗУЕМ ДРУГОЙ ШЕЙДЕР
            objectShader.setMat4("view", camera.getViewMatrix());
            objectShader.setMat4("projection", camera.getProjection(1280.0f / 720.0f));

            glUniform3f(glGetUniformLocation(objectShader.getID(), "objectColor"), 1.0f, 0.0f, 0.0f); // Красный
            glBindVertexArray(projVAO);
            for (const auto& enemyPos : enemies) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), enemyPos);
                objectShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // Рендер снарядов
            glUniform3f(glGetUniformLocation(objectShader.getID(), "objectColor"), 1.0f, 0.5f, 0.0f); // Оранжевый
            for (const auto& p : projectiles) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), p.getPosition());
                model = glm::scale(model, glm::vec3(0.2f));
                objectShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // === РЕНДЕР ПРИЦЕЛА ===
            glDisable(GL_DEPTH_TEST);
            uiShader.use();
            glBindVertexArray(crosshairVAO);
            glDrawArrays(GL_TRIANGLES, 0, 12);
            glEnable(GL_DEPTH_TEST);
            glBindVertexArray(0);

            window.swapBuffers();
        }

        // Очистка
        glDeleteVertexArrays(1, &mapVAO);
        glDeleteVertexArrays(1, &projVAO);
        glDeleteVertexArrays(1, &crosshairVAO);
        glDeleteBuffers(1, &mapVBO);
        glDeleteBuffers(1, &instancePosVBO);
        glDeleteBuffers(1, &instanceColorVBO);
        glDeleteBuffers(1, &projVBO);
        glDeleteBuffers(1, &crosshairVBO);
    } 
    catch (const std::exception& e) {
        std::cerr << "Critical error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}