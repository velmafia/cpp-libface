#ifndef LIBFACE_PHRASE_MAP_HPP
#define LIBFACE_PHRASE_MAP_HPP

#include <algorithm>
#include <string>
#include <vector>

#include "types.hpp"

using namespace std;

struct PrefixFinder {
    bool
    operator()(std::string const& prefix, phrase_t const &target) {
#if 1
        const int ppos = target.phrase.compare(0, prefix.size(), prefix);
        return ppos > 0;
#else
        const int ppos = target.phrase.find(prefix);
        if (!ppos) {
            return false;
        }
        return prefix < target.phrase;
#endif
    }

    bool
    operator()(phrase_t const& target, std::string const &prefix) {
#if 1
        const int ppos = target.phrase.compare(0, prefix.size(), prefix);
        return ppos < 0;
#else
        const int ppos = target.phrase.find(prefix);
        if (!ppos) {
            return false;
        }
        return target.phrase < prefix;
#endif
    }


    // ONLY FOR DEBUG VERSION TO BUILD
    // BUG in headers of 2012/2013 MSVS
    // https://connect.microsoft.com/VisualStudio/feedback/details/807044/c-stdlib-comparator-debugging-breaks-compile-of-custom-comparators-in-std-equal-range-family-of-functions
#ifdef _DEBUG
    bool
    operator()(phrase_t const &target, phrase_t const &prefix) {
#if 1
        const int ppos = target.phrase.compare(0, prefix.phrase.size(), prefix.phrase);
        return ppos > 0;
#else
        const int ppos = target.phrase.find(prefix);
        if (!ppos) {
            return false;
        }
        return prefix < target.phrase;
#endif
    }
#endif
};

class PhraseMap {
public:
    vp_t repr;

public:
    PhraseMap(uint_t _len = 15000000) {
        this->repr.reserve(_len);
    }

    void
    insert(uint_t weight, std::string const& p, StringProxy const& s) {
        this->repr.push_back(phrase_t(weight, p, s));
    }

    void
    finalize(int sorted = 0) {
        if (!sorted) {
            std::sort(this->repr.begin(), this->repr.end());
        }
    }

    pvpi_t
    query(std::string const &prefix) {
        return std::equal_range(this->repr.begin(), this->repr.end(),
                                prefix, PrefixFinder());
    }

};


pvpi_t
naive_query(PhraseMap &pm, std::string prefix);

void
show_indexes(PhraseMap &pm, std::string prefix);

namespace phrase_map {
    int
    test();
}

#endif // LIBFACE_PHRASE_MAP_HPP
