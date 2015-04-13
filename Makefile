CXX=g++
CXXFLAGS=-std=c++11
TGT=mandelbrot

SRCS := main
SRCS += gradient
SRCS += color-palette

OBJS += $(addsuffix .o,$(SRCS))

LIBS := SFML
LIBS += sfml-system
LIBS += sfml-window
LIBS += sfml-graphics

FWK_PATH := /Library/Frameworks
FWKS := $(addprefix -framework ,$(LIBS))

all: $(TGT)

run: $(TGT)
	./$(TGT)

clean:
	rm -f $(TGT) $(OBJS)

$(TGT): $(OBJS)
	$(CXX) $(OBJS) -o $@ -F $(FWK_PATH) $(FWKS)

%.o: %.cpp
	$(CXX) -o $@ -c $^ $(CXXFLAGS)

.PHONY: all clean run
