/* gcc -o mpv-remote.so mpv-remote.c `pkg-config --cflags --libs json-c mpv` -shared -fPIC  */

#define MODEMDEVICE "/dev/ttyACM0"
#include <errno.h>
#include <fcntl.h> 
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/signal.h>
#include <sys/types.h>

#include <json-c/json.h>
#include <mpv/client.h>

struct json_object *parsed_json;
        
#define BAUDRATE B9600

#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
        
volatile int keep_going = TRUE; 
        
void signal_handler_IO (int status);   /* definition of signal handler */
int wait_flag=TRUE;                    /* TRUE while no signal received */


int handle_mpv_events(mpv_handle *handle) {
    while (1) {
      mpv_event *event = mpv_wait_event(handle, 0);
      
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
    /* printf("%s\n", json_object_get_string(name)); */
    mpv_command_string(handle ,json_object_get_string(name));
  }
}


void parse_json() {
  FILE *fp;
  char buffer[1024];

  char file[256]; // or whatever, I think there is a #define for this, something like PATH_MAX
  strcat(strcpy(file, getenv("HOME")), "/.config/mpv/script-opts/mpv-remote-config.json");
  
  fp = fopen(file, "r");
  fread(buffer, 1024, 1, fp);
  fclose(fp);

  parsed_json = json_tokener_parse(buffer);
}


int mpv_open_cplugin(mpv_handle *handle) {
  parse_json();
  int fd, res;
  struct termios oldtio,newtio;
  struct sigaction saio;           /* definition of signal action */
  char buf[255];
        
  /* open the device to be non-blocking (read will return immediatly) */
  fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd <0) {
    printf("\e[0;36mmpv_remote: Error opening %s, %s\n", MODEMDEVICE, strerror(errno));
    return -1;
  }
        
  /* install the signal handler before making the device asynchronous */
  saio.sa_handler = signal_handler_IO;
  saio.sa_flags = 0;
  saio.sa_restorer = NULL;
  sigaction(SIGIO,&saio,NULL);
          
  /* allow the process to receive SIGIO */
  fcntl(fd, F_SETOWN, getpid());
  /* Make the file descriptor asynchronous (the manual page says only 
     O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
  fcntl(fd, F_SETFL, FASYNC);
        
  tcgetattr(fd,&oldtio); /* save current port settings */
  /* set new port settings for canonical input processing */
  newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR | ICRNL;
  newtio.c_oflag = 0;
  newtio.c_lflag = ICANON;
  newtio.c_cc[VMIN]=1;
  newtio.c_cc[VTIME]=0;
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&newtio);
         
  /* loop while waiting for input. normally we would do something
     useful here */ 
  while (keep_going) {
    mpv_event *event = mpv_wait_event(handle, 0);
    /* printf("event: %s\n", mpv_event_name(event->event_id)); */
    if (event->event_id != MPV_EVENT_NONE){
      keep_going = handle_mpv_events(handle);
    }
    /* after receiving SIGIO, wait_flag = FALSE, input is available
       and can be read */
    if (wait_flag==FALSE) {
      char *p;
      res = read(fd,buf,255);
      buf[res]=0;
      for (p = buf; res-- > 0; p++) {
	if (*p < ' ') *p = '\0';   /* replace any control chars with NULL bytes */
      }
      do_mpv_command(buf, handle);
      wait_flag = TRUE;      /* wait for new input */
    }
  }
  /* restore old port settings */
  tcsetattr(fd,TCSANOW,&oldtio);
  return 0;
}
                
void signal_handler_IO (int status)
{
  wait_flag = FALSE;
}
    
