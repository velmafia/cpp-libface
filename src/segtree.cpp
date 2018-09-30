#include "segtree.hpp"

namespace segtree {

    pui_t
    naive_query_max(vui_t const& v, int i, int j) {
        uint_t mv = v[i];
        uint_t mi = i;
        while (i <= j) {
            if (v[i] > mv) {
                mv = v[i];
                mi = i;
            }
            ++i;
        }
        return pui_t(mv, mi);
    }

    int
    test() {
        vui_t v;
        v.push_back(45);
        v.push_back(4);
        v.push_back(5);
        v.push_back(2);
        v.push_back(99);
        v.push_back(41);
        v.push_back(45);
        v.push_back(45);
        v.push_back(51);
        v.push_back(89);
        v.push_back(1);
        v.push_back(3);
        v.push_back(5);
        v.push_back(98);

        for (int i = 0; i < 10; ++i) {
            // printf("%d: %d\n", i, log2(i));
        }

        SegmentTree st;
        st.initialize(v);

        printf("Testing SegmentTree implementation\n");
        printf("----------------------------------\n");

        for (size_t i = 0; i < v.size(); ++i) {
            for (size_t j = i; j < v.size(); ++j) {
                pui_t one = st.query_max(i, j);
                pui_t two = naive_query_max(v, i, j);
                printf("query_max(%u, %u) == (%u, %u)\n", (uint_t)i, (uint_t)j, one.first, two.first);
                assert_eq(one.first, two.first);
            }
        }

        printf("\n");
        return 0;
    }
}
