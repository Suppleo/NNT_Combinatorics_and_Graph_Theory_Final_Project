from collections import defaultdict
from enum import Enum

class Edge:
    def __init__(self, u, v, eid):
        self.u = u
        self.v = v
        self.id = eid
    def __repr__(self):
        return f"({self.u},{self.v},id={self.id})"

class AdjacencyList:
    def __init__(self, n):
        self.n = n
        self.adj = [[] for _ in range(n)]

class AdjacencyMatrix:
    def __init__(self, n):
        self.n = n
        self.matrix = [[0] * n for _ in range(n)]

class ExtendedAdjacencyList:
    def __init__(self, n):
        self.n = n
        self.edges = []  # list of Edge
        self.incoming = [[] for _ in range(n)]
        self.outgoing = [[] for _ in range(n)]
        self.m = 0

class AdjacencyMap:
    def __init__(self, n):
        self.n = n
        self.outgoing = {i: [] for i in range(n)}
        self.incoming = {i: [] for i in range(n)}
        self.m = 0

class Representation(Enum):
    ADJ_LIST = "Adjacency List"
    ADJ_MATRIX = "Adjacency Matrix"
    EXT_ADJ_LIST = "Extended Adjacency List"
    ADJ_MAP = "Adjacency Map"

current_list = None
current_matrix = None
current_extended = None
current_map = None
current_rep = Representation.ADJ_LIST

# 1. AL <-> AM
def al_to_am(al):
    am = AdjacencyMatrix(al.n)
    for u in range(al.n):
        for v in al.adj[u]:
            am.matrix[u][v] += 1
    return am

def am_to_al(am):
    al = AdjacencyList(am.n)
    for u in range(am.n):
        for v in range(am.n):
            for _ in range(am.matrix[u][v]):
                al.adj[u].append(v)
    return al

# 2. AL <-> EAL
def al_to_eal(al):
    eal = ExtendedAdjacencyList(al.n)
    eid = 0
    for u in range(al.n):
        for v in al.adj[u]:
            if u <= v:
                eal.edges.append(Edge(u, v, eid))
                eid += 1
    eal.m = len(eal.edges)
    for i, e in enumerate(eal.edges):
        eal.outgoing[e.u].append(i)
        eal.incoming[e.v].append(i)
        if e.u != e.v:
            eal.outgoing[e.v].append(i)
            eal.incoming[e.u].append(i)
    return eal

def eal_to_al(eal):
    al = AdjacencyList(eal.n)
    for e in eal.edges:
        al.adj[e.u].append(e.v)
        if e.u != e.v:
            al.adj[e.v].append(e.u)
    return al

# 3. AL <-> AMap
def al_to_amap(al):
    amap = AdjacencyMap(al.n)
    eid = 0
    for u in range(al.n):
        for v in al.adj[u]:
            if u <= v:
                edge = Edge(u, v, eid)
                amap.outgoing[u].append(edge)
                amap.incoming[v].append(edge)
                if u != v:
                    amap.outgoing[v].append(edge)
                    amap.incoming[u].append(edge)
                eid += 1
    amap.m = eid
    return amap

def amap_to_al(amap):
    al = AdjacencyList(amap.n)
    used = set()
    for u in range(amap.n):
        for e in amap.outgoing[u]:
            if (e.id, u) not in used:
                al.adj[u].append(e.v)
                used.add((e.id, u))
            if u != e.v and (e.id, e.v) not in used:
                al.adj[e.v].append(u)
                used.add((e.id, e.v))
    return al

# 4. AM <-> EAL
def am_to_eal(am):
    eal = ExtendedAdjacencyList(am.n)
    eid = 0
    for u in range(am.n):
        for v in range(u, am.n):
            for _ in range(am.matrix[u][v]):
                eal.edges.append(Edge(u, v, eid))
                eid += 1
    eal.m = len(eal.edges)
    for i, e in enumerate(eal.edges):
        eal.outgoing[e.u].append(i)
        eal.incoming[e.v].append(i)
        if e.u != e.v:
            eal.outgoing[e.v].append(i)
            eal.incoming[e.u].append(i)
    return eal

def eal_to_am(eal):
    am = AdjacencyMatrix(eal.n)
    for e in eal.edges:
        am.matrix[e.u][e.v] += 1
        if e.u != e.v:
            am.matrix[e.v][e.u] += 1
    return am

# 5. AM <-> AMap
def am_to_amap(am):
    amap = AdjacencyMap(am.n)
    eid = 0
    for u in range(am.n):
        for v in range(u, am.n):
            for _ in range(am.matrix[u][v]):
                edge = Edge(u, v, eid)
                amap.outgoing[u].append(edge)
                amap.incoming[v].append(edge)
                if u != v:
                    amap.outgoing[v].append(edge)
                    amap.incoming[u].append(edge)
                eid += 1
    amap.m = eid
    return amap

