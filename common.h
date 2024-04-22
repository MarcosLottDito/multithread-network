#pragma once

#include <math.h>
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

typedef struct
{
   double latitude;
   double longitude;
} Coordinate;

int menu();
void server_usage();
void client_usage();
void log_exit(const char *message);
void address_to_string(const struct sockaddr *address, char *string, size_t stringSize);
int server_init(const char *version, const char *port, struct sockaddr_storage *storage);
int address_parse(const char *address, const char *port, struct sockaddr_storage *storage);

double degrees_to_radians(double degrees);
double haversine(double lat1, double lon1, double lat2, double lon2);
int distance_between_coordinates(Coordinate coord1, Coordinate coord2);