#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SIZE 4096
#define LINE_SIZE 30
char *filename = "mainTracker.txt";

void send_file(int sockfd) {
    char data[SIZE]  = {0};
    FILE *fp;

    fp = fopen (filename, "r");
    if (!fp) {
        printf("[-] Unable to open mainTracker.txt\n");
        exit(1);
    }

    while(fgets(data, SIZE, fp) != NULL) {
        if (send(sockfd, data, sizeof(data), 0) == -1) {
            perror("[-] Error in sending data\n");
            exit(1);
        }
        bzero(data, SIZE);
    }
}

void recv_newLine(int sockfd) {
    char newLine[LINE_SIZE]  = {0};
    FILE *fp;

    if (recv(sockfd, newLine, sizeof(newLine), 0) == -1) {
        perror("[-] Error receiving newLine\n");
        exit(1);
    }
    printf("[+] New host on the network %s", newLine);

    fp = fopen (filename, "a");
    if (!fp) {
        printf("[-] Unable to open mainTracker.txt\n");
        exit(1);
    }
    fprintf(fp, "%s\n", newLine);
    fclose(fp); // Close main tracker file once new line appended

    bzero(newLine, LINE_SIZE);
}

int main() {
    char *ip = "127.0.0.1";
    int port = 33333; 
    int e;

    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[SIZE] ;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("[-] Error creating socket\n");
        exit(1);
    }
    printf("[+] TrackerServer socket created\n");

    // memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (e < 0) {
        perror("[-] Error in binding\n");
        exit(1);
    }
    printf("[+] Binding successful\n");

    while(1) {
        e = listen(sockfd, 10);
        if (e == 0) {
            printf("[+] Listening...\n");
        } else {
            perror("[-] Error in listening\n");
            exit(1);
        }
        addr_size = sizeof(new_addr);
        new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
        printf("hey ");
        send_file(new_sock);
        printf("[+] Data sent successfully\n");
        recv_newLine(new_sock);
        printf("[+] New tracker line received successfully\n");

        close(new_sock);
        printf("[+] Disconnected from client\n");
    }

return 0;
}