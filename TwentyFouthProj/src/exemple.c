/*
 * GENANN - Minimal C Artificial Neural Network
 *
 * Copyright (c) 2015-2018 Lewis Van Winkle
 *
 * http://CodePlea.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */

#include "genann.h"
#include "minctest.h"
#include <stdio.h>
#include <minimath.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h> 
#include <time.h> 
#include <Windows.h>
#include <malloc.h>
#include <tchar.h>
#include <assert.h>
#include <locale.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "glfw_tinycthread.h"
#include "glfw_getopt.h"
#ifdef NANOVG_GLEW
#  include <GL/glew.h>
#endif
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#include "nanovg.h"
#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg_gl.h"

#include "mpool.h"
#include "m_vector.h"
#include "thpool.h"
#include "cJSON.h"
#include "logger.h"
// #include "c89thread.h"
// #include "c89atomic.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_GDI_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_gdi.h"

int bsz = 8;
int times = 100000;
int runtimes = 100;

void basic() {
    genann *ann = genann_init(1, 0, 0, 1);

    lequal(ann->total_weights, 2);
    double a;


    a = 0;
    ann->weight[0] = 0;
    ann->weight[1] = 0;
    lfequal(0.5, *genann_run(ann, &a));

    a = 1;
    lfequal(0.5, *genann_run(ann, &a));

    a = 11;
    lfequal(0.5, *genann_run(ann, &a));

    a = 1;
    ann->weight[0] = 1;
    ann->weight[1] = 1;
    lfequal(0.5, *genann_run(ann, &a));

    a = 10;
    ann->weight[0] = 1;
    ann->weight[1] = 1;
    lfequal(1.0, *genann_run(ann, &a));

    a = -10;
    lfequal(0.0, *genann_run(ann, &a));

    genann_free(ann);
}


void xor() {
    genann *ann = genann_init(2, 1, 2, 1);
    ann->activation_hidden = genann_act_threshold;
    ann->activation_output = genann_act_threshold;

    lequal(ann->total_weights, 9);

    /* First hidden. */
    ann->weight[0] = .5;
    ann->weight[1] = 1;
    ann->weight[2] = 1;

    /* Second hidden. */
    ann->weight[3] = 1;
    ann->weight[4] = 1;
    ann->weight[5] = 1;

    /* Output. */
    ann->weight[6] = .5;
    ann->weight[7] = 1;
    ann->weight[8] = -1;


    double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    double output[4] = {0, 1, 1, 0};

    lfequal(output[0], *genann_run(ann, input[0]));
    lfequal(output[1], *genann_run(ann, input[1]));
    lfequal(output[2], *genann_run(ann, input[2]));
    lfequal(output[3], *genann_run(ann, input[3]));

    genann_free(ann);
}


void backprop() {
    genann *ann = genann_init(1, 0, 0, 1);

    double input, output;
    input = .5;
    output = 1;

    double first_try = *genann_run(ann, &input);
    genann_train(ann, &input, &output, .5);
    double second_try = *genann_run(ann, &input);
    lok(fabs(first_try - output) > fabs(second_try - output));

    genann_free(ann);
}


void train_and() {
    double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    double output[4] = {0, 0, 0, 1};

    genann *ann = genann_init(2, 0, 0, 1);

    int i, j;

    for (i = 0; i < 50; ++i) {
        for (j = 0; j < 4; ++j) {
            genann_train(ann, input[j], output + j, .8);
        }
    }

    ann->activation_output = genann_act_threshold;
    lfequal(output[0], *genann_run(ann, input[0]));
    lfequal(output[1], *genann_run(ann, input[1]));
    lfequal(output[2], *genann_run(ann, input[2]));
    lfequal(output[3], *genann_run(ann, input[3]));

    genann_free(ann);
}


void train_or() {
    double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    double output[4] = {0, 1, 1, 1};

    genann *ann = genann_init(2, 0, 0, 1);
    genann_randomize(ann);

    int i, j;

    for (i = 0; i < 50; ++i) {
        for (j = 0; j < 4; ++j) {
            genann_train(ann, input[j], output + j, .8);
        }
    }

    ann->activation_output = genann_act_threshold;
    lfequal(output[0], *genann_run(ann, input[0]));
    lfequal(output[1], *genann_run(ann, input[1]));
    lfequal(output[2], *genann_run(ann, input[2]));
    lfequal(output[3], *genann_run(ann, input[3]));

    genann_free(ann);
}



void train_xor() {
    double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    double output[4] = {0, 1, 1, 0};

    genann *ann = genann_init(2, 1, 2, 1);

    int i, j;

    for (i = 0; i < 500; ++i) {
        for (j = 0; j < 4; ++j) {
            genann_train(ann, input[j], output + j, 3);
        }
        /* printf("%1.2f ", xor_score(ann)); */
    }

    ann->activation_output = genann_act_threshold;
    lfequal(output[0], *genann_run(ann, input[0]));
    lfequal(output[1], *genann_run(ann, input[1]));
    lfequal(output[2], *genann_run(ann, input[2]));
    lfequal(output[3], *genann_run(ann, input[3]));

    genann_free(ann);
}



void persist() {
    genann *first = genann_init(1000, 5, 50, 10);

    FILE *out = fopen("persist.txt", "w");
    genann_write(first, out);
    fclose(out);


    FILE *in = fopen("persist.txt", "r");
    genann *second = genann_read(in);
    fclose(in);

    lequal(first->inputs, second->inputs);
    lequal(first->hidden_layers, second->hidden_layers);
    lequal(first->hidden, second->hidden);
    lequal(first->outputs, second->outputs);
    lequal(first->total_weights, second->total_weights);

    int i;
    for (i = 0; i < first->total_weights; ++i) {
        lok(first->weight[i] == second->weight[i]);
    }

    genann_free(first);
    genann_free(second);
}


void copy() {
    genann *first = genann_init(1000, 5, 50, 10);

    genann *second = genann_copy(first);

    lequal(first->inputs, second->inputs);
    lequal(first->hidden_layers, second->hidden_layers);
    lequal(first->hidden, second->hidden);
    lequal(first->outputs, second->outputs);
    lequal(first->total_weights, second->total_weights);

    int i;
    for (i = 0; i < first->total_weights; ++i) {
        lfequal(first->weight[i], second->weight[i]);
    }

    genann_free(first);
    genann_free(second);
}


void sigmoid() {
    double i = -20;
    const double max = 20;
    const double d = .0001;

    while (i < max) {
        lfequal(genann_act_sigmoid(NULL, i), genann_act_sigmoid_cached(NULL, i));
        i += d;
    }
}



void test_pool()
{
    MemPool *pool = pool_create(128,10);
    char    *cp;
    int     i = 0;
    for ( ; i < 15; ++i )
    {
        char *c = (char*)m_malloc(pool);

        sprintf(c,"%d\tThis Is My Pool\n",i+1);
        printf(c);
    }

    cp = m_malloc(pool);
    m_free(pool,&cp);
    m_free(pool,&cp);

    cp = m_malloc(pool);

    pool_release(pool);
}

void * f1(void *arg)
{
    printf("==================Func 1\n");
    return NULL;
}
void * f2(void* arg)
{
    printf("==================Func 2\n");
    return NULL;
}
void test_malloc()
{
    clock_t       start, end;
    int i , j ;


    start = clock();
    for( j = 0 ;j < runtimes; ++j)
    for ( i = 0 ; i < times; ++i)
    {
       void *f = malloc(bsz);
       free(f);
    }
    end = clock();
    fprintf(stdout,"malloc Time Ellapsed:%d\n",end-start);
}

//void test_boost()
//{
//    boost::pool<>   bpool(bsz);
//    clock_t start, end;
//    int i , j;

//    start = clock();
//    for( j = 0 ;j < runtimes; ++j)
//    for ( i = 0 ; i < times; ++i)
//    {
//       void *f = bpool.malloc();
//        bpool.free(f,bsz);
//    }
//    end = clock();
//    fprintf(stdout,"Boost Pool Time Ellapsed:%dms\n",end-start);

//}

void test_mempool()
{
    MemPool       *block = pool_create(bsz,1);
    clock_t       start, end;
    int i , j ;


    start = clock();
    for( j = 0 ;j < runtimes; ++j)
    for ( i = 0 ; i < times; ++i)
    {
       void *f = m_malloc(block);
       m_free(block,&f);
    }
    end = clock();
    fprintf(stdout,"Block Pool Time Ellapsed:%d\n",end-start);
    pool_release(block);
}

//void test_threadpool()
//{
//    thPool  *pool = thpool_create(3);
//    Task    t1 = {f1,NULL}, t2 = {f2,NULL};

//    add_Task(pool,&t2);
//    add_Task(pool,&t2);
//    add_Task(pool,&t1);
//    add_Task(pool,&t1);
//    add_Task(pool,&t1);
//    add_Task(pool,&t2);
//    Sleep(3000);
//    thpool_release(pool);
//}


/* Used by some code below as an example datatype. */
struct record
{
    const char *precision;
    double lat;
    double lon;
    const char *address;
    const char *city;
    const char *state;
    const char *zip;
    const char *country;
};


/* Create a bunch of objects as demonstration. */
static int print_preallocated(cJSON *root)
{
    /* declarations */
    char *out = NULL;
    char *buf = NULL;
    char *buf_fail = NULL;
    size_t len = 0;
    size_t len_fail = 0;

    /* formatted print */
    out = cJSON_Print(root);

    /* create buffer to succeed */
    /* the extra 5 bytes are because of inaccuracies when reserving memory */
    len = strlen(out) + 5;
    buf = (char*)malloc(len);
    if (buf == NULL)
    {
        printf("Failed to allocate memory.\n");
        exit(1);
    }

    /* create buffer to fail */
    len_fail = strlen(out);
    buf_fail = (char*)malloc(len_fail);
    if (buf_fail == NULL)
    {
        printf("Failed to allocate memory.\n");
        exit(1);
    }

    /* Print to buffer */
    if (!cJSON_PrintPreallocated(root, buf, (int)len, 1)) {
        printf("cJSON_PrintPreallocated failed!\n");
        if (strcmp(out, buf) != 0) {
            printf("cJSON_PrintPreallocated not the same as cJSON_Print!\n");
            printf("cJSON_Print result:\n%s\n", out);
            printf("cJSON_PrintPreallocated result:\n%s\n", buf);
        }
        free(out);
        free(buf_fail);
        free(buf);
        return -1;
    }

    /* success */
    printf("%s\n", buf);

    /* force it to fail */
    if (cJSON_PrintPreallocated(root, buf_fail, (int)len_fail, 1)) {
        printf("cJSON_PrintPreallocated failed to show error with insufficient memory!\n");
        printf("cJSON_Print result:\n%s\n", out);
        printf("cJSON_PrintPreallocated result:\n%s\n", buf_fail);
        free(out);
        free(buf_fail);
        free(buf);
        return -1;
    }

    free(out);
    free(buf_fail);
    free(buf);
    return 0;
}

