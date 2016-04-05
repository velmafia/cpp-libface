#include "libfaceapi.hpp"
#include "defines.hpp"
#include "parser.hpp"

// C-headers
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <stdlib.h>
#include <time.h>
//#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <mman-win32/mman.h>
#else
#include <sys/mman.h>
#endif

// Custom-includes
#include <include/segtree.hpp>
#include <include/sparsetable.hpp>
#include <include/benderrmq.hpp>
#include <include/phrase_map.hpp>
#include <include/suggest.hpp>
#include <include/types.hpp>
#include <include/utils.hpp>

// C++-headers
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>

#include <assert.h>
#include <fcntl.h>


#if !defined NMAX
#define NMAX 32
#endif


#if !defined INPUT_LINE_SIZE
// Max. line size is 8191 bytes.
#define INPUT_LINE_SIZE 8192
#endif

// How many bytes to reserve for the output string
#define OUTPUT_SIZE_RESERVE 4096

// Undefine the macro below to use C-style I/O routines.
// #define USE_CXX_IO


off_t
file_size(const char *path) {
    struct stat sbuf;
    int r = stat(path, &sbuf);

    assert(r == 0);
    if (r < 0) {
        return 0;
    }

    return sbuf.st_size;
}

char
to_lowercase(char c) {
    return std::tolower(c);
}

inline void
str_lowercase(std::string &str) {
    std::transform(str.begin(), str.end(),
                   str.begin(), to_lowercase);

}

enum { IMPORT_FILE_NOT_FOUND = 1,
       IMPORT_MUNMAP_FAILED  = 2,
       IMPORT_MMAP_FAILED    = 3
};

bool is_EOF(FILE *pf) { return feof(pf); }
bool is_EOF(std::ifstream fin) { return !!fin; }

void get_line(FILE *pf, char *buff, int buff_len, int &read_len) {
    (void)buff_len;
    char *got = fgets(buff, INPUT_LINE_SIZE, pf);
    if (!got) {
        read_len = -1;
        return;
    }
    read_len = strlen(buff);
    if (read_len > 0 && buff[read_len - 1] == '\n') {
        buff[read_len - 1] = '\0';
    }
}

void get_line(std::ifstream fin, char *buff, int buff_len, int &read_len) {
    fin.getline(buff, buff_len);
    read_len = fin.gcount();
    buff[INPUT_LINE_SIZE - 1] = '\0';
}

Souffleur::Souffleur():
    if_mmap_addr(NULL),
    if_length(0),
    line_limit(-1),
    building(false)
{
}

int Souffleur::do_import(std::string file, uint_t limit,
                         int &rnadded, int &rnlines)  {
    bool is_input_sorted = true;
#if defined USE_CXX_IO
    std::ifstream fin(file.c_str());
#else
    FILE *fin = fopen(file.c_str(), "r");
#endif

    int fd = open(file.c_str(), O_RDONLY);

    DCERR("handle_import::file:" << file << "[fin: " << (!!fin) << ", fd: " << fd << "]" << endl);

    if (!fin || fd == -1) {
        perror("fopen");
        return -IMPORT_FILE_NOT_FOUND;
    }
    else {
        building = true;
        int nlines = 0;
        int foffset = 0;

        if (if_mmap_addr) {
            int r = munmap(if_mmap_addr, if_length);
            if (r < 0) {
                perror("munmap");
                building = false;
                return -IMPORT_MUNMAP_FAILED;
            }
        }

        // Potential race condition + not checking for return value
        if_length = file_size(file.c_str());

        // mmap() the input file in
        if_mmap_addr = (char*)mmap(NULL, if_length, PROT_READ, MAP_SHARED, fd, 0);
        if (if_mmap_addr == MAP_FAILED) {
            fprintf(stderr, "length: %llu, fd: %d\n", if_length, fd);
            perror("mmap");
            if (fin) { fclose(fin); }
            if (fd != -1) { close(fd); }
            building = false;
            return -IMPORT_MMAP_FAILED;
        }

        pm.repr.clear();
        char buff[INPUT_LINE_SIZE];
        std::string prev_phrase;

        while (!is_EOF(fin) && limit--) {
            buff[0] = '\0';

            int llen = -1;
            get_line(fin, buff, INPUT_LINE_SIZE, llen);
            if (llen == -1) {
                break;
            }

            ++nlines;

            int weight = 0;
            std::string phrase;
            StringProxy snippet;
            InputLineParser(if_mmap_addr, foffset, buff, &weight, &phrase, &snippet, if_mmap_addr, if_length).start_parsing();

            foffset += llen;

            if (!phrase.empty()) {
                str_lowercase(phrase);
                DCERR("Adding: " << weight << ", " << phrase << ", " << std::string(snippet) << endl);
                pm.insert(weight, phrase, snippet);
            }
            if (is_input_sorted && prev_phrase <= phrase) {
                prev_phrase.swap(phrase);
            } else if (is_input_sorted) {
                is_input_sorted = false;
            }
        }

        DCERR("Creating PhraseMap::Input is " << (!is_input_sorted ? "NOT " : "") << "sorted\n");

        fclose(fin);
        pm.finalize(is_input_sorted);
        vui_t weights;
        for (size_t i = 0; i < pm.repr.size(); ++i) {
            weights.push_back(pm.repr[i].weight);
        }
        st.initialize(weights);

        rnadded = weights.size();
        rnlines = nlines;

        building = false;
    }

    return 0;
}

bool Souffleur::import(const char *ac_file) {
    bool result = false;
    if (ac_file) {
        int nadded, nlines;
        const time_t start_time = time(NULL);
        int ret = do_import(ac_file, line_limit, nadded, nlines);
        if (ret < 0) {
            switch (-ret) {
                case IMPORT_FILE_NOT_FOUND:
                    fprintf(stderr, "The file '%s' was not found\n", ac_file);
                    break;

                case IMPORT_MUNMAP_FAILED:
                    fprintf(stderr, "munmap(2) on file '%s' failed\n", ac_file);
                    break;

                case IMPORT_MMAP_FAILED:
                    fprintf(stderr, "mmap(2) on file '%s' failed\n", ac_file);
                    break;

                default:
                    cerr<<"ERROR::Unknown error: "<<ret<<endl;
            }
            return 1;
        }
        else {
            fprintf(stderr, "INFO::Successfully added %d/%d records from \"%s\" in %d second(s)\n",
                    nadded, nlines, ac_file, (int)(time(NULL) - start_time));
            result = true;
        }
    }

    return result;
}

vp_t Souffleur::prompt(std::string prefix, uint_t n) {
    return ::suggest(pm, st, prefix, n);
}
