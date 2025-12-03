#ifndef GPS_H
#define GPS_H

#include <stdbool.h>

typedef struct {
double latitude;
double longitude;
bool valid;
} GpsCoords;


int gps_init(const char *device);

GpsCoords gps_read();

void gps_close();

#endif
