#include <stdlib.h>
#include <stdio.h>

// Need to convert YUV422 to RGB24
// The YUV422 data format shares U and V values between two pixels. As a result, these values are transmitted to the PC image buffer only once for every two pixels, resulting in an average transmission rate of 16 bits per pixel.
// https://www.flir.com/support-center/iis/machine-vision/knowledge-base/understanding-yuv-data-formats/#:~:text=The%20YUV422%20data%20format%20shares,V2%20Y3%20U4%20Y4%20V4%E2%80%A6
// U0 Y0 V0 Y1 U2 Y2 V2 Y3 U4 Y4 V4…
//#define YUV422

// But my USB camera outputs 'YUYV' (YUYV 4:2:2) which is indeed Y U Y2 V
// YUY2: "Known as YUY2, YUYV, V422 or YUNV"
// https://stackoverflow.com/questions/36228232/yuy2-vs-yuv-422
#define YUYV

#define ALGO 3

/*
  Compile thusly:
  gcc -Wall -o yuv2ppm yuv2ppm.c
*/
/* example.ppm
P3
# feep.ppm
4 4
15
 0  0  0    0  0  0    0  0  0   15  0 15
 0  0  0    0 15  7    0  0  0    0  0  0
 0  0  0    0  0  0    0 15  7    0  0  0
15  0 15    0  0  0    0  0  0    0  0  0
*/

// PPM can be binary or ASCII, define BINOUTPUT
// define BINOUTPUT

// Compat with arduino C++
#define uint8_t u_int8_t 
#define uint16_t u_int16_t 

uint16_t IMAGE_W = 320;
uint16_t IMAGE_H = 240;

int make_outfile(char *outfile, char *infile)
{
   int i;
   
   for(i=0; infile[i]; i++) outfile[i] = infile[i];
   outfile[i++] = '.';
   outfile[i++] = 'p';
   outfile[i++] = 'p';
   outfile[i++] = 'm';
   outfile[i++] = 0;
   
   return 0;
}
void yuv2rgb(uint8_t y, uint8_t u, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
   int16_t R, G, B;
   
#if ALGO == 1
   #pragma message "Algorithm 1"
   R = 1.164 * (y - 16) +                     2.018 * (v - 128);
   G = 1.164 * (y - 16) - 0.813 * (u - 128) - 0.391 * (v - 128);
   B = 1.164 * (y - 16) + 1.596 * (u - 128);

#elif ALGO == 2
   #pragma message "Algorithm 2"
   R = 1.164 * (y - 16)			    + 1.596 * (v - 128);
   G = 1.164 * (y - 16) - 0.391 * (u - 128) - 0.813 * (v - 128);
   B = 1.164 * (y - 16) + 2.018 * (u - 128);
   
#elif ALGO == 3
   // This seems to have the least amount of math and display is ok
   #pragma message "Algorithm 3"
   R = y				    + 1.370705 * (v-128);
   G = y		- 0.337633 * (u-128)- 0.698001 * (v-128) ;
   B = y		+ 1.732446 * (u-128);
   
#elif ALGO == 4
   // This looks short but the colors are wrong
   #pragma message "Algorithm 4"
   R = y + 1.403 * v;
   G = y - 0.344 * u - 0.714 * v;
   B = y + 1.770 * u;
#endif

   // Even with proper conversion, some values still need clipping.
   if (R > 255) R = 255;
   if (G > 255) G = 255;
   if (B > 255) B = 255;
   if (R < 0) R = 0;
   if (G < 0) G = 0;
   if (B < 0) B = 0;
   // bring down brightness a bit, but this adds more math (slowdown)
   *r = R * 220 / 256;
   *g = G * 220 / 256;
   *b = B * 220 / 256;
   // printf("yuv2rgb(%2x, %2x, %2x) -> %2x, %2x, %2x\n", y, u, v, *r, *g, *b);
   
   return;
}
int yuv2ppm(char *infile, char *outfile)
{
   FILE *in, *out;
   uint8_t y, u, v, y2;
   uint8_t r, g, b;
   
#ifdef BINOUTPUT
   char *ppmheader = "P6\n# Generated by yuv2ppm\n";
#else
   char *ppmheader = "P3\n# Generated by yuv2ppm\n";
#endif
   in = fopen(infile, "rb");
   out = fopen(outfile, "wb");
   if (!in  ||  !out)  return 0;
   fprintf(out, ppmheader);
   fprintf(out, "%i %i\n255\n", IMAGE_W, IMAGE_H);
   for(int i=0; i<IMAGE_W*IMAGE_H/2; i++)
   {
      // https://www.flir.com/support-center/iis/machine-vision/knowledge-base/understanding-yuv-data-formats/#:~:text=The%20YUV422%20data%20format%20shares,V2%20Y3%20U4%20Y4%20V4%E2%80%A6
      // U0 Y0 V0 Y1 U2 Y2 V2 Y3
    #ifdef YUV422
      fread(&u, 1, 1, in);
      fread(&y, 1, 1, in);
      fread(&v, 1, 1, in);
      fread(&y2, 1, 1, in);
    // called YUY2 in mplayer, mplayer -demuxer rawvideo -rawvideo w=320:h=240:format=yuy2 out.yuyv
    #elif defined(YUYV) 
      fread(&y, 1, 1, in);
      fread(&u, 1, 1, in);
      fread(&y2, 1, 1, in);
      fread(&v, 1, 1, in);
    #else
    #error "do not know YUV format"
    #endif
      
      //printf("u:%2x y:%2x v:%2x y2:%2x\n", u, y, v, y2);
      yuv2rgb(y, u, v, &r, &g, &b);

	
#ifdef BINOUTPUT
      fwrite(&r, 1, 1, out);
      fwrite(&g, 1, 1, out);
      fwrite(&b, 1, 1, out);
#else
      fprintf(out, "%u %u %u\n", r, g, b);
#endif
      
#ifdef BINOUTPUT
      yuv2rgb(y2, u, v, &r, &g, &b);
      fwrite(&r, 1, 1, out);
      fwrite(&g, 1, 1, out);
      fwrite(&b, 1, 1, out);
#else
      fprintf(out, "%i %i %i\n", r, g, b);
#endif
	 
   }
   fclose(in);
   fclose(out);
   return 1;
}
int main(int argc, char **argv)
{
  char *infile, outfile[256];
  int i;
  int success;
  
  for(i=1; i<argc; i++)
  {
     success = 0;
     infile = argv[i];
     make_outfile(outfile, infile);
     
     printf("%s -> %s\n", infile, outfile);
     fflush(stdout);
     success = yuv2ppm(infile, outfile);
     
     if(success)
     {
	printf("Done.\n");
     }
     else
     {
	printf("Failed.  Aborting.\n");
	return 1;
     }
  }
  return 0;
}
