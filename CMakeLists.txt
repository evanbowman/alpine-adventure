cmake_minimum_required(VERSION 2.8)

project(alpine)

link_directories("/usr/local/lib/")

add_executable(alpine
  c++/main.cpp
  c++/game.cpp
  c++/script.cpp
  c++/textureDB.cpp
  c++/scriptInterface.cpp
  c++/context.cpp
  c++/camera.cpp)

target_compile_options(alpine
  PRIVATE "-O2"
  PRIVATE "-std=c++11")

# target_link_libraries(alpine "-framework sfml-window -framework sfml-graphics -framework sfml-system -framework sfml-audio")
target_link_libraries(alpine sfml-window)
target_link_libraries(alpine sfml-graphics)
target_link_libraries(alpine sfml-system)
target_link_libraries(alpine sfml-audio)
target_link_libraries(alpine pthread)
target_link_libraries(alpine chibi-scheme)
