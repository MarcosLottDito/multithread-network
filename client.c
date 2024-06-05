#include "common.h"

int main(int argc, char **argv)
{
    char userResponse = UNSET;

    while (userResponse == UNSET)
    {
        if (argc != 3)
            client_usage(argc, argv);

        // storage for socket configuration
        struct sockaddr_storage storage;

        // set port, address and IP version correctly for the server connection
        if (0 != client_init(argv[1], argv[2], &storage))
            client_usage(argc, argv);

        printf("0 - Sair\n");
        printf("1 - Senhor dos Anéis\n");
        printf("2 - O Poderoso Chefão\n");
        printf("3 - Clube da Luta\n");

        userResponse = menu();
        if (userResponse == EXIT)
            break;

        // create a socket based on the stoaage configuration (IP family, UDP connection, 0 for protocol)
        int clientSocket = socket(storage.ss_family, SOCK_DGRAM, 0);
        if (clientSocket == -1)
            log_exit("Error at socket creation");

        char buffer[BUFSZ];
        memset(buffer, 0, BUFSZ);

        // The sockaddr structure is used in many network functions to hold IP address and port information.
        struct sockaddr *address = (struct sockaddr *)(&storage);
        socklen_t addressLength = sizeof(storage);

        // send the user response to the server and check for errors
        if (-1 == sendto(clientSocket, &userResponse, CHARSZ, 0,
                         (struct sockaddr *)&storage, addressLength))
            log_exit("Error at sendto");

        struct timeval timeout;
        timeout.tv_sec = TIMEOUT_SECONDS; // Set timeout in seconds

        // Set the timeout for recvfrom
        if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
        {
            perror("Error setting socket options");
            exit(EXIT_FAILURE);
        }

        while (1)
        {
            // receive the server response and check for errors
            int bytesReceived = recvfrom(clientSocket, buffer, BUFSZ, 0, address, &addressLength);

            if (bytesReceived == -1)
                log_exit("Error at recvfrom or timeout exceeded");

            if (bytesReceived == 0) // sender has closed the connection
                break;

            printf("%s\n", buffer);
            memset(buffer, 0, BUFSZ);
        }

        close(clientSocket); // close the socket connection
        userResponse = UNSET;
    }

    printf("<Encerrar programa>\n");
    return EXIT_SUCCESS;
}