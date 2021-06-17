#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <iostream>

int main(int argc , char *argv[])
{
    int max_conn = 5;
    if(argc == 1){
        std::cout << "Default value N=" << max_conn << " clients." << std::endl;
    }else{
        max_conn = atoi(argv[1]);
        std::cout << "N=" << max_conn << " clients." << std::endl;
    }

    int client_socket[max_conn];

    fd_set fd;

    for (int i = 0; i < max_conn; i++)
    {
        client_socket[i] = 0;
    }

    int sfd = socket(AF_INET , SOCK_STREAM , 0);
    if(sfd == -1){
        perror("Server socket: ");
        exit(1);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( 5000 );


    int rv = bind(sfd, (struct sockaddr *)&address, sizeof(address));
    if( rv == -1){
        perror("Bind: ");
        exit(1);
    }


    rv = listen(sfd, 5);
    if (rv == -1)
    {
        perror("Server listen: ");
        exit(1);
    }

    int addrlen = sizeof(address);

    int sd;
    int max_sd;
    while(1)
    {
        FD_ZERO(&fd);
        FD_SET(sfd, &fd);
        max_sd = sfd;

        for (int i = 0 ; i < max_conn ; i++)
        {
            sd = client_socket[i];

            if(sd > 0)
                FD_SET( sd , &fd);

            if(sd > max_sd)
                max_sd = sd;
        }

        int act = select(max_sd + 1 , &fd , NULL , NULL , NULL);
        if(act == 0){
            printf("Server select act: ");
        }


        if(FD_ISSET(sfd, &fd))
        {
            int csfd = accept(sfd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            if(csfd == -1){
                perror("Server accept: ");
                exit(1);
            }

            for (int i = 0; i < max_conn; i++)
            {
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = csfd;
                    break;
                }
            }
        }

        char buffer[100];
        for (int i = 0; i < max_conn; i++)
        {
            sd = client_socket[i];
            if (FD_ISSET( sd , &fd))
            {
                int rr = read( sd , buffer, 100);
                //rozlaczenie
                if(rr  == 0)
                {
                    getpeername(sd , (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    close(sd);
                    client_socket[i] = 0;
                }
                else
                {
                    //echo server
                    buffer[rr] = '\0';
                    send(sd , buffer , strlen(buffer) , 0 );
                }
            }
        }
    }
}