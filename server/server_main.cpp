// server.cpp
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring> // Add this for strerror

using namespace std;

int failure(int val, const char *msg)
{
    if (val < 0)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

void sockConfig(sockaddr_in &sockaddr, int port)
{
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons(port);
}

void handleConnection(int connection)
{
    const int bufferSize = 1024; // Adjust the buffer size as needed
    char buffer[bufferSize];
    auto bytesRead = read(connection, buffer, sizeof(buffer));

    cout << "Received message:\n" << buffer << endl;

    // Parse the HTTP request to extract the content length and body
    string httpRequest(buffer);
    size_t contentLengthPos = httpRequest.find("Content-Length: ");
    if (contentLengthPos != string::npos)
    {
        size_t endOfLinePos = httpRequest.find("\r\n", contentLengthPos);
        size_t startPos = contentLengthPos + strlen("Content-Length: ");
        string contentLengthStr = httpRequest.substr(startPos, endOfLinePos - startPos);
        int contentLength = stoi(contentLengthStr);

        // Find the start of the request body
        size_t requestBodyStart = httpRequest.find("\r\n\r\n") + strlen("\r\n\r\n");
        if (requestBodyStart != string::npos && bytesRead >= static_cast<int>(requestBodyStart + contentLength))
        {
            string message = httpRequest.substr(requestBodyStart, contentLength);
            cout << "Extracted message: " << message << endl;

            string response = "[+] Server Response 200! -> SUCCESS";
            send(connection, response.c_str(), response.size(), 0);
        }
    }

    close(connection);
}


void startMenu(int sockfd)
{
    cout << "[+] LISTENER SET, WAITING FOR CONNECTION......" << endl;

    while (true)
    {
        sockaddr_in sockaddr;
        auto addrlen = sizeof(sockaddr);

        int connection = accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
        failure(connection, "Failed to accept connection");
        cout << "[+] CONNECTION RECEIVED" << endl;

        handleConnection(connection);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3 || string(argv[1]) != "--port")
    {
        cerr << "Usage: " << "ikhor server | client --port <port>\n"
             << endl;
        return 1;
    }

    int port = stoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // IPv4 TCP
    failure(sockfd, "Failed to create a server | ERR: ");

    cout << "[+] SOCKET MADE" << endl;

    sockaddr_in sockaddr;
    sockConfig(sockaddr, port);

    failure(bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)), "Failed to bind socket");
    
    // Set up the listener outside the loop
    failure(listen(sockfd, 10), "Failed to listen");

    try
    {
        startMenu(sockfd);
    }
    catch (const std::exception &e)
    {
        cerr << "Exception caught: " << e.what() << endl;
    }

    close(sockfd); // Close the socket when done

    cout << "[+] THE END";
    return 0;
}