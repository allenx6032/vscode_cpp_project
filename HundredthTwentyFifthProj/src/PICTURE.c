/**************************************************************************
** picture.c
**
** Routines to load AGI picture resources, discard them, draw them and
** show the various screens.
**
** (c) Lance Ewing, 1997 - Modified (6 Jan 98)
**************************************************************************/

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dos.h>
#include <ctype.h>
#include <time.h>
#include <allegro.h>
#include <ctype.h>
#include <pc.h>

#include "general.h"
#include "agifiles.h"
#include "picture.h"

#define PIC_DEFAULT 16
#define PRI_DEFAULT 4

boolean okToShowPic=FALSE;
PictureFile loadedPictures[256];
int screenMode;
int min_print_line=1, user_input_line=23, status_line_num=0;
boolean statusLineDisplayed=FALSE, inputLineDisplayed=FALSE;

BITMAP *picture;
BITMAP *priority;
BITMAP *control;
BITMAP *agi_screen;      /* This is a subbitmap of the screen */

boolean picDrawEnabled = FALSE, priDrawEnabled = FALSE;
/* Not sure what the default patCode value is */
byte picColour=0, priColour=0, patCode, patNum;

void showPicture();
void showPriority();


/* QUEUE DEFINITIONS */

#define QMAX 20000
#define QEMPTY 0xFF

word buf[QMAX+1];
word rpos = QMAX, spos = 0;

boolean qempty()
{
   return (rpos == spos);
}

void qstore(word q)
{
   if (spos+1==rpos || (spos+1==QMAX && !rpos)) {
      nosound();
      return;
   }
   buf[spos] = q;
   spos++;
   if (spos==QMAX) spos = 0;  /* loop back */
}

word qretrieve()
{
   if (rpos==QMAX) rpos=0;  /* loop back */
   if (rpos==spos) {
      return QEMPTY;
   }
   rpos++;
   return buf[rpos-1];
}

#define SWIDTH   640  /* Screen resolution */
#define SHEIGHT  480
#define PWIDTH   160  /* Picture resolution */
#define PHEIGHT  168
#define VWIDTH   640  /* Viewport size */
#define VHEIGHT  336

/**************************************************************************
** initPicture
**
** Purpose: To initialize allegro and create the picture and priority
** bitmaps. This function gets called once at the start of an AGI program.
**************************************************************************/
void initPicture()
{
   picture = create_bitmap(PWIDTH, PHEIGHT);
   priority = create_bitmap(PWIDTH, PHEIGHT);
   control = create_bitmap(PWIDTH, PHEIGHT);
   clear_to_color(picture, PIC_DEFAULT);
   clear_to_color(priority, PRI_DEFAULT);
   clear_to_color(control, PRI_DEFAULT);
}

/**************************************************************************
** initAGIScreen
**
** Purpose: Sets the screen mode to 640x480x256. This can be called a
** number of times during the program, e.g. when switching back from a
** text mode.
**************************************************************************/
void initAGIScreen()
{
   set_gfx_mode(GFX_AUTODETECT, SWIDTH, SHEIGHT, 0, 0);
   agi_screen = create_sub_bitmap(screen, 0, 20, 640, 336);
}

/**************************************************************************
** closePicture
**
** Purpose: Sets the screen back to text mode ready for exiting to DOS.
** This function is called once at the end of an AGI program.
**************************************************************************/
void closePicture()
{
   allegro_exit();
   textmode(C80);
   destroy_bitmap(picture);
   destroy_bitmap(control);
   destroy_bitmap(priority);
}

/**************************************************************************
** clearPicture
**
** Purpose: To clear the picture and priority bitmaps so that they are
** ready for drawing another PICTURE.
**************************************************************************/
void clearPicture()
{
   clear_to_color(picture, PIC_DEFAULT);
   clear_to_color(priority, PRI_DEFAULT);
   clear_to_color(control, PRI_DEFAULT);
}

