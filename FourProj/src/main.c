#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <tchar.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <winerror.h>

#include "miniglut.h"

#define LUA_IMPL
#include "minilua.h"


// include nuklear.h before the sokol_nuklear.h implementation
// #define NK_INCLUDE_FIXED_TYPES
// #define NK_INCLUDE_STANDARD_IO
// #define NK_INCLUDE_DEFAULT_ALLOCATOR
// #define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
// #define NK_INCLUDE_FONT_BAKING
// #define NK_INCLUDE_DEFAULT_FONT
// #define NK_INCLUDE_STANDARD_VARARGS
// #define NK_IMPLEMENTATION
// #define NK_GDI_IMPLEMENTATION
// #include "nuklear.h"
// #include "nuklear_gdi.h"
// #include <time.h>
// #include <limits.h>
// #define WINDOW_WIDTH 800
// #define WINDOW_HEIGHT 600

// #ifdef INCLUDE_ALL
//   #define INCLUDE_STYLE
//   #define INCLUDE_CALCULATOR
//   #define INCLUDE_CANVAS
//   #define INCLUDE_OVERVIEW
//   #define INCLUDE_NODE_EDITOR
// #endif

// #ifdef INCLUDE_STYLE
//   #include "nk_style.c"
// #endif
// #ifdef INCLUDE_CALCULATOR
//   #include "nk_calculator.c"
// #endif
// #ifdef INCLUDE_CANVAS
//   #include "nk_canvas.c"
// #endif
// #ifdef INCLUDE_OVERVIEW
//   #include "nk_overview.c"
// #endif
// #ifdef INCLUDE_NODE_EDITOR
//   #include "nk_node_editor.c"
// #endif

// /* ===============================================================
//  *
//  *                          DEMO
//  *
//  * ===============================================================*/
// static LRESULT CALLBACK
// WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
// {
//     switch (msg)
//     {
//     case WM_DESTROY:
//         PostQuitMessage(0);
//         return 0;
//     }
//     if (nk_gdi_handle_event(wnd, msg, wparam, lparam))
//         return 0;
//     return DefWindowProcW(wnd, msg, wparam, lparam);
// }

#include "t_test.h"

int t_list();
int t_vector();
int t_tree();
int t_map();
int t_set();
int t_stack();
int t_queue();
int t_algo();

#define MINICORO_IMPL
#include "minicoro.h"

#if defined(__i386__)
static inline uint64_t rdtsc() {
  uint64_t x;
  __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
  return x;
}
#elif defined(__x86_64__)
static inline uint64_t rdtsc() {
  uint32_t hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (uint64_t)lo)|( ((uint64_t)hi)<<32 );
}
#elif defined(__aarch64__)
static inline uint64_t rdtsc() {
  uint64_t val;
  __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(val));
  return val;
}
#endif

#define SWITCH_ITERATIONS 10000000
#define INIT_ITERATIONS   100000

void coro_entry(mco_coro* co) {
  for(int i=0;i<SWITCH_ITERATIONS;++i) {
    mco_yield(co);
  }
}

void bench_switch() {
  mco_desc desc = mco_desc_init(coro_entry, 0);
  mco_coro* co;
  mco_create(&co, &desc);
  uint64_t start = rdtsc();
  for(int i=0;i<SWITCH_ITERATIONS;++i) {
    mco_resume(co);
  }
  uint64_t elapsed = rdtsc() - start;
  mco_destroy(co);
  printf("switch cycles %.1f\n", elapsed / (double)(SWITCH_ITERATIONS*2));
}

void bench_init() {
  mco_desc desc = mco_desc_init(coro_entry, 0);
  mco_coro* co = (mco_coro*)malloc(desc.coro_size);
  uint64_t init_cycles = 0;
  uint64_t uninit_cycles = 0;
  uint64_t start;
  for(int i=0;i<INIT_ITERATIONS;++i) {
    start = rdtsc();
    mco_init(co, &desc);
    init_cycles += rdtsc() - start;
    start = rdtsc();
    mco_uninit(co);
    uninit_cycles += rdtsc() - start;
  }
  free(co);
  printf("init cycles %.1f\n", init_cycles / (double)(INIT_ITERATIONS));
  printf("uninit cycles %.1f\n", uninit_cycles / (double)(INIT_ITERATIONS));
}

void bench_create() {
  mco_desc desc = mco_desc_init(coro_entry, 0);
  uint64_t create_cycles = 0;
  uint64_t destroy_cycles = 0;
  uint64_t start;
  for(int i=0;i<INIT_ITERATIONS;++i) {
    start = rdtsc();
    mco_coro* co = NULL;
    mco_create(&co, &desc);
    create_cycles += rdtsc() - start;
    start = rdtsc();
    mco_destroy(co);
    destroy_cycles += rdtsc() - start;
  }
  printf("create cycles %.1f\n", create_cycles / (double)(INIT_ITERATIONS));
  printf("destroy cycles %.1f\n", destroy_cycles / (double)(INIT_ITERATIONS));
}

static void fail(const char* message, mco_result res) {
  printf("%s: %s\n", message, mco_result_description(res));
  exit(-1);
}

static void fibonacci_coro(mco_coro* co) {
  unsigned long m = 1;
  unsigned long n = 1;
  /* Retrieve max value. */
  unsigned long max;
  mco_result res = mco_pop(co, &max, sizeof(max));
  if(res != MCO_SUCCESS)
    fail("Failed to retrieve coroutine storage", res);

  while(1) {
    /* Yield the next Fibonacci number. */
    mco_push(co, &m, sizeof(m));
    res = mco_yield(co);
    if(res != MCO_SUCCESS)
      fail("Failed to yield coroutine", res);

    unsigned long tmp = m + n;
    m = n;
    n = tmp;
    if(m >= max)
      break;
  }

  /* Yield the last Fibonacci number. */
  mco_push(co, &m, sizeof(m));
}


// #include "at_bp.h"
// #include "at_defs.h"
// #include "at_fifo.h"
// #include "at_lifo.h"
// #include "at_nargs.h"
// #include "at_slist.h"
// #include "at_bp_stmt.h"
#include "atask.h"
typedef struct 
{
    /* resource */
    short res;
    /* consumer coroutine breakpoint */
    unsigned char consumer_bp;
    /* produce coroutine breakpoint */
    unsigned char producer_bp;
} res_t;

#define TRUE    1
#define FALSE   (!TRUE)

int consumer(res_t *res)
{
    /* bpd coroutine default breakpoint pointer */
    unsigned char *bpd = &res->consumer_bp;
    /* coroutine begin */
    bpd_begin(2);
    while (1)
    {
        /* wait number of resource > 0 */
        while (res->res <= 0)
        {
            bpd_yield(1) FALSE;
        }
        /* consume */
        res->res--;
        printf("consume a resource, number of res:%d\n", res->res);
        /* wait next consume */
        bpd_yield(2) TRUE;
    }
    /* coroutine end */
    bpd_end();
}

int producer(res_t *res)
{
    /* bpd coroutine default breakpoint pointer */
    unsigned char *bpd = &res->producer_bp;
    /* coroutine begin */
    bpd_begin(2);
    while (1)
    {
        /* wait number of resource < 30 */
        while (res->res >= 30)
        {
            bpd_yield(1) FALSE;
        }
        /* produce */
        res->res++;
        printf("produce a resource, number of res:%d\n", res->res);
        /* wait next produce */
        bpd_yield(2) TRUE;
    }
    /* coroutine end */
    bpd_end();
}

static char *expr1 = "1+2";

char s1[] = {"duplicated_key"};
char s2[] = {"duplicated_key"};
char s3[] = {"duplicated_key"};


#define C89THREAD_IMPLEMENTATION
#include "c89thread.h"

#define NUM_THREADS 4
#define NUM_TASKS 100
#define NUM_ITERATIONS 500
#define EXPECTED_RESULT 2396583362

static c89mtx_t mutex;
static c89thrd_t threads[NUM_THREADS];
static mco_coro* tasks[NUM_TASKS];

static void fail_mco_t(const char* message, mco_result res) {
  printf("%s: %s\n", message, mco_result_description(res));
  exit(-1);
}

static void fail_t(const char* message) {
  printf("%s\n", message);
  exit(-1);
}

static void fibonacci_task_t(mco_coro* co) {
  unsigned int m = 1;
  unsigned int n = 1;
  for(unsigned int i=0;i<NUM_ITERATIONS;++i) {
    /* Yield the next Fibonacci number. */
    mco_result res = mco_yield(co);
    if(res != MCO_SUCCESS)
      fail_mco_t("Failed to yield coroutine", res);
    unsigned int tmp = m + n;
    m = n;
    n = tmp;
  }
  /* Yield the last Fibonacci number. */
  mco_push(co, &m, sizeof(m));
}

static mco_coro* create_fibonacci_task_t() {
  /* Create the coroutine. */
  mco_coro* co;
  mco_desc desc = mco_desc_init(fibonacci_task_t, 0);
  mco_result res = mco_create(&co, &desc);
  if(res != MCO_SUCCESS)
    fail_mco_t("Failed to create coroutine", res);
  /* Return the task as a coroutine. */
  return co;
}

