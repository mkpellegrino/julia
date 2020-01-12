#include <stdlib.h>
#include <stdio.h>
#include <math.h>
using namespace std;

#define CR 0.75
#define CC -0.12

extern "C" bool julia(double,double,double,double);
extern "C" void set_n(int);

int main(int argc, char **argv)
{
  double cc = CC;
  double cr = CR;
  double scale=1.000;
  int pixel_count=0;
  double center_x_offset=0;
  double center_y_offset=0;
  int iterations=32;
  printf( "\n     ASCII Julia - (C) 2019 - Michael K. Pellegrino - mkpelleg@freeshell.org\n");
      
  switch( argc )
    {
    case 7:
      if( atof(argv[6]) == 0 )
	{
	  iterations=32;
	}
      else
	{
	  iterations=atof( argv[6] );
	}
    case 6:
      center_x_offset=atof( argv[5] );
      center_y_offset=atof( argv[4] );
    case 4:
      scale=atof( argv[3] );
    case 3:
      cr=atof( argv[1] );
      cc=atof( argv[2] );
    case 1:
      printf( "Params: CR: %lf, CC %lf, Zoom: %lf, ctr: (%lf,%lf), n=%d\n", cr, cc, scale, center_x_offset, center_y_offset, iterations );
      break;
    default:
      printf( "Requires 0,2,3,5, or 6 numeric inputs.\n" );
      printf( "%s C_real C_imag ZoomFactor CtrX CtrY iterations\n", argv[0] );
      printf( "Try:\n" );
      printf( "%s 1.0397 0.1528 200 22 3 10\n", argv[0] );
      printf( "%s -0.435 0.624 2\n", argv[0] );
      printf( "%s 0.8 -0.1 2.35\n", argv[0] );
      printf( "%s 1 0.12 1.1\n", argv[0] );
      printf( "%s 0.75 -0.12 8.2\n", argv[0] );
      exit(-1);
    }

  for( int i=0; i<82; i++ ) printf( "=" );  printf( "\n" ); 
  set_n(iterations);

  for( double i=(-2-center_x_offset); i<=(2-center_x_offset); i+=0.05 )
    {
      printf( "|" );
      for( double j=(2-center_y_offset); j>=(-2-center_y_offset); j-=0.05 )
	{
	  if( julia(i/scale,j/scale,cr,cc)>0 )
	    {
	      printf( "*" );
	    }
	  else
	    {
	      printf( " " );
	    }
	}
      printf( "|\n" );
    }
  for( int i=0; i<82; i++ ) printf( "=" );  printf( "\n" ); 


  
}