/* Create a bunch of objects as demonstration. */
static void create_objects(void)
{
    /* declare a few. */
    cJSON *root = NULL;
    cJSON *fmt = NULL;
    cJSON *img = NULL;
    cJSON *thm = NULL;
    cJSON *fld = NULL;
    int i = 0;

    /* Our "days of the week" array: */
    const char *strings[7] =
    {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday"
    };
    /* Our matrix: */
    int numbers[3][3] =
    {
        {0, -1, 0},
        {1, 0, 0},
        {0 ,0, 1}
    };
    /* Our "gallery" item: */
    int ids[4] = { 116, 943, 234, 38793 };
    /* Our array of "records": */
    struct record fields[2] =
    {
        {
            "zip",
            37.7668,
            -1.223959e+2,
            "",
            "SAN FRANCISCO",
            "CA",
            "94107",
            "US"
        },
        {
            "zip",
            37.371991,
            -1.22026e+2,
            "",
            "SUNNYVALE",
            "CA",
            "94085",
            "US"
        }
    };
    volatile double zero = 0.0;

    /* Here we construct some JSON standards, from the JSON site. */

    /* Our "Video" datatype: */
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));
    cJSON_AddItemToObject(root, "format", fmt = cJSON_CreateObject());
    cJSON_AddStringToObject(fmt, "type", "rect");
    cJSON_AddNumberToObject(fmt, "width", 1920);
    cJSON_AddNumberToObject(fmt, "height", 1080);
    cJSON_AddFalseToObject (fmt, "interlace");
    cJSON_AddNumberToObject(fmt, "frame rate", 24);

    /* Print to text */
    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);

    /* Our "days of the week" array: */
    root = cJSON_CreateStringArray(strings, 7);

    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);

    /* Our matrix: */
    root = cJSON_CreateArray();
    for (i = 0; i < 3; i++)
    {
        cJSON_AddItemToArray(root, cJSON_CreateIntArray(numbers[i], 3));
    }

    /* cJSON_ReplaceItemInArray(root, 1, cJSON_CreateString("Replacement")); */

    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);

    /* Our "gallery" item: */
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "Image", img = cJSON_CreateObject());
    cJSON_AddNumberToObject(img, "Width", 800);
    cJSON_AddNumberToObject(img, "Height", 600);
    cJSON_AddStringToObject(img, "Title", "View from 15th Floor");
    cJSON_AddItemToObject(img, "Thumbnail", thm = cJSON_CreateObject());
    cJSON_AddStringToObject(thm, "Url", "http:/*www.example.com/image/481989943");
    cJSON_AddNumberToObject(thm, "Height", 125);
    cJSON_AddStringToObject(thm, "Width", "100");
    cJSON_AddItemToObject(img, "IDs", cJSON_CreateIntArray(ids, 4));

    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);

    /* Our array of "records": */
    root = cJSON_CreateArray();
    for (i = 0; i < 2; i++)
    {
        cJSON_AddItemToArray(root, fld = cJSON_CreateObject());
        cJSON_AddStringToObject(fld, "precision", fields[i].precision);
        cJSON_AddNumberToObject(fld, "Latitude", fields[i].lat);
        cJSON_AddNumberToObject(fld, "Longitude", fields[i].lon);
        cJSON_AddStringToObject(fld, "Address", fields[i].address);
        cJSON_AddStringToObject(fld, "City", fields[i].city);
        cJSON_AddStringToObject(fld, "State", fields[i].state);
        cJSON_AddStringToObject(fld, "Zip", fields[i].zip);
        cJSON_AddStringToObject(fld, "Country", fields[i].country);
    }

    /* cJSON_ReplaceItemInObject(cJSON_GetArrayItem(root, 1), "City", cJSON_CreateIntArray(ids, 4)); */

    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);

    root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "number", 1.0 / zero);

    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);
}

static const char* titles[] =
{
    "Red",
    "Green",
    "Blue",
    "Yellow"
};

static const struct
{
    float r, g, b;
} colors[] =
{
    { 0.95f, 0.32f, 0.11f },
    { 0.50f, 0.80f, 0.16f },
    {   0.f, 0.68f, 0.94f },
    { 0.98f, 0.74f, 0.04f }
};
typedef struct
{
    GLFWwindow* window;
    const char* title;
    float r, g, b;
    thrd_t id;
} Thread;


enum Mode
{
    LIST_MODE,
    TEST_MODE
};

static void usage(void)
{
    printf("Usage: monitors [-t]\n");
    printf("       monitors -h\n");
    printf("Usage: msaa [-h] [-s SAMPLES]\n");
}

static const char* format_mode(const GLFWvidmode* mode)
{
    static char buffer[512];
    sprintf(buffer,
            "%i x %i x %i (%i %i %i) %i Hz",
            mode->width, mode->height,
            mode->redBits + mode->greenBits + mode->blueBits,
            mode->redBits, mode->greenBits, mode->blueBits,
            mode->refreshRate);
    buffer[sizeof(buffer) - 1] = '\0';
    return buffer;
}


typedef struct Joystick
{
    GLboolean present;
    char* name;
    float* axes;
    unsigned char* buttons;
    int axis_count;
    int button_count;
} Joystick;
static Joystick joysticks[GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1 + 1];
static int joystick_count = 0;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

    switch (key)
    {
        case GLFW_KEY_SPACE:
        {
            int xpos, ypos;
            glfwGetWindowPos(window, &xpos, &ypos);
            glfwSetWindowPos(window, xpos, ypos);
            break;
        }
        case GLFW_KEY_0:
        {
            glfwSetTime(0.0);
            break;
        }
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
    }
}

static volatile GLboolean running = GL_TRUE;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static int thread_main(void* data)
{
    const Thread* thread = data;

    glfwMakeContextCurrent(thread->window);
    glfwSwapInterval(1);

    while (running)
    {
        const float v = (float) fabs(sin(glfwGetTime() * 2.f));
        glClearColor(thread->r * v, thread->g * v, thread->b * v, 0.f);

        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(thread->window);
    }

    glfwMakeContextCurrent(NULL);
    return 0;
}


static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
     printf("Framebuffer resized to %ix%i\n", width, height);
    glViewport(0, 0, width, height);
}

static void draw_joystick(Joystick* j, int x, int y, int width, int height)
{
    int i;
    const int axis_height = 3 * height / 4;
    const int button_height = height / 4;
    if (j->axis_count)
    {
        const int axis_width = width / j->axis_count;
        for (i = 0;  i < j->axis_count;  i++)
        {
            float value = j->axes[i] / 2.f + 0.5f;
            glColor3f(0.3f, 0.3f, 0.3f);
            glRecti(x + i * axis_width,
                    y,
                    x + (i + 1) * axis_width,
                    y + axis_height);
            glColor3f(1.f, 1.f, 1.f);
            glRecti(x + i * axis_width,
                    y + (int) (value * (axis_height - 5)),
                    x + (i + 1) * axis_width,
                    y + 5 + (int) (value * (axis_height - 5)));
        }
    }
    if (j->button_count)
    {
        const int button_width = width / j->button_count;
        for (i = 0;  i < j->button_count;  i++)
        {
            if (j->buttons[i])
                glColor3f(1.f, 1.f, 1.f);
            else
                glColor3f(0.3f, 0.3f, 0.3f);

            glRecti(x + i * button_width,
                    y + axis_height,
                    x + (i + 1) * button_width,
                    y + axis_height + button_height);
        }
    }
}

static void draw_joysticks(GLFWwindow* window)
{
    int i, width, height, offset = 0;
    glfwGetFramebufferSize(window, &width, &height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.f, width, height, 0.f, 1.f, -1.f);
    glMatrixMode(GL_MODELVIEW);
    for (i = 0;  i < sizeof(joysticks) / sizeof(Joystick);  i++)
    {
        Joystick* j = joysticks + i;

        if (j->present)
        {
            draw_joystick(j,
                          0, offset * height / joystick_count,
                          width, height / joystick_count);
            offset++;
        }
    }
}

static void refresh_joysticks(void)
{
    int i;
    for (i = 0;  i < sizeof(joysticks) / sizeof(Joystick);  i++)
    {
        Joystick* j = joysticks + i;
        if (glfwJoystickPresent(GLFW_JOYSTICK_1 + i))
        {
            const float* axes;
            const unsigned char* buttons;
            int axis_count, button_count;
            free(j->name);
            j->name = strdup(glfwGetJoystickName(GLFW_JOYSTICK_1 + i));
            axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + i, &axis_count);
            if (axis_count != j->axis_count)
            {
                j->axis_count = axis_count;
                j->axes = realloc(j->axes, j->axis_count * sizeof(float));
            }
            memcpy(j->axes, axes, axis_count * sizeof(float));
            buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1 + i, &button_count);
            if (button_count != j->button_count)
            {
                j->button_count = button_count;
                j->buttons = realloc(j->buttons, j->button_count);
            }
            memcpy(j->buttons, buttons, button_count * sizeof(unsigned char));
            if (!j->present)
            {
                printf("Found joystick %i named \'%s\' with %i axes, %i buttons\n",
                       i + 1, j->name, j->axis_count, j->button_count);
                joystick_count++;
            }
            j->present = GL_TRUE;
        }
        else
        {
            if (j->present)
            {
                printf("Lost joystick %i named \'%s\'\n", i + 1, j->name);
                free(j->name);
                free(j->axes);
                free(j->buttons);
                memset(j, 0, sizeof(Joystick));

                joystick_count--;
            }
        }
    }
}

static void list_modes(GLFWmonitor* monitor)
{
    int count, x, y, widthMM, heightMM, i;
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);
    glfwGetMonitorPos(monitor, &x, &y);
    glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
    printf("Name: %s (%s)\n",
           glfwGetMonitorName(monitor),
           glfwGetPrimaryMonitor() == monitor ? "primary" : "secondary");
    printf("Current mode: %s\n", format_mode(mode));
    printf("Virtual position: %i %i\n", x, y);
    printf("Physical size: %i x %i mm (%0.2f dpi)\n",
           widthMM, heightMM, mode->width * 25.4f / widthMM);
    printf("Modes:\n");
    for (i = 0;  i < count;  i++)
    {
        printf("%3u: %s", (unsigned int) i, format_mode(modes + i));
        if (memcmp(mode, modes + i, sizeof(GLFWvidmode)) == 0)
            printf(" (current mode)");
        putchar('\n');
    }
}

