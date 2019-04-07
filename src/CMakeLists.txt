
list(APPEND sources color-palette.cpp)
list(APPEND sources gradient.cpp)
list(APPEND sources transforms.cpp)
list(APPEND sources main.cpp)

find_package(SFML 2.5 COMPONENTS system graphics audio window REQUIRED)

#message(${SFML}/include)
#message("SFML Include dir is ${SFML_INCLUDE_DIR}")
include_directories(${SFML}/include)

add_executable(mandelbrot ${sources})
target_link_libraries(mandelbrot sfml-system sfml-window sfml-graphics)
if(MSVC)
  target_compile_options(mandelbrot PRIVATE /W4 /WX)
else(MSVC)
  target_compile_options(mandelbrot PRIVATE -Wall -Wextra -Werror -pedantic)
endif(MSVC)

