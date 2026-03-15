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
            for (int w : G[v]) {
                if (u < w && u != w) {
                    paths.push_back({u, v, w});
                }
            }
    }
    return paths;
}

/* ---------- remove vertices ---------- */

Multipole removeTriple(int a, int b, int c, Graph G) {
    std::vector<bool> removed(G.size(), false);
    removed[a] = removed[b] = removed[c] = true;

    std::vector<int> newId(G.size(), -1);
    int id = 0;

    for (int i = 0; i < G.size(); i++)
        if (!removed[i]) newId[i] = id++;

    Multipole F;
    F.adj.resize(id);

    for (int v = 0; v < G.size(); v++) {
        if (removed[v]) continue;

        for (int u : G[v]) {
            if (removed[u]) {
                F.semiedges.push_back({newId[v], u});
            } else {
                F.adj[newId[v]].push_back(newId[u]);
            }
        }
    }

    return F;
}

std::vector<Multipole> negator_generator(Graph G) {
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

        out << n + 3 << "\n";
        int b = n;
        std::array<std::array<int, 2>, 3> con;
        std::array<bool, 3> written = {false, false, false};
        for (int i = 0; i < n; i++) {
            for (int j : M.adj[i]) {
                out << j << " ";
            }

            for (auto& s : M.semiedges) {
                if (s.first == i) {
                    if (semiedges[s.second] == 0) {
                        semiedges[s.second] = b;
                        con[semiedges[s.second] - n][0] = i;
                        b++;
                    } else {
                        written[semiedges[s.second] - n] = true;
                        con[semiedges[s.second] - n][1] = i;
                    }
                    out << semiedges[s.second];
                }
            }
            out << "\n";
        }
        for (int i = 0; i < 3; i++) {
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
    std::vector<Multipole> allnegators;
    for (Graph& G : snarks) {
        std::vector<Multipole> negators = negator_generator(G);
        allnegators.insert(allnegators.end(), negators.begin(), negators.end());
    }
    printMBA(allnegators, "negators.ba");
}