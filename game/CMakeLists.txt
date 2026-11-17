
add_executable(game)

target_sources(game PRIVATE
    src/main.cpp
)

# PRIVATE — game конечный потребитель, никого «выше» нет.
# engine тянет за собой glfw/glm/glad транзитивно (PUBLIC в engine).
target_link_libraries(game PRIVATE engine)