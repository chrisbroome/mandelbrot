list(APPEND sources color-palette.cpp)
list(APPEND sources gradient.cpp)
list(APPEND sources transforms.cpp)
list(APPEND sources main.cpp)

find_package(SFML 2.5 COMPONENTS system graphics audio window REQUIRED)

message("SFML Include dir is ${SFML_DIR}/include")
include_directories(${SFML_DIR}/include)

add_executable(mandelbrot ${sources})
set_property(TARGET mandelbrot PROPERTY CXX_STANDARD 14)

target_link_libraries(mandelbrot sfml-system sfml-window sfml-graphics)
if(MSVC)
  target_compile_options(mandelbrot PRIVATE /W4 /WX)
else(MSVC)
  target_compile_options(mandelbrot PRIVATE -Wall -Wextra -Werror -pedantic)
endif(MSVC)
