#include <bits/stdc++.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using Graph = std::vector<std::vector<int>>;
struct Multipole {
    std::vector<std::vector<int>> adj;
    std::vector<std::pair<int, int>>
        semiedges;  // (remaining vertex, deleted vertex)
};
Graph M11;

std::vector<std::array<int, 3>> findPaths2(Graph G) {
    std::vector<std::array<int, 3>> paths;

    for (int v = 0; v < G.size(); v++) {
        for (int u : G[v])
            for (int w = 0; w < G.size(); w++) {
                if (w != u && w != v) {
                    paths.push_back({u, v, w});
                }
            }
    }
    return paths;
}

/* ---------- remove vertices ---------- */

Multipole removeTriple(int u, int v, int w, const Graph& G) {
    int n = G.size();
    int semiedge = 1;

    // mark removed vertex
    std::vector<bool> removed(n, false);
    removed[w] = true;

    // relabel remaining vertices
    std::vector<int> newId(n, -1);
    int id = 0;
    for (int i = 0; i < n; i++) {
        if (!removed[i]) newId[i] = id++;
    }

    Multipole F;
    F.adj.resize(id);

    for (int x = 0; x < n; x++) {
        if (removed[x]) continue;

        for (int y : G[x]) {

            // --- case 1: removed vertex (creates 3 semiedges)
            if (y == w) {
                F.semiedges.push_back({newId[x], semiedge++});
            }

            // --- case 2: removed edge (u-v)
            else if ((x == u && y == v) || (x == v && y == u)) {
                F.semiedges.push_back({newId[x], 0});
            }

            // --- normal edge
            else if (!removed[y]) {
                F.adj[newId[x]].push_back(newId[y]);
            }
        }
    }

    return F;
}

std::vector<Multipole> prop2_3_generator(Graph G) {
    std::vector<std::array<int, 3>> P3 = findPaths2(G);
    std::vector<Multipole> poles;
    for (auto [u, v, w] : P3) {
        poles.push_back(removeTriple(u, v, w, G));
    }
    return poles;
}

std::vector<Graph> read_ba_file(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) throw std::runtime_error("Cannot open file: " + filename);
    int idx;
    int n;
    in >> n;
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // clear line
    std::vector<Graph> graphs;
    graphs.reserve(n);

    for (int g = 0; g < n; ++g) {
        int graph_id, V;

        in >> graph_id;
        in >> V;
        in.ignore(std::numeric_limits<std::streamsize>::max(),
                  '\n');  // clear line

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

void printMBA(const std::vector<Multipole>& poles,
              const std::string& filename) {
    std::ofstream out(filename);
    int a = 0;
    if (!out.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    for (const Multipole& M : poles) {
        a++;
        out << a << "\n";  // separator between multipoles
        int n = M.adj.size();
        std::map<int, int> semiedges;

        out << n + 4 << "\n";
        int b = n;
        std::array<std::array<int, 2>, 4> con;
        std::array<bool, 4> written = {false, false, false, false};
        for (int i = 0; i < n; i++) {
            for (int j : M.adj[i]) {
                out << j << " ";
            }

            for (auto& s : M.semiedges) {
                if (s.first == i) {
                    if (semiedges[s.second] == 0) {
                        semiedges[s.second] = b;
                        con[b - n][0] = i;
                        b++;
                    } else {
                        written[semiedges[s.second] - n] = true;
                        con[semiedges[s.second] - n][1] = i;
                    }
                    out << semiedges[s.second] << " ";
                }
            }
            out << "\n";
        }
        for (int i = 0; i < 4; i++) {
            if (written[i]) {
                for (int j = 0; j < 2; j++) {
                    out << con[i][j] << " ";
                }
            } else {
                out << con[i][0];
            }
            out << "\n";
        }
    }

    out.close();
}

int main() {
    std::vector<Graph> snarks = read_ba_file("smallSnarks.ba");
    std::vector<Multipole> allpoles;
    for (Graph& G : snarks) {
        std::vector<Multipole> prop_2_3s = prop2_3_generator(G);
        allpoles.insert(allpoles.end(), prop_2_3s.begin(), prop_2_3s.end());
    }
    printMBA(allpoles, "23poles.ba");
}