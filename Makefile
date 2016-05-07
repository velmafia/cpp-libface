CXXFLAGS=       -Wall $(COPT) -D_FILE_OFFSET_BITS=64 -DLIBFACE_STATIC
LINKFLAGS=
INCDEPS=        include/segtree.hpp include/sparsetable.hpp include/benderrmq.hpp \
                include/phrase_map.hpp include/suggest.hpp include/types.hpp \
                include/utils.hpp
INCDIRS=        -I . -I deps -I
LIBFACEDEPS=    src/parser.hpp src/defines.hpp src/libfaceapi.hpp
SOURCES=        src/libfaceapi.cpp include/benderrmq.cpp include/phrase_map.cpp include/segtree.cpp include/sparsetable.cpp
DESTDIR=	../libs

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

targets: lib-face main

lib-face: $(SOURCES) $(LIBFACEDEPS) $(OBJDEPS) $(INCDEPS)
	$(CXX) -c $(SOURCES) $(INCDIRS) $(CXXFLAGS) $(LINKFLAGS)
	ar rcs libface.a libfaceapi.o benderrmq.o phrase_map.o segtree.o sparsetable.o

main: lib-face src/main.cpp
	$(CXX) -L. -lface src/main.cpp $(INCDIRS) $(CXXFLAGS) -o main

test:
	$(CXX) -o tests/containers tests/containers.cpp -I . $(CXXFLAGS)
	tests/containers

perf:
	$(CXX) -o tests/rmq_perf tests/rmq_perf.cpp -I . $(CXXFLAGS)
	tests/rmq_perf

install-lib: lib-face
	cp libface.a $(DESTDIR)
clean:
	rm -f lib-face tests/containers tests/rmq_perf
