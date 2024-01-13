#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket\n";
        return -1;
    }

    // Connect to the server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345); // Use the same port as the server
    inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)); // Use the server's IP address

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error connecting to server\n";
        close(clientSocket);
        return -1;
    }

    // Ask for user input and send to the server until the user types "exit"
    const size_t bufferSize = 1024;
    char buffer[bufferSize];

    string remoteEndpoint = "/remote";
    string clientEndpoint = "/client";

    while (true) {
        std::cout << "Enter a message (type 'exit' to quit): ";
        std::cin.getline(buffer, bufferSize);

        if (strcmp(buffer, "exit") == 0) {
            break; // Exit the loop if the user types "exit"
        }

        // Choose the appropriate endpoint based on the client type
        std::string endpoint = (isRemoteSoftware) ? remoteEndpoint : clientEndpoint;

        ssize_t bytesSent = send(clientSocket, buffer, strlen(buffer), 0);
        if (bytesSent == -1) {
            std::cerr << "Error sending data\n";
            break; // Exit the loop on error
        }

        // Wait for a response from the server (optional)
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            std::cerr << "Error receiving data\n";
            break; // Exit the loop on error
        } else if (bytesRead == 0) {
            std::cout << "Server closed the connection\n";
            break; // Exit the loop if the server closed the connection
        } else {
            buffer[bytesRead] = '\0';
            std::cout << "Received from server: " << buffer << "\n";
        }
    }

    // Close socket
    close(clientSocket);

    return 0;
}
