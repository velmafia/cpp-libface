#include <iostream>
#include <string>
#include "libfaceapi.hpp"
#include <include/types.hpp>

int
main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Filepath and word must be in the arguments");
        return -1;
    }

    Souffleur souffleur;
    if (!souffleur.import(argv[1])) {
        fprintf(stderr, "Error while import");
        return -1;
    }

    int n = 32;
    std::string q = argv[2];
    vp_t results = souffleur.prompt(q, n);

    int size = results.size();
    for (int i = 0; i < size; ++i) {
        std::cout << results[i].phrase << std::endl;
    }

    return 0;
}
