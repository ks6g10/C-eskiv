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


struct _block {
	float pos[2];
	float dim[2];
	struct _block * next;
	unsigned int isVertical:1;
	int direction:2;
}typedef block;

struct _playerstr {
	float pos[2];
	float size;
	struct _block * next;
	int dirx:2;
	int diry:2;
}typedef playerstr;


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
block * point;
char * title;
int topscore =0;
int score = 0;
#define X 0
#define Y 1
#define SDIM 0.05
#define BDIM 0.1
#define XPOS myblock->pos[X]+myblock->dim[X]
#define XNEG myblock->pos[X]-myblock->dim[X]
#define YNEG myblock->pos[Y]-myblock->dim[Y]
#define YPOS myblock->pos[Y]+myblock->dim[Y]
#define ABS(Z) (Z > 0 ? Z : -Z)
#define ROUND(N)(N >= 0.5)
#define RANDF10 ((float)rand())/RAND_MAX
#define CALCPOS(INDEX) (current->pos[INDEX] =-(1-currentsize)+(2-currentsize*2)*RANDF10)
#define PSPEED 0.05
#define DISTCALC(INDEX)(ABS((player->pos[INDEX] - current->pos[INDEX])) < current->dim[INDEX]+SDIM)
#define SETKEYCODES(INDEX) {					\
		keycodes[INDEX][0] = XKeysymToKeycode(dpy, XKEYSUMS[INDEX])/8; \
		keycodes[INDEX][1] = 0x1 << (XKeysymToKeycode(dpy, XKEYSUMS[INDEX])%8); \
}
#define CHECKKEY(INDEX) ((keys[keycodes[INDEX][0]]& keycodes[INDEX][1]) > 0)


void drawBlock(block * myblock);
void drawPlayer(playerstr * myblock);
block * create_block(playerstr * player);
void draw_point(block * myblock);
void reset_point(void);
void reset_game(playerstr * player);
void detect_hit(playerstr * player);

void DrawBlocks(playerstr * player) {
     block * current = player->next;
     glClearColor(0.0, 0.0, 0.0, 1.0);
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     glOrtho(-1., 1., -1., 1., 1., 20.);


     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);
     glBegin(GL_QUADS);
     drawPlayer(player);
     glDisable(GL_LIGHTING);
     draw_point(point);
     //    glColor4f(1., 0., 0.,0.5); 
     while(current != NULL)
     {
	  drawBlock(current);
	  current = current->next;
     }
     glEnd();
     detect_hit(player);
} 

void update_title(void)
{
	sprintf(title,"Eskiv, Score %d Highscore %d",score,topscore);
	XStoreName(dpy, win, title);

}

void draw_point(block * myblock)
{
//	glColor4f(0., 0., 1.,0.1); 
	glVertex2f(XNEG,YNEG);
	glVertex2f(XPOS,YNEG);
	glVertex2f(XPOS,YPOS);
	glVertex2f(XNEG,YPOS);
}

void drawBlock(block * myblock)
{
    
	glNormal3f(myblock->pos[X],myblock->pos[Y],0.8);
	glVertex2f(XNEG,YNEG);
	glVertex2f(XPOS,YNEG);
	glVertex2f(XPOS,YPOS);
	glVertex2f(XNEG,YPOS);
     
	if(ABS(myblock->pos[myblock->isVertical])+myblock->dim[myblock->isVertical] > 1)
		myblock->direction = -(myblock->direction);
	myblock->pos[myblock->isVertical] += 0.01*myblock->direction;     
}

void add_point(playerstr * player)
{
	reset_point();
	score += 5;
	update_title();
	player->next = create_block(player);//*/printf("HIT %d\n",i++);
}


void detect_hit(playerstr * player)
{
	block * current = point;
	static int i;
	if(DISTCALC(X) && DISTCALC(Y))
		add_point(player);
	current = player->next;
	while(current != NULL)
	{
		if(DISTCALC(X) && DISTCALC(Y))
		{
			reset_game(player);
			return;
		}
		current = current->next;
	}
}
<<<<<<< HEAD
#define PSPEED 0.05
=======

>>>>>>> d0cd08ebe158ae5c187535e856a8e186fdff8b86
void drawPlayer(playerstr * player)
{
	player->pos[X] += PSPEED*player->dirx;
	player->pos[Y] += PSPEED*player->diry;
	player->pos[X] -= PSPEED*(ABS(player->pos[X])+SDIM > 1)*player->dirx;
	player->pos[Y] -= PSPEED*(ABS(player->pos[Y])+SDIM > 1)*player->diry;
//	glBegin(GL_QUADS);
//	glColor3f(0., 1., 0.); 
	glNormal3f(player->pos[X],player->pos[Y],0.8);
	glVertex2f(player->pos[X]+SDIM,player->pos[Y]+SDIM);
	glVertex2f(player->pos[X]+SDIM,player->pos[Y]-SDIM);
	glVertex2f(player->pos[X]-SDIM,player->pos[Y]-SDIM);
	glVertex2f(player->pos[X]-SDIM,player->pos[Y]+SDIM);
//	glEnd();
	
}

