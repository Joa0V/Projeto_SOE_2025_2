#include "gps.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <stdbool.h>

static int gps_fd = -1;

// Converte NMEA ddmm.mmmm para graus decimais
static double convert_nmea_to_deg(const char *value, char dir) {
    if (!value || strlen(value) < 3) return 0.0;
    double raw = atof(value);
    int deg = (int)(raw / 100);
    double min = raw - (deg * 100);
    double dec = deg + min / 60.0;
    if (dir == 'S' || dir == 'W') dec = -dec;
    return dec;
}

// Inicializa GPS
int gps_init(const char *device) {
    gps_fd = open(device, O_RDONLY | O_NOCTTY);
    if (gps_fd < 0) return -1;

    struct termios options;
    tcgetattr(gps_fd, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8 | CREAD | CLOCAL;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    tcsetattr(gps_fd, TCSANOW, &options);
    return gps_fd;
}

// Interpreta a sentença GPGGA
static void parse_gpgga(const char *sentence, double *lat, double *lon) {
    char buffer[128];
    strncpy(buffer, sentence, sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';

    char *token = strtok(buffer, ",");
    int field = 0;
    char lat_str[16] = {0}, lon_str[16] = {0};
    char lat_dir = 'N', lon_dir = 'E';

    while(token) {
        field++;
        switch(field) {
            case 3: strncpy(lat_str, token, sizeof(lat_str)-1); break;
            case 4: lat_dir = token[0]; break;
            case 5: strncpy(lon_str, token, sizeof(lon_str)-1); break;
            case 6: lon_dir = token[0]; break;
        }
        token = strtok(NULL, ",");
    }

    *lat = convert_nmea_to_deg(lat_str, lat_dir);
    *lon = convert_nmea_to_deg(lon_str, lon_dir);
}

// Lê GPS (retorna coordenadas válidas quando uma linha completa é recebida)
GpsCoords gps_read() {
    GpsCoords r = {0.0, 0.0, false};
    static char line[256];
    static int idx = 0;

    if(gps_fd < 0) return r;

    char c;
    ssize_t n = read(gps_fd, &c, 1);
    while(n > 0) {
        if(c == '\n' || c == '\r') {
            line[idx] = '\0';
            idx = 0;
            if(strncmp(line, "$GPGGA", 6) == 0) {
                parse_gpgga(line, &r.latitude, &r.longitude);
                r.valid = true;
                return r;
            }
        } else if(idx < sizeof(line)-1) {
            line[idx++] = c;
        }
        n = read(gps_fd, &c, 1);
    }

    return r; // valid=false se não tiver linha completa
}

// Fecha GPS
void gps_close() {
    if(gps_fd >= 0) {
        close(gps_fd);
        gps_fd = -1;
    }
}

