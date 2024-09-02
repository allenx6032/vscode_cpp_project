#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <GL/glfw.h>
#include <math.h>
#include <ctype.h>
#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <winerror.h>
#include <Windows.h>
#include <malloc.h>

#define MINIGLUT_IMPL
#include "miniglut.h"

#define MINICORO_IMPL
#include "minicoro.h"

#define MINILUA_IMPL
#include "minilua.h"

#define C89THREAD_IMPLEMENTATION
#include "c89thread.h"

#define C89ATOMIC_IMPLEMENTATION
#include "c89atomic.h"

#ifndef M_PI
#define M_PI 3.141592654
#endif

static int running = 1;
static int autoexit = 0;


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


// int
// script_pushobject(lua_State *L, void * object) {
//     void **ud;
//     if (luaL_newmetatable(L, "script")) {
//         // 在注册表中创建一个表存放所有的 object 指针到 userdata 的关系。
//         // 这个表应该是一个 weak table ，当 Lua 中不再存在对 C 对象的引用会删除对应的记录。
//         lua_newtable(L);
//         lua_pushliteral(L, "kv");
//         lua_setfield(L, -2, "__mode");
//         lua_setmetatable(L, -2);
//     }
//     lua_rawgetp(L,-1,object);
//     if (lua_type(L,-1)==LUA_TUSERDATA) {
//         ud = (void **)lua_touserdata(L,-1);
//         if (*ud == object) {
//             lua_replace(L, -2);
//             return 0;
//         }
//         // C 对象指针被释放后，有可能地址被重用。
//         // 这个时候，可能取到曾经保存起来的 userdata ，里面的指针必然为空。
//         assert(*ud == NULL);
//     }
//     ud = (void **)lua_newuserdata(L, sizeof(void*));
//     *ud = object;
//     lua_pushvalue(L, -1);
//     lua_rawsetp(L, -4, object);
//     lua_replace(L, -3);
//     lua_pop(L,1);
//     return 1;
// }

// void *
// script_toobject(lua_State *L, int index) {
//     void **ud = (void **)lua_touserdata(L,index);
//     if (ud == NULL)
//         return NULL;
//     // 如果 object 已在 C 代码中销毁，*ud 为 NULL 。
//     return *ud;
// }

// void
// script_deleteobject(lua_State *L, void *object) {
//     luaL_getmetatable(L, "script");
//     if (lua_istable(L,-1)) {
//         lua_rawgetp(L, -1, object);
//         if (lua_type(L,-1) == LUA_TUSERDATA) {
//             void **ud = (void **)lua_touserdata(L,-1);
//             // 这个 assert 防止 deleteobject 被重复调用。
//             assert(*ud == object);
//             // 销毁一个被 Lua 引用住的对象，只需要把 *ud 置为 NULL 。
//             *ud = NULL;
//         }
//         lua_pop(L,2);
//     } else {
//         // 有可能从未调用过 pushobject ，此时注册表中 script 项尚未建立。
//         lua_pop(L,1);
//     }
// }

// static size_t allmem;

// static void *debug_alloc(NUIparams *params, void *ptr, size_t nsize, size_t osize) {
//     if (params->S == NULL) {
//         allmem = 0;
//         printf("[M] *** state init ***\n");
//     }
//     if (nsize == 0) {
//         allmem -= osize;
//         printf("[M] free:    %d (%d)\n", (int)allmem, (int)osize);
//         if (ptr == params->S) {
//             assert(allmem == 0);
//             printf("[M] *** state free ***\n");
//         }
//         free(ptr);
//         return NULL;
//     }
//     if (ptr == NULL) {
//         assert(osize == 0);
//         allmem += nsize;
//         printf("[M] malloc:  %d (%d)\n", (int)allmem, (int)nsize);
//         return malloc(nsize);
//     }
//     allmem += nsize;
//     allmem -= osize;
//     printf("[M] realloc: %d (%d->%d)\n", (int)allmem, (int)osize, (int)nsize);
//     return realloc(ptr, nsize);
// }

// typedef struct NUIcomp_name {
//     NUIcomp base;
//     const char *name;
// } NUIcomp_name;

