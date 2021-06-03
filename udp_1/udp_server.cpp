//
// Created by antoni on 02.06.2021.
//
#include <iostream>
#include "wolframStructur.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>

void calculate(wolframStruct*, char[]);
short parserPort(int, char**);

int main(int argc, char* argv[]){
    short port = parserPort(argc, argv);

    // Create socket
    int sfd = socket( AF_INET, SOCK_DGRAM, 0 );
    if( sfd == -1){
        perror("Error server socket: ");
        exit(1);
    }

    // Structure for rx
    sockaddr_in address_local{};
    memset(&address_local, 0, sizeof(address_local));
    address_local.sin_family = AF_INET;
    address_local.sin_port   = htons(port);
    address_local.sin_addr.s_addr = INADDR_ANY;

    // Structure for tx
    sockaddr_in tx_address;
    socklen_t tx_address_len;
    memset(&tx_address, 0, sizeof(tx_address));

    // Bind
    int rv = bind(sfd, (const struct sockaddr*)&address_local, sizeof(address_local));
    if (rv == -1)
    {
        perror("UDP server bind");
        exit(1);
    }

    //Receive structure
    void *receive;
    wolframStruct *receive_struct;
    const int BUFF_SIZE = 100;
    char response[BUFF_SIZE];

    while (true){
        rv = recvfrom ( sfd, receive, sizeof(wolframStruct), 0,
                        (sockaddr *)&tx_address, &tx_address_len
        );
        if (rv == -1)
        {
            perror("Error server recvfrom: ");
            exit(1);
        }

        receive_struct = reinterpret_cast<wolframStruct*>(receive);
        memset(&response, 0, sizeof(response));

        calculate(receive_struct, response);

        rv = sendto( sfd, response, BUFF_SIZE, 0, (const struct sockaddr *)&tx_address, tx_address_len);
        if (rv == -1)
        {
            perror("Error server sendto: ");
            exit(1);
        }

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

void calculate(wolframStruct* wolf, char resp []){
    long result = 0;
    switch (wolf->op) {
        case '+':
            result = wolf->op1 + wolf->op2;
            break;
        case '-':
            result = wolf->op1 - wolf->op2;
            break;
        case '*':
            result = wolf->op1 * wolf->op2;
            break;
        case '/':
            if(wolf->op2 == 0) {
                strncpy(resp, "Divide by 0!", 12);
                break;
            }
            result = wolf->op1 / wolf->op2;
        default:
            strncpy(resp, "Unknown expression!", 19);
    }
    if(result != 0){
        sprintf(resp, "%1ld", result);
    }
}
