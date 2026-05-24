.PHONY: all check doc clean

CXX      ?= c++
CXXFLAGS  = -fsyntax-only -std=c++17 -I include

all: check doc

check:
	$(CXX) $(CXXFLAGS) check/l1.cpp
	$(CXX) $(CXXFLAGS) check/l0.cpp

doc:
	doxygen Doxyfile

clean:
	rm -rf docs/
