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
#include<math.h>
#include<time.h>
#include"light.h"

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
float brightness = 0.0;
playerstr * player;
enum keyindex {W,A,S,D};
int keycodes[4][2];

//CHANGE THESE TO REBIND KEYS UP,LEFT,DOWN,RIGHT
const XKEYSUMS[4] = {XK_w,XK_a,XK_s,XK_d};

#define X 0
#define Y 1
#define SDIM 0.05
#define BDIM 0.1
#define PSPEED 0.03
#define XPOS myblock->pos[X]+myblock->dim[X]
#define XNEG myblock->pos[X]-myblock->dim[X]
#define YNEG myblock->pos[Y]-myblock->dim[Y]
#define YPOS myblock->pos[Y]+myblock->dim[Y]
#define ABS(Z) (Z > 0 ? Z : -Z)
#define ROUND(N)(N >= 0.5)
#define RANDF10 ((float)rand())/RAND_MAX
#define CALCPOS(INDEX) (current->pos[INDEX] =-(1-currentsize)+(2-currentsize*2)*RANDF10)
#define DISTCALC(INDEX)(ABS((player->pos[INDEX] - current->pos[INDEX])) < current->dim[INDEX]+SDIM)

#define SETKEYCODES(INDEX) {						\
		keycodes[INDEX][0] = XKeysymToKeycode(dpy, XKEYSUMS[INDEX])/8; \
		keycodes[INDEX][1] = 0x1 << (XKeysymToKeycode(dpy, XKEYSUMS[INDEX])%8); \
	}
#define CHECKKEY(INDEX) ((keys[keycodes[INDEX][0]]& keycodes[INDEX][1]) > 0)


void render_block(block * myblock);
void drawPlayer(playerstr * myblock);
block * create_block(playerstr * player);
void draw_point(block * myblock);
void update_point(void);
void reset_game(playerstr * player);
void detect_hit(playerstr * player);
void draw_frame();
float dist(float x1, float y1);


//Render start---------------------------
void render_point(block * myblock)
{
	glColor4f(0., 0., 1.,1); 
	glNormal3f(myblock->pos[X],myblock->pos[Y],1);
	glVertex2f(XNEG,YNEG);
	glVertex2f(XPOS,YNEG);
	glVertex2f(XPOS,YPOS);
	glVertex2f(XNEG,YPOS);
}

void render_block(block * myblock)
{
	glNormal3f(myblock->pos[X],myblock->pos[Y],0.8);
	glVertex2f(XNEG,YNEG);
	glVertex2f(XPOS,YNEG);
	glVertex2f(XPOS,YPOS);
	glVertex2f(XNEG,YPOS);     
}

#define POINTXY	glVertex2f(point->pos[X],point->pos[Y])
void render_lines(block * argmyblock)
{
	glColor4f(0., 1., 0.,0.6);
	block * myblock = argmyblock;
	glLineWidth(20); 
//	glBegin(GL_LINES); 
	float dist[4];
	int i = 0;
	while(myblock != NULL)
	{
		i = 0;		
		//distance[i++] = dist(XNEG,YNEG);
		//distance[i++] = dist(XPOS,YNEG);
		//distance[i++] = dist(XPOS,YPOS);
		//distance[i++] = dist(XNEG,YPOS);     
		glVertex2f(XNEG,YNEG);
		POINTXY;
		POINTXY;
		glVertex2f(XPOS,YPOS);
		glVertex2f(XPOS,YNEG);
		POINTXY;
		glVertex2f(XNEG,YPOS);     
		POINTXY;
		myblock = myblock->next;
	}
	//glEnd();
}

float dist(float x1, float y1)
{
	float x2 = point->pos[X];
	float y2 = point->pos[Y];
	float xtmp = ABS((x1-x2));
	xtmp *= xtmp;
	float ytmp = ABS((y1-y2));
	ytmp *= ytmp;
	return (sqrt((ytmp+xtmp)));
}


void render_player()
{
	static int count;
	if(player->dirx != 0 || player->diry != 0 || count == 0)
	{
	glColor4f(0., 0., 1.,1); 
	glNormal3f(player->pos[X],player->pos[Y],0.8);
	glVertex2f(player->pos[X]+SDIM,player->pos[Y]+SDIM);
	glVertex2f(player->pos[X]+SDIM,player->pos[Y]-SDIM);
	glVertex2f(player->pos[X]-SDIM,player->pos[Y]-SDIM);
	glVertex2f(player->pos[X]-SDIM,player->pos[Y]+SDIM);
	count++;
	}
	else
	{
		glPopMatrix();
		count = 0;
	}
	glPushMatrix();
	
}

