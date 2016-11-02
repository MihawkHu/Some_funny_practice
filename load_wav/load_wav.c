// used to load *.wav file
// print basic information on console, output sample data to txt
// one channel
#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>

int main()
{
    SNDFILE *sf;
    SF_INFO info;
    int num, num_items;
    int *buf;
    int f, sr, c, i, j;
    FILE *out;
    
    info.format = 0;
    sf = sf_open("test.wav", SFM_READ, &info);
    if (sf == NULL) {
        printf("Open file error!\n");
        exit(-1);
    }

    f = info.frames;
    sr = info.samplerate;
    c = info.channels;
    printf("Frame number: %d\n", f);
    printf("Samplerate: %d\n", sr);
    printf("Channel number: %d\n", c);
    num_items = f * c;
    printf("Items number: %d\n", num_items);

    buf = (int *) malloc(num_items * sizeof(int));
    num = sf_read_int(sf, buf, num_items);
    sf_close(sf);
    printf("Total read number: %d\n", num);

    out = fopen("out.txt", "w");
    for (i = 0; i < num; i += c) {
        for (j = 0; j < c; ++j) {
            fprintf(out, "%d ", buf[i+j]);
        }
        fprintf(out, "\n");
    }
    fclose(out);
    
    return 0;
}
