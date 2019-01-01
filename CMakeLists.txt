cmake_minimum_required(VERSION 2.8)

project(alpine)

link_directories("/usr/local/lib/")
include_directories("/usr/local/include")

file(GLOB_RECURSE SOURCES "c++/*.cpp")
file(GLOB_RECURSE HEADERS "c++/*.hpp")

add_executable(alpine
  ${SOURCES})

target_compile_options(alpine
  PRIVATE "-O2"
  PRIVATE "-std=c++11")

# target_link_libraries(alpine "-framework sfml-window -framework sfml-graphics -framework sfml-system -framework sfml-audio")
target_link_libraries(alpine sfml-window)
target_link_libraries(alpine sfml-graphics)
target_link_libraries(alpine sfml-system)
target_link_libraries(alpine sfml-audio)
target_link_libraries(alpine pthread)
target_link_libraries(alpine ebl-lisp)

add_custom_target(clang-format
  COMMAND clang-format --style=file -i ${SOURCES} ${HEADERS}
  COMMENT "Running clang-format"
  VERBATIM)