// static void putname(NUInode *n) {
//     NUIstate *S = nui_state(n);
//     NUItype *t;
//     NUIcomp_name *comp = NULL;
//     const char *name = NULL;
//     if (S == NULL) { printf("NULL"); return; }
//     t = nui_gettype(S, NUI_(name));
//     if (t) comp = (NUIcomp_name*)nui_getcomp(n, t);
//     if (comp) name = comp->name;
//     if (name != NULL)
//         printf("%s", (char*)name);
//     else
//         printf("node: %p", n);
// }

// static NUItype *open_name_type(NUIstate *S) {
//     return nui_newtype(S, NUI_(name), 0, sizeof(NUIcomp_name));
// }

// static NUInode *new_named_node(NUIstate *S, const char *name) {
//     NUInode *n = nui_newnode(S);
//     NUIcomp_name *comp = (NUIcomp_name*)nui_addcomp(n, nui_gettype(S, NUI_(name)));
//     comp->name = name;
//     nui_setparent(n, nui_rootnode(S));
//     return n;
// }

// static void *get_event_data(const NUIevent *evt, void *key) {
//     const NUIptrentry *e = (NUIptrentry*)nui_gettable(nui_eventdata(evt), key);
//     return e ? e->value : NULL;
// }

// static void set_event_data(NUIstate *S, NUIevent *evt, void *key, void *value) {
//     NUIptrentry *e = (NUIptrentry*)nui_settable(S, nui_eventdata(evt), key);
//     if (e) e->value = value;
// }

// static void add_child(void *ud, NUInode *n, const NUIevent *evt) {
//     NUIstate *S = nui_state(n);
//     printf("add_child:\t");
//     putname(nui_eventnode(evt));
//     printf(" <- ");
//     putname((NUInode*)get_event_data(evt, NUI_(child)));
//     printf("\n");
// }

// static void remove_child(void *ud, NUInode *n, const NUIevent *evt) {
//     NUIstate *S = nui_state(n);
//     printf("remove_child:\t");
//     putname(nui_eventnode(evt));
//     printf(" <- ");
//     putname((NUInode*)get_event_data(evt, NUI_(child)));
//     printf("\n");
// }

// static void delete_node(void *ud, NUInode *n, const NUIevent *evt) {
//     printf("delete:\t\t"); putname(nui_eventnode(evt)); printf("\n");
// }

// static int add_listener(NUIstate *S) {
//     nui_addhandler(nui_rootnode(S), NUI_(add_child), 1, add_child, NULL);
//     nui_addhandler(nui_rootnode(S), NUI_(remove_child), 1, remove_child, NULL);
//     nui_addhandler(nui_rootnode(S), NUI_(delete_node), 1, delete_node, NULL);
//     nui_addhandler(nui_rootnode(S), NUI_(add_child), 0, add_child, NULL);
//     nui_addhandler(nui_rootnode(S), NUI_(remove_child), 0, remove_child, NULL);
//     nui_addhandler(nui_rootnode(S), NUI_(delete_node), 0, delete_node, NULL);
//     return 1;
// }

// static int tracked_node = 0;

// static void track_node_delete(void *ud, NUInode *n, const NUIevent *evt) {
//     printf("node: %p deleted\n", n);
//     --tracked_node;
// }

// static NUInode *new_track_node(NUIstate *S) {
//     NUInode *n = nui_newnode(S);
//     nui_addhandler(n, NUI_(delete_node), 0, track_node_delete, NULL);
//     printf("new node: %p\n", n);
//     ++tracked_node;
//     return n;
// }

