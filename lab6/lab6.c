#include <stdio.h>
#include <X11/Xlib.h>
#include <string.h>
#include "stdlib.h"

Display *dspl;
int screen;
Window hwnd;
XEvent event;
GC gc;
int x,y,w,h;
int count = 0;

void main(){

dspl = XOpenDisplay(NULL);
if(dspl == 0){
printf("Error XOpenDisplay\n");
return;
}
screen = XDefaultScreen(dspl);

hwnd = XCreateSimpleWindow(dspl, RootWindow(dspl, screen), 100, 50, 300, 200, 3, BlackPixel(dspl, screen), WhitePixel(dspl, screen));
if(hwnd == 0){
printf("Error XCreateSimpleWindow\n");
return;
}
XSelectInput(dspl, hwnd, KeyPressMask | ButtonPressMask | ExposureMask | StructureNotifyMask);
XMapWindow(dspl, hwnd);
gc=XCreateGC(dspl, hwnd, 0, NULL);

int k = 10;
char text[1];
char c[15];
char buf[64];

KeySym key;

	while(1){
		XNextEvent(dspl, &event);
	
		if(event.type == KeyPress && XLookupString(&event.xkey, text, 255, &key, 0) == 1){
			if(text[0] == 'q') { 
				XCloseDisplay ( dspl );
				return;
			}
		XClearWindow(dspl, hwnd);
		gc = XCreateGC ( dspl, hwnd, 0 , NULL );
		XSetForeground ( dspl, gc, BlackPixel ( dspl, 0) );
		printf("button: %c \n", text[0]);
	
			if(XDrawString ( dspl, hwnd, gc, k, 10, text, sizeof(text) )) k += 30;
			XFreeGC ( dspl, gc );
			XFlush(dspl);
		}

		if(event.type == ButtonPress) {
		XClearWindow(dspl, hwnd);
		gc = XCreateGC ( dspl, hwnd, 0 , NULL );
		XSetForeground ( dspl, gc, BlackPixel ( dspl, 0) );
	
		count = snprintf(c, 14,"X:%d Y:%d", event.xbutton.x, event.xbutton.y);
		XDrawString ( dspl, hwnd, gc, event.xbutton.x, event.xbutton.y, c, count);
		
		XFreeGC ( dspl, gc );
		XFlush(dspl);
		}
	
		if ( event.type == ConfigureNotify)
	            {
		gc = XCreateGC ( dspl, hwnd, 0 , NULL );
	                x = event.xconfigure.x;
	                y = event.xconfigure.y;
	                w = event.xconfigure.width;
	                h = event.xconfigure.height;
	                count = snprintf(buf, 64,"Position is X:%d Y:%d H:%d W:	%d",x,y,h,w);
	                XClearWindow(dspl,hwnd);
	                XDrawString(dspl, hwnd, gc, 10, 10,buf, count);
	                printf("Position is X:%d Y:%d H:%d W:%d\n",x,y,h,w);
	XFreeGC ( dspl, gc );
	XFlush(dspl);
	                
	}
	}
}
	
