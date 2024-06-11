#include "Server.h"
#include "Functions.h"


SocketState sockets[MAX_SOCKETS] = { 0 };
vector<Phrase> phrases;
int socketsCount = 0;

//to check the 2 minutes thing in the task

int main()
{
    StartServer();
    return 0;
}

void StartServer()
{
    LoadDLL();
        
    SOCKET listenSocket = GetListenSocket();

    sockaddr_in serverService;
    serverService.sin_family = AF_INET;
    serverService.sin_addr.s_addr = INADDR_ANY;
    serverService.sin_port = htons(TIME_PORT);

    BindServerAddressToListenSocket(listenSocket, serverService);
    ListenForClients(listenSocket);
    AddSocket(listenSocket, LISTEN);

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
                    AcceptConnection(i);
                }
                else
                {
                    ReceiveMessage(i);
                }
            }

            else if ((FD_ISSET(sockets[i].sock, &setOfSocketsInSendStatus)))
            {
                countOfWaitingList--;
                SendAMessage(i);
            }
        }
    }

    cout << "Time Server: Closing Connection.\n";
    closesocket(listenSocket);
    WSACleanup();
}

bool AddSocket(SOCKET id, int status)
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

void RemoveSocket(int index)
{
    sockets[index].isWaitingForClientMsg = EMPTY;
    sockets[index].isSendNeeded = EMPTY;
    socketsCount--;
}

void AcceptConnection(int index) 
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
    if (ioctlsocket(clientChannelSocket, FIONBIO, &flag) != 0)
    {
        cout << "Time Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
    }

    if (!AddSocket(clientChannelSocket, CLIENT_CHANNEL))
    {
        cout << "\t\tToo many connections, dropped!\n";
        closesocket(sock);
    }
}

void ReceiveMessage(int index)
{
    SOCKET sock = sockets[index].sock;
    int len = sockets[index].len;
    int bytesRecv = recv(sock, &sockets[index].buffer[len], sizeof(sockets[index].buffer) - len, 0);

    if (SOCKET_ERROR == bytesRecv) 
    {
        cout << "Time Server: Error at recv(): " << WSAGetLastError() << endl;
        closesocket(sock);
        RemoveSocket(index);
        return;
    }
    if (bytesRecv == 0)
    {
        closesocket(sock);
        RemoveSocket(index);
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
            HandleHttpRequest(index);
        }
    }
}

void SendAMessage(int index)
{
    SOCKET msgSocket = sockets[index].sock;
    int bytesSent = send(msgSocket, sockets[index].buffer, sockets[index].len, 0);
    if (SOCKET_ERROR == bytesSent)
    {
        cout << "Time Server: Error at send(): " << WSAGetLastError() << endl;
        return;
    }

    cout << "Time Server: Sent: " << bytesSent << " bytes" << endl;
    sockets[index].isSendNeeded = false;
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
        "Note: You can specify the language with the 'lang' query parameter. e.g., lang=en (options: en,he,fr,es).\n"
        "[POST] - Adds new string data to the server. The request body should contain the string to be added (can't be empty).\n"
        "[PUT] - Updates existing data. Requires an 'id' query parameter with the ID of the item to update, and a body with the new string.\n"
        "[DELETE] - Deletes an item. Requires an 'id' query parameter with the ID of the item to delete.\n"
        "[HEAD] - Same as GET request, but returns only the headers without the body.\n"
        "[TRACE] - Returns the same body as the request body (echo).\n"
        "[OPTIONS] - Lists all available options.\n";

    size_t length = strlen(menu) + 1; 
    char* result = new char[length];
    strcpy(result, menu);

    return result;
}

void HandleHttpRequest(int socketIndex)
{
    char method[8], path[256], version[16];
    sscanf(sockets[socketIndex].buffer, "%s %s %s", method, path, version);

    if (strcmp(method, "GET") == 0)
    {
        HandleGetRequest(socketIndex, path);
    }
    else if (strcmp(method, "POST") == 0)
    {
        HandlePostRequest(socketIndex, path);
    }
    else if (strcmp(method, "PUT") == 0)
    {
        HandlePutRequest(socketIndex, path);
    }
    else if (strcmp(method, "DELETE") == 0)
    {
        HandleDeleteRequest(socketIndex, path);
    }
    else if (strcmp(method, "HEAD") == 0)
    {
        HandleHeadRequest(socketIndex, path);
    }
    else if (strcmp(method, "OPTIONS") == 0)
    {
        ConstructHttpResponse(socketIndex, 200, "OK", GenerateOptionsMenu(), true, true);

    }
    else if (strcmp(method, "TRACE") == 0)
    {
        HandleTraceRequest(socketIndex, path);
    }
    else
    {
        ConstructHttpResponse(socketIndex, 501, "Not Implemented", "This server only supports GET and POST.", false, true);
    }
}

