#include "common.h"
#define BUFSZ 1024

int main(int argc, char **argv)
{
    if (argc != 3)
        client_usage(argc, argv);

    int userResponse = -1;

    while (userResponse == -1)
    {
        printf("0 - Sair\n");
        printf("1 - Solicitar corrida\n");

        userResponse = menu();
        if (userResponse == 0)
            break;

        if (userResponse == 1)
        {
            struct sockaddr_storage storage;
            if (0 != address_parse(argv[1], argv[2], &storage))
                client_usage(argc, argv);

            int clientSocket = socket(storage.ss_family, SOCK_STREAM, 0);
            if (clientSocket == -1)
                log_exit("Error at socket creation");

            struct sockaddr *address = (struct sockaddr *)(&storage);
            if (0 != connect(clientSocket, address, sizeof(storage)))
                log_exit("Error at connect");

            int serverResponse = -1;
            while (serverResponse == -1)
            {
                size_t bytesReceived = recv(clientSocket, &serverResponse, sizeof(serverResponse), 0);

                if (bytesReceived <= 0 || serverResponse == 0)
                {
                    printf("Não foi encontrado um motorista.\n");
                    close(clientSocket);
                    userResponse = -1;
                    break;
                }
            }

            if (serverResponse == 0)
                continue;

            char buf[BUFSZ];
            memset(buf, 0, BUFSZ);

            Coordinate clientCoordinate = {-19.9259, -43.9342};
            sprintf(buf, "%lf %lf", clientCoordinate.latitude, clientCoordinate.longitude);

            size_t bytesWritten = send(clientSocket, buf, strlen(buf) + 1, 0);
            if (bytesWritten != strlen(buf) + 1)
                log_exit("Error at send");

            memset(buf, 0, BUFSZ);
            while (1)
            {
                bytesWritten = recv(clientSocket, buf, BUFSZ - 1, 0);

                if (bytesWritten == 0) // Connection closed
                {
                    printf("O motorista chegou!\n");
                    printf("<Encerrar programa>\n");
                    return EXIT_SUCCESS;
                }
                printf("Server response: %s\n", buf);
            }

            close(clientSocket);
            userResponse = -1;
        }
    }

    printf("<Encerrar programa>\n");
    return EXIT_SUCCESS;
}