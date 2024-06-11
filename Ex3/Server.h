#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <time.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <codecvt>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define TIME_PORT 27015
#define MAX_SOCKETS 60
#define MAX_PHRASES 100
#define EMPTY 0
#define LISTEN 1
#define CLIENT_CHANNEL 2
#define GET 1
#define POST 2
#define HTML_FILE_NAME "website.html"
#define DEFAULT_LANGUAGE "en"
#define EMPTY_STRING ""
#define BUFFER_SIZE 8192

typedef struct SocketState
{
    SOCKET sock;            // Socket handle
    int socket_type;
    bool isWaitingForClientMsg;             // Receiving?
    bool isSendNeeded;             // Sending?
    int sendSubType;      // Sending sub-type
    char buffer[BUFFER_SIZE];    // Increase buffer size for HTTP requests
    int len;
} SocketState;

typedef struct Phrase
{
    int id;
    string str;
} Phrase;

extern SocketState sockets[MAX_SOCKETS];
extern vector<Phrase> phrases;
extern int socketsCount;

void startServer();
bool addSocket(SOCKET id, int what);
void removeSocket(int index);
void acceptConnection(int index);
void receiveMessage(int index);
void sendMessage(int index);
void handleHttpRequest(int index);
void handleGetRequest(int index, const char* path);
void handlePostRequest(int index, const char* path);
void handleDeleteRequest(int socketIndex, const char* path);
void handlePutRequest(int socketIndex, const char* path);
void handleHeadRequest(int socketIndex, const char* path);
void handleTraceRequest(int socketIndex, const char* path);
void constructHttpResponse(int index, int statusCode, const char* statusMessage, const char* responseBody, bool isHeadersOnly, const char* lang = DEFAULT_LANGUAGE);
const char* GetLangQueryParam(const char* path);
int GetIdQueryParam(const char* path);
void LoadDLL();
void PerformExit();
SOCKET GetListenSocket();
void BindServerAddressToListenSocket(SOCKET& listenSocket, sockaddr_in& serverService);
void ListenForClients(SOCKET& listenSocket);
char* GenerateOptionsMenu();


