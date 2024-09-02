#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <tchar.h>
#include <math.h>
#include "miniglut.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
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


// // Coroutine entry function.
// void coro_entry(mco_coro* co) {
//   printf("coroutine 1\n");
//   mco_yield(co);
//   printf("coroutine 2\n");
// }

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


#include "bp.h"
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



#include "cc_array.h"
#include "cc_array_sort.h"
#include "cc_binary.h"
#include "cc_common.h"
#include "cc_hash_map.h"
#include "cc_list_map.h"
#include "cc_list.h"
#include "cc_ring.h"
#include "cc_stack.h"
#include <assert.h>



static const char *sample = "A quick brown fox jumps over the lazy dog.";

static int cmp_char(char *left, char *right)
{
	// return *left - *right;
	return -(*left - *right);
}

static void char_array_display(struct cc_array *chars, const char *prefix)
{
	struct cc_array_iter iter;
	char *tmp;

	cc_debug_print("%s", prefix);
	assert(!cc_array_iter_init(&iter, chars));
	while (!cc_iter_next(&iter, &tmp, NULL))
		cc_debug_print("%c", *tmp);
}

struct blah {
	const char *name;
	int age;
};

/// define a global array to simplify the initializing of the testing generic_sequence.
static const struct blah people[] = {{"Harry", 10}, {"Albus", 109}, {"Severus", 50}};

static void blah_display(struct blah *self)
{
	cc_debug_print("<name %s, age: %d> ", self->name, self->age);
}

static void blah_array_display(struct cc_array *blahs, const char *prefix)
{
	struct cc_array_iter iter;
	struct blah *tmp;

	cc_debug_print("%s", prefix);
	assert(!cc_array_iter_init(&iter, blahs));
	while (!cc_iter_next(&iter, &tmp, NULL))
		blah_display(tmp);
}

static int cmp_name(struct blah *left, struct blah *right)
{
	// return strcmp(left->name, right->name);
	return -strcmp(left->name, right->name);
}

static int cmp_age(struct blah *left, struct blah *right)
{
	// return left->age - right->age;
	return -(left->age - right->age);
}

int show_tree_elements(struct cc_binary *tree)
{
	struct cc_binary_iter *iter;
	size_t *tmp, index;
	assert(!cc_binary_iter_new(&iter, tree, CC_TRAVERSE_DEPTH_LEFT));
	while (!cc_iter_next(iter, &tmp, &index))
		cc_debug_print("(%d)%u, ", index, *tmp);
	cc_debug_print("\n");
	assert(!cc_binary_iter_delete(iter));
	return 0;
}

static int print_1(void *data)
{
	return cc_debug_print("%ld", (uintptr_t)data);
}

// clang-format off
enum blah_node_type {NUMBER, OP};
// clang-format on
struct blah_node {
	enum blah_node_type type;
	union {
		char op_sign;
		int number;
	};
};

int blah_node_new_number(struct blah_node **self, int number)
{
	struct blah_node *tmp;
	tmp = malloc(sizeof(*tmp));
	if (tmp == NULL)
		return 1;
	tmp->type = NUMBER;
	tmp->number = number;
	*self = tmp;
	return 0;
}

int blah_node_new_op(struct blah_node **self, char op_sign)
{
	struct blah_node *tmp;
	tmp = malloc(sizeof(*tmp));
	if (tmp == NULL)
		return 1;
	tmp->type = OP;
	tmp->op_sign = op_sign;
	*self = tmp;
	return 0;
}

int blah_node_print(struct blah_node *node)
{
	if (node == NULL)
		return 0;
	if (node->type == NUMBER)
		return cc_debug_print("%d", node->number);
	else
		return cc_debug_print("<%c>", node->op_sign);
}

int blah_node_delete(struct blah_node *self)
{
	free(self);
	return 0;
}

struct parser {
	char *input;
	struct cc_binary *root;
};

int parser_step(struct parser *self, int *error)
{
	struct blah_node *new_node;
	char c;
	*error = 0;
	c = *self->input++;
	if (c == '\0')
		return 1;
	if (c >= '0' && c <= '9')
		blah_node_new_number(&new_node, c - '0');
	else
		blah_node_new_op(&new_node, c);
	if (new_node == NULL) {
		*error = 2;
		goto fail;
	}
	if (cc_binary_insert_left(self->root, new_node)) {
		*error = 3;
		goto fail;
	}
	return 0;
fail:
	return 1;
}

