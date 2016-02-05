#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

// compile against opencv
// gcc image2array.c `pkg-config --libs --cflags opencv`

char command[80]; 
unsigned char temp;
int fd_led[9];

// LED to GPIO pin number, match this to your wiring
// LED[8] is for frame 'trigger'
char LED[9]= {16, 16, 16, 16, 16, 16, 16, 16, 16};

void gpio_init(){
int i;
for (i=0; i<9; i++){
	sprintf(command,"echo %d >> /sys/class/gpio/export", LED[i]);
	if (system(command)) return(1);
	sprintf(command,"echo out > /sys/class/gpio/gpio%d/direction", LED[i])
	if (system(command)) return(1);
	sprintf(command,"/sys/class/gpio/gpio%d/value", LED[i]);
	fd_led[i]= open(command, O_WRONLY);
	if (fd_led[i]==-1) return(2);
	}
printf("initialized GPIO-out: %d %d %d %d %d %d %d %d %d\n",\
	LED[0], LED[1], LED[2], LED[3], LED[4], LED[5], LED[6], LED[7], LED[8]);
}

void gpio_blink(char value){
	if (value&1) write(fd[0], "1",1);
	else write(fd_led[0], "0",1);
	if (value&2) write(fd_led[1], "1",1);
	else write(fd_led[1], "0",1);
	if (value&4) write(fd_led[2], "1",1);
	else write(fd_led[2], "0",1);
	if (value&8) write(fd_led[3], "1",1);
	else write(fd_led[3], "0",1);
	if (value&16) write(fd_led[4], "1",1);
	else write(fd_led[4], "0",1);
	if (value&32) write(fd_led[5], "1",1);
	else write(fd_led[5], "0",1);
	if (value&64) write(fd_led[6], "1",1);
	else write(fd_led[6], "0",1);
	if (value&128) write(fd_led[7], "1",1);
	else write(fd_led[7], "0",1);
	write(fd_led[8], "1",1); // the 'trigger' thing
	write(fd_led[8], "0",1);
}

int main(int argc, char **argv){
	gpio_init();
    CvMat *matA = cvLoadImageM(argv[1],0);
    if(!matA) return(-1);
    else printf("%s is %dx%d\n",argv[1],matA->cols,matA->rows);
	int i, j;
	imageblink:
    for (j=0; j<matA->rows; j++){
		if ((j==0) & (i==0)){
			gpio_blink(0xc);
			gpio_blink(0xc); // just to be safe
			}
		else{
			gpio_blink(0xd);
			gpio_blink(0xd); // just to be safe
			}
		for (i=0; i<matA->cols; i++){	
			temp= matA->data.ptr[j*matA->cols+i];
			if ((temp==0xc) || (temp==0xd)) gpio_blink(0xe);
			else gpio_blink(temp);
			}	
		}
	goto imageblink;	
    }
