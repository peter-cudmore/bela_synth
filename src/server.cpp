#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <regex>
#include <arpa/inet.h>

#include "patch.h"
#include "server.h"
//
// Created by Pete on 9/09/2023.
//


extern int gShouldStop;

std::string handle_request(std::string request);

class Server {
public:
    Server(int port=8080, unsigned int timeout=1): timeout(timeout),
    port(port){}

    ~Server() {
        stop();
    }

    bool try_start(int max_connections = 1){

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;// htonl converts a long integer (e.g. address) to a network representation INADDR_ANY chooses any IP, WiFi or Ethernet.
        address.sin_port = htons(port);      /* htons converts a short integer (e.g. port) to a network representation */
        memset(address.sin_zero, '\0', sizeof address.sin_zero);

        if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){ //AF_INET = IPv4
            std::cerr << "Cannot create socket for server\n";
            server_fd = -1;
            return false;
        }

        struct timeval tv;
        tv.tv_sec = timeout;
        tv.tv_usec = 0;
        setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
            std::cerr << "Cannot bind server socket\n";
            stop();
            return false;
        }

        if(listen(server_fd, max_connections) != 0){
            stop();
            std::cerr << "Cannot listen to socket\n";
            return false;
        }
        return true;
    }

    void stop() {
        if (server_fd > 0) {
            if (close(server_fd) == -1){
                std::cerr << "Failed to close server socket.";
            }
            server_fd = 0;
        }
    }

    int wait_for_client(){
        if (server_fd == 0) {
            std::cerr << "No server started" << std::endl;
            return -1;
        }
        int addrlen = sizeof(address);
        int fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        return fd;
    }

private:
    struct sockaddr_in address;
    int port = 80;
    int server_fd = 0;
    unsigned int timeout;
};


void run_server(){

    int fd;
    const size_t buffersize = 524288;	//somehow larger requests stop at 524288 bytes
    char buffer[buffersize];
    int bytes_read;
    Server server {8080, 10};

    if (!server.try_start()){
        std::cerr << "Failed to start webserver\n";
        return;
    }

    while (!gShouldStop) {
        if ((fd = server.wait_for_client()) < 0){
            std::cerr  << "Connection refused\n";
            continue;
        }

        if((bytes_read = recv(fd, buffer, buffersize, 0)) > 4) {
            std::cout << "Read " << bytes_read << " bytes\n";
            std::string request = std::string (buffer);
            std::string response = handle_request(request);
            send(fd, response.c_str(), response.length(), 0);
        }
        close(fd);
    }
}

std::string handle_put(std::string path) {
    

    return std::string("HTTP/1.1 404 Not Found");
}

std::string handle_get(std::string path) {
    json object = patch;

    std::string response = std::string("HTTP/1.1 ");

    std::regex reg("[\\/]");

    std::sregex_token_iterator iter(path.begin(), path.end(), reg, -1);
    std::sregex_token_iterator end;
    iter++;
    std::vector<std::string> vec(iter, end);

    for (auto &a: vec) {
        std::cout << "Token: [" << a <<"]";
        if (object.contains(a)){
            object = object.at(a);
        } else {
            return std::string("HTTP/1.1 404 Not Found");
        }
    }

    response += "200 OK\n";
    response += "Content-Type: application/json\r\n";

    std::string contents = object.dump();
    response += "Content-Length: ";
    response += std::to_string(contents.length());
    response += "\n\n";
    response += contents;

    return response;
}

std::string handle_request(std::string request) {

    std::string filepath = request.substr(4, request.find(" ", 4) - 4);

    if (request.substr(0,3) == "GET") {
        return handle_get(filepath);
    } else if (request.substr(0,3) == "PUT") {
        return handle_put(filepath);
    } else {
        return std::string("HTTP/1.1 501 Unsupported");
    }
}