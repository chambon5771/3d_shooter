add_library(engine STATIC)

target_sources(engine PRIVATE
    src/Window.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Map.cpp
    src/Projectile.cpp
    src/Player.cpp
)

# PUBLIC — потому что game включает Window.hpp / Shader.hpp
# из engine/include/ и должен видеть этот путь
target_include_directories(engine PUBLIC include)

# PUBLIC — потому что engine выставляет glfw/glad/glm в своих
# публичных заголовках (Window.hpp включает <GLFW/glfw3.h>,
# <glad/glad.h>; Shader.hpp включает <glm/glm.hpp>).
# Потребитель (game) должен получить их транзитивно.
target_link_libraries(engine PUBLIC
    glfw
    glm
    glad
)