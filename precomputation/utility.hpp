#include <bits/stdc++.h>
using namespace std;

const int mask = 0xFF;
const int shifts[3] = {0, 8, 16};

void writeThreeBytes (ofstream &fout, int value) {
    char bytes[3];
    for (int i = 0; i < 3; i++)
        bytes[i] = (value >> shifts[i]) & mask;
    fout.write(bytes, 3);
}

int readThreeBytes (ifstream &fin) {
    char bytes[3]; fin.read(bytes, 3);
    int value = 0;
    for (int i = 0; i < 3; i++)
        value |= (bytes[i] << shifts[i]);
    return value;
}