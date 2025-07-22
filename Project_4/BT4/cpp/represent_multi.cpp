#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <set>
using namespace std;

// Multigraph representations
struct AdjacencyMatrix {
    vector<vector<int>> matrix; // matrix[i][j] = number of edges between i and j
    int n;
};

struct AdjacencyList {
    vector<vector<int>> adj; // adj[i] = list of neighbors (may repeat)
    int n;
};

struct ExtendedAdjacencyList {
    vector<pair<int, int>> edges; // each edge is (u,v), may repeat
    vector<vector<int>> incoming; // incoming[i] = indices of edges into i
    vector<vector<int>> outgoing; // outgoing[i] = indices of edges from i
    int n, m;
};

struct AdjacencyMap {
    unordered_map<int, vector<pair<int, int>>> incoming; // incoming[i] = list of (u,v) edges into i
    unordered_map<int, vector<pair<int, int>>> outgoing; // outgoing[i] = list of (u,v) edges from i
    int n, m;
};

enum Representation {
    ADJ_LIST,
    ADJ_MATRIX,
    EXT_ADJ_LIST,
    ADJ_MAP
};

AdjacencyList current_list;
AdjacencyMatrix current_matrix;
ExtendedAdjacencyList current_extended;
AdjacencyMap current_map;
Representation current_rep = ADJ_LIST;

