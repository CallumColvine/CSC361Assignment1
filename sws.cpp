extern "C" {
    #include <stdio.h>
    #include <errno.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <unistd.h> /* for close() for socket */ 
    // #include <stdlib.h>
}
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>

bool checkArguments(int argc, char* argv[]){
    // Case where there's too few/too many arguments
    if (argc != 2)
    {
        return false;
    }
    if (std::all_of(argv[1].begin(), argv[1].end(), ::isdigit) == false)
    {
        std::cout << "The string is not made of numbers" << std::endl;
        return false;
    }
    // for (int i = 1; i < argc; i++)
    // {
    //     std::cout << argv[i] << std::endl;
    // }
    return true;
}

void exitProgramGracefully(){

}
// UDP Server
int main(int argc, char *argv[])
{
    // std::cout << "array size total is " << sizeof(argv) << std::endl;
    // std::cout << "element size is " << sizeof(argv[0]) << std::endl;
    // std::cout << "division is " << (sizeof(argv)/sizeof(argv[0])) << std::endl;
    // int argvLen = (sizeof(argv)/sizeof(argv[0])); 


    bool argsGood = checkArguments(argc, argv);
    std::cout << "Are arguments okay? " << argsGood << std::endl;
    if (argsGood == false){
        exitProgramGracefully()
    } 

    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in sa; 
    char buffer[1024];
    ssize_t recsize;
    socklen_t fromlen;

    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons(8080);
    fromlen = sizeof(sa);

    if (-1 == bind(sock, (struct sockaddr *)&sa, sizeof sa)) {
        perror("error bind failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    for (;;) {
        recsize = recvfrom(sock, (void*)buffer, sizeof buffer, 0, (struct sockaddr*)&sa, &fromlen);
        if (recsize < 0) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("datagram: %.*s\n", (int)recsize, buffer);
    }
}

