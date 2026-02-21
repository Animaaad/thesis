#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using Graph = std::vector<std::vector<int>>;
using Multipole = std::vector<std::vector<int>>;

Multipole V4(10);
Multipole Hexagram(12);
Graph K4(4);

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

Graph even_2_2_2_junction(Multipole M1, Multipole M2) {
    int shift = M1.size() - 6;
    for (int i = 1; i < 7; i++) {
        std::tie(M1, M2) =
            vertices_junction(M1, M2, M1.size() - 1, M2.size() - 1, shift);
    }
    Graph G = (Graph)M1;
    for (size_t i = 0; i < M2.size(); ++i) {
        G.push_back(M2[i]);
    }
    for (size_t i = M1.size(); i < M1.size() + M2.size(); ++i) {
        for (size_t j = 0; j < 3; ++j) {
            G[i][j] += shift;
        }
    }
    return G;
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

Multipole connect_dangles(Multipole& G, int n, std::vector<int> connector1,
                          std::vector<int> connector2, bool last,
                          bool flipped) {
    int v1 = connector1[0];
    int v2 = connector1[1];
    int u1 = connector2[0];
    int u2 = connector2[1];
    int erase = 0;
    for (int& j : G[v1]) {
        if (j >= v1 / n * n + n || j < v1 / n * n) {
            erase = j;
        }
    }
    G[v1].erase(G[v1].begin() + erase);

    for (int& j : G[v2]) {
        if (j >= v2 / n * n + n || j < v2 / n * n) {
            erase = j;
        }
    }
    G[v2].erase(G[v2].begin() + erase);

    for (int& j : G[u1]) {
        if (j >= u1 / n * n + n || j < u1 / n * n) {
            erase = j;
        }
    }
    if (!last) {
        G[u1].erase(G[u1].begin() + erase);
    }

    for (int& j : G[u2]) {
        if (j >= u2 / n * n + n || j < u2 / n * n) {
            erase = j;
        }
    }
    if (!last) {
        G[u2].erase(G[u2].begin() + erase);
    }

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

std::vector<int> find_dangles2(int k, Graph G, int n) {
    std::vector<int> vec;
    for (int i = k * n; i < k * n + n; i++) {
        for (int& j : G[i]) {
            if (j >= k * n + n || j < k * n) vec.push_back(i);
        }
    }
    return vec;
}

std::vector<Multipole> replacement(Multipole& G, Multipole Replacement_multipole) {
    std::vector<Multipole> ret;
    std::vector<Multipole> ret2;
    Multipole Gr1;
    int n = Replacement_multipole.size() - 6;
    std::vector<std::vector<int>> connectors;
    std::map<int, int> connectors_counter;
    for (int i = 0; i < 3; i++) {
        std::vector<int> vec;
        connectors.push_back(vec);
    }
    for (int j = 0; j < 6; j++) {
        connectors[j / 2].push_back(Replacement_multipole[n + j][0]);
    }
    for (int j = 0; j < 6; j++) {
        Replacement_multipole.erase(Replacement_multipole.end() - 1);
    }
    for (int i = 0; i < G.size() - 3; i++) {
        Gr1.insert(Gr1.end(), Replacement_multipole.begin(),
                   Replacement_multipole.end());
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < Replacement_multipole[j].size(); k++) {
                Replacement_multipole[j][k] += n;
            }
        }
    }
    ret.push_back(Gr1);
    for (int i = 0; i < G.size() - 3; i++) {
        for (int& j : G[i]) {
            for (Multipole Gr : ret) {
                if (j >= G.size() - 3) {
                    int next = Gr.size();
                    std::vector<int> connector1;
                    std::vector<int> connector2;
                    Gr.push_back(connector1);
                    Gr.push_back(connector2);
                    for (int x = 0; x < 2; x++) {
                        connector1.push_back(
                            connectors[connectors_counter[i]][x] + n * i);
                        connector2.push_back(next + x);
                    }
                    Gr = connect_dangles(Gr, n, connector1, connector2, true,
                                         false);
                    ret2.push_back(Gr);
                } else if (i < j) {
                    std::vector<int> connector1;
                    std::vector<int> connector2;
                    for (int x = 0; x < 2; x++) {
                        connector1.push_back(
                            connectors[connectors_counter[i]][x] + n * i);
                        connector2.push_back(
                            connectors[connectors_counter[j]][x] + n * j);
                    }
                    Multipole Gr2 = Gr;
                    Gr = connect_dangles(Gr, n, connector1, connector2, false,
                                         false);
                    Gr2 = connect_dangles(Gr2, n, connector1, connector2, false,
                                          true);
                    ret2.push_back(Gr);
                    ret2.push_back(Gr2);

                }
            }
            if (j >= G.size() - 3 || i < j) {
                connectors_counter[i]++;
                connectors_counter[j]++;
                ret.clear();
            }
            
            ret.insert(ret.end(), ret2.begin(), ret2.end());
            ret2.clear();
        }
    }
    return ret;
}