void getRealPal(PALETTE pal)
{
   int c;
   byte r, g, b;

   outportb(0x3c7, 0);

   for (c=0; c<256; c++) {
      pal[c].r = inportb(0x3c9);
      pal[c].g = inportb(0x3c9);
      pal[c].b = inportb(0x3c9);
   }
}

void setRealPal(PALETTE pal)
{
   int c;
   byte r, g, b;

   outportb(0x3c8, 0);

   for (c=0; c<256; c++) {
      outportb(0x3c9, pal[c].r);
      outportb(0x3c9, pal[c].g);
      outportb(0x3c9, pal[c].b);
   }
}

void initPalette()
{
   #include "palette.h"

   setRealPal(pal);
}

/**************************************************************************
** picPSet
**
** Draws a pixel in the picture screen.
**************************************************************************/
void picPSet(word x, word y)
{
   if (x > 159) return;
   if (y > 167) return;
   picture->line[y][x] = picColour + 1;
}

/**************************************************************************
** priPSet
**
** Draws a pixel in the priority screen.
**************************************************************************/
void priPSet(word x, word y)
{
   if (x > 159) return;
   if (y > 167) return;
   priority->line[y][x] = priColour;
}

/**************************************************************************
** pset
**
** Draws a pixel in each screen depending on whether drawing in that
** screen is enabled or not.
**************************************************************************/
void pset(word x, word y)
{
   if (picDrawEnabled) picPSet(x, y);
   if (priDrawEnabled) priPSet(x, y);
}

/**************************************************************************
** picGetPixel
**
** Get colour at x,y on the picture page.
**************************************************************************/
byte picGetPixel(word x, word y)
{
   if (x > 159) return(PIC_DEFAULT);
   if (y > 167) return(PIC_DEFAULT);
   return (picture->line[y][x]);
}

/**************************************************************************
** priGetPixel
**
** Get colour at x,y on the priority page.
**************************************************************************/
byte priGetPixel(word x, word y)
{
   if (x > 159) return(PRI_DEFAULT);
   if (y > 167) return(PRI_DEFAULT);
   return (priority->line[y][x]);
}

/**************************************************************************
** round
**
** Rounds a float to the closest int. Takes into actions which direction
** the current line is being drawn when it has a 50:50 decision about
** where to put a pixel.
**************************************************************************/
int round(float aNumber, float dirn)
{
   if (dirn < 0)
      return ((aNumber - floor(aNumber) <= 0.501)? floor(aNumber) : ceil(aNumber));
   return ((aNumber - floor(aNumber) < 0.499)? floor(aNumber) : ceil(aNumber));
}

/**************************************************************************
** drawline
**
** Draws an AGI line.
**************************************************************************/
void drawline(word x1, word y1, word x2, word y2)
{
   int height, width, startX, startY;
   float x, y, addX, addY;

   height = (y2 - y1);
   width = (x2 - x1);
   addX = (height==0?height:(float)width/abs(height));
   addY = (width==0?width:(float)height/abs(width));

   if (abs(width) > abs(height)) {
      y = y1;
      addX = (width == 0? 0 : (width/abs(width)));
      for (x=x1; x!=x2; x+=addX) {
	 pset(round(x, addX), round(y, addY));
	 y+=addY;
      }
      pset(x2,y2);
   }
   else {
      x = x1;
      addY = (height == 0? 0 : (height/abs(height)));
      for (y=y1; y!=y2; y+=addY) {
	 pset(round(x, addX), round(y, addY));
	 x+=addX;
      }
      pset(x2,y2);
   }

}

/**************************************************************************
** okToFill
**************************************************************************/
boolean okToFill(byte x, byte y)
{
   if (!picDrawEnabled && !priDrawEnabled) return FALSE;
   if (picColour == PIC_DEFAULT) return FALSE;
   if (!priDrawEnabled) return (picGetPixel(x, y) == PIC_DEFAULT);
   if (priDrawEnabled && !picDrawEnabled) return (priGetPixel(x, y) == PRI_DEFAULT);
   return (picGetPixel(x, y) == PIC_DEFAULT);
}

