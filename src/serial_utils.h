#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <syslog.h>
#include <sys/ioctl.h>

#ifndef SERIAL_UTILS_H
#define SERIAL_UTILS_H
    int send_data_serial(const char* port, char* data, char* resp, int respSize);
#endif