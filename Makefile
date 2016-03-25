CXXFLAGS=       -Wall $(COPT) -D_FILE_OFFSET_BITS=64 -DLIBFACE_STATIC
LINKFLAGS=
INCDEPS=        include/segtree.hpp include/sparsetable.hpp include/benderrmq.hpp \
                include/phrase_map.hpp include/suggest.hpp include/types.hpp \
                include/utils.hpp
INCDIRS=        -I . -I deps -I
LIBFACEDEPS=    src/parser.hpp src/defines.hpp src/libfaceapi.hpp
SOURCES=        src/main.cpp src/libfaceapi.cpp include/benderrmq.cpp include/phrase_map.cpp include/segtree.cpp include/sparsetable.cpp

.PHONY: all clean debug test perf

all:
	CXXFLAGS = $(CXXFLAGS) -O2
all: targets

debug:
	CXXFLAGS = $(CXXFLAGS) -g -DDEBUG
debug: targets

test:
	CXXFLAGS = $(CXXFLAGS) -g -DDEBUG
perf:
	CXXFLAGS = $(CXXFLAGS) -O2

targets: lib-face

lib-face: $(SOURCES) $(LIBFACEDEPS) $(OBJDEPS) $(INCDEPS)
	$(CXX) $(SOURCES) $(INCDIRS) $(CXXFLAGS) $(LINKFLAGS) -o main

test:
	$(CXX) -o tests/containers tests/containers.cpp -I . $(CXXFLAGS)
	tests/containers

perf:
	$(CXX) -o tests/rmq_perf tests/rmq_perf.cpp -I . $(CXXFLAGS)
	tests/rmq_perf

clean:
	rm -f lib-face tests/containers tests/rmq_perf
