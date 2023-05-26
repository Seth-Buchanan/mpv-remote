#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <json-c/json.h>

#include <mpv/client.h>
#define TRUE 1
#define FALSE 0

/* Code sourced from this video:
   https://www.youtube.com/watch?v=dQyXuFWylm4 */
struct json_object *parsed_json;

int parse_json() {

  FILE *fp;
  char buffer[1024];

  fp = fopen("test.json", "r");
  fread(buffer, 1024, 1, fp);
  fclose(fp);

  parsed_json = json_tokener_parse(buffer);
  return 0;
}

void doMpvCmd(char text[]){
  struct json_object *name;
  printf("%s\n", text);
  json_object_object_get_ex(parsed_json, text, &name);
  printf("Called Command: %s\n", json_object_get_string(name));  
}

/* Code sourced from this video:
   https://www.youtube.com/watch?v=n2s8Y8slL28 */
int start_scanning() {
  int fd, len;
  char text[255];
  struct termios options; /* Serial ports setting */

  fd = open("/dev/ttyACM0", O_RDWR | O_NDELAY | O_NOCTTY);
  /* Set read/write, no waiting until amount of chars is received
     and no control tty */

  if (fd < 0) {
    perror("Error opening serial port");
    return -1;
  }
  /* Read current serial port settings  */
  // tcgetattr(fd, &options);

  
  options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
  /* Set Baud rate, the size of data, idk, and to read the port */
  options.c_iflag = IGNPAR;	/* Makes no interupts when a parity error is encountered */
  options.c_oflag = 0;		/* No ouptut settings */
  options.c_lflag = 0;		/* No local settins */
  
  /* Apply the settings */
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd, TCSANOW, &options);

  while(TRUE){
    sleep(.1);
    memset(text, 0, 255);
    len = read(fd, text, 255);	/* Max number of bytes you can read is 255 */
    if (text[0] != '\0') { doMpvCmd(text); }
  }

  
  close(fd);
  return 0;
}


int main(){
  parse_json();
  start_scanning();
  
}
