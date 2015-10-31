#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "artnavsegda.h"

XImage *create_image_from_buffer (Display *dis, int screen, int *width, int *height) {
	int magiclength = strlen(artnavsegda);
	char magic[sizeof(artnavsegda)];
	fgets(magic,magiclength+1,stdin);
	if (strcmp(artnavsegda,magic)!=0)
	{
		printf("incorrect format\n");
		exit(1);
	}
	*width = fgetc(stdin);
	*height = fgetc(stdin);
	XImage *img = NULL;
	Visual *vis;
	double rRatio;
	double gRatio;
	double bRatio;
	int outIndex = 0;	
	int i;
	int numBufBytes = (3 * (*width * *height));
	vis = DefaultVisual (dis, screen);
	rRatio = vis->red_mask / 255.0;
	gRatio = vis->green_mask / 255.0;
	bRatio = vis->blue_mask / 255.0;
	size_t numNewBufBytes = (4 * (*width * *height));
	u_int32_t *newBuf = malloc (numNewBufBytes);
	for (i = 0; i < numBufBytes; ++i)
	{
		unsigned int r, g, b;
		r = (fgetc(stdin) * rRatio);
		++i;
		g = (fgetc(stdin) * gRatio);
		++i;
		b = (fgetc(stdin) * bRatio);
		//r &= vis->red_mask;
		//g &= vis->green_mask;
		//b &= vis->blue_mask;
		newBuf[outIndex] = r | g | b;
		++outIndex;
	}		
	img = XCreateImage (dis,CopyFromParent, 24,ZPixmap, 0,(char *) newBuf,*width, *height,32, 0);
	XInitImage (img);
	img->byte_order = LSBFirst;
	img->bitmap_bit_order = MSBFirst;
	return img;
}		

int main (int argc, char *argv[])
{
	int imageWidth;
	int imageHeight;
	XImage *img;		
	Window mainWin;
	unsigned long windowMask;
	XSetWindowAttributes winAttrib;
	int screen;
	Display *dis;
	GC copyGC;
	dis = XOpenDisplay (NULL);
	screen = DefaultScreen (dis);
	img = create_image_from_buffer (dis, screen, &imageWidth, &imageHeight);
	windowMask = CWBackPixel | CWBorderPixel;
	winAttrib.border_pixel = BlackPixel (dis, screen);
	winAttrib.background_pixel = BlackPixel (dis, screen);
	winAttrib.override_redirect = 0;
	mainWin = XCreateWindow (dis, DefaultRootWindow (dis),20, 20,imageWidth,imageHeight,0, DefaultDepth (dis, screen),InputOutput, CopyFromParent,CWBackPixel | CWBorderPixel, &winAttrib);
	copyGC = XCreateGC (dis, mainWin, 0, NULL);
	XMapWindow (dis, mainWin);
	XSelectInput(dis, mainWin, ExposureMask | KeyPressMask);
	Atom WM_DELETE_WINDOW = XInternAtom(dis, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dis, mainWin, &WM_DELETE_WINDOW, 1);

	while (1) {
		XEvent event;
		XNextEvent (dis, &event);
		switch  (event.type) {
			case Expose:
				XPutImage (dis, mainWin, copyGC, img, 0, 0, 0, 0, imageWidth, imageHeight);
				XFlush (dis);
			break;            
			case KeyPress:
				if (XK_q == XLookupKeysym (&event.xkey, 0)) {
					exit (EXIT_SUCCESS);
				}
			break;
			case ClientMessage:
				XCloseDisplay(dis);
				exit (EXIT_SUCCESS);
			break;
		}			
	}
	return EXIT_FAILURE;
} 