static void test_modes(GLFWmonitor* monitor)
{
    int i, count;
    GLFWwindow* window;
    const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);
    for (i = 0;  i < count;  i++)
    {
        const GLFWvidmode* mode = modes + i;
        GLFWvidmode current;
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        printf("Testing mode %u on monitor %s: %s\n",
               (unsigned int) i,
               glfwGetMonitorName(monitor),
               format_mode(mode));
        window = glfwCreateWindow(mode->width, mode->height,
                                  "Video Mode Test",
                                  glfwGetPrimaryMonitor(),
                                  NULL);
        if (!window)
        {
            printf("Failed to enter mode %u: %s\n",
                   (unsigned int) i,
                   format_mode(mode));
            continue;
        }
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetKeyCallback(window, key_callback);
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        glfwSetTime(0.0);
        while (glfwGetTime() < 5.0)
        {
            glClear(GL_COLOR_BUFFER_BIT);
            glfwSwapBuffers(window);
            glfwPollEvents();
            if (glfwWindowShouldClose(window))
            {
                printf("User terminated program\n");

                glfwTerminate();
                exit(EXIT_SUCCESS);
            }
        }
        glGetIntegerv(GL_RED_BITS, &current.redBits);
        glGetIntegerv(GL_GREEN_BITS, &current.greenBits);
        glGetIntegerv(GL_BLUE_BITS, &current.blueBits);
        glfwGetWindowSize(window, &current.width, &current.height);
        if (current.redBits != mode->redBits ||
            current.greenBits != mode->greenBits ||
            current.blueBits != mode->blueBits)
        {
            printf("*** Color bit mismatch: (%i %i %i) instead of (%i %i %i)\n",
                   current.redBits, current.greenBits, current.blueBits,
                   mode->redBits, mode->greenBits, mode->blueBits);
        }
        if (current.width != mode->width || current.height != mode->height)
        {
            printf("*** Size mismatch: %ix%i instead of %ix%i\n",
                   current.width, current.height,
                   mode->width, mode->height);
        }
        printf("Closing window\n");
        glfwDestroyWindow(window);
        window = NULL;
        glfwPollEvents();
    }
}

static unsigned int counter = 0;
typedef struct
{
    GLFWwindow* window;
    int number;
    int closeable;
} Slot;
static void event_usage(void)
{
    printf("Usage: events [-f] [-h] [-n WINDOWS]\n");
    printf("Options:\n");
    printf("  -f use full screen\n");
    printf("  -h show this help\n");
    printf("  -n the number of windows to create\n");
}


static const char* get_key_name(int key)
{
    switch (key)
    {
        // Printable keys
        case GLFW_KEY_A:            return "A";
        case GLFW_KEY_B:            return "B";
        case GLFW_KEY_C:            return "C";
        case GLFW_KEY_D:            return "D";
        case GLFW_KEY_E:            return "E";
        case GLFW_KEY_F:            return "F";
        case GLFW_KEY_G:            return "G";
        case GLFW_KEY_H:            return "H";
        case GLFW_KEY_I:            return "I";
        case GLFW_KEY_J:            return "J";
        case GLFW_KEY_K:            return "K";
        case GLFW_KEY_L:            return "L";
        case GLFW_KEY_M:            return "M";
        case GLFW_KEY_N:            return "N";
        case GLFW_KEY_O:            return "O";
        case GLFW_KEY_P:            return "P";
        case GLFW_KEY_Q:            return "Q";
        case GLFW_KEY_R:            return "R";
        case GLFW_KEY_S:            return "S";
        case GLFW_KEY_T:            return "T";
        case GLFW_KEY_U:            return "U";
        case GLFW_KEY_V:            return "V";
        case GLFW_KEY_W:            return "W";
        case GLFW_KEY_X:            return "X";
        case GLFW_KEY_Y:            return "Y";
        case GLFW_KEY_Z:            return "Z";
        case GLFW_KEY_1:            return "1";
        case GLFW_KEY_2:            return "2";
        case GLFW_KEY_3:            return "3";
        case GLFW_KEY_4:            return "4";
        case GLFW_KEY_5:            return "5";
        case GLFW_KEY_6:            return "6";
        case GLFW_KEY_7:            return "7";
        case GLFW_KEY_8:            return "8";
        case GLFW_KEY_9:            return "9";
        case GLFW_KEY_0:            return "0";
        case GLFW_KEY_SPACE:        return "SPACE";
        case GLFW_KEY_MINUS:        return "MINUS";
        case GLFW_KEY_EQUAL:        return "EQUAL";
        case GLFW_KEY_LEFT_BRACKET: return "LEFT BRACKET";
        case GLFW_KEY_RIGHT_BRACKET: return "RIGHT BRACKET";
        case GLFW_KEY_BACKSLASH:    return "BACKSLASH";
        case GLFW_KEY_SEMICOLON:    return "SEMICOLON";
        case GLFW_KEY_APOSTROPHE:   return "APOSTROPHE";
        case GLFW_KEY_GRAVE_ACCENT: return "GRAVE ACCENT";
        case GLFW_KEY_COMMA:        return "COMMA";
        case GLFW_KEY_PERIOD:       return "PERIOD";
        case GLFW_KEY_SLASH:        return "SLASH";
        case GLFW_KEY_WORLD_1:      return "WORLD 1";
        case GLFW_KEY_WORLD_2:      return "WORLD 2";

        // Function keys
        case GLFW_KEY_ESCAPE:       return "ESCAPE";
        case GLFW_KEY_F1:           return "F1";
        case GLFW_KEY_F2:           return "F2";
        case GLFW_KEY_F3:           return "F3";
        case GLFW_KEY_F4:           return "F4";
        case GLFW_KEY_F5:           return "F5";
        case GLFW_KEY_F6:           return "F6";
        case GLFW_KEY_F7:           return "F7";
        case GLFW_KEY_F8:           return "F8";
        case GLFW_KEY_F9:           return "F9";
        case GLFW_KEY_F10:          return "F10";
        case GLFW_KEY_F11:          return "F11";
        case GLFW_KEY_F12:          return "F12";
        case GLFW_KEY_F13:          return "F13";
        case GLFW_KEY_F14:          return "F14";
        case GLFW_KEY_F15:          return "F15";
        case GLFW_KEY_F16:          return "F16";
        case GLFW_KEY_F17:          return "F17";
        case GLFW_KEY_F18:          return "F18";
        case GLFW_KEY_F19:          return "F19";
        case GLFW_KEY_F20:          return "F20";
        case GLFW_KEY_F21:          return "F21";
        case GLFW_KEY_F22:          return "F22";
        case GLFW_KEY_F23:          return "F23";
        case GLFW_KEY_F24:          return "F24";
        case GLFW_KEY_F25:          return "F25";
        case GLFW_KEY_UP:           return "UP";
        case GLFW_KEY_DOWN:         return "DOWN";
        case GLFW_KEY_LEFT:         return "LEFT";
        case GLFW_KEY_RIGHT:        return "RIGHT";
        case GLFW_KEY_LEFT_SHIFT:   return "LEFT SHIFT";
        case GLFW_KEY_RIGHT_SHIFT:  return "RIGHT SHIFT";
        case GLFW_KEY_LEFT_CONTROL: return "LEFT CONTROL";
        case GLFW_KEY_RIGHT_CONTROL: return "RIGHT CONTROL";
        case GLFW_KEY_LEFT_ALT:     return "LEFT ALT";
        case GLFW_KEY_RIGHT_ALT:    return "RIGHT ALT";
        case GLFW_KEY_TAB:          return "TAB";
        case GLFW_KEY_ENTER:        return "ENTER";
        case GLFW_KEY_BACKSPACE:    return "BACKSPACE";
        case GLFW_KEY_INSERT:       return "INSERT";
        case GLFW_KEY_DELETE:       return "DELETE";
        case GLFW_KEY_PAGE_UP:      return "PAGE UP";
        case GLFW_KEY_PAGE_DOWN:    return "PAGE DOWN";
        case GLFW_KEY_HOME:         return "HOME";
        case GLFW_KEY_END:          return "END";
        case GLFW_KEY_KP_0:         return "KEYPAD 0";
        case GLFW_KEY_KP_1:         return "KEYPAD 1";
        case GLFW_KEY_KP_2:         return "KEYPAD 2";
        case GLFW_KEY_KP_3:         return "KEYPAD 3";
        case GLFW_KEY_KP_4:         return "KEYPAD 4";
        case GLFW_KEY_KP_5:         return "KEYPAD 5";
        case GLFW_KEY_KP_6:         return "KEYPAD 6";
        case GLFW_KEY_KP_7:         return "KEYPAD 7";
        case GLFW_KEY_KP_8:         return "KEYPAD 8";
        case GLFW_KEY_KP_9:         return "KEYPAD 9";
        case GLFW_KEY_KP_DIVIDE:    return "KEYPAD DIVIDE";
        case GLFW_KEY_KP_MULTIPLY:  return "KEYPAD MULTPLY";
        case GLFW_KEY_KP_SUBTRACT:  return "KEYPAD SUBTRACT";
        case GLFW_KEY_KP_ADD:       return "KEYPAD ADD";
        case GLFW_KEY_KP_DECIMAL:   return "KEYPAD DECIMAL";
        case GLFW_KEY_KP_EQUAL:     return "KEYPAD EQUAL";
        case GLFW_KEY_KP_ENTER:     return "KEYPAD ENTER";
        case GLFW_KEY_PRINT_SCREEN: return "PRINT SCREEN";
        case GLFW_KEY_NUM_LOCK:     return "NUM LOCK";
        case GLFW_KEY_CAPS_LOCK:    return "CAPS LOCK";
        case GLFW_KEY_SCROLL_LOCK:  return "SCROLL LOCK";
        case GLFW_KEY_PAUSE:        return "PAUSE";
        case GLFW_KEY_LEFT_SUPER:   return "LEFT SUPER";
        case GLFW_KEY_RIGHT_SUPER:  return "RIGHT SUPER";
        case GLFW_KEY_MENU:         return "MENU";

        default:                    return "UNKNOWN";
    }
}

static const char* get_action_name(int action)
{
    switch (action)
    {
        case GLFW_PRESS:
            return "pressed";
        case GLFW_RELEASE:
            return "released";
        case GLFW_REPEAT:
            return "repeated";
    }

    return "caused unknown action";
}

