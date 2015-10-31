#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#ifndef u_char
#define u_char unsigned char
#endif		

#include "artnavsegda.h"

u_char *decode_artnavsegda (char *filename, int *widthPtr, int *heightPtr) {
	int magiclength = strlen(artnavsegda);
	char magic[sizeof(artnavsegda)];
	int bytesPerPix;
	FILE *inFile;
	u_char *retBuf;
	
	inFile = fopen (filename, "rb");
	if (NULL == inFile) { 
		perror (NULL);
		return NULL;
	}

	fgets(magic,magiclength+1,inFile);
	if (strcmp(artnavsegda,magic)!=0)
	{
		printf("incorrect format\n");
		exit(1);
	}

	*widthPtr = fgetc(inFile);
	*heightPtr = fgetc(inFile);

	retBuf = malloc (3 * (*widthPtr * *heightPtr));
		
	if (NULL == retBuf) {
		perror (NULL);
		return NULL;
	}
		
	int lineOffset = (*widthPtr * 3);
	int x;
	int y;
		
	for (y = 0; y < *heightPtr; ++y)
	{
		for (x = 0; x < lineOffset; ++x)
		{
			retBuf[(lineOffset * y) + x] = fgetc(inFile);
			++x;
			retBuf[(lineOffset * y) + x] = fgetc(inFile);
			++x;
			retBuf[(lineOffset * y) + x] = fgetc(inFile);
		}
	}
	fclose (inFile);
			
	return retBuf;
}

XImage *create_image_from_buffer (Display *dis, int screen, u_char *buf, int width, int height) {
	XImage *img = NULL;
	Visual *vis;
	double rRatio;
	double gRatio;
	double bRatio;
	int outIndex = 0;	
	int i;
	int numBufBytes = (3 * (width * height));
		
	vis = DefaultVisual (dis, screen);

	rRatio = vis->red_mask / 255.0;
	gRatio = vis->green_mask / 255.0;
	bRatio = vis->blue_mask / 255.0;
		
	size_t numNewBufBytes = (4 * (width * height));
	u_int32_t *newBuf = malloc (numNewBufBytes);
	
	for (i = 0; i < numBufBytes; ++i)
	{
		unsigned int r, g, b;
		r = (buf[i] * rRatio);
		++i;
		g = (buf[i] * gRatio);
		++i;
		b = (buf[i] * bRatio);
		r &= vis->red_mask;
		g &= vis->green_mask;
		b &= vis->blue_mask;
		newBuf[outIndex] = r | g | b;
		++outIndex;
	}		
		
	img = XCreateImage (dis,CopyFromParent, 24,ZPixmap, 0,(char *) newBuf,width, height,32, 0);
	//img = XCreateImage (dis,CopyFromParent, 24,ZPixmap, 0,(char *) buf,width, height,32, 0);
	XInitImage (img);
	img->byte_order = LSBFirst;
	img->bitmap_bit_order = MSBFirst;

	return img;
}		

Window create_window (Display *dis, int screen, int x, int y, int width, int height) {
	Window win;
	unsigned long windowMask;
	XSetWindowAttributes winAttrib;	
			
	windowMask = CWBackPixel | CWBorderPixel;	
	winAttrib.border_pixel = BlackPixel (dis, screen);
	winAttrib.background_pixel = BlackPixel (dis, screen);
	winAttrib.override_redirect = 0;
		
	win = XCreateWindow (dis, DefaultRootWindow (dis),x, y,	width, height,0, DefaultDepth (dis, screen),InputOutput, CopyFromParent,windowMask, &winAttrib);
		
	return win;
}	
		
int main (int argc, char *argv[])
{
	int imageWidth;
	int imageHeight;
	XImage *img;		
	Window mainWin;
	int screen;
	Display *dis;
	u_char *buf;
	GC copyGC;

	if (2 != argc) {
		fprintf (stderr, "please specify a filename to %s\n", argv[0]);
		exit (EXIT_FAILURE);
	}

	buf = decode_artnavsegda (argv[1], &imageWidth, &imageHeight);
	
	if (NULL == buf) {
		fprintf (stderr, "unable to decode JPEG");
		exit (EXIT_FAILURE);
	}
	
	dis = XOpenDisplay (NULL);
	screen = DefaultScreen (dis);
	
	img = create_image_from_buffer (dis, screen, buf, imageWidth, imageHeight);
			
	if (NULL == img) {
		exit (EXIT_FAILURE);		
	}
				
	free (buf);
					
	mainWin = create_window (dis, screen, 20, 20, imageWidth, imageHeight);
	copyGC = XCreateGC (dis, mainWin, 0, NULL);
		
	XMapRaised (dis, mainWin);
	
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
