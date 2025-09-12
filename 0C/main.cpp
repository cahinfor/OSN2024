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

    vector<int> rowWhiteCount, colWhiteCount;
    for (int i = 0; i < N; ++i) {
        int whiteCell = 0;
        for (int j = 0; j < M; ++j)
            if (Grid[i][j] == '.') ++whiteCell;
        rowWhiteCount.push_back(whiteCell);
    }
    vector<int> row_idx(N);
    iota(row_idx.begin(), row_idx.end(), 0);
    ranges::sort(row_idx, [&](int a, int b){
        if (rowWhiteCount[a] != rowWhiteCount[b]) return rowWhiteCount[a] > rowWhiteCount[b]; // DESC
        return a < b; // tie-break by smaller index
    });

    for (int i = 0; i < M; ++i) {
        int whiteCell = 0;
        for (int j = 0; j < N; ++j)
            if (Grid[j][i] == '.') ++whiteCell;
        colWhiteCount.push_back(whiteCell);
    }
    vector<int> col_idx(M);
    iota(col_idx.begin(), col_idx.end(), 0);
    ranges::sort(col_idx, [&](int a, int b){
        if (colWhiteCount[a] != colWhiteCount[b]) return colWhiteCount[a] < colWhiteCount[b]; // ASC
        return a < b; // tie-break by smaller index
    });

    vector<Cmd> cmds;
    int i = 0;
    int j = 0;
    while (i < rowWhiteCount.size() || j < colWhiteCount.size()) {
        if (i < N) {
            int currentRowCount = rowWhiteCount[row_idx[i]];
            int k = i;
            while (k < N && rowWhiteCount[row_idx[k]] == currentRowCount) ++k;

            // rows [i, k) all have the same highest remaining count
            for (int t = i; t < k; ++t) {
                int rowIndex = row_idx[t] + 1; // 1-based
                cmds.push_back({"BARIS", rowIndex, '#'});
            }
            i = k; // advance to next "tier"
        }

        if (j < M) {
            while (colWhiteCount[col_idx[j]] == 0) ++j; // skip if the col has no '.'
            int currentColCount = colWhiteCount[col_idx[j]];
            int k = j;
            while (k < M && colWhiteCount[col_idx[k]] == currentColCount) ++k;

            // cols [j, k) all have the same lowest remaining count
            for (int t = j; t < k; ++t) {
                int colIndex = col_idx[t] + 1; // 1-based
                cmds.push_back({"KOLOM", colIndex, '.'});
            }
            j = k; // advance to next "tier"
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