int parser_parse(struct parser *self, struct cc_binary **result)
{
	int error;
	if (try_reset_double_p(result))
		return 255;
	while (!parser_step(self, &error) && !error)
		;
	*result = self->root;
	return error;
}

int parser_new(struct parser **self, char *input)
{
	struct parser *tmp;
	int code = 0;
	if (input == NULL) {
		code = 1;
		goto fail1;
	}
	tmp = malloc(sizeof(*tmp));
	if (tmp == NULL) {
		code = 2;
		goto fail1;
	}
	tmp->input = input;
	if (cc_binary_new(&tmp->root, NULL, NULL)) {
		code = 3;
		goto fail2;
	}
	*self = tmp;
	return 0;
fail2:
	free(tmp);
fail1:
	return code;
}

/// Delete the parser along with the result cc_binary tree.
int parser_delete(struct parser *self)
{
	struct cc_binary_iter *iter;
	struct blah_node **tmp;
	if (cc_binary_iter_new(&iter, self->root->left, CC_TRAVERSE_DEPTH_LEFT))
		return 1;
	while (!cc_iter_next(iter, &tmp, NULL))
		assert(!blah_node_delete(*tmp));
	if (cc_binary_iter_delete(iter))
		return 2;
	if (cc_binary_delete(self->root))
		return 3;
	free(self);
	return 0;
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



#include "luabind.h"
static void foobar(struct vars *input, struct vars *output) {
	assert(lbind_type(input, 0) == LT_INTEGER);
	int a = lbind_tointeger(input, 0);
	lbind_pushinteger(output, a*2);
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

  lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	lbind_register(L, "double", foobar);
	lbind_dofile(L, "test.lua");
	struct vars * args = lbind_args(L);
	lbind_pushstring(args, "foobar");
	struct vars * result = lbind_call(L, "hello", args);
	assert(lbind_type(result, 0) == LT_INTEGER);
	printf("sizeof 'foobar' = %d\n", lbind_tointeger(result, 0));
	lua_close(L);

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


struct cc_array array1;
unsigned char buffer1[10];
assert(!cc_array_init(&array1, buffer1, 10, 1));
for (int i = 0; i < 10; i++)
  assert(!cc_array_set(&array1, i, (void *)&sample[i]));
char_array_display(&array1, "\nbefore sort: ");
/// sort the sequence
// assert(!cc_array_sort_bubble(&array, (cc_cmp_fn)cmp_char));
assert(!cc_array_sort_quick(&array1, (cc_cmp_fn_t)cmp_char));
// assert(!strncmp((const char *)array.buffer, "  Abcikqru", 10));
assert(!strncmp((const char *)array1.buffer, "urqkicbA  ", 10));
char_array_display(&array1, "\nafter sort: ");
putchar('\n');


struct cc_array array2;
unsigned char buffer2[3 * sizeof(struct blah)];
struct blah tmp;
assert(!cc_array_init(&array2, buffer2, 3, sizeof(struct blah)));
/// Pushing values to generic sequence
for (int i = 0; i < 3; i++)
  assert(!cc_array_set(&array2, i, (void *)&people[i]));
blah_array_display(&array2, "\nbefore sort:\n");
assert(!cc_array_sort_bubble(&array2, (cc_cmp_fn_t)cmp_name));
blah_array_display(&array2, "\nafter sort by name:\n");
assert(!cc_array_get(&array2, 0, &tmp));
assert(tmp.age == 50);
assert(!cc_array_sort_bubble(&array2, (cc_cmp_fn_t)cmp_age));
blah_array_display(&array2, "\nafter sort by age:\n");
assert(!cc_array_get(&array2, 0, &tmp));
assert(tmp.age == 109);
putchar('\n');


struct cc_binary *root;
assert(!cc_binary_new(&root, NULL, NULL));
assert(!cc_binary_insert_left(root, (void **)1));
assert(!cc_binary_insert_left(root, (void **)2));
assert(!cc_binary_insert_left(root, (void **)3));
assert(!cc_binary_insert_left(root, (void **)4));
assert(!cc_binary_insert_right(root, (void **)5));
assert(!cc_binary_insert_right(root, (void **)6));
assert(!cc_binary_insert_right(root, (void **)7));
assert(root->left->data == (void *)4);
assert(root->left->left->data == (void *)3);
assert(root->left->left->left->data == (void *)2);
assert(root->left->left->left->left->data == (void *)1);
assert(root->right->data == (void *)7);
assert(root->right->right->data == (void *)6);
assert(root->right->right->right->data == (void *)5);
assert(!cc_binary_print(root, 0, print_1));
show_tree_elements(root);
assert(!cc_binary_rotate_left(&root->right));
assert(!cc_binary_rotate_left(&root->right));
assert(cc_binary_rotate_left(&root->right));
assert(!cc_binary_rotate_right(&root->left));
assert(!cc_binary_rotate_right(&root->left));
assert(!cc_binary_rotate_right(&root->left));
assert(cc_binary_rotate_right(&root->left));
assert(root->left->data == (void *)1);
assert(root->left->right->data == (void *)2);
assert(root->left->right->right->data == (void *)3);
assert(root->left->right->right->right->data == (void *)4);
assert(root->right->data == (void *)5);
assert(root->right->left->data == (void *)6);
assert(root->right->left->left->data == (void *)7);
assert(!cc_binary_print(root, 0, print_1));
show_tree_elements(root);
assert(!cc_binary_delete(root));


struct parser *parser;
struct cc_binary *result_b;
assert(!parser_new(&parser, expr1));
cc_debug_print("parsing expression: \"%s\"\n", expr1);
assert(!parser_parse(parser, &result_b));
assert(!cc_binary_rotate_right(&result_b->left));
assert(((struct blah_node *)result_b->left->data)->op_sign == '+');
cc_binary_print(result_b, 0, (cc_simple_fn_1_t)blah_node_print);
assert(!parser_delete(parser));


struct cc_hash_map *map;
struct cc_hash_map_iter iter;
struct cc_map_item *item_tmp;
uintptr_t tmp_t;
size_t index;
assert(!cc_hash_map_new(&map,10, NULL, NULL));
// assert(!cc_hash_map_print(map, "\n"));
assert(!cc_hash_map_set(map, (void *)11, (void *)101));
assert(!cc_hash_map_set(map, (void *)12, (void *)102));
assert(!cc_hash_map_set(map, (void *)19, (void *)109));
assert(!cc_map_set(map, (void *)22, (void *)202));
assert(!cc_map_set(map, (void *)32, (void *)302));
assert(!cc_map_set(map, (void *)41, (void *)401));
assert(!cc_map_set(map, (void *)52, (void *)502));
assert(!cc_map_set(map, (void *)63, (void *)603));
assert(!cc_map_set(map, (void *)66, (void *)606));
assert(cc_map_set_new(map, (void *)66, (void *)607));
assert(!cc_map_set(map, (void *)66, (void *)608));
assert(!cc_hash_map_print(map, "\n"));
assert(!cc_hash_map_get(map, (void *)11, (void **)&tmp_t));
assert(tmp_t == 101);
assert(!cc_map_get(map, (void *)19, (void **)&tmp_t));
assert(tmp_t == 109);
assert(!cc_map_get(map, (void *)12, (void **)&tmp_t));
assert(tmp_t == 102);
assert(!cc_map_del(map, (void *)19, (void **)&tmp_t));
assert(tmp_t == 109);
assert(!cc_hash_map_print(map, "\n"));
assert(!cc_hash_map_iter_init(&iter, map));
while (!cc_iter_next(&iter, &item_tmp, &index))
  cc_debug_print("(%d){%zu -> %zu} ", index, item_tmp->key, item_tmp->value);
assert(!cc_hash_map_delete(map));


struct cc_hash_map *map_cc;
uintptr_t tmp_cc;
/// When using `char *`s as keys, you should use `strcmp` as the `cmp` function.
// assert(!cc_hash_map_new(&map, 20, NULL, cc_str_hash_fn_bkdr));
assert(!cc_hash_map_new(&map_cc, 20, (cc_cmp_fn_t)strcmp, cc_str_hash_fn_bkdr));
// assert(!cc_hash_map_new(&map, 20, (cc_cmp_fn_t)strcmp, cc_str_hash_fn_simple));
// assert(!cc_hash_map_new(&map, 20, (cc_cmp_fn_t)strcmp, NULL));
assert(map_cc != NULL);
// assert(!cc_hash_map_print(map, "\n"));
assert(!cc_map_set(map_cc, "lazy_dog", (void *)1));
assert(!cc_map_set(map_cc, "quick_fox", (void *)2));
assert(!cc_map_set(map_cc, "dumb_bunny", (void *)3));
assert(!cc_map_set(map_cc, "sneaky_fox", (void *)4));
assert(!cc_map_set(map_cc, "slow_sloth", (void *)5));
assert(!cc_map_set(map_cc, "strong_bull", (void *)6));
assert(!cc_map_set(map_cc, "blah1", (void *)15));
assert(!cc_map_set(map_cc, "blah2", (void *)16));
assert(!cc_map_set(map_cc, "blah3", (void *)17));
assert(!cc_map_set(map_cc, "blah4", (void *)18));
assert(!cc_map_set(map_cc, "blah5", (void *)19));
assert(!cc_map_set(map_cc, "blah6", (void *)20));
assert(!cc_map_set(map_cc, "blah7", (void *)31));
assert(!cc_map_set(map_cc, "blah8", (void *)32));
assert(!cc_map_set(map_cc, "blah9", (void *)33));
assert(!cc_map_set(map_cc, "blah10", (void *)34));
assert(!cc_map_set(map_cc, "blah11", (void *)35));
assert(!cc_map_set(map_cc, "blah12", (void *)36));
assert(!cc_map_set(map_cc, s1, (void *)40));
assert(!cc_map_set(map_cc, s2, (void *)41));
assert(!cc_map_set(map_cc, s3, (void *)42));
assert(!cc_hash_map_print(map_cc, "\n"));
assert(!cc_map_get(map_cc, "lazy_dog", (void **)&tmp_cc));
assert(tmp_cc == 1);
assert(!cc_map_get(map_cc, "strong_bull", (void **)&tmp_cc));
assert(tmp_cc == 6);
assert(!cc_map_del(map_cc, s3, (void **)&tmp_cc));
assert(!cc_hash_map_print(map_cc, "\n"));
assert(!cc_hash_map_delete(map_cc));


struct cc_list_map *map_list;
/// Caution: The `&tmp` is used as `void *` to accapt data.
/// So the size of `tmp` should NOT be smaller than `uintptr_t`, or data nearby would be overwritten.
uintptr_t tmp_list;
assert(!cc_list_map_new(&map_list, NULL));
assert(!cc_list_map_set(map_list, (void *)1, (void *)'a'));
assert(!cc_map_set(map_list, (void *)2, (void *)'b'));
assert(!cc_map_set(map_list, (void *)9, (void *)'c'));
assert(!cc_list_map_print(map_list, "\n"));
assert(!cc_list_map_get(map_list, (void *)1, (void **)&tmp_list));
assert(tmp_list == 'a');
assert(!cc_map_get(map_list, (void *)9, (void **)&tmp_list));
assert(tmp_list == 'c');
assert(!cc_map_get(map_list, (void *)2, (void **)&tmp_list));
assert(tmp_list == 'b');
assert(!cc_map_del(map_list, (void *)2, (void **)&tmp_list));
assert(tmp_list == 'b');
assert(!cc_list_map_print(map_list, "\n"));
assert(!cc_list_map_delete(map_list));


struct cc_list *list_tmp;
uintptr_t i_tmp;
assert(!cc_list_new(&list_tmp));
for (i_tmp = 0; i_tmp < 10; i_tmp++) {
  assert(!cc_list_append(list_tmp, (void *)i_tmp));
  assert(list_tmp->root.size == i_tmp + 1);
}
assert(cc_list_insert(list_tmp, 11, (void *)55));
assert(!cc_list_insert(list_tmp, 10, (void *)66));
assert(!cc_list_insert(list_tmp, 0, (void *)77));
assert(!cc_list_insert(list_tmp, 3, (void *)88));
cc_list_print(list_tmp, 1);
assert(cc_list_remove(list_tmp, 13, (void **)&i_tmp));
assert(!cc_list_remove(list_tmp, 12, (void **)&i_tmp));
assert(i_tmp == 66);
assert(!cc_list_remove(list_tmp, 3, (void **)&i_tmp));
assert(i_tmp == 88);
assert(!cc_list_remove(list_tmp, 0, (void **)&i_tmp));
assert(i_tmp == 77);
cc_list_print(list_tmp, 1);
assert(!cc_list_delete(list_tmp));

struct cc_list *list_l;
uintptr_t tmp_l;
assert(!cc_list_new(&list_l));
assert(cc_list_remove(list_l, 0, (void **)&tmp_l));
assert(!cc_list_insert(list_l, 0, (void *)1));
assert(!cc_list_remove(list_l, 0, (void **)&tmp_l));
assert(tmp_l == 1);
assert(!cc_list_insert(list_l, 0, (void *)1));
assert(!cc_list_insert(list_l, 0, (void *)2));
assert(!cc_list_insert(list_l, 0, (void *)3));
assert(!cc_list_remove(list_l, 0, (void **)&tmp_l));
assert(tmp_l == 3);
assert(!cc_list_remove(list_l, 0, (void **)&tmp_l));
assert(tmp_l == 2);
assert(!cc_list_remove(list_l, 0, (void **)&tmp_l));
assert(tmp_l == 1);
assert(cc_list_remove(list_l, 0, (void **)&tmp_l));
assert(!cc_list_delete(list_l));


struct cc_ring *ring_r;
char i_r, tmp_r;
assert(!cc_ring_new(&ring_r, 8, sizeof(char)));
assert(cc_ring_space(ring_r) == 8);
for (i_r = 0; i_r < 8; i_r++) {
  assert(!cc_ring_append(ring_r, &i_r));
  assert(cc_ring_space(ring_r) == (size_t)(8 - i_r - 1));
}
assert(cc_ring_space(ring_r) == 0);
/// this should fail since ring is full
assert(cc_ring_append(ring_r, &i_r));
for (i_r = 0; i_r < 8; i_r++) {
  assert(!cc_ring_shift(ring_r, &tmp_r));
  assert(cc_ring_space(ring_r) == (size_t)i_r + 1);
}
assert(cc_ring_space(ring_r) == 8);
/// this should fail since ring is empty
assert(cc_ring_shift(ring_r, &tmp_r));
assert(!cc_ring_delete(ring_r));


struct cc_array array_s;
struct cc_stack stack_s, *p_stack_s;
// unsigned char buffer[10];
char i_s, tmp_s;
assert(!cc_stack_new(&p_stack_s, 10, sizeof(char)));
// assert(!cc_array_init(&array, buffer, 10, 1));
// assert(!cc_stack_init(&stack, &array));
// p_stack = &stack;
for (i_s = 0; i_s < 10; i_s++)
  assert(!cc_stack_push(p_stack_s, &i_s));
for (i_s = 9; i_s >= 0; i_s--) {
  assert(!cc_stack_pop(p_stack_s, &tmp_s));
  assert(tmp_s == i_s);
}
assert(!cc_stack_delete(p_stack_s));


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


int i_f, test_aa = 1, test_srgb = 1;
unsigned int mode = GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE;
// for(i_f=1; i_f<argc; i_f++) {
// 	if(strcmp(argv[i_f], "-ms") == 0) {
// 		test_aa = 1;
// 	} else if(strcmp(argv[i_f], "-srgb") == 0) {
// 		test_srgb = 1;
// 	}
// }
if(test_aa) mode |= GLUT_MULTISAMPLE;
if(test_srgb) mode |= GLUT_SRGB;
glutInit(&argc, argv);
glutInitWindowSize(800, 600);
glutInitDisplayMode(mode);
glutCreateWindow("miniglut test");
glutDisplayFunc(display);
glutReshapeFunc(reshape);
glutKeyboardFunc(keypress);
glutMouseFunc(mouse);
glutMotionFunc(motion);
glutSpaceballMotionFunc(sball_motion);
glutSpaceballRotateFunc(sball_rotate);
glutSpaceballButtonFunc(sball_button);
glEnable(GL_DEPTH_TEST);
glEnable(GL_CULL_FACE);
glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);
if(test_aa) {
  glEnable(GL_MULTISAMPLE);
}
if(test_srgb) {
  glEnable(GL_FRAMEBUFFER_SRGB);
}
torus = glGenLists(1);
glNewList(torus, GL_COMPILE);
glutSolidTorus(0.3, 1, 16, 24);
glEndList();
cone = glGenLists(1);
glNewList(cone, GL_COMPILE);
glutSolidCone(1.1, 2, 16, 2);
glEndList();
sphere = glGenLists(1);
glNewList(sphere, GL_COMPILE);
glutSolidSphere(0.4, 16, 8);
glEndList();
teapot = glGenLists(1);
glNewList(teapot, GL_COMPILE);
glutSolidTeapot(1.0);
glEndList();
glutMainLoop();

#ifdef WIN32
	system("pause");
#endif	
  // getchar();
	return 0;
	// return EXIT_SUCCESS;
}
