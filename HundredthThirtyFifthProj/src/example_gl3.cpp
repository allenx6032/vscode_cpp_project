//
// Copyright (c) 2013 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <json.hpp>
using json = nlohmann::json;

#ifdef NANOVG_GLEW
#	include <GL/glew.h>
#endif
#ifdef __APPLE__
#	define GLFW_INCLUDE_GLCOREARB
#endif
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"
#include "nanovg_demo.h"
#include "nanovg_perf.h"

#include "minihttp.h"
using namespace std;


#include "quickjs.h"
// #include "quickjs-cutils.h"
// #include "quickjs-libc.h"
// JSRuntime *rt;
// JSContext *ctx;
JSValue add(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    if (argc != 2) {
        return JS_EXCEPTION;
    }
    int a = JS_VALUE_GET_INT(argv[0]);
    int b = JS_VALUE_GET_INT(argv[1]);
    int result = a + b;
    return JS_NewInt32(ctx, result);
}
// int eval_buf(JSContext *ctx, const void *buf, int buf_len, const char *filename, int eval_flags)
// {
//     JSValue val;
//     int ret;
//     if ((eval_flags & JS_EVAL_TYPE_MASK) == JS_EVAL_TYPE_MODULE) {
//         /* for the modules, we compile then run to be able to set
//            import.meta */
//         val = JS_Eval(ctx, buf, buf_len, filename, eval_flags | JS_EVAL_FLAG_COMPILE_ONLY);
//         if (!JS_IsException(val)) {
//             js_module_set_import_meta(ctx, val, TRUE, TRUE);
//             val = JS_EvalFunction(ctx, val);
//         }
//         val = js_std_await(ctx, val);
//     } else {
//         val = JS_Eval(ctx, buf, buf_len, filename, eval_flags);
//     }
//     if (JS_IsException(val)) {
//         js_std_dump_error(ctx);
//         ret = -1;
//     } else {
//         ret = 0;
//     }
//     JS_FreeValue(ctx, val);
//     return ret;
// }
// int eval_file(JSContext *ctx, const char *filename, int module)
// {
//     uint8_t *buf;
//     int ret, eval_flags;
//     size_t buf_len;

//     buf = js_load_file(ctx, &buf_len, filename);
//     if (!buf) {
//         perror(filename);
//         exit(1);
//     }
//     if (module < 0) {
//         module = (has_suffix(filename, ".mjs") || JS_DetectModule((const char *)buf, buf_len));
//     }
//     if (module)
//         eval_flags = JS_EVAL_TYPE_MODULE;
//     else
//         eval_flags = JS_EVAL_TYPE_GLOBAL;
//     ret = eval_buf(ctx, buf, buf_len, filename, eval_flags);
//     js_free(ctx, buf);
//     return ret;
// }
// BOOL qjs_init(){
// 	rt = JS_NewRuntime();
//     if(!rt) return FALSE;
// 	ctx = JS_NewContext(rt);
//     if(!ctx) return FALSE;
//     js_std_init_handlers(rt);
//     /* bignum support */
//     JS_AddIntrinsicBigFloat(ctx);
//     JS_AddIntrinsicBigDecimal(ctx);
//     JS_AddIntrinsicOperators(ctx);
//     JS_EnableBignumExt(ctx, 1);
//     /* std, os modules */
//     js_init_module_std(ctx, "std");
//     js_init_module_os(ctx, "os");
// 	js_std_add_helpers(ctx, 0, NULL);
// 	/* loader for ES6 js modules or xxx.so file */
//     JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);
//     /* make 'std' and 'os' visible to non module code */
//     if (FALSE) {
//         const char *str = "import * as std from 'std';\n"
//             "import * as os from 'os';\n"
//             "globalThis.std = std;\n"
//             "globalThis.os = os;\n";
//         eval_buf(ctx, str, strlen(str), "<input>", JS_EVAL_TYPE_MODULE);
//     }
//     return TRUE;
// }
// int qjs_run_buf(const void *buf, int buf_len, int module) {
//     int flags = 0;
//     if( module ){
//         flags = JS_EVAL_TYPE_MODULE;
//     }
//     if( rt >0 && ctx > 0 ){
//         return eval_buf(ctx, buf, buf_len, "user_buf", flags);
//     }
//     return 0;
// }
// int qjs_run_file(const char *filename, int module){
//     if( rt >0 && ctx > 0 ){
//         return eval_file(ctx, filename, module);
//     }
//     return 0;
// }
// void qjs_exit(){
//     if( rt >0 && ctx > 0 ){
//         js_std_free_handlers(rt);
//         JS_FreeContext(ctx);
//         JS_FreeRuntime(rt);
//     }
// }
// void qjs_loop(){
//     if( rt >0 && ctx > 0 ){
//         js_std_loop(ctx);
//     }
// }

