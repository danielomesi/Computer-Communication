#include "Server.h"
#include "Functions.h"


SocketState sockets[MAX_SOCKETS] = { 0 };
vector<Phrase> phrases;
int socketsCount = 0;


//to avoid globalism
//to change socket array to vector\
//change all strings to char* etc

int main()
{
    Phrase p1, p2, p3, p4;

    p1.id = 1;
    p1.str = "hey";
    p2.id = 2;
    p2.str = "bla";
    p3.id = 3;
    p3.str = "martg";
    p4.id = 4;
    p4.str = "dude";

    phrases.push_back(p1);
    phrases.push_back(p2);
    phrases.push_back(p3);
    phrases.push_back(p4);


    startServer();
    return 0;
}

void startServer()
{
    LoadDLL();
        
    SOCKET listenSocket = GetListenSocket();

    sockaddr_in serverService;
    serverService.sin_family = AF_INET;
    serverService.sin_addr.s_addr = INADDR_ANY;
    serverService.sin_port = htons(TIME_PORT);

    BindServerAddressToListenSocket(listenSocket, serverService);
    ListenForClients(listenSocket);
    addSocket(listenSocket, LISTEN);

    while (true)
    {
        fd_set setOfSocketsInReceiveStatus;
        FD_ZERO(&setOfSocketsInReceiveStatus);
        for (int i = 0; i < MAX_SOCKETS; i++)
        {
            if (sockets[i].isWaitingForClientMsg == true)
                FD_SET(sockets[i].sock, &setOfSocketsInReceiveStatus);
        }

        fd_set setOfSocketsInSendStatus;
        FD_ZERO(&setOfSocketsInSendStatus);
        for (int i = 0; i < MAX_SOCKETS; i++)
        {
            if (sockets[i].isSendNeeded == true)
                FD_SET(sockets[i].sock, &setOfSocketsInSendStatus);
        }

        int countOfWaitingList;
        countOfWaitingList = select(0, &setOfSocketsInReceiveStatus, &setOfSocketsInSendStatus, NULL, NULL);

        if (countOfWaitingList == SOCKET_ERROR)
        {
            cout << "Time Server: Error at select(): " << WSAGetLastError() << endl;
            WSACleanup();
            return;
        }

        for (int i = 0; i < MAX_SOCKETS && countOfWaitingList > 0; i++)
        {
            if (FD_ISSET(sockets[i].sock, &setOfSocketsInReceiveStatus))
            {
                countOfWaitingList--;
                if (sockets[i].socket_type == LISTEN)
                {
                    acceptConnection(i);
                }
                else
                {
                    receiveMessage(i);
                }
            }

            else if ((FD_ISSET(sockets[i].sock, &setOfSocketsInSendStatus)))
            {
                countOfWaitingList--;
                sendMessage(i);
            }
        }
    }

    cout << "Time Server: Closing Connection.\n";
    closesocket(listenSocket);
    WSACleanup();
}

bool addSocket(SOCKET id, int status)
{
    for (int i = 0; i < MAX_SOCKETS; i++)
    {
        if (sockets[i].isWaitingForClientMsg == EMPTY)
        {
            sockets[i].sock = id;
            sockets[i].socket_type = status;
            sockets[i].isWaitingForClientMsg = true;
            sockets[i].isSendNeeded = false;
            sockets[i].len = 0;
            socketsCount++;
            return true;
        }
    }
    return false;
}

void removeSocket(int index)
{
    sockets[index].isWaitingForClientMsg = EMPTY;
    sockets[index].isSendNeeded = EMPTY;
    socketsCount--;
}

void acceptConnection(int index) 
{
    SOCKET sock = sockets[index].sock;
    struct sockaddr_in clientAddressHolder;
    int lenOfClientAddressHolder = sizeof(clientAddressHolder);

    SOCKET clientChannelSocket = accept(sock, (struct sockaddr*)&clientAddressHolder, &lenOfClientAddressHolder);
    if (INVALID_SOCKET == clientChannelSocket)
 {
        cout << "Time Server: Error at accept(): " << WSAGetLastError() << endl;
        return;
    }
    cout << "Time Server: Client " << inet_ntoa(clientAddressHolder.sin_addr) << ":" << ntohs(clientAddressHolder.sin_port) << " is connected." << endl;

    unsigned long flag = 1;
    if (ioctlsocket(clientChannelSocket, FIONBIO, &flag) != 0) {
        cout << "Time Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
    }

    if (!addSocket(clientChannelSocket, CLIENT_CHANNEL))
    {
        cout << "\t\tToo many connections, dropped!\n";
        closesocket(sock);
    }
}