int thread_worker_t(void* data) {
  (void)data;
  while(1) {
    mco_coro* task = NULL;
    int task_id = 0;
    if(c89mtx_lock(&mutex) != c89thrd_success)
      fail_t("Unable to lock mutex");
    for(int i=0;i<NUM_TASKS;++i) {
      if(tasks[i] != NULL) {
        task = tasks[i];
        tasks[i] = NULL;
        task_id = i;
        break;
      }
    }
    if(c89mtx_unlock(&mutex) != c89thrd_success)
      fail_t("Unable to unlock mutex");
    if(!task) {
      /* All tasks finished. */
      return 0;
    }
    mco_result res = mco_resume(task);
    if(res != MCO_SUCCESS)
      fail_mco_t("Failed to yield coroutine", res);
    switch(mco_status(task)) {
      case MCO_SUSPENDED: { /* Task is not finished yet. */
        /* Add it back to task list. */
        if(c89mtx_lock(&mutex) != c89thrd_success)
          fail_t("Unable to lock mutex");
        tasks[task_id] = task;
        if(c89mtx_unlock(&mutex) != c89thrd_success)
          fail_t("Unable to unlock mutex");
        break;
      }
      case MCO_DEAD: { /* Task finished. */
        /* Retrieve storage set in last coroutine yield. */
        unsigned int ret = 0;
        res = mco_pop(task, &ret, sizeof(ret));
        if(res != MCO_SUCCESS)
          fail_mco_t("Failed to retrieve coroutine storage", res);
        /* Check task result. */
        if(ret != EXPECTED_RESULT) {
          printf("Unexpected result %u\n", ret);
          exit(-1);
        }
        /* Destroy the task. */
        mco_result res = mco_destroy(task);
        if(res != MCO_SUCCESS)
          fail_mco_t("Failed to destroy coroutine", res);
        break;
      }
      default: {
        fail_t("Unexpected task state");
        break;
      }
    }
  }
  return 0;
}

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// #include <GL/freeglut.h>
// int g_LeaveGameMode = 0;
// int g_InGameMode = 1;
// /*
//  * Call this function to have some text drawn at given coordinates
//  */
// void PrintText( int nX, int nY, char* pszText )
// {
//     int lines;
//     char *p;
//     /*
//      * Prepare the OpenGL state
//      */
//     glDisable( GL_LIGHTING );
//     glDisable( GL_DEPTH_TEST );
//     glMatrixMode( GL_PROJECTION );
//     glPushMatrix();
//     glLoadIdentity();
//     /*
//      * Have an orthogonal projection matrix set
//      */
//     glOrtho( 0, glutGet( GLUT_WINDOW_WIDTH ), 0, glutGet( GLUT_WINDOW_HEIGHT ), -1, +1);
//     /*
//      * Now the matrix mode
//      */
//     glMatrixMode( GL_MODELVIEW );
//     glPushMatrix();
//     glLoadIdentity();
//     /*
//      * Now the main text
//      */
//     glColor3ub( 0, 0, 0 );
//     glRasterPos2i( nX, nY );
//     for( p=pszText, lines=0; *p; p++ )
//     {
//         if( *p == '\n' )
//         {
//             lines++;
//             glRasterPos2i( nX, nY-(lines*18) );
//         }
//         glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18,  *p );
//     }
//     /*
//      * Revert to the old matrix modes
//      */
//     glMatrixMode( GL_PROJECTION );
//     glPopMatrix();
//     glMatrixMode( GL_MODELVIEW );
//     glPopMatrix();
//     /*
//      * Restore the old OpenGL states
//      */
//     glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
//     glEnable( GL_DEPTH_TEST );
//     glEnable( GL_LIGHTING );
// }

// /*
//  * This is the display routine for our sample FreeGLUT windows
//  */
// static float g_fTime = 0.0f;

// void SampleDisplay( void )
// {
//     /*
//      * Clear the screen
//      */
//     glClearColor( 0, 0.5, 1, 1 );
//     glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//     /*
//      * Have the cube rotated
//      */
//     glMatrixMode( GL_MODELVIEW );
//     glPushMatrix();
//     glRotatef( g_fTime, 0, 0, 1 );
//     glRotatef( g_fTime, 0, 1, 0 );
//     glRotatef( g_fTime, 1, 0, 0 );
//     /*
//      * And then drawn...
//      */
//     glColor3f( 1, 1, 0 );
//     /* glutWireCube( 20.0 ); */
//     glutWireTeapot( 20.0 );
//     /* glutWireSpher( 15.0, 15, 15 ); */
//     /* glColor3f( 0, 1, 0 ); */
//     /* glutWireCube( 30.0 ); */
//     /* glutSolidCone( 10, 20, 10, 2 ); */
//     /*
//      * Don't forget about the model-view matrix
//      */
//     glPopMatrix( );
//     /*
//      * Draw a silly text
//      */
//     if( g_InGameMode == 0 )
//         PrintText( 20, 20, "Hello there cruel world!" );
//     else
//         PrintText( 20, 20, "Press ESC to leave the game mode!" );
//     /*
//      * And swap this context's buffers
//      */
//     glutSwapBuffers( );
// }

// /*
//  * This is a sample idle function
//  */
// void SampleIdle( void )
// {
//     g_fTime += 0.5f;
//     if( g_LeaveGameMode == 1 )
//     {
//         glutLeaveGameMode( );
//         g_LeaveGameMode = 0;
//         g_InGameMode = 0;
//     }
// }

// /*
//  * The reshape function
//  */
// void SampleReshape( int nWidth, int nHeight )
// {
//     GLfloat fAspect = (GLfloat) nHeight / (GLfloat) nWidth;
//     GLfloat fPos[ 4 ] = { 0.0f, 0.0f, 10.0f, 0.0f };
//     GLfloat fCol[ 4 ] = { 0.5f, 1.0f,  0.0f, 1.0f };
//     /*
//      * Update the viewport first
//      */
//     glViewport( 0, 0, nWidth, nHeight );
//     /*
//      * Then the projection matrix
//      */
//     glMatrixMode( GL_PROJECTION );
//     glLoadIdentity();
//     glFrustum( -1.0, 1.0, -fAspect, fAspect, 1.0, 80.0 );
//     /*
//      * Move back the camera a bit
//      */
//     glMatrixMode( GL_MODELVIEW );
//     glLoadIdentity( );
//     glTranslatef( 0.0, 0.0, -40.0f );
//     /*
//      * Enable some features...
//      */
//     glEnable( GL_CULL_FACE );
//     glEnable( GL_DEPTH_TEST );
//     glEnable( GL_NORMALIZE );
//     /*
//      * Set up some lighting
//      */
//     glLightfv( GL_LIGHT0, GL_POSITION, fPos );
//     glEnable( GL_LIGHTING );
//     glEnable( GL_LIGHT0 );
//     /*
//      * Set up a sample material
//      */
//     glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fCol );
// }

// /*
//  * A sample keyboard callback
//  */
// void SampleKeyboard( unsigned char cChar, int nMouseX, int nMouseY )
// {
//     printf( "SampleKeyboard(): keypress '%c' at (%i,%i)\n", cChar, nMouseX, nMouseY );
// }

// /*
//  * A sample keyboard callback (for game mode window)
//  */
// void SampleGameModeKeyboard( unsigned char cChar, int nMouseX, int nMouseY )
// {
//     if( cChar == 27 ) g_LeaveGameMode = 1;
// }

// /*
//  * A sample special callback
//  */
// void SampleSpecial( int nSpecial, int nMouseX, int nMouseY )
// {
//     printf( "SampleSpecial(): special keypress %i at (%i,%i)\n", nSpecial, nMouseX, nMouseY );
// }

// /*
//  * A sample menu callback
//  */
// void SampleMenu( int menuID )
// {
//     /*
//      * Just print something funny
//      */
//     printf( "SampleMenu() callback executed, menuID is %i\n", menuID );
// }

void idle(void);
void display(void);
void reshape(int x, int y);
void keypress(unsigned char key, int x, int y);
void mouse(int bn, int st, int x, int y);
void motion(int x, int y);
void sball_motion(int x, int y, int z);
void sball_rotate(int rx, int ry, int rz);
void sball_button(int bn, int state);
static void vcross(float *res, const float *a, const float *b);
static void qmul(float *a, const float *b);
static void qrotation(float *q, float angle, float x, float y, float z);
static void qrotate(float *q, float angle, float x, float y, float z);
static void mrotation_quat(float *m, const float *q);
float cam_theta, cam_phi = 25, cam_dist = 8;
int mouse_x, mouse_y;
int bnstate[8];
int anim;
float torus_pos[3], torus_rot[4] = {0, 0, 0, 1};
int teapot, torus, cone, sphere;
long nframes;
#ifndef GL_FRAMEBUFFER_SRGB
#define GL_FRAMEBUFFER_SRGB	0x8db9
#endif
#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809d
#endif

void idle(void)
{
	glutPostRedisplay();
}

void display(void)
{
	long tm;
	float lpos[] = {-1, 2, 3, 0};
	float sbrot_xform[16];
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -cam_dist);
	glRotatef(cam_phi, 1, 0, 0);
	glRotatef(cam_theta, 0, 1, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glPushMatrix();
	if(anim) {
		tm = glutGet(GLUT_ELAPSED_TIME);
		glRotatef(tm / 10.0f, 1, 0, 0);
		glRotatef(tm / 10.0f, 0, 1, 0);
	}
	glCallList(torus);
	glPopMatrix();
	glCallList(sphere);
	glPushMatrix();
	glTranslatef(torus_pos[0] - 2.5, torus_pos[1], torus_pos[2]);
	mrotation_quat(sbrot_xform, torus_rot);
	glMultMatrixf(sbrot_xform);
	glutSolidCube(1.5);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(2.5, -1, 0);
	glRotatef(-90, 1, 0, 0);
	glCallList(cone);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, -0.5, 2.5);
	glFrontFace(GL_CW);
	glCallList(teapot);
	glFrontFace(GL_CCW);
	glPopMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-5, -1.3, 5);
	glVertex3f(5, -1.3, 5);
	glVertex3f(5, -1.3, -5);
	glVertex3f(-5, -1.3, -5);
	glEnd();
	glutSwapBuffers();
	nframes++;
}