static unsigned long GetTickCount(void)
{
	return (unsigned long)time(NULL);
}

uint32_t getTicks()
{
#ifdef _WIN32
	return GetTickCount();
#else
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return((spec.tv_nsec / 1.0e6) + (spec.tv_sec * 1000));
#endif
}

struct Timer {
  clock_t tick;
  string mName;
  int mIterations;
 
  Timer( const string &name, int iterations) : mName( name ), mIterations(iterations) {
    tick = clock();
  }
 
  ~Timer() {
    clock_t end = clock();
    double elapsed_secs = double(end - tick) / CLOCKS_PER_SEC;
    cout << "Timer " << mName << ", elapsed time " << (elapsed_secs / mIterations * 1000.0) << "ms\n";
  }
};

// Overloaded socket class that handles incoming data.
// This one just prints to standard output.
class HttpDumpSocket : public minihttp::HttpSocket
{
public:
    virtual ~HttpDumpSocket() {}

protected:
    virtual void _OnClose()
    {
        puts("_OnClose()");
        minihttp::HttpSocket::_OnClose();
    }
    virtual void _OnOpen()
    {
        puts("_OnOpen()");
        char buf[1024] = { 0 };
        minihttp::SSLResult sr = verifySSL(buf, sizeof(buf));
        printf("SSL status flags (0 is good): 0x%x. Info: %s\n", sr, buf);
        minihttp::HttpSocket::_OnOpen();
    }

    virtual void _OnRequestDone()
    {
        printf("_OnRequestDone(): %s\n", GetCurrentRequest().resource.c_str());
        // Do *NOT* call close() in here!
    }

    virtual void _OnRecv(void *buf, unsigned int size)
    {
        if(!size)
            return;
        printf("===START==[Status:%d, Size:%d]======\n", GetStatusCode(), size);
        fwrite(buf, 1, size, stdout);
        puts("\n===END====================");
    }
};

#define MAX_DOWNLOAD_SIZE 1048576	//1 MB oughta be plenty
string	sBuf;
bool	bStop;
string	sRedir;
string	sSearch;

class HttpGet : public minihttp::HttpSocket
{
public:
	HttpGet() : minihttp::HttpSocket()	{bStop = false; sBuf.clear(); sRedir.clear(); sSearch.clear();};
	virtual ~HttpGet()					{};
	string getBuf()						{return sBuf;};
	int getSize()						{return sBuf.size();};
	bool isStopped()					{return bStop;};
	string getRedir()					{return sRedir;};
	void searchFor(string s)			{sSearch = s;};

protected:
	virtual void _OnRecv(void *buf, unsigned int size)
	{
		char* tmp = (char*) buf;
		if(IsRedirecting())
			sRedir = Hdr("location");
		
		if(!size)
			return;
			
		for(char* i = tmp; i < tmp+size; i++)
			sBuf.push_back(*i);
		if(sBuf.size() >= MAX_DOWNLOAD_SIZE || (sSearch.size() > 1 && sBuf.find(sSearch) != string::npos))
			bStop = true;
	}
};

