#include "benderrmq.hpp"

std::string
bitmap_str(uint_t i)
 {
    std::string out;
    for (int x = 17; x >= 0; --x) {
	out += '0' + ((i & (1L << x)) ? 1 : 0);
    }
    return out;
}

/* This is a destructive function - one which deletes the tree rooted
 * at node n
 */
void
euler_tour(BinaryTreeNode *n,
	   vui_t &output, /* Where the output is written. Should be empty */
	   vui_t &levels, /* Where the level for each node is written. Should be empty */
	   vui_t &mapping /* mapping stores representative
                             indexes which maps from the original index to the index
                             into the euler tour array, which is a +- RMQ */,
	   vui_t &rev_mapping /* Reverse mapping to go from +-RMQ
				 indexes to user provided indexes */,
	   int level) {
    DPRINTF("euler_tour(%d, %d)\n", n?n->data:-1, n?n->index:-1);
    if (!n) {
	return;
    }
    output.push_back(n->data);
    mapping[n->index] = output.size() - 1;
    DPRINTF("mapping[%d] = %d\n", n->index, mapping[n->index]);
    rev_mapping.push_back(n->index);
    levels.push_back(level);
    if (n->left) {
	euler_tour(n->left, output, levels, mapping, rev_mapping, level+1);
	output.push_back(n->data);
	rev_mapping.push_back(n->index);
	levels.push_back(level);
    }
    if (n->right) {
	euler_tour(n->right, output, levels, mapping, rev_mapping, level+1);
	output.push_back(n->data);
	rev_mapping.push_back(n->index);
	levels.push_back(level);
    }
    // We don't delete the node here since the clear() function on the
    // SimpleFixedObjectAllocator<BinaryTreeNode> will take care of
    // cleaning up the associated memory.
    //
    // delete n;
}

BinaryTreeNode*
make_cartesian_tree(vui_t const &input, SimpleFixedObjectAllocator<BinaryTreeNode> &alloc) {
    BinaryTreeNode *curr = NULL;
    std::stack<BinaryTreeNode*> stk;

    if (input.empty()) {
	return NULL;
    }

    for (uint_t i = 0; i < input.size(); ++i) {
	curr = alloc.get();
        new (curr) BinaryTreeNode(input[i], i);
	DPRINTF("ct(%d, %d)\n", curr->data, curr->index);

	if (stk.empty()) {
	    stk.push(curr);
	    DPRINTF("[1] stack top (%d, %d)\n", curr->data, curr->index);
	} else {
	    if (input[i] <= stk.top()->data) {
		// Just add it
		stk.push(curr);
		DPRINTF("[2] stack top (%d, %d)\n", curr->data, curr->index);
	    } else {
		// Back up till we are the largest node on the stack
		BinaryTreeNode *top = NULL;
		BinaryTreeNode *prev = NULL;
		while (!stk.empty() && stk.top()->data < input[i]) {
		    prev = top;
		    top = stk.top();
		    DPRINTF("[1] popping & setting (%d, %d)->right = (%d, %d)\n", top->data, top->index,
			    prev?prev->data:-1, prev?prev->index:-1);
		    top->right = prev;
		    stk.pop();
		}
		curr->left = top;
		DPRINTF("(%d, %d)->left = (%d, %d)\n", curr->data, curr->index, top->data, top->index);
		stk.push(curr);
		DPRINTF("stack top is now (%d, %d)\n", curr->data, curr->index);
	    }
	}
    }

    assert(!stk.empty());
    BinaryTreeNode *top = NULL;
    BinaryTreeNode *prev = NULL;
    while (!stk.empty()) {
	prev = top;
	top = stk.top();
	DPRINTF("[2] popping & setting (%d, %d)->right = (%d, %d)\n", top->data, top->index,
		prev?prev->data:-1, prev?prev->index:-1);
	top->right = prev;
	stk.pop();
    }
    DPRINTF("returning top = (%d, %d)\n", top->data, top->index);

    return top;
}

std::string
toGraphViz(BinaryTreeNode* par, BinaryTreeNode *n) {
    if (!n) {
	return "";
    }

    std::ostringstream sout;

    if (par) {
	sout<<'"'<<par->data<<"_"<<par->index<<"\" -> \""<<n->data<<"_"<<n->index<<"\"\n";
    }
    sout<<toGraphViz(n, n->left);
    sout<<toGraphViz(n, n->right);
    return sout.str();
}

