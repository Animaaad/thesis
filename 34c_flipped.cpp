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



bool isValidAdjList(const std::vector<std::vector<int>>& g) {
    int n = g.size();

    // 1–3: bounds, self-loops, duplicates
    for (int i = 0; i < n; i++) {
        std::unordered_set<int> seen;

        for (int j : g[i]) {
            if (j < 0 || j >= n)
                return false;

            if (j == i)
                return false;

            if (seen.count(j))
                return false;

            seen.insert(j);
        }
    }

    // 4: symmetry
    for (int i = 0; i < n; i++) {
        for (int j : g[i]) {
            bool found = false;
            for (int x : g[j]) {
                if (x == i) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
    }

    return true;
}



int get1Connector(const Multipole& P) {
    std::map<int, std::vector<int>> byExtra;

    for (int i = 0; i < (int)P.semiedges.size(); i++) {
        byExtra[P.semiedges[i].second].push_back(i);
    }

    std::vector<std::vector<int>> res;

    for (auto& kv : byExtra) {
        if (kv.second.size() == 1) return kv.second[0];
    }
    return -1;
}

std::vector<std::vector<std::vector<int>>> get2Connectors(const Multipole& P) {
    std::map<int, std::vector<int>> byExtra;

    for (int i = 0; i < (int)P.semiedges.size(); i++) {
        byExtra[P.semiedges[i].second].push_back(i);
    }

    std::vector<std::vector<std::vector<int>>> res(
        2, std::vector<std::vector<int>>(3, std::vector<int>(2)));

    int i = 0;
    for (auto& kv : byExtra) {
        if (kv.second.size() == 2) {
            res[0][i] = kv.second;
            i++;
        }
    }

    res[1][0][0] = res[0][0][1];
    res[1][0][1] = res[0][0][0];
    res[1][1][0] = res[0][1][1];
    res[1][1][1] = res[0][1][0];
    res[1][2][0] = res[0][2][1];
    res[1][2][1] = res[0][2][0];

    return res;
}

std::vector<std::vector<int>> get3Connectors(const Multipole& P) {
    std::map<int, std::vector<int>> byExtra;

    for (int i = 0; i < (int)P.semiedges.size(); i++) {
        byExtra[P.semiedges[i].second].push_back(i);
    }

    std::vector<std::vector<int>> res(6, std::vector<int>(3));

    int i = 0;
    for (auto& kv : byExtra) {
        if (kv.second.size() == 3) {
            res[0] = kv.second;
            i++;
        }
    }

    std::vector<std::vector<int>> permutations = {
        {0, 1, 2}, {0, 2, 1}, {2, 1, 0}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}};

    for (int i = 1; i < 6; i++) {
        for (int j = 0; j < 3; j++) {
            res[i][j] = res[0][permutations[i][j]];
        }
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

void glue2connectors(Multipole& A, Multipole& B, const std::vector<int>& ca,
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

void connectRest(Multipole& T1, Multipole& R, Multipole& N, Multipole& T2,
                 std::vector<int> B, std::vector<int> t1, std::vector<int> t2,
                 int v, int subR, int subN, int subT2) {
    int t2Max = T2.adj.size();
    T2.adj.resize(t2Max + 1);

    int a0 = T1.semiedges[t1[0]].first;
    int a1 = T1.semiedges[t1[1]].first;
    int a2 = T1.semiedges[t1[2]].first;
    int b0 = T2.semiedges[t2[0]].first;
    int b1 = T2.semiedges[t2[1]].first;
    int b2 = T2.semiedges[t2[2]].first;
    int r0 = R.semiedges[B[0]].first;
    int r1 = R.semiedges[B[1]].first;

    int n = N.semiedges[get1Connector(N)].first;

    T1.adj[a0].push_back(v);
    T1.adj[a1].push_back(r0);
    T1.adj[a2].push_back(n);

    T2.adj[b0 - subT2].push_back(v);
    T2.adj[b1 - subT2].push_back(v);
    T2.adj[b2 - subT2].push_back(r1);

    T2.adj[t2Max].push_back(a0);
    T2.adj[t2Max].push_back(b0);
    T2.adj[t2Max].push_back(b1);

    R.adj[r0 - subR].push_back(a1);
    R.adj[r1 - subR].push_back(b2);

    N.adj[n - subN].push_back(a2);
}

void connectPoles(std::vector<Multipole>& T1, std::vector<Multipole>& R,
                  std::vector<Multipole>& N, std::vector<Multipole>& T2) {
    int id = 0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                for (int l = 0; l < 2; l++) {
                    for (int f = 0; f < 3; f++) {
                        for (int h = 0; h < 6; h++) {
                            auto CA2 = get2Connectors(T1[0]);
                            auto CB2 = get2Connectors(T2[0]);
                            auto CN = get2Connectors(N[0]);
                            auto CR = get2Connectors(R[0]);
                            auto CA3 = get3Connectors(T1[0]);
                            auto CB3 = get3Connectors(T2[0]);

                            for (auto& b1 : CN[j])
                                for (auto& b2 : CN[k]) {
                                    if (b1 == b2 || b1 == CN[j][2] ||
                                        b2 == CN[k][2] || b1[0] == b2[1])
                                        continue;

                                    int n1 = T1[0].adj.size();
                                    int n2 = n1 + R[0].adj.size();
                                    int n3 = n2 + N[0].adj.size();

                                    offsetMultipole(R[id], n1, 0);
                                    offsetMultipole(N[id], n2, 0);
                                    offsetMultipole(T2[id], n3, 0);

                                    glue2connectors(T1[id], R[id], CA2[i][0],
                                                    CR[0][0], 0, n1);
                                    glue2connectors(R[id], N[id], CR[0][2], b1,
                                                    n1, n2);
                                    glue2connectors(N[id], T2[id], b2,
                                                    CB2[0][0], n2, n3);

                                    connectRest(T1[id], R[id], N[id], T2[id],
                                                CR[l][1], CA3[h], CB3[f],
                                                n3 + T2[id].adj.size(), n1, n2,
                                                n3);
                                    id++;
                                }
                        }
                    }
                }
            }
        }
    }
}

void makeSnarks(const std::vector<Multipole>& negators,
                const std::vector<Multipole>& poles,
                const std::vector<Multipole>& rs) {
    std::ofstream file("snarks.ba");
    int graph_counter = 0;

    int m = poles.size();
    int n = negators.size();

    int y = 0;
    for (int i = 0; i < m; i++)
        for (int j = 0; j < m; j++)
            for (int k = 0; k < n; k++) {
                for (int l = 0; l < 2; l++) {
                    std::vector<Multipole> T1s(576);
                    std::vector<Multipole> T2s(576);
                    std::vector<Multipole> Ns(576);
                    std::vector<Multipole> Rs(576);

                    for (int f = 0; f < 576; f++) {
                        T1s[f] = poles[i];
                        T2s[f] = poles[j];
                        Ns[f] = negators[k];
                        Rs[f] = rs[l];
                    }

                    connectPoles(T1s, Rs, Ns, T2s);

                    /*remove2connectors(AB, false, a);
                    remove2connectors(BB, true, b1);
                    remove2connectors(CBp, false, c);*/

                    for (int f = 0; f < 576; f++) {
                        Graph Ret;

                        Ret = T1s[f].adj;
                        Ret.insert(Ret.end(), Rs[f].adj.begin(),
                                   Rs[f].adj.end());
                        Ret.insert(Ret.end(), Ns[f].adj.begin(),
                                   Ns[f].adj.end());
                        Ret.insert(Ret.end(), T2s[f].adj.begin(),
                                   T2s[f].adj.end());

                        Graph g = Ret;

                        isValidAdjList(g);
                        int n = g.size();
                        graph_counter++;
                        file << graph_counter << "\n" << n << "\n";
                        for (int i = 0; i < n; i++) {
                            for (int j : g[i]) {
                                file << j << " ";
                            }
                            file << "\n";
                        }
                    }
                }
            }
}

Multipole formNegator(const Graph& adjInput) {
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

Multipole form23pole(const Graph& adjInput) {
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
    std::vector<std::pair<int, int>> con3;

    for (int i = 0; i < n; ++i) {
        if (!isLeaf[i]) continue;
        if (adjInput[i].size() == 1) {
            con3.push_back({oldToNew[adjInput[i][0]], 0});
        } else {
            for (int j : adjInput[i]) {
                M.semiedges.push_back(std::make_pair(oldToNew[j], 1));
            }
        }
    }

    M.semiedges.insert(M.semiedges.end(), con3.begin(), con3.end());
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
    std::vector<Graph> negators = read_ba_file("negators.ba");
    std::vector<Multipole> negators2;
    for (Graph& negator : negators) {
        negators2.push_back(formNegator(negator));
    }
    std::vector<Graph> proper23s = read_ba_file("23poles.ba");
    std::vector<Multipole> prop_23_poles;
    for (Graph& pole : proper23s) {
        prop_23_poles.push_back(form23pole(pole));
    }
    Multipole r1;
    r1.adj = {{1, 7}, {0, 2, 5}, {1, 3}, {2, 4},
              {3, 5}, {1, 4, 6}, {5, 7}, {0, 6}};
    r1.semiedges = {{0, 0}, {4, 0}, {2, 1}, {6, 1}, {3, 2}, {7, 2}};
    Multipole r2;
    r2.adj = {{1, 9},    {0, 2, 6}, {1, 3},    {2, 4}, {3, 5},
              {4, 6, 7}, {1, 5, 7}, {5, 6, 8}, {7, 9}, {0, 8}};
    r2.semiedges = {{0, 0}, {4, 0}, {2, 1}, {8, 1}, {3, 2}, {9, 2}};
    std::vector<Multipole> rs = {r1, r2};

    makeSnarks(negators2, prop_23_poles, rs);
}