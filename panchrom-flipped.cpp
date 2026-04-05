#include <bits/stdc++.h>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

int id = 0;

using namespace std;

using Graph = std::vector<std::vector<int>>;
using Multipole = std::vector<std::vector<int>>;

Multipole V4(10);
Multipole Hexagram(12);
Graph K4(4);

bool contains(const std::vector<pair<int, int>>& v, const pair<int, int>& x) {
    return std::find(v.begin(), v.end(), x) != v.end();
}

struct Blossom {
    int n;
    vector<vector<int>> g;
    vector<int> match, p, base;
    vector<bool> used, blossom;
    queue<int> q;

    // Constructor now takes adjacency list
    Blossom(const vector<vector<int>>& graph)
        : n(graph.size()),
          g(graph),
          match(n, -1),
          p(n),
          base(n),
          used(n),
          blossom(n) {}

    int lca(int a, int b) {
        vector<bool> used_lca(n, false);
        while (true) {
            a = base[a];
            used_lca[a] = true;
            if (match[a] == -1) break;
            a = p[match[a]];
        }
        while (true) {
            b = base[b];
            if (used_lca[b]) return b;
            b = p[match[b]];
        }
    }

    void mark_path(int v, int b, int children) {
        while (base[v] != b) {
            blossom[base[v]] = blossom[base[match[v]]] = true;
            p[v] = children;
            children = match[v];
            v = p[match[v]];
        }
    }

    int find_path(int root) {
        fill(used.begin(), used.end(), false);
        fill(p.begin(), p.end(), -1);
        for (int i = 0; i < n; i++) base[i] = i;

        q = queue<int>();
        q.push(root);
        used[root] = true;

        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int u : g[v]) {
                if (base[v] == base[u] || match[v] == u) continue;

                if (u == root || (match[u] != -1 && p[match[u]] != -1)) {
                    int curbase = lca(v, u);
                    fill(blossom.begin(), blossom.end(), false);
                    mark_path(v, curbase, u);
                    mark_path(u, curbase, v);

                    for (int i = 0; i < n; i++) {
                        if (blossom[base[i]]) {
                            base[i] = curbase;
                            if (!used[i]) {
                                used[i] = true;
                                q.push(i);
                            }
                        }
                    }
                } else if (p[u] == -1) {
                    p[u] = v;
                    if (match[u] == -1) {
                        // augmenting path found
                        v = u;
                        while (v != -1) {
                            int pv = p[v];
                            int nv = (pv == -1 ? -1 : match[pv]);
                            match[v] = pv;
                            if (pv != -1) match[pv] = v;
                            v = nv;
                        }
                        return 1;
                    } else {
                        used[match[u]] = true;
                        q.push(match[u]);
                    }
                }
            }
        }
        return 0;
    }

    int max_matching() {
        int matching = 0;
        for (int i = 0; i < n; i++) {
            if (match[i] == -1) {
                matching += find_path(i);
            }
        }
        return matching;
    }
};

vector<pair<int, int>> get_matching(const Blossom& solver) {
    vector<pair<int, int>> res;
    for (int i = 0; i < solver.n; i++) {
        if (solver.match[i] != -1 && i < solver.match[i]) {
            res.emplace_back(i, solver.match[i]);
        }
    }
    return res;
}

std::tuple<Multipole, Multipole> vertices_junction(Multipole& M1, Multipole& M2,
                                                   int u, int v, int shift) {
    int a = M1[u][0];
    int b = M2[v][0];
    M1[a].push_back(b + shift);
    M2[b].push_back(a - shift);
    M1[a].erase(std::find(M1[a].begin(), M1[a].end(), u));
    M2[b].erase(std::find(M2[b].begin(), M2[b].end(), v));
    M1.erase(M1.begin() + u);
    M2.erase(M2.begin() + v);
    return {M1, M2};
}

void printGraph(Graph& G) {
    for (size_t i = 0; i < G.size(); ++i) {
        std::cout << i << ": ";
        for (int& x : G[i]) {
            std::cout << x << " ";
        }
        std::cout << '\n';
    }
}

