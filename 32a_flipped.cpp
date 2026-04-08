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

Multipole M11;

std::vector<std::vector<int>> get2Connectors(const Multipole& P) {
    std::map<int, std::vector<int>> byExtra;

    for (int i = 0; i < (int)P.semiedges.size(); i++) {
        byExtra[P.semiedges[i].second].push_back(i);
    }

    std::vector<std::vector<int>> res;

    for (auto& kv : byExtra) {
        if (kv.second.size() == 2) res.push_back(kv.second);
    }

    return res;
}

void offsetMultipole(Multipole& M, int offset, int conoffset) {
    int oldN = M.adj.size();
    std::vector<std::vector<int>> newAdj = M.adj;

    for (int i = 0; i < oldN; i++) {
        for (int v = 0; v < M.adj[i].size(); v++) {
            newAdj[i][v] += offset;
        }
    }

    M.adj = newAdj;

    // shift semiedge attachment points
    for (std::pair<int, int>& s : M.semiedges) {
        s.first += offset;
        s.second += conoffset;
    }
}

std::vector<Graph> connect7polesAll(const Multipole& A, const Multipole& B) {
    std::vector<Graph> result;

    int nA = A.adj.size();
    int nB = B.adj.size();

    // --- classify connectors ---
    std::map<int, std::vector<int>> Aconn;
    std::map<int, std::vector<int>> Bconn;

    for (std::size_t i = 0; i < A.semiedges.size(); ++i)
        Aconn[A.semiedges[i].second].push_back(A.semiedges[i].first);

    for (std::size_t i = 0; i < B.semiedges.size(); ++i)
        Bconn[B.semiedges[i].second].push_back(B.semiedges[i].first + nA);

    std::vector<std::pair<std::vector<int>, std::vector<int>>> one;
    std::vector<std::pair<std::vector<int>, std::vector<int>>> two;

    for (int i = 0; i < 5; i++) {
        auto it = Aconn.begin();
        auto it2 = Bconn.begin();
        std::advance(it, i);
        std::advance(it2, i);

        if (it->second.size() == 1)
            one.push_back({it->second, it2->second});
        else
            two.push_back({it->second, it2->second});
    }

    if (two.size() != 2) {
        throw std::runtime_error("Expected exactly 2 two-connectors.");
    }

    // --- enumerate the 4 options for flipping two-connectors ---
    // connector indices: 0 and 1
    std::vector<std::pair<int, int>> flips[4] = {
        {{0, 1}, {0, 1}},  // a0->b0, a1->b1 for both connectors
        {{1, 0}, {0, 1}},  // flip first connector
        {{0, 1}, {1, 0}},  // flip second connector
        {{1, 0}, {1, 0}}   // flip both connectors
    };

    for (int f = 0; f < 4; ++f) {
        Graph G(nA + nB);

        // copy A
        for (int i = 0; i < nA; ++i) G[i] = A.adj[i];

        // copy B (shifted)
        for (int i = 0; i < nB; ++i)
            for (int v : B.adj[i]) G[i + nA].push_back(v + nA);

        // --- glue 2-connectors ---
        for (int c = 0; c < 2; ++c) {
            const auto& a = two[c].first;
            const auto& b = two[c].second;

            int idx0 = flips[f][c].first;
            int idx1 = flips[f][c].second;

            G[a[0]].push_back(b[idx0]);
            G[b[idx0]].push_back(a[0]);

            G[a[1]].push_back(b[idx1]);
            G[b[idx1]].push_back(a[1]);
        }

        // --- glue 1-connectors ---
        for (const auto& conn : one) {
            int a = conn.first[0];
            int b = conn.second[0];
            G[a].push_back(b);
            G[b].push_back(a);
        }

        result.push_back(std::move(G));
    }

    return result;
}

void glueStraight(Multipole& A, Multipole& B, const std::vector<int>& ca,
                  const std::vector<int>& cb, int subA, int subB) {
    int a0 = A.semiedges[ca[0]].first;
    int a1 = A.semiedges[ca[1]].first;

    int b0 = B.semiedges[cb[0]].first;
    int b1 = B.semiedges[cb[1]].first;

    A.adj[a0 - subA].push_back(b0);
    A.adj[a1 - subA].push_back(b1);

    B.adj[b0 - subB].push_back(a0);
    B.adj[b1 - subB].push_back(a1);
}

void glueFlipped(Multipole& A, Multipole& B, const std::vector<int>& ca,
                 const std::vector<int>& cb, int subA, int subB) {
    int a0 = A.semiedges[ca[0]].first;
    int a1 = A.semiedges[ca[1]].first;

    int b0 = B.semiedges[cb[0]].first;
    int b1 = B.semiedges[cb[1]].first;

    A.adj[a0 - subA].push_back(b1);
    A.adj[a1 - subA].push_back(b0);

    B.adj[b0 - subB].push_back(a1);
    B.adj[b1 - subB].push_back(a0);
}

void glue2connectors(std::vector<Multipole>& A, std::vector<Multipole>& B,
                     std::vector<Multipole>& C, const std::vector<int>& ca,
                     const std::vector<int>& cb1, const std::vector<int>& cb2,
                     const std::vector<int>& cc) {
    int subA = 0;
    int subB = A[0].adj.size();
    int subC = subB + B[0].adj.size();
    glueStraight(A[0], B[0], ca, cb1, subA, subB);
    glueStraight(B[0], C[0], cb2, cc, subB, subC);
    glueStraight(A[1], B[1], ca, cb1, subA, subB);
    glueFlipped(B[1], C[1], cb2, cc, subB, subC);
    glueFlipped(A[2], B[2], ca, cb1, subA, subB);
    glueStraight(B[2], C[2], cb2, cc, subB, subC);
    glueFlipped(A[3], B[3], ca, cb1, subA, subB);
    glueFlipped(B[3], C[3], cb2, cc, subB, subC);
}

