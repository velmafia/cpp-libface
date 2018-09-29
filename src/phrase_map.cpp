#include "phrase_map.hpp"

pvpi_t
naive_query(PhraseMap &pm, std::string prefix) {
    vpi_t f = pm.repr.begin(), l = pm.repr.begin();
    while (f != pm.repr.end() && f->phrase.substr(0, prefix.size()) < prefix) {
        ++f;
    }
    l = f;
    while (l != pm.repr.end() && l->phrase.substr(0, prefix.size()) == prefix) {
        ++l;
    }
    return std::make_pair(f, l);
}

void
show_indexes(PhraseMap &pm, std::string prefix) {
    pvpi_t nq = naive_query(pm, prefix);
    pvpi_t q  = pm.query(prefix);

    cout<<"naive[first] = "<<nq.first - pm.repr.begin()<<", naive[last] = "<<nq.second - pm.repr.begin()<<endl;
    cout<<"phmap[first] = "<<q.first - pm.repr.begin()<<", phmap[last] = "<<q.second - pm.repr.begin()<<endl;
}

namespace phrase_map {
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

        show_indexes(pm, "a");
        assert(naive_query(pm, "a") == pm.query("a"));

        show_indexes(pm, "b");
        assert(naive_query(pm, "b") == pm.query("b"));

        show_indexes(pm, "c");
        assert(naive_query(pm, "c") == pm.query("c"));

        show_indexes(pm, "d");
        assert(naive_query(pm, "d") == pm.query("d"));

        show_indexes(pm, "duck");
        assert(naive_query(pm, "duck") == pm.query("duck"));

        show_indexes(pm, "ka");
        assert(naive_query(pm, "ka") == pm.query("ka"));

        return 0;
    }
}