Multipole connect_dangles(Multipole G, int n1, int n2,
                          std::vector<int> connector1,
                          std::vector<int> connector2, bool flipped) {
    int v1 = connector1[0];
    int v2 = connector1[1];
    int u1 = connector2[0];
    int u2 = connector2[1];
    int erase = 2;
    
    G[v1].erase(G[v1].begin() + erase);
    
    G[v2].erase(G[v2].begin() + erase);
    
    G[u1].erase(G[u1].begin() + erase);

    G[u2].erase(G[u2].begin() + erase);

    if (!flipped) {
        G[v1].push_back(u1);
        G[v2].push_back(u2);
        G[u1].push_back(v1);
        G[u2].push_back(v2);
    } else {
        G[v1].push_back(u2);
        G[v2].push_back(u1);
        G[u1].push_back(v2);
        G[u2].push_back(v1);
    }
    
    return G;
}

Graph connectMatchingDangles(Graph& G, int n1, int n2, int u, int v) {
    int erase = 2;

    G[u].erase(G[u].begin() + erase);

    G[v].erase(G[v].begin() + erase);

    G[u].push_back(v);
    G[v].push_back(u);
    return G;
}

void checkGraph(const Graph& adj) {
    bool hasLoop = false;
    bool hasMultiEdge = false;

    int n = adj.size();

    for (int u = 0; u < n; u++) {
        std::set<int> seen;

        for (int v : adj[u]) {
            // check loop
            if (v == u) {
                hasLoop = true;
            }

            // check multiedge
            if (seen.count(v)) {
                hasMultiEdge = true;
                std::cout << v << " " << u << std::endl;
            } else {
                seen.insert(v);
            }
        }
    }
    id++;
    if (hasLoop) std::cout << "Graph has loops\n";
    if (hasMultiEdge) {
        std::cout << id << std::endl;
    }
}

std::vector<Graph> replacement(Graph& G,
                               std::vector<Multipole> Replacement_multipoles,
                               int bits,
                               std::vector<std::pair<int, int>> matching) {
    std::vector<Graph> ret;
    std::vector<Graph> ret2;
    Graph Gr1;
    int ns[2];
    std::vector<std::vector<int>> connectors[2];
    for (int k = 0; k < 2; k++) {
        ns[k] = Replacement_multipoles[k].size() - 7;

        for (int j = 0; j < 4; j++) {
            std::vector<int> vec;
            connectors[k].push_back(vec);
        }

        for (int j = 0; j < 7; j++) {
            connectors[k][j / 2].push_back(
                Replacement_multipoles[k][ns[k] + j][0]);
        }

        for (int j = 0; j < 7; j++) {
            Replacement_multipoles[k].erase(Replacement_multipoles[k].end() -
                                            1);
        }
    }

    std::map<int, int> connectors_counter;
    std::vector<int> current_n(G.size() + 1, 0);

    for (int i = 0; i < G.size(); i++) {
        int which = (bits >> i) & 1;
        current_n[i + 1] = current_n[i] + ns[which];

        Gr1.insert(Gr1.end(), Replacement_multipoles[which].begin(),
                   Replacement_multipoles[which].end());

        for (int k = 0; k < 2; k++) {
            for (int j = 0; j < ns[k]; j++) {
                for (int l = 0; l < Replacement_multipoles[k][j].size(); l++) {
                    Replacement_multipoles[k][j][l] += ns[which];
                }
            }
        }
    }

    ret.push_back(Gr1);

    for (int i = 0; i < G.size(); i++) {
        for (int& j : G[i]) {
            if (i < j) {
                for (Multipole Gr : ret) {
                    int which1 = (bits >> i) & 1;
                    int which2 = (bits >> j) & 1;
                    int n1 = ns[which1];
                    int n2 = ns[which2];
                    if (contains(matching, {i, j}) ||
                        contains(matching, {j, i})) {
                        connectMatchingDangles(
                            Gr, n1, n2, connectors[which1][3][0] + current_n[i],
                            connectors[which2][3][0] + current_n[j]);
                    } else {
                        std::vector<int> connector1;
                        std::vector<int> connector2;
                        for (int x = 0; x < 2; x++) {
                            connector1.push_back(
                                connectors[which1][connectors_counter[i]][x] +
                                current_n[i]);
                            connector2.push_back(
                                connectors[which2][connectors_counter[j]][x] +
                                current_n[j]);
                        }
                        Graph Gr2 = Gr;
                        Gr =
                            connect_dangles(Gr, n1, n2, connector1, connector2, false);
                        Gr2 =
                            connect_dangles(Gr, n1, n2, connector1, connector2, true);

                        ret2.push_back(Gr);
                        ret2.push_back(Gr2);
                    }
                }
                if (i < j && !(contains(matching, {i, j}) ||
                              contains(matching, {j, i}))) {
                    connectors_counter[i]++;
                    connectors_counter[j]++;
                    ret.clear();
                }
                ret.insert(ret.end(), ret2.begin(), ret2.end());
                ret2.clear();
            }
        }
    }

    // checkGraph(Gr);
    return ret;
}