std::vector<Multipole> subdivision(Graph G) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 0; u < G.size(); u++) {
        for (int v : G[u]) {
            if (u < v) edges.push_back({u, v});
        }
    }

    std::vector<Multipole> result;
    int m = edges.size();
    for (int i = 0; i < m; i++) {
        for (int j = i + 1; j < m; j++) {
            for (int k = j + 1; k < m; k++) {
                Graph H = G;
                int newVertex = H.size();
                for (int i = 0; i < 6; i++) {
                    std::vector<int> vec;
                    H.push_back(vec);
                }
                std::vector<int> edgeIndices = {i, j, k};
                for (int idx = 0; idx < 3; idx++) {
                    int e = edgeIndices[idx];
                    int u = edges[e].first;
                    int v = edges[e].second;
                    int w = newVertex + idx;
                    int z = w + 3;
                    H[u].erase(remove(H[u].begin(), H[u].end(), v), H[u].end());
                    H[v].erase(remove(H[v].begin(), H[v].end(), u), H[v].end());
                    H[u].push_back(w);
                    H[v].push_back(w);
                    H[w].push_back(u);
                    H[w].push_back(v);
                    H[w].push_back(z);
                    H[z].push_back(w);
                }
                result.push_back(H);
            }
        }
    }

    return result;
}

#include <bits/stdc++.h>

std::vector<Graph> read_ba_file(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) throw std::runtime_error("Cannot open file: " + filename);

    int n;
    in >> n;
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
    try {
        std::ofstream file("snarks.ba");
        std::string line;
        int line_number;
        std::vector<Graph> input = read_ba_file("cub.ba");

        V4 = {{1, 2, 3}, {0, 4, 5}, {0, 6, 7}, {0, 8, 9}, {1},
              {1},       {2},       {2},       {3},       {3}};
        Hexagram = {{1, 5, 6}, {0, 2, 8},  {1, 3, 10}, {2, 4, 7},
                    {3, 5, 9}, {0, 4, 11}, {0},        {3},
                    {1},       {4},        {2},        {5}};

        Multipole M1 = {{1, 2, 3}, {0, 2, 4}, {0, 1, 5}, {0, 6, 7}, {1, 6, 8},
                        {2, 6, 9}, {3, 4, 5}, {3},       {4},       {5}};
        K4 = {{1, 2, 3}, {0, 2, 3}, {0, 1, 3}, {0, 1, 2}};

        int graph_counter = 0;
        for (Graph G1 : input) {
            std::vector<Multipole> vec = subdivision(G1);
            for (Multipole M : vec) {
                std::vector<Graph> gs = replacement(M, Hexagram);
                for (Graph G2 : gs) {
                    graph_counter++;
                    G2 = even_2_2_2_junction(G2, V4);
                    int n = G2.size();
                    file << graph_counter << "\n" << n << "\n";
                    for (int i = 0; i < n; i++) {
                        for (int j : G2[i]) {
                            file << j << " ";
                        }
                        file << "\n";
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }

} /**/
