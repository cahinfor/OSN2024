//
// Created by cahInfor on 10/09/25.
//

#include <iostream>

using namespace std;

int solve() {
    int A;
    cout << "TANYA " << 128 << endl;
    for (int i = 1; i <= 128; i++)
        cout << i << " ";
    cout << endl;
    cin >> A;

    int B;
    cout << "TANYA " << 128 << endl;
    for (int i = 1; i <= 128; i++)
        cout << 128 + i << " ";
    cout << endl;
    cin >> B;

    int C;
    cout << "TANYA " << 128 << endl;
    for (int i = 1; i <= 64; i++)
        cout << i << " ";
    for (int i = 1; i <= 64; i++)
        cout << 128 + 64 + i << " ";
    cout << endl;
    cin >> C;

    if (A == 0 && C == 0) return B;
    if (B == 0 && C == 0) return 256 - A + 1;

    if (A == 0) return 64 + C;
    return 128 + 64 - C + 1;
}

int main() {
    int result = solve();
    cout << "JAWAB " << result << endl;
}