// static void test_mem(void) {
//     NUIparams params = { debug_alloc };
//     NUIstate *S = nui_newstate(&params);
//     printf("new state: %p\n", S);
//     NUItimer *timer = nui_newtimer(S, NULL, NULL);
//     printf("new timer: %p\n", timer);
//     NUIkey *name = NUI_(observer);
//     printf("new key: %p\n", name);
//     printf("new key: %p\n", NUI_(observer));
//     NUInode *n = new_track_node(S); /* a pending node */
//     n = new_track_node(S);
//     NUIevent evt; nui_initevent(&evt, NUI_(test), 1, 1);
//     set_event_data(S, &evt, NUI_(foo), nui_newfstring(S, "bar"));
//     nui_emitevent(n, &evt);
//     printf("new event: %p\n", &evt);
//     nui_deldata(S, get_event_data(&evt, NUI_(foo)));
//     nui_freeevent(S, &evt);
//     NUIattr attr = { NULL };
//     nui_setattr(n, NUI_(attr), &attr);
//     nui_delattr(n, NUI_(attr));
//     printf("new attr: %p\n", &attr);
//     nui_release(n);
//     printf("release node\n");
//     NUItype *t = nui_newtype(S, NUI_(observer), 0, sizeof(NUIcomp_name));
//     printf("new type: %p\n", t);
//     NUInode *n1 = new_track_node(S);
//     nui_setparent(n1, nui_rootnode(S));
//     NUIcomp *comp1 = nui_addcomp(n1, t);
//     printf("new node with comp: %p\n", comp1);
//     NUIcomp *comp = nui_getcomp(n1, t);
//     assert(comp == comp1);
//     assert(comp == nui_addcomp(n1, t));
//     NUInode *n2 = new_track_node(S);
//     nui_setparent(n2, n1);
//     comp = nui_addcomp(n2, t);
//     assert(comp == nui_getcomp(n2, t));
//     nui_close(S);
//     assert(tracked_node == 0);
// }

// static void test_node(void) {
//     NUIparams params = { debug_alloc };
//     NUIstate *S = nui_newstate(&params);
//     NUItype *t = open_name_type(S);
//     ((NUIcomp_name*)nui_addcomp(nui_rootnode(S), t))->name = "root";
//     add_listener(S);
//     printf("-------------\n");
//     NUInode *n = new_named_node(S, "n");
//     printf("-------------\n");                                  
//     NUInode *n1 = new_named_node(S, "n1");
//     printf("-------------\n");
//     NUInode *n2 = new_named_node(S, "n2");
//     printf("-------------\n");
//     NUInode *n3 = new_named_node(S, "n3");
//     printf("-------------\n");
//     NUInode *n4 = new_named_node(S, "n4");
//     printf("-------------\n");
//     nui_setparent(n1, n);
//     nui_setparent(n2, n);
//     nui_setparent(n3, n);
//     assert(nui_childcount(n) == 3);
//     assert(nui_nodeindex(n1) == 0);
//     assert(nui_nodeindex(n2) == 1);
//     assert(nui_nodeindex(n3) == 2);
//     assert(nui_nodeindex(nui_indexnode(n, 0)) == 0);
//     assert(nui_nodeindex(nui_indexnode(n, 1)) == 1);
//     assert(nui_nodeindex(nui_indexnode(n, 2)) == 2);
//     nui_setchildren(n4, n1);
//     nui_setparent(n4, NULL);
//     assert(nui_childcount(n) == 0);
//     assert(nui_nodeindex(n4) == -1);
//     nui_setparent(n1, n);
//     assert(nui_childcount(n) == 1);
//     assert(nui_nodeindex(n1) == 0);
//     nui_insert(n1, n2);
//     assert(nui_childcount(n) == 2);
//     assert(nui_nodeindex(n2) == 0);
//     assert(nui_nodeindex(n1) == 1);
//     nui_append(n1, n3);
//     assert(nui_childcount(n) == 3);
//     assert(nui_nodeindex(n3) == 2);
//     nui_close(S);
// }

// static void on_foo(void *ud, NUInode *n, const NUIevent *evt) {
//     NUIstate *S = nui_state(n);
//     NUIevent new_evt; nui_initevent(&new_evt, NUI_(foo), 0, 0);
//     nui_emitevent(n, &new_evt); /* should loop */
//     nui_freeevent(S, &new_evt);
// }

// static void on_remove_child(void *ud, NUInode *n, const NUIevent *evt) {
//     NUIstate *S = nui_state(n);
//     NUInode *child = (NUInode*)((NUIptrentry*)nui_gettable(nui_eventdata(evt), NUI_(child)))->value;
//     nui_detach(child); /* trigger new event, should loop */
// }

// static void test_event(void) {
//     NUIparams params = { debug_alloc };
//     NUIstate *S = nui_newstate(&params);
//     nui_addhandler(nui_rootnode(S), NUI_(foo), 0, on_foo, NULL);
//     printf("trigger loop event\n");
//     on_foo(NULL, nui_rootnode(S), NULL); /* emit first event */
//     nui_addhandler(nui_rootnode(S), NUI_(remove_child), 0, on_remove_child, NULL);
//     NUInode *n1 = nui_newnode(S);
//     nui_setparent(n1, nui_rootnode(S));
//     NUInode *n2 = nui_newnode(S);
//     nui_setparent(n2, nui_rootnode(S));
//     printf("trigger loop event by changing node\n");
//     nui_setparent(n1, n2);
//     assert(nui_parent(n1) == n2); /* event handler should have no effect */