void receiveMessage(int index)
{
    SOCKET sock = sockets[index].sock;
    int len = sockets[index].len;
    int bytesRecv = recv(sock, &sockets[index].buffer[len], sizeof(sockets[index].buffer) - len, 0);

    if (SOCKET_ERROR == bytesRecv) {
        cout << "Time Server: Error at recv(): " << WSAGetLastError() << endl;
        closesocket(sock);
        removeSocket(index);
        return;
    }
    if (bytesRecv == 0) {
        closesocket(sock);
        removeSocket(index);
        return;
    }
    else
    {
        sockets[index].buffer[len + bytesRecv] = '\0';
        cout << "Time Server: Received: " << bytesRecv << " bytes of:" << endl;
        cout << "---start---" << endl;
        cout<<&sockets[index].buffer[len] << endl;
        cout << "---end---" << endl;
        sockets[index].len += bytesRecv;

        if (sockets[index].len > 0)
        {
            handleHttpRequest(index);
        }
    }
}

void sendMessage(int index) {
    SOCKET msgSocket = sockets[index].sock;
    int bytesSent = send(msgSocket, sockets[index].buffer, sockets[index].len, 0);
    if (SOCKET_ERROR == bytesSent) {
        cout << "Time Server: Error at send(): " << WSAGetLastError() << endl;
        return;
    }

    cout << "Time Server: Sent: " << bytesSent << " bytes of:" << endl;
    cout << "---start---" << endl;
    cout<<sockets[index].buffer<<endl;
    cout << "---end---" << endl;
    sockets[index].isSendNeeded = false;
}

void handleHttpRequest(int socketIndex)
{
    char method[8], path[256], version[16];
    sscanf(sockets[socketIndex].buffer, "%s %s %s", method, path, version);

    if (strcmp(method, "GET") == 0)
    {
        handleGetRequest(socketIndex, path);
    }
    else if (strcmp(method, "POST") == 0) 
    {
        handlePostRequest(socketIndex, path);
    }
    else if (strcmp(method, "PUT") == 0)
    {
        handlePutRequest(socketIndex, path);
    }
    else if (strcmp(method, "DELETE") == 0) 
    {
        handleDeleteRequest(socketIndex, path);
    }
    else if (strcmp(method, "HEAD") == 0) 
    {
        handleHeadRequest(socketIndex, path);
    }
    else if (strcmp(method, "OPTIONS") == 0)
    {
        constructHttpResponse(socketIndex, 200, "OK", GenerateOptionsMenu(), true);
    }
    else if (strcmp(method, "TRACE") == 0) {
        handleTraceRequest(socketIndex, path);
    }
    else {
        constructHttpResponse(socketIndex, 501, "Not Implemented", "This server only supports GET and POST.", true);
    }
}

void handleGetRequest(int socketIndex, const char* path) 
{
    const char* lang = GetLangQueryParam(path);
    string htmlBody = GenerteHTMLBody(lang);
            
    constructHttpResponse(socketIndex, 200, "OK", htmlBody.c_str(), true);

}

void handlePostRequest(int socketIndex, const char* path) {
    const char* body = strstr(sockets[socketIndex].buffer, "\r\n\r\n");
    if (body != NULL) {
        body += 4;  // Skip over the \r\n\r\n
        AddPhrase(body);
        constructHttpResponse(socketIndex, 200, "OK", "Added successfully", true);
    }
    else 
    {
        constructHttpResponse(socketIndex, 400, "Bad Request", "No string in the request.", true);
    }
}

void handleTraceRequest(int socketIndex, const char* path) 
{
    const char* body = strstr(sockets[socketIndex].buffer, "\r\n\r\n");
    if (body != NULL)
    {
        body += 4;  // Skip over the \r\n\r\n
    }
    else
    {
        body = EMPTY_STRING;
    }
    constructHttpResponse(socketIndex, 200, "OK", body, true);
}

void handleDeleteRequest(int socketIndex, const char* path)
{
    int idToDelete = GetIdQueryParam(path);

    if (idToDelete > 0 && RemovePhrase(idToDelete))
    {
        constructHttpResponse(socketIndex, 200, "OK", "Removed Succesfully", true);
    }
    else
    {
        constructHttpResponse(socketIndex, 404, "Not Found", "The requested resource was not found on this server.", true);
    }
}

