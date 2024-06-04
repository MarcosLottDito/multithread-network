#include "common.h"

int menu()
{
    char input[CHARSZ];

    if (fgets(input, sizeof(input), stdin) != NULL)
    {
        if (input[0] == EXIT)
            return 0;

        if (input[0] == LORD_OF_THE_RINGS)
            return 1;

        if (input[0] == THE_GODFATHER)
            return 2;

        if (input[0] == FIGHT_CLUB)
            return 3;

        return menu();
    }
    else
    {
        log_exit("Error reading input.\n");
        exit(1);
    }
}

void server_usage()
{
    printf("Usage: <ipv4|ipv6> <port>\n");
    printf("Example: ipv4 5151\n");
    exit(EXIT_FAILURE);
}

void client_usage()
{
    printf("Usage: <host> <port>\n");
    printf("Example: 127.0.0.1 5151\n");
    exit(EXIT_FAILURE);
}

void log_exit(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

int server_init(const char *version, const char *port, struct sockaddr_storage *storage)
{
    uint16_t serverPort = (uint16_t)atoi(port);

    if (serverPort == 0)
        return EXIT_FAILURE;

    serverPort = htons(serverPort);
    memset(storage, 0, sizeof(*storage));

    if (0 == strcmp(version, "ipv4"))
    {
        struct sockaddr_in *addressV4 = (struct sockaddr_in *)storage;
        addressV4->sin_family = AF_INET;
        addressV4->sin_port = serverPort;
        addressV4->sin_addr.s_addr = INADDR_ANY;
        return EXIT_SUCCESS;
    }

    if (0 == strcmp(version, "ipv6"))
    {
        struct sockaddr_in6 *addressV6 = (struct sockaddr_in6 *)storage;
        addressV6->sin6_family = AF_INET6;
        addressV6->sin6_port = serverPort;
        addressV6->sin6_addr = in6addr_any;
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

int address_parse(const char *address, const char *port, struct sockaddr_storage *storage)
{
    if (address == NULL || port == NULL)
        return EXIT_FAILURE;

    uint16_t portNumber = (uint16_t)atoi(port);

    if (portNumber == 0)
        return EXIT_FAILURE;

    portNumber = htons(portNumber);

    struct in_addr inAdressV4;
    if (inet_pton(AF_INET, address, &inAdressV4))
    {
        struct sockaddr_in *adressV4 = (struct sockaddr_in *)storage;
        adressV4->sin_family = AF_INET;
        adressV4->sin_port = portNumber;
        adressV4->sin_addr = inAdressV4;
        return EXIT_SUCCESS;
    }

    struct in6_addr inAdressV6;
    if (inet_pton(AF_INET6, address, &inAdressV6))
    {
        struct sockaddr_in6 *adressV6 = (struct sockaddr_in6 *)storage;
        adressV6->sin6_family = AF_INET6;
        adressV6->sin6_port = portNumber;
        memcpy(&(adressV6->sin6_addr), &inAdressV6, sizeof(inAdressV6));
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

void getLordOfTheRings(int message, char *buffer)
{
    switch (message)
    {
    case 1:
        strcpy(buffer, "Um anel para a todos governar\n");
        break;
    case 2:
        strcpy(buffer, "Na terra de Mordor onde as sombras se deitam\n");
        break;
    case 3:
        strcpy(buffer, "Não é o que temos, mas o que fazemos com o que temos\n");
        break;
    case 4:
        strcpy(buffer, "Não há mal que sempre dure\n");
        break;
    case 5:
        strcpy(buffer, "O mundo está mudando, senhor Frodo\n");
        break;
    default:
        log_exit("Invalid message\n");
        break;
    }
}

void getTheGodfather(int message, char *buffer)
{
    switch (message)
    {
    case 1:
        strcpy(buffer, "Vou fazer uma oferta que ele não pode recusar\n");
        break;
    case 2:
        strcpy(buffer, "Mantenha seus amigos por perto e seus inimigos mais perto ainda\n");
        break;
    case 3:
        strcpy(buffer, "Não é o que temos, mas o que fazemos com o que temos\n");
        break;
    case 4:
        strcpy(buffer, "A vingança é um prato que se come frio\n");
        break;
    case 5:
        strcpy(buffer, "Nunca deixe que ninguém saiba o que você está pensando\n");
        break;
    default:
        log_exit("Invalid message\n");
        break;
    }
}

void getFightClub(int message, char *buffer)
{
    switch (message)
    {
    case 1:
        strcpy(buffer, "Primeira regra do Clube da Luta: você não fala sobre o Clube da Luta\n");
        break;
    case 2:
        strcpy(buffer, "Segunda regra do Clube da Luta: você não fala sobre o Clube da Luta\n");
        break;
    case 3:
        strcpy(buffer, "O que você possui acabará possuindo você\n");
        break;
    case 4:
        strcpy(buffer, "É apenas depois de perder tudo que somos livres para fazer qualquer coisa\n");
        break;
    case 5:
        strcpy(buffer, "Escolha suas lutas com sabedoria\n");
        break;
    default:
        log_exit("Invalid message\n");
        break;
    }
}