def amap_to_am(amap):
    am = AdjacencyMatrix(amap.n)
    used = set()
    for u in range(amap.n):
        for e in amap.outgoing[u]:
            key = (e.id, min(e.u, e.v), max(e.u, e.v))
            if key not in used:
                am.matrix[e.u][e.v] += 1
                if e.u != e.v:
                    am.matrix[e.v][e.u] += 1
                used.add(key)
    return am

# 6. EAL <-> AMap
def eal_to_amap(eal):
    amap = AdjacencyMap(eal.n)
    for e in eal.edges:
        amap.outgoing[e.u].append(e)
        amap.incoming[e.v].append(e)
        if e.u != e.v:
            amap.outgoing[e.v].append(e)
            amap.incoming[e.u].append(e)
    amap.m = len(eal.edges)
    return amap

def amap_to_eal(amap):
    eal = ExtendedAdjacencyList(amap.n)
    used = set()
    for u in range(amap.n):
        for e in amap.outgoing[u]:
            key = (e.id, min(e.u, e.v), max(e.u, e.v))
            if key not in used:
                eal.edges.append(e)
                used.add(key)
    eal.m = len(eal.edges)
    for i, e in enumerate(eal.edges):
        eal.outgoing[e.u].append(i)
        eal.incoming[e.v].append(i)
        if e.u != e.v:
            eal.outgoing[e.v].append(i)
            eal.incoming[e.u].append(i)
    return eal

# Display functions
def display_matrix(matrix):
    print("Adjacency Matrix (số cạnh):")
    print("  ", end="")
    for i in range(matrix.n):
        print(f"{i} ", end="")
    print()
    for i in range(matrix.n):
        print(f"{i} ", end="")
        for j in range(matrix.n):
            print(matrix.matrix[i][j], end=" ")
        print()

def display_list(adj_list):
    print("Adjacency List (đa đồ thị):")
    for i in range(adj_list.n):
        print(f"{i}: {' '.join(map(str, adj_list.adj[i]))}")

def display_extended(ext):
    print("Extended Adjacency List (đa đồ thị):")
    print("Edges:", end=" ")
    for e in ext.edges:
        print(f"({e.u},{e.v},id={e.id})", end=" ")
    print()
    print("Outgoing edges:")
    for i in range(ext.n):
        print(f"{i}: {' '.join(map(str, ext.outgoing[i]))}")
    print("Incoming edges:")
    for i in range(ext.n):
        print(f"{i}: {' '.join(map(str, ext.incoming[i]))}")

def display_map(amap):
    print("Adjacency Map:")
    print("Outgoing mappings:")
    for i in range(amap.n):
        print(f"{i}: ", end="")
        for e in amap.outgoing[i]:
            print(f"({e.u}->{e.v},{e.id})", end=" ")
        print()
    print("Incoming mappings:")
    for i in range(amap.n):
        print(f"{i}: ", end="")
        for e in amap.incoming[i]:
            print(f"({e.u}->{e.v},{e.id})", end=" ")
        print()

def display_current_representation():
    print(f"\n=== DO THI HIEN TAI === ({current_rep.value})")
    if current_rep == Representation.ADJ_LIST:
        display_list(current_list)
    elif current_rep == Representation.ADJ_MATRIX:
        display_matrix(current_matrix)
    elif current_rep == Representation.EXT_ADJ_LIST:
        display_extended(current_extended)
    elif current_rep == Representation.ADJ_MAP:
        display_map(current_map)
    print()

def is_valid_conversion(choice):
    if choice == 1:
        return True
    elif choice in [2, 3, 4]:
        return current_rep == Representation.ADJ_LIST
    elif choice in [5, 6, 7]:
        return current_rep == Representation.ADJ_MATRIX
    elif choice in [8, 9, 10]:
        return current_rep == Representation.EXT_ADJ_LIST
    elif choice in [11, 12, 13]:
        return current_rep == Representation.ADJ_MAP
    elif choice == 14:
        return True
    else:
        return False

