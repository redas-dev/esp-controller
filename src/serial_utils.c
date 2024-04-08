#include "serial_utils.h"

int send_data_serial(const char* port, char* data, char* resp, int respSize)
{
    struct termios tty;
    int tty_fd;

    // Open port
    tty_fd = open(port, O_RDWR | O_NOCTTY);

    // Get port attributes
    tcgetattr(tty_fd, &tty);

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag |= CLOCAL | CREAD ; //ignore modem control lines
    tty.c_iflag |= IGNPAR | ICRNL;

    tty.c_oflag = 0;

    tty.c_lflag |= ICANON;

    // Put the terminal into exclusive mode
    if (ioctl(tty_fd, TIOCEXCL, NULL) < 0){
        close(tty_fd);
        return -1;
    }

    // Set all attributes
    if ((tcsetattr(tty_fd, TCSANOW, &tty)) != 0){
        close(tty_fd);
        return -1;
    }

    // Clear pending data
    tcflush(tty_fd, TCIOFLUSH);

    // Write data to serial
    int ret = write(tty_fd, data, strlen(data));

    if (ret != strlen(data)){
        close(tty_fd);
        return -1;
    }

    // Read response from serial
    if (read(tty_fd, resp, respSize) < 0){
        resp = NULL;
        close(tty_fd);
        return -1;
    }

    close(tty_fd);

    return 0;
}