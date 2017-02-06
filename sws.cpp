extern "C" {
    #include <stdio.h>
    #include <errno.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <unistd.h> /* for close() for socket */ 
    #include <dirent.h>
    // #include <stdlib.h>
    #include <pthread.h>
}
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <locale> 


bool numOnly(const char *s)
{
    while (*s) {
        if (isdigit(*s++) == 0){
            std::cout << "Is not digits" << std::endl;
            return false;
        } 
    }
    return true;
}

bool notValidPath(const char *s){
    DIR* attemptDir = opendir(s);
    if (attemptDir)
    {
        closedir(attemptDir);
        return true;
    }
    else if (ENOENT == errno)
    {
        return false;
    }
    else {       
        return false;
    }
}

bool checkArguments(int argc, char* argv[]){
    // Case where there's too few/too many arguments
    if (argc != 3)
    {
        std::cout << "Wrong number of input arguments" << std::endl;
        return false;
    }
    // Case where the port # is not a numebr 
    if (numOnly(argv[1]) == false)
    {
        std::cout << "The string is not made of numbers" << std::endl;
        return false;
    }
    // Case where the path is not valid 
    if (notValidPath(argv[2]) == false)
    {
        std::cout << "The path is not valid" << std::endl;        
        return false;
    }

    return true;
}

void exitProgramGracefully(){
    std::cout << 
        "Invalid input \nPlease use input in the form: ./sws 8080 testdir/ " 
        << std::endl;
}

bool checkInput(){
    char input = 'x';
    std::cin >> input;
    if (input == 'q')
    {
        // std::cout << "input is bad" << std::endl;
        return false;
    }
    // std::cout << "input okay" << std::endl;
    return true;
}

void* monitorInput(void *threadid){
    bool exitAndRelese = false;
    for(;;){
        char input = 'x';
        std::cin >> input;
        if (input == 'q')
        {
            // std::cout << "input is bad" << std::endl;
            exitAndRelese = true;
        }
        // std::cout << "input okay" << std::endl;
        if (exitAndRelese == true){
            std::cout << "Exit program" << std::endl;
            exit (EXIT_FAILURE);
        }
    }
    pthread_exit(NULL);
}

void portStuff(){
    // Thread stuff
    pthread_t threads[1];
    int rc;
    rc = pthread_create(&threads[0], NULL, monitorInput, nullptr);
    if (rc){
        std::cout << "Error:unable to create thread," << rc << std::endl;
        exit(-1);
    }


    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    // int enable = 1;
    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval); 
// error("setsockopt(SO_REUSEADDR) failed"));
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

// UDP Server
int main(int argc, char *argv[])
{
    // std::cout << "array size total is " << sizeof(argv) << std::endl;
    // std::cout << "element size is " << sizeof(argv[0]) << std::endl;
    // std::cout << "division is " << (sizeof(argv)/sizeof(argv[0])) << std::endl;
    // int argvLen = (sizeof(argv)/sizeof(argv[0])); 


    bool argsGood = checkArguments(argc, argv);
    if (argsGood == false){
        exitProgramGracefully();
    }
    int portNum = atoi(argv[1]);
    char* inPath = argv[2];
    std::cout << "\nsws is running on UDP port " << portNum << 
        " and serving " << inPath << "\npress ‘q’ to quit ..." << std::endl;

    portStuff();

}