def main():
    global current_list, current_matrix, current_extended, current_map, current_rep
    print("=== CHUONG TRINH CHUYEN DOI BIEU DIEN DA DO THI (MULTIGRAPH) ===\n")
    n, m = map(int, input("Nhap so dinh va so canh: ").split())
    current_list = AdjacencyList(n)
    print(f"Nhap {m} canh (dinh dau dinh cuoi, khong cho phep khuyen):")
    for _ in range(m):
        u, v = map(int, input().split())
        if u == v:
            print("Khong cho phep khuyen! Bo qua canh nay.")
            continue
        current_list.adj[u].append(v)
        current_list.adj[v].append(u)
    display_current_representation()
    while True:
        print("\n=== MENU CHUYEN DOI ===")
        print("1. Hien thi do thi hien tai")
        print("=== ADJACENCY LIST CONVERSIONS ===")
        print("2. AL -> Adjacency Matrix")
        print("3. AL -> Extended Adjacency List")
        print("4. AL -> Adjacency Map")
        print("\n=== ADJACENCY MATRIX CONVERSIONS ===")
        print("5. AM -> Adjacency List")
        print("6. AM -> Extended Adjacency List")
        print("7. AM -> Adjacency Map")
        print("\n=== EXTENDED ADJACENCY LIST CONVERSIONS ===")
        print("8. EAL -> Adjacency List")
        print("9. EAL -> Adjacency Matrix")
        print("10. EAL -> Adjacency Map")
        print("\n=== ADJACENCY MAP CONVERSIONS ===")
        print("11. AMap -> Adjacency List")
        print("12. AMap -> Adjacency Matrix")
        print("13. AMap -> Extended Adjacency List")
        print("\n14. Thoat")
        choice = input("Chon: ")
        try:
            choice = int(choice)
        except ValueError:
            print("Lua chon khong hop le! Vui long nhap so.")
            continue
        if not is_valid_conversion(choice):
            print(f"\nLOI: Khong the chuyen doi! Do thi hien tai dang o dang {current_rep.value} nhung ban chon chuyen doi tu dang khac.")
            print("Vui long chon lai!")
            continue
        if choice == 1:
            display_current_representation()
        elif choice == 2:
            current_matrix = al_to_am(current_list)
            current_rep = Representation.ADJ_MATRIX
            print("\nDa chuyen doi thanh cong: Adjacency List -> Adjacency Matrix")
            display_current_representation()
        elif choice == 3:
            current_extended = al_to_eal(current_list)
            current_rep = Representation.EXT_ADJ_LIST
            print("\nDa chuyen doi thanh cong: Adjacency List -> Extended Adjacency List")
            display_current_representation()
        elif choice == 4:
            current_map = al_to_amap(current_list)
            current_rep = Representation.ADJ_MAP
            print("\nDa chuyen doi thanh cong: Adjacency List -> Adjacency Map")
            display_current_representation()
        elif choice == 5:
            current_list = am_to_al(current_matrix)
            current_rep = Representation.ADJ_LIST
            print("\nDa chuyen doi thanh cong: Adjacency Matrix -> Adjacency List")
            display_current_representation()
        elif choice == 6:
            current_extended = am_to_eal(current_matrix)
            current_rep = Representation.EXT_ADJ_LIST
            print("\nDa chuyen doi thanh cong: Adjacency Matrix -> Extended Adjacency List")
            display_current_representation()
        elif choice == 7:
            current_map = am_to_amap(current_matrix)
            current_rep = Representation.ADJ_MAP
            print("\nDa chuyen doi thanh cong: Adjacency Matrix -> Adjacency Map")
            display_current_representation()
        elif choice == 8:
            current_list = eal_to_al(current_extended)
            current_rep = Representation.ADJ_LIST
            print("\nDa chuyen doi thanh cong: Extended Adjacency List -> Adjacency List")
            display_current_representation()
        elif choice == 9:
            current_matrix = eal_to_am(current_extended)
            current_rep = Representation.ADJ_MATRIX
            print("\nDa chuyen doi thanh cong: Extended Adjacency List -> Adjacency Matrix")
            display_current_representation()
        elif choice == 10:
            current_map = eal_to_amap(current_extended)
            current_rep = Representation.ADJ_MAP
            print("\nDa chuyen doi thanh cong: Extended Adjacency List -> Adjacency Map")
            display_current_representation()
        elif choice == 11:
            current_list = amap_to_al(current_map)
            current_rep = Representation.ADJ_LIST
            print("\nDa chuyen doi thanh cong: Adjacency Map -> Adjacency List")
            display_current_representation()
        elif choice == 12:
            current_matrix = amap_to_am(current_map)
            current_rep = Representation.ADJ_MATRIX
            print("\nDa chuyen doi thanh cong: Adjacency Map -> Adjacency Matrix")
            display_current_representation()
        elif choice == 13:
            current_extended = amap_to_eal(current_map)
            current_rep = Representation.EXT_ADJ_LIST
            print("\nDa chuyen doi thanh cong: Adjacency Map -> Extended Adjacency List")
            display_current_representation()
        elif choice == 14:
            print("Tam biet!")
            break
        else:
            print("Lua chon khong hop le!")

if __name__ == "__main__":
    main()
