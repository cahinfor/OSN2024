//
// Created by cahInfor on 10/09/25.
//

// judge.cpp
// Batch judge: compares ./outputs/output_<id>.out against ./inputs/input_<id>.in
// Scans both directories, matches files by <id>, simulates painting, prints per-case score.

#include <bits/stdc++.h>
#include <dirent.h>
#include <cmath>
using namespace std;

struct CasePaths {
    int id;
    string input_path;
    string output_path;
};

static void paint(int N, int M, vector<string>& s, const string& d, int x, char c) {
    if (d == "ROW" || d == "BARIS") {
        if (x < 1 || x > N) return;
        for (int j = 0; j < M; ++j) s[x-1][j] = c;
    } else if (d == "COLUMN" || d == "KOLOM") {
        if (x < 1 || x > M) return;
        for (int i = 0; i < N; ++i) s[i][x-1] = c;
    }
}

static bool read_target(const string& path, int& N, int& M, vector<string>& target) {
    ifstream in(path.c_str());
    if (!in) return false;
    if (!(in >> N >> M)) return false;
    target.assign(N, string());
    for (int i = 0; i < N; ++i) {
        string row; if (!(in >> row)) return false;
        if ((int)row.size() != M) return false;
        target[i] = row;
    }
    return true;
}

static bool read_commands(const string& path, vector<tuple<string,int,char>>& cmds) {
    ifstream in(path.c_str());
    if (!in) return false;
    int K; if (!(in >> K)) return false;
    cmds.clear(); cmds.reserve(K);
    for (int i = 0; i < K; ++i) {
        string dir; int x; char c;
        if (!(in >> dir >> x >> c)) return false;
        cmds.emplace_back(dir, x, c);
    }
    return true;
}

static double judge_one(const string& in_path, const string& out_path, bool print_grid) {
    int N, M;
    vector<string> target;
    if (!read_target(in_path, N, M, target)) {
        cerr << "[ERR] Failed to read input: " << in_path << "\n";
        return 0.0;
    }

    vector<tuple<string,int,char>> cmds;
    if (!read_commands(out_path, cmds)) {
        cerr << "[ERR] Failed to read output: " << out_path << "\n";
        return 0.0;
    }

    vector<string> board(N, string(M, '.'));

    for (auto& t : cmds) {
        const string& dir = get<0>(t);
        int x = get<1>(t);
        char c = get<2>(t);
        paint(N, M, board, dir, x, c);
    }

    if (print_grid) {
        for (int i = 0; i < N; ++i) cout << board[i] << "\n";
    }

    long long match = 0, total = 1LL * N * M;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            if (board[i][j] == target[i][j]) ++match;

    double frac = (double)match / (double)total;          // avoid integer division
    double score = pow(frac, 3.0) * 20.0;                 // same formula as yours
    cout << fixed << setprecision(6) << score << "\n";
    return score;
}

static void collect_ids(const string& dir, const string& prefix, const string& ext, unordered_map<int,string>& out) {
    regex pat("^" + prefix + "_(\\d+)\\." + ext + "$");
    if (DIR* d = opendir(dir.c_str())) {
        dirent* ent;
        while ((ent = readdir(d)) != nullptr) {
            string name = ent->d_name;
            smatch m;
            if (regex_match(name, m, pat)) {
                int id = stoi(m[1].str());
                out[id] = dir + "/" + name;
            }
        }
        closedir(d);
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const string in_dir  = "./inputs";
    const string out_dir = "./outputs";

    // Map id -> path for inputs and outputs
    unordered_map<int,string> in_map, out_map;
    collect_ids(in_dir,  "mosaik",  "in",  in_map);
    collect_ids(out_dir, "mosaik", "out", out_map);

    // Build pairs that exist in both
    vector<int> ids;
    ids.reserve(in_map.size());
    for (auto& kv : in_map) {
        int id = kv.first;
        if (out_map.count(id)) ids.push_back(id);
    }
    sort(ids.begin(), ids.end());

    if (ids.empty()) {
        cerr << "[WARN] No matching input/output pairs found.\n";
        return 0;
    }

    // Judge all cases
    double total = 0.0;
    for (int id : ids) {
        // Print a header (optional); comment out if you want raw scores only
        // cout << "Case " << id << ":\n";
        // Only print the grid if you want (set to false to suppress)
        bool print_grid = false;
        total += judge_one(in_map[id], out_map[id], print_grid);
        // cout << "\n";
    }

    // Overall average (optional)
    // cout << "Average score: " << fixed << setprecision(6) << (total / ids.size()) << "\n";
    return 0;
}