//     nui_close(S);
// }

// static NUItime on_timer(void *ud, NUItimer *t, NUItime elapsed) {
//     printf("on_timer: %p: %u\n", t, elapsed);
//     return ud ? 1000 : 0;
// }

// static void test_timer(void) {
//     NUIparams params = { debug_alloc };
//     NUIstate *S = nui_newstate(&params);
//     nui_starttimer(nui_newtimer(S, on_timer, NULL), 1000);
//     nui_starttimer(nui_newtimer(S, on_timer, NULL), 2000);
//     nui_starttimer(nui_newtimer(S, on_timer, NULL), 3000);
//     nui_starttimer(nui_newtimer(S, on_timer, NULL), 4000);
//     nui_starttimer(nui_newtimer(S, on_timer, NULL), 5000);
//     nui_starttimer(nui_newtimer(S, on_timer, NULL), 4000);
//     nui_loop(S);
//     nui_close(S);
// }


static void
gear(GLfloat inner_radius, GLfloat outer_radius, GLfloat width,
  GLint teeth, GLfloat tooth_depth)
{
  GLint i;
  GLfloat r0, r1, r2;
  GLfloat angle, da;
  GLfloat u, v, len;
  r0 = inner_radius;
  r1 = outer_radius - tooth_depth / 2.f;
  r2 = outer_radius + tooth_depth / 2.f;
  da = 2.f * (float) M_PI / teeth / 4.f;
  glShadeModel(GL_FLAT);
  glNormal3f(0.f, 0.f, 1.f);
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.f * (float) M_PI / teeth;
    glVertex3f(r0 * (float) cos(angle), r0 * (float) sin(angle), width * 0.5f);
    glVertex3f(r1 * (float) cos(angle), r1 * (float) sin(angle), width * 0.5f);
    if (i < teeth) {
      glVertex3f(r0 * (float) cos(angle), r0 * (float) sin(angle), width * 0.5f);
      glVertex3f(r1 * (float) cos(angle + 3 * da), r1 * (float) sin(angle + 3 * da), width * 0.5f);
    }
  }
  glEnd();
  glBegin(GL_QUADS);
  da = 2.f * (float) M_PI / teeth / 4.f;
  for (i = 0; i < teeth; i++) {
    angle = i * 2.f * (float) M_PI / teeth;
    glVertex3f(r1 * (float) cos(angle), r1 * (float) sin(angle), width * 0.5f);
    glVertex3f(r2 * (float) cos(angle + da), r2 * (float) sin(angle + da), width * 0.5f);
    glVertex3f(r2 * (float) cos(angle + 2 * da), r2 * (float) sin(angle + 2 * da), width * 0.5f);
    glVertex3f(r1 * (float) cos(angle + 3 * da), r1 * (float) sin(angle + 3 * da), width * 0.5f);
  }
  glEnd();
  glNormal3f(0.0, 0.0, -1.0);
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.f * (float) M_PI / teeth;
    glVertex3f(r1 * (float) cos(angle), r1 * (float) sin(angle), -width * 0.5f);
    glVertex3f(r0 * (float) cos(angle), r0 * (float) sin(angle), -width * 0.5f);
    if (i < teeth) {
      glVertex3f(r1 * (float) cos(angle + 3 * da), r1 * (float) sin(angle + 3 * da), -width * 0.5f);
      glVertex3f(r0 * (float) cos(angle), r0 * (float) sin(angle), -width * 0.5f);
    }
  }
  glEnd();
  glBegin(GL_QUADS);
  da = 2.f * (float) M_PI / teeth / 4.f;
  for (i = 0; i < teeth; i++) {
    angle = i * 2.f * (float) M_PI / teeth;
    glVertex3f(r1 * (float) cos(angle + 3 * da), r1 * (float) sin(angle + 3 * da), -width * 0.5f);
    glVertex3f(r2 * (float) cos(angle + 2 * da), r2 * (float) sin(angle + 2 * da), -width * 0.5f);
    glVertex3f(r2 * (float) cos(angle + da), r2 * (float) sin(angle + da), -width * 0.5f);
    glVertex3f(r1 * (float) cos(angle), r1 * (float) sin(angle), -width * 0.5f);
  }
  glEnd();
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i < teeth; i++) {
    angle = i * 2.f * (float) M_PI / teeth;
    glVertex3f(r1 * (float) cos(angle), r1 * (float) sin(angle), width * 0.5f);
    glVertex3f(r1 * (float) cos(angle), r1 * (float) sin(angle), -width * 0.5f);
    u = r2 * (float) cos(angle + da) - r1 * (float) cos(angle);
    v = r2 * (float) sin(angle + da) - r1 * (float) sin(angle);
    len = (float) sqrt(u * u + v * v);
    u /= len;
    v /= len;
    glNormal3f(v, -u, 0.0);
    glVertex3f(r2 * (float) cos(angle + da), r2 * (float) sin(angle + da), width * 0.5f);
    glVertex3f(r2 * (float) cos(angle + da), r2 * (float) sin(angle + da), -width * 0.5f);
    glNormal3f((float) cos(angle), (float) sin(angle), 0.f);
    glVertex3f(r2 * (float) cos(angle + 2 * da), r2 * (float) sin(angle + 2 * da), width * 0.5f);
    glVertex3f(r2 * (float) cos(angle + 2 * da), r2 * (float) sin(angle + 2 * da), -width * 0.5f);
    u = r1 * (float) cos(angle + 3 * da) - r2 * (float) cos(angle + 2 * da);
    v = r1 * (float) sin(angle + 3 * da) - r2 * (float) sin(angle + 2 * da);
    glNormal3f(v, -u, 0.f);
    glVertex3f(r1 * (float) cos(angle + 3 * da), r1 * (float) sin(angle + 3 * da), width * 0.5f);
    glVertex3f(r1 * (float) cos(angle + 3 * da), r1 * (float) sin(angle + 3 * da), -width * 0.5f);
    glNormal3f((float) cos(angle), (float) sin(angle), 0.f);
  }
  glVertex3f(r1 * (float) cos(0), r1 * (float) sin(0), width * 0.5f);
  glVertex3f(r1 * (float) cos(0), r1 * (float) sin(0), -width * 0.5f);
  glEnd();
  glShadeModel(GL_SMOOTH);
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.f * (float) M_PI / teeth;
    glNormal3f(-(float) cos(angle), -(float) sin(angle), 0.f);
    glVertex3f(r0 * (float) cos(angle), r0 * (float) sin(angle), -width * 0.5f);
    glVertex3f(r0 * (float) cos(angle), r0 * (float) sin(angle), width * 0.5f);
  }
  glEnd();
}

