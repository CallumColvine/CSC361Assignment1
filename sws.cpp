// Callum Colvine
// V00789488
// Code for ports is from the lab slides

extern "C" {
    // #include <stdio.h>
    #include <errno.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <sys/select.h>
    #include <netinet/in.h>
    #include <unistd.h> /* for close() for socket */
    #include <dirent.h>
    // #include <stdlib.h>
    #include <pthread.h>
    #include <arpa/inet.h>
}

// #include <ctime>
#include <typeinfo>
#include <sstream>
#include <string>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <locale>
#include <iomanip>
#include <fstream>
#include <ios>


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

bool validPathString(std::string s){
    std::ifstream inFile(s);
    if (inFile.good())
    {
        inFile.close();
        return true;
    } else {
        return false;
    }
}

bool validPath(const char *s){
    std::cout << "checking if " << s << " is valid " << std::endl;
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
    if (validPath(argv[2]) == false)
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
    exit (EXIT_FAILURE);
}

// bool checkInput(){
//     char input = 'x';
//     std::cin >> input;
//     if (input == 'q')
//     {
//         // std::cout << "input is bad" << std::endl;
//         return false;
//     }
//     // std::cout << "input okay" << std::endl;
//     return true;
// }

// Used as thread method
// void* monitorInput(void *threadid){
//     bool exitAndRelese = false;
//     for(;;){
//         char input = 'x';
//         std::cin >> input;
//         if (input == 'q')
//         {
//             // std::cout << "input is bad" << std::endl;
//             exitAndRelese = true;
//         }
//         // std::cout << "input okay" << std::endl;
//         if (exitAndRelese == true){
//             std::cout << "Exit program" << std::endl;
//             exit (EXIT_FAILURE);
//         }
//     }
//     pthread_exit(NULL);
// }

std::vector<std::string> splitString(std::string input)
{
    std::istringstream iss(input);
    std::vector<std::string> dateVector;
    do
    {
        std::string sub;
        iss >> sub;
        dateVector.push_back(sub);

    } while (iss);
    return dateVector;
}

std::string getTime(){
    time_t tobj;
    time(&tobj);
    // struct tm * now = localtime(&tobj);
    // ctime (&rawtime)
    std::string now = ctime(&tobj);
    // std::cout << now << std::endl;
    std::stringstream returnTimeS;
    std::vector<std::string> splitTime = splitString(now);
    returnTimeS << splitTime[1] << " " << splitTime[2] << " " <<
        splitTime[3];
    std::string returnTime = returnTimeS.str();
    return returnTime;
}

// Multithreading instead of select() for input filtering
void useThreads(){
    // pthread_t threads[1];
    // int rc;
    // rc = pthread_create(&threads[0], NULL, monitorInput, nullptr);
    // if (rc){
    //     std::cout << "Error:unable to create thread," << rc << std::endl;
    //     exit(-1);
    // }
}

// May need socket passed in
void gracefulShutdown(){
    // Releases socket
    std::cout << "Exit program through graceful shutdown" << std::endl;
    exit (EXIT_FAILURE);
}

// Make this return a bool so I can cleanup before calling gracefulShutdown()
void checkInput(int servSocket){
    char input;
    std::cin >> input;
    if (input == 'q')
    {
        close(servSocket);
        gracefulShutdown();
    } else {
        std::cout << "Invalid input" << std::endl;
    }
}

std::string return400(){
    return "HTTP/1.0 400 Bad Request\n";
}

std::string return404(){
    return "HTTP/1.0 404 Not Found\n";
}

std::string return200(){
    return "HTTP/1.0 200 OK\n";
}

// Adding 'index.html' to filename
std::string formatFilename(std::string inName, std::string curPath){
    std::string readFileName = inName;
    if (inName.back() == '/')
    {
        readFileName.append("index.html");
    }
    curPath.pop_back();
    readFileName = curPath + readFileName;
    return readFileName;
}

// ToDo: test if I can open a file I don't have permissions for
// Don't think I can though
int analyzeRequest(char* buffer, std::string curPath){
    std::vector<std::string> splitReq = splitString(buffer);
    if (splitReq[0] != "GET")
    {
        return 400;
    } else if (splitReq[2] != "HTTP/1.0"){
        return 400;
    }
    std::string readFileName = formatFilename(splitReq[1], curPath);
    // std::cout << "readFileName is " << readFileName << std::endl;
    if (!validPathString(readFileName)){
        return 404;
    } else {
        return 200;
    }

}