/**************************************************************************
** agiFill
**************************************************************************/
void agiFill(word x, word y)
{
   byte x1, y1;
   rpos = spos = 0;

   qstore(x);
   qstore(y);

   for (;;) {

      x1 = qretrieve();
      y1 = qretrieve();

      if ((x1 == QEMPTY) || (y1 == QEMPTY))
	      break;
      else {

	 if (okToFill(x1,y1)) {

	    pset(x1, y1);

	    if (okToFill(x1, y1-1) && (y1!=0)) {
	       qstore(x1);
	       qstore(y1-1);
	    }
	    if (okToFill(x1-1, y1) && (x1!=0)) {
	       qstore(x1-1);
	       qstore(y1);
	    }
	    if (okToFill(x1+1, y1) && (x1!=159)) {
	       qstore(x1+1);
	       qstore(y1);
	    }
	    if (okToFill(x1, y1+1) && (y1!=167)) {
	       qstore(x1);
	       qstore(y1+1);
	    }

	 }

      }

   }

}

/**************************************************************************
** xCorner
**
** Draws an xCorner  (drawing action 0xF5)
**************************************************************************/
void xCorner(byte **data)
{
   byte x1, x2, y1, y2;

   x1 = *((*data)++);
   y1 = *((*data)++);

   pset(x1,y1);

   for (;;) {
      x2 = *((*data)++);
      if (x2 >= 0xF0) break;
      drawline(x1, y1, x2, y1);
      x1 = x2;
      y2 = *((*data)++);
      if (y2 >= 0xF0) break;
      drawline(x1, y1, x1, y2);
      y1 = y2;
   }

   (*data)--;
}

/**************************************************************************
** yCorner
**
** Draws an yCorner  (drawing action 0xF4)
**************************************************************************/
void yCorner(byte **data)
{
   byte x1, x2, y1, y2;

   x1 = *((*data)++);
   y1 = *((*data)++);

   pset(x1, y1);

   for (;;) {
      y2 = *((*data)++);
      if (y2 >= 0xF0) break;
      drawline(x1, y1, x1, y2);
      y1 = y2;
      x2 = *((*data)++);
      if (x2 >= 0xF0) break;
      drawline(x1, y1, x2, y1);
      x1 = x2;
   }

   (*data)--;
}

/**************************************************************************
** relativeDraw
**
** Draws short lines relative to last position.  (drawing action 0xF7)
**************************************************************************/
void relativeDraw(byte **data)
{
   byte x1, y1, disp;
   char dx, dy;

   x1 = *((*data)++);
   y1 = *((*data)++);

   pset(x1, y1);

   for (;;) {
      disp = *((*data)++);
      if (disp >= 0xF0) break;
      dx = ((disp & 0xF0) >> 4) & 0x0F;
      dy = (disp & 0x0F);
      if (dx & 0x08) dx = (-1)*(dx & 0x07);
      if (dy & 0x08) dy = (-1)*(dy & 0x07);
      drawline(x1, y1, x1 + dx, y1 + dy);
      x1 += dx;
      y1 += dy;
   }

   (*data)--;
}

/**************************************************************************
** fill
**
** Agi flood fill.  (drawing action 0xF8)
**************************************************************************/
void fill(byte **data)
{
   byte x1, y1;

   for (;;) {
      if ((x1 = *((*data)++)) >= 0xF0) break;
      if ((y1 = *((*data)++)) >= 0xF0) break;
      agiFill(x1, y1);
   }

   (*data)--;
}

/**************************************************************************
** absoluteLine
**
** Draws long lines to actual locations (cf. relative) (drawing action 0xF6)
**************************************************************************/
void absoluteLine(byte **data)
{
   byte x1, y1, x2, y2;

   x1 = *((*data)++);
   y1 = *((*data)++);

   pset(x1, y1);

   for (;;) {
      if ((x2 = *((*data)++)) >= 0xF0) break;
      if ((y2 = *((*data)++)) >= 0xF0) break;
      drawline(x1, y1, x2, y2);
      x1 = x2;
      y1 = y2;
   }

   (*data)--;
}


