project(Look_a_Cat)
add_executable(${PROJECT_NAME} camera.cpp cat.cpp  ground.cpp main.cpp window.cpp)
enable_abcg(${PROJECT_NAME})
