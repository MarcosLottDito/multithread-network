#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>

#define BUFSZ 1024
#define CHARSZ 3
#define MESSAGES 5
#define TIMEOUT_SECONDS 5

#define UNSET '*'
#define EXIT '0'
#define LORD_OF_THE_RINGS '1'
#define THE_GODFATHER '2'
#define FIGHT_CLUB '3'

char menu();
void server_usage();
void client_usage();
void log_exit(const char *message);
int server_init(const char *version, const char *port, struct sockaddr_storage *storage);
int client_init(const char *address, const char *port, struct sockaddr_storage *storage);

void getLordOfTheRings(int message, char *buffer);
void getTheGodfather(int message, char *buffer);
void getFightClub(int message, char *buffer);