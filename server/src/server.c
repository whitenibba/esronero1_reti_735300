
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

#include "header.h"
#include <stdio.h>
#include <ctype.h>

#define DEFAULT_PORT 12345
#define MAX_PENDING_CONNECTIONS 5

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
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <0) {
        perror("Error creating socket");
        closesocket(server_socket);
        winsock_cleaner();
        _exit(0);
    }

    // Set up server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(DEFAULT_PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        closesocket(server_socket);
        winsock_cleaner();
        _exit(0);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_PENDING_CONNECTIONS) == -1) {
        perror("Error listening for connections");
        closesocket(server_socket);
        winsock_cleaner();
        _exit(0);
    }

    printf("Server listening on port %d...\n", DEFAULT_PORT);
    int client_size = sizeof(client_addr);
    while (1) {
        // Accept a connection
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_size)) == -1) {
            perror("Error accepting connection");
            closesocket(client_socket);
            winsock_cleaner();
            _exit(0);
        }

        // Print connection details
        printf("Connection established with %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Handle client
        handle_client(client_socket);

        // Close the connection
        closesocket(client_socket);
    }



    return 0;
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int num1, num2;
    int result;
    char operation;



    while(1){

		// Read operation and numbers from client
		if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
			perror("Error receiving data from client");
			break;

		}

		// Parse data

		if(buffer[0] == '=') break;

		sscanf(buffer, "%c %d %d", &operation, &num1, &num2);
		memset(buffer, 0, BUFFER_SIZE);


		// Perform the requested operation
		switch (operation) {
			case '+':
				result = add(num1, num2);
				break;
			case 'x':
				result = mult(num1, num2);
				break;
			case '-':
				result = sub(num1, num2);
				break;
			case '/':
				result = division(num1, num2);
				break;
		}

		// Send the result to the client
		sprintf(buffer, "%d", result);
		if (send(client_socket, buffer, strlen(buffer), 0) <0) {
			perror("Error sending data to client");
			closesocket(client_socket);
			winsock_cleaner();
			_exit(0);
		}
		memset(buffer, 0, BUFFER_SIZE);

		printf("Result sent to client: %d\n", result);
    }

}
int add(int i1, int i2){
	return i1+i2;
}
int sub(int i1, int i2){
	return i1-i2;
}
int mult(int i1,int i2){
	return i1*i2;
}
int division(int i1,int i2){
	return i1/i2;
}
