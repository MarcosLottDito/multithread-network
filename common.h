#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include <sys/types.h>
#include <sys/socket.h>

int menu();
void server_usage();
void client_usage();
void log_exit(const char *message);
void address_to_string(const struct sockaddr *address, char *string, size_t stringSize);
int server_init(const char *version, const char *port, struct sockaddr_storage *storage);
int address_parse(const char *address, const char *port, struct sockaddr_storage *storage);