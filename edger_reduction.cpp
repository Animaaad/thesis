#include <bits/stdc++.h>
using namespace std;
namespace fs = std::filesystem;


using pii = pair<int,int>;

vector<pii> extract_pairs_until(ifstream& file, string& line, const string& stop1, const string& stop2) {
    vector<pii> pairs;
    regex r(R"(\[\s*(\d+)\s*,\s*(\d+)\s*\])");
    smatch m;

    while (true) {
        string s = line;

        while (regex_search(s, m, r)) {
            pairs.emplace_back(stoi(m[1]), stoi(m[2]));
            s = m.suffix();
        }

        if (line.find(stop1) != string::npos || line.find(stop2) != string::npos)
            break;

        if (!getline(file, line)) break;
    }

    return pairs;
}

struct GraphData {
    vector<pii> kriticke;
    vector<pii> nekriticke;
    vector<pii> kokriticke;
    vector<pii> nekokriticke;
};

vector<GraphData> parse_file(const string& filename) {
    ifstream file(filename);
    vector<GraphData> result;

    string line;

    while (getline(file, line)) {

        if (line.find("TESTOVANIE GRAFU") != string::npos) {

            GraphData gdata;

            while (getline(file, line)) {

                // 🔹 FIRST BLOCK (adjacent)
                if (line.find("mnozina dvojic susednych") != string::npos) {

                    // move to first line with '['
                    while (getline(file, line) && line.find('[') == string::npos);

                    auto pairs = extract_pairs_until(file, line, "je kriticky", "nie je kriticky");

                    if (line.find("je kriticky") != string::npos) {
                        gdata.kriticke = pairs;
                    } else {
                        gdata.nekriticke = pairs;
                    }
                }

                // 🔹 SECOND BLOCK (non-adjacent)
                if (line.find("mnozina dvojic nesusednych") != string::npos) {

                    while (getline(file, line) && line.find('[') == string::npos);

                    auto pairs = extract_pairs_until(file, line, "je kokriticky", "nie je kokriticky");

                    if (line.find("je kokriticky") != string::npos) {
                        gdata.kokriticke = pairs;
                    } else {
                        gdata.nekokriticke = pairs;
                    }

                    // finished graph
                    result.push_back(gdata);
                    break;
                }
            }
        }
    }

    return result;
}


int main() {
    string folder = "results";

    vector<GraphData> all_graphs;

    for (const auto& entry : fs::directory_iterator(folder)) {
        if (!entry.is_regular_file()) continue;

        cout << "Reading: " << entry.path() << "\n";

        auto graphs = parse_file(entry.path().string());

        // append to global result
        all_graphs.insert(all_graphs.end(), graphs.begin(), graphs.end());
    }

    cout << "\nTotal graphs parsed: " << all_graphs.size() << "\n";

    // 🔹 optional debug output
    for (int i = 0; i < (int)all_graphs.size(); i++) {
        cout << "\nGraph " << i+1 << ":\n";

        cout << "  kriticke pairs: " << all_graphs[i].kriticke.size() << "\n";
        cout << "  nekriticke pairs: " << all_graphs[i].nekriticke.size() << "\n";
        cout << "  kokriticke pairs: " << all_graphs[i].kokriticke.size() << "\n";
        cout << "  nekokriticke pairs: " << all_graphs[i].nekokriticke.size() << "\n";
    }

    return 0;
}