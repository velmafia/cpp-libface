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
