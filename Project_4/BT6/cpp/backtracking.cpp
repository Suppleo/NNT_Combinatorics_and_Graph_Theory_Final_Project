// Tree Edit Distance by Backtracking
// Author: Nguyễn Ngọc Thạch
// Implements the backtracking algorithm for tree edit distance as described in the reading
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <memory>

using namespace std;

struct TreeNode {
    int label;
    vector<TreeNode*> children;
    TreeNode* parent = nullptr;
    int depth = 0;
    int preorder = 0;
    int order = 0; // sibling order
    TreeNode(int l) : label(l) {}
};

struct Tree {
    vector<unique_ptr<TreeNode>> nodes;
    TreeNode* root = nullptr;
    TreeNode* dummy = nullptr; // dummy node for deletions
    int depth = 0;

    TreeNode* add_node(int label) {
        nodes.push_back(make_unique<TreeNode>(label));
        return nodes.back().get();
    }
    void set_root(TreeNode* node) {
        root = node;
    }
    void add_edge(TreeNode* parent, TreeNode* child) {
        parent->children.push_back(child);
        child->parent = parent;
    }
    void assign_preorder_and_depth() {
        int order_counter = 0;
        function<void(TreeNode*, int, int)> dfs = [&](TreeNode* node, int depth, int sibling_order) {
            node->depth = depth;
            node->preorder = order_counter++;
            node->order = sibling_order;
            for (int i = 0; i < (int)node->children.size(); ++i) {
                dfs(node->children[i], depth+1, i);
            }
        };
        dfs(root, 0, 0);
        int max_depth = 0;
        for (auto& n : nodes) max_depth = max(max_depth, n->depth);
        depth = max_depth;
    }
    void add_dummy() {
        dummy = add_node(-1); // -1 for dummy
        dummy->depth = -1;
    }
};

// Set up candidate nodes for mapping
// C[v] = list of nodes in T2 at same depth as v, plus dummy
map<TreeNode*, vector<TreeNode*>> set_up_candidate_nodes(Tree* T1, Tree* T2) {
    map<TreeNode*, vector<TreeNode*>> C;
    for (auto& v_ptr : T1->nodes) {
        TreeNode* v = v_ptr.get();
        if (v == T1->dummy) continue;
        C[v].push_back(T2->dummy);
        for (auto& w_ptr : T2->nodes) {
            TreeNode* w = w_ptr.get();
            if (w == T2->dummy) continue;
            if (v->depth == w->depth) C[v].push_back(w);
        }
    }
    return C;
}

// Refine candidate nodes after mapping v -> w
void refine_candidate_nodes(Tree* T1, Tree* T2, map<TreeNode*, vector<TreeNode*>>& C, TreeNode* v, TreeNode* w) {
    // Remove w from all candidates if w is not dummy (bijection constraint)
    if (w != T2->dummy) {
        for (auto& kv : C) {
            auto& vec = kv.second;
            vec.erase(remove(vec.begin(), vec.end(), w), vec.end());
        }
    }
    // For each child x of v, remove y from C[x] if y is not dummy and parent[y] != w
    for (auto x : v->children) {
        auto& vec = C[x];
        vec.erase(remove_if(vec.begin(), vec.end(), [&](TreeNode* y) {
            return y != T2->dummy && y->parent != w;
        }), vec.end());
    }
    // Sibling order constraint
    if (v->parent && w != T2->dummy) {
        auto& siblings_v = v->parent->children;
        for (auto x : siblings_v) {
            if (v->order < x->order) {
                auto& vec = C[x];
                vec.erase(remove_if(vec.begin(), vec.end(), [&](TreeNode* y) {
                    return y != T2->dummy && w->order > y->order;
                }), vec.end());
            }
        }
    }
}

// Recursive extension of mapping
void extend_tree_edit(Tree* T1, Tree* T2, map<TreeNode*, TreeNode*>& M, vector<map<TreeNode*, TreeNode*>>& L, map<TreeNode*, vector<TreeNode*>>& C, TreeNode* v, const vector<TreeNode*>& preorder_list_T1) {
    for (auto w : C[v]) {
        M[v] = w;
        if (v == preorder_list_T1.back()) {
            L.push_back(M);
        } else {
            auto N = C;
            refine_candidate_nodes(T1, T2, N, v, w);
            auto it = find(preorder_list_T1.begin(), preorder_list_T1.end(), v);
            int idx = distance(preorder_list_T1.begin(), it);
            TreeNode* next_v = preorder_list_T1[idx+1];
            extend_tree_edit(T1, T2, M, L, N, next_v, preorder_list_T1);
        }
    }
}

vector<map<TreeNode*, TreeNode*>> backtracking_tree_edit(Tree* T1, Tree* T2) {
    T1->assign_preorder_and_depth();
    T2->assign_preorder_and_depth();
    T2->add_dummy();
    T1->add_dummy(); // For uniformity, but not used in mapping
    vector<TreeNode*> preorder_list_T1;
    for (auto& n : T1->nodes) if (n.get() != T1->dummy) preorder_list_T1.push_back(n.get());
    auto C = set_up_candidate_nodes(T1, T2);
    map<TreeNode*, TreeNode*> M;
    vector<map<TreeNode*, TreeNode*>> L;
    TreeNode* v = preorder_list_T1[0];
    extend_tree_edit(T1, T2, M, L, C, v, preorder_list_T1);
    return L;
}

// Example usage: build two trees and run the algorithm
int main() {
    // Example: T1: 1(root)-2,3; T2: 4(root)-5
    Tree T1, T2;
    TreeNode* n1 = T1.add_node(1);
    TreeNode* n2 = T1.add_node(2);
    TreeNode* n3 = T1.add_node(3);
    T1.set_root(n1);
    T1.add_edge(n1, n2);
    T1.add_edge(n1, n3);

    TreeNode* m1 = T2.add_node(4);
    TreeNode* m2 = T2.add_node(5);
    T2.set_root(m1);
    T2.add_edge(m1, m2);

    auto results = backtracking_tree_edit(&T1, &T2);
    cout << "Number of valid mappings: " << results.size() << endl;
    int i = 1;
    for (auto& mapping : results) {
        cout << "Mapping " << i++ << ":\n";
        for (auto& kv : mapping) {
            cout << "  T1 node " << kv.first->label << " -> T2 node ";
            if (kv.second == T2.dummy) cout << "λ";
            else cout << kv.second->label;
            cout << endl;
        }
    }
    return 0;
}
