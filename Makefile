
all: mandelbrot

run:
	./mandelbrot

clean:
	rm -f mandelbrot *.o

mandelbrot: main.o
	g++ main.o -o mandelbrot -F /Library/Frameworks -framework SFML -framework sfml-system -framework sfml-window -framework sfml-graphics

main.o: main.cpp
	g++ -std=c++11 -c main.cpp

.PHONY: all
