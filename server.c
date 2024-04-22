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

    char buf[BUFSZ];
    memset(buf, 0, BUFSZ);

    size_t bytesRead = recv(clientData->clientSocket, buf, BUFSZ - 1, 0);

    // If the client closed the connection, close thread
    if (bytesRead <= 0)
    {
        memset(buf, 0, BUFSZ);
        close(clientData->clientSocket);
        pthread_exit(EXIT_SUCCESS);
    }

    Coordinate serverCoordinate = {-19.9227, -43.9451};

    Coordinate clientCoordinate;
    sscanf(buf, "%lf %lf", &clientCoordinate.latitude, &clientCoordinate.longitude);

    int distance = distance_between_coordinates(clientCoordinate, serverCoordinate);

    while (distance > 0)
    {
        sprintf(buf, "Motorista a %dm\n", distance);
        size_t bytesWritten = send(clientData->clientSocket, buf, strlen(buf) + 1, 0);

        if (bytesWritten != strlen(buf) + 1)
            log_exit("Error at send");

        distance -= 400;

        memset(buf, 0, BUFSZ);
        sleep(2); // Wait for 2 seconds
    }

    printf("O motorista chegou!\n");

    memset(buf, 0, BUFSZ);
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