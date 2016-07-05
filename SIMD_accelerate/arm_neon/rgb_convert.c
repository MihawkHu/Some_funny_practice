#include <stdio.h>
#include <arm_neon.h>
#include <time.h>
#include <stdlib.h>

// common way used to convert
void normal_convert(uint8_t * __restrict dest, uint8_t * __restrict src, int n) {    
    int i;    
    for (i = 0; i < n; i++) {    
      int r = *src++;
        int g = *src++;
        int b = *src++; 
   
        int y = r * 77 + g * 151 + b * 28;    
   
        *dest++ = (y >> 8);    
    }    
}

// using neon to convert
void neon_convert(uint8_t * __restrict dest, uint8_t * __restrict src, int n) {    
    int i;    
    uint8x8_t rfac = vdup_n_u8(77);
    uint8x8_t gfac = vdup_n_u8(151);
    uint8x8_t bfac = vdup_n_u8(28);
    n /= 8;    
   
    for (i = 0; i < n; i++) {    
        uint16x8_t temp;    
        uint8x8x3_t rgb = vld3_u8(src);    
        uint8x8_t result;    
        
        temp = vmull_u8(rgb.val[0], rfac);
        temp = vmlal_u8(temp, rgb.val[1], gfac);
        temp = vmlal_u8(temp, rgb.val[2], bfac);
   
        result = vshrn_n_u16(temp, 8);
        vst1_u8(dest, result);
        src  += 8 * 3;
        dest += 8;
    }
}

int main()
{
    int N = 400, nn = 1;
    uint8_t src[N][3];
    uint8_t des1[N];
    uint8_t des2[N];
    clock_t t1, t2;
    
    for (int i = 0; i < N; ++i)
        for(int j = 0; j < 3; ++j) {
            src[i][j] = rand() % 255;
        }
    
    t1 = clock();
    for (int i = 0; i < nn; ++i) {
        normal_convert(des1, src, N);
    }
    t2 = clock();
    float time1 = (((float)t2 - (float)t1) / CLOCKS_PER_SEC ) * 1000;
    
    t1 = clock();
    for (int i = 0; i < nn; ++i) {
        neon_convert(des2, src, N);
    }
    t2 = clock();
    float time2 = (((float)t2 - (float)t1) / CLOCKS_PER_SEC ) * 1000;

    printf("Time1: %f\nTime2: %f", time1, time2);
    
    return 0;
}
