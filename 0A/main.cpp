//
// Created by cahInfor on 9/3/2025.
//

#include <iostream>

using namespace std;

int main() {
  int N;
  cin >> N;

  double total = 0;
  int termuda = 200001;
  int tertua = -1;

  for (int i=1; i<=N; i++) {
    int A;
    cin >> A;
    total = total + A + i - 1;
    termuda = min(termuda+1, A);
    tertua = max(tertua+1, A);

    printf("%d %d %.4lf\n", termuda, tertua, total/i);
  }
}
