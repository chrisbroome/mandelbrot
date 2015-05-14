cmake_minimum_required(VERSION 3.2)

project("mandelbrot")

add_executable(mandelbrot
  src/color-palette.cpp
  src/gradient.cpp
  src/main.cpp
)

set(FWKDIR /Library/Frameworks)
set(LIBS
  SFML
  sfml-system
  sfml-window
  sfml-graphics
)

foreach(lib ${LIBS})
  set(FWKS "${FWKS} -framework ${lib}")
endforeach()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -F ${FWKDIR} ${FWKS}")
