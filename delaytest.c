#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <png.h>

int delay;
int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers;
struct timeval tv_start, tv_stop;
char command[80]; 
unsigned char temp;
int fd_led[9];

// LED to GPIO pin number, match this to your wiring
// LED[8] is for frame 'trigger'
char LED[9]= {5, 6, 13, 19, 26, 12, 16, 20, 21};

int gpio_init(){
int i;
for (i=0; i<=9; i++){
	sprintf(command,"echo %d >> /sys/class/gpio/export", LED[i]);
	if (system(command)==-1) exit(1);
	sprintf(command,"echo out > /sys/class/gpio/gpio%d/direction", LED[i]);
	if (system(command)==-1) exit(1);
	sprintf(command,"/sys/class/gpio/gpio%d/value", LED[i]);
	fd_led[i]= open(command, O_WRONLY);
	if (fd_led[i]==-1) exit(2);
	}
printf("initialized GPIO-out: %d %d %d %d %d %d %d %d %d\n",\
	LED[0], LED[1], LED[2], LED[3], LED[4], LED[5], LED[6], LED[7], LED[8]);
}

void gpio_blink(char value){
	if (value&1) write(fd_led[0], "0",1);
	else write(fd_led[0], "1",1);
	if (value&2) write(fd_led[1], "0",1);
	else write(fd_led[1], "1",1);
	if (value&4) write(fd_led[2], "0",1);
	else write(fd_led[2], "1",1);
	if (value&8) write(fd_led[3], "0",1);
	else write(fd_led[3], "1",1);
	if (value&16) write(fd_led[4], "0",1);
	else write(fd_led[4], "1",1);
	if (value&32) write(fd_led[5], "0",1);
	else write(fd_led[5], "1",1);
	if (value&64) write(fd_led[6], "0",1);
	else write(fd_led[6], "1",1);
	if (value&128) write(fd_led[7], "0",1);
	else write(fd_led[7], "1",1);
	write(fd_led[8], "1",1);
	usleep(delay);
	write(fd_led[8], "0",1); // the 'trigger' thing
	}

int main(int argc, char **argv){
	gpio_init();
	delay=atoi(argv[1]);
	printf("delay= %d\n",delay);
	char tempval;
	gettimeofday(&tv_start, NULL);
	while(1){
	gpio_blink(tempval++);
	gettimeofday(&tv_stop, NULL);
	gettimeofday(&tv_start, NULL);
	printf("\r interval: %d usec\n",(tv_stop.tv_sec - tv_start.tv_sec)*1000+\
	tv_stop.tv_usec - tv_start.tv_usec);
	}
    }