static GLfloat view_rotx = 20.f, view_roty = 30.f, view_rotz = 0.f;
static GLint gear1, gear2, gear3;
static GLfloat angle = 0.f;

static void draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  glRotatef(view_rotx, 1.0, 0.0, 0.0);
  glRotatef(view_roty, 0.0, 1.0, 0.0);
  glRotatef(view_rotz, 0.0, 0.0, 1.0);
  glPushMatrix();
  glTranslatef(-3.0, -2.0, 0.0);
  glRotatef(angle, 0.0, 0.0, 1.0);
  glCallList(gear1);
  glPopMatrix();
  glPushMatrix();
  glTranslatef(3.1f, -2.f, 0.f);
  glRotatef(-2.f * angle - 9.f, 0.f, 0.f, 1.f);
  glCallList(gear2);
  glPopMatrix();
  glPushMatrix();
  glTranslatef(-3.1f, 4.2f, 0.f);
  glRotatef(-2.f * angle - 25.f, 0.f, 0.f, 1.f);
  glCallList(gear3);
  glPopMatrix();
  glPopMatrix();
}

static void animate(void)
{
  angle = 100.f * (float) glfwGetTime();
}

void key( int k, int action )
{
  if( action != GLFW_PRESS ) return;
  switch (k) {
  case 'Z':
    if( glfwGetKey( GLFW_KEY_LSHIFT ) )
      view_rotz -= 5.0;
    else
      view_rotz += 5.0;
    break;
  case GLFW_KEY_ESC:
    running = 0;
    break;
  case GLFW_KEY_UP:
    view_rotx += 5.0;
    break;
  case GLFW_KEY_DOWN:
    view_rotx -= 5.0;
    break;
  case GLFW_KEY_LEFT:
    view_roty += 5.0;
    break;
  case GLFW_KEY_RIGHT:
    view_roty -= 5.0;
    break;
  default:
    return;
  }
}

