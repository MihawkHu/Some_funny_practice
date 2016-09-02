#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "jzmedia.h"

#define FIXED16_PRECISION 7
#define FIXED16_ONE (int16_t)128
#define FIXED16_ZERO (int16_t)0

#define fixed16_xmul(A, B) ((int16_t)(((int32_t)(A) * (int32_t)(B)) >> FIXED16_PRECISION))
#define fixed16_add(A, B) ((A) + (B))
#define fixed16_xdiv(A, B) ((int16_t)(((int32_t)(A) << (FIXED16_PRECISION)) / (int32_t)(B)))
#define fixed16(R) ((int16_t)(((R) * ((int16_t)1 << (FIXED16_PRECISION))) + ((R) >= 0 ? 0.5 : -0.5)))
#define fixed16_tofloat(I) (((float)(I)) / ((int16_t)1 << (FIXED16_PRECISION)))


int16_t fixed16_pow2(int16_t x)
{
    static int16_t pow_p0 = fixed16(121.2740575f);
    static int16_t pow_p1 = fixed16(27.7280233f);
    static int16_t pow_p2 = fixed16(4.84252568f);
    static int16_t pow_p3 = fixed16(1.49012907f);
    static int16_t limit = fixed16(-126.0f);

    do {
        int16_t offset = (x < 0) ? FIXED16_ONE : 0;
        int16_t clipp = (x < limit) ? limit : x;
        int16_t w = (int16_t)fixed16_tofloat(clipp);
        int16_t z = clipp + offset - fixed16(w);
        int16_t y;
        union {
            int16_t i;
            float f;
        } v;

        y = (clipp + pow_p0 + fixed16_xdiv(pow_p1, (pow_p2 - z)) - fixed16_xmul(pow_p3, z));
        y = fixed16_xmul((1 << 23), y);
        v.i = y;

        return fixed16(v.f);
    } while (0);
}

int16_t fixed16_exp(int16_t x)
{
    static int16_t p1 = fixed16(1.442695040f);

    return fixed16_pow2(fixed16_xmul(p1, x));
}

void matrix_mul_fix16(int M, int N, int K, int16_t alpha, int16_t *A, int lda, int16_t *B, int ldb, int16_t beta, int16_t *C, int ldc)
{
    if(alpha == FIXED16_ZERO && beta == FIXED16_ONE) {
        return;
    }
    
    if (beta == FIXED16_ZERO) {
        for (int i = 0; i < N; i++) {
            int16_t *pC = &(C[ldc * i]);

            for (int j = 0; j < M; j++) {
                *(pC++) = FIXED16_ZERO;
            }
        }
    } 
    else if (beta != FIXED16_ONE) {
        for (int i = 0; i < N; i++) {
            int16_t *pC = &(C[ldc * i]);

            for (int j = 0; j < M; j++) {
                *(pC) = fixed16_xmul(beta, *(pC));
                pC++;
            }
        }
    }
    
    if (alpha == FIXED16_ZERO) {
        return;
    }
    else if (alpha != FIXED16_ONE) {
        for (int i = 0; i < K; ++i) {
            int16_t *pB = &(B[ldc * i]);
            
            for (int j = 0; j < N; ++j) {
                if (*pB != FIXED16_ZERO)
                    *(pB) = fixed16_xmul(alpha, *(pB));
                pB++;
            }
        }
    }
    
    S32I2M(xr16, 0xffffffff); // enable mxu
    int M_reduced_8 = (M - (M & 7));
    for (int k = 0; k < K; ++k) {
        int16_t *pB = &(B[k * ldb]);
        
        for (int j = 0; j < N; ++j) {
            if (*(pB + j) == FIXED16_ZERO) continue;
            
            S16LDD(xr1, pB + j, 0, 0);

            int16_t b = B[k * ldb + j];
            int16_t *pA = &(A[k * lda]);
            int16_t *pC = &(C[j * ldc]);
            
            int i = 0;
            for (; i < M_reduced_8; i += 8) {
                S32LDD(xr2, pA, 0);
                S32LDD(xr5, pA, 4);
                S32LDD(xr8, pA, 8);
                S32LDD(xr11, pA, 12);
                
                D16MUL_LW(xr3, xr1, xr2, xr4);
                D16MUL_LW(xr6, xr1, xr5, xr7);
                D16MUL_LW(xr9, xr1, xr8, xr10);
                D16MUL_LW(xr12, xr1, xr11, xr13);
                
                D32SARL(xr2, xr3, xr4, 7);
                D32SARL(xr5, xr6, xr7, 7);
                D32SARL(xr8, xr9, xr10, 7);
                D32SARL(xr11, xr12, xr13, 7);
                
                S32LDD(xr3, pC, 0);
                S32LDD(xr6, pC, 4);
                S32LDD(xr9, pC, 8);
                S32LDD(xr12, pC, 12);
                
                Q16ADD_AA_WW(xr4, xr2, xr3, xr4);
                Q16ADD_AA_WW(xr7, xr5, xr6, xr7);
                Q16ADD_AA_WW(xr10, xr8, xr9, xr10);
                Q16ADD_AA_WW(xr13, xr11, xr12, xr13);
                
                S32STD(xr4, pC, 0);
                S32STD(xr7, pC, 4);
                S32STD(xr10, pC, 8);
                S32STD(xr13, pC, 12);
                
                pA += 8;
                pC += 8;
            }
            for (; i < M; ++i) {
                C[j * ldc + i] += fixed16_xmul(A[k * lda + i], b);
            }
        }
    }
    S32I2M(xr16, 0x0); // disable mxu
    
}

void matrix_mul_float32(int M, int N, int K, float alpha, float *A, int lda, float *B, int ldb, float beta, float *C, int ldc)
{
    if (alpha == 0.0 && beta == 1.0) {
        return;
    }

    if (beta == 0.0) {
        for (int i = 0; i < N; ++i) {
            memset(&(C[ldc * i]), 0, sizeof(float) * M);
        }
    }
    else if (beta != 1.0) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                C[ldc * i + j] *= beta;
            }
        }
    }

    if (alpha == 0.0) {
        return;
    }
    
    for (int k = 0; k < K; ++k) {
        for (int j = 0; j < N; ++j) {
            if (B[k * ldb + j] == 0.0) continue;

            float b = B[k * ldb + j];
            for (int i = 0; i < M; ++i) {
                C[j * ldc + i] += A[k * lda + i] * b;
            }
        }
    }
    
}
