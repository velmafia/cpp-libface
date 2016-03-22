CXXFLAGS=       -Wall $(COPT) -D_FILE_OFFSET_BITS=64
LINKFLAGS=	-lm -lrt -pthread
INCDEPS=        include/segtree.hpp include/sparsetable.hpp include/benderrmq.hpp \
                include/phrase_map.hpp include/suggest.hpp include/types.hpp \
                include/utils.hpp
INCDIRS=        -I . -I deps
OBJDEPS=
HTTPSERVERDEPS= src/httpserver.cpp include/httpserver.hpp include/utils.hpp \
		include/types.hpp

.PHONY: all clean debug test perf

all: CXXFLAGS += -O2
all: targets

debug: CXXFLAGS += -g -DDEBUG
debug: targets

test: CXXFLAGS += -g -DDEBUG
perf: CXXFLAGS += -O2

targets: lib-face

lib-face: src/main.cpp $(OBJDEPS) $(INCDEPS)
	$(CXX) -o lib-face src/main.cpp $(OBJDEPS) $(INCDIRS) $(CXXFLAGS) $(LINKFLAGS)

test:
	$(CXX) -o tests/containers tests/containers.cpp -I . $(CXXFLAGS)
	tests/containers

perf:
	$(CXX) -o tests/rmq_perf tests/rmq_perf.cpp -I . $(CXXFLAGS)
	tests/rmq_perf

clean:
	rm -f lib-face tests/containers tests/rmq_perf
