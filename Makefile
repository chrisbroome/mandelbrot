NAME=mandelbrot
CXX=g++
CXXFLAGS=-std=c++11 -Wall -Wextra

SRCEXT:=.cpp
OBJEXT:=.o
SRCDIR:=src
OBJDIR:=obj
BINDIR:=bin
TGT:=$(BINDIR)/$(NAME)

MODS:=main
MODS+=gradient
MODS+=color-palette

SRCS+=$(addprefix $(SRCDIR)/,$(addsuffix $(SRCEXT),$(MODS)))
OBJS+=$(addprefix $(OBJDIR)/,$(addsuffix $(OBJEXT),$(MODS)))

LIBS+=sfml-system
LIBS+=sfml-window
LIBS+=sfml-graphics
ifeq ($(OS),Windows_NT)
else
  UNAME=$(shell uname)
	ifeq ($(UNAME),Linux)
	  CCFLAGS += -D Linux
    LIBRARIES := $(addprefix -l,$(LIBS))
	endif
	ifeq ($(UNAME),Darwin)
	  CCFLAGS += -D OSX
		LIBS += SFML
		FWKDIR := /Library/Frameworks
		FWKS := $(addprefix -framework ,$(LIBS))
		LIBRARIES := $(FWKDIR) $(FWKS)
	endif
endif

all: $(TGT)

$(TGT): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBRARIES)

$(OBJDIR)/%$(OBJEXT): $(SRCDIR)/%$(SRCEXT)
	$(CXX) -o $@ -c $^ $(CXXFLAGS)

run: $(TGT)
	./$(TGT)

clean:
	rm -f $(TGT) $(OBJS)

.PHONY: all clean run
