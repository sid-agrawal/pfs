#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SERVER "93.184.215.14" // "example.com"
#define PORT 80
#define REQUEST "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n"

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];
    ssize_t bytes_sent, bytes_received;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert server address to binary form
    if (inet_pton(AF_INET, SERVER, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Send a ping request
    bytes_sent = send(sockfd, REQUEST, strlen(REQUEST), 0);
    if (bytes_sent < 0) {
        perror("Send failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Receive the response
    bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        perror("Receive failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Null-terminate the received data
    buffer[bytes_received] = '\0';
    printf("Response:\n%s\n", buffer);

    while (1);
    // Close the socket
    close(sockfd);
    return 0;
}