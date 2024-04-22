#include "common.h"
#define BUFSZ 1024

struct client_data
{
    int clientSocket;
    struct sockaddr_storage clientStorage;
};

void *clientThread(void *data)
{
    struct client_data *clientData = (struct client_data *)data;
    struct sockaddr *clientAddress = (struct sockaddr *)(&clientData->clientStorage);

    char clientAddressString[BUFSZ];
    address_to_string(clientAddress, clientAddressString, BUFSZ);

    char buf[BUFSZ];
    memset(buf, 0, BUFSZ);

    int clientResponse = -1;

    while (clientResponse == -1)
    {
        printf("Corrida disponível:\n");
        printf("0 - Recusar\n");
        printf("1 - Aceitar\n");

        clientResponse = menu();
        send(clientData->clientSocket, &clientResponse, sizeof(clientResponse), 0);
        if (clientResponse == 0)
        {
            close(clientData->clientSocket);
            pthread_exit(EXIT_SUCCESS);
        }
    }

    while (1)
    {
        size_t bytesRead = recv(clientData->clientSocket, buf, BUFSZ - 1, 0);

        // If the client closed the connection, break the loop
        if (bytesRead <= 0)
            break;

        printf("[mensagem]: %s, %d bytes: %s", clientAddressString, (int)bytesRead, buf);

        sprintf(buf, "Remote endpoint: %.1000s\n", clientAddressString);
        size_t bytesWritten = send(clientData->clientSocket, buf, strlen(buf) + 1, 0);

        if (bytesWritten != strlen(buf) + 1)
            log_exit("Error at send");

        // Clear the buffer after sending a response
        memset(buf, 0, BUFSZ);
    }

    // Move the close function outside the while loop
    close(clientData->clientSocket);

    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    if (argc != 3)
        server_usage();

    struct sockaddr_storage storage;
    if (0 != server_init(argv[1], argv[2], &storage))
        server_usage();

    int serverSocket = socket(storage.ss_family, SOCK_STREAM, 0);
    if (serverSocket == -1)
        log_exit("Error at socket creation");

    int enable = 1;
    if (0 != setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
        log_exit("Error at setsockopt");

    struct sockaddr *address = (struct sockaddr *)(&storage);
    if (0 != bind(serverSocket, address, sizeof(storage)))
        log_exit("Error at bind");

    if (0 != listen(serverSocket, 10))
        log_exit("Error at listen");

    char addressString[BUFSZ];
    address_to_string(address, addressString, BUFSZ);
    printf("Aguardando solicitação.\n");

    while (1)
    {
        struct sockaddr_storage clientStorage;
        struct sockaddr *clientAddress = (struct sockaddr *)(&clientStorage);
        socklen_t clientAddressLength = sizeof(clientStorage);

        int clientSocket = accept(serverSocket, clientAddress, &clientAddressLength);
        if (clientSocket == -1)
            log_exit("Error at accept");

        struct client_data *clientData = malloc(sizeof(*clientData));
        if (!clientData)
            log_exit("Error at malloc");

        clientData->clientSocket = clientSocket;
        memcpy(&(clientData->clientStorage), &clientStorage, sizeof(clientStorage));

        pthread_t tid;
        pthread_create(&tid, NULL, clientThread, clientData);
    }

    exit(EXIT_SUCCESS);
}