void handlePutRequest(int socketIndex, const char* path)
{
    int idToUpdate = GetIdQueryParam(path);

    if (idToUpdate > 0 && IsInPhrases(idToUpdate))
    {
        const char* body = strstr(sockets[socketIndex].buffer, "\r\n\r\n");
        if (body != NULL)
        {
            body += 4;  // Skip over the \r\n\r\n
            UpdatePhrase(idToUpdate, constConverter(body));
            constructHttpResponse(socketIndex, 200, "OK", "Updated successfully", true);
        }
        else
        {
            constructHttpResponse(socketIndex, 400, "Bad Request", "No string in the request.", true);
        }
    }
    else
    {
        constructHttpResponse(socketIndex, 404, "Not Found", "The requested resource was not found on this server.", true);
    }

}

void handleHeadRequest(int socketIndex, const char* path)
{
    const char* lang = GetLangQueryParam(path);
    string htmlBody = GenerteHTMLBody(lang);

    constructHttpResponse(socketIndex, 200, "OK", htmlBody.c_str(), false);
}



void constructHttpResponse(int index, int statusCode, const char* statusMessage, const char* responseBody, bool isSendingBody, const char* lang)
{
    char* mutuableResponseBody;
    char empty[1] = "";

    if (isSendingBody)
    {
        mutuableResponseBody = const_cast<char*>(responseBody);
    }
    else
    {
        mutuableResponseBody = empty;
    }

    snprintf(sockets[index].buffer, sizeof(sockets[index].buffer),
    "HTTP/1.1 %d %s\r\n"
    "Content-Length: %zu\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Language: %s\r\n"
    "Connection: close\r\n"
    "\r\n"
    "%s",
    statusCode, statusMessage, strlen(responseBody), lang, mutuableResponseBody);

    sockets[index].len = strlen(sockets[index].buffer);
    sockets[index].isSendNeeded = true;
}

const char* GetLangQueryParam(const char* path) 
{
    const char* langParamKey = "lang=";
    int lenToSkip = strlen(langParamKey);

    const char* lang = strstr(path, "lang=");
    char res[3];
    if (lang != NULL) 
    {
        lang += lenToSkip;
        if (strncmp(lang, "en", 2) == 0 || strncmp(lang, "he", 2) == 0 || strncmp(lang, "fr", 2) == 0) {
            strncpy(const_cast<char*>(res), lang, 2);
            res[2] = '\0';
            return res;
        }
    }
    return DEFAULT_LANGUAGE;
}

int GetIdQueryParam(const char* path)
{
    int idNum;
    const char* idParamKey = "id=";
    int lenToSkip = strlen(idParamKey);

    const char* id = strstr(path, idParamKey);
    
    if (id != NULL)
    {
        id += lenToSkip;
    }

    idNum = atoi(id);

    return idNum;
}

void LoadDLL()
{
    WSAData wsaData;
    if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        cout << "Time Server: Error at WSAStartup()"<<endl;
        PerformExit();
    }
}

void PerformExit()
{
    cout << "An error occured, exiting..." << endl;
}

SOCKET GetListenSocket()
{
    SOCKET resSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == resSocket)
    {
        cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        PerformExit();
    }

    return resSocket;
}

void BindServerAddressToListenSocket(SOCKET& listenSocket, sockaddr_in& serverService)
{
    if (SOCKET_ERROR == bind(listenSocket, (SOCKADDR*)&serverService, sizeof(serverService))) {
        cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        PerformExit();
    }
}

void ListenForClients(SOCKET& listenSocket)
{
    if (SOCKET_ERROR == listen(listenSocket, 5))
    {
        cout << "Time Server: Error at listen(): " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        PerformExit();
    }
}

char* GenerateOptionsMenu() 
{
    const char* menu =
        "This server holds a simple array of items, each with a unique ID and string.\n"
        "The server offers the following options:\n"
        "[GET] - Retrieves HTML text displaying a welcome message, list of current items, and a timestamp. (text/html)\n"
        "   Note: You can specify the language with the 'lang' query parameter (e.g., lang=en).\n"
        "[POST] - Adds new data to the server. The request body should contain the string to be added.\n"
        "[PUT] - Updates existing data. Requires an 'id' query parameter with the ID of the item to update, and a body with the new string.\n"
        "[DELETE] - Deletes an item. Requires an 'id' query parameter with the ID of the item to delete.\n"
        "[HEAD] - Same as GET request, but returns only the header without the body.\n"
        "[TRACE] - Returns the same body as the request body (echo).\n"
        "[OPTIONS] - Lists all available options.\n";

    // Calculate the length of the menu string
    size_t length = strlen(menu) + 1; // +1 for the null terminator

    // Allocate memory for the new string
    char* result = new char[length];

    // Copy the menu string into the result buffer
    strcpy(result, menu);

    // Return the result
    return result;
}