block * create_block(playerstr * player)
{
	block * current = malloc(sizeof(block));
	float currentsize;
	current->isVertical = ROUND(RANDF10);
	current->dim[current->isVertical] = BDIM;
	current->dim[current->isVertical ^ 1] = SDIM;
	currentsize = current->dim[X];
	CALCPOS(X);
//	printf("%f\n",current->pos[X]);
	currentsize = current->dim[Y];
	CALCPOS(Y);

	if(player == NULL)
		return current;

	currentsize = current->dim[current->isVertical];
	while(DISTCALC(current->isVertical))
	{
		CALCPOS(current->isVertical);
	}
	current->direction = -1+2*ROUND(RANDF10);
	
	current->next = player->next;
     	return current;
}


enum keyindex {W,A,S,D};
int keycodes[4][2];

//CHANGE THESE TO REBIND KEYS UP,LEFT,DOWN,RIGHT
const XKEYSUMS[4] = {XK_w,XK_a,XK_s,XK_d};
void handle_keys(playerstr * player)
{

	char keys[32];
	XQueryKeymap(dpy, keys);
	unsigned int wKey = CHECKKEY(W);
	unsigned int aKey = CHECKKEY(A);
	unsigned int sKey = CHECKKEY(S);
	unsigned int dKey = CHECKKEY(D);
	player->diry = (wKey - sKey);
	player->dirx = (dKey - aKey);
}


void init_keycodes(void)
{
	SETKEYCODES(W);
	SETKEYCODES(A);
	SETKEYCODES(S);
	SETKEYCODES(D);
}



void reset_point(void)
{
	if(point != NULL)
		free(point);
	point = create_block(NULL);
	GLfloat light_position[] = {point->pos[X],point->pos[Y],1,1};
	glNormal3f(point->pos[X],point->pos[Y],10);
	glLightfv(GL_LIGHT0, GL_POSITION,light_position);
	point->dim[X] = point->dim[Y] = SDIM;
}


void reset_player(playerstr * player)
{
	player->pos[X] = player->pos[Y] = 0;
}

void free_blocks(playerstr * player)
{
	block * current = player->next;
	block * tmp;
	
	while(current != NULL)
	{
		tmp = current;
		current = current->next;
		if(tmp != NULL)
			free(tmp);
	}
	player->next = NULL;
}

void reset_game(playerstr * player)
{
	if(score > topscore)
	{
		topscore = score;
		printf("Your new highscore is %d\n",score);
	}
	else
	{
		printf("Your Score is %d, highscore is %d\n",score,topscore);
	}
	score = 0;
	reset_player(player);
	free_blocks(player);
	reset_point();
	update_title();
	//player->next = create_block(NULL);
}

void draw_frame()
{
	glColor3f(0., 0., 0.); 
	glLineWidth(20); 
	glBegin(GL_LINES); 
//Bottom border
	glVertex2f(-1., -1.); 
	glVertex2f(1.,-1.); 
//Top border
	glVertex2f(-1., 1.); 
	glVertex2f(1.,1.); 
//Rigth border
	glVertex2f(-1., -1.); 
	glVertex2f(-1.,1.); 
//Left border
	glVertex2f(1., -1.); 
	glVertex2f(1.,1.); 
	glEnd();
}

 
int main(int argc, char *argv[]) {
//	static block myblock2 = {{.3,.0},{SDIM,BDIM},NULL,1,1};
//	static block myblock = {{.1,.0},{BDIM,SDIM},&myblock2,0,1};
     static playerstr player = {{.0,.0},.0,NULL,0,0};
     srand(time(NULL));
     title = malloc(sizeof(char)*200);

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
     glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
     glXMakeCurrent(dpy, win, glc);
 
     GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
     GLfloat mat_shininess[] = { 50.0 };
     GLfloat light_position[] = { 1.0, 1.0, 1.0, 1.0 };
     glClearColor (0.0, 0.0, 0.0, 0.0);
     glShadeModel (GL_SMOOTH);

     glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
     glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
     glLightfv(GL_LIGHT0, GL_POSITION, light_position);
     
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glEnable(GL_DEPTH_TEST);

     update_title();
     init_keycodes();
     reset_point();
     while(1) {

	     XGetWindowAttributes(dpy, win, &gwa);
	     if(gwa.width < gwa.height)
		     glViewport(0, 0, gwa.width,gwa.width);
	     else
		     glViewport(0, 0, gwa.height,gwa.height);
	     DrawBlocks(&player); 
	     // draw_frame();
	     glXSwapBuffers(dpy, win);
	     handle_keys(&player);
	} /* this closes while(1) { */
} /* this is the } which closes int main(int argc, char *argv[]) { */









