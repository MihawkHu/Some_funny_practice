#include <iostream>
#include <immintrin.h>
#include <time.h>
using namespace std;


int main()
{
    const int col = 32, row = 64, tt = 1000;
    float m1[row][col];  // matrix 2
    float m2[col][row];  // matrix 1
    float ans[row][row];
    float temp[8];
    
    // init m1 and m2
    srand(time(NULL));
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            m1[i][j] = (float)(rand() % 1000) / 700.0f;
        }
    }    
    for (int i = 0; i < col; ++i) {
        for (int j = 0; j < row; ++j) {
            m2[i][j] = (float)(rand() % 1000) / 700.0f;
        }
    }
    
    // normal way
    int t1 = clock();
    for (int r = 0; r < tt; ++r) {
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < row; ++j) {
                int res = 0;
                for (int k = 0; k < col; ++k) {
                    res += m1[i][k] * m2[k][j];
                }
                ans[i][j] = res;
            }
        }
    }
    
    int t2 = clock();
    int time1 = (((float)t2 - (float)t1) / CLOCKS_PER_SEC ) * 1000;
    
    // avx
    t1 = clock();
    __m256 ymm0, ymm1;
    for (int r = 0; r < tt; ++r) {
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < row; ++j) {
                float res = 0;
                for (int p = 0; p < col; p += 8) {
                    ymm0 = __builtin_ia32_loadups256(&m1[i][p]);
                    ymm1 = __builtin_ia32_loadups256(&m2[p][j]);
                    ymm0 = __builtin_ia32_mulps256(ymm0, ymm1);
                    ymm0 = __builtin_ia32_addps256(ymm0, ymm1);
                    __builtin_ia32_storeups256(temp, ymm0);
                    
                    for (int k = 0; k < 8; ++k)
                        res += temp[k];
                }
                ans[i][j] = res;
            }
        }
    }
    t2 = clock();
    int time2 = (((float)t2 - (float)t1) / CLOCKS_PER_SEC ) * 1000;
    
    cout << "Normal time: " << time1 << endl;
    cout << "Avx time: " << time2 << endl;
    return 0;
}