static const char* get_button_name(int button)
{
    switch (button)
    {
        case GLFW_MOUSE_BUTTON_LEFT:
            return "left";
        case GLFW_MOUSE_BUTTON_RIGHT:
            return "right";
        case GLFW_MOUSE_BUTTON_MIDDLE:
            return "middle";
        default:
        {
            static char name[16];
            sprintf(name, "%i", button);
            return name;
        }
    }
}

static const char* get_mods_name(int mods)
{
    static char name[512];

    if (mods == 0)
        return " no mods";

    name[0] = '\0';

    if (mods & GLFW_MOD_SHIFT)
        strcat(name, " shift");
    if (mods & GLFW_MOD_CONTROL)
        strcat(name, " control");
    if (mods & GLFW_MOD_ALT)
        strcat(name, " alt");
    if (mods & GLFW_MOD_SUPER)
        strcat(name, " super");

    return name;
}

static const char* get_character_string(int codepoint)
{
    // This assumes UTF-8, which is stupid
    static char result[6 + 1];

    int length = wctomb(result, codepoint);
    if (length == -1)
        length = 0;

    result[length] = '\0';
    return result;
}

static void window_pos_callback(GLFWwindow* window, int x, int y)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Window position: %i %i\n",
           counter++, slot->number, glfwGetTime(), x, y);
}

static void window_size_callback(GLFWwindow* window, int width, int height)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Window size: %i %i\n",
           counter++, slot->number, glfwGetTime(), width, height);
}

static void sframebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Framebuffer size: %i %i\n",
           counter++, slot->number, glfwGetTime(), width, height);
    glViewport(0, 0, width, height);
}


static void window_close_callback(GLFWwindow* window)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Window close\n",
           counter++, slot->number, glfwGetTime());
    glfwSetWindowShouldClose(window, slot->closeable);
}

static void window_refresh_callback(GLFWwindow* window)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Window refresh\n",
           counter++, slot->number, glfwGetTime());
    glfwMakeContextCurrent(window);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
}

static void window_focus_callback(GLFWwindow* window, int focused)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Window %s\n",
           counter++, slot->number, glfwGetTime(),
           focused ? "focused" : "defocused");
}

static void window_iconify_callback(GLFWwindow* window, int iconified)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Window was %s\n",
           counter++, slot->number, glfwGetTime(),
           iconified ? "iconified" : "restored");
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Mouse button %i (%s) (with%s) was %s\n",
           counter++, slot->number, glfwGetTime(), button,
           get_button_name(button),
           get_mods_name(mods),
           get_action_name(action));
}

static void cursor_position_callback(GLFWwindow* window, double x, double y)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Cursor position: %f %f\n",
           counter++, slot->number, glfwGetTime(), x, y);
}

static void cursor_enter_callback(GLFWwindow* window, int entered)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Cursor %s window\n",
           counter++, slot->number, glfwGetTime(),
           entered ? "entered" : "left");
}

static void scroll_callback(GLFWwindow* window, double x, double y)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Scroll: %0.3f %0.3f\n",
           counter++, slot->number, glfwGetTime(), x, y);
}

static void skey_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Key 0x%04x Scancode 0x%04x (%s) (with%s) was %s\n",
           counter++, slot->number, glfwGetTime(), key, scancode,
           get_key_name(key),
           get_mods_name(mods),
           get_action_name(action));
    if (action != GLFW_PRESS)
        return;
    switch (key)
    {
        case GLFW_KEY_C:
        {
            slot->closeable = !slot->closeable;
            printf("(( closing %s ))\n", slot->closeable ? "enabled" : "disabled");
            break;
        }
    }
}

static void char_callback(GLFWwindow* window, unsigned int codepoint)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Character 0x%08x (%s) input\n",
           counter++, slot->number, glfwGetTime(), codepoint,
           get_character_string(codepoint));
}

static void char_mods_callback(GLFWwindow* window, unsigned int codepoint, int mods)
{
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Character 0x%08x (%s) with modifiers (with%s) input\n",
            counter++, slot->number, glfwGetTime(), codepoint,
            get_character_string(codepoint),
            get_mods_name(mods));
}

static void drop_callback(GLFWwindow* window, int count, const char** paths)
{
    int i;
    Slot* slot = glfwGetWindowUserPointer(window);
    printf("%08x to %i at %0.3f: Drop input\n",
           counter++, slot->number, glfwGetTime());
    for (i = 0;  i < count;  i++)
        printf("  %i: \"%s\"\n", i, paths[i]);
}

static void monitor_callback(GLFWmonitor* monitor, int event)
{
    if (event == GLFW_CONNECTED)
    {
        int x, y, widthMM, heightMM;
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwGetMonitorPos(monitor, &x, &y);
        glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
        printf("%08x at %0.3f: Monitor %s (%ix%i at %ix%i, %ix%i mm) was connected\n",
               counter++,
               glfwGetTime(),
               glfwGetMonitorName(monitor),
               mode->width, mode->height,
               x, y,
               widthMM, heightMM);
    }
    else
    {
        printf("%08x at %0.3f: Monitor %s was disconnected\n",
               counter++,
               glfwGetTime(),
               glfwGetMonitorName(monitor));
    }
}

#define CURSOR_FRAME_COUNT 60
static double cursor_x;
static double cursor_y;
static int swap_interval = 1;
static GLboolean wait_events = GL_FALSE;
static GLboolean animate_cursor = GL_FALSE;
static GLboolean track_cursor = GL_FALSE;
static GLFWcursor* standard_cursors[6];

static float star(int x, int y, float t)
{
    const float c = 64 / 2.f;
    const float i = (0.25f * (float) sin(2.f * M_PI * t) + 0.75f);
    const float k = 64 * 0.046875f * i;
    const float dist = (float) sqrt((x - c) * (x - c) + (y - c) * (y - c));
    const float salpha = 1.f - dist / c;
    const float xalpha = (float) x == c ? c : k / (float) fabs(x - c);
    const float yalpha = (float) y == c ? c : k / (float) fabs(y - c);
    return (float) fmax(0.f, fmin(1.f, i * salpha * 0.2f + salpha * xalpha * yalpha));
}

static GLFWcursor* create_cursor_frame(float t)
{
    int i = 0, x, y;
    unsigned char buffer[64 * 64 * 4];
    const GLFWimage image = { 64, 64, buffer };
    for (y = 0;  y < image.width;  y++)
    {
        for (x = 0;  x < image.height;  x++)
        {
            buffer[i++] = 255;
            buffer[i++] = 255;
            buffer[i++] = 255;
            buffer[i++] = (unsigned char) (255 * star(x, y, t));
        }
    }
    return glfwCreateCursor(&image, image.width / 2, image.height / 2);
}

static void ccursor_position_callback(GLFWwindow* window, double x, double y)
{
    printf("%0.3f: Cursor position: %f %f (%+f %+f)\n",
           glfwGetTime(),
           x, y, x - cursor_x, y - cursor_y);
    cursor_x = x;
    cursor_y = y;
}

static void ckey_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;
    switch (key)
    {
        case GLFW_KEY_A:
        {
            animate_cursor = !animate_cursor;
            if (!animate_cursor)
                glfwSetCursor(window, NULL);

            break;
        }
        case GLFW_KEY_ESCAPE:
        {
            if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
            {
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            }
            /* FALLTHROUGH */
        }
        case GLFW_KEY_N:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            printf("(( cursor is normal ))\n");
            break;
        case GLFW_KEY_D:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            printf("(( cursor is disabled ))\n");
            break;
        case GLFW_KEY_H:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            printf("(( cursor is hidden ))\n");
            break;
        case GLFW_KEY_SPACE:
            swap_interval = 1 - swap_interval;
            printf("(( swap interval: %i ))\n", swap_interval);
            glfwSwapInterval(swap_interval);
            break;
        case GLFW_KEY_W:
            wait_events = !wait_events;
            printf("(( %sing for events ))\n", wait_events ? "wait" : "poll");
            break;
        case GLFW_KEY_T:
            track_cursor = !track_cursor;
            break;
        case GLFW_KEY_0:
            glfwSetCursor(window, NULL);
            break;
        case GLFW_KEY_1:
            glfwSetCursor(window, standard_cursors[0]);
            break;
        case GLFW_KEY_2:
            glfwSetCursor(window, standard_cursors[1]);
            break;
        case GLFW_KEY_3:
            glfwSetCursor(window, standard_cursors[2]);
            break;
        case GLFW_KEY_4:
            glfwSetCursor(window, standard_cursors[3]);
            break;
        case GLFW_KEY_5:
            glfwSetCursor(window, standard_cursors[4]);
            break;
        case GLFW_KEY_6:
            glfwSetCursor(window, standard_cursors[5]);
            break;
    }
}


#if defined(__APPLE__)
 #define MODIFIER GLFW_MOD_SUPER
#else
 #define MODIFIER GLFW_MOD_CONTROL
#endif

static void clipboard_usage(void)
{
    printf("Usage: clipboard [-h]\n");
}


static void cb_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;
    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_V:
            if (mods == MODIFIER)
            {
                const char* string;
                string = glfwGetClipboardString(window);
                if (string)
                    printf("Clipboard contains \"%s\"\n", string);
                else
                    printf("Clipboard does not contain a string\n");
            }
            break;
        case GLFW_KEY_C:
            if (mods == MODIFIER)
            {
                const char* string = "Hello GLFW World!";
                glfwSetClipboardString(window, string);
                printf("Setting clipboard to \"%s\"\n", string);
            }
            break;
    }
}



#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define LIB_NAME "clua"

static void open_file(lua_State* L, const char* filename){
	if (luaL_dofile(L, filename) != LUA_TNIL){
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
	}
}

int l_system( lua_State* L )
{
	if ( lua_gettop( L ) != 1 )
	{
		return luaL_error(L, "expected 1 argument");
	}
	if ( lua_type( L, -1 ) != LUA_TSTRING )
	{
		return luaL_error( L, "argument 1 - this isn't a string" );
	}
	const char* str = lua_tostring( L, -1 );
	system( str );
	return 1;
}

int l_sleep( lua_State* L )
{
	if ( lua_gettop( L ) != 1 )
	{
		return luaL_error( L, "expected 1 argument" );
	}
	if ( lua_type( L, -1 ) != LUA_TNUMBER )
	{
		return luaL_error( L, "argument 1 - this isn't a number" );
	}
	const float time = lua_tonumber( L, -1 );
	Sleep( ( DWORD )time );
	return 1;
}