void LookupTables::initialize(int nbits) {
    int ntables = 1 << nbits;
    repr.resize(ntables);
    std::vector<int> tmp(nbits);

    for (int i = 0; i < ntables; ++i) {
        const int start = 40;
        int diff = 0;
        for (int j = 0; j < nbits; ++j) {
            const int mask = 1L << j;
            if (i & mask) {
                diff += 1;
            } else {
                diff -= 1;
            }
            tmp[j] = start + diff;
        }

        // Always perform a lookup with the lower index
        // first. i.e. table[3][5] and NOT table[5][3]. Never
        // lookup with the same index on both dimenstion (for
        // example: table[3][3]).
        char_array_2d_t table(nbits-1, vc_t(nbits, -1));

        // Compute the lookup table for the bitmap in 'i'.
        for (int r = 0; r < nbits-1; ++r) {
            int maxi = r;
            int maxv = tmp[r];

            for (int c = r+1; c < nbits; ++c) {
                if (tmp[c] > maxv) {
                    maxv = tmp[c];
                    maxi = c;
                }
                table[r][c] = maxi;

            } // for(c)

        } // for (r)

        repr[i].swap(table);

    } // for (i)
}

uint_t LookupTables::query_max(uint_t index, uint_t l, uint_t u) {
    assert_le(l, u);
    assert_lt(index, (uint_t)repr.size());
    assert_lt(l, (uint_t)repr[0].size() + 1);
    assert_lt(u, (uint_t)repr[0][0].size());

    if (u == l) {
        return u;
    }

    return repr[index][l][u];
}


namespace benderrmq {

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
	printf("Testing BenderRMQ implementation\n");
	printf("--------------------------------\n");

	LookupTables lt;
	lt.initialize(4);
	// lt.show_tables();
	// return 0;

        vui_t v;
        v.push_back(45);
        v.push_back(4);
        v.push_back(5);
        v.push_back(2);
        v.push_back(99);
        v.push_back(41);
        v.push_back(45);
        v.push_back(47);
        v.push_back(51);
        v.push_back(89);
        v.push_back(1);
        v.push_back(3);
        v.push_back(5);
        v.push_back(98);
        v.push_back(289);
        v.push_back(14);
        v.push_back(95);
        v.push_back(88);

	BenderRMQ brmq;
        brmq.initialize(v);

        for (size_t i = 0; i < v.size(); ++i) {
            for (size_t j = i; j < v.size(); ++j) {
                pui_t one = brmq.query_max(i, j);
                pui_t two = naive_query_max(v, i, j);
                printf("query_max(%u, %u) == (%u, %u)\n", (uint_t)i, (uint_t)j, one.first, two.first);
                assert_eq(one.first, two.first);
            }
        }

	printf("\n");
        return 0;
    }
}

void BenderRMQ::initialize(const vui_t &elems) {
    len = elems.size();

    if (len < MIN_SIZE_FOR_BENDER_RMQ) {
        st.initialize(elems);
        return;
    }

    vui_t levels;
    SimpleFixedObjectAllocator<BinaryTreeNode> alloc(len);

    euler.reserve(elems.size() * 2);
    mapping.resize(elems.size());
    BinaryTreeNode *root = make_cartesian_tree(elems, alloc);

    DPRINTF("GraphViz (paste at: http://ashitani.jp/gv/):\n%s\n", toGraphViz(NULL, root).c_str());

    euler_tour(root, euler, levels, mapping, rev_mapping);

    root = NULL; // This tree has now been deleted
    alloc.clear();

    assert_eq(levels.size(), euler.size());
    assert_eq(levels.size(), rev_mapping.size());

    uint_t n = euler.size();
    lgn_by_2 = log2(n) / 2;
    _2n_lgn  = n / lgn_by_2 + 1;

    DPRINTF("n = %u, lgn/2 = %d, 2n/lgn = %d\n", n, lgn_by_2, _2n_lgn);
    lt.initialize(lgn_by_2);

    table_map.resize(_2n_lgn);
    vui_t reduced;

    for (uint_t i = 0; i < n; i += lgn_by_2) {
        uint_t max_in_block = euler[i];
        int bitmap = 1L;
        DPRINTF("Sequence: (%u, ", euler[i]);
        for (int j = 1; j < lgn_by_2; ++j) {
            int curr_level, prev_level;
            uint_t value;
            if (i+j < n) {
                curr_level = levels[i+j];
                prev_level = levels[i+j-1];
                value = euler[i+j];
            } else {
                curr_level = 1;
                prev_level = 0;
                value = 0;
            }

            const uint_t bit = (curr_level < prev_level);
            bitmap |= (bit << j);
            max_in_block = std::max(max_in_block, value);
            DPRINTF("%u, ", value);
        }
        DPRINTF("), Bitmap: %s\n", bitmap_str(bitmap).c_str());
        table_map[i / lgn_by_2] = bitmap;
        reduced.push_back(max_in_block);
    }

    DPRINTF("reduced.size(): %u\n", reduced.size());
    st.initialize(reduced);
    DCERR("initialize() completed"<<endl);
}
