#include <bits/stdc++.h>
#include <dirent.h>
#include <sys/stat.h>
using namespace std;

struct Cmd { string type; int idx; char ch; };

static void ensure_dir(const string& dir) {
    struct stat st;
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
        if ((int)row.size() != M) return false;
        F[i] = row;
    }
    return true;
}

static bool write_commands_to_file(const string& path, const vector<Cmd>& cmds) {
    ofstream out(path.c_str());
    if (!out) return false;
    out << (int)cmds.size() << "\n";
    for (auto &c : cmds) out << c.type << ' ' << c.idx << ' ' << c.ch << "\n";
    return true;
}

// Fast solver using mask-group ordering (no dense DAG)
static vector<Cmd> solve_one_fast(const vector<string>& F) {
    const int N = (int)F.size();
    const int M = N ? (int)F[0].size() : 0;

    // Build mask per row (bit = 1 means wants '.')
    vector<uint32_t> rowMask(N, 0);
    vector<int> rowsWithSharp; rowsWithSharp.reserve(N);
    for (int i = 0; i < N; ++i) {
        uint32_t m = 0;
        bool anySharp = false;
        for (int j = 0; j < M; ++j) {
            if (F[i][j] == '.') m |= (1u << j);
            else anySharp = true;
        }
        rowMask[i] = m;
        if (anySharp) rowsWithSharp.push_back(i);
    }

    if (rowsWithSharp.empty()) {
        // All dots already; starting grid is dots, so nothing to do.
        return {};
    }

    // Compress by mask
    unordered_map<uint32_t, int> maskId;
    maskId.reserve(rowsWithSharp.size()*2);
    vector<uint32_t> masks; masks.reserve(rowsWithSharp.size());
    vector<int> cnt; cnt.reserve(rowsWithSharp.size());
    vector<vector<int>> rowsPerMask; rowsPerMask.reserve(rowsWithSharp.size());

    for (int r : rowsWithSharp) {
        uint32_t m = rowMask[r];
        auto it = maskId.find(m);
        if (it == maskId.end()) {
            int id = (int)masks.size();
            maskId[m] = id;
            masks.push_back(m);
            cnt.push_back(1);
            rowsPerMask.push_back(vector<int>{r});
        } else {
            int id = it->second;
            cnt[id] += 1;
            rowsPerMask[id].push_back(r);
        }
    }

    const int U = (int)masks.size();

    // dotRemaining per column
    vector<long long> dotRemaining(M, 0);
    for (int id = 0; id < U; ++id) {
        uint32_t m = masks[id];
        for (int j = 0; j < M; ++j) if (m & (1u << j)) {
            dotRemaining[j] += cnt[id];
        }
    }

    // Build the processing order of rows (indices into original F)
    vector<int> order; order.reserve(rowsWithSharp.size());

    // Fbits: columns that still have any dot remaining (1 = still need dots)
    uint32_t Fbits = 0;
    for (int j = 0; j < M; ++j) if (dotRemaining[j] > 0) Fbits |= (1u << j);

    vector<char> done(U, 0);
    bool progressed = true;
    while (progressed) {
        progressed = false;

        // Scan all masks; pick any mask available: (m & Fbits) == Fbits
        for (int id = 0; id < U; ++id) if (!done[id] && cnt[id] > 0) {
            uint32_t m = masks[id];
            if ( (m & Fbits) == Fbits ) {
                // Emit all rows of this mask (any order is fine)
                for (int r : rowsPerMask[id]) order.push_back(r);

                // Update counts
                int take = cnt[id];
                cnt[id] = 0;
                done[id] = 1;
                for (int j = 0; j < M; ++j) if (m & (1u << j)) {
                    dotRemaining[j] -= take;
                    if (dotRemaining[j] == 0) {
                        Fbits &= ~(1u << j); // this column no longer forces dot
                    }
                }
                progressed = true;
                // Keep scanning; Fbits may have shrunk, unlocking more masks.
            }
        }
    }

    // Safety: if some masks remained (shouldn't happen for valid mosaics), append them anyway.
    for (int id = 0; id < U; ++id) if (cnt[id] > 0) {
        for (int r : rowsPerMask[id]) order.push_back(r);
        cnt[id] = 0;
    }

    // Build placement of KOLOM after last row needing '.' in that column
    vector<int> lastPos(M, -1);
    vector<int> pos(N, -1);
    for (int i = 0; i < (int)order.size(); ++i) pos[order[i]] = i;
    for (int j = 0; j < M; ++j) {
        int lp = -1;
        for (int idx = 0; idx < (int)order.size(); ++idx) {
            int r = order[idx];
            if (F[r][j] == '.') lp = idx;
        }
        lastPos[j] = lp; // -1 means no dots needed in this column
    }

    vector<vector<int>> buckets(order.size()); // columns to set '.' after a certain row index
    for (int j = 0; j < M; ++j) if (lastPos[j] != -1) buckets[lastPos[j]].push_back(j);

    // Emit commands
    vector<Cmd> cmds;
    cmds.reserve(order.size() + M);
    for (int k = 0; k < (int)order.size(); ++k) {
        int r = order[k];
        // Only paint rows that actually need any '#'
        if (F[r].find('#') != string::npos) cmds.push_back({"BARIS", r + 1, '#'});
        if (!buckets[k].empty()) {
            sort(buckets[k].begin(), buckets[k].end());
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

    // Collect inputs: input_<number>.in
    vector<pair<int,string>> jobs;
    regex pat(R"(mosaik_(\d+)\.in)");
    if (DIR* d = opendir(input_dir.c_str())) {
        if (d) {
            dirent* ent;
            while ((ent = readdir(d)) != nullptr) {
                string name = ent->d_name;
                smatch m;
                if (regex_match(name, m, pat)) {
                    int id = stoi(m[1].str());
                    jobs.emplace_back(id, input_dir + "/" + name);
                }
            }
            closedir(d);
        }
    }
    sort(jobs.begin(), jobs.end());

    for (auto &job : jobs) {
        int id; string inpath;
        tie(id, inpath) = job;

        vector<string> F;
        if (!read_grid_from_file(inpath, F)) {
            cerr << "Failed to read " << inpath << "\n";
            continue;
        }

        auto cmds = solve_one_fast(F);

        ostringstream oss;
        oss << output_dir << "/mosaik_" << id << ".out";
        string outpath = oss.str();

        if (!write_commands_to_file(outpath, cmds)) {
            cerr << "Failed to write " << outpath << "\n";
        }
    }
    return 0;
}
