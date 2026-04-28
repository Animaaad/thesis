#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <fstream>
using namespace std;

vector<vector<int>> g6_to_adjlist(const string &g6) {
    int pos = 0;
    int n = 0;

    // Decode number of vertices
    if (g6[pos] != '~') {
        n = g6[pos++] - 63;
    } else {
        pos++; // skip '~'
        n = 0;
        for (int i = 0; i < 3; ++i) {
            n = (n << 6) | (g6[pos++] - 63);
        }
    }

    vector<vector<int>> adj(n);

    int i = 0, j = 1;
    int bit_pos = 0;
    int current = 0;

    while (j < n) {
        if (bit_pos == 0) {
            current = g6[pos++] - 63;
            bit_pos = 6;
        }

        bit_pos--;
        int bit = (current >> bit_pos) & 1;

        if (bit) {
            adj[i].push_back(j);
            adj[j].push_back(i);
        }

        i++;
        if (i == j) {
            i = 0;
            j++;
        }
    }

    return adj;
}

vector<vector<vector<int>>> read_g6_file(const string &filename) {
    ifstream infile(filename);
    if (!infile) {
        throw runtime_error("Cannot open file: " + filename);
    }

    vector<vector<vector<int>>> graphs;
    string line;
    int smth = 0;
    while (getline(infile, line)) {
        
        smth++;
        // Trim whitespace
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#')
            continue;

        graphs.push_back(g6_to_adjlist(line));
    }

    return graphs;
}/**/


int main() {
    
    try {
        int graph_count = 0;
        std::ofstream out("rofl.ba");
        for (int i = 4; i <= 4; i += 2) {
            std::string filename = "snarks_unique.g6";
            auto graphs = read_g6_file(filename);
            cout << "Read " << graphs.size() << " graphs\n";
            out << graphs.size() << "\n";
            for (int g = 0; g < graphs.size(); ++g) {
                graph_count++;
                out << graph_count << "\n" << graphs[g].size() << "\n";
                for (int i = 0; i < graphs[g].size(); ++i) {
                    for (int v : graphs[g][i]) out << v << " ";
                    out << "\n";
                }
            }
        }
    } catch (const exception &e) {
        cerr << e.what() << "\n";
    }/**/
}
