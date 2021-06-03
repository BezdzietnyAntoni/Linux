//
// Created by antoni on 02.06.2021.
//

#include <iostream>
#include "wolframStructur.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

void readDataFromConsole(wolframStruct &);
short parserPort(int, char**);

int main(int argc, char* argv[]){
    short port = parserPort(argc, argv);

    // Create socket
    int sfd = socket( AF_INET, SOCK_DGRAM, 0 );
    if( sfd == -1){
        perror("Error client socket: ");
        exit(1);
    }

    // Create structure to tx
    struct sockaddr_in address_server;
    memset( &address_server, 0, sizeof(address_server) );
    address_server.sin_family = AF_INET;
    address_server.sin_port   = htons(port);
    address_server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Create structure to rx
    struct sockaddr_in address_receiver{};
    memset( &address_receiver, 0, sizeof(address_receiver));
    socklen_t address_len_receiver;

    // Create structure to send
    struct wolframStruct tx_structure{};
    // Response buffer
    const int RX_BUFFER_SIZE = 100;
    char buffer[RX_BUFFER_SIZE];

    while ( true ){
        //Read data from user
        readDataFromConsole(tx_structure);

        //Send message
        int rv = sendto( sfd, reinterpret_cast<const void*>(&tx_structure), sizeof(tx_structure), 0, (const sockaddr*)&address_server, sizeof(address_server));
        if( rv == -1){
            perror("Error client sendto: ");
            exit(1);
        }

        //Wait for response
        memset(&buffer, 0, sizeof(buffer));
        rv = recvfrom( sfd, buffer, RX_BUFFER_SIZE, 0, (sockaddr*)&address_receiver, &address_len_receiver);
        if( rv == -1 ){
            perror("Error client recvfrom: ");
            exit(1);
        }
        buffer[rv] = 0;

        // Display response
        std::cout << buffer << std::endl;
    }
}

short parserPort(int argc, char** argv){
    short port = 5000;

    if(argc > 2){
        std::cout << "Too many arguments !" << std::endl;
        exit(1);
    }

    if(argc == 1){
        return port;
    }

    int arg = std::stoi(argv[1]);
    if(arg<=1024 || arg>=10000){
        std::cout << "Port range (1024 10000)!" << std::endl;
        exit(1);
    }

    return (short)arg;
}

void readDataFromConsole(wolframStruct & wolfram){
    std::string raw_data;
    std::string op1;
    std::string op2;
    int pos_op = -1;
    std::cin >> raw_data;
    std::cout << raw_data << "=";


    int num_non_digital = 0;
    for(int i=0; i<raw_data.size(); ++i){
        if( isdigit(raw_data[i]) ){
            switch (num_non_digital) {
                case 0:
                    op1+=raw_data[i];
                    break;
                case 1:
                    op2+=raw_data[i];
                    break;
                default:
                    std::cout << "Invalid expression" << std::endl;
                    exit(1);
            }
        } else{
            if(i < 1){
                std::cout << "Invalid expression" << std::endl;
                exit(1);
            }

            if(pos_op == -1)
                pos_op = i;

            ++num_non_digital;
        }
    }

    wolfram.op  = raw_data[pos_op];
    wolfram.op1 = std::stoi(op1);
    wolfram.op2 = std::stoi(op2);
}