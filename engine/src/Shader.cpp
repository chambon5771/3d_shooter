#include "Shader.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>

namespace Engine {

    Shader::Shader(const char* vertexCode, const char* fragmentCode){
        unsigned int vertex, fragment;

        // Компиляция вертекс шейдера
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexCode, nullptr);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        // Компиляция фрагмент шейдера
        fragment = glad_glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentCode, nullptr);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        // Сборка вертекс + фрагмент в 1 программу
        m_id = glCreateProgram();
        glAttachShader(m_id, vertex);
        glAttachShader(m_id, fragment);
        glLinkProgram(m_id);
        checkCompileErrors(m_id, "PROGRAMM");

        // Удаление шейдеров
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    Shader::~Shader() {
        if (m_id != 0){
            glDeleteProgram(m_id);
        }
    }


    void Shader::use() const {
        glUseProgram(m_id);
    }


    void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {

        // Ищем местоположение переменной в шейдере и передаём матрицу
        glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));

    }


    void Shader::checkCompileErrors(unsigned int shader, const std::string& type){
        int success;
        char infoLog[1024];
        
        if (type != "PROGRAMM") {
            // Проверка компиляции шейдера
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                throw std::runtime_error("Shader compilation error (" + type + ") :\n" + infoLog);
            }
        } else {
            // Проверка линковки программы
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                throw std::runtime_error("Shader linking error (" + type + ") :\n" + infoLog);
            }
        }
    }

} // namespace Engine