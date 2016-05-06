#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <iostream>

#define WIDTH 256
#define HEIGHT 256
FILE *moo;

int x, y, i, j;
unsigned char temp, prev;
char done;
	
using namespace cv;


int main( int argc, char** argv )
{
//char* imageName = argv[1];
Mat image = Mat(HEIGHT, WIDTH, CV_8UC1);
moo=fopen(argv[1], "r");

// read led value
while (fscanf(moo, "%d\n",&temp)!=-1){
	if (temp==0xc){
		if (prev!=0xc){
			i= 0;
			j= 0;
			if (done) {
				// save image
				imwrite("wokoko.jpg", image);} 
				imshow( "result", image );
			if (!done) done= 1;
			}
		}
	else if (temp==0x3){
		if (prev!=0x3){
			j++;
			i= 0;
			printf("rows %d\n",j);
			}
		}
	else if (i<WIDTH){
		//pix[j][i]= temp;
		image.data[j*image.cols+i]= temp;
		i++;
		}
	}
 
 waitKey(0);

 return 0;
}
