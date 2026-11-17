cmake_minimum_required(VERSION 3.14)
project(3D_Shooter_Project VERSION 1.0 LANGUAGES CXX)

# Стандарт C++20 с запретом отката на более старый
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ============================================================
# GLFW 3.4 — окно, контекст OpenGL, ввод
# ============================================================
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ============================================================
# GLM 1.0.1 — математика (header-only)
# ============================================================
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm)

# ============================================================
# GLAD v0.1.36 — загрузчик расширений OpenGL 3.3 Core
# ВАЖНО: set(CMAKE_POLICY_VERSION_MINIMUM) СТРОГО до Declare —
# иначе современный CMake заблокирует GLAD со старыми политиками
# ============================================================
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v0.1.36
)
set(GLAD_PROFILE "core"  CACHE STRING "" FORCE)
set(GLAD_API     "gl=3.3" CACHE STRING "" FORCE)
FetchContent_MakeAvailable(glad)

# ============================================================
# Подпроекты — ТОЛЬКО после FetchContent,
# иначе таргеты glfw/glm/glad ещё не существуют
# ============================================================
add_subdirectory(engine)
add_subdirectory(game)