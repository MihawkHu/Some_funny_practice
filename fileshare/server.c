// Filename: server.c
// function: 
#include "include.h"

#define PORT_NUM 2680
#define LENGTH 512

char* get_ip();

int main(int argc, char *argv[]) {
    int ls;  // listen socket
    int s;   // socket descriptor
    char buff[LENGTH];
    char *ptr = buff;
    int len; // number waiting to be recieved
    int max_len = sizeof(buff); // max recieve number
    int n;   // number of each recv() call
    int wait_size = 16; // waiting client number
    int clientaddr_len; // clinet address length
    
    struct sockaddr_in ser_addr;
    struct sockaddr_in client_addr;
    
    // print server name (IP address)
    char *server_ip_address = get_ip();
    printf("Server IP: %s\n", server_ip_address);
    
    // Get the Socket file descriptor
    if ((ls = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("ERROR: Listen socket failed.\n");
        exit(1);
    }
    
    // Init local socket address
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(PORT_NUM);
    ser_addr.sin_addr.s_addr = INADDR_ANY; // AutoFill local address
    bzero(&(ser_addr.sin_zero), 8); // Flush the rest of struct
    
    // bind socket address
    if ( bind(ls, (struct sockaddr*)&ser_addr, sizeof(struct sockaddr)) < 0) {
        perror("ERROR: Binding failed.\n");
        exit(1);
    }
    
    // listen connection request
    if (listen(ls, wait_size) < 0) {
        perror("ERROR: Listening failed.\n");
        exit(1);
    }
    
    // process connection
    for (;;) {
        if ((s = accept(ls, (struct sockaddr*)&client_addr, &clientaddr_len)) < 0) {
            perror("ERROR: Accepting failed.\n");
            exit(1);
        }
        // recieve file name
        while ((n = recv(s, ptr, max_len, 0)) > 0) {
            ptr += n;
            max_len -=n;
            len += n;
            if (n == 0 || n != LENGTH) break;
        }
        // send file to client 
        char* file_name = buff;
        file_name[n] = '\0';
        char sdbuf[LENGTH];
        FILE *fs = fopen(file_name, "r");
        if (fs == NULL) {
            printf("ERROR: File open failed.\n");
            exit(1);
        }
        
        bzero(sdbuf, LENGTH); 
        int fread_len; 
        while ((fread_len = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0) {
            if (send(s, sdbuf, fread_len, 0) < 0) {
                printf("ERROR: Send file Failed.\n");
                exit(1);
            }
            bzero(sdbuf, LENGTH);
        }
        printf("Send File %s done.\n", file_name);
        close(s);
    }
    
    return 0;
}

char* get_ip() {
    char* google_dns_server = (char*)"8.8.8.8";
    char* google_dns_port = (char*)"53";
    
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* info;
    int ret = 0;
    if ((ret = getaddrinfo(google_dns_server, google_dns_port, &hints, &info)) != 0) {
        printf("[ERROR]: getaddrinfo error: %s\n", gai_strerror(ret));
        return NULL;
    }

    if (info->ai_family == AF_INET6){
        printf("[ERROR]: do not support IPv6 yet.\n");
        return NULL;
    }

    int sock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    if (sock <= 0){
        perror("socket");
        return NULL;
    }

    if (connect(sock, info->ai_addr, info->ai_addrlen) < 0) {
        perror("connect");
        close(sock);
        return NULL;
    }

    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    if (getsockname(sock, (struct sockaddr*)&local_addr, &addr_len) < 0){
        perror("getsockname");
        close(sock);
        return NULL;
    }

    char *myip, myip1[INET_ADDRSTRLEN];
    myip=(char *)malloc(INET_ADDRSTRLEN*sizeof(char));
    if (inet_ntop(local_addr.sin_family, &(local_addr.sin_addr), myip1, sizeof(myip1)) == NULL){
        perror("inet_ntop");
        return NULL;
    }
    strcpy(myip,myip1);
    fflush(stdout);
    return myip;
}