// 1. Adjacency List <-> Adjacency Matrix
AdjacencyMatrix listToMatrix(const AdjacencyList& list) {
    AdjacencyMatrix matrix;
    matrix.n = list.n;
    matrix.matrix.assign(matrix.n, vector<int>(matrix.n, 0));
    for (int i = 0; i < list.n; i++) {
        for (int j : list.adj[i]) {
            matrix.matrix[i][j]++;
        }
    }
    return matrix;
}
AdjacencyList matrixToList(const AdjacencyMatrix& matrix) {
    AdjacencyList list;
    list.n = matrix.n;
    list.adj.assign(list.n, vector<int>());
    for (int i = 0; i < matrix.n; i++) {
        for (int j = 0; j < matrix.n; j++) {
            for (int k = 0; k < matrix.matrix[i][j]; k++) {
                list.adj[i].push_back(j);
            }
        }
    }
    return list;
}
// 2. Adjacency List <-> Extended Adjacency List
ExtendedAdjacencyList listToExtended(const AdjacencyList& list) {
    ExtendedAdjacencyList ext;
    ext.n = list.n;
    vector<vector<int>> count(list.n, vector<int>(list.n, 0));
    // To avoid double counting, only add edge (i,j) when i <= j, but for each occurrence
    for (int i = 0; i < list.n; i++) {
        for (int j : list.adj[i]) {
            if (i < j && count[i][j] < count[j][i]) continue; // only add from one side
            if (i > j && count[j][i] < count[i][j]) continue;
            if (i == j) continue; // no loops
            ext.edges.push_back({i, j});
            count[i][j]++;
        }
    }
    ext.m = ext.edges.size();
    ext.incoming.assign(ext.n, vector<int>());
    ext.outgoing.assign(ext.n, vector<int>());
    for (int idx = 0; idx < ext.edges.size(); idx++) {
        int u = ext.edges[idx].first, v = ext.edges[idx].second;
        ext.outgoing[u].push_back(idx);
        ext.incoming[v].push_back(idx);
        if (u != v) {
            ext.outgoing[v].push_back(idx);
            ext.incoming[u].push_back(idx);
        }
    }
    return ext;
}
AdjacencyList extendedToList(const ExtendedAdjacencyList& ext) {
    AdjacencyList list;
    list.n = ext.n;
    list.adj.assign(list.n, vector<int>());
    for (auto& e : ext.edges) {
        int u = e.first, v = e.second;
        if (u != v) {
            list.adj[u].push_back(v);
            list.adj[v].push_back(u);
        }
    }
    return list;
}
// 3. Adjacency List <-> Adjacency Map
AdjacencyMap listToMap(const AdjacencyList& list) {
    AdjacencyMap map;
    map.n = list.n;
    map.m = 0;
    vector<vector<int>> count(list.n, vector<int>(list.n, 0));
    for (int i = 0; i < list.n; i++) {
        for (int j : list.adj[i]) {
            if (i < j && count[i][j] < count[j][i]) continue;
            if (i > j && count[j][i] < count[i][j]) continue;
            if (i == j) continue;
            map.outgoing[i].push_back({i, j});
            map.incoming[j].push_back({i, j});
            if (i != j) {
                map.outgoing[j].push_back({i, j});
                map.incoming[i].push_back({i, j});
            }
            count[i][j]++;
            map.m++;
        }
    }
    return map;
}
AdjacencyList mapToList(const AdjacencyMap& map) {
    AdjacencyList list;
    list.n = map.n;
    list.adj.assign(list.n, vector<int>());
    for (auto& p : map.outgoing) {
        int u = p.first;
        for (auto& e : p.second) {
            int v = e.second;
            if (u != v) list.adj[u].push_back(v);
        }
    }
    return list;
}
// 4. Adjacency Matrix <-> Extended Adjacency List
ExtendedAdjacencyList matrixToExtended(const AdjacencyMatrix& matrix) {
    ExtendedAdjacencyList ext;
    ext.n = matrix.n;
    for (int i = 0; i < matrix.n; i++) {
        for (int j = i+1; j < matrix.n; j++) {
            for (int k = 0; k < matrix.matrix[i][j]; k++) {
                ext.edges.push_back({i, j});
            }
        }
    }
    ext.m = ext.edges.size();
    ext.incoming.assign(ext.n, vector<int>());
    ext.outgoing.assign(ext.n, vector<int>());
    for (int idx = 0; idx < ext.edges.size(); idx++) {
        int u = ext.edges[idx].first, v = ext.edges[idx].second;
        ext.outgoing[u].push_back(idx);
        ext.incoming[v].push_back(idx);
        ext.outgoing[v].push_back(idx);
        ext.incoming[u].push_back(idx);
    }
    return ext;
}
AdjacencyMatrix extendedToMatrix(const ExtendedAdjacencyList& ext) {
    AdjacencyMatrix matrix;
    matrix.n = ext.n;
    matrix.matrix.assign(matrix.n, vector<int>(matrix.n, 0));
    for (auto& e : ext.edges) {
        int u = e.first, v = e.second;
        if (u != v) {
            matrix.matrix[u][v]++;
            matrix.matrix[v][u]++;
        }
    }
    return matrix;
}
// 5. Adjacency Matrix <-> Adjacency Map
AdjacencyMap matrixToMap(const AdjacencyMatrix& matrix) {
    AdjacencyMap map;
    map.n = matrix.n;
    map.m = 0;
    for (int i = 0; i < matrix.n; i++) {
        for (int j = i+1; j < matrix.n; j++) {
            for (int k = 0; k < matrix.matrix[i][j]; k++) {
                map.outgoing[i].push_back({i, j});
                map.incoming[j].push_back({i, j});
                map.outgoing[j].push_back({i, j});
                map.incoming[i].push_back({i, j});
                map.m++;
            }
        }

    }
    return map;
}
AdjacencyMatrix mapToMatrix(const AdjacencyMap& map) {
    AdjacencyMatrix matrix;
    matrix.n = map.n;
    matrix.matrix.assign(matrix.n, vector<int>(matrix.n, 0));
    for (auto& p : map.outgoing) {
        int u = p.first;
        for (auto& e : p.second) {
            int v = e.second;
            if (u != v) {
                matrix.matrix[u][v]++;
                matrix.matrix[v][u]++;
            }
        }
    }
    return matrix;
}
// 6. Extended Adjacency List <-> Adjacency Map
AdjacencyMap extendedToMap(const ExtendedAdjacencyList& ext) {
    AdjacencyMap map;
    map.n = ext.n;
    map.m = ext.m;
    for (int idx = 0; idx < ext.edges.size(); idx++) {
        int u = ext.edges[idx].first, v = ext.edges[idx].second;
        map.outgoing[u].push_back({u, v});
        map.incoming[v].push_back({u, v});
        if (u != v) {
            map.outgoing[v].push_back({u, v});
            map.incoming[u].push_back({u, v});
        }
    }
    return map;
}
ExtendedAdjacencyList mapToExtended(const AdjacencyMap& map) {
    ExtendedAdjacencyList ext;
    ext.n = map.n;
    for (auto& p : map.outgoing) {
        int u = p.first;
        for (auto& e : p.second) {
            int v = e.second;
            if (u < v) ext.edges.push_back({u, v});
        }
    }
    ext.m = ext.edges.size();
    ext.incoming.assign(ext.n, vector<int>());
    ext.outgoing.assign(ext.n, vector<int>());
    for (int idx = 0; idx < ext.edges.size(); idx++) {
        int u = ext.edges[idx].first, v = ext.edges[idx].second;
        ext.outgoing[u].push_back(idx);
        ext.incoming[v].push_back(idx);
        ext.outgoing[v].push_back(idx);
        ext.incoming[u].push_back(idx);
    }
    return ext;
}
// Display functions
void displayMatrix(const AdjacencyMatrix& matrix) {
    cout << "Adjacency Matrix (so canh):\n  ";
    for (int i = 0; i < matrix.n; i++) cout << i << " ";
    cout << "\n";
    for (int i = 0; i < matrix.n; i++) {
        cout << i << " ";
        for (int j = 0; j < matrix.n; j++) cout << matrix.matrix[i][j] << " ";
        cout << "\n";
    }
}
void displayList(const AdjacencyList& list) {
    cout << "Adjacency List (da do thi):\n";
    for (int i = 0; i < list.n; i++) {
        cout << i << ": ";
        for (int j : list.adj[i]) cout << j << " ";
        cout << "\n";
    }
}
void displayExtended(const ExtendedAdjacencyList& ext) {
    cout << "Extended Adjacency List (da do thi):\nEdges: ";
    for (int i = 0; i < ext.edges.size(); i++) cout << "(" << ext.edges[i].first << "," << ext.edges[i].second << ") ";
    cout << "\nOutgoing edges:\n";
    for (int i = 0; i < ext.n; i++) {
        cout << i << ": ";
        for (int idx : ext.outgoing[i]) cout << idx << " ";
        cout << "\n";
    }
    cout << "Incoming edges:\n";
    for (int i = 0; i < ext.n; i++) {
        cout << i << ": ";
        for (int idx : ext.incoming[i]) cout << idx << " ";
        cout << "\n";
    }
}
void displayMap(const AdjacencyMap& map) {
    cout << "Adjacency Map (da do thi):\nOutgoing:\n";
    for (int i = 0; i < map.n; i++) {
        cout << i << ": ";
        auto it = map.outgoing.find(i);
        if (it != map.outgoing.end()) {
            for (auto& e : it->second) cout << "(" << e.first << "," << e.second << ") ";
        }
        cout << "\n";
    }
    cout << "Incoming:\n";
    for (int i = 0; i < map.n; i++) {
        cout << i << ": ";
        auto it = map.incoming.find(i);
        if (it != map.incoming.end()) {
            for (auto& e : it->second) cout << "(" << e.first << "," << e.second << ") ";
        }
        cout << "\n";
    }
}
void displayCurrentRepresentation() {
    cout << "\n=== DO THI HIEN TAI ===";
    switch (current_rep) {
        case ADJ_LIST:
            cout << " (Adjacency List)\n";
            displayList(current_list);
            break;
        case ADJ_MATRIX:
            cout << " (Adjacency Matrix)\n";
            displayMatrix(current_matrix);
            break;
        case EXT_ADJ_LIST:
            cout << " (Extended Adjacency List)\n";
            displayExtended(current_extended);
            break;
        case ADJ_MAP:
            cout << " (Adjacency Map)\n";
            displayMap(current_map);
            break;
    }
    cout << "\n";
}
bool isValidConversion(int choice) {
    switch (choice) {
        case 1: return true;
        case 2: case 3: case 4: return current_rep == ADJ_LIST;
        case 5: case 6: case 7: return current_rep == ADJ_MATRIX;
        case 8: case 9: case 10: return current_rep == EXT_ADJ_LIST;
        case 11: case 12: case 13: return current_rep == ADJ_MAP;
        case 14: return true;
        default: return false;
    }
}
string getRepName(Representation rep) {
    switch (rep) {
        case ADJ_LIST: return "Adjacency List";
        case ADJ_MATRIX: return "Adjacency Matrix";
        case EXT_ADJ_LIST: return "Extended Adjacency List";
        case ADJ_MAP: return "Adjacency Map";
        default: return "Unknown";
    }
}
int main() {
    cout << "=== CHUONG TRINH CHUYEN DOI BIEU DIEN DA DO THI (MULTIGRAPH) ===\n\n";
    int n, m;
    cout << "Nhap so dinh va so canh: ";
    cin >> n >> m;
    current_list.n = n;
    current_list.adj.assign(n, vector<int>());
    cout << "Nhap " << m << " canh (dinh dau dinh cuoi, khong cho phep khuyen):\n";
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        if (u == v) {
            cout << "Khong cho phep khuyen! Bo qua canh nay.\n";
            continue;
        }
        current_list.adj[u].push_back(v);
        current_list.adj[v].push_back(u);
    }
    displayCurrentRepresentation();
    while (true) {
        cout << "\n=== MENU CHUYEN DOI ===\n";
        cout << "1. Hien thi do thi hien tai\n";
        cout << "=== ADJACENCY LIST CONVERSIONS ===\n";
        cout << "2. AL -> Adjacency Matrix\n";
        cout << "3. AL -> Extended Adjacency List\n";
        cout << "4. AL -> Adjacency Map\n";
        cout << "\n=== ADJACENCY MATRIX CONVERSIONS ===\n";
        cout << "5. AM -> Adjacency List\n";
        cout << "6. AM -> Extended Adjacency List\n";
        cout << "7. AM -> Adjacency Map\n";
        cout << "\n=== EXTENDED ADJACENCY LIST CONVERSIONS ===\n";
        cout << "8. EAL -> Adjacency List\n";
        cout << "9. EAL -> Adjacency Matrix\n";
        cout << "10. EAL -> Adjacency Map\n";
        cout << "\n=== ADJACENCY MAP CONVERSIONS ===\n";
        cout << "11. AMap -> Adjacency List\n";
        cout << "12. AMap -> Adjacency Matrix\n";
        cout << "13. AMap -> Extended Adjacency List\n";
        cout << "\n14. Thoat\n";
        cout << "Chon: ";
        int choice;
        cin >> choice;
        if (!isValidConversion(choice)) {
            cout << "\nLOI: Khong the chuyen doi! Do thi hien tai dang o dang "
                 << getRepName(current_rep) << " nhung ban chon chuyen doi tu dang khac.\n";
            cout << "Vui long chon lai!\n";
            continue;
        }
        switch (choice) {
            case 1:
                displayCurrentRepresentation();
                break;
            case 2:
                current_matrix = listToMatrix(current_list);
                current_rep = ADJ_MATRIX;
                cout << "\nDa chuyen doi thanh cong: Adjacency List -> Adjacency Matrix\n";
                displayCurrentRepresentation();
                break;
            case 3:
                current_extended = listToExtended(current_list);
                current_rep = EXT_ADJ_LIST;
                cout << "\nDa chuyen doi thanh cong: Adjacency List -> Extended Adjacency List\n";
                displayCurrentRepresentation();
                break;
            case 4:
                current_map = listToMap(current_list);
                current_rep = ADJ_MAP;
                cout << "\nDa chuyen doi thanh cong: Adjacency List -> Adjacency Map\n";
                displayCurrentRepresentation();
                break;
            case 5:
                current_list = matrixToList(current_matrix);
                current_rep = ADJ_LIST;
                cout << "\nDa chuyen doi thanh cong: Adjacency Matrix -> Adjacency List\n";
                displayCurrentRepresentation();
                break;
            case 6:
                current_extended = matrixToExtended(current_matrix);
                current_rep = EXT_ADJ_LIST;
                cout << "\nDa chuyen doi thanh cong: Adjacency Matrix -> Extended Adjacency List\n";
                displayCurrentRepresentation();
                break;
            case 7:
                current_map = matrixToMap(current_matrix);
                current_rep = ADJ_MAP;
                cout << "\nDa chuyen doi thanh cong: Adjacency Matrix -> Adjacency Map\n";
                displayCurrentRepresentation();
                break;
            case 8:
                current_list = extendedToList(current_extended);
                current_rep = ADJ_LIST;
                cout << "\nDa chuyen doi thanh cong: Extended Adjacency List -> Adjacency List\n";
                displayCurrentRepresentation();
                break;
            case 9:
                current_matrix = extendedToMatrix(current_extended);
                current_rep = ADJ_MATRIX;
                cout << "\nDa chuyen doi thanh cong: Extended Adjacency List -> Adjacency Matrix\n";
                displayCurrentRepresentation();
                break;
            case 10:
                current_map = extendedToMap(current_extended);
                current_rep = ADJ_MAP;
                cout << "\nDa chuyen doi thanh cong: Extended Adjacency List -> Adjacency Map\n";
                displayCurrentRepresentation();
                break;
            case 11:
                current_list = mapToList(current_map);
                current_rep = ADJ_LIST;
                cout << "\nDa chuyen doi thanh cong: Adjacency Map -> Adjacency List\n";
                displayCurrentRepresentation();
                break;
            case 12:
                current_matrix = mapToMatrix(current_map);
                current_rep = ADJ_MATRIX;
                cout << "\nDa chuyen doi thanh cong: Adjacency Map -> Adjacency Matrix\n";
                displayCurrentRepresentation();
                break;
            case 13:
                current_extended = mapToExtended(current_map);
                current_rep = EXT_ADJ_LIST;
                cout << "\nDa chuyen doi thanh cong: Adjacency Map -> Extended Adjacency List\n";
                displayCurrentRepresentation();
                break;
            case 14:
                cout << "Tam biet!\n";
                return 0;
            default:
                cout << "Lua chon khong hop le!\n";
        }
    }
    return 0;
}

