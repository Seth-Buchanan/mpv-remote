#define SERIALTERMINAL      "/dev/ttyACM0"
#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <json-c/json.h>

struct json_object *parsed_json;

int set_interface_attribs(int fd, int speed) {
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    tty.c_lflag |= ICANON | ISIG;  /* canonical input */
    tty.c_lflag &= ~(ECHO | ECHOE | ECHONL | IEXTEN);

    tty.c_iflag &= ~IGNCR;  /* preserve carriage return */
    tty.c_iflag &= ~INPCK;
    tty.c_iflag &= ~(INLCR | ICRNL | IUCLC | IMAXBEL);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);   /* no SW flowcontrol */

    tty.c_oflag &= ~OPOST;

    tty.c_cc[VEOL] = 0;
    tty.c_cc[VEOL2] = 0;
    tty.c_cc[VEOF] = 0x04;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void doMpvCmd(char buf[]) {
  struct json_object *name;
  /* printf("%s\n", buf); */
  json_object_object_get_ex(parsed_json, buf, &name);
  if(name != 0){
    printf("Called Command: %s\n", json_object_get_string(name));
  }

}

int start_scanning() {
    char *portname = SERIALTERMINAL;
    int fd;

    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }
    /*baudrate 9600, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(fd, B9600);

    /* simple canonical input */
    while (1) {
        char buf[83];
        char *p;
        int rdlen;

        rdlen = read(fd, buf, sizeof(buf) - 1);
        if (rdlen > 0) {
            buf[rdlen] = 0;
            for (p = buf; rdlen-- > 0; p++) {
                if (*p < ' ')
                    *p = '\0';   /* replace any control chars */
            }
	    doMpvCmd(buf);
            /* printf("%s\n", buf); */
        } else if (rdlen < 0) {
            printf("Error from read: %d: %s\n", rdlen, strerror(errno));
        } else {  /* rdlen == 0 */
            printf("Nothing read. EOF?\n");
        }               
    }
}

int parse_json() {
  FILE *fp;
  char buffer[1024];

  fp = fopen("test.json", "r");
  fread(buffer, 1024, 1, fp);
  fclose(fp);

  parsed_json = json_tokener_parse(buffer);
  return 0;
}

int main() {
  parse_json();
  start_scanning();
}
