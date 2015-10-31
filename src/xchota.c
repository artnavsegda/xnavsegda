#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXLENGTH 1000
XPoint massive[2000];
 
int developmassive()
{
	int length = 0;
	int a, b, c, d, e, f, g, h, i, j, k, l, m;
	FILE *sora;
	sora = fopen("./chota.txt","r");
	if (sora == NULL)
	{
		printf("fucking error");
		exit(0);
	}
	while (fscanf(sora,"%d %d %d %d %d %d %d %d %d %d %d %d %d\n", &a,&b,&c,&d,&e,&f,&g,&h,&i,&j,&k,&l,&m)!=EOF)
	{
		length++;
		if (length > MAXLENGTH)
			break;
		massive[length].x = length;
		massive[length].y = e-2000;
	}
	return 0;
}

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
	w = XCreateWindow(d, RootWindow(d, s), 10, 10, 100, 100, 1, DefaultDepth(d, s),InputOutput,CopyFromParent,CWBackPixel | CWBorderPixel,&wa);
	//w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, 100, 100, 1, BlackPixel(d, s), WhitePixel(d, s));
	XSelectInput(d, w, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
	XMapWindow(d, w);
	Atom WM_DELETE_WINDOW = XInternAtom(d, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(d, w, &WM_DELETE_WINDOW, 1);

	developmassive();
 
	while (1)
	{
		XNextEvent(d, &e);
		printf("event %d\n",r++);
		if (e.type == ConfigureNotify)
		{
			printf("structure %d %d %d\n",x++,e.xconfigure.width,e.xconfigure.height);
		}
		if (e.type == Expose)
		{
			printf("expose %d %d %d %d %d\n",i++,e.xexpose.x,e.xexpose.y,e.xexpose.width,e.xexpose.height);
			XFillRectangle(d, w, DefaultGC(d, s), 20, 20, 10, 10);
			XDrawString(d, w, DefaultGC(d, s), 10, 50, msg, strlen(msg));
			XDrawPoints(d, w, DefaultGC(d, s), massive, 1000, CoordModeOrigin);
		}
		if (e.type == KeyPress)
		{
			XLookupString(&e.xkey,buf,100,NULL,NULL);
			printf("keypress %s\n",buf);
			if (XK_q == XLookupKeysym (&e.xkey, 0))
				break;
		}
		if (e.type == ClientMessage)
			break;
		if (e.type == ButtonPress)
			break;
	}
 
	XCloseDisplay(d);
	return 0;
}

