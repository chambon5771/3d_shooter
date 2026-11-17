#include "Map.hpp"
#include <algorithm>
#include <cmath>

namespace Engine {

    Map::Map() {
        // Резервируем память под большое количество блоков
        m_blocks.reserve(10000);
    }

    // Обновлено: принимает позицию и цвет
    void Map::addBlock(const glm::vec3& position, const glm::vec3& color) {
        if (!hasBlock(position)) {
            m_blocks.push_back({position, color});
        }
    }

    void Map::removeBlock(const glm::vec3& position) {
        auto it = std::find_if(m_blocks.begin(), m_blocks.end(), 
            [&position](const Block& b) { 
                return b.position == position; 
            });
        if (it != m_blocks.end()) m_blocks.erase(it);
    }

    bool Map::hasBlock(const glm::vec3& position) const {
        auto it = std::find_if(m_blocks.begin(), m_blocks.end(), 
            [&position](const Block& b) { 
                return b.position == position; 
            });
        return it != m_blocks.end();
    }

    bool Map::checkCollision(const glm::vec3& boxMin, const glm::vec3& boxMax) const {
        int minX = std::floor(boxMin.x);
        int maxX = std::floor(boxMax.x);
        int minY = std::floor(boxMin.y);
        int maxY = std::floor(boxMax.y);
        int minZ = std::floor(boxMin.z);
        int maxZ = std::floor(boxMax.z);

        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                for (int z = minZ; z <= maxZ; ++z) {
                    if (hasBlock(glm::vec3((float)x, (float)y, (float)z))) {
                        // Границы блока в мировых координатах (от -0.5 до +0.5)
                        float blockMinX = x - 0.5f;
                        float blockMaxX = x + 0.5f;
                        float blockMinY = y - 0.5f;
                        float blockMaxY = y + 0.5f;
                        float blockMinZ = z - 0.5f;
                        float blockMaxZ = z + 0.5f;

                        // Строгое пересечение (если мы просто касаемся грани, это не коллизия)
                        if (boxMin.x < blockMaxX && boxMax.x > blockMinX &&
                            boxMin.y < blockMaxY && boxMax.y > blockMinY &&
                            boxMin.z < blockMaxZ && boxMax.z > blockMinZ) {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    void Map::generateArenaMap() {
        m_blocks.clear(); // Очищаем карту перед генерацией

        // Цвета для разных частей арены
        glm::vec3 colorFloor  = glm::vec3(0.2f, 0.3f, 0.2f); // Темно-зеленый (трава)
        glm::vec3 colorWall   = glm::vec3(0.4f, 0.4f, 0.45f); // Серый камень
        glm::vec3 colorHigh   = glm::vec3(0.45f, 0.35f, 0.25f); // Коричневый (земля/дерево)
        glm::vec3 colorCover  = glm::vec3(0.6f, 0.6f, 0.65f); // Светло-серый (бетон)

        int arenaSize = 10; // От -10 до 10 (20x20 блоков)

        // 1. Создаем пол
        for (int x = -arenaSize; x < arenaSize; ++x) {
            for (int z = -arenaSize; z < arenaSize; ++z) {
                addBlock(glm::vec3((float)x, -1.0f, (float)z), colorFloor);
            }
        }

        // 2. Создаем стены по периметру (высотой 3 блока)
        for (int y = 0; y < 3; ++y) {
            for (int i = -arenaSize; i < arenaSize; ++i) {
                // Северная и Южная стены
                addBlock(glm::vec3((float)i, (float)y, (float)-arenaSize), colorWall);
                addBlock(glm::vec3((float)i, (float)y, (float)(arenaSize - 1)), colorWall);
                // Западная и Восточная стены
                addBlock(glm::vec3((float)-arenaSize, (float)y, (float)i), colorWall);
                addBlock(glm::vec3((float)(arenaSize - 1), (float)y, (float)i), colorWall);
            }
        }

        // 3. Возвышенности (платформы) по углам
        // Игрок (Юг)
        for (int x = -8; x <= -5; ++x) {
            for (int z = 5; z <= 8; ++z) {
                addBlock(glm::vec3((float)x, 0.0f, (float)z), colorHigh);
            }
        }
        for (int x = 5; x <= 8; ++x) {
            for (int z = 5; z <= 8; ++z) {
                addBlock(glm::vec3((float)x, 0.0f, (float)z), colorHigh);
            }
        }
        
        // Враги (Север)
        for (int x = -8; x <= -5; ++x) {
            for (int z = -8; z <= -5; ++z) {
                addBlock(glm::vec3((float)x, 0.0f, (float)z), colorHigh);
            }
        }
        for (int x = 5; x <= 8; ++x) {
            for (int z = -8; z <= -5; ++z) {
                addBlock(glm::vec3((float)x, 0.0f, (float)z), colorHigh);
            }
        }

        // 4. Укрытия (симметричные)
        // Центральное большое укрытие
        for (int x = -2; x <= 2; ++x) {
            for (int y = 0; y < 2; ++y) {
                addBlock(glm::vec3((float)x, (float)y, -1.0f), colorCover);
                addBlock(glm::vec3((float)x, (float)y, 0.0f), colorCover);
            }
        }

        // Боковые укрытия (у игрока)
        addBlock(glm::vec3(-5.0f, 0.0f, 2.0f), colorCover);
        addBlock(glm::vec3(-5.0f, 1.0f, 2.0f), colorCover);
        addBlock(glm::vec3(5.0f, 0.0f, 2.0f), colorCover);
        addBlock(glm::vec3(5.0f, 1.0f, 2.0f), colorCover);

        // Боковые укрытия (у врагов)
        addBlock(glm::vec3(-5.0f, 0.0f, -3.0f), colorCover);
        addBlock(glm::vec3(-5.0f, 1.0f, -3.0f), colorCover);
        addBlock(glm::vec3(5.0f, 0.0f, -3.0f), colorCover);
        addBlock(glm::vec3(5.0f, 1.0f, -3.0f), colorCover);
    }

} // namespace Engine