void render_scene() {
	block * current = player->next;
	glClearColor(0.1, 0.1, 0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1., 1., -1., 1., -1., 10.);       
	gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);
	glBegin(GL_QUADS);
	render_player();
	glLoadIdentity();
	render_point(point);
	render_lines(player->next);
	glColor4f(1., 0., 0.,0.5); 
	while(current != NULL)
	{
		render_block(current);
		current = current->next;
	}

	glEnd();   
} 

//	Render end---------------------------
//Update start---------------------------
void update_point(void)
{
	int tmp;
	if(point != NULL) {
		tmp = point->direction;
		free(point);
	}
	point = create_block(NULL);
	point->dim[X] = point->dim[Y] = SDIM;
	point->direction = tmp;
	GLfloat light_position[] = {point->pos[X],point->pos[Y],1,0.8};
	glNormal3f(point->pos[X],point->pos[Y],10);
	glLightfv(GL_LIGHT0, GL_POSITION,light_position);
	printf("B %f\n",brightness);
}

void update_block(block * myblock)
{
	if(ABS(myblock->pos[myblock->isVertical])+myblock->dim[myblock->isVertical] > 1)
		myblock->direction = -(myblock->direction);
	myblock->pos[myblock->isVertical] += 0.01*myblock->direction;     
}

void update_player()
{
	player->pos[X] += PSPEED*player->dirx;
	player->pos[Y] += PSPEED*player->diry;
	player->pos[X] -= PSPEED*(ABS(player->pos[X])+SDIM > 1)*player->dirx;
	player->pos[Y] -= PSPEED*(ABS(player->pos[Y])+SDIM > 1)*player->diry;
}



void update_scene()
{
	block * current = player->next;
	
	update_player();
	while(current != NULL)
	{
	  update_block(current);
	  current = current->next;
	}
	detect_hit(player);
}

void update_title(void)
{
	sprintf(title,"Eskiv, Score %d Highscore %d",score,topscore);
	XStoreName(dpy, win, title);
}

//Update end---------------------------

void add_point(playerstr * player)
{
	update_point();
	score += 5;
	update_title();
	player->next = create_block(player);
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

block * create_block(playerstr * player)
{
	block * current = malloc(sizeof(block));
	float currentsize;
	current->isVertical = ROUND(RANDF10);
	current->dim[current->isVertical] = BDIM;
	current->dim[current->isVertical ^ 1] = SDIM;
	currentsize = current->dim[X];
	CALCPOS(X);
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


__inline__ void reset_player(playerstr * player)
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
	update_point();
	update_title();
	//player->next = create_block(NULL);
}

void draw_frame()
{
	glColor3f(0., 1., 0.); 
	glNormal3f(-point->pos[X],-point->pos[Y],0);
	glVertex2f(1.,1.);
	glVertex2f(1.,-1);
	glVertex2f(-1,-1);
	glVertex2f(-1,1);

	return;
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
	player = calloc(1,sizeof(playerstr));//{{.0,.0},.0,NULL,0,0};
	
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
     //glLightfv(GL_LIGHT0, GL_POSITION, light_position);
     
     // glEnable(GL_LIGHTING);
     //glEnable(GL_LIGHT0);
     //  glEnable(GL_DEPTH_TEST);
     //glEnable(GL_COLOR_MATERIAL);
     update_title();
     init_keycodes();
     update_point();
     reset_player(player);
     point->direction = 1;
     glEnable (GL_BLEND);
     glBlendFunc (GL_SRC_ALPHA, GL_ONE);
     glShadeModel (GL_FLAT);
     
     glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

     int fps = 0;
     clock_t start, end;
     double elapsed;
     start = clock();
     while(1) {

	     XGetWindowAttributes(dpy, win, &gwa);
	     if(gwa.width < gwa.height)
		     glViewport(0, 0, gwa.width,gwa.width);
	     else
		     glViewport(0, 0, gwa.height,gwa.height);
	     render_scene(); 
	     glXSwapBuffers(dpy, win);
	     update_scene();
	     handle_keys(player);
	     end = clock();
	     elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
	     fps++;
	     if(elapsed > 0.01)
	     {
		     printf("FPS %d\n",fps);
		     fps = 0;
		     start = clock();
	     }
	} /* this closes while(1) { */
} /* this is the } which closes int main(int argc, char *argv[]) { */
