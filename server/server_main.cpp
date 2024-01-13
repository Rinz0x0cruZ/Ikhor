#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>     // Add this for strerror

using namespace std;

int failure(int val, const char* msg) {
    if (val < 0) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

void sockConfig(sockaddr_in& sockaddr, int port) {
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons(port);
}

void startMenu(int port) {
    cout << "Starting server at localhost:" << port << "......." << endl;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // IPv4 TCP
    failure(sockfd, "Failed to create a server | ERR: ");

    cout << "[+] SOCKET MADE" << endl;

    sockaddr_in sockaddr;
    sockConfig(sockaddr, port);


    failure(bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)), "Failed to bind socket");
    cout << "[+] CREDS BOUND" << endl;

    failure(listen(sockfd, 10), "Failed to listen");
    cout << "[+] LISTENER SET, WAITING FOR CONNECTION......" << endl;

    auto addrlen = sizeof(sockaddr);
    int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
    failure(connection, "Failed to accept connection");
    cout << "[+] CONNECTION RECEIVED" << endl;

    char buffer[100];
    auto bytesRead = read(connection, buffer, 100);

    cout << "Message " << buffer << endl;

    string response = "[+] Server Response 200! -> SUCCESS";

    send(connection, response.c_str(), response.size(), 0);

    close(connection);

    close(sockfd);
}

int main(int argc, char* argv[]) {
    if (argc != 3 || string(argv[1]) != "--port") {
        cerr << "Usage: " << "ikhor server | client --port <port>\n"
             << endl;
        return 1;
    }

    int port = stoi(argv[2]);

    while (true) {
        try {
            startMenu(port);
        } catch (const std::exception& e) {
            cerr << "Exception caught: " << e.what() << endl;
        }

        cout<<"\n\n\n\n[+] NEW CONNECTION\n\n\n\n";
    }
    cout << "[+] THE END";
    return 0;
}
