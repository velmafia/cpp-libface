#ifndef LIBFACE_SUGGEST_HPP
#define LIBFACE_SUGGEST_HPP

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "phrase_map.hpp"
#include "types.hpp"
#include "utils.hpp"

struct PhraseRange {
    // first & last are both inclusive of the range. i.e. The range is
    // [first, last] and NOT [first, last)
    uint_t first, last;

    // weight is the value of the best solution in the range [first,
    // last] and index is the index of this best solution in the
    // original array of strings.
    uint_t weight, index;

    PhraseRange(uint_t f, uint_t l, uint_t w, uint_t i)
        : first(f), last(l), weight(w), index(i)
    { }

    bool
    operator<(PhraseRange const &rhs) const {
        return this->weight < rhs.weight;
    }
};

typedef std::priority_queue<PhraseRange> pqpr_t;



vp_t
suggest(PhraseMap &pm, RMQ &st, std::string prefix, uint_t n = 16) {
    pvpi_t phrases = pm.query(prefix);
    // cerr<<"Got "<<phrases.second - phrases.first<<" candidate phrases from PhraseMap"<<std::endl;

    uint_t first = phrases.first  - pm.repr.begin();
    uint_t last  = phrases.second - pm.repr.begin();

    if (first == last) {
        return vp_t();
    }

    vp_t ret;
    --last;

    pqpr_t heap;
    pui_t best = st.query_max(first, last);
    heap.push(PhraseRange(first, last, best.first, best.second));

    while (ret.size() < n && !heap.empty()) {
        PhraseRange pr = heap.top();
        heap.pop();
        // cerr<<"Top phrase is at index: "<<pr.index<<std::endl;
        // cerr<<"And is: "<<pm.repr[pr.index].first<<std::endl;

        ret.push_back(pm.repr[pr.index]);

        uint_t lower = pr.first;
        uint_t upper = pr.index - 1;

        // Prevent underflow
        if (pr.index - 1 < pr.index && lower <= upper) {
            // cerr<<"[1] adding to heap: "<<lower<<", "<<upper<<", "<<best.first<<", "<<best.second<<std::endl;

            best = st.query_max(lower, upper);
            heap.push(PhraseRange(lower, upper, best.first, best.second));
        }

        lower = pr.index + 1;
        upper = pr.last;

        // Prevent overflow
        if (pr.index + 1 > pr.index && lower <= upper) {
            // cerr<<"[2] adding to heap: "<<lower<<", "<<upper<<", "<<best.first<<", "<<best.second<<std::endl;

            best = st.query_max(lower, upper);
            heap.push(PhraseRange(lower, upper, best.first, best.second));
        }
    }

    return ret;
}

vp_t
naive_suggest(PhraseMap& pm, RMQ& st, std::string prefix, uint_t n = 16) {
    (void)st;
    pvpi_t phrases = pm.query(prefix);
    std::vector<uint_t> indexes;
    vp_t ret;

    while (phrases.first != phrases.second) {
        indexes.push_back(phrases.first - pm.repr.begin());
        ++phrases.first;
    }

    while (ret.size() < n && !indexes.empty()) {
        uint_t mi = 0;
        for (size_t i = 1; i < indexes.size(); ++i) {
            if (pm.repr[indexes[i]].weight > pm.repr[indexes[mi]].weight) {
                mi = i;
            }
        }
        ret.push_back(pm.repr[indexes[mi]]);
        indexes.erase(indexes.begin() + mi);
    }
    return ret;
}

namespace _suggest {
    int
    test() {
        PhraseMap pm;
        pm.insert(1, "duckduckgo", "");
        pm.insert(2, "duckduckgeese", "");
        pm.insert(1, "duckduckgoose", "");
        pm.insert(9, "duckduckgoo", "");
        pm.insert(10, "duckgo", "");
        pm.insert(3, "dukgo", "");
        pm.insert(2, "luckkuckgo", "");
        pm.insert(5, "chuckchuckgo", "");
        pm.insert(15, "dilli - no one killed jessica", "");
        pm.insert(11, "aaitbaar - no one killed jessica", "");

        pm.finalize();

        RMQ st;
        vui_t weights;
        for (size_t i = 0; i < pm.repr.size(); ++i) {
            weights.push_back(pm.repr[i].weight);
        }

        st.initialize(weights);

        std::cout<<"\n";
        std::cout<<"suggest(\"d\"):\n"<<suggest(pm, st, "d")<<std::endl;
        std::cout<<"naive_suggest(\"d\"):\n"<<naive_suggest(pm, st, "d")<<std::endl;

        std::cout<<"\n";
        std::cout<<"suggest(\"a\"):\n"<<suggest(pm, st, "a")<<std::endl;
        std::cout<<"naive_suggest(\"a\"):\n"<<naive_suggest(pm, st, "a")<<std::endl;

        std::cout<<"\n";
        std::cout<<"suggest(\"b\"):\n"<<suggest(pm, st, "b")<<std::endl;
        std::cout<<"naive_suggest(\"b\"):\n"<<naive_suggest(pm, st, "b")<<std::endl;

        std::cout<<"\n";
        std::cout<<"suggest(\"duck\"):\n"<<suggest(pm, st, "duck")<<std::endl;
        std::cout<<"naive_suggest(\"duck\"):\n"<<naive_suggest(pm, st, "duck")<<std::endl;

        std::cout<<"\n";
        std::cout<<"suggest(\"k\"):\n"<<suggest(pm, st, "k")<<std::endl;
        std::cout<<"naive_suggest(\"k\"):\n"<<naive_suggest(pm, st, "k")<<std::endl;

        std::cout<<"\n";
        std::cout<<"suggest(\"ka\"):\n"<<suggest(pm, st, "ka")<<std::endl;
        std::cout<<"naive_suggest(\"ka\"):\n"<<naive_suggest(pm, st, "ka")<<std::endl;

        std::cout<<"\n";
        std::cout<<"suggest(\"c\"):\n"<<suggest(pm, st, "c")<<std::endl;
        std::cout<<"naive_suggest(\"c\"):\n"<<naive_suggest(pm, st, "c")<<std::endl;

        return 0;
    }
}

#endif // LIBFACE_SUGGEST_HPP
