.PHONY: all check doc clean

CXX      ?= c++
CXXFLAGS  = -fsyntax-only -std=c++17 -I spec/include

GIT_SHA  := $(shell git rev-parse --short HEAD 2>/dev/null || echo unknown)
export GIT_SHA

all: check doc

check:
	$(CXX) $(CXXFLAGS) check/l1.cpp
	$(CXX) $(CXXFLAGS) check/l0.cpp

doc:
	doxygen spec/docs/Doxyfile
	find spec/docs/html -name "*.html" -exec sed -i \
		's/&lt; \([A-Za-z0-9_]\)/\&lt;\1/g; s/\([A-Za-z0-9_]\) &gt;/\1\&gt;/g' {} \;

clean:
	$(RM) -rf spec/docs/html
