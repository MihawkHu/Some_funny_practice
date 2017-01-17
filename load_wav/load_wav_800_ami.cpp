// used to load *.wav file
// for data set ami
// print basic information on console, output sample data to txt
// one channel
// optional frame size
// input linecount as the second parameter
#include <cstdio>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <sndfile.h>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <string>
#include <dirent.h>
using namespace std;

int frame_size = 800;
const int stride = 160;

int main(int argc, char const *argv[])
{
    ifstream fin("segments");
    FILE *out;
    out = fopen("out_800.txt", "w");
    
    int linecount = atoi(argv[1]);
    for (int ck = 0; ck < linecount; ++ck) {
        string file_name, file_path;
        float head, tail;
        fin >> file_name >> file_path >> head >> tail;
        
        // get wav path
        file_path = file_path.substr(4, 7);
        if (file_path[6] == '_') {
            file_path = file_path.substr(0, 6);
        }
        cout << ck + 1 << " " << "current file: " << file_path << " " <<
            head << " " << tail << endl;
        file_path = string("/speechlab/data/import/AMI/amicorpus/beamformed/")
            + file_path + string("/") + file_path + string("_MDM8.wav");

        // wav process
        SNDFILE *sf;
        SF_INFO info;
        int num, num_tt, num_items;
        double *buf_tt, *buf;
        int f, sr, c, i, j = 0, t, v;
        int ttmp = (frame_size - 400) / 2;
        
        info.format = 0;
        sf = sf_open(file_path.c_str(), SFM_READ, &info);
        if (sf == NULL) {
            printf("Open file error!\n");
            exit(-1);
        }
        
        f = info.frames;
        sr = info.samplerate;
        c = info.channels;
        // printf("Frame number: %d\n", f);
        // printf("Samplerate: %d\n", sr);
        // printf("Channel number: %d\n", c);
        num_items = f * c;
        // printf("Items number: %d\n", num_items);
        
        buf_tt = (double*)malloc(num_items * sizeof(double));
        num_tt = sf_read_double(sf, buf_tt, num_items);
        sf_close(sf);
        // printf("Total read number: %d\n", num_tt);
        int utt_start = head * sr, utt_end = tail * sr;
        num = utt_end - utt_start;
        
        buf = (double*)malloc(num * sizeof(double));
        memcpy(buf, buf_tt + utt_start, num * sizeof(double));
        free(buf_tt);
        
        // fprintf(out, "%s  [\n", files[ff].substr(0, 9).c_str());
        fprintf(out, "%s  [\n", file_name.c_str());
        for (i = -ttmp; i + 400 + ttmp <= num; i += c * stride) {
            fprintf(out, "  ");
            if (i < 0) {
                for (v = i; v < 0 && v < frame_size; ++v) {
                    // for (j = 0; j < c; ++j) {
                    fprintf(out, "%.7g ", 1000.0 * buf[j]);
                    // }
                }
                for (t = 0; t < frame_size + i; ++t) {
                    // for (j = 0; j < c; ++j) {
                    fprintf(out, "%.7g ", 1000.0 * buf[t + j]);
                    }
            }
            else {
                for (t = 0; t < frame_size; t += c) {
                    if (i + t > num) {
                        fprintf(out, "%.7g ", 1000.0 * buf[num - 1]);
                    }
                        else {
                        // for (j = 0; j < c; ++j) {
                        fprintf(out, "%.7g ", 1000.0 * buf[i + t + j]);
                        // }
                    }
                }
            }
            if (i + c * stride + 400 + ttmp <= num) fprintf(out, "\n");
        }
        fprintf(out, "]\n");
        free(buf);
    }
    
    fclose(out);
    fin.close();
    return 0;
}


