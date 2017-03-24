// used to load *.wav file
// print basic information on console, output sample data to txt
// one channel
// frame size = 800
// do normalization on utterence layer
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <sndfile.h>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <string>
#include <dirent.h>
#include <cmath>
using namespace std;

vector<string> getFiles(string cate_dir) {
    vector<string> files;
  
    DIR *dir;  
    struct dirent *ptr;
   
    if ((dir = opendir(cate_dir.c_str())) == NULL)  
        {  
        perror("Open dir error...");  
                exit(1);  
        }  
   
    while ((ptr = readdir(dir)) != NULL)  
    {  
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..") == 0)
                continue;
        else if(ptr->d_type == 8)
            files.push_back(ptr->d_name);  
        else if(ptr->d_type == 10)  
            continue;  
        else if(ptr->d_type == 4)
        {  
            files.push_back(ptr->d_name);  
        }  
    }  
    closedir(dir);  
  
    sort(files.begin(), files.end());  
    return files;  
}  

int frame_size = 800;
const int stride = 160;

vector<string> getFiles(string cate_dir);

int main(int argc, char const *argv[])
{
    char current_address[100];
    memset(current_address, 0, 100); 
    getcwd(current_address, 100);
    
    vector<string> files = getFiles(current_address);
    for (int i=0; i<files.size(); i++)  
    {  
        cout<<files[i]<<endl;  
    } 
    
    FILE *out;
    out = fopen("out_800.txt", "w");
    
    if (argc == 2) frame_size = atoi(argv[1]);
    for (int ff = 0; ff < files.size(); ++ff) {
        if (files[ff].substr(files[ff].length() - 4, files[ff].length()).
                compare(".wav") != 0) continue;
        
        cout << ff << "  current file: " << files[ff] << endl;
        
        SNDFILE *sf;
        SF_INFO info;
        int num, num_items;
        int *buf1;
        int f, sr, c, i, j = 0, t, v;
        int ttmp = (frame_size - 400) / 2;
        
        info.format = 0;
        sf = sf_open(files[ff].c_str(), SFM_READ, &info);
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

        buf1 = (int *) malloc(num_items * sizeof(int));
		double *buf = (double *) malloc(num_items * sizeof(int));
        num = sf_read_int(sf, buf1, num_items);
        sf_close(sf);
        // printf("Total read number: %d\n", num);
        
		double avg = 0.0;
		for (int abc = 0; abc < num; ++abc) {
			avg += buf1[abc];
		}
		avg = avg / num;
		double var = 0.0;
		for (int abc = 0; abc < num; ++abc) {
			var += (buf1[abc] - avg) * (buf1[abc] - avg);
		}
		var = var / num;
		var = sqrt(var);
		cout << "avg" << avg << endl;
		cout << "var" << var << endl;
		for (int abc = 0; abc < num; ++abc) {
			buf[abc] = ((double)buf1[abc] - avg) / var;
		}

        fprintf(out, "%s  [\n", files[ff].substr(0, 9).c_str());
        for (i = -ttmp; i + 400 + ttmp <= num; i += c * stride) {
            fprintf(out, "  ");
            if (i < 0) {
                for (v = i; v < 0 && v < frame_size; ++v) {
                    // for (j = 0; j < c; ++j) {
                    fprintf(out, "%.7g ", buf[j]);
                    // }
                }
                for (t = 0; t < frame_size + i; ++t) {
                    // for (j = 0; j < c; ++j) {
                    fprintf(out, "%.7g ", buf[t + j]);
                    // }
                }
            }
            else {
                for (t = 0; t < frame_size; t += c) {
                    if (i + t > num) {
                        fprintf(out, "%.7g ", buf[num - 1]);
                    }
                    else {
                        // for (j = 0; j < c; ++j) {
                        fprintf(out, "%.7g ", buf[i + t + j]);
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
    
    return 0;
}