int l_setcolor( lua_State* L )
{
	if ( lua_gettop( L ) != 1 )
	{
		return luaL_error( L, "expected 1 argument" );
	}
	if ( lua_type( L, -1 ) != LUA_TNUMBER )
	{
		return luaL_error( L, "argument 1 - this isn't a number" );
	}
	const float num = lua_tonumber( L, -1 );
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), (int)num );
	return 1;
}

int l_openfile( lua_State* L )
{
	if ( lua_gettop( L ) != 1 )
	{
		return luaL_error( L, "expected 1 argument" );
	}
	if ( lua_type( L, -1 ) != LUA_TSTRING )
	{
		return luaL_error( L, "argument 1 - this isn't a string" );
	}
	const char* filename = lua_tostring( L, -1 );
	FILE* handle;
	handle = fopen( filename, "r" );
	if (handle == NULL){
		//no such file!
		lua_getglobal(L, "print");
		lua_pushstring(L, "File reading error");
		lua_pcall(L, 1, 1, 0);
		return 1;
	}
	char buffer[1024];
	int i = 0;
	while (!feof(handle)){
		buffer[i] = (char)fgetc(handle);
		i++;
	}
	buffer[i] = '\0';
	lua_pushstring( L, buffer );
	fclose(handle);
	return 1;
}

int l_appendfile(lua_State* L){
	if (lua_gettop(L) != 2){
		return luaL_error(L, "expected 2 arguments");
	}
	if (lua_type(L, -1) != LUA_TSTRING){
		return luaL_error(L, "argument 2 - this isn't a string");
	}
	if (lua_type(L, -2) != LUA_TSTRING){
		return luaL_error(L, "argument 1 - this isn't a string");
	}
	const char* content = lua_tostring(L, -1);
	const char* filename = lua_tostring(L, -2);
	FILE* handle;
	handle = fopen(filename, "a");
	if (handle == NULL){
		lua_getglobal(L, "print");
		lua_pushstring(L, "File writing error");
		lua_pcall(L, 1, 1, 0);
		return 2;
	}
	fputs(content, handle);
	fclose(handle);
	return 2;
}

int l_writefile(lua_State* L){
	if (lua_gettop(L) != 2){
		return luaL_error(L, "expected 2 arguments");
	}
	if (lua_type(L, -1) != LUA_TSTRING){
		return luaL_error(L, "argument 2 - this isn't a string");
	}
	if (lua_type(L, -2) != LUA_TSTRING){
		return luaL_error(L, "argument 1 - this isn't a string");
	}
	const char* content = lua_tostring(L, -1);
	const char* filename = lua_tostring(L, -2);
	FILE* handle;
	handle = fopen(filename, "w");
	if (handle == NULL){
		lua_getglobal(L, "print");
		lua_pushstring(L, "File writing error");
		lua_pcall(L, 1, 1, 0);
		return 2;
	}
	fputs(content, handle);
	fclose(handle);
	return 2;
}

int l_include(lua_State* L){
	if (lua_gettop(L) != 1){
		return luaL_error(L, "expected 1 argument");
	}
	if (lua_type(L, -1) != LUA_TSTRING){
		return luaL_error(L, "argument 1 - this isn't a string");
	}
	const char* path = lua_tostring(L, -1);
	open_file(L, path);
	return 1;
}

static const struct luaL_Reg bindFuncs[] = {
	{"system", l_system },
	{"sleep", l_sleep },
	{"setcolor", l_setcolor },
	{"openfile", l_openfile },
	{"writefile", l_writefile},
	{"appendfile", l_appendfile},
	{"include", l_include},
	{NULL, NULL }
};

static LRESULT CALLBACK
WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    if (nk_gdi_handle_event(wnd, msg, wparam, lparam))
        return 0;

    return DefWindowProcW(wnd, msg, wparam, lparam);
}

void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

int blowup = 0;
int screenshot = 0;
int premult = 0;

static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	NVG_NOTUSED(scancode);
	NVG_NOTUSED(mods);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		blowup = !blowup;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		screenshot = 1;
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		premult = !premult;
}



#define MINIH264_IMPLEMENTATION
//#define MINIH264_ONLY_SIMD
#include "minih264e.h"

#define DEFAULT_GOP 20
#define DEFAULT_QP 33
#define DEFAULT_DENOISE 0

#define ENABLE_TEMPORAL_SCALABILITY 0
#define MAX_LONG_TERM_FRAMES        8 // used only if ENABLE_TEMPORAL_SCALABILITY==1

#define DEFAULT_MAX_FRAMES  99999

H264E_create_param_t create_param;
H264E_run_param_t run_param;
H264E_io_yuv_t yuv;
uint8_t *buf_in, *buf_save;
uint8_t *coded_data;
FILE *fin, *fout;
int sizeof_coded_data, frame_size, g_w, g_h, _qp;

#ifdef _WIN32
// only vs2017 have aligned_alloc
#define ALIGNED_ALLOC(n, size) malloc(size)
#else
#define ALIGNED_ALLOC(n, size) aligned_alloc(n, (size + n - 1)/n*n)
#endif

#if H264E_MAX_THREADS
#include "minisystem.h"
typedef struct
{
    void *event_start;
    void *event_done;
    void (*callback)(void*);
    void *job;
    void *thread;
    int terminated;
} h264e_thread_t;

static THREAD_RET THRAPI minih264_thread_func(void *arg)
{
    h264e_thread_t *t = (h264e_thread_t *)arg;
    thread_name("h264");
    for (;;)
    {
        event_wait(t->event_start, INFINITE);
        if (t->terminated)
            break;
        t->callback(t->job);
        event_set(t->event_done);
    }
    return 0;
}

void *h264e_thread_pool_init(int max_threads)
{
    int i;
    h264e_thread_t *threads = (h264e_thread_t *)calloc(sizeof(h264e_thread_t), max_threads);
    if (!threads)
        return 0;
    for (i = 0; i < max_threads; i++)
    {
        h264e_thread_t *t = threads + i;
        t->event_start = event_create(0, 0);
        t->event_done  = event_create(0, 0);
        t->thread = thread_create(minih264_thread_func, t);
    }
    return threads;
}

void h264e_thread_pool_close(void *pool, int max_threads)
{
    int i;
    h264e_thread_t *threads = (h264e_thread_t *)pool;
    for (i = 0; i < max_threads; i++)
    {
        h264e_thread_t *t = threads + i;
        t->terminated = 1;
        event_set(t->event_start);
        thread_wait(t->thread);
        thread_close(t->thread);
        event_destroy(t->event_start);
        event_destroy(t->event_done);
    }
    free(pool);
}

void h264e_thread_pool_run(void *pool, void (*callback)(void*), void *callback_job[], int njobs)
{
    h264e_thread_t *threads = (h264e_thread_t*)pool;
    int i;
    for (i = 0; i < njobs; i++)
    {
        h264e_thread_t *t = threads + i;
        t->callback = (void (*)(void *))callback;
        t->job = callback_job[i];
        event_set(t->event_start);
    }
    for (i = 0; i < njobs; i++)
    {
        h264e_thread_t *t = threads + i;
        event_wait(t->event_done, INFINITE);
    }
}
#endif

struct
{
    const char *input_file;
    const char *output_file;
    int gen, gop, qp, kbps, max_frames, threads, speed, denoise, stats, psnr;
} cmdline[1];

static int str_equal(const char *pattern, char **p)
{
    if (!strncmp(pattern, *p, strlen(pattern)))
    {
        *p += strlen(pattern);
        return 1;
    } else
    {
        return 0;
    }
}

static int read_cmdline_options(int argc, char *argv[])
{
    int i;
    memset(cmdline, 0, sizeof(*cmdline));
    cmdline->gop = DEFAULT_GOP;
    cmdline->qp = DEFAULT_QP;
    cmdline->max_frames = DEFAULT_MAX_FRAMES;
    cmdline->kbps = 0;
    //cmdline->kbps = 2048;
    cmdline->denoise = DEFAULT_DENOISE;
    for (i = 1; i < argc; i++)
    {
        char *p = argv[i];
        if (*p == '-')
        {
            p++;
            if (str_equal(("gen"), &p))
            {
                cmdline->gen = 1;
            } else if (str_equal(("gop"), &p))
            {
                cmdline->gop = atoi(p);
            } else if (str_equal(("qp"), &p))
            {
                cmdline->qp = atoi(p);
            } else if (str_equal(("kbps"), &p))
            {
                cmdline->kbps = atoi(p);
            } else if (str_equal(("maxframes"), &p))
            {
                cmdline->max_frames = atoi(p);
            } else if (str_equal(("threads"), &p))
            {
                cmdline->threads = atoi(p);
            } else if (str_equal(("speed"), &p))
            {
                cmdline->speed = atoi(p);
            } else if (str_equal(("denoise"), &p))
            {
                cmdline->denoise = 1;
            } else if (str_equal(("stats"), &p))
            {
                cmdline->stats = 1;
            } else if (str_equal(("psnr"), &p))
            {
                cmdline->psnr = 1;
            } else
            {
                printf("ERROR: Unknown option %s\n", p - 1);
                return 0;
            }
        } else if (!cmdline->input_file && !cmdline->gen)
        {
            cmdline->input_file = p;
        } else if (!cmdline->output_file)
        {
            cmdline->output_file = p;
        } else
        {
            printf("ERROR: Unknown option %s\n", p);
            return 0;
        }
    }
    if (!cmdline->input_file && !cmdline->gen)
    {
        printf("Usage:\n"
               "    h264e_test [options] <input[frame_size].yuv> <output.264>\n"
               "Frame size can be: WxH sqcif qvga svga 4vga sxga xga vga qcif 4cif\n"
               "    4sif cif sif pal ntsc d1 16cif 16sif 720p 4SVGA 4XGA 16VGA 16VGA\n"
               "Options:\n"
               "    -gen            - generate input instead of passing <input.yuv>\n"
               "    -qop<n>         - key frame period >= 0\n"
               "    -qp<n>          - set QP [10..51]\n"
               "    -kbps<n>        - set bitrate (fps=30 assumed)\n"
               "    -maxframes<n>   - encode no more than given number of frames\n"
               "    -threads<n>     - use <n> threads for encode\n"
               "    -speed<n>       - speed [0..10], 0 means best quality\n"
               "    -denoise        - use temporal noise supression\n"
               "    -stats          - print frame statistics\n"
               "    -psnr           - print psnr statistics\n");
        return 0;
    }
    return 1;
}

typedef struct
{
    const char *size_name;
    int g_w;
    int h;
} frame_size_descriptor_t;