void remove2connectors(Multipole& A, bool all, const std::vector<int>& ca) {
    if (ca[0] > ca[1]) {
        A.semiedges.erase(A.semiedges.begin() + ca[0]);
        A.semiedges.erase(A.semiedges.begin() + ca[1]);
    } else {
        A.semiedges.erase(A.semiedges.begin() + ca[1]);
        A.semiedges.erase(A.semiedges.begin() + ca[0]);
    }
    if (all) {
        for (int i = 0; i < 2; i++) {
            A.semiedges.erase(A.semiedges.begin() + 1);
        }
    }
}

void makeSnarks(const std::vector<Multipole>& poles) {
    std::ofstream file("snarks.ba");
    int graph_counter = 0;

    int m = poles.size();
    int y = 0;
    for (int i = 0; i < m; i++)
        for (int j = 0; j < m; j++)
            for (int k = 0; k < m; k++) {
                const Multipole& A = poles[i];
                const Multipole& B = poles[j];
                const Multipole& C = poles[k];

                auto CA = get2Connectors(A);
                auto CB = get2Connectors(B);
                auto CC = get2Connectors(C);

                for (auto& a : CA)
                    for (auto& b1 : CB)
                        for (auto& b2 : CB)
                            for (auto& c : CC) {
                                if (b1 == b2) continue;

                                std::vector<Multipole> As(4);
                                std::vector<Multipole> Bs(4);
                                std::vector<Multipole> Cs(4);

                                for (int i = 0; i < 4; i++) {
                                    As[i] = A;
                                    Bs[i] = B;
                                    Cs[i] = C;
                                    offsetMultipole(Bs[i], A.adj.size(), 3);
                                    offsetMultipole(
                                        Cs[i], A.adj.size() + B.adj.size(), 4);
                                }

                                glue2connectors(As, Bs, Cs, a, b1, b2, c);
                                for (int i = 0; i < 4; i++) {
                                    remove2connectors(As[i], false, a);
                                    remove2connectors(Bs[i], true, b1);
                                    remove2connectors(Cs[i], false, c);
                                }

                                std::vector<Multipole> R(4);
                                for (int i = 0; i < 4; i++) {
                                    R[i].adj = As[i].adj;
                                    R[i].adj.insert(R[i].adj.end(),
                                                    Bs[i].adj.begin(),
                                                    Bs[i].adj.end());
                                    R[i].adj.insert(R[i].adj.end(),
                                                    Cs[i].adj.begin(),
                                                    Cs[i].adj.end());

                                    R[i].semiedges = As[i].semiedges;
                                    R[i].semiedges.insert(
                                        R[i].semiedges.end(),
                                        Bs[i].semiedges.begin(),
                                        Bs[i].semiedges.end());
                                    R[i].semiedges.insert(
                                        R[i].semiedges.end(),
                                        Cs[i].semiedges.begin(),
                                        Cs[i].semiedges.end());

                                    // Connect two 7-poles and write all
                                    // resulting graphs
                                    std::vector<Graph> graphs =
                                        connect7polesAll(R[i], M11);
                                    for (const auto& g : graphs) {
                                        int n = g.size();
                                        graph_counter++;
                                        file << graph_counter << "\n";
                                        file << n << "\n";
                                        for (int v = 0; v < n; ++v) {
                                            for (int u : g[v]) {
                                                file << u << " ";
                                            }
                                            file << "\n";
                                        }
                                    }
                                }
                            }
            }
}

Multipole fromAdjWithLeafSemiedges(const Graph& adjInput) {
    Multipole M;

    int n = adjInput.size();

    std::vector<int> isLeaf(n, 0);
    std::vector<int> oldToNew(n, -1);

    // --- Step 1: detect leaves (semiedges) ---
    for (int i = 0; i < n; ++i) {
        if (adjInput[i].size() == 1) isLeaf[i] = 1;
        if (adjInput[i].size() == 2) isLeaf[i] = 2;
    }

    // --- Step 2: build new index map (skip leaves) ---
    int newIndex = 0;
    for (int i = 0; i < n; ++i) {
        if (!isLeaf[i]) oldToNew[i] = newIndex++;
    }

    M.adj.resize(newIndex);

    // --- Step 3: rebuild adjacency without leaves ---
    for (int i = 0; i < n; ++i) {
        if (isLeaf[i]) continue;

        int u = oldToNew[i];

        for (int v : adjInput[i]) {
            if (!isLeaf[v]) M.adj[u].push_back(oldToNew[v]);
        }
    }

    // --- Step 4: collect semiedges ---
    int connectorID = 0;

    for (int i = 0; i < n; ++i) {
        if (!isLeaf[i]) continue;
        for (int j : adjInput[i]) {
            M.semiedges.push_back(std::make_pair(oldToNew[j], connectorID));
        }
        connectorID++;
    }

    return M;
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

int main() {
    M11.adj = {{1, 2},    {0, 3, 8}, {0, 4, 7}, {1, 5}, {2, 6}, {3, 6, 10},
               {4, 5, 9}, {2, 10},   {1, 9},    {6, 8}, {5, 7}};
    M11.semiedges = {{3, 0}, {7, 1}, {9, 1}, {0, 2}, {4, 3}, {8, 4}, {10, 4}};
    std::vector<Graph> negators = read_ba_file("negators.ba");
    std::vector<Multipole> negators2;
    for (Graph& negator : negators) {
        negators2.push_back(fromAdjWithLeafSemiedges(negator));
    }
    makeSnarks(negators2);
}