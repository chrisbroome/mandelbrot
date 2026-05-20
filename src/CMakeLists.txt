list(APPEND sources color-palette.cpp)
list(APPEND sources gradient.cpp)
list(APPEND sources transforms.cpp)
list(APPEND sources main.cpp)

if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/usr/local")
endif()
find_package(SFML 3 COMPONENTS Graphics REQUIRED)

add_executable(mandelbrot ${sources})
set_property(TARGET mandelbrot PROPERTY CXX_STANDARD 17)

target_link_libraries(mandelbrot SFML::Graphics)
if(MSVC)
  target_compile_options(mandelbrot PRIVATE /W4 /WX)
else(MSVC)
  target_compile_options(mandelbrot PRIVATE -Wall -Wextra -Werror -pedantic)
endif(MSVC)
