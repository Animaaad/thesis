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
int id = 0;

Multipole J;

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

void offsetMultipole(Multipole& M, int offset) {
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
        // s.second stays unchanged
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

void remove2connectors(Multipole& A) {
    for (int i = 0; i < 4; i++) {
        A.semiedges.erase(A.semiedges.begin() + 1);
    }
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
                std::cout << v << " " << u;
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

void connectRest(Multipole& AB, Multipole& BB, Multipole& CB, Multipole& DB,
                 Multipole& EB, std::vector<int> ca, std::vector<int> ce, int v,
                 int subB, int subC, int subD, int subE) {
    int eMax = EB.adj.size();
    EB.adj.resize(eMax + 1);

    int a0 = AB.semiedges[ca[0]].first;
    int a1 = AB.semiedges[ca[1]].first;
    int e0 = EB.semiedges[ce[0]].first;
    int e1 = EB.semiedges[ce[1]].first;

    int a = AB.semiedges[get1Connector(AB)].first;
    int b = BB.semiedges[get1Connector(BB)].first;
    int c = CB.semiedges[get1Connector(CB)].first;
    int d = DB.semiedges[get1Connector(DB)].first;
    int e = EB.semiedges[get1Connector(EB)].first;

    AB.adj[a0].push_back(v);
    EB.adj[e0 - subE].push_back(v);
    CB.adj[c - subC].push_back(v);
    EB.adj[eMax].push_back(a0);
    EB.adj[eMax].push_back(e0);
    EB.adj[eMax].push_back(c);
    
    AB.adj[a].push_back(e);
    EB.adj[e - subE].push_back(v);

    AB.adj[a1].push_back(d);
    EB.adj[e1 - subE].push_back(b);
    BB.adj[b - subB].push_back(e1);
    DB.adj[d - subD].push_back(a1);

}

void makeSnarks(const std::vector<Multipole>& negators) {
    std::ofstream file("snarks.ba");
    int graph_counter = 0;
    int m = negators.size();
    int y = 0;
    for (int i = 0; i < m; i++)
        for (int j = 0; j < m; j++)
            for (int k = 0; k < m; k++) {
                for (int l = 0; l < m; l++)
                    for (int f = 0; f < m; f++) {
                        const Multipole& A = negators[i];
                        const Multipole& B = negators[j];
                        const Multipole& C = negators[k];
                        const Multipole& D = negators[l];
                        const Multipole& E = negators[f];

                        Multipole AB = A;
                        Multipole BB = B;
                        Multipole CBp = C;
                        Multipole DB = D;
                        Multipole EB = E;

                        int n1 = A.adj.size();
                        int n2 = n1 + B.adj.size();
                        int n3 = n2 + C.adj.size();
                        int n4 = n3 + D.adj.size();

                        offsetMultipole(BB, n1);
                        offsetMultipole(CBp, n2);
                        offsetMultipole(DB, n3);
                        offsetMultipole(EB, n4);

                        auto CA = get2Connectors(AB);
                        auto CB = get2Connectors(BB);
                        auto CC = get2Connectors(CBp);
                        auto CD = get2Connectors(DB);
                        auto CE = get2Connectors(DB);

                        glue2connectors(AB, BB, CA[0], CB[1], 0, n1);
                        glue2connectors(BB, CBp, CB[0], CC[1], n1, n2);
                        glue2connectors(CBp, DB, CC[0], CD[1], n2, n3);
                        glue2connectors(DB, EB, CD[0], CE[1], n3, n4);

                        connectRest(AB, BB, CBp, DB, EB, CA[1], CE[0],
                                    n4 + E.adj.size(), n1, n2, n3, n4);

                        remove2connectors(AB);
                        remove2connectors(BB);
                        remove2connectors(CBp);
                        remove2connectors(DB);
                        Graph g;

                        g = AB.adj;
                        g.insert(g.end(), BB.adj.begin(), BB.adj.end());
                        g.insert(g.end(), CBp.adj.begin(), CBp.adj.end());
                        g.insert(g.end(), DB.adj.begin(), DB.adj.end());
                        g.insert(g.end(), EB.adj.begin(), EB.adj.end());

                        // checkGraph(g);
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
    int graph_counter = 0;
    J.adj = {{1, 8, 4}, {0, 2, 3}, {1, 6, 10}, {1, 7, 11},  {0, 5}, {4, 6, 7},
             {2, 5},    {3, 5},    {0, 9},     {8, 10, 11}, {2, 9}, {3, 9}};
    J.semiedges = {{7, 0}, {4, 0}, {6, 0}, {11, 1}, {8, 1}, {10, 1}};
    std::vector<Graph> negators = read_ba_file("negators_unique.ba");
    std::vector<Multipole> negators2;
    for (Graph& negator : negators) {
        negators2.push_back(fromAdjWithLeafSemiedges(negator));
    }
    makeSnarks(negators2);
}