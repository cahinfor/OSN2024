//
// Created by cahInfor on 10/09/25.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>
#include <dirent.h>

using namespace std;

// Represents a single coloring operation on a row or column.
struct Operation {
    string type;
    int index;
    char color;
};

// The core logic to solve the problem for a single grid, given a strategy.
// Strategy 1: Rows then Columns
// Strategy 2: Columns then Rows
// ----------------- NEW SOLVER (drop-in) -----------------
static pair<long long, vector<Operation>>
solve_only_hash_union(int N, int M, const vector<string>& S,
                      bool bias_hash_on_tie, bool init_rows_by_majority) {
    // Build sparse lists of '#' positions
    vector<vector<int>> rows_hash_cols(N);
    vector<vector<int>> cols_hash_rows(M);
    vector<int> row_hash_cnt(N, 0), col_hash_cnt(M, 0);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (S[i][j] == '#') {
                rows_hash_cols[i].push_back(j);
                cols_hash_rows[j].push_back(i);
                row_hash_cnt[i]++;
                col_hash_cnt[j]++;
            }
        }
    }

    // Decisions: R[i]=1 if paint row i '#', C[j]=1 if paint col j '#'
    vector<unsigned char> R(N, 0), C(M, 0);

    // Optional initialization: set R by row-majority
    if (init_rows_by_majority) {
        for (int i = 0; i < N; ++i) {
            // If we paint row i -> whole row becomes '#', matches = row_hash_cnt[i]
            // If we don't -> matches come only from columns later; for init we can start simple
            // Here we just seed by whether row has majority '#'
            if (row_hash_cnt[i] * 2 >= M) R[i] = 1;
        }
    }

    vector<int> col_hash_in_R1(M, 0), row_hash_in_C1(N, 0);

    auto update_col_hash_in_R1 = [&]() {
        fill(col_hash_in_R1.begin(), col_hash_in_R1.end(), 0);
        for (int i = 0; i < N; ++i) if (R[i]) {
            for (int j : rows_hash_cols[i]) ++col_hash_in_R1[j];
        }
    };
    auto update_row_hash_in_C1 = [&]() {
        fill(row_hash_in_C1.begin(), row_hash_in_C1.end(), 0);
        for (int j = 0; j < M; ++j) if (C[j]) {
            for (int i : cols_hash_rows[j]) ++row_hash_in_C1[i];
        }
    };

    const int MAX_ITERS = 12;
    for (int it = 0; it < MAX_ITERS; ++it) {
        bool changed = false;

        // Optimize columns given rows
        update_col_hash_in_R1();
        int r1cnt = 0; for (int i = 0; i < N; ++i) r1cnt += R[i];
        int r0cnt = N - r1cnt;

        for (int j = 0; j < M; ++j) {
            // If we set C[j]=1, whole column predicted '#': matches = # in column = col_hash_cnt[j]
            int matches_if_C1 = col_hash_cnt[j];

            // If C[j]=0, prediction equals R[i] per row.
            // Matches from column j:
            // = (# rows with R=1 and S='#') + (# rows with R=0 and S='.')
            // = col_hash_in_R1[j] + (r0cnt - (# rows with R=0 and S='#'))
            // (# rows with R=0 and S='#') = col_hash_cnt[j] - col_hash_in_R1[j]
            // => matches_if_C0 = r0cnt - col_hash_cnt[j] + 2 * col_hash_in_R1[j]
            int matches_if_C0 = r0cnt - col_hash_cnt[j] + (col_hash_in_R1[j] << 1);

            bool choose1 = (matches_if_C1 > matches_if_C0) ||
                           (bias_hash_on_tie && matches_if_C1 == matches_if_C0);
            unsigned char newC = choose1 ? 1 : 0;
            if (newC != C[j]) { C[j] = newC; changed = true; }
        }

        // Optimize rows given columns
        update_row_hash_in_C1();
        int c1cnt = 0; for (int j = 0; j < M; ++j) c1cnt += C[j];
        int c0cnt = M - c1cnt;

        for (int i = 0; i < N; ++i) {
            // If R[i]=1, whole row predicted '#': matches = row_hash_cnt[i]
            int matches_if_R1 = row_hash_cnt[i];

            // If R[i]=0, prediction equals C[j]
            // Matches on row i:
            // = (# cols with C=1 and S='#') + (# cols with C=0 and S='.')
            // Let a = row_hash_in_C1[i] = '# in row i at C=1'
            // Then (# of '#' at C=0) = row_hash_cnt[i] - a
            // matches_if_R0 = a + (c0cnt - (row_hash_cnt[i] - a)) = c0cnt - row_hash_cnt[i] + 2a
            int a = row_hash_in_C1[i];
            int matches_if_R0 = c0cnt - row_hash_cnt[i] + (a << 1);

            bool choose1 = (matches_if_R1 > matches_if_R0) ||
                           (bias_hash_on_tie && matches_if_R1 == matches_if_R0);
            unsigned char newR = choose1 ? 1 : 0;
            if (newR != R[i]) { R[i] = newR; changed = true; }
        }

        if (!changed) break;
    }

    // Count matches and build ops
    long long matches = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            char pred = (R[i] || C[j]) ? '#' : '.';
            if (pred == S[i][j]) ++matches;
        }
    }

    vector<Operation> ops;
    ops.reserve(N + M);
    for (int i = 0; i < N; ++i) if (R[i]) ops.push_back({"BARIS", i+1, '#'});
    for (int j = 0; j < M; ++j) if (C[j]) ops.push_back({"KOLOM", j+1, '#'});
    return {matches, ops};
}