#define ZNEAR	0.5f
void reshape(int x, int y)
{
	float vsz, aspect = (float)x / (float)y;
	glViewport(0, 0, x, y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	vsz = 0.4663f * ZNEAR;
	glFrustum(-aspect * vsz, aspect * vsz, -vsz, vsz, 0.5, 500.0);
}

void keypress(unsigned char key, int x, int y)
{
	static int fullscr;
	static int prev_xsz, prev_ysz;
	static long start_msec;
	switch(key) {
	case 27:
	case 'q':
		glutExit();
		break;
	case ' ':
		anim ^= 1;
		glutIdleFunc(anim ? idle : 0);
		glutPostRedisplay();
		if(anim) {
			start_msec = glutGet(GLUT_ELAPSED_TIME);
			nframes = 0;
		} else {
			long tm = glutGet(GLUT_ELAPSED_TIME) - start_msec;
			long fps = (nframes * 100000) / tm;
			printf("framerate: %ld.%ld fps\n", fps / 100, fps % 100);
		}
		break;
	case '\n':
	case '\r':
		if(!(glutGetModifiers() & GLUT_ACTIVE_ALT)) {
			break;
		}
	case 'f':
		fullscr ^= 1;
		if(fullscr) {
			prev_xsz = glutGet(GLUT_WINDOW_WIDTH);
			prev_ysz = glutGet(GLUT_WINDOW_HEIGHT);
			glutFullScreen();
		} else {
			glutReshapeWindow(prev_xsz, prev_ysz);
		}
		break;
	}
}

void mouse(int bn, int st, int x, int y)
{
	int bidx = bn - GLUT_LEFT_BUTTON;
	bnstate[bidx] = st == GLUT_DOWN;
	mouse_x = x;
	mouse_y = y;
}

void motion(int x, int y)
{
	int dx = x - mouse_x;
	int dy = y - mouse_y;
	mouse_x = x;
	mouse_y = y;
	if(!(dx | dy)) return;
	if(bnstate[0]) {
		cam_theta += dx * 0.5;
		cam_phi += dy * 0.5;
		if(cam_phi < -90) cam_phi = -90;
		if(cam_phi > 90) cam_phi = 90;
		glutPostRedisplay();
	}
	if(bnstate[2]) {
		cam_dist += dy * 0.1;
		if(cam_dist < 0) cam_dist = 0;
		glutPostRedisplay();
	}
}

void sball_motion(int x, int y, int z)
{
	torus_pos[0] += x * 0.001f;
	torus_pos[1] += y * 0.001f;
	torus_pos[2] -= z * 0.001f;
	glutPostRedisplay();
}

static float rsqrt(float number)
{
	int i;
	float x2, y;
	static const float threehalfs = 1.5f;
	x2 = number * 0.5f;
	y = number;
	i = *(int*)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float*)&i;
	y *= threehalfs - (x2 * y * y);
	y *= threehalfs - (x2 * y * y);
	return y;
}

void sball_rotate(int rx, int ry, int rz)
{
	if(rx | ry | rz) {
		float s = (float)rsqrt(rx * rx + ry * ry + rz * rz);
		qrotate(torus_rot, 0.001f / s, rx * s, ry * s, -rz * s);
		glutPostRedisplay();
	}
}

void sball_button(int bn, int state)
{
	if(state == GLUT_DOWN) {
		torus_pos[0] = torus_pos[1] = torus_pos[2] = 0;
		torus_rot[0] = torus_rot[1] = torus_rot[2] = 0;
		torus_rot[3] = 1;
		glutPostRedisplay();
	}
}

static void vcross(float *res, const float *a, const float *b)
{
	res[0] = a[1] * b[2] - a[2] * b[1];
	res[1] = a[2] * b[0] - a[0] * b[2];
	res[2] = a[0] * b[1] - a[1] * b[0];
}

static void qmul(float *a, const float *b)
{
	float x, y, z, dot;
	float cross[3];
	dot = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	vcross(cross, a, b);
	x = a[3] * b[0] + b[3] * a[0] + cross[0];
	y = a[3] * b[1] + b[3] * a[1] + cross[1];
	z = a[3] * b[2] + b[3] * a[2] + cross[2];
	a[3] = a[3] * b[3] - dot;
	a[0] = x;
	a[1] = y;
	a[2] = z;
}

void mglut_sincos(float angle, float *sptr, float *cptr);
float mglut_tan(float x);

static void qrotation(float *q, float angle, float x, float y, float z)
{
	float sa, ca;
	mglut_sincos(angle * 0.5f, &sa, &ca);
	q[3] = ca;
	q[0] = x * sa;
	q[1] = y * sa;
	q[2] = z * sa;
}

static void qrotate(float *q, float angle, float x, float y, float z)
{
	float qrot[4];
	qrotation(qrot, angle, x, y, z);
	qmul(qrot, q);
	q[0] = qrot[0];
	q[1] = qrot[1];
	q[2] = qrot[2];
	q[3] = qrot[3];
}

static void mrotation_quat(float *m, const float *q)
{
	float xsq2 = 2.0f * q[0] * q[0];
	float ysq2 = 2.0f * q[1] * q[1];
	float zsq2 = 2.0f * q[2] * q[2];
	float sx = 1.0f - ysq2 - zsq2;
	float sy = 1.0f - xsq2 - zsq2;
	float sz = 1.0f - xsq2 - ysq2;
	m[3] = m[7] = m[11] = m[12] = m[13] = m[14] = 0.0f;
	m[15] = 1.0f;
	m[0] = sx;
	m[1] = 2.0f * q[0] * q[1] + 2.0f * q[3] * q[2];
	m[2] = 2.0f * q[2] * q[0] - 2.0f * q[3] * q[1];
	m[4] = 2.0f * q[0] * q[1] - 2.0f * q[3] * q[2];
	m[5] = sy;
	m[6] = 2.0f * q[1] * q[2] + 2.0f * q[3] * q[0];
	m[8] = 2.0f * q[2] * q[0] + 2.0f * q[3] * q[1];
	m[9] = 2.0f * q[1] * q[2] - 2.0f * q[3] * q[0];
	m[10] = sz;
}

#include "LightHook.h"
static HookInformation testHook;
typedef int(*TestFunction_t)(int param1, int param2);
__declspec(noinline) int TestFunction(int param1, int param2)
{
    param2 -= param1;
    param1 += param2;
    for (int i = 0; i < 10; i++)
        param1 += param2 * param2;
    param2 *= param1;
    param1 *= param2;
    for (int i = 0; i < 5; i++)
        param1 += param2 * param2;
    printf("in func: %i\n", param1);
    return param1;
}

__declspec(noinline) int HookedTestFunction()
{
    printf("hook called\n");
    TestFunction_t original = (TestFunction_t)testHook.Trampoline;
    return original(0, 2);
}

static int foobar(lua_State *L) {
	// assert(lbind_type(input, 0) == LT_INTEGER);
	// int a = lbind_tointeger(input, 0);
	// lbind_pushinteger(output, a*2);
	printf("foobar\n");
	int n=lua_gettop(L);
	double sum=0;
	for(int i=1; i<=n; i++) {
		if(!lua_isnumber(L,i))
		{
			lua_pushstring(L, "Incorrect argument to 'foobar'");
			lua_error(L);
		}
		sum += lua_tonumber(L,i);
	}
	lua_pushnumber(L, sum/n);
	lua_pushnumber(L, sum);
	return 2;
}

#include "atask.h"
struct iocp_evt_s
{
    event_t event;
    OVERLAPPED overlapped;
    ULONG_PTR key;
    DWORD numberOfBytes;
    DWORD dwError;
};
static HANDLE s_iocp = NULL;
static SOCKET s_sock = -1;
#ifndef WSAID_ACCEPTEX
#define WSAID_ACCEPTEX {0xb5367df1,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92}}
#endif
#ifndef WSAID_GETACCEPTEXSOCKADDRS
#define WSAID_GETACCEPTEXSOCKADDRS { 0xb5367df2,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92} }
#endif
typedef BOOL (WINAPI *MY_LPFN_ACCEPTEX)(SOCKET, SOCKET, PVOID, DWORD, DWORD, DWORD, LPDWORD, LPOVERLAPPED);
typedef VOID (WINAPI *MY_LPFN_GETACCEPTEXSOCKADDRS) (PVOID, DWORD, DWORD, DWORD, struct sockaddr **, LPINT, struct sockaddr **, LPINT);
MY_LPFN_ACCEPTEX lpfnAcceptEx = NULL;
MY_LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockAddrs = NULL;
/* IOCP事件循环 */
void iocp_atask_run(HANDLE iocp)
{
    DWORD dwError;
    DWORD numberOfBytes;
    ULONG_PTR key;
    OVERLAPPED *pOverlapped;
    time_nclk_t due, now;
    time_ms_t timeout;
    struct iocp_evt_s *iocp_evt;
    while (1)
    {
        due = el_schedule();
        /* 计算超时 */
        now = time_nclk_get();
        timeout = due < now ? 0 : time_nclk_to_us(due - now) / 1000;
        timeout = timeout > UINT32_MAX ? INFINITE : timeout;
        /* 若IOCP存在，则使用IOCP超时，否则使用Sleep替代 */
        if (iocp != NULL)
        {
            /* 获取IOCP队列中已完成的事件，直到完全获取 */
            while (1)
            {
                dwError = GetQueuedCompletionStatus(iocp, &numberOfBytes, &key, &pOverlapped, (DWORD)timeout) ? 0 : GetLastError();
                /* 若获取到IOCP事件，则提交至事件循环 */
                if (pOverlapped != NULL)
                {
                    iocp_evt = container_of(pOverlapped, struct iocp_evt_s, overlapped);
                    iocp_evt->key = key;
                    iocp_evt->numberOfBytes = numberOfBytes;
                    iocp_evt->dwError = dwError;
                    el_event_post(&iocp_evt->event);
                }
                else
                {
                    break;
                }
                timeout = 0;
            };
        }
        else
        {
            Sleep((DWORD)timeout);
        }
    }
}
/* Http客户端数据接收超时 */
#define HTTP_CLIENT_RECV_TIMEOUT_MS         5000
/* Http客户端请求任务的栈大小 */
#define HTTP_CLIENT_REQUST_TASK_STACK_SIZE  (256 + 3072)
/* Http客户端最大数量 */
#define HTTP_CLIENT_MAX_NUMS                30000
#define SERVER_STRING "Server: libatask httpd 1.0\r\n"
struct http_task_with_stack_s
{
    task_t task;
    event_t free_ev;
    uint8_t stack[HTTP_CLIENT_REQUST_TASK_STACK_SIZE];
};
static uint8_t http_client_tasks_slab_buff[HTTP_CLIENT_REQUST_TASK_STACK_SIZE * HTTP_CLIENT_MAX_NUMS + sizeof(slab_t)];
static slab_t *http_client_tasks_slab = NULL;
/* 从客户端获取数据 */
void http_client_data_get(task_t *task, 
                            event_t *ev, 
                            SOCKET _cli_sock, 
                            void *_buf, 
                            uint32_t _buf_size,
                            uint32_t *_recv_size,
                            time_ms_t _recv_timeout)
{
    uint8_t *bpd = TASK_BPD(task);
    struct vars
    {
        SOCKET cli_sock;
        timer_event_t timer;
        struct iocp_evt_s iocp_ev;
        WSABUF wsa_buf;
        uint32_t *recv_size;
    } *vars = (struct vars *)task_asyn_vars_get(task, sizeof(*vars));
    DWORD flags;
    DWORD dwError;
    /* 协程开始 */
    bpd_begin(2);
    /* 保存参数 */
    vars->cli_sock = _cli_sock;
    vars->recv_size = _recv_size;
    /* 初始化定时器 */
    timer_init_inherit(&vars->timer, &task->event);
    /* 初始化IOCP事件 */
    memset(&vars->iocp_ev, 0, sizeof(vars->iocp_ev));
    event_init_inherit(&vars->iocp_ev.event, &task->event);
    vars->wsa_buf.buf = _buf;
    vars->wsa_buf.len = _buf_size;
    flags = 0;
    /* 启动WSARecv操作 */
    if (SOCKET_ERROR == WSARecv(vars->cli_sock, 
                    &vars->wsa_buf, 
                    1,
                    NULL,
                    &flags, 
                    &vars->iocp_ev.overlapped, NULL))
    {
        dwError = WSAGetLastError();
        if (dwError != ERROR_IO_PENDING)
        {
            task->ret_val.u32 = dwError;
            /* 协程结束 */
            bpd_break;
        }
    }
    /* 开启接收超时 */
    el_timer_start_ms(&vars->timer, _recv_timeout);
    /* 挂起协程，等待事件 */
    bpd_yield(1);
    /* 若为超时事件，则结束请求 */
    if (ev == &vars->timer.event)
    {
        /* 若IOCP事件已就绪，但未到达 */
        if (el_event_is_ready(&vars->iocp_ev.event))
        {
            /* 从事件循环中取消该事件 */
            el_event_cancel(&vars->iocp_ev.event);
        }
        else
        {
            /* 取消IOCP请求 */
            if (FALSE == CancelIo((HANDLE)vars->cli_sock))
            {
                printf("Warnnig: CancelIo Failed, Error:%u\n", (uint32_t)GetLastError());
            }
            /* 等待IOCP事件返回错误 */
            bpd_yield(2);
            *vars->recv_size = 0;
            task->ret_val.u32 = WAIT_TIMEOUT;
            /* 协程结束 */
            bpd_break;
        }
    }
    /* 若是IOCP事件，停止定时器 */
    else
    {
        el_timer_stop(&vars->timer);
    }
    task->ret_val.u32 = vars->iocp_ev.dwError;
    *vars->recv_size = vars->iocp_ev.numberOfBytes;
    /* 协程结束 */
    bpd_end();
    /* 异步返回 */
    task_asyn_return(task);
}
int mystrcasecmp(const char *s1, const char *s2)
{
    while (*s1 && *s2)
    {
        if (*s1 != *s2
          && tolower(*s1) != tolower(*s2))
        {
            break;
        }
        s1++;
        s2++;
    }
    return (signed char)*s1 - (signed char)*s2;
}

