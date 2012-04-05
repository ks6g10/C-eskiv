
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
	unsigned int isHoriz:1;
	int direction:2;
}typedef block;

#define X 0
#define Y 1
#define XDIM 0.1
#define YDIM 0.2
const float DIM[2] ={YDIM,XDIM};
void drawBlock(block * myblock);
void DrawBlocks(block * myblock) {
	static float posx;
	static float posy;
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
	while(current != NULL)
	{
		drawBlock(current);
		current = current->next;
	}
} 
#define XPOS myblock->pos[X]+DIM[myblock->isHoriz]
#define XNEG myblock->pos[X]-DIM[myblock->isHoriz]
#define YNEG myblock->pos[Y]-DIM[myblock->isHoriz ^ 1]
#define YPOS myblock->pos[Y]+DIM[myblock->isHoriz ^ 1]
#define ABS(Z) (Z > 0 ? Z : -Z)

void drawBlock(block * myblock)
{
	glBegin(GL_QUADS);
	glColor3f(1., 0., 0.); 
	
	glVertex2f(XNEG,YNEG);
	glVertex2f(XPOS,YNEG);
	glVertex2f(XPOS,YPOS);
	glVertex2f(XNEG,YPOS);

	if(ABS(myblock->pos[myblock->isHoriz]+DIM[myblock->isHoriz]) > 1)
		{
			myblock->direction =-(myblock->direction);
		printf("direction %d\n", myblock->direction);
}
	myblock->pos[myblock->isHoriz] += 0.01*myblock->direction;
	glEnd(); 
}
 
int main(int argc, char *argv[]) {
	static block myblock2 = {{.1,.0},NULL,1,1};
	static block myblock = {{.1,.0},&myblock2,0,1};
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
//		XNextEvent(dpy, &xev);
		
//		if(xev.type == Expose) {
		XGetWindowAttributes(dpy, win, &gwa);
		glViewport(0, 0, gwa.width, gwa.height);
		DrawBlocks(&myblock); 
		glXSwapBuffers(dpy, win);
			//}
                
		/* else if(xev.type == KeyPress) { */
		/* 	glXMakeCurrent(dpy, None, NULL); */
		/* 	glXDestroyContext(dpy, glc); */
		/* 	XDestroyWindow(dpy, win); */
		/* 	XCloseDisplay(dpy); */
		/* 	exit(0); } */
	} /* this closes while(1) { */
} /* this is the } which closes int main(int argc, char *argv[]) { */









