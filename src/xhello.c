#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
int main(int argc, char *argv[])
{
	char buf[100];
	int i = 0;
	int x = 0;
	int r = 0;
	Display *d;
	Window w;
	XEvent e;
	XSetWindowAttributes wa;
	char *msg = "Hello, World!";
	int s;
 
	d = XOpenDisplay(NULL);
	if (d == NULL)
	{
		fprintf(stderr, "Cannot open display\n");
		exit(1);
	}
	else
		fprintf(stderr, "display sucessfully opened\n");
 
	s = DefaultScreen(d);
	wa.border_pixel = BlackPixel(d, s);
	wa.background_pixel = WhitePixel(d, s);
	wa.override_redirect = 0;
	wa.event_mask = ExposureMask | KeyPressMask;
	w = XCreateWindow(d, RootWindow(d, s), 10, 10, 300, 300, 1, DefaultDepth(d, s),InputOutput,CopyFromParent,CWBackPixel | CWBorderPixel,&wa);
	//w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, 100, 100, 1, BlackPixel(d, s), WhitePixel(d, s));
	XSelectInput(d, w, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
	XMapWindow(d, w);
	Atom WM_DELETE_WINDOW = XInternAtom(d, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(d, w, &WM_DELETE_WINDOW, 1);
 
	while (1)
	{
		XNextEvent(d, &e);
		//printf("event %d\n",r++);
		switch(e.type)
		{
		case ConfigureNotify:
			//printf("structure %d %d %d\n",x++,e.xconfigure.width,e.xconfigure.height);
			break;
		case Expose:
			printf("expose %d %d %d %d %d\n",i++,e.xexpose.x,e.xexpose.y,e.xexpose.width,e.xexpose.height);
			XFillRectangle(d, w, DefaultGC(d, s), 75, 75, 150, 150);
			//XDrawString(d, w, DefaultGC(d, s), 10, 50, msg, strlen(msg));
			break;
		case KeyPress:
			XLookupString(&e.xkey,buf,100,NULL,NULL);
			printf("keypress %s\n",buf);
			switch(XLookupKeysym (&e.xkey, 0))
			{
			case XK_q:
				XCloseDisplay(d);
				exit(0);
				break;
			}
			break;
		case ClientMessage:
			XCloseDisplay(d);
			exit(0);
			break;
		case ButtonPress:
			break;
		}
	}
	return 0;
}

