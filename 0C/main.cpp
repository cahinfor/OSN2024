#include <bits/stdc++.h>
#include <dirent.h>
#include <sys/stat.h>
using namespace std;

struct Cmd { string type; int idx; char ch; };

static void ensure_dir(const string& dir) {
    struct stat st{};
    if (stat(dir.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) return;
    mkdir(dir.c_str(), 0755);
}

static bool read_grid_from_file(const string& path, vector<string>& F) {
    ifstream in(path.c_str());
    if (!in) return false;
    int N, M;
    if (!(in >> N >> M)) return false;
    F.assign(N, string());
    for (int i = 0; i < N; ++i) {
        string row; in >> row;
        if (static_cast<int>(row.size()) != M) return false;
        F[i] = row;
    }
    return true;
}

static bool write_commands_to_file(const string& path, const vector<Cmd>& cmds) {
    ofstream out(path.c_str());
    if (!out) return false;
    out << static_cast<int>(cmds.size()) << "\n";
    for (const auto &[type, idx, ch] : cmds) out << type << ' ' << idx << ' ' << ch << "\n";
    return true;
}

static vector<Cmd> solve(const vector<string>& Grid) {
    const int N = static_cast<int>(Grid.size());
    const int M = N ? static_cast<int>(Grid[0].size()) : 0;

    // Collect rows that have at least one '#'
    vector<int> rowsWithSharp;
    rowsWithSharp.reserve(N);
    for (int i = 0; i < N; ++i) {
        if (Grid[i].find('#') != string::npos) {
            rowsWithSharp.push_back(i);
        }
    }
    if (rowsWithSharp.empty()) {
        return {};
    }

    // Group rows with identical strings
    unordered_map<string,int> maskId;
    maskId.reserve(rowsWithSharp.size() * 2);

    vector<string> masks;
    vector<int> counter;
    vector<vector<int>> rowsPerMask;

    for (int row : rowsWithSharp) {
        const string& key = Grid[row];
        auto it = maskId.find(key);
        if (it == maskId.end()) {
            int id = masks.size();
            maskId[key] = id;
            masks.push_back(key);
            counter.push_back(1);
            rowsPerMask.push_back(vector<int>{row});
        } else {
            int id = it->second;
            counter[id]++;
            rowsPerMask[id].push_back(row);
        }
    }

    int U = masks.size();

    // Count how many rows need '.' in each column
    vector<long long> dotRemaining(M, 0);
    for (int id = 0; id < U; ++id) {
        for (int j = 0; j < M; ++j)
            if (masks[id][j] == '.') {
                dotRemaining[j] += counter[id];
            }
    }

    // Fbits as string: mark columns that still need any '.'
    string Fbits(M, '0');
    for (int j = 0; j < M; ++j)
        if (dotRemaining[j] > 0) Fbits[j] = '1';

    vector<char> done(U, 0);
    bool progressed = true;
    vector<int> order; order.reserve(rowsWithSharp.size());

    while (progressed) {
        progressed = false;
        for (int id = 0; id < U; ++id) if (!done[id] && counter[id] > 0) {
            // check if this mask has '.' in all columns where Fbits[j]=='1'
            bool ok = true;
            for (int j = 0; j < M; ++j) {
                if (Fbits[j] == '1' && masks[id][j] != '.') {
                    ok = false; break;
                }
            }
            if (ok) {
                // emit all rows with this mask
                for (int r : rowsPerMask[id]) order.push_back(r);

                int take = counter[id];
                counter[id] = 0;
                done[id] = 1;

                // update dotRemaining and Fbits
                for (int j = 0; j < M; ++j) if (masks[id][j] == '.') {
                    dotRemaining[j] -= take;
                    if (dotRemaining[j] == 0) Fbits[j] = '0';
                }
                progressed = true;
            }
        }
    }

    // Safety
    for (int id = 0; id < U; ++id) if (counter[id] > 0) {
        for (int r : rowsPerMask[id]) order.push_back(r);
        counter[id] = 0;
    }

    // Decide where to put KOLOM
    vector<int> lastPos(M, -1);
    for (int idx = 0; idx < (int)order.size(); ++idx) {
        int r = order[idx];
        for (int j = 0; j < M; ++j)
            if (Grid[r][j] == '.') lastPos[j] = idx;
    }

    vector<vector<int>> buckets(order.size());
    for (int j = 0; j < M; ++j)
        if (lastPos[j] != -1) buckets[lastPos[j]].push_back(j);

    // Emit commands
    vector<Cmd> cmds;
    cmds.reserve(order.size() + M);
    for (int k = 0; k < (int)order.size(); ++k) {
        int r = order[k];
        cmds.push_back({"BARIS", r + 1, '#'}); // row had at least one '#'
        if (!buckets[k].empty()) {
            ranges::sort(buckets[k]);
            for (int j : buckets[k]) cmds.push_back({"KOLOM", j + 1, '.'});
        }
    }

    return cmds;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const string input_dir = "./inputs";
    const string output_dir = "./outputs";
    ensure_dir(output_dir);

    // Collect inputs: mosaik_<number>.in
    vector<pair<int,string>> jobs;
    regex pat(R"(mosaik_(\d+)\.in)");
    if (DIR* d = opendir(input_dir.c_str())) {
        dirent* ent;
        while ((ent = readdir(d)) != nullptr) {
            string name = ent->d_name;
            if (smatch m; regex_match(name, m, pat)) {
                int id = stoi(m[1].str());
                jobs.emplace_back(id, input_dir + "/" + name);
            }
        }
        closedir(d);
    }
    ranges::sort(jobs);

    for (auto &job : jobs) {
        int id; string inpath;
        tie(id, inpath) = job;

        vector<string> F;
        if (!read_grid_from_file(inpath, F)) {
            cerr << "Failed to read " << inpath << "\n";
            continue;
        }

        auto cmds = solve(F);

        ostringstream oss;
        oss << output_dir << "/mosaik_" << id << ".out";

        if (string outpath = oss.str(); !write_commands_to_file(outpath, cmds)) {
            cerr << "Failed to write " << outpath << "\n";
        }
    }
    return 0;
}