void reshape( int width, int height )
{
  GLfloat h = (GLfloat) height / (GLfloat) width;
  GLfloat xmax, znear, zfar;
  znear = 5.0f;
  zfar  = 30.0f;
  xmax  = znear * 0.5f;
  glViewport( 0, 0, (GLint) width, (GLint) height );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glFrustum( -xmax, xmax, -xmax*h, xmax*h, znear, zfar );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glTranslatef( 0.0, 0.0, -20.0 );
}

static void init(int argc, char *argv[])
{
  static GLfloat pos[4] = {5.f, 5.f, 10.f, 0.f};
  static GLfloat red[4] = {0.8f, 0.1f, 0.f, 1.f};
  static GLfloat green[4] = {0.f, 0.8f, 0.2f, 1.f};
  static GLfloat blue[4] = {0.2f, 0.2f, 1.f, 1.f};
  GLint i;
  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  gear1 = glGenLists(1);
  glNewList(gear1, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
  gear(1.f, 4.f, 1.f, 20, 0.7f);
  glEndList();
  gear2 = glGenLists(1);
  glNewList(gear2, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
  gear(0.5f, 2.f, 2.f, 10, 0.7f);
  glEndList();
  gear3 = glGenLists(1);
  glNewList(gear3, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
  gear(1.3f, 2.f, 0.5f, 10, 0.7f);
  glEndList();
  glEnable(GL_NORMALIZE);
  for ( i=1; i<argc; i++ ) {
    if (strcmp(argv[i], "-info")==0) {
      printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
      printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
      printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
      printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
    }
    else if ( strcmp(argv[i], "-exit")==0) {
      autoexit = 30;
      printf("Auto Exit after %i seconds.\n", autoexit );
    }
  }
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



// #define LIB_NAME "clua"
// static void open_file(lua_State* L, const char* filename){
// 	if (luaL_dofile(L, filename) != LUA_OK){
// 		fprintf(stderr, "%s\n", lua_tostring(L, -1));
// 	}
// }

// int l_system( lua_State* L )
// {
// 	if ( lua_gettop( L ) != 1 )
// 	{
// 		return luaL_error(L, "expected 1 argument");
// 	}
// 	if ( lua_type( L, -1 ) != LUA_TSTRING )
// 	{
// 		return luaL_error( L, "argument 1 - this isn't a string" );
// 	}
// 	const char* str = lua_tostring( L, -1 );
// 	system( str );
// 	return 1;
// }

// int l_sleep( lua_State* L )
// {
// 	if ( lua_gettop( L ) != 1 )
// 	{
// 		return luaL_error( L, "expected 1 argument" );
// 	}
// 	if ( lua_type( L, -1 ) != LUA_TNUMBER )
// 	{
// 		return luaL_error( L, "argument 1 - this isn't a number" );
// 	}
// 	const float time = lua_tonumber( L, -1 );
// 	Sleep( ( DWORD )time );
// 	return 1;
// }

// int l_setcolor( lua_State* L )
// {
// 	if ( lua_gettop( L ) != 1 )
// 	{
// 		return luaL_error( L, "expected 1 argument" );
// 	}
// 	if ( lua_type( L, -1 ) != LUA_TNUMBER )
// 	{
// 		return luaL_error( L, "argument 1 - this isn't a number" );
// 	}
// 	const float num = lua_tonumber( L, -1 );
// 	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), (int)num );
// 	return 1;
// }

// int l_openfile( lua_State* L )
// {
// 	if ( lua_gettop( L ) != 1 )
// 	{
// 		return luaL_error( L, "expected 1 argument" );
// 	}
// 	if ( lua_type( L, -1 ) != LUA_TSTRING )
// 	{
// 		return luaL_error( L, "argument 1 - this isn't a string" );
// 	}
// 	const char* filename = lua_tostring( L, -1 );
// 	FILE* handle;
// 	handle = fopen( filename, "r" );
// 	if (handle == NULL){
// 		//no such file!
// 		lua_getglobal(L, "print");
// 		lua_pushstring(L, "File reading error");
// 		lua_pcall(L, 1, 1, 0);
// 		return 1;
// 	}
// 	char buffer[1024];
// 	int i = 0;
// 	while (!feof(handle)){
// 		buffer[i] = (char)fgetc(handle);
// 		i++;
// 	}
// 	buffer[i] = '\0';
// 	lua_pushstring( L, buffer );
// 	fclose(handle);
// 	return 1;
// }

// int l_appendfile(lua_State* L){
// 	if (lua_gettop(L) != 2){
// 		return luaL_error(L, "expected 2 arguments");
// 	}
// 	if (lua_type(L, -1) != LUA_TSTRING){
// 		return luaL_error(L, "argument 2 - this isn't a string");
// 	}
// 	if (lua_type(L, -2) != LUA_TSTRING){
// 		return luaL_error(L, "argument 1 - this isn't a string");
// 	}
// 	const char* content = lua_tostring(L, -1);
// 	const char* filename = lua_tostring(L, -2);
// 	FILE* handle;
// 	handle = fopen(filename, "a");
// 	if (handle == NULL){
// 		lua_getglobal(L, "print");
// 		lua_pushstring(L, "File writing error");
// 		lua_pcall(L, 1, 1, 0);
// 		return 2;
// 	}
// 	fputs(content, handle);
// 	fclose(handle);
// 	return 2;
// }

// int l_writefile(lua_State* L){
// 	if (lua_gettop(L) != 2){
// 		return luaL_error(L, "expected 2 arguments");
// 	}
// 	if (lua_type(L, -1) != LUA_TSTRING){
// 		return luaL_error(L, "argument 2 - this isn't a string");
// 	}
// 	if (lua_type(L, -2) != LUA_TSTRING){
// 		return luaL_error(L, "argument 1 - this isn't a string");
// 	}
// 	const char* content = lua_tostring(L, -1);
// 	const char* filename = lua_tostring(L, -2);
// 	FILE* handle;
// 	handle = fopen(filename, "w");
// 	if (handle == NULL){
// 		lua_getglobal(L, "print");
// 		lua_pushstring(L, "File writing error");
// 		lua_pcall(L, 1, 1, 0);
// 		return 2;
// 	}
// 	fputs(content, handle);
// 	fclose(handle);
// 	return 2;
// }

// int l_include(lua_State* L){
// 	if (lua_gettop(L) != 1){
// 		return luaL_error(L, "expected 1 argument");
// 	}
// 	if (lua_type(L, -1) != LUA_TSTRING){
// 		return luaL_error(L, "argument 1 - this isn't a string");
// 	}
// 	const char* path = lua_tostring(L, -1);
// 	open_file(L, path);
// 	return 1;
// }

// static const struct luaL_Reg bindFuncs[] = {
// 	{"system", l_system },
// 	{"sleep", l_sleep },
// 	{"setcolor", l_setcolor },
// 	{"openfile", l_openfile },
// 	{"writefile", l_writefile},
// 	{"appendfile", l_appendfile},
// 	{"include", l_include},
// 	{NULL, NULL }
// };

int main(int argc, char *argv[])
{

  bench_switch();
  bench_init();
  bench_create();

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

  // lua_State* L = luaL_newstate();
	// luaL_openlibs( L );
	// lua_register( L, LIB_NAME, bindFuncs);
	// open_file( L, "init.lua" );
	// lua_close( L );

  if( !glfwInit() )
  {
      fprintf( stderr, "Failed to initialize GLFW\n" );
      exit( EXIT_FAILURE );
  }
  if( !glfwOpenWindow( 300,300, 0,0,0,0, 16,0, GLFW_WINDOW ) )
  {
      fprintf( stderr, "Failed to open GLFW window\n" );
      glfwTerminate();
      exit( EXIT_FAILURE );
  }
  glfwSetWindowTitle( "Gears" );
  glfwEnable( GLFW_KEY_REPEAT );
  glfwSwapInterval( 1 );
  init(argc, argv);
  glfwSetWindowSizeCallback( reshape );
  glfwSetKeyCallback( key );
  while( running )
  {
      draw();
      animate();
      glfwSwapBuffers();
      if( !glfwGetWindowParam( GLFW_OPENED ) )
      {
          running = 0;
      }
  }
  glfwTerminate();
  exit( EXIT_SUCCESS );

  // #ifdef WIN32
  // 	system("pause");
  // #endif	
  // 	return EXIT_SUCCESS;
}