static const char http_501_rsp_text[] = 
"HTTP/1.0 501 Method Not Implemented\r\n"
SERVER_STRING
"Content-Type: text/html\r\n"
"\r\n"
"<html><head><title>Method Not Implemented\r\n"
"</title></head>\r\n"
"<body><p>HTTP request method not supported.\r\n"
"</p></body></html>\r\n";
static const char http_404_rsp_text[] = 
"HTTP/1.0 404 NOT FOUND\r\n"
SERVER_STRING
"Content-Type: text/html\r\n"
"\r\n"
"<html><head><title>Not Found\r\n"
"</title></head>\r\n"
"<body><p>The service could not respond to the path you requested.\r\n"
"</p></body></html>\r\n";
static const char http_ok_rsp_html[] = 
"HTTP/1.0 200 OK\r\n"
SERVER_STRING
"Content-Type: text/html\r\n"
"\r\n";
static const char http_ok_rsp_js[] = 
"HTTP/1.0 200 OK\r\n"
SERVER_STRING
"Content-Type: application/x-javascript\r\n"
"\r\n";
static const char http_ok_rsp_css[] = 
"HTTP/1.0 200 OK\r\n"
SERVER_STRING
"Content-Type: text/css\r\n"
"\r\n";
static const char http_ok_rsp_other[] = 
"HTTP/1.0 200 OK\r\n"
SERVER_STRING
"Content-Type: application/octet-stream\r\n"
"\r\n";
/* http客户端发送数据 */
void http_client_send(task_t *task, event_t *ev, SOCKET _cli_sock, const void *_buf, uint32_t _buf_size)
{
    uint8_t *bpd = TASK_BPD(task);
    struct vars
    {
        SOCKET cli_sock;
        DWORD send_bytes;
        WSABUF wsa_buf;
        struct iocp_evt_s iocp_ev;
        char *buf;
        uint32_t buf_size;
    } *vars = (struct vars *)task_asyn_vars_get(task, sizeof(*vars));
    DWORD dwError;
    /* 协程开始 */
    bpd_begin(1);
    /* 保存参数 */
    vars->buf = (char *)_buf;
    vars->buf_size = _buf_size;
    vars->cli_sock = _cli_sock;
    vars->send_bytes = 0;
    /* 初始化IOCP事件 */
    memset(&vars->iocp_ev, 0, sizeof(vars->iocp_ev));
    event_init_inherit(&vars->iocp_ev.event, &task->event);
    while (vars->send_bytes < vars->buf_size)
    {
        vars->wsa_buf.buf = vars->buf + vars->send_bytes;
        vars->wsa_buf.len = vars->buf_size - vars->send_bytes;
        /* 启动WSASend操作 */
        if (SOCKET_ERROR == WSASend(vars->cli_sock, 
                        &vars->wsa_buf, 
                        1,
                        NULL,
                        0, 
                        &vars->iocp_ev.overlapped, NULL))
        {
            dwError = WSAGetLastError();
            if (dwError != ERROR_IO_PENDING)
            {
                task->ret_val.u32 = vars->send_bytes;
                /* 协程结束 */
                bpd_break;
            }
        }
        /* 挂起协程，等待事件 */
        bpd_yield(1);
        /* 发送失败 */
        if (vars->iocp_ev.dwError != 0 || vars->iocp_ev.numberOfBytes == 0)
        {
            task->ret_val.u32 = vars->send_bytes;
            /* 协程结束 */
            bpd_break;
        }
        vars->send_bytes += vars->iocp_ev.numberOfBytes;
    }
    task->ret_val.u32 = vars->send_bytes;
    /* 协程结束 */
    bpd_end();
    /* 异步返回 */
    task_asyn_return(task);
}
/* 客户端请求处理任务函数 */
void http_client_requst_task_handler(task_t *task, event_t *ev, SOCKET _cli_sock, const struct sockaddr_in *_client_addr)
{
    uint8_t *bpd = TASK_BPD(task);
    struct vars
    {
        SOCKET cli_sock;
        struct sockaddr_in client_addr;
        uint8_t buf[2048];
        char method[6];
        char url[400];
        char path[512];
        char *query_string;
        uint32_t data_size;
        uint8_t cgi;
        uint8_t bps;
        uint8_t header_end;
        uint32_t i;
        HANDLE hFile;
        struct iocp_evt_s read_iocp_evt;
        uint64_t offset;
    } *vars = (struct vars *)task_asyn_vars_get(task, sizeof(*vars));
    uint8_t *buf;
    uint8_t *buf_end;
    WIN32_FIND_DATAA findInfo;
    HANDLE hFind;
    const char *str;
    /* 协程开始 */
    bpd_begin(14);
    /* 保存传入的参数至异步变量 */
    vars->cli_sock = _cli_sock;
    vars->client_addr = *_client_addr;
    vars->header_end = 0;
    while (1)
    {
        /* 异步调用http_client_data_get方法，带超时地获取客户端数据 */
        task_bpd_asyn_call(1, task, http_client_data_get, 
                            vars->cli_sock, 
                            vars->buf,
                            sizeof(vars->buf),
                            &vars->data_size,
                            HTTP_CLIENT_RECV_TIMEOUT_MS);
        if (task->ret_val.u32 != 0 || vars->data_size == 0)
        {
            closesocket(vars->cli_sock);
            bpd_break;
        }
        /* 设置buf, buf_end以供bpd语句块解析。
         * buf中存在换行，则设置buf_end换行处
         */
        buf = vars->buf;
        for (buf_end = buf; buf_end < buf + vars->data_size; buf_end++)
        {
            if (*buf_end == '\r' || *buf_end == '\n')
            {
                break;
            }
        }
        /* bpd语句块开始 */
        #define BP_STMT_DEFAULT         vars->bps
        #define BP_STMT_DEFAULT_PREFIX  parse_data_label
        bpd_stmt_begin(4)
        {
            /* 过滤掉前面的空白符 */
            do
            {
                /* 等待buf非空 */
                bpd_stmt_yield_until(1, buf < buf_end);
            } while (isspace(*buf) && buf++);
            /* 获取method字段 */
            vars->i = 0;
            while (1)
            {
                /* 等待buf非空 */
                bpd_stmt_yield_until(2, buf < buf_end);
                if (!isspace(*buf))
                {
                    if (vars->i < sizeof(vars->method) - 1)
                    {
                        vars->method[vars->i++] = *buf;
                    }
                    buf++;
                }
                else
                {
                    break;
                }
            }
            vars->method[vars->i] = 0;
            /* 过滤掉前面的空白符 */
            do
            {
                /* 等待buf非空 */
                bpd_stmt_yield_until(3, buf < buf_end);
            } while (isspace(*buf) && buf++);
            /* 获取url字段 */
            vars->i = 0;
            while (1)
            {
                /* 等待buf非空 */
                bpd_stmt_yield_until(4, buf < buf_end);

                if (!isspace(*buf))
                {
                    if (vars->i < sizeof(vars->url) - 1)
                    {
                        vars->url[vars->i++] = *buf;
                    }
                    buf++;
                }
                else
                {
                    break;
                }
            }
            vars->url[vars->i] = 0;
        }
        /* bpd语句块结束 */
        bpd_stmt_end();
        #undef BP_STMT_DEFAULT
        #undef BP_STMT_DEFAULT_PREFIX
        /* 在一行中，未解析出method和url，http请求错误 */
        if (buf_end != vars->buf + vars->data_size&& vars->bps != 0)
        {
            closesocket(vars->cli_sock);
            bpd_break;
        }
        /* 解析成功 */
        if (vars->bps == BP_STMT_INIT_VAL)
        {
            break;
        }
    }
    /* 读取整个header */
    while (1)
    {
        /* 检查上一个buf是否存在header结尾标识 */
        buf = buf_end;
        buf_end = vars->buf + vars->data_size;
        /* bpd语句块开始 */
        #define BP_STMT_DEFAULT         vars->bps
        #define BP_STMT_DEFAULT_PREFIX  find_header_end
        bpd_stmt_begin(3)
        {
            while (1)
            {
                bpd_stmt_yield_until(1, buf < buf_end);
                if (*buf == '\n')
                {
                    buf++;
                    bpd_stmt_yield_until(2, buf < buf_end);
                    if (*buf == '\r')
                    {
                        buf++;
                        bpd_stmt_yield_until(3, buf < buf_end);
                    }
                    /* 成功 */
                    if (*buf == '\n')
                    {
                        bpd_stmt_break;
                    }
                }
                else
                {
                    buf++;
                }
            }
        }
        /* bpd语句块结束 */
        bpd_stmt_end();
        #undef BP_STMT_DEFAULT
        #undef BP_STMT_DEFAULT_PREFIX
        /* 读取整个头部结束 */
        if (vars->bps == BP_STMT_INIT_VAL)
        {
            break;
        }
        /* 异步调用http_client_data_get方法，带超时地获取客户端数据 */
        task_bpd_asyn_call(2, task, http_client_data_get, 
                            vars->cli_sock, 
                            vars->buf,
                            sizeof(vars->buf),
                            &vars->data_size,
                            HTTP_CLIENT_RECV_TIMEOUT_MS);
        if (task->ret_val.u32 != 0 || vars->data_size == 0)
        {
            closesocket(vars->cli_sock);
            bpd_break;
        }
        buf_end = vars->buf;
    }
    /* 不支持非GET和非POST方法 */
    if (mystrcasecmp(vars->method, "GET") != 0 
     && mystrcasecmp(vars->method, "POST") != 0)
    {
        task_bpd_asyn_call(3, task, http_client_send,
                            vars->cli_sock,
                            http_501_rsp_text,
                            sizeof(http_501_rsp_text) - 1);

        goto client_close;
    }
    /* 处理URL及参数信息 */
    if (mystrcasecmp(vars->method, "GET") == 0)
    {
        vars->query_string = vars->url;
        while ((*vars->query_string != '?') && (*vars->query_string != '\0'))
        {
            vars->query_string++;
        }
        if (*vars->query_string == '?')
        {
            vars->cgi = 1;
            *vars->query_string = '\0';
            vars->query_string++;
        }
    }
    /* 生成Path */
    sprintf(vars->path, "www%s", vars->url);
    if (vars->path[strlen(vars->path) - 1] == '/')
    {
        strcat(vars->path, "index.html");
    }
    /* 判断文件是否存在 */
    hFind = FindFirstFileA(vars->path, &findInfo);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        task_bpd_asyn_call(4, task, http_client_send,
                            vars->cli_sock,
                            http_404_rsp_text,
                            sizeof(http_404_rsp_text) - 1);

        goto client_close;
    }
    FindClose(hFind);
    /* 是一个目录 */
    if (findInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        strcat(vars->path, "/index.html");
        hFind = FindFirstFileA(vars->path, &findInfo);
        if (hFind == INVALID_HANDLE_VALUE)
        {
            task_bpd_asyn_call(5, task, http_client_send,
                                vars->cli_sock,
                                http_404_rsp_text,
                                sizeof(http_404_rsp_text) - 1);

            goto client_close;
        }
        FindClose(hFind);
    }
    /* 打开文件 */
    vars->hFile = CreateFileA(vars->path, 
                    GENERIC_READ,
                    FILE_SHARE_READ,
                    NULL,
                    OPEN_EXISTING,
                    FILE_FLAG_OVERLAPPED,
                    NULL);
    if (vars->hFile == INVALID_HANDLE_VALUE)
    {
        task_bpd_asyn_call(6, task, http_client_send,
                            vars->cli_sock,
                            http_404_rsp_text,
                            sizeof(http_404_rsp_text) - 1);
        goto client_close;
    }
    /* 添加HANDLE到IOCP */
    if (NULL == CreateIoCompletionPort(vars->hFile, s_iocp, 0, 0))
    {
        task_bpd_asyn_call(7, task, http_client_send,
                            vars->cli_sock,
                            http_404_rsp_text,
                            sizeof(http_404_rsp_text) - 1);
        CloseHandle(vars->hFile);
        goto client_close;
    }
    /* 发送HTTP头 */
    str = strrchr(vars->path, '.');
    if (str == NULL)
    {
        task_bpd_asyn_call(8, task, http_client_send,
                            vars->cli_sock,
                            http_ok_rsp_other,
                            sizeof(http_ok_rsp_other) - 1);
    }
    else if (mystrcasecmp(str, ".htm") == 0 || mystrcasecmp(str, ".html") == 0)
    {
        task_bpd_asyn_call(9, task, http_client_send,
                            vars->cli_sock,
                            http_ok_rsp_html,
                            sizeof(http_ok_rsp_html) - 1);
    }
    else if (mystrcasecmp(str, ".js") == 0)
    {
        task_bpd_asyn_call(10, task, http_client_send,
                            vars->cli_sock,
                            http_ok_rsp_js,
                            sizeof(http_ok_rsp_js) - 1);
    }
    else if (mystrcasecmp(str, ".css") == 0)
    {
        task_bpd_asyn_call(11, task, http_client_send,
                            vars->cli_sock,
                            http_ok_rsp_css,
                            sizeof(http_ok_rsp_css) - 1);
    }
    else
    {
        task_bpd_asyn_call(12, task, http_client_send,
                            vars->cli_sock,
                            http_ok_rsp_other,
                            sizeof(http_ok_rsp_other) - 1);
    }
    /***发送文件到客户端***/
    memset(&vars->read_iocp_evt, 0, sizeof(vars->read_iocp_evt));
    event_init_inherit(&vars->read_iocp_evt.event, &task->event);
    vars->offset = 0;
    while (1)
    {
        vars->read_iocp_evt.overlapped.Offset = vars->offset & 0xFFFFFFFF;
        vars->read_iocp_evt.overlapped.OffsetHigh = vars->offset >> 32;
        /* 读文件 */
        if (FALSE == ReadFile(vars->hFile, 
                        vars->buf, 
                        sizeof(vars->buf),
                        NULL,
                        &vars->read_iocp_evt.overlapped))
        {
            if (GetLastError() != ERROR_IO_PENDING)
            {
                CloseHandle(vars->hFile);
                goto client_close;
            }
        }
        bpd_yield(13);
        /* 发送到客户端 */
        if (vars->read_iocp_evt.numberOfBytes != 0
         && (vars->read_iocp_evt.dwError == 0 || vars->read_iocp_evt.dwError == ERROR_HANDLE_EOF))
        {
            task_bpd_asyn_call(14, task, http_client_send,
                                vars->cli_sock,
                                vars->buf,
                                vars->read_iocp_evt.numberOfBytes);
            if (task->ret_val.u32 != vars->read_iocp_evt.numberOfBytes)
            {
                CloseHandle(vars->hFile);
                goto client_close;
            }
        }
        if (vars->read_iocp_evt.dwError == ERROR_HANDLE_EOF)
        {
            CloseHandle(vars->hFile);
            goto client_close;
        }
        vars->offset += vars->read_iocp_evt.numberOfBytes;
    }

