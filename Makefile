NAME=mandelbrot
CXX=g++
CXXFLAGS=-std=c++11

SRCDIR:=src
OBJDIR:=obj
BINDIR:=bin
TGT:=$(BINDIR)/mandelbrot

MODS:=main
MODS+=gradient
MODS+=color-palette

SRCS+=$(addprefix $(SRCDIR)/,$(addsuffix .cpp,$(MODS)))
OBJS+=$(addprefix $(OBJDIR)/,$(addsuffix .o,$(MODS)))

LIBS:=SFML
LIBS+=sfml-system
LIBS+=sfml-window
LIBS+=sfml-graphics

FWKDIR:=/Library/Frameworks
FWKS:=$(addprefix -framework ,$(LIBS))

all: $(TGT)

$(TGT): $(OBJS)
	$(CXX) $(OBJS) -o $@ -F $(FWKDIR) $(FWKS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -o $@ -c $^ $(CXXFLAGS)

run: $(TGT)
	./$(TGT)

clean:
	rm -f $(TGT) $(OBJS)

.PHONY: all clean run