string HTTPGet(string sURL, string sExtra = "", string sPostName = "", string sPost = "")
{
	HttpGet* ht = new HttpGet;
	
	minihttp::POST post;
	if(sPost.size())
		post.add(sPostName.c_str(), sPost.c_str());

	ht->SetBufsizeIn(MAX_DOWNLOAD_SIZE);
	ht->Download(sURL, (sExtra.size())?(sExtra.c_str()):(NULL), NULL, (sPost.size())?(&post):(NULL));
	ht->SetAlwaysHandle(true);
	minihttp::SocketSet ss;
	ss.add(ht, true);
	uint32_t startTicks = getTicks();
	while(ss.size() && !bStop && getTicks() < startTicks + 1000*10)	//Just spin here (for a maximum of 10 seconds)
		ss.update();
	return sBuf;
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

int main()
{
	GLFWwindow* window;
	DemoData data;
	NVGcontext* vg = NULL;
	GPUtimer gpuTimer;
	PerfGraph fps, cpuGraph, gpuGraph;
	double prevt = 0, cpuTime = 0;

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}

	initGraph(&fps, GRAPH_RENDER_FPS, "Frame Time");
	initGraph(&cpuGraph, GRAPH_RENDER_MS, "CPU Time");
	initGraph(&gpuGraph, GRAPH_RENDER_MS, "GPU Time");

	glfwSetErrorCallback(errorcb);
#ifndef _WIN32 // don't require this on win32, and works with more cards
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

#ifdef DEMO_MSAA
	glfwWindowHint(GLFW_SAMPLES, 4);
#endif
	window = glfwCreateWindow(1000, 600, "NanoVG", NULL, NULL);
//	window = glfwCreateWindow(1000, 600, "NanoVG", glfwGetPrimaryMonitor(), NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key);

	glfwMakeContextCurrent(window);
#ifdef NANOVG_GLEW
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		return -1;
	}
	// GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();
#endif

#ifdef DEMO_MSAA
	vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
#else
	vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif
	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}