client_close:
    closesocket(vars->cli_sock);
    /* 协程结束 */
    bpd_end();
    /* 异步返回 */
    task_asyn_return(task);
}

static void on_task_end(void *task, event_t *ev)
{
    /* 释放task */
    slab_free(http_client_tasks_slab, task);
}

/* Accept任务函数 */
void http_accept_task_handler(task_t *task, event_t *ev)
{
    uint8_t *bpd = TASK_BPD(task);
    struct http_task_with_stack_s *task_with_stack;
    DWORD dwError;
    /* 从任务的栈中获取或分配异步变量。
       协程在挂起时，函数的局部变量将被销毁，而异步变量则依旧存在 */
    struct vars
    {
        struct iocp_evt_s accept_evt;
        char acceptex_out_buffer[(sizeof(struct sockaddr_in) + 16) * 2 + 16];
        SOCKET cli_sock;
        DWORD dwBytes;
        struct sockaddr_in *clientAddr, *localAddr;
        int clientAddrLen, localAddrLen;
        timer_event_t timer;
        slab_alloc_event_t alloc_ev;
    } *vars = (struct vars *)task_asyn_vars_get(task, sizeof(*vars));
    /* 协程开始 */
    bpd_begin(3);
    /* 清零accept_evt结构体 */
    memset(&vars->accept_evt, 0, sizeof(vars->accept_evt));
    /* accept_evt的事件从task中继承，
       这样accpet_evt事件将由本协程处理 */
    event_init_inherit(&vars->accept_evt.event, &task->event);
    /* 初始化timer */
    timer_init_inherit(&vars->timer, &task->event);
    /* 初始化slab分配器事件 */
    slab_alloc_event_init_inherit(&vars->alloc_ev, &task->event);
    /* 初始化http客户端slab */
    http_client_tasks_slab = slab_create(http_client_tasks_slab_buff, 
                                sizeof(http_client_tasks_slab_buff), 
                                sizeof(struct http_task_with_stack_s));
    while (1)
    {
        /* 为新客户端创建一个套接字 */
        vars->cli_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (vars->cli_sock == INVALID_SOCKET) {
            printf("Create accept socket failed with error: %u\n", WSAGetLastError());
            /* 500毫秒后重试 */
            el_timer_start_ms(&vars->timer, 500);
            /* 挂起协程，并等待定时器 */
            bpd_yield(1);
            continue;
        }
        /* 开始AcceptEx，当AcceptEx重叠I/O完成时，
           将触发accept_evt事件，重新进入协程 */
        if (FALSE == lpfnAcceptEx(s_sock, 
                    vars->cli_sock, 
                    vars->acceptex_out_buffer, 
                    0, 
                    sizeof(struct sockaddr_in) + 16, 
                    sizeof(struct sockaddr_in) + 16, 
                    &vars->dwBytes, 
                    &vars->accept_evt.overlapped))
        {
            dwError = WSAGetLastError();
            if (dwError != ERROR_IO_PENDING)
            {
                printf("AcceptEx failed! error:%u\n", (uint32_t)dwError);
                closesocket(vars->cli_sock);
                continue;
            }
        }
        /* 挂起协程，并等待事件 */
        bpd_yield(2);
        /* 判断操作是否成功 */
        if (vars->accept_evt.dwError != 0)
        {
            printf("AcceptEx overlapped I/O failed! error:%u\n", (uint32_t)vars->accept_evt.dwError);
            closesocket(vars->cli_sock);
            continue;
        }
        /* 获取客户端信息 */
        lpfnGetAcceptExSockAddrs(vars->acceptex_out_buffer, 
                0, 
                sizeof(struct sockaddr_in),
                sizeof(struct sockaddr_in),
                (LPSOCKADDR *)&vars->localAddr,
                &vars->localAddrLen,
                (LPSOCKADDR *)&vars->clientAddr,
                &vars->clientAddrLen);
        /* 添加Socket句柄到IOCP */
        if (NULL == CreateIoCompletionPort((HANDLE)vars->cli_sock, s_iocp, 0, 0))
        {
            printf("Add socket handle failed! error:%d\n", (uint32_t)GetLastError());
            closesocket(vars->cli_sock);
            continue;
        }
        /* 从slab分配一个task */
        task_with_stack = (struct http_task_with_stack_s *)slab_alloc(http_client_tasks_slab);
        while (task_with_stack == NULL)
        {
            /* 等待slab可用 */
            slab_wait(http_client_tasks_slab, &vars->alloc_ev);
            bpd_yield(3);

            task_with_stack = (struct http_task_with_stack_s *)vars->alloc_ev.mem_blk;
        }
        /* 初始化http客户端请求task */
        task_init(&task_with_stack->task, 
                    task_with_stack->stack, 
                    sizeof(task_with_stack->stack),
                    EVENT_PRIORITY(&task->event));
        /* 监听任务结束，并在on_task_end释放使用的内存 */
        event_init(&task_with_stack->free_ev, on_task_end, task_with_stack, MIDDLE_GROUP_PRIORITY);
        task_end_wait(&task_with_stack->task, &task_with_stack->free_ev);
        /* 处理该客户端Http请求 */
        task_start(&task_with_stack->task, 
                    http_client_requst_task_handler, 
                    vars->cli_sock,
                    vars->clientAddr);
    }
    /* 协程结束 */
    bpd_end();
}

