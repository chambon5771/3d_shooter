#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Engine {

    class Shader {

        public:
            Shader(const char* vertexCode, const char* fragmentCode);
            ~Shader();


            Shader(const Shader&) = delete;
            Shader&  operator=(const Shader&) = delete;

            void use() const;


            void setMat4(const std::string& name, const glm::mat4& mat) const;

            unsigned int getID() const { return m_id; };

        private:
        unsigned int m_id = 0;
        void checkCompileErrors(unsigned int shader, const std::string& type);
    };

} // namespace Engine