void HandleGetRequest(int socketIndex, const char* path)
{
    const char* lang = GetLangQueryParam(path);
    wstring htmlBody = GenerateHTMLBody(lang);

    char* toSend = ConvertWstrToCharArray(htmlBody);

    ConstructHttpResponse(socketIndex, 200, "OK", toSend, true, true);
}

void HandlePostRequest(int socketIndex, const char* path) {
    const char* body = strstr(sockets[socketIndex].buffer, "\r\n\r\n");
    if (body != NULL && body[4]!='\0')
    {
        int givenId;
        body += 4;  // Skip over the \r\n\r\n
        AddPhrase(body, &givenId);
        string msg = "The string \"" + string(body) + "\" was added successfully. Given id : " + to_string(givenId);
        cout << "Time Server: " << msg << endl;
        ConstructHttpResponse(socketIndex, 200, "OK", msg.c_str(), false, true);
    }
    else
    {
        ConstructHttpResponse(socketIndex, 400, "Bad Request", "No string in the request.", false, true);
    }
}

void HandleTraceRequest(int socketIndex, const char* path)
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
    ConstructHttpResponse(socketIndex, 200, "OK", body, false, true);
}

void HandleDeleteRequest(int socketIndex, const char* path)
{
    int idToDelete = GetIdQueryParam(path);

    if (idToDelete > 0 && RemovePhrase(idToDelete))
    {
        ConstructHttpResponse(socketIndex, 200, "OK", "Removed Succesfully", false, true);
    }
    else
    {
        ConstructHttpResponse(socketIndex, 404, "Not Found", "The requested resource was not found on this server.", false, true);
    }
}

void HandlePutRequest(int socketIndex, const char* path)
{
    int idToUpdate = GetIdQueryParam(path);

    if (idToUpdate > 0 && IsInPhrases(idToUpdate))
    {
        const char* body = strstr(sockets[socketIndex].buffer, "\r\n\r\n");
        if (body != NULL && body[4] != '\0')
        {
            body += 4;  // Skip over the \r\n\r\n
            UpdatePhrase(idToUpdate, ConstConverter(body));
            ConstructHttpResponse(socketIndex, 200, "OK", "Updated successfully", false, true);
        }
        else
        {
            ConstructHttpResponse(socketIndex, 400, "Bad Request", "No string in the request.", false, true);
        }
    }
    else
    {
        ConstructHttpResponse(socketIndex, 404, "Not Found", "The requested resource was not found on this server.", false, true);
    }

}

void HandleHeadRequest(int socketIndex, const char* path)
{
    const char* lang = GetLangQueryParam(path);
    wstring htmlBody = GenerateHTMLBody(lang);

    char* toSend = ConvertWstrToCharArray(htmlBody);
    ConstructHttpResponse(socketIndex, 200, "OK", toSend, true, false);
}

void ConstructHttpResponse(int index, int statusCode, const char* statusMessage, const char* responseBody, bool isDynamicallyAllocated, bool isSendingBody, const char* lang)
{
    char* mutuableResponseBody, * bodyToSend;
    char empty[] = "";

    mutuableResponseBody = const_cast<char*>(responseBody);

    if (isSendingBody)
    {
        bodyToSend = mutuableResponseBody;
    }
    else
    {
        bodyToSend = empty;
    }

    snprintf(sockets[index].buffer, sizeof(sockets[index].buffer),
        "HTTP/1.1 %d %s\r\n"
        "Content-Length: %zu\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Language: %s\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        statusCode, statusMessage, strlen(responseBody), lang, bodyToSend);

    if (isDynamicallyAllocated)
    {
        delete responseBody;
    }

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
        if (strncmp(lang, "en", 2) == 0 || strncmp(lang, "he", 2) == 0 || strncmp(lang, "fr", 2) == 0 || strncmp(lang, "es", 2) == 0)
        {
            strncpy(const_cast<char*>(res), lang, 2);
            res[2] = '\0';
            return res;
        }
    }
    return DEFAULT_LANG;
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
