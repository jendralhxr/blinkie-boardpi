/*
 * A simple libpng example program to manipulate raspi GPIO
 * --jendralhxr
 * 
 * compile against libpng
 * 
 * Huge thanks to:
 * 
 * http://zarb.org/~gc/html/libpng.html
 * (2002-2010) Guillaume Cottenceau.
 *
 * Yoshimasa Niwa to make it much simpler and support 
 * all defined color_type. (July 2013)
 * 
 * This software may be freely redistributed under the terms
 * of the X11 license.
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <png.h>

int width, height;
int delay;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers;

char command[80]; 
unsigned char temp;
int fd_led[9];

// LED to GPIO pin number, match this to your wiring
// LED[8] is for frame 'trigger'
char LED[9]= {5, 6, 13, 19, 26, 12, 16, 20, 21};

int gpio_init(){
int i;
for (i=0; i<9; i++){
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
	write(fd_led[8], "0",1); // the 'trigger' thing
	write(fd_led[8], "1",1);
	usleep(delay);
}


void read_png_file(char *filename) {
  FILE *fp = fopen(filename, "rb");

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) abort();

  png_infop info = png_create_info_struct(png);
  if(!info) abort();

  if(setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  png_read_info(png, info);

  width      = png_get_image_width(png, info);
  height     = png_get_image_height(png, info);
  color_type = png_get_color_type(png, info);
  bit_depth  = png_get_bit_depth(png, info);

  // Read any color_type into 8bit depth, RGBA format.
  // See http://www.libpng.org/pub/png/libpng-manual.txt

  if(bit_depth == 16)
    png_set_strip_16(png);

  if(color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  if(png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  // These color_type don't have an alpha channel then fill it with 0xff.
  if(color_type == PNG_COLOR_TYPE_RGB ||
     color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

  if(color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

  png_read_update_info(png, info);

  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
  int y;
  for(y = 0; y < height; y++) {
    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }

  png_read_image(png, row_pointers);

  fclose(fp);
}

int main(int argc, char **argv){
	gpio_init();
    read_png_file(argv[1]);
    delay=atoi(argv[1]);
    int i, j;
    png_bytep row, px;
	imageblink:
	gettimeofday(&tv_start, NULL);
    for (j=0; j<height; j++){
		row = row_pointers[j];
		if ((j==0) & (i==0)){
			gpio_blink(0xc);
			gpio_blink(0xc); // just to be safe
			}
		else{
			gpio_blink(0xd);
			gpio_blink(0xd); // just to be safe
			}
		for (i=0; i<width; i++){	
			px = &(row[i * 4]);
			temp= px[0];
			if ((temp==0xc) || (temp==0xd)) gpio_blink(0xe);
			else gpio_blink(temp);
			}	
		}
	gettimeofday(&tv_stop, NULL);
	printf("\r interval: %d usec\n",(tv_stop.tv_sec - tv_start.tv_sec)*1000+\
	tv_stop.tv_usec - tv_start.tv_usec);	
	gettimeofday(&tv_start, NULL);
	goto imageblink;	
    }
