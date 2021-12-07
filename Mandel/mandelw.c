//Sequential Mandelbrot (windows) Visual C
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <graphics.h>
#include <conio.h>

    typedef struct { float real, imag;} complex;

    int cal_pixel(complex c){
		int count = 0, max = 256;
		float temp, lengthsq;
		complex z;
		z.real = 0;
		z.imag = 0;
		do{
		   temp = z.real*z.real - z.imag*z.imag + c.real;
		   z.imag = 2*z.real*z.imag + c.imag;
		   z.real = temp;
		   lengthsq = z.real*z.real + z.imag*z.imag;
		   count++;
		  } while((lengthsq<4) && (count<max));
		return count;
    }

 int main () {
	int width  = 640, height = 480;
	int gdriver = 0, gmode = 0; //DETECT
	detectgraph(&gdriver, &gmode); 
	initgraph  (&gdriver, &gmode, "", 0,0);// Visual C

	float rmin = -2, imin = -2, rmax = 2, imax = 2; // OR get it from input

	float scale_real =(rmax - rmin)/width; //disp_width;
	float scale_imag =(imax - imin)/height;//disp_height;

	complex c;

	for(int x = 0; x < width; x++)
	   for(int y = 0; y < height; y++){
		 c.real = rmin + ((float) x*scale_real);
		 c.imag = imin + ((float) y*scale_imag);
		 int color = cal_pixel(c);
		 putpixel(x,y,color);
	    }

	getch();	
	closegraph();
	return 1;
}
