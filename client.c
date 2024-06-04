#include "common.h"

int main(int argc, char **argv)
{
    if (argc != 3)
        client_usage(argc, argv);

    int userResponse = -1;

    while (userResponse == -1)
    {
        printf("0 - Sair\n");
        printf("1 - Senhor dos Anéis\n");
        printf("2 - O Poderoso Chefão\n");
        printf("3 - Clube da Luta\n");

        userResponse = menu();
        if (userResponse == 0)
            break;

        else
        {
            struct sockaddr_storage storage;
            if (0 != address_parse(argv[1], argv[2], &storage))
                client_usage(argc, argv);

            int clientSocket = socket(storage.ss_family, SOCK_DGRAM, 0);
            if (clientSocket == -1)
                log_exit("Error at socket creation");

            char buffer[BUFSZ];
            memset(buffer, 0, BUFSZ);

            struct sockaddr *address = (struct sockaddr *)(&storage);
            socklen_t addressLength = sizeof(storage);

            if (-1 == sendto(clientSocket, userResponse, strlen(userResponse) + 1, 0,
                             (struct sockaddr *)&storage, addressLength))
                log_exit("Error at sendto");

            while (1)
            {
                int bytesReceived = recvfrom(clientSocket, buffer, BUFSZ, 0, address, &addressLength);

                if (bytesReceived == -1)
                    log_exit("Error at recvfrom");

                if (bytesReceived == 0) // Connection closed
                {
                    userResponse = -1;
                    break;
                }
                printf("%s\n", buffer);
                memset(buffer, 0, BUFSZ);
            }

            close(clientSocket);
            userResponse = -1;
        }
    }

    printf("<Encerrar programa>\n");
    return EXIT_SUCCESS;
}