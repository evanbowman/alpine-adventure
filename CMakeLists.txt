cmake_minimum_required(VERSION 2.8)

project(alpine)

add_executable(alpine
  main.cpp
  game.cpp
  script.cpp
  textureDB.cpp
  scriptInterface.cpp
  context.cpp
  camera.cpp)

target_compile_options(alpine
  PRIVATE "-O2"
  PRIVATE "-std=c++11")

# target_link_libraries(alpine "-framework sfml-window -framework sfml-graphics -framework sfml-system -framework sfml-audio")
target_link_libraries(alpine sfml-window)
target_link_libraries(alpine sfml-graphics)
target_link_libraries(alpine sfml-system)
target_link_libraries(alpine sfml-audio)

target_link_libraries(alpine chibi-scheme)
