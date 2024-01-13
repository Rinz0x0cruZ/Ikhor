#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <curl/curl.h>

using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

string PerformRequest(const string& method, const string& ip, unsigned int port, const string& uri, const string& data) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Error initializing cURL." << endl;
        return "";
    }

    string protocol = "http";  // Adjust the protocol as needed
    string url = protocol + "://" + ip + ":" + to_string(port) + uri;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    if (method == "post") {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    }

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

    int port = stoi(argv[2]);
    string ip=string(argv[1]);
    string uri = "";  // Change this to the actual URI on your server

    curl_global_init(CURL_GLOBAL_DEFAULT);

    string serverResponse;

    // Send initial message to inform the server that the client is alive
    PerformRequest("post", ip, port, uri, "Client is alive");

restart_loop:

    // Check if the server has sent something
    serverResponse = PerformRequest("get", ip, port, uri, "");

    if (!serverResponse.empty()) {
        // Inform the server that the client received the message
        PerformRequest("post", ip, port, uri, "Received: " + serverResponse);
    } else {
        // If no message, sleep for 3 seconds
        this_thread::sleep_for(chrono::seconds(3));
    }

    // End the loop if the server sends "Kill Client"
    if (serverResponse != "Kill Client") {
        goto restart_loop;
    }

    curl_global_cleanup();

    cout << "Client shutting down." << endl;

    return 0;
}