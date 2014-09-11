
all: mandelbrot

run: mandelbrot
	./mandelbrot

clean:
	rm -f mandelbrot *.o

mandelbrot: main.o gradient.o color-palette.o
	g++ main.o gradient.o color-palette.o -o mandelbrot -F /Library/Frameworks -framework SFML -framework sfml-system -framework sfml-window -framework sfml-graphics

main.o: main.cpp
	g++ -std=c++11 -c main.cpp

gradient.o: gradient.h gradient.cpp
	g++ -std=c++11 -c gradient.cpp

color-palette.o: color-palette.h color-palette.cpp
	g++ -std=c++11 -c color-palette.cpp

.PHONY: all clean run
