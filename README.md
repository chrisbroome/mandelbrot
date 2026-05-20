# mandelbrot
A Mandelbrot Set viewer written in C++ using SFML

## Controls

### Navigation
| Key | Action |
|-----|--------|
| `W` | Pan up |
| `S` | Pan down |
| `A` | Pan left |
| `D` | Pan right |
| `R` | Reset view to the default position |
| `Escape` | Quit |

### Mouse
| Action | Result |
|--------|--------|
| Click and drag | Draw a selection rectangle, then release to zoom into that region |

### Window
The window can be resized freely; the view will adjust to the new dimensions.

## Building

Requires [SFML 3](https://www.sfml-dev.org/) and a C++17 compiler.

```sh
make
```

## Running

```sh
make run
# or
./bin/mandelbrot
```