pair<long long, vector<Operation>> solve(int N, int M, const vector<string>& S, int /*strategy_type_unused*/) {
    // Try a few small variants and pick the best:
    // 1) bias to paint '#' on ties; start with empty R
    // 2) bias to paint '#' on ties; start with rows seeded by majority
    // 3) neutral tie (bias off);   start with empty R
    // 4) neutral tie (bias off);   start with rows seeded by majority
    pair<long long, vector<Operation>> best = { -1, {}};

    auto try_update = [&](bool bias, bool seed_rows) {
        auto cand = solve_only_hash_union(N, M, S, bias, seed_rows);
        if (cand.first > best.first) best = cand;
    };

    try_update(true,  false);
    try_update(true,  true);
    try_update(false, false);
    try_update(false, true);

    return best;
}
// ----------------- END NEW SOLVER -----------------

int main() {
    // Define input and output directory paths
    const string input_dir = "inputs/";
    const string output_dir = "outputs/";

    DIR *dir;
    struct dirent *ent;

    // Open the directory and read file names
    if ((dir = opendir(input_dir.c_str())) != NULL) {
        // Iterate through all files and directories in the input directory.
        while ((ent = readdir(dir)) != NULL) {
            string filename = ent->d_name;

            // Process only files with a ".in" extension
            if (filename.length() > 3 && filename.substr(filename.length() - 3) == ".in") {
                string input_filepath = input_dir + filename;
                string output_filepath = output_dir + filename;
                output_filepath.replace(output_filepath.length() - 3, 3, ".out");

                cout << "Processing " << input_filepath << "..." << endl;

                ifstream fin(input_filepath);
                if (!fin.is_open()) {
                    cerr << "Error: Could not open input file " << input_filepath << endl;
                    continue;
                }

                ofstream fout(output_filepath);
                if (!fout.is_open()) {
                    cerr << "Error: Could not open output file " << output_filepath << endl;
                    fin.close();
                    continue;
                }

                int N, M;
                fin >> N >> M;
                vector<string> S(N);
                for (int i = 0; i < N; ++i) {
                    fin >> S[i];
                }

                // Run both strategies and choose the best result
                pair<long long, vector<Operation>> result1 = solve(N, M, S, 1);
                pair<long long, vector<Operation>> result2 = solve(N, M, S, 2);

                vector<Operation> final_ops;
                if (result1.first >= result2.first) {
                    final_ops = result1.second;
                } else {
                    final_ops = result2.second;
                }

                // Write the solution to the output file
                fout << final_ops.size() << endl;
                for (const auto& op : final_ops) {
                    fout << op.type << " " << op.index << " " << op.color << endl;
                }

                fin.close();
                fout.close();
                cout << "Done." << endl;
            }
        }
        closedir(dir);
    } else {
        // Could not open the directory
        cerr << "Error: Could not open directory " << input_dir << endl;
        return 1;
    }

    return 0;
}
