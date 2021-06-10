#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cstring>
#include <sstream>
#include <unistd.h>


int main() {

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if( sfd == -1 ){
        perror("Client socket: ");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(5000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int rv = connect ( sfd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    if( rv == -1){
        perror("Client connect: ");
        exit(1);
    }

    for(int i=0; i<10; ++i){
        std::stringstream ss;
        ss << "Wiadomosc numer " << i << std::endl;

        std::string  tmp = ss.str();

        rv = send(sfd, tmp.c_str(), tmp.length(), 0);
        if( rv == -1){
            perror("Client send: ");
            exit(1);
        }


        char buf[100];
        rv = recv (sfd, buf, 100-1, 0);
        buf[rv] = 0;
        std::cout << rv << " " << buf << std::endl;
        sleep(1);

    }
    close(sfd);




    return 0;
}
