#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Engine {

    // Новая структура для хранения блока (позиция + цвет)
    struct Block {
        glm::vec3 position;
        glm::vec3 color;
    };

    class Map {
    public:
        Map();

        // Изменено: теперь принимает позицию и цвет
        void addBlock(const glm::vec3& position, const glm::vec3& color);
        
        void removeBlock(const glm::vec3& position);
        bool hasBlock(const glm::vec3& position) const;
        
        // Изменено: возвращает вектор структур Block
        const std::vector<Block>& getBlocks() const { return m_blocks; }
        
        bool checkCollision(const glm::vec3& boxMin, const glm::vec3& boxMax) const;

        // Новая функция генерации арены
        void generateArenaMap();

    private:
        std::vector<Block> m_blocks;
    };

} // namespace Engine