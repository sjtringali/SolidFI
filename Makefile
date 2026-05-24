.PHONY: all check doc clean

CXX      ?= c++
CXXFLAGS  = -fsyntax-only -std=c++17 -I include

all: check doc

check:
	$(CXX) $(CXXFLAGS) include/solidfi/solidfi.hpp
	$(CXX) $(CXXFLAGS) include/solidfi/l0/L0.hpp

doc:
	doxygen Doxyfile

clean:
	rm -rf docs/
