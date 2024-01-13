#include <iostream>
#include <string>
#include <thread>
#include <curl/curl.h>

using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

string SendCommand(const string& ip, unsigned int port, const string& command) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Error initializing cURL." << endl;
        return "";
    }

    string protocol = "http";  // Adjust the protocol as needed
    string url = protocol + "://" + ip + ":" + to_string(port);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Set the command as the POST data
    string postData = "C" + command; // Assuming 'C' indicates a command
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

    string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "cURL request failed: " << curl_easy_strerror(res) << endl;
    }

    curl_easy_cleanup(curl);

    return response;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <server_ip> <server_port>\n";
        return 1;
    }

    string ip = argv[1];
    int port = stoi(argv[2]);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    string command;

    // Continue taking user input until "exit" is entered
    while (true) {
        cout << "Enter a command (type 'exit' to stop): ";
        getline(cin, command);

        if (command == "exit") {
            break;
        }

        // Send command to the server
        string serverResponse = SendCommand(ip, port, command);

        // Process the server response
        cout << "Server response: " << serverResponse << endl;

        // Introduce a delay (e.g., 3 seconds) to avoid rapid successive requests
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    curl_global_cleanup();

    return 0;
}