std::vector<Graph> read_ba_file(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) throw std::runtime_error("Cannot open file: " + filename);

    int n;
    in >> n;
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // clear line

    std::vector<Graph> graphs;
    graphs.reserve(n);

    for (int g = 0; g < n; ++g) {
        int graph_id, V;

        in >> graph_id;
        in >> V;
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        Graph adj(V);

        for (int v = 0; v < V; ++v) {
            std::string line;
            getline(in, line);

            std::stringstream ss(line);
            int neighbor;

            while (ss >> neighbor) {
                adj[v].push_back(neighbor);
            }
        }

        graphs.push_back(adj);
    }

    return graphs;
}

int main() {
    Multipole R1 = {
        {3, 4, 32},   {2, 4, 23},   {1, 3, 5},    {0, 2, 9},    {0, 1, 6},
        {2, 6, 31},   {4, 5, 7},    {6, 8, 26},   {7, 10, 11},  {3, 11, 12},
        {8, 12, 15},  {8, 9, 14},   {9, 10, 13},  {12, 14, 25}, {11, 13, 15},
        {10, 14, 16}, {15, 20, 21}, {19, 22, 24}, {19, 21, 23}, {17, 18, 20},
        {16, 19, 35}, {16, 18, 22}, {17, 21, 36}, {1, 18, 37},  {17, 27, 28},
        {13, 28, 29}, {7, 27, 30},  {24, 26, 29}, {24, 25, 30}, {25, 27, 34},
        {26, 28, 33}, {5},          {0},          {30},         {29},
        {20},         {22},         {23}};
    Multipole R2 = {
        {3, 4, 34},   {2, 4, 23},   {1, 3, 5},    {0, 2, 9},    {0, 1, 6},
        {2, 6, 33},   {4, 5, 7},    {6, 8, 26},   {7, 10, 11},  {3, 11, 12},
        {8, 12, 15},  {8, 9, 14},   {9, 10, 13},  {12, 14, 25}, {11, 13, 15},
        {10, 14, 16}, {15, 20, 21}, {19, 22, 24}, {19, 21, 23}, {17, 18, 20},
        {16, 19, 37}, {16, 18, 22}, {17, 21, 38}, {1, 18, 39},  {17, 27, 28},
        {13, 28, 29}, {7, 27, 31},  {24, 26, 29}, {24, 25, 30}, {25, 27, 36},
        {28, 31, 32}, {26, 30, 32}, {30, 31, 35}, {5},          {0},
        {30},         {29},         {20},         {22},         {23}};

    try {
        std::ofstream file("snarks.ba");
        int graph_counter = 0;
        int line_number;
        std::vector<Graph> input = read_ba_file("4reg.ba");
        for (Graph G : input) {
            Blossom matching_solver(G);
            int matching_size = matching_solver.max_matching();
            std::vector<std::pair<int, int>> matching =
                get_matching(matching_solver);
            int bits = 0;
            std::vector<Multipole> Rms = {R1, R2};
            std::vector<Graph> gs = replacement(G, Rms, bits, matching);
            for (Graph G2 : gs) {
                graph_counter++;
                int n = G2.size();
                file << graph_counter << "\n" << n << "\n";
                for (int i = 0; i < n; i++) {
                    for (int j : G2[i]) {
                        file << j << " ";
                    }
                    file << "\n";
                }

                bits++;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }
} /**/