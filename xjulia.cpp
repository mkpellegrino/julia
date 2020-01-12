#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
Display *d;
Window w;
GC gc;
XWindowAttributes a;

int screen;
int XRES=0;//200;
int YRES=0;//200;
double ZOOM=1;

double x_offset=0;
double y_offset=0;

extern "C" int julia(double,double,double,double);
extern "C" void set_n(int);


void init_x()
{
/* get the colors black and white (see section for details) */        
	unsigned long black,white;

	d=XOpenDisplay((char *)0);
   	screen=DefaultScreen(d);
	black=BlackPixel(d,screen),
	white=WhitePixel(d,screen);
   	w=XCreateSimpleWindow(d,DefaultRootWindow(d),0,0,300, 300, 5,black, white);
	
	Atom wmDelete=XInternAtom(d, "WM_DELETE_WINDOW", True);

	XSetWMProtocols(d, w, &wmDelete, 1);
	XSelectInput(d, w, KeyPressMask|ButtonPressMask|ExposureMask|StructureNotifyMask);
        gc=XCreateGC(d, w, 0,0);
	XMapWindow(d, w);
	

	XSetBackground(d,gc,white);
	XSetForeground(d,gc,black);
	XMapRaised(d, w);
};

void close_x()
{
	XFreeGC(d, gc);
	XDestroyWindow(d,w);
	XCloseDisplay(d);	
};

void drawMenu()
{
  
  XGetWindowAttributes(d,w,&a);
  int x=a.width;
  int y=a.height;
  XClearArea(d,w,0,y-20, x, 20, 0);
  XDrawRectangle(d,w,DefaultGC(d, screen),0,y-21,x,20);
  XDrawString(d, w, DefaultGC(d, screen), 10, a.height-5, "EXIT", strlen("EXIT"));
  XDrawString(d, w, DefaultGC(d, screen), a.width-50, a.height-5, "RESET", strlen("RESET"));
  XDrawString(d, w, DefaultGC(d, screen), 80, a.height-5, "(1) i++ (2) i-- (=) zoom+ (-) zoom-  (arrows) move ctr", strlen("(1) i++ (2) i-- (=) zoom+ (-) zoom-  (arrows) move ctr"));
  return;
}
void draw(double cr, double cc, int iterations, double ZOOM)
{
#ifdef DEBUG
  printf( "draw\tcr=%lf\tcc=%lf\titerations=%d\tzoom=%lf\n", cr, cc, iterations, ZOOM );
#endif
  XClearWindow(d, w);
  
  XGetWindowAttributes(d,w,&a);
  set_n(iterations);


  if( a.width==0 || a.height==0 ) return;


  // Square Boundaries are
  // -2 and 2
  double boundary=((double)2)/ZOOM;
#ifdef DEBUG
  printf( "boundary: %lf\n", boundary );
#endif  
  

  for( double i=0; i<=a.width; i++)
    {
      double i2=( i/a.width )*2*boundary - boundary;
      for( double j=0; j<a.height; j++ )
	{
	  double j2=boundary-(j/a.height)*2*boundary;
	  
	  if(julia(i2+x_offset,j2+y_offset,cr,cc)>0)
	    {
	      XDrawPoint(d,w,DefaultGC(d,DefaultScreen(d)), i, j);
	    }
	}
    }
  XClearArea(d,w,0,0, a.width, 20, 0);
  char* title="X Julia - (C) 2019 - Michael K. Pellegrino - mkpelleg@freeshell.org";
  XDrawString(d, w, DefaultGC(d, screen), a.width/2 - 195 /*x*/, 10 /*y*/, title, strlen(title));

  XSync(d, 0);
};