/* Asynchronous function 1 */
/* 异步函数1 */
static void async_func1(task_t *task, event_t *ev, time_ms_t interval, time_ms_t timeout)
{
    /* 定义协程（异步函数）所需的bpd指针 */
    uint8_t *bpd = TASK_BPD(task);
    /* 定义协程（异步函数）的异步变量 */
    struct vars
    {
        /* 定义一个定时器 */
        timer_event_t timer;
        /* 总执行时间 */
        time_ms_t total;
        /* 已执行时间*/
        time_ms_t current;
        /* 执行间隔 */
        time_ms_t interval;
        /* 将异步变量的地址绑定到当前任务的栈空间中 */
    } *vars = task_asyn_vars_get(task, sizeof(*vars));
    /* 开始协程 */
    bpd_begin(2);
    /* 可直接使用task中的event对其他事件进行初始化，
     * 保证这些事件返回之后可恢复当前协程 */
    timer_init_inherit(&vars->timer, &task->event);
    /* 在yield之前先将参数保存到异步变量中，使之yield之后仍能使用 */
    vars->total = timeout;
    vars->interval = interval;
    vars->current = 0;
    for (vars->current = 0; 
        vars->current < vars->total;
        vars->current += vars->interval * 2)
    {
        /* 延时interval ms */
        el_timer_start_ms(&vars->timer, vars->interval);
        bpd_yield(1);
        printf("LED ON\n");
        /* 延时interval ms */
        el_timer_start_ms(&vars->timer, vars->interval);
        bpd_yield(2);
        printf("LED OFF\n");
    }
    /* 结束协程 */
    bpd_end();
    /* 协程（异步函数）返回到异步调用者 */
    task_asyn_return(task);
}

/* 异步函数2 */
static void async_func2(task_t *task, event_t *ev)
{
    uint8_t *bpd = TASK_BPD(task);
    bpd_begin(2);
    while (1)
    {
        /* 异步调用异步函数1 参数：1000，8000*/
        task_bpd_asyn_call(1, task, async_func1, 1000, 8000);
        /* 异步调用异步函数1 参数：2000，10000*/
        task_bpd_asyn_call(2, task, async_func1, 2000, 10000);
    }
    bpd_end();
}

static LARGE_INTEGER freq = { 0 };

/* 获取当前时间时钟数 */
time_nclk_t time_nclk_get(void)
{
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    return (time_nclk_t)time.QuadPart;
}

/* 获取当前时间微秒数 */
time_us_t time_us_get(void)
{
    LARGE_INTEGER time;
    if (!freq.QuadPart)
    {
        QueryPerformanceFrequency(&freq);
    }
    QueryPerformanceCounter(&time);
    return (time_us_t)((double)time.QuadPart * 1000000 / (double)freq.QuadPart);
}

/* 将时钟数转为微秒 */
time_us_t time_nclk_to_us(time_nclk_t time_nclk)
{
    if (!freq.QuadPart)
    {
        QueryPerformanceFrequency(&freq);
    }
    return (time_us_t)((double)time_nclk * 1000000 / (double)freq.QuadPart);
}

/* 将微秒转为时钟数 */
time_nclk_t time_us_to_nclk(time_us_t time_us)
{
    if (!freq.QuadPart)
    {
        QueryPerformanceFrequency(&freq);
    }
    return (time_nclk_t)((double)time_us * freq.QuadPart / 1000000);
}

/* 异步函数1 */
static void async_func3(task_t *task, event_t *ev, time_ms_t interval, time_ms_t timeout)
{
    /* 定义协程（异步函数）所需的bpd指针 */
    uint8_t *bpd = TASK_BPD(task);
    /* 定义协程（异步函数）的异步变量 */
    struct vars
    {
        /* 定义一个定时器 */
        timer_event_t timer;
        /* 总执行时间 */
        time_ms_t total;
        /* 已执行时间*/
        time_ms_t current;
        /* 执行间隔 */
        time_ms_t interval;
        /* 将异步变量的地址绑定到当前任务的栈空间中 */
    } *vars = task_asyn_vars_get(task, sizeof(*vars));

    /* 开始协程 */
    bpd_begin(2);
    /* 可直接使用task中的event对其他事件进行初始化，
     * 保证这些事件返回之后可恢复当前协程 */
    timer_init_inherit(&vars->timer, &task->event);
    /* 在yield之前先将参数保存到异步变量中，使之yield之后仍能使用 */
    vars->total = timeout;
    vars->interval = interval;
    vars->current = 0;
    for (vars->current = 0; 
        vars->current < vars->total;
        vars->current += vars->interval * 2)
    {
        /* 延时interval ms */
        el_timer_start_ms(&vars->timer, vars->interval);
        bpd_yield(1);
        printf("LED ON\n");
        /* 延时interval ms */
        el_timer_start_ms(&vars->timer, vars->interval);
        bpd_yield(2);
        printf("LED OFF\n");
    }
    /* 结束协程 */
    bpd_end();
    /* 协程（异步函数）返回到异步调用者 */
    task_asyn_return(task);
}

/* 异步函数2 */
static void async_func4(task_t *task, event_t *ev)
{
    uint8_t *bpd = TASK_BPD(task);
    bpd_begin(2);
    while (1)
    {
        /* 异步调用异步函数1 参数：1000，8000*/
        task_bpd_asyn_call(3, task, async_func3, 1000, 8000);
        /* 异步调用异步函数1 参数：2000，10000*/
        task_bpd_asyn_call(4, task, async_func3, 2000, 10000);
    }
    bpd_end();
}

/* 任务结束回调 */
static void on_task_end_lw(void *ctx, event_t *ev)
{
    printf("%s\n", (const char *)ctx);
}

/* 单独的定时器回调 */
static void on_timer(void *ctx, event_t *ev)
{
    printf("%s\n", (const char *)ctx);
    el_timer_start_ms((timer_event_t*)ev, 1000);
}