#define plotPatternPoint() \
   if (patCode & 0x20) { \
      if ((splatterMap[bitPos>>3] >> (7-(bitPos&7))) & 1) pset(x1, y1); \
      bitPos++; \
      if (bitPos == 0xff) bitPos=0; \
   } else pset(x1, y1)

/**************************************************************************
** plotPattern
**
** Draws pixels, circles, squares, or splatter brush patterns depending
** on the pattern code.
**************************************************************************/
void plotPattern(byte x, byte y)
{ 
  static char circles[][15] = { /* agi circle bitmaps */
    {0x80},
    {0xfc},
    {0x5f, 0xf4},
    {0x66, 0xff, 0xf6, 0x60},
    {0x23, 0xbf, 0xff, 0xff, 0xee, 0x20},
    {0x31, 0xe7, 0x9e, 0xff, 0xff, 0xde, 0x79, 0xe3, 0x00},
    {0x38, 0xf9, 0xf3, 0xef, 0xff, 0xff, 0xff, 0xfe, 0xf9, 0xf3, 0xe3, 0x80},
    {0x18, 0x3c, 0x7e, 0x7e, 0x7e, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x7e,
     0x7e, 0x3c, 0x18}
  };

  static byte splatterMap[32] = { /* splatter brush bitmaps */
    0x20, 0x94, 0x02, 0x24, 0x90, 0x82, 0xa4, 0xa2,
    0x82, 0x09, 0x0a, 0x22, 0x12, 0x10, 0x42, 0x14,
    0x91, 0x4a, 0x91, 0x11, 0x08, 0x12, 0x25, 0x10,
    0x22, 0xa8, 0x14, 0x24, 0x00, 0x50, 0x24, 0x04
  };

  static byte splatterStart[128] = { /* starting bit position */
    0x00, 0x18, 0x30, 0xc4, 0xdc, 0x65, 0xeb, 0x48,
    0x60, 0xbd, 0x89, 0x05, 0x0a, 0xf4, 0x7d, 0x7d,
    0x85, 0xb0, 0x8e, 0x95, 0x1f, 0x22, 0x0d, 0xdf,
    0x2a, 0x78, 0xd5, 0x73, 0x1c, 0xb4, 0x40, 0xa1,
    0xb9, 0x3c, 0xca, 0x58, 0x92, 0x34, 0xcc, 0xce,
    0xd7, 0x42, 0x90, 0x0f, 0x8b, 0x7f, 0x32, 0xed,
    0x5c, 0x9d, 0xc8, 0x99, 0xad, 0x4e, 0x56, 0xa6,
    0xf7, 0x68, 0xb7, 0x25, 0x82, 0x37, 0x3a, 0x51,
    0x69, 0x26, 0x38, 0x52, 0x9e, 0x9a, 0x4f, 0xa7,
    0x43, 0x10, 0x80, 0xee, 0x3d, 0x59, 0x35, 0xcf,
    0x79, 0x74, 0xb5, 0xa2, 0xb1, 0x96, 0x23, 0xe0,
    0xbe, 0x05, 0xf5, 0x6e, 0x19, 0xc5, 0x66, 0x49,
    0xf0, 0xd1, 0x54, 0xa9, 0x70, 0x4b, 0xa4, 0xe2,
    0xe6, 0xe5, 0xab, 0xe4, 0xd2, 0xaa, 0x4c, 0xe3,
    0x06, 0x6f, 0xc6, 0x4a, 0xa4, 0x75, 0x97, 0xe1
  };

  int circlePos = 0;
  byte x1, y1, penSize, bitPos = splatterStart[patNum];

  penSize = (patCode&7);

  /* Don't know exactly what happens at the borders, but it can definitely
  ** cause problems if it isn't right. */
  /*
  if (x<((penSize/2)+1)) x=((penSize/2)+1);
  else if (x>160-((penSize/2)+1)) x=160-((penSize/2)+1);
  if (y<penSize) y = penSize;
  else if (y>=168-penSize) y=167-penSize;
  */
  if (x<penSize) x = penSize - 1;
  if (y<penSize) y = penSize;
  //else if (y>=168-penSize) y=167-penSize;

  for (y1=y-penSize; y1<=y+penSize; y1++) {
    for (x1=x-(ceil((float)penSize/2)); x1<=x+(floor((float)penSize/2)); x1++) {
      if (patCode & 0x10) { /* Square */
	      plotPatternPoint();
      }
      else { /* Circle */
	      if ((circles[patCode&7][circlePos>>3] >> (7-(circlePos&7)))&1) {
	         plotPatternPoint();
	      }
	      circlePos++;
      }
    }
  }

} 