void sendFileContents(int senderSock, struct sockaddr_in sa,
                      std::string readFileName){
    int bytes_sent;
    int bytes_read;
    char buffer[200];
    char *fileContents = new char [200];
    // Sending the first message
    std::string firstPart = "HTTP/1.0 200 OK\n";
    strcpy(buffer, firstPart.c_str());
    bytes_sent = sendto(senderSock, buffer, strlen(buffer), 0,
            (struct sockaddr*)&sa, sizeof sa);
    if (bytes_sent < 0) {
        printf("Error sending packet: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // Sending file contents
    std::ifstream inFile(readFileName, std::ios::binary | std::ios::ate);
    int fileLen = inFile.tellg();
    inFile.close();
    FILE * inFileC = fopen(readFileName.c_str(), "r");
    if (fileLen <= 200){
        std::cout << "Sending message of len < 200" << std::endl;
        // inFile.read(fileContents, fileLen);
        bytes_read = fread(fileContents, sizeof(char), fileLen, inFileC);

        std::cout << "Buffer is " << fileContents <<
                     "fileLen is " << fileLen << std::endl;
        bytes_sent = sendto(senderSock, fileContents, strlen(fileContents), 0,
                (struct sockaddr*)&sa, sizeof sa);
        if (bytes_sent < 0) {
            printf("Error sending packet: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    } else {
        std::cout << "Sending message of len > 200" << std::endl;
        for (int i = 0; i < fileLen; i += 200)
        {

            inFile.read(buffer, 200);
            bytes_sent = sendto(senderSock, buffer, strlen(buffer), 0,
                    (struct sockaddr*)&sa, sizeof sa);
            if (bytes_sent < 0) {
                printf("Error sending packet: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }
    // fclose(inFile);

}

void generateResponse(int code, int port, std::string senderIP, int senderSock,
                      struct sockaddr_in sa, std::string readFileName){
    int bytes_sent;
    char buffer[200];
    std::cout << "Sender IP is " << senderIP << std::endl;
    std::cout << "Sender port is " << port << std::endl;
    if (code == 404){
        std::cout << "Sending 404 Not Found to sender" << std::endl;
        strcpy(buffer, "HTTP/1.0 404 Not Found\n");
        bytes_sent = sendto(senderSock, buffer, strlen(buffer), 0,
                (struct sockaddr*)&sa, sizeof sa);
        if (bytes_sent < 0) {
            printf("Error sending packet: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    } else if (code == 400){
        std::cout << "Sending 400 Not Found to sender" << std::endl;
        strcpy(buffer, "HTTP/1.0 400 Bad Request\n");
        bytes_sent = sendto(senderSock, buffer, strlen(buffer), 0,
                (struct sockaddr*)&sa, sizeof sa);
        if (bytes_sent < 0) {
            printf("Error sending packet: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    } else {
        std::cout << "Sending 200 OK to sender" << std::endl;
        sendFileContents(senderSock, sa, readFileName);
    }
}

// Method for trimming the last element of a string from
// http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

void portStuff(char* curPath, int port){
    // ToDo: Check return values for all socket related functions
    int servSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    // int enable = 1;
    int optval = 1;
    int sockOpt = setsockopt(servSocket, SOL_SOCKET, SO_REUSEADDR, &optval,
             sizeof optval);
    if (sockOpt < 0)
    {
        perror("Error setting socket option");
    }
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

    if (-1 == bind(servSocket, (struct sockaddr *)&sa, sizeof sa)) {
        perror("Error server socket bind failed");
        close(servSocket);
        exit(EXIT_FAILURE);
    }
    // For select()
    fd_set read_fds;
    // Unspecified GET looks in the running directory for index.html
    // GET / HTTP/1.0
    // Specified GET looks in the folder for the specified file
    // GET /icons/new.gif
    // int i = 0;
    for (;;) {
        // use select() for this part to look at both inputs
            // stdin keyboard and recvfrom buffer
        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(servSocket, &read_fds);

        if (select(servSocket + 1, &read_fds, 0, 0, 0) == -1)
        {
            std::cout << "Select error" << std::endl;
            perror("Failed to select an input");
            gracefulShutdown();
        } if (FD_ISSET(0, &read_fds)){
            checkInput(servSocket);
        } if (FD_ISSET(servSocket, &read_fds)){
            // ToDo: get port # form recvfrom (2nd last element)
            // sendto with C++ strings
            // http://stackoverflow.com/questions/19535978/c-using-sendto-with-a-basic-string
            recsize = recvfrom(servSocket, (void*)buffer, sizeof buffer, 0,
                    (struct sockaddr*)&sa, &fromlen);
            if (recsize < 0) {
                fprintf(stderr, "%s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            std::string cstrBuffer = std::string(buffer);
            cstrBuffer.resize(recsize);
            cstrBuffer = rtrim(cstrBuffer);
            // std::stringstream inputRequestS = std::setw(recsize) << buffer;
            // std::cout << "Full input request is " << inputRequestS.str() << std::endl;
            // Getting time of message arrival
            std::string timePart = getTime();
            // Getting address and port of message sender
            char senderIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(sa.sin_addr), senderIP, INET_ADDRSTRLEN);
            // Analyze input
            int response = analyzeRequest(buffer, std::string(curPath));
            std::string responseCode;
            std::string readFileName;
            if (response == 400){
                responseCode = return400();
                responseCode = rtrim(responseCode);
                responseCode += ";";
            }
            else if (response == 404){
                responseCode = return404();
                responseCode = rtrim(responseCode);
                responseCode += ";";
            }
            else {
                std::vector<std::string> splitReq = splitString(buffer);
                readFileName = formatFilename(splitReq[1], curPath);
                responseCode = return200();
                responseCode = rtrim(responseCode);
                responseCode = responseCode + "; " + readFileName;
            }
            // Log Message
            std::cout << timePart << ' ' << senderIP << ':' <<
                    htons(sa.sin_port) << "; " << cstrBuffer << "; " <<
                    responseCode << std::endl;
            generateResponse(response, port, senderIP, servSocket, sa,
                             readFileName);
            // i ++;
            // if (i == 20) break;
            // getAddrAndPort()
            // std::cout << str << std::endl;
        }

    }
    close(servSocket);
}


    // time_t t = time(0);   // get time now
    // cout << (now->tm_year + 1900) << '-'
    //      << (now->tm_mon + 1) << '-'
    //      <<  now->tm_mday
    //      << endl;

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

    portStuff(inPath, portNum);

}
