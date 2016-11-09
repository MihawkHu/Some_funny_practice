// used to load *.wav file
// print basic information on console, output sample data to txt
// one channel
// fixed frame size
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

const int frame_size = 400;
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
    out = fopen("../out.txt", "w");
    
    for (int ff = 0; ff < files.size(); ++ff) {
        if (files[ff].substr(files[ff].length() - 4, files[ff].length()).
                compare(".wav") != 0) continue;
        
        cout << ff << "  current file: " << files[ff] << endl;
        
        SNDFILE *sf;
        SF_INFO info;
        int num, num_items;
        double *buf;
        int f, sr, c, i, j;
        
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

        buf = (double *) malloc(num_items * sizeof(double));
        num = sf_read_double(sf, buf, num_items);
        sf_close(sf);
        // printf("Total read number: %d\n", num);
        
        fprintf(out, "%s  [\n", files[ff].substr(0, 9).c_str());
        for (i = 0; i + frame_size <= num; i += c * stride) {
            fprintf(out, "  ");
            for (int t = 0; t < frame_size; t += c) {
                for (j = 0; j < c; ++j) {
                    fprintf(out, "%.7g ", 1000.0 * buf[i+t]);
                }
            }
            if (i + c * stride + frame_size <= num) fprintf(out, "\n");
        }
        fprintf(out, "]\n");
        free(buf);
    }
    fclose(out);
    
    return 0;
}


