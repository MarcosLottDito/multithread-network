#include "common.h"

// global counter for number of clients connected and mutex to protect the client counter
int clientCount = 0;
pthread_mutex_t clientCountMutex = PTHREAD_MUTEX_INITIALIZER;

struct client_data
{
    int socket;
    struct sockaddr_storage storage;
    char movieId;
};

// thread function that deal with client connection
void *clientThread(void *data)
{
    // cast the data to the client data struct
    struct client_data *clientData = (struct client_data *)data;
    socklen_t clientAddressLength = sizeof(clientData->storage);

    char buffer[BUFSZ];
    memset(buffer, 0, BUFSZ);

    for (int i = 1; i <= MESSAGES; i++)
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

        // send the current movie message to the client
        sendto(clientData->socket, buffer, BUFSZ, 0, (struct sockaddr *)&clientData->storage, clientAddressLength);

        sleep(3);
    }

    // send an empty message to the client to signal the end of the communication
    sendto(clientData->socket, 0, 0, 0, (struct sockaddr *)&clientData->storage, clientAddressLength);

    // locks the mutex in order to update the client count and then unlocks it
    pthread_mutex_lock(&clientCountMutex);
    clientCount -= 1;
    pthread_mutex_unlock(&clientCountMutex);

    // free the client data and close the socket
    memset(buffer, 0, BUFSZ);
    close(clientData->socket);
    free(clientData);
    pthread_exit(EXIT_SUCCESS);
}

// thread function that deals with server monitoring
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

    // storage for socket configuration
    struct sockaddr_storage storage;
    // set port, address and IP version correctly for the server open connection
    if (0 != server_init(argv[1], argv[2], &storage))
        server_usage();

    // create a socket based on the storage configuration (IP family, UDP connection, 0 for protocol)
    int serverSocket = socket(storage.ss_family, SOCK_DGRAM, 0);
    if (serverSocket == -1)
        log_exit("Error at server socket creation");

    // set the socket to reuse the address
    int enable = 1;
    if (0 != setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
        log_exit("Error at setsockopt");

    // bind the socket to the address and port
    struct sockaddr *address = (struct sockaddr *)(&storage);
    if (0 != bind(serverSocket, address, sizeof(storage)))
        log_exit("Error at bind");

    // creates server thread to monitor the number of clients connected
    pthread_t serverThreadID;
    pthread_create(&serverThreadID, NULL, serverThread, NULL);

    while (1)
    {
        // storage for client address and its data settings
        struct sockaddr_storage clientStorage;
        struct sockaddr *clientAddress = (struct sockaddr *)(&clientStorage);
        socklen_t clientAddressLength = sizeof(clientStorage);

        // create a socket for the client connection
        int clientSocket = socket(storage.ss_family, SOCK_DGRAM, 0);
        if (clientSocket == -1)
            log_exit("Error at client socket creation");

        char clientChoice[CHARSZ];
        memset(clientChoice, 0, CHARSZ);

        // receive the client movie choice and check for errors
        int bytesRecieved = recvfrom(serverSocket, clientChoice, CHARSZ, 0, clientAddress, &clientAddressLength);
        if (bytesRecieved == -1)
            log_exit("Error at recvfrom");

        // locks the mutex in order to update the client count and then unlocks it
        pthread_mutex_lock(&clientCountMutex);
        clientCount += 1;
        pthread_mutex_unlock(&clientCountMutex);

        // create a client data struct to pass to the client thread
        struct client_data *clientData = malloc(sizeof(*clientData));
        if (!clientData)
            log_exit("Error at malloc");

        clientData->socket = clientSocket;
        clientData->movieId = *clientChoice;
        memcpy(&(clientData->storage), &clientStorage, sizeof(clientStorage));

        // create a client thread to deal with the client connection
        pthread_t clientThreadID;
        pthread_create(&clientThreadID, NULL, clientThread, clientData);
    }

    exit(EXIT_SUCCESS);
}