static const frame_size_descriptor_t g_frame_size_descriptor[] =
{
    {"sqcif",  128,   96},
    { "qvga",  320,  240},
    { "svga",  800,  600},
    { "4vga", 1280,  960},
    { "sxga", 1280, 1024},
    {  "xga", 1024,  768},
    {  "vga",  640,  480},
    { "qcif",  176,  144},
    { "4cif",  704,  576},
    { "4sif",  704,  480},
    {  "cif",  352,  288},
    {  "sif",  352,  240},
    {  "pal",  720,  576},
    { "ntsc",  720,  480},
    {   "d1",  720,  480},
    {"16cif", 1408, 1152},
    {"16sif", 1408,  960},
    { "720p", 1280,  720},
    {"4SVGA", 1600, 1200},
    { "4XGA", 2048, 1536},
    {"16VGA", 2560, 1920},
    {"16VGA", 2560, 1920},
    {NULL, 0, 0},
};

/**
*   Guess image size specification from ASCII string.
*   If string have several specs, only last one taken.
*   Spec may look like "352x288" or "qcif", "cif", etc.
*/
static int guess_format_from_name(const char *file_name, int *w, int *h)
{
    int i = (int)strlen(file_name);
    int found = 0;
    while(--i >= 0)
    {
        const frame_size_descriptor_t *fmt = g_frame_size_descriptor;
        const char *p = file_name + i;
        int prev_found = found;
        found = 0;
        if (*p >= '0' && *p <= '9')
        {
            char * end;
            int width = strtoul(p, &end, 10);
            if (width && (*end == 'x' || *end == 'X') && (end[1] >= '1' && end[1] <= '9'))
            {
                int height = strtoul(end + 1, &end, 10);
                if (height)
                {
                    *w = width;
                    *h = height;
                    found = 1;
                }
            }
        }
        do
        {
            if (!strncmp(file_name + i, fmt->size_name, strlen(fmt->size_name)))
            {
                *w = fmt->g_w;
                *h = fmt->h;
                found = 1;
            }
        } while((++fmt)->size_name);

        if (!found && prev_found)
        {
            return prev_found;
        }
    }
    return found;
}

// PSNR estimation results
typedef struct
{
    double psnr[4];             // PSNR, db
    double kpbs_30fps;          // bitrate, kbps, assuming 30 fps
    double psnr_to_logkbps_ratio;  // cumulative quality metric
    double psnr_to_kbps_ratio;  // another variant of cumulative quality metric
} rd_t;


static struct
{
    // Y,U,V,Y+U+V
    double noise[4];
    double count[4];
    double bytes;
    int frames;
} g_psnr;

static void psnr_init()
{
    memset(&g_psnr, 0, sizeof(g_psnr));
}

static void psnr_add(unsigned char *p0, unsigned char *p1, int w, int h, int bytes)
{
    int i, k;
    for (k = 0; k < 3; k++)
    {
        double s = 0;
        for (i = 0; i < w*h; i++)
        {
            int d = *p0++ - *p1++;
            s += d*d;
        }
        g_psnr.count[k] += w*h;
        g_psnr.noise[k] += s;
        if (!k) w >>= 1, h >>= 1;
    }
    g_psnr.count[3] = g_psnr.count[0] + g_psnr.count[1] + g_psnr.count[2];
    g_psnr.noise[3] = g_psnr.noise[0] + g_psnr.noise[1] + g_psnr.noise[2];
    g_psnr.frames++;
    g_psnr.bytes += bytes;
}

static rd_t psnr_get()
{
    int i;
    rd_t rd;
    double fps = 30;
    double realkbps = g_psnr.bytes*8./((double)g_psnr.frames/(fps))/1000;
    double db = 10*log10(255.*255/(g_psnr.noise[0]/g_psnr.count[0]));
    for (i = 0; i < 4; i++)
    {
        rd.psnr[i] = 10*log10(255.*255/(g_psnr.noise[i]/g_psnr.count[i]));
    }
    rd.psnr_to_kbps_ratio = 10*log10((double)g_psnr.count[0]*g_psnr.count[0]*3/2 * 255*255/(g_psnr.noise[0] * g_psnr.bytes));
    rd.psnr_to_logkbps_ratio = db / log10(realkbps);
    rd.kpbs_30fps = realkbps;
    return rd;
}

static void psnr_print(rd_t rd)
{
    int i;
    printf("%5.0f kbps@30fps  ", rd.kpbs_30fps);
    for (i = 0; i < 3; i++)
    {
        //printf("  %.2f db ", rd.psnr[i]);
        printf(" %s=%.2f db ", i ? (i == 1 ? "UPSNR" : "VPSNR") : "YPSNR", rd.psnr[i]);
    }
    printf("  %6.2f db/rate ", rd.psnr_to_kbps_ratio);
    printf("  %6.3f db/lgrate ", rd.psnr_to_logkbps_ratio);
    printf("  \n");
}

static int pixel_of_chessboard(double x, double y)
{
#if 0
    int mid = (fabs(x) < 4 && fabs(y) < 4);
    int i = (int)(x);
    int j = (int)(y);
    int cx, cy;
    cx = (i & 16) ? 255 : 0;
    cy = (j & 16) ? 255 : 0;
    if ((i & 15) == 0) cx *= (x - i);
    if ((j & 15) == 0) cx *= (y - j);
    return (cx + cy + 1) >> 1;
#else
    int mid = (fabs(x ) < 4 && fabs(y) < 4);
    int i = (int)(x);
    int j = (int)(y);
    int black = (mid) ? 128 : i/16;
    int white = (mid) ? 128 : 255 - j/16;
    int c00 = (((i >> 4) + (j >> 4)) & 1) ? white : black;
    int c01 = ((((i + 1)>> 4) + (j >> 4)) & 1) ? white : black;
    int c10 = (((i >> 4) + ((j + 1) >> 4)) & 1) ? white : black;
    int c11 = ((((i + 1) >> 4) + ((j + 1) >> 4)) & 1) ? white : black;
    int s    = (int)((c00 * (1 - (x - i)) + c01*(x - i))*(1 - (y - j)) +
                     (c10 * (1 - (x - i)) + c11*(x - i))*((y - j)) + 0.5);
    return s < 0 ? 0 : s > 255 ? 255 : s;
#endif
}

static void gen_chessboard_rot(unsigned char *p, int w, int h, int frm)
{
    int r, c;
    double x, y;
    double co = cos(.01*frm);
    double si = sin(.01*frm);
    int hw = w >> 1;
    int hh = h >> 1;
    for (r = 0; r < h; r++)
    {
        for (c = 0; c < w; c++)
        {
            x =  co*(c - hw) + si*(r - hh);
            y = -si*(c - hw) + co*(r - hh);
            p[r*w + c] = pixel_of_chessboard(x, y);
        }
    }
}