int main(int argc, char** argv)
{
  
  double cr=1.0397;
  double cc=0.15280;
  double center_x_offset=0;
  double center_y_offset=0;



  int iterations=32;

  
  printf( "\nX Julia - (C) 2019 - Michael K. Pellegrino - mkpelleg@freeshell.org\n");
      
  switch( argc )
    {
    case 6:
      if( atof(argv[5]) == 0 )
	{
	  iterations=32;
	}
      else
	{
	  iterations=atof( argv[5] );
	}
    case 5:
      center_x_offset=atof( argv[4] );
      center_y_offset=atof( argv[3] );
    case 3:
      cr=atof( argv[1] );
      cc=atof( argv[2] );
    case 1:
      printf( "Params: CR: %lf, CC %lf, ctr: (%lf,%lf), n= %d\n", cr, cc, center_x_offset, center_y_offset, iterations );
      break;
    default:
      printf( "Requires a number of numeric inputs.\n" );
      printf( "%s C_real C_imag ZoomFactor CtrX CtrY iterations\n", argv[0] );
      printf( "Try:\n" );
      printf( "%s 1.0397 0.1528 200 22 3 10\n", argv[0] );
      printf( "%s -0.435 0.624 2\n", argv[0] );
      printf( "%s 0.8 -0.1 2.35\n", argv[0] );
      printf( "%s 1 0.12 1.1\n", argv[0] );
      printf( "%s 0.75 -0.12 8.2\n", argv[0] );
      exit(-1);
    }

  XEvent e;
  int pixel_count=0;

  d = XOpenDisplay(NULL); 
  
  if (d == NULL)
    {
      fprintf(stderr, "Cannot open display\n");
      exit(1);
    }
   init_x();

   int t=1;
   int first=1;
   int x,y;// button click x and y
   while( XCheckMaskEvent(d,KeyPressMask|ButtonPressMask|ExposureMask|StructureNotifyMask, &e ))
     {
       XNextEvent(d, &e);
     }


   while(t)
     {
       XNextEvent(d, &e);
       switch( e.type )
	 {
	 case ButtonPress:
	   XGetWindowAttributes(d,w,&a);
	   
	   switch(e.xbutton.button)
	     {
	     case Button1:
	       x=e.xbutton.x;
	       y=e.xbutton.y;
	       
	       if( x<30 && y>(a.height-20))
		 {
		   close_x();
		   exit(0);
		 }
	       if( x>a.width-70 && y>(a.height-20))
		 {
		   ZOOM=1;
		   x_offset=0;
		   y_offset=0;
		   iterations=32;
		   
		 }
	       break;
	     case Button2:
	       break;
	     case Button3:
	       break;
	     default:
	       break; 
	     }
	 case ConfigureNotify:
	   break;
	 case Expose:
	   break;
	 case KeyPress:

	   printf( "Keypress: %d\n", e.xkey.keycode ); 
	   if ( e.xkey.keycode == 32 )
	     {
	       ZOOM*=1.1;
	     }
	   if ( e.xkey.keycode == 35 )
	     {
	       ZOOM/=1.1;
	     }
	   if( e.xkey.keycode == 26 )
	     {
	       iterations++;
	       set_n(iterations);
	     }
	   if( e.xkey.keycode == 27 )
	     {
	       iterations--;
	       set_n(iterations);
	     }
	   if( e.xkey.keycode == 131 )
	     {
	       x_offset-=1/ZOOM;
	     }
	   if( e.xkey.keycode == 132 )
	     {
	       x_offset+=1/ZOOM;
	     }

	   if( e.xkey.keycode == 133 )
	     {
	       y_offset-=1/ZOOM;
	     }
	   if( e.xkey.keycode == 134 )
	     {
	       y_offset+=1/ZOOM;
	     }
	   break;

	   
	 default:
	   printf( "default\n" );
	   
	   break;
	 } /* end switch */

       // clear out any superfluous events
       while( XCheckMaskEvent(d,KeyPressMask|ButtonPressMask|ExposureMask|StructureNotifyMask, &e ))
	 {
	   XNextEvent(d, &e);
	 }
       
       draw(cr,cc,iterations, ZOOM);	   
       drawMenu();

     }
   close_x();
   return (0);
}