int main(int argc, char *argv[])
{
  t_list();
  t_vector();
  t_tree();
  t_map();
  t_set();
  t_stack();
  t_queue();
  t_algo();
	
// First initialize a `desc` object through `mco_desc_init`.
// mco_desc desc = mco_desc_init(coro_entry, 0);
// // Configure `desc` fields when needed (e.g. customize user_data or allocation functions).
// desc.user_data = NULL;
// // Call `mco_create` with the output coroutine pointer and `desc` pointer.
// mco_coro* co;
// mco_result res = mco_create(&co, &desc);
// assert(res == MCO_SUCCESS);
// // The coroutine should be now in suspended state.
// assert(mco_status(co) == MCO_SUSPENDED);
// // Call `mco_resume` to start for the first time, switching to its context.
// res = mco_resume(co); // Should print "coroutine 1".
// assert(res == MCO_SUCCESS);
// // We get back from coroutine context in suspended state (because it's unfinished).
// assert(mco_status(co) == MCO_SUSPENDED);
// // Call `mco_resume` to resume for a second time.
// res = mco_resume(co); // Should print "coroutine 2".
// assert(res == MCO_SUCCESS);
// // The coroutine finished and should be now dead.
// assert(mco_status(co) == MCO_DEAD);
// // Call `mco_destroy` to destroy the coroutine.
// res = mco_destroy(co);
// assert(res == MCO_SUCCESS);

/* Create the coroutine. */
mco_coro* co;
mco_desc desc = mco_desc_init(fibonacci_coro, 0);
mco_result res = mco_create(&co, &desc);
if(res != MCO_SUCCESS)
fail("Failed to create coroutine", res);

/* Set storage. */
unsigned long max = 1000000000;
mco_push(co, &max, sizeof(max));

int counter = 1;
while(mco_status(co) == MCO_SUSPENDED) {
/* Resume the coroutine. */
res = mco_resume(co);
if(res != MCO_SUCCESS)
	fail("Failed to resume coroutine", res);

/* Retrieve storage set in last coroutine yield. */
unsigned long ret = 0;
res = mco_pop(co, &ret, sizeof(ret));
if(res != MCO_SUCCESS)
	fail("Failed to retrieve coroutine storage", res);
printf("fib %d = %lu\n", counter, ret);
counter = counter + 1;
}

/* Destroy the coroutine. */
res = mco_destroy(co);
if(res != MCO_SUCCESS)
fail("Failed to destroy coroutine", res);


bench_switch();
bench_init();
bench_create();


res_t rest = {0, BP_INIT_VAL, BP_INIT_VAL};
int nrand,round;
/* initialize random */
srand(122);
/* randomly produce and consume */
round = (rand() % 8) + 8;
while (1)
{
    nrand = rand() % 16;
    while (nrand--)
    {
        if (consumer(&rest) == FALSE)
            break;
    }
    nrand = rand() % 16;
    while (nrand--)
    {
       if (producer(&rest) == FALSE)
         break;
    }
    if(round < 0)
      break;
    round--;

}

/* init gui state */
// struct nk_context ctx;
// nk_init_fixed(&ctx, calloc(1, MAX_MEMORY), MAX_MEMORY, &font);
// enum {EASY, HARD};
// static int op = EASY;
// static float value = 0.6f;
// static int i =  20;
// if (nk_begin(&ctx, "Show", nk_rect(50, 50, 220, 220),
//     NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
//     /* fixed widget pixel width */
//     nk_layout_row_static(&ctx, 30, 80, 1);
//     if (nk_button_label(&ctx, "button")) {
//         /* event handling */
//     }
//     /* fixed widget window ratio width */
//     nk_layout_row_dynamic(&ctx, 30, 2);
//     if (nk_option_label(&ctx, "easy", op == EASY)) op = EASY;
//     if (nk_option_label(&ctx, "hard", op == HARD)) op = HARD;
//     /* custom widget pixel width */
//     nk_layout_row_begin(&ctx, NK_STATIC, 30, 2);
//     {
//         nk_layout_row_push(&ctx, 50);
//         nk_label(&ctx, "Volume:", NK_TEXT_LEFT);
//         nk_layout_row_push(&ctx, 110);
//         nk_slider_float(&ctx, 0, &value, 1.0f, 0.1f);
//     }
//     nk_layout_row_end(&ctx);
// }
// nk_end(&ctx);

// GdiFont* font;
// struct nk_context *ctx;
// WNDCLASSW wc;
// ATOM atom;
// RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
// DWORD style = WS_OVERLAPPEDWINDOW;
// DWORD exstyle = WS_EX_APPWINDOW;
// HWND wnd;
// HDC dc;
// int running = 1;
// int needs_refresh = 1;
// /* Win32 */
// memset(&wc, 0, sizeof(wc));
// wc.style = CS_DBLCLKS;
// wc.lpfnWndProc = WindowProc;
// wc.hInstance = GetModuleHandleW(0);
// wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
// wc.hCursor = LoadCursor(NULL, IDC_ARROW);
// wc.lpszClassName = L"NuklearWindowClass";
// atom = RegisterClassW(&wc);
// AdjustWindowRectEx(&rect, style, FALSE, exstyle);
// wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"Nuklear GDI Demo",
//     style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
//     rect.right - rect.left, rect.bottom - rect.top,
//     NULL, NULL, wc.hInstance, NULL);
// dc = GetDC(wnd);
// /* GUI */
// font = nk_gdifont_create("Arial", 14);
// ctx = nk_gdi_init(font, dc, WINDOW_WIDTH, WINDOW_HEIGHT);
// while (running)
// {
//     /* Input */
//     MSG msg;
//     nk_input_begin(ctx);
//     if (needs_refresh == 0) {
//         if (GetMessageW(&msg, NULL, 0, 0) <= 0)
//             running = 0;
//         else {
//             TranslateMessage(&msg);
//             DispatchMessageW(&msg);
//         }
//         needs_refresh = 1;
//     } else needs_refresh = 0;
//     while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
//         if (msg.message == WM_QUIT)
//             running = 0;
//         TranslateMessage(&msg);
//         DispatchMessageW(&msg);
//         needs_refresh = 1;
//     }
//     nk_input_end(ctx);
//     /* GUI */
//     if (nk_begin(ctx, "Demo", nk_rect(50, 50, 200, 200),
//         NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
//         NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
//     {
//         enum {EASY, HARD};
//         static int op = EASY;
//         static int property = 20;

//         nk_layout_row_static(ctx, 30, 80, 1);
//         if (nk_button_label(ctx, "button"))
//             fprintf(stdout, "button pressed\n");
//         nk_layout_row_dynamic(ctx, 30, 2);
//         if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
//         if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
//         nk_layout_row_dynamic(ctx, 22, 1);
//         nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);
//     }
//     nk_end(ctx);
//     /* -------------- EXAMPLES ---------------- */
//     #ifdef INCLUDE_CALCULATOR
//       calculator(ctx);
//     #endif
//     #ifdef INCLUDE_CANVAS
//       canvas(ctx);
//     #endif
//     #ifdef INCLUDE_OVERVIEW
//       overview(ctx);
//     #endif
//     #ifdef INCLUDE_NODE_EDITOR
//       node_editor(ctx);
//     #endif
//     /* ----------------------------------------- */
//     /* Draw */
//     nk_gdi_render(nk_rgb(30,30,30));
// }
// nk_gdifont_del(font);
// ReleaseDC(wnd, dc);
// UnregisterClassW(wc.lpszClassName, wc.hInstance);

 /* Initialize mutex. */
if(c89mtx_init(&mutex, c89mtx_plain) != c89thrd_success)
  fail_t("Failed to create mutex");
/* Create coroutine tasks. */
for(int i=0;i<NUM_TASKS;++i) {
  tasks[i] = create_fibonacci_task_t();
}
/* Create thread workers. */
for(size_t i=0;i<NUM_THREADS;++i) {
  if(c89thrd_create(&threads[i], thread_worker_t, NULL) != c89thrd_success)
    fail_t("Failed to create a thread");
}
/* Wait all threads to finish. */
for(size_t i=0;i<NUM_THREADS;++i) {
  if(c89thrd_join(threads[i], NULL) != c89thrd_success)
    fail_t("Failed to join a thread!");
}
for(int i=0;i<NUM_TASKS;++i) {
  if(tasks[i] != NULL)
    fail_t("A task is unfinished");
}
/* Destroy mutex. */
c89mtx_destroy(&mutex);
printf("Multithread test succeeded!\n");

// ma_result result_ma;
// ma_engine engine;
// result_ma = ma_engine_init(NULL, &engine);
// if (result_ma != MA_SUCCESS) {
//     return -1;
// }
// ma_engine_play_sound(&engine, "phone_ringing.wav", NULL);
// printf("Press Enter to quit...");
// getchar();
// ma_engine_uninit(&engine);

// int menuID, subMenuA, subMenuB;
// glutInitDisplayString( "stencil~2 rgb double depth>=16 samples" );
// glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
// glutInitWindowPosition( 100, 100 );
// glutInit(&argv,argc);
// subMenuA = glutCreateMenu( SampleMenu );
// glutAddMenuEntry( "Sub menu A1 (01)", 1 );
// glutAddMenuEntry( "Sub menu A2 (02)", 2 );
// glutAddMenuEntry( "Sub menu A3 (03)", 3 );
// subMenuB = glutCreateMenu( SampleMenu );
// glutAddMenuEntry( "Sub menu B1 (04)", 4 );
// glutAddMenuEntry( "Sub menu B2 (05)", 5 );
// glutAddMenuEntry( "Sub menu B3 (06)", 6 );
// glutAddSubMenu( "Going to sub menu A", subMenuA );
// menuID = glutCreateMenu( SampleMenu );
// glutAddMenuEntry( "Entry one",   1 );
// glutAddMenuEntry( "Entry two",   2 );
// glutAddMenuEntry( "Entry three", 3 );
// glutAddMenuEntry( "Entry four",  4 );
// glutAddMenuEntry( "Entry five",  5 );
// glutAddSubMenu( "Enter sub menu A", subMenuA );
// glutAddSubMenu( "Enter sub menu B", subMenuB );
// glutCreateWindow( "Hello world!" );
// glutDisplayFunc( SampleDisplay );
// glutReshapeFunc( SampleReshape );
// glutKeyboardFunc( SampleKeyboard );
// glutSpecialFunc( SampleSpecial );
// glutIdleFunc( SampleIdle );
// glutAttachMenu( GLUT_LEFT_BUTTON );
// glutInitWindowPosition( 200, 200 );
// glutCreateWindow( "I am not Jan B." );
// glutDisplayFunc( SampleDisplay );
// glutReshapeFunc( SampleReshape );
// glutKeyboardFunc( SampleKeyboard );
// glutSpecialFunc( SampleSpecial );
// glutIdleFunc( SampleIdle );
// glutAttachMenu( GLUT_LEFT_BUTTON );
// printf( "Testing game mode string parsing, don't panic!\n" );
// glutGameModeString( "320x240:32@100" );
// glutGameModeString( "640x480:16@72" );
// glutGameModeString( "1024x768" );
// glutGameModeString( ":32@120" );
// glutGameModeString( "Toudi glupcze, Danwin bedzie moj!" );
// glutGameModeString( "640x480:16@72" );
// glutEnterGameMode();
// glutDisplayFunc( SampleDisplay );
// glutReshapeFunc( SampleReshape );
// glutKeyboardFunc( SampleGameModeKeyboard );
// glutIdleFunc( SampleIdle );
// glutAttachMenu( GLUT_LEFT_BUTTON );
// printf( "current window is %ix%i+%i+%i",glutGet( GLUT_WINDOW_X ), glutGet( GLUT_WINDOW_Y ),glutGet( GLUT_WINDOW_WIDTH ), glutGet( GLUT_WINDOW_HEIGHT ));
// /*
//   * Enter the main FreeGLUT processing loop
//   */
// glutMainLoop();
// printf( "glutMainLoop() termination works fine!\n" );
// /*
//   * This is never reached in FreeGLUT. Is that good?
//   */
// return EXIT_SUCCESS;


int output = TestFunction(0, 1);
printf("before hook: %u\n", output);
testHook = CreateHook((void*)&TestFunction, (void*)&HookedTestFunction);
printf("size: %u\n", testHook.BytesToCopy);
int status = EnableHook(&testHook);
printf("status: %u\n", status);
printf("trampoline: 0x%p\n", testHook.Trampoline);
output = TestFunction(0, 1);
printf("after hook: %u\n", output);
status = DisableHook(&testHook);
printf("status: %u\n", status);
output = TestFunction(0, 1);
printf("disabled hook: %u\n", output);
status = EnableHook(&testHook);
printf("status: %u\n", status);
printf("trampoline: 0x%p\n", testHook.Trampoline);
output = TestFunction(0, 1);
printf("after hook: %u\n", output);


lua_State *L = luaL_newstate();
if(L == NULL)
	return -1;	
luaL_openlibs(L);
// luaopen_base(L);
// luaopen_table(L);
// luaopen_io(L);
// luaopen_string(L);
// luaopen_math(L);

//   lbind_register(L, "double", foobar);
//   lbind_dofile(L, "test.lua");
//   struct vars * args = lbind_args(L);
//   lbind_pushstring(args, "foobar");
//   struct vars * result = lbind_call(L, "hello", args);
//   assert(lbind_type(result, 0) == LT_INTEGER);
//   printf("sizeof 'foobar' = %d\n", lbind_tointeger(result, 0));

lua_register(L, "foobar", foobar);
int ret=luaL_dofile(L, "test.lua");
long value = rand()%20;
if(ret != 0)
{
	printf(" %s\n", lua_tostring(L, -1));
}
printf(" %d\n", value);
luaL_loadstring(L, "print 'hello world'");
lua_getglobal(L, "fact");
lua_pushinteger(L, value);
lua_pcall(L, 1, 1, 0);
lua_Number result = lua_tonumber(L, -1);
printf("Result from Lua Fact %ld", (long) result);
// lua_call(L, 0, 0);
lua_close(L);

// event_t task_end_ev;
// timer_event_t timer;

// time_nclk_t due, now;
// time_ms_t timeout;
// /* 
//   * 定义几个任务，栈大小128字节，事件优先级为LOWER_GROUP_PRIORITY
//   */
// TASK_DEFINE(task1, 128, LOWER_GROUP_PRIORITY);
// TASK_DEFINE(task2, 128, LOWER_GROUP_PRIORITY);
// TASK_DEFINE(task3, 128, LOWER_GROUP_PRIORITY);
// TASK_DEFINE(task4, 128, LOWER_GROUP_PRIORITY);
// TASK_DEFINE(task5, 128, LOWER_GROUP_PRIORITY);
// /* 启动异步函数（任务）2 */
// task_start(&task1, async_func2);
// /* 开启多个异步函数（任务）2 */
// task_start(&task2, async_func2);
// /* 开启多个异步函数（任务）3 */
// task_start(&task3, async_func4);
// /* 开启多个异步函数（任务）4 */
// task_start(&task4, async_func4);
// /* 初始化一个事件 */
// event_init(&task_end_ev, on_task_end_lw, "Async function 1 end!\n", LOWER_GROUP_PRIORITY);
// /* 设置为task5结束通知事件 */
// task_end_wait(&task5, &task_end_ev);
// /* 带参数启动异步函数（任务）1 */
// task_start(&task5, async_func1, 5000, 15000);
// /* 初始化定时器 */
// timer_init(&timer, on_timer, "This is a timer event!\n", LOWER_GROUP_PRIORITY);
// /* 启动单独的定时器事件 */
// el_timer_start_ms(&timer, 1500);
// /* 运行atask内核 */
// while (1)
// {
//     el_schedule();
// }


 WSADATA wsaData;
struct sockaddr_in server_addr;
GUID GuidAcceptEx = WSAID_ACCEPTEX;
GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
DWORD dwBytes;
TASK_DEFINE(http_accept_task, 280, MIDDLE_GROUP_PRIORITY);
WSAStartup(MAKEWORD(2,2), &wsaData);
/* 创建IOCP端口 */
s_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
if (s_iocp == NULL)
{
    printf("Create IOCP failed! error:%d\n", (uint32_t)GetLastError());
    WSACleanup();
    return -1;
}
/* Win Socket初始化 */
s_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
if (s_sock == INVALID_SOCKET)
{
    printf("Create Socket failed! error:%d\n", (uint32_t)GetLastError());
    WSACleanup();
    return -1;
}
/* 添加Socket句柄到IOCP */
if (NULL == CreateIoCompletionPort((HANDLE)s_sock, s_iocp, 0, 0))
{
    printf("Add socket handle failed! error:%d\n", (uint32_t)GetLastError());
    closesocket(s_sock);
    WSACleanup();
    return -1;
}
/* 设置监听地址 */
memset(&server_addr, 0, sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
server_addr.sin_port = htons(8080);
/* 绑定 */
if (SOCKET_ERROR == bind(s_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)))
{
    printf("Bind server address failed! error:%d\n", WSAGetLastError());
    closesocket(s_sock);
    WSACleanup();
    return -1;
}
/* 监听 */
if (SOCKET_ERROR == listen(s_sock, SOMAXCONN))
{
    printf("Listen server address failed! error:%d\n", WSAGetLastError());
    closesocket(s_sock);
    WSACleanup();
    return -1;
}
/* 使用WSAIoctl将ACCEPTEX函数加载到内存中。
WSAIoctl函数是ioctlsocket()函数的扩展，该函数可以使用重叠的I/O。
该函数的第3到第6个参数是输入和输出缓冲区，我们将指针传递给ACCEPTEX函数。
这样我们就可以直接调用ACCEPTEX函数，而不是引用mswsock.lib库。*/
if (SOCKET_ERROR == WSAIoctl(s_sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
          &GuidAcceptEx, sizeof (GuidAcceptEx), 
          &lpfnAcceptEx, sizeof (lpfnAcceptEx), 
          &dwBytes, NULL, NULL))
{
    printf("WSAIoctl failed with error: %u\n", WSAGetLastError());
    closesocket(s_sock);
    WSACleanup();
    return -1;
}
if (SOCKET_ERROR == WSAIoctl(s_sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
          &GuidGetAcceptExSockAddrs, sizeof (GuidGetAcceptExSockAddrs), 
          &lpfnGetAcceptExSockAddrs, sizeof (lpfnGetAcceptExSockAddrs), 
          &dwBytes, NULL, NULL))
{
    printf("WSAIoctl failed with error: %u\n", WSAGetLastError());
    closesocket(s_sock);
    WSACleanup();
    return -1;
}
/* 启动http accept任务 */
task_start(&http_accept_task, http_accept_task_handler);
/* 运行带IOCP的atask事件循环 */
iocp_atask_run(s_iocp);
closesocket(s_sock);
WSACleanup();

// int i_f, test_aa = 1, test_srgb = 1;
// unsigned int mode = GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE;
// for(i_f=1; i_f<argc; i_f++) {
// 	if(strcmp(argv[i_f], "-ms") == 0) {
// 		test_aa = 1;
// 	} else if(strcmp(argv[i_f], "-srgb") == 0) {
// 		test_srgb = 1;
// 	}
// }
// if(test_aa) mode |= GLUT_MULTISAMPLE;
// if(test_srgb) mode |= GLUT_SRGB;
// glutInit(&argc, argv);
// glutInitWindowSize(800, 600);
// glutInitDisplayMode(mode);
// glutCreateWindow("miniglut test");
// glutDisplayFunc(display);
// glutReshapeFunc(reshape);
// glutKeyboardFunc(keypress);
// glutMouseFunc(mouse);
// glutMotionFunc(motion);
// glutSpaceballMotionFunc(sball_motion);
// glutSpaceballRotateFunc(sball_rotate);
// glutSpaceballButtonFunc(sball_button);
// glEnable(GL_DEPTH_TEST);
// glEnable(GL_CULL_FACE);
// glEnable(GL_LIGHTING);
// glEnable(GL_LIGHT0);
// if(test_aa) {
//   glEnable(GL_MULTISAMPLE);
// }
// if(test_srgb) {
//   glEnable(GL_FRAMEBUFFER_SRGB);
// }
// torus = glGenLists(1);
// glNewList(torus, GL_COMPILE);
// glutSolidTorus(0.3, 1, 16, 24);
// glEndList();
// cone = glGenLists(1);
// glNewList(cone, GL_COMPILE);
// glutSolidCone(1.1, 2, 16, 2);
// glEndList();
// sphere = glGenLists(1);
// glNewList(sphere, GL_COMPILE);
// glutSolidSphere(0.4, 16, 8);
// glEndList();
// teapot = glGenLists(1);
// glNewList(teapot, GL_COMPILE);
// glutSolidTeapot(1.0);
// glEndList();
// glutMainLoop();

#ifdef WIN32
	system("pause");
#endif	
  // getchar();
	return 0;
	// return EXIT_SUCCESS;
}
