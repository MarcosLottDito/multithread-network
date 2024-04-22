#include "common.h"

void log_exit(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
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

void address_to_string(const struct sockaddr *address, char *string, size_t stringSize)
{
    int version = 0;
    char addressString[INET6_ADDRSTRLEN + 1] = "";
    uint16_t port;

    if (address->sa_family == AF_INET)
        version = 4;
    if (address->sa_family == AF_INET6)
        version = 6;
    if (version == 0)
        log_exit("Unknown address family");

    if (version == 4)
    {
        struct sockaddr_in *addresV4 = (struct sockaddr_in *)address;
        if (!inet_ntop(AF_INET, &(addresV4->sin_addr), addressString, INET6_ADDRSTRLEN + 1))
        {
            log_exit("Error at inet_ntop");
        }
        port = ntohs(addresV4->sin_port);
    }
    if (version == 6)
    {
        struct sockaddr_in6 *addresV6 = (struct sockaddr_in6 *)address;
        if (!inet_ntop(AF_INET6, &(addresV6->sin6_addr), addressString, INET6_ADDRSTRLEN + 1))
        {
            log_exit("Error at inet_ntop");
        }
        port = ntohs(addresV6->sin6_port);
    }

    if (string)
        snprintf(string, stringSize, "IPv%d %s %hu", version, addressString, port);
}

int server_init(const char *version, const char *port, struct sockaddr_storage *storage)
{
    uint16_t serverPort = (uint16_t)atoi(port);

    if (serverPort == 0)
        return EXIT_FAILURE;

    serverPort = htons(serverPort);

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

int menu()
{
    char input[3];

    if (fgets(input, sizeof(input), stdin) != NULL)
    {
        if (input[0] == '0')
            return 0;

        if (input[0] == '1')
            return 1;

        return menu();
    }
    else
    {
        log_exit("Error reading input.\n");
        exit(1);
    }
}