//	if (loadDemoData(vg, &data) == -1)
//		return -1;

	glfwSwapInterval(0);

	initGPUTimer(&gpuTimer);

	glfwSetTime(0);
	prevt = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		double mx, my, t, dt;
		int winWidth, winHeight;
		int fbWidth, fbHeight;
		float pxRatio;
		float gpuTimes[3];
		int i, n;

		t = glfwGetTime();
		dt = t - prevt;
		prevt = t;

		startGPUTimer(&gpuTimer);

		glfwGetCursorPos(window, &mx, &my);
		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		// Calculate pixel ration for hi-dpi devices.
		pxRatio = (float)fbWidth / (float)winWidth;

		// Update and render
		glViewport(0, 0, fbWidth, fbHeight);
		if (premult)
			glClearColor(0,0,0,0);
		else
			glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

		nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

		renderDemo(vg, mx,my, winWidth,winHeight, t, blowup, &data);

		renderGraph(vg, 5,5, &fps);
		renderGraph(vg, 5+200+5,5, &cpuGraph);
		if (gpuTimer.supported)
			renderGraph(vg, 5+200+5+200+5,5, &gpuGraph);

		nvgEndFrame(vg);

		// Measure the CPU time taken excluding swap buffers (as the swap may wait for GPU)
		cpuTime = glfwGetTime() - t;

		updateGraph(&fps, dt);
		updateGraph(&cpuGraph, cpuTime);

		// We may get multiple results.
		n = stopGPUTimer(&gpuTimer, gpuTimes, 3);
		for (i = 0; i < n; i++)
			updateGraph(&gpuGraph, gpuTimes[i]);

		if (screenshot) {
			screenshot = 0;
			saveScreenShot(fbWidth, fbHeight, premult, "dump.png");
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	freeDemoData(vg, &data);

	nvgDeleteGL3(vg);

	printf("Average Frame Time: %.2f ms\n", getGraphAverage(&fps) * 1000.0f);
	printf("          CPU Time: %.2f ms\n", getGraphAverage(&cpuGraph) * 1000.0f);
	printf("          GPU Time: %.2f ms\n", getGraphAverage(&gpuGraph) * 1000.0f);

	glfwTerminate();

  // minihttp::Download() in example 1 does this automatically,
    // but if this function is not used this has to be called first to init networking properly.
    minihttp::InitNetwork();
    atexit(minihttp::StopNetwork);

    HttpDumpSocket *ht = new HttpDumpSocket;
    ht->SetKeepAlive(3);
    ht->SetBufsizeIn(64 * 1024);
    ht->SetUserAgent("minihttp"); // Let the server know who we are. This is optional but it seems that some servers check that this is set.

    // HTTP GET
    ht->Download("example.com");
    ht->Download("http://www.ietf.org/rfc/rfc2616.txt");
    // Downloads requested in succession will be queued and processed one after another

    // HTTP GET with SSL, if SSL support is enabled:
    ht->Download("https://example.com"); // SSL connection
    ht->Download("https://gitdl.cn/https://raw.githubusercontent.com/fgenesis/minihttp/master/minihttp.h"); // transparent HTTP -> HTTPS redirection

    // Example HTTP POST request:
    minihttp::POST post;
    post.add("a", "b");
    post.add("x", "y");
    post.add("long string", "possibly invalid data: /x/&$+*#'?!;");
    post.add("normal", "data");
    ht->Download("https://httpbin.org/post", NULL, NULL, &post);
    // minihttp::SocketSet ss;
    // ss.add(ht, true); // true: Delete socket if closed and no task left.
    //	// This is non-blocking and could be done in background or by another thread.
    //	// Hogs quite some CPU doing it this way, though.
    // while(ss.size())
    //     ss.update();

	string str = HTTPGet("https://gateway-test.tokbuy.dev/business/v1/user/isRegister?userAccount=1%401.1&inviteCode=&isoCode=CN", "");
	printf("HTTPGet ==  %s\n",str.c_str());

	// const char* hostname = "www.google.com";
	// CF_HttpsRequest request = cf_https_get(hostname, 443, "/", true);
	// while (1) {
	// 	CF_HttpsResult state = cf_https_process(request);
	// 	if (state < 0) {
	// 		printf("%s\n", cf_https_result_to_string(state));
	// 		cf_https_destroy(request);
	// 		return -1;
	// 	}
	// 	if (state == CF_HTTPS_RESULT_OK) {
	// 		printf("Connected!\n");
	// 		break;
	// 	}
	// }
	// CF_HttpsResponse response = cf_https_response(request);
	// FILE* fp = fopen("response.txt", "wb");
	// if (!fp) {
	// 	printf("Unable to open response.txt.\n");
	// 	return -1;
	// }
	// fwrite(cf_https_response_content(response), cf_https_response_content_length(response), 1, fp);
	// fclose(fp);
	// cf_https_destroy(request);
	// printf("Saved response in response.txt\n");

	// // 创建一个QuickJS运行时环境
    // JSRuntime* rt = JS_NewRuntime();
    // // 创建一个上下文
    // JSContext* ctx = JS_NewContext(rt);
    // // 评估一段JavaScript代码
    // const char* js_code = "console.log('Hello from JavaScript in C++ using QuickJS');";
    // JSValue val = JS_Eval(ctx, js_code, strlen(js_code), "<input>", JS_EVAL_TYPE_GLOBAL);
    // if (JS_IsException(val)) {
    //     // 如果出现异常，打印异常信息
    //     //JSValueStack stack;
    //     //JS_PrintException(ctx, val, &stack);
    // }
    // // 释放资源
    // JS_FreeContext(ctx);
    // JS_FreeRuntime(rt);

	// JSRuntime* runtime = JS_NewRuntime();
	// JSContext* ctx = JS_NewContext(runtime);
	// JSValue result =
	//     JS_Eval(ctx, str, strlen(str), "<evalScript>", JS_EVAL_TYPE_GLOBAL);
	// if (JS_IsException(result)) {
	// 	JSValue realException = JS_GetException(ctx);
	// 	return JS_ToCString(ctx, realException);
	// }
	// JSValue json = JS_JSONStringify(ctx, result, JS_UNDEFINED, JS_UNDEFINED);
	// JS_FreeValue(ctx, result);
	Timer myTimer("Sort idiomatic c++", 1000);


	string raw_json = "{\"key\":\"value\"}";
 	nlohmann::json jres = nullptr;
    try {
        jres = nlohmann::json::parse(raw_json);
    } catch (nlohmann::json::parse_error &e) {
        cout << "[TDCLIENT EXECUTE] JSON Parse error " << e.what() << "\n";
    }
    cout << jres << endl;

	// printf("hello: demo to use quickjs like qjs!\r\n");
    // if( qjs_init() ){
    //     qjs_run_file("nvg.js", 1);
    //     qjs_loop();
    //     qjs_exit();
    // }

 	JSRuntime *rt = JS_NewRuntime();
    JSContext *ctx = JS_NewContext(rt);
    //JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);
    //JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);
    JSValue global_obj = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global_obj, "add", JS_NewCFunction(ctx, add, "add", 2));
    const char *code = "var result = add(1, 2); console.log(result);";
    JS_Eval(ctx, code, strlen(code), "<input>", JS_EVAL_TYPE_MODULE);
    JS_FreeValue(ctx, global_obj);
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);

	getchar();
	return 0;
}
