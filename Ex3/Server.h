#pragma once
#pragma comment(lib, "Ws2_32.lib")
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

#define TIME_PORT 8080
#define MAX_SOCKETS 60
#define MAX_PHRASES 100
#define EMPTY 0
#define LISTEN 1
#define CLIENT_CHANNEL 2
#define GET 1
#define POST 2
#define HTML_FILE_NAME "index.html"
#define HTML_FILE_DIRECTORY_PATH "C:\\Temp"
#define DEFAULT_LANG "he"
#define EMPTY_STRING ""
#define BUFFER_SIZE 8192

typedef struct SocketState
{
    SOCKET sock;                            
    int socket_type;                        //  "LISTEN" or "CLIENT_CHANNEL"
    bool isWaitingForClientMsg;             
    bool isSendNeeded;                      
    char buffer[BUFFER_SIZE];   
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

void StartServer();
bool AddSocket(SOCKET id, int what);
void RemoveSocket(int index);
void AcceptConnection(int index);
void ReceiveMessage(int index);
void SendAMessage(int index);
void LoadDLL();
void PerformExit();
SOCKET GetListenSocket();
void BindServerAddressToListenSocket(SOCKET& listenSocket, sockaddr_in& serverService);
void ListenForClients(SOCKET& listenSocket);
char* GenerateOptionsMenu();

//HTTP METHODS HANDLERS
void HandleGetRequest(int index, const char* path);
void HandlePostRequest(int index, const char* path);
void HandleDeleteRequest(int socketIndex, const char* path);
void HandlePutRequest(int socketIndex, const char* path);
void HandleHeadRequest(int socketIndex, const char* path);
void HandleTraceRequest(int socketIndex, const char* path);

//HTTP LOGIC HELP FUNCS
void HandleHttpRequest(int index);
void ConstructHttpResponse(int index, int statusCode, const char* statusMessage, const char* responseBody, bool isDynamicallyAllocated, bool isHeadersOnly, const char* lang = DEFAULT_LANG);
const char* GetLangQueryParam(const char* path);
int GetIdQueryParam(const char* path);


