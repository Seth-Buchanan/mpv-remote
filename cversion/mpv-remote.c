/* gcc -o mpv-remote.so mpv-remote.c `pkg-config --cflags --libs json-c mpv` -shared -fPIC  */

#define SERIALTERMINAL      "/dev/ttyACM0"
#include <errno.h>
#include <fcntl.h> 
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <json-c/json.h>
#include <mpv/client.h>

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
    tty.c_cflag |= CS8;		   /* 8-bit characters */
    tty.c_cflag &= ~PARENB;        /* no parity bit */
    tty.c_cflag &= ~CSTOPB;	   /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;	   /* no hardware flowcontrol */

    tty.c_lflag |= ICANON | ISIG;  /* canonical input */
    tty.c_lflag &= ~(ECHO | ECHOE | ECHONL | IEXTEN);

    tty.c_iflag &= ~IGNCR;	   /* preserve carriage return */
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


int handle_mpv_events(mpv_handle *handle) {
    while (1) {
      mpv_event *event = mpv_wait_event(handle, 0);
      printf("event: %s\n", mpv_event_name(event->event_id));
      
      if (event->event_id == MPV_EVENT_NONE){
	break;
      } else if (event->event_id == MPV_EVENT_SHUTDOWN){
	return 0;
      }
	
    }
    return 1;
}


void do_mpv_command(char buf[], mpv_handle *handle) {
  struct json_object *name;
  json_object_object_get_ex(parsed_json, buf, &name);
  if(name != 0){
    printf("%s\n", json_object_get_string(name));
    mpv_command_string(handle ,json_object_get_string(name));
  }
}


void parse_json() {
  FILE *fp;
  char buffer[1024];

  fp = fopen("/home/seth/.config/mpv/script-opts/test.json", "r");
  fread(buffer, 1024, 1, fp);
  fclose(fp);

  parsed_json = json_tokener_parse(buffer);
}


int mpv_open_cplugin(mpv_handle *handle) {
  parse_json();
  char *portname = SERIALTERMINAL;
  int fd;
  int keep_going = 1;
  /*baudrate 9600, 8 bits, no parity, 1 stop bit */
  fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);

  if (fd < 0) {
    printf("\e[0;36mmpv_remote: Error opening %s, %s\n", portname, strerror(errno));
    return -1;
  }
  set_interface_attribs(fd, B9600);
  
  /* simple canonical input */
  while (keep_going) {
    char buf[83];
    char *p;
    int rdlen;
    /* handle_mpv_events(handle); */
    mpv_event *event = mpv_wait_event(handle, 0);
    printf("event: %s\n", mpv_event_name(event->event_id));
    
    rdlen = read(fd, buf, sizeof(buf) - 1);
    if (rdlen > 0) {
      buf[rdlen] = 0;
      for (p = buf; rdlen-- > 0; p++) {
	if (*p < ' ') *p = '\0';   /* replace any control chars with NULL bytes */
      }
      do_mpv_command(buf, handle);
    } else if (rdlen < 0) {
      printf("Error from read: %d: %s\n", rdlen, strerror(errno));
    } else {  /* rdlen == 0 */
      printf("Nothing read. EOF?\n");
    }

    if (event->event_id != MPV_EVENT_NONE){
      keep_going = handle_mpv_events(handle);
    }
  }
  printf("done");
  return 0;
}