int CJSON_CDECL main(int argc, char** argv)
{
	{
		int i, frames = 0;
		const char *fnin, *fnout;

		if (!read_cmdline_options(argc, argv))
			return 1;
		fnin  = cmdline->input_file;
		fnout = cmdline->output_file;

		if (!cmdline->gen)
		{
			g_w = 352;
			g_h = 288;
			guess_format_from_name(fnin, &g_w, &g_h);
			fin = fopen(fnin, "rb");
			if (!fin)
			{
				printf("ERROR: cant open input file %s\n", fnin);
				return 1;
			}
		} else
		{
			g_w = 1024;
			g_h = 768;
		}

		if (!fnout)
			fnout = "out.264";
		fout = fopen(fnout, "wb");
		if (!fout)
		{
			printf("ERROR: cant open output file %s\n", fnout);
			return 1;
		}

		create_param.enableNEON = 1;
		#if H264E_SVC_API
		create_param.num_layers = 1;
		create_param.inter_layer_pred_flag = 0;
		#endif
		create_param.gop = cmdline->gop;
		create_param.height = g_h;
		create_param.width  = g_w;
		create_param.max_long_term_reference_frames = 0;
		#if ENABLE_TEMPORAL_SCALABILITY
		create_param.max_long_term_reference_frames = MAX_LONG_TERM_FRAMES;
		#endif
		create_param.fine_rate_control_flag = 0;
		create_param.const_input_flag = cmdline->psnr ? 0 : 1;
		//create_param.vbv_overflow_empty_frame_flag = 1;
		//create_param.vbv_underflow_stuffing_flag = 1;
		create_param.vbv_size_bytes = cmdline->kbps*1000/8*2; // 2 seconds vbv buffer for quality, so rate control can allocate more bits for intra frame
		create_param.temporal_denoise_flag = cmdline->denoise;

		#if H264E_MAX_THREADS
		void *thread_pool = NULL;
		create_param.max_threads = cmdline->threads;
		if (cmdline->threads)
		{
			thread_pool = h264e_thread_pool_init(cmdline->threads);
			create_param.token = thread_pool;
			create_param.run_func_in_thread = h264e_thread_pool_run;
		}
		#endif

		frame_size = g_w*g_h*3/2;
		buf_in   = (uint8_t*)ALIGNED_ALLOC(64, frame_size);
		buf_save = (uint8_t*)ALIGNED_ALLOC(64, frame_size);

		if (!buf_in || !buf_save)
		{
			printf("ERROR: not enough memory\n");
			return 1;
		}
		//for (cmdline->qp = 10; cmdline->qp <= 51; cmdline->qp += 10)
		//for (cmdline->qp = 40; cmdline->qp <= 51; cmdline->qp += 10)
		//for (cmdline->qp = 50; cmdline->qp <= 51; cmdline->qp += 2)
		//printf("encoding %s to %s with qp = %d\n", fnin, fnout, cmdline->qp);
		{
			int sum_bytes = 0;
			int max_bytes = 0;
			int min_bytes = 10000000;
			int sizeof_persist = 0, sizeof_scratch = 0, error;
			H264E_persist_t *enc = NULL;
			H264E_scratch_t *scratch = NULL;
			if (cmdline->psnr)
				psnr_init();

			error = H264E_sizeof(&create_param, &sizeof_persist, &sizeof_scratch);
			if (error)
			{
				printf("H264E_init error = %d\n", error);
				return 0;
			}
			printf("sizeof_persist = %d sizeof_scratch = %d\n", sizeof_persist, sizeof_scratch);
			enc     = (H264E_persist_t *)ALIGNED_ALLOC(64, sizeof_persist);
			scratch = (H264E_scratch_t *)ALIGNED_ALLOC(64, sizeof_scratch);
			error = H264E_init(enc, &create_param);

			if (fin)
				fseek(fin, 0, SEEK_SET);

			for (i = 0; cmdline->max_frames; i++)
			{
				if (!fin)
				{
					if (i > 300) break;
					memset(buf_in + g_w*g_h, 128, g_w*g_h/2);
					gen_chessboard_rot(buf_in, g_w, g_h, i);
				} else
					if (!fread(buf_in, frame_size, 1, fin)) break;
				if (cmdline->psnr)
					memcpy(buf_save, buf_in, frame_size);

				yuv.yuv[0] = buf_in; yuv.stride[0] = g_w;
				yuv.yuv[1] = buf_in + g_w*g_h; yuv.stride[1] = g_w/2;
				yuv.yuv[2] = buf_in + g_w*g_h*5/4; yuv.stride[2] = g_w/2;

				run_param.frame_type = 0;
				run_param.encode_speed = cmdline->speed;
				//run_param.desired_nalu_bytes = 100;

				if (cmdline->kbps)
				{
					run_param.desired_frame_bytes = cmdline->kbps*1000/8/30;
					run_param.qp_min = 10;
					run_param.qp_max = 50;
				} else
				{
					run_param.qp_min = run_param.qp_max = cmdline->qp;
				}

		#if ENABLE_TEMPORAL_SCALABILITY
				{
				int level, logmod = 1;
				int j, mod = 1 << logmod;
				static int fresh[200] = {-1,-1,-1,-1};

				run_param.frame_type = H264E_FRAME_TYPE_CUSTOM;

				for (level = logmod; level && (~i & (mod >> level)); level--){}

				run_param.long_term_idx_update = level + 1;
				if (level == logmod && logmod > 0)
					run_param.long_term_idx_update = -1;
				if (level == logmod - 1 && logmod > 1)
					run_param.long_term_idx_update = 0;

				//if (run_param.long_term_idx_update > logmod) run_param.long_term_idx_update -= logmod+1;
				//run_param.long_term_idx_update = logmod - 0 - level;
				//if (run_param.long_term_idx_update > 0)
				//{
				//    run_param.long_term_idx_update = logmod - run_param.long_term_idx_update;
				//}
				run_param.long_term_idx_use    = fresh[level];
				for (j = level; j <= logmod; j++)
				{
					fresh[j] = run_param.long_term_idx_update;
				}
				if (!i)
				{
					run_param.long_term_idx_use = -1;
				}
				}
		#endif
				error = H264E_encode(enc, scratch, &run_param, &yuv, &coded_data, &sizeof_coded_data);
				assert(!error);

				if (i)
				{
					sum_bytes += sizeof_coded_data - 4;
					if (min_bytes > sizeof_coded_data - 4) min_bytes = sizeof_coded_data - 4;
					if (max_bytes < sizeof_coded_data - 4) max_bytes = sizeof_coded_data - 4;
				}

				if (cmdline->stats)
					printf("frame=%d, bytes=%d\n", frames++, sizeof_coded_data);

				if (fout)
				{
					if (!fwrite(coded_data, sizeof_coded_data, 1, fout))
					{
						printf("ERROR writing output file\n");
						break;
					}
				}
				if (cmdline->psnr)
					psnr_add(buf_save, buf_in, g_w, g_h, sizeof_coded_data);
			}
			//fprintf(stderr, "%d avr = %6d  [%6d %6d]\n", qp, sum_bytes/299, min_bytes, max_bytes);

			if (cmdline->psnr)
				psnr_print(psnr_get());

			if (enc)
				free(enc);
			if (scratch)
				free(scratch);
		}
		free(buf_in);
		free(buf_save);

		if (fin)
			fclose(fin);
		if (fout)
			fclose(fout);
		#if H264E_MAX_THREADS
		if (thread_pool)
		{
			h264e_thread_pool_close(thread_pool, cmdline->threads);
		}
		#endif
		getchar();
	}
	

	lua_State* L = luaL_newstate();
    luaL_openlibs( L );
    luaL_register( L, LIB_NAME, bindFuncs);
    open_file( L, "init.lua" );
    lua_close( L );
	getchar();

    GdiFont* font;
    struct nk_context *ctx;

    WNDCLASSW wc;
    ATOM atom;
    RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exstyle = WS_EX_APPWINDOW;
    HWND wnd;
    HDC dc;
    int running = 1;
    int needs_refresh = 1;

    /* Win32 */
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleW(0);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"NuklearWindowClass";
    atom = RegisterClassW(&wc);
    AdjustWindowRectEx(&rect, style, FALSE, exstyle);
    wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"Nuklear GDI Demo",
        style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, wc.hInstance, NULL);
    dc = GetDC(wnd);
    /* GUI */
    font = nk_gdifont_create("Arial", 14);
    ctx = nk_gdi_init(font, dc, WINDOW_WIDTH, WINDOW_HEIGHT);
    while (running)
    {
        /* Input */
        MSG msg;
        nk_input_begin(ctx);
        if (needs_refresh == 0) {
            if (GetMessageW(&msg, NULL, 0, 0) <= 0)
                running = 0;
            else {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            needs_refresh = 1;
        } else needs_refresh = 0;
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                running = 0;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            needs_refresh = 1;
        }
        nk_input_end(ctx);
        /* GUI */
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 200, 200),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");
            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(ctx, 22, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);
        }
        nk_end(ctx);
        /* -------------- EXAMPLES ---------------- */
        #ifdef INCLUDE_CALCULATOR
          calculator(ctx);
        #endif
        #ifdef INCLUDE_CANVAS
          canvas(ctx);
        #endif
        #ifdef INCLUDE_OVERVIEW
          overview(ctx);
        #endif
        #ifdef INCLUDE_NODE_EDITOR
          node_editor(ctx);
        #endif
        /* ----------------------------------------- */
        /* Draw */
        nk_gdi_render(nk_rgb(30,30,30));
    }
    nk_gdifont_del(font);
    ReleaseDC(wnd, dc);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
	getchar();


    printf("GENANN TEST SUITE\n");
    srand(100); //Repeatable test results.
    lrun("basic", basic);
    lrun("xor", xor);
    lrun("backprop", backprop);
    lrun("train and", train_and);
    lrun("train or", train_or);
    lrun("train xor", train_xor);
    lrun("persist", persist);
    lrun("copy", copy);
    lrun("sigmoid", sigmoid);
    lresults();

    test_pool();
    test_mempool();
    test_malloc();

    printf("Version: %s\n", cJSON_Version());
    create_objects();

    LoggerInit();
    printf("\nERROR, WARNING, SUCCESS LOGS\n");
    printf("========\n");
    LoggerSetLevel(LOGGER_LEVEL_ERROR | LOGGER_LEVEL_WARNING |  LOGGER_LEVEL_SUCCESS);
    LoggerError("Error one : %d two : %f three : %s", 1, 2.0, "three");
    LoggerWarning("warning one : %d two : %f three : %s", 1, 2.0, "three");
    LoggerSuccess("success one : %d two : %f three : %s", 1, 2.0, "three");
    LoggerInfo("Info one : %d two : %f three : %s", 1, 2.0, "three");
    LoggerDebug("Debug one : %d two : %f three : %s", 1, 2.0, "three");
    printf("\nALL LOGS\n");
    printf("========\n");
    LoggerSetLevel(LOGGER_LEVEL_ALL);
    LoggerError("assets path not found %s", "../assets/image.png");
    LoggerWarning("retry fetch, network error");
    LoggerSuccess("data fetched %dms", 230);
    LoggerInfo("init queue");
    LoggerDebug("user email is : %s", "user@test.test");
    LoggerClose();



	int ch, samples = 4,  mode = LIST_MODE;
    int i, result;
    int count;
	// GLFWmonitor** monitors;
    // while ((ch = getopt(argc, argv, "th")) != -1)
    // {
    //     switch (ch)
    //     {
    //         case 'h':
    //             usage();
    //             exit(EXIT_SUCCESS);
    //         case 't':
    //             mode = TEST_MODE;
    //             break;
    //         default:
    //             usage();
    //             exit(EXIT_FAILURE);
    //     }
    // }
    // glfwSetErrorCallback(error_callback);
    // if (!glfwInit())
    //     exit(EXIT_FAILURE);
    // monitors = glfwGetMonitors(&count);
    // for (i = 0;  i < count;  i++)
    // {
    //     if (mode == LIST_MODE)
    //         list_modes(monitors[i]);
    //     else if (mode == TEST_MODE)
    //         test_modes(monitors[i]);
    // }
    // glfwTerminate();

    // GLFWwindow* msaa_window;
    // while ((ch = getopt(argc, argv, "hs:")) != -1)
    // {
    //     switch (ch)
    //     {
    //         case 'h':
    //             usage();
    //             exit(EXIT_SUCCESS);
    //         case 's':
    //             samples = atoi(optarg);
    //             break;
    //         default:
    //             usage();
    //             exit(EXIT_FAILURE);
    //     }
    // }
    // glfwSetErrorCallback(error_callback);
    // if (!glfwInit())
    //     exit(EXIT_FAILURE);
    // if (samples)
    //     printf("Requesting MSAA with %i samples\n", samples);
    // else
    //     printf("Requesting that MSAA not be available\n");
    // glfwWindowHint(GLFW_SAMPLES, samples);
    // glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    // msaa_window = glfwCreateWindow(800, 400, "Aliasing Detector", NULL, NULL);
    // if (!msaa_window)
    // {
    //     glfwTerminate();
    //     exit(EXIT_FAILURE);
    // }
    // glfwSetKeyCallback(msaa_window, key_callback);
    // glfwSetFramebufferSizeCallback(msaa_window, framebuffer_size_callback);
    // glfwMakeContextCurrent(msaa_window);
    // glfwSwapInterval(1);
    // if (!glfwExtensionSupported("GL_ARB_multisample"))
    // {
    //     printf("GL_ARB_multisample extension not supported\n");
    //     glfwTerminate();
    //     exit(EXIT_FAILURE);
    // }
    // glfwShowWindow(msaa_window);
    // glGetIntegerv(GL_SAMPLES_ARB, &samples);
    // if (samples)
    //     printf("Context reports MSAA is available with %i samples\n", samples);
    // else
    //     printf("Context reports MSAA is unavailable\n");
    // glMatrixMode(GL_PROJECTION);
    // glOrtho(0.f, 1.f, 0.f, 0.5f, 0.f, 1.f);
    // glMatrixMode(GL_MODELVIEW);
    // while (!glfwWindowShouldClose(msaa_window))
    // {
    //     GLfloat time = (GLfloat) glfwGetTime();
    //     glClear(GL_COLOR_BUFFER_BIT);
    //     glLoadIdentity();
    //     glTranslatef(0.25f, 0.25f, 0.f);
    //     glRotatef(time, 0.f, 0.f, 1.f);
    //     glDisable(GL_MULTISAMPLE_ARB);
    //     glRectf(-0.15f, -0.15f, 0.15f, 0.15f);
    //     glLoadIdentity();
    //     glTranslatef(0.75f, 0.25f, 0.f);
    //     glRotatef(time, 0.f, 0.f, 1.f);
    //     glEnable(GL_MULTISAMPLE_ARB);
    //     glRectf(-0.15f, -0.15f, 0.15f, 0.15f);
    //     glfwSwapBuffers(window);
    //     glfwPollEvents();
    // }
    // glfwTerminate();

    Slot* slots;
    GLFWmonitor* monitor = NULL;
    int swidth, sheight, scount = 1;
    setlocale(LC_ALL, "");

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    printf("Library initialized\n");
    glfwSetMonitorCallback(monitor_callback);
    while ((ch = getopt(argc, argv, "hfn:")) != -1)
    {
        switch (ch)
        {
            case 'h':
                usage();
                exit(EXIT_SUCCESS);
            case 'f':
                monitor = glfwGetPrimaryMonitor();
                break;
            case 'n':
                scount = (int) strtol(optarg, NULL, 10);
                break;
            default:
                usage();
                exit(EXIT_FAILURE);
        }
    }
    if (monitor)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        swidth = mode->width;
        sheight = mode->height;
    }
    else
    {
        swidth  = 640;
        sheight = 480;
    }
    if (!scount)
    {
        fprintf(stderr, "Invalid user\n");
        exit(EXIT_FAILURE);
    }
    slots = calloc(scount, sizeof(Slot));
    for (i = 0;  i < scount;  i++)
    {
        char title[128];
        slots[i].closeable = GL_TRUE;
        slots[i].number = i + 1;
        sprintf(title, "Event Linter (Window %i)", slots[i].number);
        if (monitor)
        {
            printf("Creating full screen window %i (%ix%i on %s)\n",
                   slots[i].number,
                   swidth, sheight,
                   glfwGetMonitorName(monitor));
        }
        else
        {
            printf("Creating windowed mode window %i (%ix%i)\n",
                   slots[i].number,
                   swidth, sheight);
        }
        slots[i].window = glfwCreateWindow(swidth, sheight, title, monitor, NULL);
        if (!slots[i].window)
        {
            free(slots);
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwSetWindowUserPointer(slots[i].window, slots + i);
        glfwSetWindowPosCallback(slots[i].window, window_pos_callback);
        glfwSetWindowSizeCallback(slots[i].window, window_size_callback);
        glfwSetFramebufferSizeCallback(slots[i].window, sframebuffer_size_callback);
        glfwSetWindowCloseCallback(slots[i].window, window_close_callback);
        glfwSetWindowRefreshCallback(slots[i].window, window_refresh_callback);
        glfwSetWindowFocusCallback(slots[i].window, window_focus_callback);
        glfwSetWindowIconifyCallback(slots[i].window, window_iconify_callback);
        glfwSetMouseButtonCallback(slots[i].window, mouse_button_callback);
        glfwSetCursorPosCallback(slots[i].window, cursor_position_callback);
        glfwSetCursorEnterCallback(slots[i].window, cursor_enter_callback);
        glfwSetScrollCallback(slots[i].window, scroll_callback);
        glfwSetKeyCallback(slots[i].window, skey_callback);
        glfwSetCharCallback(slots[i].window, char_callback);
        glfwSetCharModsCallback(slots[i].window, char_mods_callback);
        glfwSetDropCallback(slots[i].window, drop_callback);
        glfwMakeContextCurrent(slots[i].window);
        glfwSwapInterval(1);
    }
    printf("Main loop starting\n");
    for (;;)
    {
        for (i = 0;  i < scount;  i++)
        {
            if (glfwWindowShouldClose(slots[i].window))
                break;
        }
        if (i < scount)
            break;
        glfwWaitEvents();
        fflush(stdout);
    }
    free(slots);
    glfwTerminate();


    GLFWwindow* joystick_window;
    memset(joysticks, 0, sizeof(joysticks));

    Thread threads[] =
    {
        { NULL, "Red", 1.f, 0.f, 0.f, 0 },
        { NULL, "Green", 0.f, 1.f, 0.f, 0 },
        { NULL, "Blue", 0.f, 0.f, 1.f, 0 },
        { NULL, "YELLOW", 0.5f, 0.5f, 1.f, 0 }
    };
    const int mycount = sizeof(threads) / sizeof(Thread);

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    joystick_window = glfwCreateWindow(640, 480, "Joystick Test", NULL, NULL);
    if (!joystick_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetFramebufferSizeCallback(joystick_window, framebuffer_size_callback);
    glfwMakeContextCurrent(joystick_window);
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(joystick_window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        refresh_joysticks();
        draw_joysticks(joystick_window);
        glfwSwapBuffers(joystick_window);
        glfwPollEvents();
    }
    glfwTerminate();



    GLFWwindow* cursor_window;
    GLFWcursor* star_cursors[CURSOR_FRAME_COUNT];
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    for (i = 0;  i < CURSOR_FRAME_COUNT;  i++)
    {
        star_cursors[i] = create_cursor_frame(i / (float) CURSOR_FRAME_COUNT);
        if (!star_cursors[i])
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
    }
    for (i = 0;  i < sizeof(standard_cursors) / sizeof(standard_cursors[0]);  i++)
    {
        const int shapes[] = {
            GLFW_ARROW_CURSOR,
            GLFW_IBEAM_CURSOR,
            GLFW_CROSSHAIR_CURSOR,
            GLFW_HAND_CURSOR,
            GLFW_HRESIZE_CURSOR,
            GLFW_VRESIZE_CURSOR
        };
        standard_cursors[i] = glfwCreateStandardCursor(shapes[i]);
        if (!standard_cursors[i])
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
    }
    cursor_window = glfwCreateWindow(640, 480, "Cursor Test", NULL, NULL);
    if (!cursor_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(cursor_window);
    glfwGetCursorPos(cursor_window, &cursor_x, &cursor_y);
    printf("Cursor position: %f %f\n", cursor_x, cursor_y);
    glfwSetCursorPosCallback(cursor_window, ccursor_position_callback);
    glfwSetKeyCallback(cursor_window, ckey_callback);
    while (!glfwWindowShouldClose(cursor_window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        if (track_cursor)
        {
            int wnd_width, wnd_height, fb_width, fb_height;
            float scale;
            glfwGetWindowSize(cursor_window, &wnd_width, &wnd_height);
            glfwGetFramebufferSize(cursor_window, &fb_width, &fb_height);
            scale = (float) fb_width / (float) wnd_width;
            glViewport(0, 0, fb_width, fb_height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0.f, fb_width, 0.f, fb_height, 0.f, 1.f);
            glBegin(GL_LINES);
            glVertex2f(0.f, (GLfloat) (fb_height - cursor_y * scale));
            glVertex2f((GLfloat) fb_width, (GLfloat) (fb_height - cursor_y * scale));
            glVertex2f((GLfloat) cursor_x * scale, 0.f);
            glVertex2f((GLfloat) cursor_x * scale, (GLfloat) fb_height);
            glEnd();
        }
        glfwSwapBuffers(cursor_window);
        if (animate_cursor)
        {
            const int i = (int) (glfwGetTime() * 30.0) % CURSOR_FRAME_COUNT;
            glfwSetCursor(cursor_window, star_cursors[i]);
        }
        if (wait_events)
            glfwWaitEvents();
        else
            glfwPollEvents();
        // Workaround for an issue with msvcrt and mintty
        fflush(stdout);
    }
    glfwDestroyWindow(cursor_window);
    for (i = 0;  i < CURSOR_FRAME_COUNT;  i++)
        glfwDestroyCursor(star_cursors[i]);
    for (i = 0;  i < sizeof(standard_cursors) / sizeof(standard_cursors[0]);  i++)
        glfwDestroyCursor(standard_cursors[i]);
    glfwTerminate();



    GLFWwindow* clipboard_window;
    while ((ch = getopt(argc, argv, "h")) != -1)
    {
        switch (ch)
        {
            case 'h':
                clipboard_usage();
                exit(EXIT_SUCCESS);

            default:
                clipboard_usage();
                exit(EXIT_FAILURE);
        }
    }
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }
    clipboard_window = glfwCreateWindow(200, 200, "Clipboard Test", NULL, NULL);
    if (!clipboard_window)
    {
        glfwTerminate();

        fprintf(stderr, "Failed to open GLFW window\n");
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(clipboard_window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(clipboard_window, cb_key_callback);
    glfwSetFramebufferSizeCallback(clipboard_window, framebuffer_size_callback);
    glMatrixMode(GL_PROJECTION);
    glOrtho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.5f, 0.5f, 0.5f, 0);
    while (!glfwWindowShouldClose(clipboard_window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(0.8f, 0.2f, 0.4f);
        glRectf(-0.5f, -0.5f, 0.5f, 0.5f);
        glfwSwapBuffers(clipboard_window);
        glfwWaitEvents();
    }
    glfwTerminate();


    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    for (i = 0;  i < mycount;  i++)
    {
        int left, top, right, bottom;
        threads[i].window = glfwCreateWindow(200, 200,
                                             threads[i].title,
                                             NULL, NULL);
        if (!threads[i].window)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwSetKeyCallback(threads[i].window, key_callback);
        // glfwMakeContextCurrent(threads[i].window);
        // glClearColor(colors[i].r, colors[i].g, colors[i].b, 1.f);
        glfwSetWindowPos(threads[i].window, 200 + 250 * i, 200);
        glfwShowWindow(threads[i].window);
        glfwGetWindowFrameSize(threads[i].window, &left, &top, &right, &bottom);
        glfwSetWindowPos(threads[i].window,
                         100 + (i & 1) * (200 + left + right),
                         100 + (i >> 1) * (200 + top + bottom));
        if (thrd_create(&threads[i].id, thread_main, threads + i) !=
            thrd_success)
        {
            fprintf(stderr, "Failed to create secondary thread\n");

            glfwTerminate();
            exit(EXIT_FAILURE);
        }
    }

    while (running)
    {
        // glfwWaitEvents();
        for (i = 0;  i < mycount;  i++)
        {
            glfwMakeContextCurrent(threads[i].window);
            glClear(GL_COLOR_BUFFER_BIT);
            glfwSwapBuffers(threads[i].window);
            if (glfwWindowShouldClose(threads[i].window))
                running = GL_FALSE;
        }
        // glfwPollEvents();
    }
    for (i = 0;  i < mycount;  i++)
        glfwHideWindow(threads[i].window);
    for (i = 0;  i < mycount;  i++)
        thrd_join(threads[i].id, &result);

    return lfails != 0;
}
