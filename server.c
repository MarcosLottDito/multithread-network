#include "common.h"

int clientCount = 0;
pthread_mutex_t clientCountMutex = PTHREAD_MUTEX_INITIALIZER;

struct client_data
{
    int socket;
    struct sockaddr_storage storage;
    char movieId;
};

void *clientThread(void *data)
{
    struct client_data *clientData = (struct client_data *)data;
    socklen_t clientAddressLength = sizeof(clientData->storage);

    char buffer[BUFSZ];
    memset(buffer, 0, BUFSZ);

    for (int i = 0; i < 5; i++)
    {
        switch (clientData->movieId)
        {
        case LORD_OF_THE_RINGS:
            getLordOfTheRings(i, buffer);
            break;

        case THE_GODFATHER:
            getTheGodfather(i, buffer);
            break;

        case FIGHT_CLUB:
            getFightClub(i, buffer);
            break;

        default:
            log_exit("Invalid movie ID");
            break;
        }
        sendto(clientData->socket, buffer, BUFSZ, 0, (struct sockaddr *)&clientData->storage, clientAddressLength);

        sleep(3);
    }

    pthread_mutex_lock(&clientCountMutex);
    clientCount -= 1;
    pthread_mutex_unlock(&clientCountMutex);

    memset(buffer, 0, BUFSZ);
    close(clientData->socket);
    free(clientData);
    pthread_exit(EXIT_SUCCESS);
}

void *serverThread(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&clientCountMutex);
        printf("Clientes: %d\n", clientCount);
        pthread_mutex_unlock(&clientCountMutex);
        sleep(4);
    }
    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    if (argc != 3)
        server_usage();

    struct sockaddr_storage storage;
    if (0 != server_init(argv[1], argv[2], &storage))
        server_usage();

    int serverSocket = socket(storage.ss_family, SOCK_DGRAM, 0);
    if (serverSocket == -1)
        log_exit("Error at server socket creation");

    int enable = 1;
    if (0 != setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
        log_exit("Error at setsockopt");

    struct sockaddr *address = (struct sockaddr *)(&storage);
    if (0 != bind(serverSocket, address, sizeof(storage)))
        log_exit("Error at bind");

    pthread_t serverThreadID;
    pthread_create(&serverThreadID, NULL, serverThread, NULL);

    while (1)
    {
        struct sockaddr_storage clientStorage;
        struct sockaddr *clientAddress = (struct sockaddr *)(&clientStorage);
        socklen_t clientAddressLength = sizeof(clientStorage);

        int clientSocket = socket(storage.ss_family, SOCK_DGRAM, 0);
        if (clientSocket == -1)
            log_exit("Error at client socket creation");

        char clientChoice[CHARSZ];
        memset(clientChoice, 0, CHARSZ);
        int bytesRecieved = recvfrom(serverSocket, clientChoice, CHARSZ, 0, clientAddress, &clientAddressLength);
        if (bytesRecieved == -1)
            log_exit("Error at recvfrom");

        pthread_mutex_lock(&clientCountMutex);
        clientCount += 1;
        pthread_mutex_unlock(&clientCountMutex);

        struct client_data *clientData = malloc(sizeof(*clientData));
        if (!clientData)
            log_exit("Error at malloc");

        clientData->socket = clientSocket;
        clientData->movieId = clientChoice;
        memcpy(&(clientData->storage), &clientStorage, sizeof(clientStorage));

        pthread_t clientThreadID;
        pthread_create(&clientThreadID, NULL, clientThread, clientData);
    }

    exit(EXIT_SUCCESS);
}