#include "include.h"

#define PORT_NUM 2680
#define LENGTH 512

int getch();

int main(int argc, char *argv[]) {
    int s;   // socket descriptor
    int n;   // number of each recv() call
    char buff[LENGTH];
    char *ser_name; // server name
        
    struct sockaddr_in ser_addr;
    
    // check input
    if (argc != 2) {
        printf("ERROR: One arguments needed.\n");
        printf("./client <server name(IP)>\n");
        exit(1);
    }
    
    ser_name = argv[1];
    // Get the Socket file descriptor
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ERROR: Socket creation failed.\n");
        exit(1);
    }
    
    // init socket
    ser_addr.sin_family = AF_INET; 
    ser_addr.sin_port = htons(PORT_NUM); 
    inet_pton(AF_INET, ser_name, &ser_addr.sin_addr); 
    bzero(&(ser_addr.sin_zero), 8);
    
    // connect to the server
    if (connect(s, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr)) < 0) {
        perror("ERROR: Connection failed.\n");
        exit(1);
    }
    
    printf("Conneted to the server %s\n", ser_name);
    printf("You can download file/text from this server.\n");
    
    int isfirst = 0;
    for (;;) {
        printf("Press any key to go on.\n");
        printf("Press <ESC> to exit.\n");
        char ch = getch();
        if (isfirst != 0) ch = getch();
        isfirst = 1;
        if(ch == 27) {
            printf("EXIT...\n");
            break;
        }
        
        printf("File name you want to download: ");
        char file_in[20], file_out[50];
        scanf("%s", file_in);
        printf("local file dictory you want to save: ");
        scanf("%s", file_out);
        char *file_name = file_in;
        char *file_dict = file_out;
        
        // send file name
        send(s, file_name, strlen(file_name), 0);
        
        // open file local
        FILE *fr = fopen(file_dict, "a");
        if(fr == NULL) {
            printf("ERROR: File opening failed.\n");
        }
        
        // recieve file
        bzero(buff, LENGTH);
        while ((n = recv(s, buff, LENGTH, 0)) > 0) {
            int write_sz = fwrite(buff, sizeof(char), n, fr);
            if(write_sz < n) {
                perror("ERROR: File write failed.\n");
            }
            bzero(buff, LENGTH);
            if (n == 0 || n != 512)  {
                break;
            }
        }
        printf("Recieve file %s done. Saved as %s\n", file_name, file_dict);
        fclose(fr);
    }
    
    return 0;
}

int getch( ) {
    struct termios oldt, newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
}