#ifndef LIBFACE_API
#define LIBFACE_API

#include <include/segtree.hpp>
#include <include/sparsetable.hpp>
#include <include/benderrmq.hpp>
#include <include/phrase_map.hpp>
#include <include/types.hpp>

#include <string>
#include "defines.hpp"

class LIBFACE_DLL_EXPORTED Souffleur
{
  public:
    Souffleur();

  private:
    int do_import(std::string file, uint_t limit,
                  int &rnadded, int &rnlines);
  public:
    bool import(const char *ac_file);
    vp_t prompt(std::string prefix, uint_t n = 16);

  private:
    PhraseMap pm;                   // Phrase Map (usually a sorted array of strings)
    RMQ st;                         // An instance of the RMQ Data Structure
    char *if_mmap_addr;      // Pointer to the mmapped area of the file
    off_t if_length;            // The length of the input file
    int line_limit;            // The number of lines to import from the input file
    volatile bool building;
};

#endif // LIBFACE_API