/**************************************************************************
** plotBrush
**
** Plots points and various brush patterns.
**************************************************************************/
void plotBrush(byte **data)
{
   byte x1, y1, store;

   for (;;) {
     if (patCode & 0x20) {
	     if ((patNum = *((*data)++)) >= 0xF0) break;
        patNum = ((patNum >> 1) & 0x7f);
     }
     if ((x1 = *((*data)++)) >= 0xF0) break;
     if ((y1 = *((*data)++)) >= 0xF0) break;
     plotPattern(x1, y1);
   }

   (*data)--;
}

/**************************************************************************
** showPriority
**
** Show the current state of the priority screen.
**************************************************************************/
void showPriority()
{
   BITMAP *tempPri = create_bitmap(160, 168);
   int x, y;

   for (y=0; y<168; y++) {
      for (x=0; x<160; x++) {
         tempPri->line[y][x] = priority->line[y][x] + 1;
      }
   }
   stretch_blit(tempPri, agi_screen, 0, 0, PWIDTH, PHEIGHT, 0, 0, VWIDTH, VHEIGHT);
   destroy_bitmap(tempPri);
}

/**************************************************************************
** showPicture
**
** Show the current state of the visual screen.
**************************************************************************/
void showPicture()
{
   show_mouse(NULL);
   stretch_blit(picture, agi_screen, 0, 0, PWIDTH, PHEIGHT, 0, 0, VWIDTH, VHEIGHT);
   show_mouse(screen);
}

/**************************************************************************
** showControl
**
** Show the current state of the control screen.
**************************************************************************/
void showControl()
{
   BITMAP *tempCtrl = create_bitmap(160, 168);
   int x, y;

   for (y=0; y<168; y++) {
      for (x=0; x<160; x++) {
         tempCtrl->line[y][x] = control->line[y][x] + 1;
      }
   }
   stretch_blit(tempCtrl, agi_screen, 0, 0, PWIDTH, PHEIGHT, 0, 0, VWIDTH, VHEIGHT);
   destroy_bitmap(tempCtrl);
}

/**************************************************************************
** splitPriority
**
** Purpose: To split the priority colours from the control colours. It
** determines the priority information for pixels that are overdrawn by
** control lines by the same method used in Sierras interpreter (at this
** stage). This could change later on.
**************************************************************************/
void splitPriority()
{
   int x, y, dy;
   byte data;
   boolean priFound;

   for (x=0; x<160; x++) {
      for (y=0; y<168; y++) {
         data = priority->line[y][x];
         if (data == 3)  {
            priority->line[y][x] = 4;
            control->line[y][x] = data;
         }
         if (data < 3) {
            control->line[y][x] = data;
            dy = y + 1;
            priFound = FALSE;
            while (!priFound && (dy < 168)) {
               data = priority->line[dy][x];
               /* The following if statement is a hack to fix a problem
               ** in KQ1 room 1.
               */
               if (data == 3) {
                  priFound = TRUE;
                  priority->line[y][x] = 4;
               }
               else if (data > 3) {
                  priFound = TRUE;
                  priority->line[y][x] = data;
               }
               else
                  dy++;
            }
         }
      }
   }
}

int picFNum=0;

