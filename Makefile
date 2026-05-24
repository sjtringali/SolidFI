.PHONY: all check doc clean

CXX      ?= c++
CXXFLAGS  = -fsyntax-only -std=c++17 -I include

all: check doc

check:
	$(CXX) $(CXXFLAGS) check/l1.cpp
	$(CXX) $(CXXFLAGS) check/l0.cpp

doc:
	doxygen Doxyfile
	find docs/html -name "*.html" -exec sed -i \
		's/&lt; \([A-Za-z0-9_]\)/\&lt;\1/g; s/\([A-Za-z0-9_]\) &gt;/\1\&gt;/g' {} \;

clean:
	rm -rf docs/
