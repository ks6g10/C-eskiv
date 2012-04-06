/*
 * This is a simple eskiv clone made in C using opengl.
 * Copyright (C) 2012  Kim Svensson ks@linux.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

Display                 *dpy;
Window                  root;
GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo             *vi;
Colormap                cmap;
XSetWindowAttributes    swa;
Window                  win;
GLXContext              glc;
XWindowAttributes       gwa;
XEvent                  xev;

struct _block {
     float pos[2];
     struct _block * next;
     unsigned int isVertical:1;
     int direction:2;
}typedef block;

#define X 0
#define Y 1
#define XDIM 0.1
#define YDIM 0.2
const float DIM[2] ={YDIM,XDIM};
void drawBlock(block * myblock);
void drawPlayer(block * myblock);
void DrawBlocks(block * myblock) {
     block * current = myblock;
     glClearColor(1.0, 1.0, 1.0, 1.0);
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     glOrtho(-1., 1., -1., 1., 1., 20.);

     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);
//	printf("x %f y %f direction %d\n",myblock->pos[X],myblock->pos[Y],myblock->direction);
     drawPlayer(current);
     current = current->next;
     while(current != NULL)
     {
	  drawBlock(current);
	  current = current->next;
     }
} 
#define XPOS myblock->pos[X]+DIM[myblock->isVertical]
#define XNEG myblock->pos[X]-DIM[myblock->isVertical]
#define YNEG myblock->pos[Y]-DIM[myblock->isVertical ^ 1]
#define YPOS myblock->pos[Y]+DIM[myblock->isVertical ^ 1]
#define ABS(Z) (Z > 0 ? Z : -Z)

void drawBlock(block * myblock)
{
     glBegin(GL_QUADS);
     glColor3f(1., 0., 0.); 
     glVertex2f(XNEG,YNEG);
     glVertex2f(XPOS,YNEG);
     glVertex2f(XPOS,YPOS);
     glVertex2f(XNEG,YPOS);
     
     if(ABS(myblock->pos[myblock->isVertical]+DIM[myblock->isVertical]) > 1)
	  myblock->direction = -(myblock->direction);
     myblock->pos[myblock->isVertical] += 0.01*myblock->direction;
     glEnd();
     
}

void drawPlayer(block * player)
{
     glBegin(GL_QUADS);
     glColor3f(1., 0., 0.); 
     glVertex2f(player->pos[X]+DIM[Y],player->pos[Y]+DIM[Y]);
     glVertex2f(player->pos[X]+DIM[Y],player->pos[Y]-DIM[Y]);
     glVertex2f(player->pos[X]-DIM[Y],player->pos[Y]-DIM[Y]);
     glVertex2f(player->pos[X]-DIM[Y],player->pos[Y]+DIM[Y]);
     glEnd();
}
 
int main(int argc, char *argv[]) {
     static block myblock2 = {{.3,.0},NULL,1,1};
     static block myblock = {{.1,.0},&myblock2,0,1};
     static block player = {{.0,.0},&myblock,1,1};
     dpy = XOpenDisplay(NULL);
 
     if(dpy == NULL) {
	  printf("\n\tcannot connect to X server\n\n");
	  exit(0); }
        
     root = DefaultRootWindow(dpy);
	
     vi = glXChooseVisual(dpy, 0, att);
	
     if(vi == NULL) {
	  printf("\n\tno appropriate visual found\n\n");
	  exit(0); } 
     else {
	  printf("\n\tvisual %p selected\n", (void *)vi->visualid); 
     }/* %p creates hexadecimal output like in glxinfo */


     cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

     swa.colormap = cmap;
     swa.event_mask = ExposureMask | KeyPressMask;
 
     win = XCreateWindow(dpy, root, 0, 0, 600, 600, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

     XMapWindow(dpy, win);
     XStoreName(dpy, win, "VERY SIMPLE APPLICATION");
 
     glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
     glXMakeCurrent(dpy, win, glc);
 
     glEnable(GL_DEPTH_TEST); 
 
     while(1) {

	  XGetWindowAttributes(dpy, win, &gwa);
	  glViewport(0, 0, gwa.width, gwa.height);
	  DrawBlocks(&player); 
	  glXSwapBuffers(dpy, win);

	  // w 25 a 38 s 39 d 40
	  if(XPending(dpy)) {/*If there is an event pending*/
	       XNextEvent(dpy, &xev);
	       if(xev.type == KeyPress || xev.type == KeyRelease) { 
		    XKeyEvent xkey = xev.xkey;
		    switch(xkey.keycode)
		    {
		    case 111:
		    case 25: /*W*/
			 player.pos[Y] += 0.1;
			 break;

		    case 113:
		    case 38: /*A*/
			 player.pos[X] -= 0.1;
			 break;
			 
		    case 116:
		    case 39: /*S*/
			 player.pos[Y] -= 0.1;
			 break;
			 
		    case 114:
		    case 40: /*D*/
			 player.pos[X] += 0.1;
			 break;
		    default:
			 break;
		    }
		    printf("keycode %u\n",xkey.keycode);
	       }
	    
	  }

	} /* this closes while(1) { */
} /* this is the } which closes int main(int argc, char *argv[]) { */









