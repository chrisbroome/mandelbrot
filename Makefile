
all: mandelbrot

run:
	./mandelbrot

clean:
	rm -f mandelbrot *.o

mandelbrot: main.o gradient.o
	g++ main.o gradient.o -o mandelbrot -F /Library/Frameworks -framework SFML -framework sfml-system -framework sfml-window -framework sfml-graphics

main.o: main.cpp
	g++ -std=c++11 -c main.cpp

gradient.o: gradient.h gradient.cpp
	g++ -std=c++11 -c gradient.cpp

.PHONY: all clean run
