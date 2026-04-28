#include <bits/stdc++.h>
using namespace std;
namespace fs = std::filesystem;

using Graph = std::vector<std::vector<int>>;

using pii = pair<int, int>;

vector<pii> extract_pairs_until(ifstream& file, string& line,
                                const string& stop1, const string& stop2) {
    vector<pii> pairs;
    regex r(R"(\[\s*(\d+)\s*,\s*(\d+)\s*\])");
    smatch m;

    while (true) {
        string s = line;

        while (regex_search(s, m, r)) {
            pairs.emplace_back(stoi(m[1]), stoi(m[2]));
            s = m.suffix();
        }

        if (line.find(stop1) != string::npos ||
            line.find(stop2) != string::npos)
            break;

        if (!getline(file, line)) break;
    }

    return pairs;
}

struct GraphData {
    vector<pii> kriticke;
    vector<pii> kokriticke;
};

vector<GraphData> parse_file(const string& filename) {
    ifstream file(filename);
    vector<GraphData> result;

    string line;

    while (getline(file, line)) {
        if (line.find("graf je snark") != string::npos) {
            GraphData gdata;

            while (getline(file, line)) {
                // 🔹 FIRST BLOCK (adjacent)
                if (line.find("TESTOVANIE") != string::npos ||
                    line.find("pocet testovanych grafov") != string::npos) {
                    if (gdata.kriticke.empty()) {
                        gdata.kokriticke = {{0, 0}};
                    }
                    result.push_back(gdata);
                    break;
                }
                if (line.find("mnozina dvojic susednych vrcholov, ktorych") !=
                    string::npos) {
                    // move to first line with '['
                    while (getline(file, line) &&
                           line.find('[') == string::npos);

                    auto pairs = extract_pairs_until(file, line, "je kriticky",
                                                     "nie je kriticky");

                    if (line.find("je kriticky") != string::npos) {
                        gdata.kriticke = pairs;
                    }
                }

                // 🔹 SECOND BLOCK (non-adjacent)
                if (line.find("mnozina dvojic nesusednych vrcholov, ktorych") !=
                    string::npos) {
                    // move to first line with '['
                    while (getline(file, line) &&
                           line.find('[') == string::npos);

                    auto pairs = extract_pairs_until(file, line, "je kokriticky",
                                                     "nie je kokriticky");

                    if (line.find("je kokriticky") != string::npos) {
                        gdata.kokriticke = pairs;
                        result.push_back(gdata);
                        break;
                    }
                }
            }
        }
    }

    return result;
}

Graph reduce(Graph g, vector<pair<int, int>> edges) {
    int n = g.size();

    // maps original vertices → current vertices
    vector<int> repr(n);
    iota(repr.begin(), repr.end(), 0);

    for (auto [orig_u, orig_v] : edges) {
        int u = repr[orig_u];
        int v = repr[orig_v];

        if (u == v) continue;  // already merged

        // 🔹 find neighbors (excluding each other)
        vector<int> nu, nv;

        for (int x : g[u])
            if (x != v) nu.push_back(x);
        for (int x : g[v])
            if (x != u) nv.push_back(x);

        if (nu.size() != 2 || nv.size() != 2) {
            throw runtime_error("Invalid cubic structure");
        }

        int a = nu[0], b = nu[1];
        int c = nv[0], d = nv[1];

        // 🔹 choose representatives (IMPORTANT)
        int rep_u = a;
        int rep_v = c;

        // 🔹 build new graph (remove u, v)
        int old_n = g.size();
        vector<int> new_id(old_n, -1);

        int idx = 0;
        for (int i = 0; i < old_n; i++) {
            if (i == u || i == v) continue;
            new_id[i] = idx++;
        }

        vector<vector<int>> new_g(old_n - 2);

        // copy edges
        for (int i = 0; i < old_n; i++) {
            if (i == u || i == v) continue;

            for (int j : g[i]) {
                if (j == u || j == v) continue;
                new_g[new_id[i]].push_back(new_id[j]);
            }
        }

        // helper
        auto add_edge = [&](int x, int y) {
            if (x == y) return;
            if (find(new_g[x].begin(), new_g[x].end(), y) == new_g[x].end()) {
                new_g[x].push_back(y);
                new_g[y].push_back(x);
            }
        };

        // smoothing edges
        add_edge(new_id[a], new_id[b]);
        add_edge(new_id[c], new_id[d]);

        // 🔹 update repr mapping
        for (int i = 0; i < n; i++) {
            if (repr[i] == u) repr[i] = rep_u;
            if (repr[i] == v) repr[i] = rep_v;
        }

        // 🔹 reindex repr
        for (int i = 0; i < n; i++) {
            if (repr[i] == u || repr[i] == v) continue;
            repr[i] = new_id[repr[i]];
        }

        // 🔹 move graph
        g = move(new_g);
    }

    return g;
}

int main() {
    string folder = "criticality";

    vector<GraphData> all_graphs;

    int graph_counter = 0;

    std::ofstream file("snarks.ba");

    vector<fs::path> filesA, filesB;

    for (auto& e : fs::directory_iterator("criticality"))
        if (e.is_regular_file()) filesA.push_back(e.path());

    for (auto& e : fs::directory_iterator("32a"))
        if (e.is_regular_file()) filesB.push_back(e.path());

    // sort to align
    sort(filesA.begin(), filesA.end());
    sort(filesB.begin(), filesB.end());

    for (int i = 0; i < filesA.size(); i++) {
        fs::path entry = filesA[i];

        cout << "Reading: " << entry.string() << "\n";

        auto graphs = parse_file(entry.string());

        std::ifstream in(filesB[i]);
        if (!in) throw std::runtime_error("Cannot open file");
        int idx;
        int n;
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        in >> n;
        in.ignore(std::numeric_limits<std::streamsize>::max(),
                  '\n');  // clear line

        std::vector<Graph> graphs2;
        graphs2.reserve(n);

        for (int g = 0; g < graphs.size(); ++g) {
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

            graphs2.push_back(adj);
        }

        for (int i = 0; i < graphs.size(); i++) {
            if (graphs[i].kriticke.size() <= 2 && !graphs[i].kriticke.empty() &&
                graphs[i].kokriticke.empty()) {
                Graph g = graphs2[i];

                g = reduce(g, graphs[i].kriticke);

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

        // append to global result
        all_graphs.insert(all_graphs.end(), graphs.begin(), graphs.end());
    }

    cout << "\nTotal graphs parsed: " << all_graphs.size() << "\n";

    // 🔹 optional debug output
    for (int i = 0; i < (int)all_graphs.size(); i++) {
        cout << "\nGraph " << i + 1 << ":\n";

        cout << "  kriticke pairs: " << all_graphs[i].kriticke.size() << "\n";
        cout << "  kokriticke pairs: " << all_graphs[i].kokriticke.size()
             << "\n";
    }

    return 0;
}