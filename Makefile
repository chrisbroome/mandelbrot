NAME=mandelbrot
CXX=g++
CXXFLAGS+=-std=c++17 -Wall -Wextra -Werror -pedantic

SRCEXT:=.cpp
OBJEXT:=.o
SRCDIR:=src
OBJDIR:=obj
BINDIR:=bin
TGT:=$(BINDIR)/$(NAME)

MODS:=main
MODS+=gradient
MODS+=color-palette
MODS+=transforms

SRCS+=$(addprefix $(SRCDIR)/,$(addsuffix $(SRCEXT),$(MODS)))
OBJS+=$(addprefix $(OBJDIR)/,$(addsuffix $(OBJEXT),$(MODS)))

LIBS+=sfml-graphics
LIBS+=sfml-system
LIBS+=sfml-window
ifeq ($(OS),Windows_NT)
else
  UNAME=$(shell uname)
	ifeq ($(UNAME),Linux)
	  CXXFLAGS += -D Linux
    LIBRARIES := $(addprefix -l,$(LIBS))
	endif
	ifeq ($(UNAME),Darwin)
	  CXXFLAGS += -D OSX -I/usr/local/include
		LIBRARIES := -L/usr/local/lib $(addprefix -l,$(LIBS))
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
