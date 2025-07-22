# Tree Edit Distance by Backtracking
# Author: Nguyễn Ngọc Thạch
# Implements the backtracking algorithm for tree edit distance as described in the reading

from collections import defaultdict, deque
import copy

class TreeNode:
    def __init__(self, label):
        self.label = label
        self.children = []
        self.parent = None
        self.depth = 0
        self.preorder = 0
        self.order = 0  # sibling order

class Tree:
    def __init__(self):
        self.nodes = []  # list of TreeNode
        self.root = None
        self.dummy = None  # dummy node for deletions
        self.depth = 0

    def add_node(self, label):
        node = TreeNode(label)
        self.nodes.append(node)
        return node

    def set_root(self, node):
        self.root = node

    def add_edge(self, parent, child):
        parent.children.append(child)
        child.parent = parent

    def assign_preorder_and_depth(self):
        # Assign preorder number, depth, and sibling order
        order_counter = [0]
        def dfs(node, depth, sibling_order):
            node.depth = depth
            node.preorder = order_counter[0]
            node.order = sibling_order
            order_counter[0] += 1
            for i, child in enumerate(node.children):
                dfs(child, depth+1, i)
        dfs(self.root, 0, 0)
        # Set tree depth
        self.depth = max(n.depth for n in self.nodes)

    def add_dummy(self):
        # Add a dummy node for deletions
        self.dummy = TreeNode('λ')
        self.dummy.depth = -1
        self.nodes.append(self.dummy)

# Set up candidate nodes for mapping
# C[v] = list of nodes in T2 at same depth as v, plus dummy

def set_up_candidate_nodes(T1, T2):
    C = dict()
    for v in T1.nodes:
        if v == T1.dummy:
            continue
        C[v] = [T2.dummy]
        for w in T2.nodes:
            if w == T2.dummy:
                continue
            if v.depth == w.depth:
                C[v].append(w)
    return C

# Refine candidate nodes after mapping v -> w

def refine_candidate_nodes(T1, T2, C, v, w):
    # Remove w from all candidates if w is not dummy (bijection constraint)
    if w != T2.dummy:
        for x in C:
            C[x] = [y for y in C[x] if y != w]
    # For each child x of v, remove y from C[x] if y is not dummy and parent[y] != w
    for x in v.children:
        C[x] = [y for y in C[x] if y == T2.dummy or (y.parent == w)]
    # Sibling order constraint
    if v.parent and w != T2.dummy:
        siblings_v = v.parent.children
        for x in siblings_v:
            if v.order < x.order:
                C[x] = [y for y in C[x] if y == T2.dummy or (w.order <= y.order)]
    return C

# Recursive extension of mapping

def extend_tree_edit(T1, T2, M, L, C, v, preorder_list_T1):
    for w in C[v]:
        M[v] = w
        if v == preorder_list_T1[-1]:
            L.append(copy.deepcopy(M))
        else:
            N = copy.deepcopy(C)
            N = refine_candidate_nodes(T1, T2, N, v, w)
            idx = preorder_list_T1.index(v)
            next_v = preorder_list_T1[idx+1]
            extend_tree_edit(T1, T2, M, L, N, next_v, preorder_list_T1)
    return

def backtracking_tree_edit(T1, T2):
    T1.assign_preorder_and_depth()
    T2.assign_preorder_and_depth()
    T2.add_dummy()
    T1.add_dummy()  # For uniformity, but not used in mapping
    preorder_list_T1 = [n for n in T1.nodes if n != T1.dummy]
    C = set_up_candidate_nodes(T1, T2)
    M = dict()
    L = []
    v = preorder_list_T1[0]
    extend_tree_edit(T1, T2, M, L, C, v, preorder_list_T1)
    return L

# Example usage: build two trees and run the algorithm
if __name__ == "__main__":
    # Example: T1: 1(root)-2,3; T2: 4(root)-5
    T1 = Tree()
    n1 = T1.add_node(1)
    n2 = T1.add_node(2)
    n3 = T1.add_node(3)
    T1.set_root(n1)
    T1.add_edge(n1, n2)
    T1.add_edge(n1, n3)

    T2 = Tree()
    m1 = T2.add_node(4)
    m2 = T2.add_node(5)
    T2.set_root(m1)
    T2.add_edge(m1, m2)

    results = backtracking_tree_edit(T1, T2)
    print(f"Number of valid mappings: {len(results)}")
    for i, mapping in enumerate(results):
        print(f"Mapping {i+1}:")
        for v, w in mapping.items():
            print(f"  T1 node {v.label} -> T2 node {w.label if w != T2.dummy else 'λ'}")
