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

/* ---------- Bit helpers for arbitrary M ---------- */

static int num_chunks(const int M) { return (M + 63) >> 6; }

static bool get_bit(const vector<uint64_t>& v, int j) {
    return (v[j >> 6] >> (j & 63)) & 1ULL;
}

static void set_bit(vector<uint64_t>& v, int j) {
    v[j >> 6] |= (1ULL << (j & 63));
}

static void clear_bit(vector<uint64_t>& v, int j) {
    v[j >> 6] &= ~(1ULL << (j & 63));
}

static bool contains_all(const vector<uint64_t>& a, const vector<uint64_t>& need) {
    // Return true if (a & need) == need  <=>  (~a & need) == 0
    for (size_t k = 0; k < need.size(); ++k) {
        if ((~a[k]) & need[k]) return false;
    }
    return true;
}

/* Serialize chunk vector into a string key (fast hashing, equality) */
static string chunks_key(const vector<uint64_t>& v) {
    string s;
    s.resize(v.size() * sizeof(uint64_t));
    memcpy(&s[0], v.data(), v.size() * sizeof(uint64_t));
    return s;
}

/* ---------- Fast solver with chunked masks (works for M up to millions) ---------- */
static vector<Cmd> solve(const vector<string>& F) {
    const int N = static_cast<int>(F.size());
    const int M = N ? static_cast<int>(F[0].size()) : 0;
    if (N == 0 || M == 0) return {};

    const int C = num_chunks(M);

    // Build mask per row: bit=1 means the final needs '.' at that column
    vector rowMask(N, vector<uint64_t>(C, 0));
    vector<int> rowsWithSharp; rowsWithSharp.reserve(N);

    for (int i = 0; i < N; ++i) {
        bool anySharp = false;
        for (int j = 0; j < M; ++j) {
            if (F[i][j] == '.') set_bit(rowMask[i], j);
            else anySharp = true;
        }
        if (anySharp) rowsWithSharp.push_back(i);
    }

    if (rowsWithSharp.empty()) {
        // All dots; starting grid is dots too, so no commands.
        return {};
    }

    // Compress by mask using serialized chunk key
    unordered_map<string, int> maskId;
    maskId.reserve(rowsWithSharp.size() * 2);

    vector<vector<uint64_t>> masks; masks.reserve(rowsWithSharp.size());
    vector<int> cnt; cnt.reserve(rowsWithSharp.size());
    vector<vector<int>> rowsPerMask; rowsPerMask.reserve(rowsWithSharp.size());

    for (int r : rowsWithSharp) {
        string key = chunks_key(rowMask[r]);
        if (auto it = maskId.find(key); it == maskId.end()) {
            int id = static_cast<int>(masks.size());
            maskId.emplace(std::move(key), id);
            masks.push_back(rowMask[r]);
            cnt.push_back(1);
            rowsPerMask.push_back(vector{r});
        } else {
            int id = it->second;
            cnt[id] += 1;
            rowsPerMask[id].push_back(r);
        }
    }

    const int U = static_cast<int>(masks.size());

    // dotRemaining per column
    vector<long long> dotRemaining(M, 0);
    for (int id = 0; id < U; ++id) {
        for (int j = 0; j < M; ++j) if (get_bit(masks[id], j)) {
            dotRemaining[j] += cnt[id];
        }
    }

    // Order of rows to paint
    vector<int> order; order.reserve(rowsWithSharp.size());

    // Fbits: columns that still need any '.'
    vector<uint64_t> Fbits(C, 0);
    for (int j = 0; j < M; ++j) if (dotRemaining[j] > 0) set_bit(Fbits, j);

    vector<char> done(U, 0);
    bool progressed = true;
    while (progressed) {
        progressed = false;
        for (int id = 0; id < U; ++id) if (!done[id] && cnt[id] > 0) {
            if (contains_all(masks[id], Fbits)) {
                // Emit all rows with this mask
                for (int r : rowsPerMask[id]) order.push_back(r);

                // Update counts and Fbits
                int take = cnt[id];
                cnt[id] = 0;
                done[id] = 1;

                for (int j = 0; j < M; ++j) if (get_bit(masks[id], j)) {
                    dotRemaining[j] -= take;
                    if (dotRemaining[j] == 0) clear_bit(Fbits, j);
                }
                progressed = true;
            }
        }
    }
    // Safety (shouldn't be needed if guaranteed solvable)
    for (int id = 0; id < U; ++id) if (cnt[id] > 0) {
        for (int r : rowsPerMask[id]) order.push_back(r);
        cnt[id] = 0;
    }

    // Place KOLOM after the last row that needs '.' in that column
    vector lastPos(M, -1);
    for (int idx = 0; idx < static_cast<int>(order.size()); ++idx) {
        int r = order[idx];
        for (int j = 0; j < M; ++j) if (F[r][j] == '.') lastPos[j] = idx;
    }

    vector<vector<int>> buckets(order.size()); // columns to set '.' after row index
    for (int j = 0; j < M; ++j) if (lastPos[j] != -1) buckets[lastPos[j]].push_back(j);

    // Emit commands: BARIS then KOLOM bucket
    vector<Cmd> cmds;
    cmds.reserve(order.size() + M);
    for (int k = 0; k < static_cast<int>(order.size()); ++k) {
        if (int r = order[k]; F[r].find('#') != string::npos) cmds.push_back({"BARIS", r + 1, '#'});
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
            smatch m;
            if (regex_match(name, m, pat)) {
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
        string outpath = oss.str();

        if (!write_commands_to_file(outpath, cmds)) {
            cerr << "Failed to write " << outpath << "\n";
        }
    }
    return 0;
}