/**************************************************************************
** drawPic
**
** Purpose: To draw an AGI picture from the given PICTURE resource data.
**
**  pLen = length of PICTURE data
**************************************************************************/
void drawPic(byte *data, int pLen, boolean okToClearScreen)
{
   byte action;
   boolean stillDrawing = TRUE;

   if (okToClearScreen) clearPicture();
   patCode = 0x00;

   do {
      action = *(data++);

      switch (action) {
  	      case 0xFF: stillDrawing = 0; break;
	      case 0xF0: picColour = *(data++);
		      picDrawEnabled = TRUE;
		      break;
	      case 0xF1: picDrawEnabled = FALSE; break;
	      case 0xF2: priColour = *(data++);
		      priDrawEnabled = TRUE;
		      break;
	      case 0xF3: priDrawEnabled = FALSE; break;
	      case 0xF4: yCorner(&data); break;
	      case 0xF5: xCorner(&data); break;
	      case 0xF6: absoluteLine(&data); break;
	      case 0xF7: relativeDraw(&data); break;
	      case 0xF8: fill(&data); break;
	      case 0xF9: patCode = *(data++); break;
	      case 0xFA: plotBrush(&data); break;
	      default: printf("Unknown picture code : %X\n", action); exit(0);
      }

      //if (picFNum == 3) {
      //   showPicture();
      //   if ((readkey() >> 8) == KEY_ESC) closedown();
      //}

   } while((data < (data + pLen)) && stillDrawing);

   splitPriority();
}

/***************************************************************************
** showPic
**
** Purpose: To allow a way of viewing AGI pictures and switching between
** the two screens.
***************************************************************************/
void showPic()
{
   byte ch='v';

   do {
      switch (tolower(ch)) {
	     case 'v': showPicture(); break;
	     case 'p': showPriority(); break;
        case 'c': showControl(); break;
      }
   } while ((ch = (readkey() & 0xff)) != 0x1B);
}

void showDebugPri()
{
   byte ch='p';

   show_mouse(NULL);
   do {
      switch (tolower(ch)) {
	     case 'p': showPriority(); break;
        case 'c': showControl(); break;
      }
   } while ((ch = (readkey() & 0xff)) != 0x1B);
   showPicture();
   show_mouse(screen);

   okToShowPic = TRUE;
}

void initPictures()
{
   int i;

   for (i=0; i<256; i++) {
      loadedPictures[i].loaded = FALSE;
   }
}

/***************************************************************************
** loadPictureFile
**
** Purpose: To load the picture resource with the given number.
***************************************************************************/
void loadPictureFile(int picFileNum)
{
   AGIFile tempAGI;

   loadAGIFile(PICTURE, picdir[picFileNum], &tempAGI);
   loadedPictures[picFileNum].size = tempAGI.size;
   loadedPictures[picFileNum].data = (byte *)malloc(tempAGI.size);
   memcpy(loadedPictures[picFileNum].data, tempAGI.data, tempAGI.size);
   loadedPictures[picFileNum].loaded = TRUE;
   free(tempAGI.data);   /* Deallocate original buffer. */
}

void discardPictureFile(int picFileNum)
{
   if (loadedPictures[picFileNum].loaded) {
      loadedPictures[picFileNum].loaded = FALSE;
      free(loadedPictures[picFileNum].data);
   }
}

/***************************************************************************
** saveSnapShot
**
** Purpose: To take a snapshot of the current screen contents.
***************************************************************************/
void saveSnapShot()
{
   char fileName[80];
   BITMAP *bmp;
   PALETTE pal;
   static int numOfSnaps=0;

   /* Previous snapshots are only recognised if they were taken during the
   ** current session, otherwise they are overwritten. */
   sprintf(fileName, "SNAP%d.PCX", numOfSnaps);
   numOfSnaps++;
   getRealPal(pal);
   bmp = create_sub_bitmap(screen, 0, 0, 640, 480);
   save_bitmap(fileName, bmp, pal);
   destroy_bitmap(bmp);
}

