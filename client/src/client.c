#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include "calculator.h"

#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 12345
#define BUFFER_SIZE 256


void winsock_cleaner() {
#if defined WIN32
	WSACleanup();
#endif
}

int main() {
#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int res = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (res != 0) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <0) {
    	closesocket(client_socket);
    	winsock_cleaner();
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(DEFAULT_IP);
    server_addr.sin_port = htons(DEFAULT_PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    	closesocket(client_socket);
    	winsock_cleaner();
    	perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    // Send and receive data
    char buffer[BUFFER_SIZE];
    char operation;
    int num1, num2, result;

    while (1) {
        // Read operation and numbers from user
        printf("Enter operation and two numbers (e.g., + 23 45): ");
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%c %d %d", &operation, &num1, &num2);

        // Send data to server
        if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
        	closesocket(client_socket);
        	winsock_cleaner();
        	perror("Error sending data to server");
            exit(EXIT_FAILURE);
        }
        memset(buffer, 0, BUFFER_SIZE);

        // Receive result from server
        if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
        	closesocket(client_socket);
        	winsock_cleaner();
            perror("Error receiving data from server");
            exit(EXIT_FAILURE);
        }

        // Parse and print result
        sscanf(buffer, "%d", &result);
        memset(buffer, 0, BUFFER_SIZE);
        printf("Result from server: %d\n", result);

        // Check if the client should terminate
        if (operation == '=') {
            break;
        }
    }

    // Close the socket
	closesocket(client_socket);
	winsock_cleaner();

    return 0;
}
