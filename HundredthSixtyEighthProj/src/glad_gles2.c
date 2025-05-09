#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad_gles2.h>

#ifndef GLAD_IMPL_UTIL_C_
#define GLAD_IMPL_UTIL_C_

#ifdef _MSC_VER
#define GLAD_IMPL_UTIL_SSCANF sscanf_s
#else
#define GLAD_IMPL_UTIL_SSCANF sscanf
#endif

#endif /* GLAD_IMPL_UTIL_C_ */


int GLAD_GL_ES_VERSION_2_0 = 0;
int GLAD_GL_ES_VERSION_3_0 = 0;
int GLAD_GL_ES_VERSION_3_1 = 0;
int GLAD_GL_ES_VERSION_3_2 = 0;
int GLAD_GL_KHR_debug = 0;


static void _pre_call_gles2_callback_default(const char *name, GLADapiproc apiproc, int len_args, ...) {
    (void) len_args;

    if (apiproc == NULL) {
        fprintf(stderr, "GLAD: ERROR %s is NULL!\n", name);
        return;
    }
    if (glad_glGetError == NULL) {
        fprintf(stderr, "GLAD: ERROR glGetError is NULL!\n");
        return;
    }

    (void) glad_glGetError();
}
static void _post_call_gles2_callback_default(void *ret, const char *name, GLADapiproc apiproc, int len_args, ...) {
    GLenum error_code;

    (void) ret;
    (void) apiproc;
    (void) len_args;

    error_code = glad_glGetError();

    if (error_code != GL_NO_ERROR) {
        fprintf(stderr, "GLAD: ERROR %d in %s!\n", error_code, name);
    }
}

static GLADprecallback _pre_call_gles2_callback = _pre_call_gles2_callback_default;
void gladSetGLES2PreCallback(GLADprecallback cb) {
    _pre_call_gles2_callback = cb;
}
static GLADpostcallback _post_call_gles2_callback = _post_call_gles2_callback_default;
void gladSetGLES2PostCallback(GLADpostcallback cb) {
    _post_call_gles2_callback = cb;
}

PFNGLACTIVESHADERPROGRAMPROC glad_glActiveShaderProgram = NULL;
static void GLAD_API_PTR glad_debug_impl_glActiveShaderProgram(GLuint pipeline, GLuint program) {
    _pre_call_gles2_callback("glActiveShaderProgram", (GLADapiproc) glad_glActiveShaderProgram, 2, pipeline, program);
    glad_glActiveShaderProgram(pipeline, program);
    _post_call_gles2_callback(NULL, "glActiveShaderProgram", (GLADapiproc) glad_glActiveShaderProgram, 2, pipeline, program);
    
}
PFNGLACTIVESHADERPROGRAMPROC glad_debug_glActiveShaderProgram = glad_debug_impl_glActiveShaderProgram;
PFNGLACTIVETEXTUREPROC glad_glActiveTexture = NULL;
static void GLAD_API_PTR glad_debug_impl_glActiveTexture(GLenum texture) {
    _pre_call_gles2_callback("glActiveTexture", (GLADapiproc) glad_glActiveTexture, 1, texture);
    glad_glActiveTexture(texture);
    _post_call_gles2_callback(NULL, "glActiveTexture", (GLADapiproc) glad_glActiveTexture, 1, texture);
    
}
PFNGLACTIVETEXTUREPROC glad_debug_glActiveTexture = glad_debug_impl_glActiveTexture;
PFNGLATTACHSHADERPROC glad_glAttachShader = NULL;
static void GLAD_API_PTR glad_debug_impl_glAttachShader(GLuint program, GLuint shader) {
    _pre_call_gles2_callback("glAttachShader", (GLADapiproc) glad_glAttachShader, 2, program, shader);
    glad_glAttachShader(program, shader);
    _post_call_gles2_callback(NULL, "glAttachShader", (GLADapiproc) glad_glAttachShader, 2, program, shader);
    
}
PFNGLATTACHSHADERPROC glad_debug_glAttachShader = glad_debug_impl_glAttachShader;
PFNGLBEGINQUERYPROC glad_glBeginQuery = NULL;
static void GLAD_API_PTR glad_debug_impl_glBeginQuery(GLenum target, GLuint id) {
    _pre_call_gles2_callback("glBeginQuery", (GLADapiproc) glad_glBeginQuery, 2, target, id);
    glad_glBeginQuery(target, id);
    _post_call_gles2_callback(NULL, "glBeginQuery", (GLADapiproc) glad_glBeginQuery, 2, target, id);
    
}
PFNGLBEGINQUERYPROC glad_debug_glBeginQuery = glad_debug_impl_glBeginQuery;
PFNGLBEGINTRANSFORMFEEDBACKPROC glad_glBeginTransformFeedback = NULL;
static void GLAD_API_PTR glad_debug_impl_glBeginTransformFeedback(GLenum primitiveMode) {
    _pre_call_gles2_callback("glBeginTransformFeedback", (GLADapiproc) glad_glBeginTransformFeedback, 1, primitiveMode);
    glad_glBeginTransformFeedback(primitiveMode);
    _post_call_gles2_callback(NULL, "glBeginTransformFeedback", (GLADapiproc) glad_glBeginTransformFeedback, 1, primitiveMode);
    
}
PFNGLBEGINTRANSFORMFEEDBACKPROC glad_debug_glBeginTransformFeedback = glad_debug_impl_glBeginTransformFeedback;
PFNGLBINDATTRIBLOCATIONPROC glad_glBindAttribLocation = NULL;
static void GLAD_API_PTR glad_debug_impl_glBindAttribLocation(GLuint program, GLuint index, const GLchar * name) {
    _pre_call_gles2_callback("glBindAttribLocation", (GLADapiproc) glad_glBindAttribLocation, 3, program, index, name);
    glad_glBindAttribLocation(program, index, name);
    _post_call_gles2_callback(NULL, "glBindAttribLocation", (GLADapiproc) glad_glBindAttribLocation, 3, program, index, name);
    
}
PFNGLBINDATTRIBLOCATIONPROC glad_debug_glBindAttribLocation = glad_debug_impl_glBindAttribLocation;
PFNGLBINDBUFFERPROC glad_glBindBuffer = NULL;
static void GLAD_API_PTR glad_debug_impl_glBindBuffer(GLenum target, GLuint buffer) {
    _pre_call_gles2_callback("glBindBuffer", (GLADapiproc) glad_glBindBuffer, 2, target, buffer);
    glad_glBindBuffer(target, buffer);
    _post_call_gles2_callback(NULL, "glBindBuffer", (GLADapiproc) glad_glBindBuffer, 2, target, buffer);
    
}
PFNGLBINDBUFFERPROC glad_debug_glBindBuffer = glad_debug_impl_glBindBuffer;
PFNGLBINDBUFFERBASEPROC glad_glBindBufferBase = NULL;
static void GLAD_API_PTR glad_debug_impl_glBindBufferBase(GLenum target, GLuint index, GLuint buffer) {
    _pre_call_gles2_callback("glBindBufferBase", (GLADapiproc) glad_glBindBufferBase, 3, target, index, buffer);
    glad_glBindBufferBase(target, index, buffer);
    _post_call_gles2_callback(NULL, "glBindBufferBase", (GLADapiproc) glad_glBindBufferBase, 3, target, index, buffer);
    
}
PFNGLBINDBUFFERBASEPROC glad_debug_glBindBufferBase = glad_debug_impl_glBindBufferBase;
PFNGLBINDBUFFERRANGEPROC glad_glBindBufferRange = NULL;
static void GLAD_API_PTR glad_debug_impl_glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) {
    _pre_call_gles2_callback("glBindBufferRange", (GLADapiproc) glad_glBindBufferRange, 5, target, index, buffer, offset, size);
    glad_glBindBufferRange(target, index, buffer, offset, size);
    _post_call_gles2_callback(NULL, "glBindBufferRange", (GLADapiproc) glad_glBindBufferRange, 5, target, index, buffer, offset, size);
    
}
PFNGLBINDBUFFERRANGEPROC glad_debug_glBindBufferRange = glad_debug_impl_glBindBufferRange;
PFNGLBINDFRAMEBUFFERPROC glad_glBindFramebuffer = NULL;
static void GLAD_API_PTR glad_debug_impl_glBindFramebuffer(GLenum target, GLuint framebuffer) {
    _pre_call_gles2_callback("glBindFramebuffer", (GLADapiproc) glad_glBindFramebuffer, 2, target, framebuffer);
    glad_glBindFramebuffer(target, framebuffer);
    _post_call_gles2_callback(NULL, "glBindFramebuffer", (GLADapiproc) glad_glBindFramebuffer, 2, target, framebuffer);
    
}
PFNGLBINDFRAMEBUFFERPROC glad_debug_glBindFramebuffer = glad_debug_impl_glBindFramebuffer;
PFNGLBINDIMAGETEXTUREPROC glad_glBindImageTexture = NULL;
static void GLAD_API_PTR glad_debug_impl_glBindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) {
    _pre_call_gles2_callback("glBindImageTexture", (GLADapiproc) glad_glBindImageTexture, 7, unit, texture, level, layered, layer, access, format);
    glad_glBindImageTexture(unit, texture, level, layered, layer, access, format);
    _post_call_gles2_callback(NULL, "glBindImageTexture", (GLADapiproc) glad_glBindImageTexture, 7, unit, texture, level, layered, layer, access, format);
    
}
PFNGLBINDIMAGETEXTUREPROC glad_debug_glBindImageTexture = glad_debug_impl_glBindImageTexture;
PFNGLBINDPROGRAMPIPELINEPROC glad_glBindProgramPipeline = NULL;
static void GLAD_API_PTR glad_debug_impl_glBindProgramPipeline(GLuint pipeline) {
    _pre_call_gles2_callback("glBindProgramPipeline", (GLADapiproc) glad_glBindProgramPipeline, 1, pipeline);
    glad_glBindProgramPipeline(pipeline);
    _post_call_gles2_callback(NULL, "glBindProgramPipeline", (GLADapiproc) glad_glBindProgramPipeline, 1, pipeline);
    
}
PFNGLBINDPROGRAMPIPELINEPROC glad_debug_glBindProgramPipeline = glad_debug_impl_glBindProgramPipeline;
PFNGLBINDRENDERBUFFERPROC glad_glBindRenderbuffer = NULL;
static void GLAD_API_PTR glad_debug_impl_glBindRenderbuffer(GLenum target, GLuint renderbuffer) {
    _pre_call_gles2_callback("glBindRenderbuffer", (GLADapiproc) glad_glBindRenderbuffer, 2, target, renderbuffer);
    glad_glBindRenderbuffer(target, renderbuffer);
    _post_call_gles2_callback(NULL, "glBindRenderbuffer", (GLADapiproc) glad_glBindRenderbuffer, 2, target, renderbuffer);
    
}
PFNGLBINDRENDERBUFFERPROC glad_debug_glBindRenderbuffer = glad_debug_impl_glBindRenderbuffer;
PFNGLBINDSAMPLERPROC glad_glBindSampler = NULL;
static void GLAD_API_PTR glad_debug_impl_glBindSampler(GLuint unit, GLuint sampler) {
    _pre_call_gles2_callback("glBindSampler", (GLADapiproc) glad_glBindSampler, 2, unit, sampler);
    glad_glBindSampler(unit, sampler);
    _post_call_gles2_callback(NULL, "glBindSampler", (GLADapiproc) glad_glBindSampler, 2, unit, sampler);
    
}
PFNGLBINDSAMPLERPROC glad_debug_glBindSampler = glad_debug_impl_glBindSampler;
PFNGLBINDTEXTUREPROC glad_glBindTexture = NULL;
static void GLAD_API_PTR glad_debug_impl_glBindTexture(GLenum target, GLuint texture) {
    _pre_call_gles2_callback("glBindTexture", (GLADapiproc) glad_glBindTexture, 2, target, texture);
    glad_glBindTexture(target, texture);
    _post_call_gles2_callback(NULL, "glBindTexture", (GLADapiproc) glad_glBindTexture, 2, target, texture);
    
}
PFNGLBINDTEXTUREPROC glad_debug_glBindTexture = glad_debug_impl_glBindTexture;
PFNGLBINDTRANSFORMFEEDBACKPROC glad_glBindTransformFeedback = NULL;
static void GLAD_API_PTR glad_debug_impl_glBindTransformFeedback(GLenum target, GLuint id) {
    _pre_call_gles2_callback("glBindTransformFeedback", (GLADapiproc) glad_glBindTransformFeedback, 2, target, id);
    glad_glBindTransformFeedback(target, id);
    _post_call_gles2_callback(NULL, "glBindTransformFeedback", (GLADapiproc) glad_glBindTransformFeedback, 2, target, id);
    
}
PFNGLBINDTRANSFORMFEEDBACKPROC glad_debug_glBindTransformFeedback = glad_debug_impl_glBindTransformFeedback;
PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray = NULL;
static void GLAD_API_PTR glad_debug_impl_glBindVertexArray(GLuint array) {
    _pre_call_gles2_callback("glBindVertexArray", (GLADapiproc) glad_glBindVertexArray, 1, array);
    glad_glBindVertexArray(array);
    _post_call_gles2_callback(NULL, "glBindVertexArray", (GLADapiproc) glad_glBindVertexArray, 1, array);
    
}
PFNGLBINDVERTEXARRAYPROC glad_debug_glBindVertexArray = glad_debug_impl_glBindVertexArray;
PFNGLBINDVERTEXBUFFERPROC glad_glBindVertexBuffer = NULL;
static void GLAD_API_PTR glad_debug_impl_glBindVertexBuffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) {
    _pre_call_gles2_callback("glBindVertexBuffer", (GLADapiproc) glad_glBindVertexBuffer, 4, bindingindex, buffer, offset, stride);
    glad_glBindVertexBuffer(bindingindex, buffer, offset, stride);
    _post_call_gles2_callback(NULL, "glBindVertexBuffer", (GLADapiproc) glad_glBindVertexBuffer, 4, bindingindex, buffer, offset, stride);
    
}
PFNGLBINDVERTEXBUFFERPROC glad_debug_glBindVertexBuffer = glad_debug_impl_glBindVertexBuffer;
PFNGLBLENDBARRIERPROC glad_glBlendBarrier = NULL;
static void GLAD_API_PTR glad_debug_impl_glBlendBarrier(void) {
    _pre_call_gles2_callback("glBlendBarrier", (GLADapiproc) glad_glBlendBarrier, 0);
    glad_glBlendBarrier();
    _post_call_gles2_callback(NULL, "glBlendBarrier", (GLADapiproc) glad_glBlendBarrier, 0);
    
}
PFNGLBLENDBARRIERPROC glad_debug_glBlendBarrier = glad_debug_impl_glBlendBarrier;
PFNGLBLENDCOLORPROC glad_glBlendColor = NULL;
static void GLAD_API_PTR glad_debug_impl_glBlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    _pre_call_gles2_callback("glBlendColor", (GLADapiproc) glad_glBlendColor, 4, red, green, blue, alpha);
    glad_glBlendColor(red, green, blue, alpha);
    _post_call_gles2_callback(NULL, "glBlendColor", (GLADapiproc) glad_glBlendColor, 4, red, green, blue, alpha);
    
}
PFNGLBLENDCOLORPROC glad_debug_glBlendColor = glad_debug_impl_glBlendColor;
PFNGLBLENDEQUATIONPROC glad_glBlendEquation = NULL;
static void GLAD_API_PTR glad_debug_impl_glBlendEquation(GLenum mode) {
    _pre_call_gles2_callback("glBlendEquation", (GLADapiproc) glad_glBlendEquation, 1, mode);
    glad_glBlendEquation(mode);
    _post_call_gles2_callback(NULL, "glBlendEquation", (GLADapiproc) glad_glBlendEquation, 1, mode);
    
}
PFNGLBLENDEQUATIONPROC glad_debug_glBlendEquation = glad_debug_impl_glBlendEquation;
PFNGLBLENDEQUATIONSEPARATEPROC glad_glBlendEquationSeparate = NULL;
static void GLAD_API_PTR glad_debug_impl_glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) {
    _pre_call_gles2_callback("glBlendEquationSeparate", (GLADapiproc) glad_glBlendEquationSeparate, 2, modeRGB, modeAlpha);
    glad_glBlendEquationSeparate(modeRGB, modeAlpha);
    _post_call_gles2_callback(NULL, "glBlendEquationSeparate", (GLADapiproc) glad_glBlendEquationSeparate, 2, modeRGB, modeAlpha);
    
}
PFNGLBLENDEQUATIONSEPARATEPROC glad_debug_glBlendEquationSeparate = glad_debug_impl_glBlendEquationSeparate;
PFNGLBLENDEQUATIONSEPARATEIPROC glad_glBlendEquationSeparatei = NULL;
static void GLAD_API_PTR glad_debug_impl_glBlendEquationSeparatei(GLuint buf, GLenum modeRGB, GLenum modeAlpha) {
    _pre_call_gles2_callback("glBlendEquationSeparatei", (GLADapiproc) glad_glBlendEquationSeparatei, 3, buf, modeRGB, modeAlpha);
    glad_glBlendEquationSeparatei(buf, modeRGB, modeAlpha);
    _post_call_gles2_callback(NULL, "glBlendEquationSeparatei", (GLADapiproc) glad_glBlendEquationSeparatei, 3, buf, modeRGB, modeAlpha);
    
}
PFNGLBLENDEQUATIONSEPARATEIPROC glad_debug_glBlendEquationSeparatei = glad_debug_impl_glBlendEquationSeparatei;
PFNGLBLENDEQUATIONIPROC glad_glBlendEquationi = NULL;
static void GLAD_API_PTR glad_debug_impl_glBlendEquationi(GLuint buf, GLenum mode) {
    _pre_call_gles2_callback("glBlendEquationi", (GLADapiproc) glad_glBlendEquationi, 2, buf, mode);
    glad_glBlendEquationi(buf, mode);
    _post_call_gles2_callback(NULL, "glBlendEquationi", (GLADapiproc) glad_glBlendEquationi, 2, buf, mode);
    
}
PFNGLBLENDEQUATIONIPROC glad_debug_glBlendEquationi = glad_debug_impl_glBlendEquationi;
PFNGLBLENDFUNCPROC glad_glBlendFunc = NULL;
static void GLAD_API_PTR glad_debug_impl_glBlendFunc(GLenum sfactor, GLenum dfactor) {
    _pre_call_gles2_callback("glBlendFunc", (GLADapiproc) glad_glBlendFunc, 2, sfactor, dfactor);
    glad_glBlendFunc(sfactor, dfactor);
    _post_call_gles2_callback(NULL, "glBlendFunc", (GLADapiproc) glad_glBlendFunc, 2, sfactor, dfactor);
    
}
PFNGLBLENDFUNCPROC glad_debug_glBlendFunc = glad_debug_impl_glBlendFunc;
PFNGLBLENDFUNCSEPARATEPROC glad_glBlendFuncSeparate = NULL;
static void GLAD_API_PTR glad_debug_impl_glBlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha) {
    _pre_call_gles2_callback("glBlendFuncSeparate", (GLADapiproc) glad_glBlendFuncSeparate, 4, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    glad_glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    _post_call_gles2_callback(NULL, "glBlendFuncSeparate", (GLADapiproc) glad_glBlendFuncSeparate, 4, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    
}
PFNGLBLENDFUNCSEPARATEPROC glad_debug_glBlendFuncSeparate = glad_debug_impl_glBlendFuncSeparate;
PFNGLBLENDFUNCSEPARATEIPROC glad_glBlendFuncSeparatei = NULL;
static void GLAD_API_PTR glad_debug_impl_glBlendFuncSeparatei(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) {
    _pre_call_gles2_callback("glBlendFuncSeparatei", (GLADapiproc) glad_glBlendFuncSeparatei, 5, buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
    glad_glBlendFuncSeparatei(buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
    _post_call_gles2_callback(NULL, "glBlendFuncSeparatei", (GLADapiproc) glad_glBlendFuncSeparatei, 5, buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
    
}
PFNGLBLENDFUNCSEPARATEIPROC glad_debug_glBlendFuncSeparatei = glad_debug_impl_glBlendFuncSeparatei;
PFNGLBLENDFUNCIPROC glad_glBlendFunci = NULL;
static void GLAD_API_PTR glad_debug_impl_glBlendFunci(GLuint buf, GLenum src, GLenum dst) {
    _pre_call_gles2_callback("glBlendFunci", (GLADapiproc) glad_glBlendFunci, 3, buf, src, dst);
    glad_glBlendFunci(buf, src, dst);
    _post_call_gles2_callback(NULL, "glBlendFunci", (GLADapiproc) glad_glBlendFunci, 3, buf, src, dst);
    
}
PFNGLBLENDFUNCIPROC glad_debug_glBlendFunci = glad_debug_impl_glBlendFunci;
PFNGLBLITFRAMEBUFFERPROC glad_glBlitFramebuffer = NULL;
static void GLAD_API_PTR glad_debug_impl_glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) {
    _pre_call_gles2_callback("glBlitFramebuffer", (GLADapiproc) glad_glBlitFramebuffer, 10, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
    glad_glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
    _post_call_gles2_callback(NULL, "glBlitFramebuffer", (GLADapiproc) glad_glBlitFramebuffer, 10, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
    
}
PFNGLBLITFRAMEBUFFERPROC glad_debug_glBlitFramebuffer = glad_debug_impl_glBlitFramebuffer;
PFNGLBUFFERDATAPROC glad_glBufferData = NULL;
static void GLAD_API_PTR glad_debug_impl_glBufferData(GLenum target, GLsizeiptr size, const void * data, GLenum usage) {
    _pre_call_gles2_callback("glBufferData", (GLADapiproc) glad_glBufferData, 4, target, size, data, usage);
    glad_glBufferData(target, size, data, usage);
    _post_call_gles2_callback(NULL, "glBufferData", (GLADapiproc) glad_glBufferData, 4, target, size, data, usage);
    
}
PFNGLBUFFERDATAPROC glad_debug_glBufferData = glad_debug_impl_glBufferData;
PFNGLBUFFERSUBDATAPROC glad_glBufferSubData = NULL;
static void GLAD_API_PTR glad_debug_impl_glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void * data) {
    _pre_call_gles2_callback("glBufferSubData", (GLADapiproc) glad_glBufferSubData, 4, target, offset, size, data);
    glad_glBufferSubData(target, offset, size, data);
    _post_call_gles2_callback(NULL, "glBufferSubData", (GLADapiproc) glad_glBufferSubData, 4, target, offset, size, data);
    
}
PFNGLBUFFERSUBDATAPROC glad_debug_glBufferSubData = glad_debug_impl_glBufferSubData;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus = NULL;
static GLenum GLAD_API_PTR glad_debug_impl_glCheckFramebufferStatus(GLenum target) {
    GLenum ret;
    _pre_call_gles2_callback("glCheckFramebufferStatus", (GLADapiproc) glad_glCheckFramebufferStatus, 1, target);
    ret = glad_glCheckFramebufferStatus(target);
    _post_call_gles2_callback((void*) &ret, "glCheckFramebufferStatus", (GLADapiproc) glad_glCheckFramebufferStatus, 1, target);
    return ret;
}
PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_debug_glCheckFramebufferStatus = glad_debug_impl_glCheckFramebufferStatus;
PFNGLCLEARPROC glad_glClear = NULL;
static void GLAD_API_PTR glad_debug_impl_glClear(GLbitfield mask) {
    _pre_call_gles2_callback("glClear", (GLADapiproc) glad_glClear, 1, mask);
    glad_glClear(mask);
    _post_call_gles2_callback(NULL, "glClear", (GLADapiproc) glad_glClear, 1, mask);
    
}
PFNGLCLEARPROC glad_debug_glClear = glad_debug_impl_glClear;
PFNGLCLEARBUFFERFIPROC glad_glClearBufferfi = NULL;
static void GLAD_API_PTR glad_debug_impl_glClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) {
    _pre_call_gles2_callback("glClearBufferfi", (GLADapiproc) glad_glClearBufferfi, 4, buffer, drawbuffer, depth, stencil);
    glad_glClearBufferfi(buffer, drawbuffer, depth, stencil);
    _post_call_gles2_callback(NULL, "glClearBufferfi", (GLADapiproc) glad_glClearBufferfi, 4, buffer, drawbuffer, depth, stencil);
    
}
PFNGLCLEARBUFFERFIPROC glad_debug_glClearBufferfi = glad_debug_impl_glClearBufferfi;
PFNGLCLEARBUFFERFVPROC glad_glClearBufferfv = NULL;
static void GLAD_API_PTR glad_debug_impl_glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat * value) {
    _pre_call_gles2_callback("glClearBufferfv", (GLADapiproc) glad_glClearBufferfv, 3, buffer, drawbuffer, value);
    glad_glClearBufferfv(buffer, drawbuffer, value);
    _post_call_gles2_callback(NULL, "glClearBufferfv", (GLADapiproc) glad_glClearBufferfv, 3, buffer, drawbuffer, value);
    
}
PFNGLCLEARBUFFERFVPROC glad_debug_glClearBufferfv = glad_debug_impl_glClearBufferfv;
PFNGLCLEARBUFFERIVPROC glad_glClearBufferiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint * value) {
    _pre_call_gles2_callback("glClearBufferiv", (GLADapiproc) glad_glClearBufferiv, 3, buffer, drawbuffer, value);
    glad_glClearBufferiv(buffer, drawbuffer, value);
    _post_call_gles2_callback(NULL, "glClearBufferiv", (GLADapiproc) glad_glClearBufferiv, 3, buffer, drawbuffer, value);
    
}
PFNGLCLEARBUFFERIVPROC glad_debug_glClearBufferiv = glad_debug_impl_glClearBufferiv;
PFNGLCLEARBUFFERUIVPROC glad_glClearBufferuiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint * value) {
    _pre_call_gles2_callback("glClearBufferuiv", (GLADapiproc) glad_glClearBufferuiv, 3, buffer, drawbuffer, value);
    glad_glClearBufferuiv(buffer, drawbuffer, value);
    _post_call_gles2_callback(NULL, "glClearBufferuiv", (GLADapiproc) glad_glClearBufferuiv, 3, buffer, drawbuffer, value);
    
}
PFNGLCLEARBUFFERUIVPROC glad_debug_glClearBufferuiv = glad_debug_impl_glClearBufferuiv;
PFNGLCLEARCOLORPROC glad_glClearColor = NULL;
static void GLAD_API_PTR glad_debug_impl_glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    _pre_call_gles2_callback("glClearColor", (GLADapiproc) glad_glClearColor, 4, red, green, blue, alpha);
    glad_glClearColor(red, green, blue, alpha);
    _post_call_gles2_callback(NULL, "glClearColor", (GLADapiproc) glad_glClearColor, 4, red, green, blue, alpha);
    
}
PFNGLCLEARCOLORPROC glad_debug_glClearColor = glad_debug_impl_glClearColor;
PFNGLCLEARDEPTHFPROC glad_glClearDepthf = NULL;
static void GLAD_API_PTR glad_debug_impl_glClearDepthf(GLfloat d) {
    _pre_call_gles2_callback("glClearDepthf", (GLADapiproc) glad_glClearDepthf, 1, d);
    glad_glClearDepthf(d);
    _post_call_gles2_callback(NULL, "glClearDepthf", (GLADapiproc) glad_glClearDepthf, 1, d);
    
}
PFNGLCLEARDEPTHFPROC glad_debug_glClearDepthf = glad_debug_impl_glClearDepthf;
PFNGLCLEARSTENCILPROC glad_glClearStencil = NULL;
static void GLAD_API_PTR glad_debug_impl_glClearStencil(GLint s) {
    _pre_call_gles2_callback("glClearStencil", (GLADapiproc) glad_glClearStencil, 1, s);
    glad_glClearStencil(s);
    _post_call_gles2_callback(NULL, "glClearStencil", (GLADapiproc) glad_glClearStencil, 1, s);
    
}
PFNGLCLEARSTENCILPROC glad_debug_glClearStencil = glad_debug_impl_glClearStencil;
PFNGLCLIENTWAITSYNCPROC glad_glClientWaitSync = NULL;
static GLenum GLAD_API_PTR glad_debug_impl_glClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout) {
    GLenum ret;
    _pre_call_gles2_callback("glClientWaitSync", (GLADapiproc) glad_glClientWaitSync, 3, sync, flags, timeout);
    ret = glad_glClientWaitSync(sync, flags, timeout);
    _post_call_gles2_callback((void*) &ret, "glClientWaitSync", (GLADapiproc) glad_glClientWaitSync, 3, sync, flags, timeout);
    return ret;
}
PFNGLCLIENTWAITSYNCPROC glad_debug_glClientWaitSync = glad_debug_impl_glClientWaitSync;
PFNGLCOLORMASKPROC glad_glColorMask = NULL;
static void GLAD_API_PTR glad_debug_impl_glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) {
    _pre_call_gles2_callback("glColorMask", (GLADapiproc) glad_glColorMask, 4, red, green, blue, alpha);
    glad_glColorMask(red, green, blue, alpha);
    _post_call_gles2_callback(NULL, "glColorMask", (GLADapiproc) glad_glColorMask, 4, red, green, blue, alpha);
    
}
PFNGLCOLORMASKPROC glad_debug_glColorMask = glad_debug_impl_glColorMask;
PFNGLCOLORMASKIPROC glad_glColorMaski = NULL;
static void GLAD_API_PTR glad_debug_impl_glColorMaski(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a) {
    _pre_call_gles2_callback("glColorMaski", (GLADapiproc) glad_glColorMaski, 5, index, r, g, b, a);
    glad_glColorMaski(index, r, g, b, a);
    _post_call_gles2_callback(NULL, "glColorMaski", (GLADapiproc) glad_glColorMaski, 5, index, r, g, b, a);
    
}
PFNGLCOLORMASKIPROC glad_debug_glColorMaski = glad_debug_impl_glColorMaski;
PFNGLCOMPILESHADERPROC glad_glCompileShader = NULL;
static void GLAD_API_PTR glad_debug_impl_glCompileShader(GLuint shader) {
    _pre_call_gles2_callback("glCompileShader", (GLADapiproc) glad_glCompileShader, 1, shader);
    glad_glCompileShader(shader);
    _post_call_gles2_callback(NULL, "glCompileShader", (GLADapiproc) glad_glCompileShader, 1, shader);
    
}
PFNGLCOMPILESHADERPROC glad_debug_glCompileShader = glad_debug_impl_glCompileShader;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glad_glCompressedTexImage2D = NULL;
static void GLAD_API_PTR glad_debug_impl_glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void * data) {
    _pre_call_gles2_callback("glCompressedTexImage2D", (GLADapiproc) glad_glCompressedTexImage2D, 8, target, level, internalformat, width, height, border, imageSize, data);
    glad_glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
    _post_call_gles2_callback(NULL, "glCompressedTexImage2D", (GLADapiproc) glad_glCompressedTexImage2D, 8, target, level, internalformat, width, height, border, imageSize, data);
    
}
PFNGLCOMPRESSEDTEXIMAGE2DPROC glad_debug_glCompressedTexImage2D = glad_debug_impl_glCompressedTexImage2D;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glad_glCompressedTexImage3D = NULL;
static void GLAD_API_PTR glad_debug_impl_glCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void * data) {
    _pre_call_gles2_callback("glCompressedTexImage3D", (GLADapiproc) glad_glCompressedTexImage3D, 9, target, level, internalformat, width, height, depth, border, imageSize, data);
    glad_glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data);
    _post_call_gles2_callback(NULL, "glCompressedTexImage3D", (GLADapiproc) glad_glCompressedTexImage3D, 9, target, level, internalformat, width, height, depth, border, imageSize, data);
    
}
PFNGLCOMPRESSEDTEXIMAGE3DPROC glad_debug_glCompressedTexImage3D = glad_debug_impl_glCompressedTexImage3D;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glad_glCompressedTexSubImage2D = NULL;
static void GLAD_API_PTR glad_debug_impl_glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void * data) {
    _pre_call_gles2_callback("glCompressedTexSubImage2D", (GLADapiproc) glad_glCompressedTexSubImage2D, 9, target, level, xoffset, yoffset, width, height, format, imageSize, data);
    glad_glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
    _post_call_gles2_callback(NULL, "glCompressedTexSubImage2D", (GLADapiproc) glad_glCompressedTexSubImage2D, 9, target, level, xoffset, yoffset, width, height, format, imageSize, data);
    
}
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glad_debug_glCompressedTexSubImage2D = glad_debug_impl_glCompressedTexSubImage2D;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glad_glCompressedTexSubImage3D = NULL;
static void GLAD_API_PTR glad_debug_impl_glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void * data) {
    _pre_call_gles2_callback("glCompressedTexSubImage3D", (GLADapiproc) glad_glCompressedTexSubImage3D, 11, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
    glad_glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
    _post_call_gles2_callback(NULL, "glCompressedTexSubImage3D", (GLADapiproc) glad_glCompressedTexSubImage3D, 11, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
    
}
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glad_debug_glCompressedTexSubImage3D = glad_debug_impl_glCompressedTexSubImage3D;
PFNGLCOPYBUFFERSUBDATAPROC glad_glCopyBufferSubData = NULL;
static void GLAD_API_PTR glad_debug_impl_glCopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) {
    _pre_call_gles2_callback("glCopyBufferSubData", (GLADapiproc) glad_glCopyBufferSubData, 5, readTarget, writeTarget, readOffset, writeOffset, size);
    glad_glCopyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size);
    _post_call_gles2_callback(NULL, "glCopyBufferSubData", (GLADapiproc) glad_glCopyBufferSubData, 5, readTarget, writeTarget, readOffset, writeOffset, size);
    
}
PFNGLCOPYBUFFERSUBDATAPROC glad_debug_glCopyBufferSubData = glad_debug_impl_glCopyBufferSubData;
PFNGLCOPYIMAGESUBDATAPROC glad_glCopyImageSubData = NULL;
static void GLAD_API_PTR glad_debug_impl_glCopyImageSubData(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth) {
    _pre_call_gles2_callback("glCopyImageSubData", (GLADapiproc) glad_glCopyImageSubData, 15, srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, srcWidth, srcHeight, srcDepth);
    glad_glCopyImageSubData(srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, srcWidth, srcHeight, srcDepth);
    _post_call_gles2_callback(NULL, "glCopyImageSubData", (GLADapiproc) glad_glCopyImageSubData, 15, srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, srcWidth, srcHeight, srcDepth);
    
}
PFNGLCOPYIMAGESUBDATAPROC glad_debug_glCopyImageSubData = glad_debug_impl_glCopyImageSubData;
PFNGLCOPYTEXIMAGE2DPROC glad_glCopyTexImage2D = NULL;
static void GLAD_API_PTR glad_debug_impl_glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) {
    _pre_call_gles2_callback("glCopyTexImage2D", (GLADapiproc) glad_glCopyTexImage2D, 8, target, level, internalformat, x, y, width, height, border);
    glad_glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
    _post_call_gles2_callback(NULL, "glCopyTexImage2D", (GLADapiproc) glad_glCopyTexImage2D, 8, target, level, internalformat, x, y, width, height, border);
    
}
PFNGLCOPYTEXIMAGE2DPROC glad_debug_glCopyTexImage2D = glad_debug_impl_glCopyTexImage2D;
PFNGLCOPYTEXSUBIMAGE2DPROC glad_glCopyTexSubImage2D = NULL;
static void GLAD_API_PTR glad_debug_impl_glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) {
    _pre_call_gles2_callback("glCopyTexSubImage2D", (GLADapiproc) glad_glCopyTexSubImage2D, 8, target, level, xoffset, yoffset, x, y, width, height);
    glad_glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
    _post_call_gles2_callback(NULL, "glCopyTexSubImage2D", (GLADapiproc) glad_glCopyTexSubImage2D, 8, target, level, xoffset, yoffset, x, y, width, height);
    
}
PFNGLCOPYTEXSUBIMAGE2DPROC glad_debug_glCopyTexSubImage2D = glad_debug_impl_glCopyTexSubImage2D;
PFNGLCOPYTEXSUBIMAGE3DPROC glad_glCopyTexSubImage3D = NULL;
static void GLAD_API_PTR glad_debug_impl_glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) {
    _pre_call_gles2_callback("glCopyTexSubImage3D", (GLADapiproc) glad_glCopyTexSubImage3D, 9, target, level, xoffset, yoffset, zoffset, x, y, width, height);
    glad_glCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height);
    _post_call_gles2_callback(NULL, "glCopyTexSubImage3D", (GLADapiproc) glad_glCopyTexSubImage3D, 9, target, level, xoffset, yoffset, zoffset, x, y, width, height);
    
}
PFNGLCOPYTEXSUBIMAGE3DPROC glad_debug_glCopyTexSubImage3D = glad_debug_impl_glCopyTexSubImage3D;
PFNGLCREATEPROGRAMPROC glad_glCreateProgram = NULL;
static GLuint GLAD_API_PTR glad_debug_impl_glCreateProgram(void) {
    GLuint ret;
    _pre_call_gles2_callback("glCreateProgram", (GLADapiproc) glad_glCreateProgram, 0);
    ret = glad_glCreateProgram();
    _post_call_gles2_callback((void*) &ret, "glCreateProgram", (GLADapiproc) glad_glCreateProgram, 0);
    return ret;
}
PFNGLCREATEPROGRAMPROC glad_debug_glCreateProgram = glad_debug_impl_glCreateProgram;
PFNGLCREATESHADERPROC glad_glCreateShader = NULL;
static GLuint GLAD_API_PTR glad_debug_impl_glCreateShader(GLenum type) {
    GLuint ret;
    _pre_call_gles2_callback("glCreateShader", (GLADapiproc) glad_glCreateShader, 1, type);
    ret = glad_glCreateShader(type);
    _post_call_gles2_callback((void*) &ret, "glCreateShader", (GLADapiproc) glad_glCreateShader, 1, type);
    return ret;
}
PFNGLCREATESHADERPROC glad_debug_glCreateShader = glad_debug_impl_glCreateShader;
PFNGLCREATESHADERPROGRAMVPROC glad_glCreateShaderProgramv = NULL;
static GLuint GLAD_API_PTR glad_debug_impl_glCreateShaderProgramv(GLenum type, GLsizei count, const GLchar *const* strings) {
    GLuint ret;
    _pre_call_gles2_callback("glCreateShaderProgramv", (GLADapiproc) glad_glCreateShaderProgramv, 3, type, count, strings);
    ret = glad_glCreateShaderProgramv(type, count, strings);
    _post_call_gles2_callback((void*) &ret, "glCreateShaderProgramv", (GLADapiproc) glad_glCreateShaderProgramv, 3, type, count, strings);
    return ret;
}
PFNGLCREATESHADERPROGRAMVPROC glad_debug_glCreateShaderProgramv = glad_debug_impl_glCreateShaderProgramv;
PFNGLCULLFACEPROC glad_glCullFace = NULL;
static void GLAD_API_PTR glad_debug_impl_glCullFace(GLenum mode) {
    _pre_call_gles2_callback("glCullFace", (GLADapiproc) glad_glCullFace, 1, mode);
    glad_glCullFace(mode);
    _post_call_gles2_callback(NULL, "glCullFace", (GLADapiproc) glad_glCullFace, 1, mode);
    
}
PFNGLCULLFACEPROC glad_debug_glCullFace = glad_debug_impl_glCullFace;
PFNGLDEBUGMESSAGECALLBACKPROC glad_glDebugMessageCallback = NULL;
static void GLAD_API_PTR glad_debug_impl_glDebugMessageCallback(GLDEBUGPROC callback, const void * userParam) {
    _pre_call_gles2_callback("glDebugMessageCallback", (GLADapiproc) glad_glDebugMessageCallback, 2, callback, userParam);
    glad_glDebugMessageCallback(callback, userParam);
    _post_call_gles2_callback(NULL, "glDebugMessageCallback", (GLADapiproc) glad_glDebugMessageCallback, 2, callback, userParam);
    
}
PFNGLDEBUGMESSAGECALLBACKPROC glad_debug_glDebugMessageCallback = glad_debug_impl_glDebugMessageCallback;
PFNGLDEBUGMESSAGECALLBACKKHRPROC glad_glDebugMessageCallbackKHR = NULL;
static void GLAD_API_PTR glad_debug_impl_glDebugMessageCallbackKHR(GLDEBUGPROCKHR callback, const void * userParam) {
    _pre_call_gles2_callback("glDebugMessageCallbackKHR", (GLADapiproc) glad_glDebugMessageCallbackKHR, 2, callback, userParam);
    glad_glDebugMessageCallbackKHR(callback, userParam);
    _post_call_gles2_callback(NULL, "glDebugMessageCallbackKHR", (GLADapiproc) glad_glDebugMessageCallbackKHR, 2, callback, userParam);
    
}
PFNGLDEBUGMESSAGECALLBACKKHRPROC glad_debug_glDebugMessageCallbackKHR = glad_debug_impl_glDebugMessageCallbackKHR;
PFNGLDEBUGMESSAGECONTROLPROC glad_glDebugMessageControl = NULL;
static void GLAD_API_PTR glad_debug_impl_glDebugMessageControl(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint * ids, GLboolean enabled) {
    _pre_call_gles2_callback("glDebugMessageControl", (GLADapiproc) glad_glDebugMessageControl, 6, source, type, severity, count, ids, enabled);
    glad_glDebugMessageControl(source, type, severity, count, ids, enabled);
    _post_call_gles2_callback(NULL, "glDebugMessageControl", (GLADapiproc) glad_glDebugMessageControl, 6, source, type, severity, count, ids, enabled);
    
}
PFNGLDEBUGMESSAGECONTROLPROC glad_debug_glDebugMessageControl = glad_debug_impl_glDebugMessageControl;
PFNGLDEBUGMESSAGECONTROLKHRPROC glad_glDebugMessageControlKHR = NULL;
static void GLAD_API_PTR glad_debug_impl_glDebugMessageControlKHR(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint * ids, GLboolean enabled) {
    _pre_call_gles2_callback("glDebugMessageControlKHR", (GLADapiproc) glad_glDebugMessageControlKHR, 6, source, type, severity, count, ids, enabled);
    glad_glDebugMessageControlKHR(source, type, severity, count, ids, enabled);
    _post_call_gles2_callback(NULL, "glDebugMessageControlKHR", (GLADapiproc) glad_glDebugMessageControlKHR, 6, source, type, severity, count, ids, enabled);
    
}
PFNGLDEBUGMESSAGECONTROLKHRPROC glad_debug_glDebugMessageControlKHR = glad_debug_impl_glDebugMessageControlKHR;
PFNGLDEBUGMESSAGEINSERTPROC glad_glDebugMessageInsert = NULL;
static void GLAD_API_PTR glad_debug_impl_glDebugMessageInsert(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * buf) {
    _pre_call_gles2_callback("glDebugMessageInsert", (GLADapiproc) glad_glDebugMessageInsert, 6, source, type, id, severity, length, buf);
    glad_glDebugMessageInsert(source, type, id, severity, length, buf);
    _post_call_gles2_callback(NULL, "glDebugMessageInsert", (GLADapiproc) glad_glDebugMessageInsert, 6, source, type, id, severity, length, buf);
    
}
PFNGLDEBUGMESSAGEINSERTPROC glad_debug_glDebugMessageInsert = glad_debug_impl_glDebugMessageInsert;
PFNGLDEBUGMESSAGEINSERTKHRPROC glad_glDebugMessageInsertKHR = NULL;
static void GLAD_API_PTR glad_debug_impl_glDebugMessageInsertKHR(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * buf) {
    _pre_call_gles2_callback("glDebugMessageInsertKHR", (GLADapiproc) glad_glDebugMessageInsertKHR, 6, source, type, id, severity, length, buf);
    glad_glDebugMessageInsertKHR(source, type, id, severity, length, buf);
    _post_call_gles2_callback(NULL, "glDebugMessageInsertKHR", (GLADapiproc) glad_glDebugMessageInsertKHR, 6, source, type, id, severity, length, buf);
    
}
PFNGLDEBUGMESSAGEINSERTKHRPROC glad_debug_glDebugMessageInsertKHR = glad_debug_impl_glDebugMessageInsertKHR;
PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers = NULL;
static void GLAD_API_PTR glad_debug_impl_glDeleteBuffers(GLsizei n, const GLuint * buffers) {
    _pre_call_gles2_callback("glDeleteBuffers", (GLADapiproc) glad_glDeleteBuffers, 2, n, buffers);
    glad_glDeleteBuffers(n, buffers);
    _post_call_gles2_callback(NULL, "glDeleteBuffers", (GLADapiproc) glad_glDeleteBuffers, 2, n, buffers);
    
}
PFNGLDELETEBUFFERSPROC glad_debug_glDeleteBuffers = glad_debug_impl_glDeleteBuffers;
PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers = NULL;
static void GLAD_API_PTR glad_debug_impl_glDeleteFramebuffers(GLsizei n, const GLuint * framebuffers) {
    _pre_call_gles2_callback("glDeleteFramebuffers", (GLADapiproc) glad_glDeleteFramebuffers, 2, n, framebuffers);
    glad_glDeleteFramebuffers(n, framebuffers);
    _post_call_gles2_callback(NULL, "glDeleteFramebuffers", (GLADapiproc) glad_glDeleteFramebuffers, 2, n, framebuffers);
    
}
PFNGLDELETEFRAMEBUFFERSPROC glad_debug_glDeleteFramebuffers = glad_debug_impl_glDeleteFramebuffers;
PFNGLDELETEPROGRAMPROC glad_glDeleteProgram = NULL;
static void GLAD_API_PTR glad_debug_impl_glDeleteProgram(GLuint program) {
    _pre_call_gles2_callback("glDeleteProgram", (GLADapiproc) glad_glDeleteProgram, 1, program);
    glad_glDeleteProgram(program);
    _post_call_gles2_callback(NULL, "glDeleteProgram", (GLADapiproc) glad_glDeleteProgram, 1, program);
    
}
PFNGLDELETEPROGRAMPROC glad_debug_glDeleteProgram = glad_debug_impl_glDeleteProgram;
PFNGLDELETEPROGRAMPIPELINESPROC glad_glDeleteProgramPipelines = NULL;
static void GLAD_API_PTR glad_debug_impl_glDeleteProgramPipelines(GLsizei n, const GLuint * pipelines) {
    _pre_call_gles2_callback("glDeleteProgramPipelines", (GLADapiproc) glad_glDeleteProgramPipelines, 2, n, pipelines);
    glad_glDeleteProgramPipelines(n, pipelines);
    _post_call_gles2_callback(NULL, "glDeleteProgramPipelines", (GLADapiproc) glad_glDeleteProgramPipelines, 2, n, pipelines);
    
}
PFNGLDELETEPROGRAMPIPELINESPROC glad_debug_glDeleteProgramPipelines = glad_debug_impl_glDeleteProgramPipelines;
PFNGLDELETEQUERIESPROC glad_glDeleteQueries = NULL;
static void GLAD_API_PTR glad_debug_impl_glDeleteQueries(GLsizei n, const GLuint * ids) {
    _pre_call_gles2_callback("glDeleteQueries", (GLADapiproc) glad_glDeleteQueries, 2, n, ids);
    glad_glDeleteQueries(n, ids);
    _post_call_gles2_callback(NULL, "glDeleteQueries", (GLADapiproc) glad_glDeleteQueries, 2, n, ids);
    
}
PFNGLDELETEQUERIESPROC glad_debug_glDeleteQueries = glad_debug_impl_glDeleteQueries;
PFNGLDELETERENDERBUFFERSPROC glad_glDeleteRenderbuffers = NULL;
static void GLAD_API_PTR glad_debug_impl_glDeleteRenderbuffers(GLsizei n, const GLuint * renderbuffers) {
    _pre_call_gles2_callback("glDeleteRenderbuffers", (GLADapiproc) glad_glDeleteRenderbuffers, 2, n, renderbuffers);
    glad_glDeleteRenderbuffers(n, renderbuffers);
    _post_call_gles2_callback(NULL, "glDeleteRenderbuffers", (GLADapiproc) glad_glDeleteRenderbuffers, 2, n, renderbuffers);
    
}
PFNGLDELETERENDERBUFFERSPROC glad_debug_glDeleteRenderbuffers = glad_debug_impl_glDeleteRenderbuffers;
PFNGLDELETESAMPLERSPROC glad_glDeleteSamplers = NULL;
static void GLAD_API_PTR glad_debug_impl_glDeleteSamplers(GLsizei count, const GLuint * samplers) {
    _pre_call_gles2_callback("glDeleteSamplers", (GLADapiproc) glad_glDeleteSamplers, 2, count, samplers);
    glad_glDeleteSamplers(count, samplers);
    _post_call_gles2_callback(NULL, "glDeleteSamplers", (GLADapiproc) glad_glDeleteSamplers, 2, count, samplers);
    
}
PFNGLDELETESAMPLERSPROC glad_debug_glDeleteSamplers = glad_debug_impl_glDeleteSamplers;
PFNGLDELETESHADERPROC glad_glDeleteShader = NULL;
static void GLAD_API_PTR glad_debug_impl_glDeleteShader(GLuint shader) {
    _pre_call_gles2_callback("glDeleteShader", (GLADapiproc) glad_glDeleteShader, 1, shader);
    glad_glDeleteShader(shader);
    _post_call_gles2_callback(NULL, "glDeleteShader", (GLADapiproc) glad_glDeleteShader, 1, shader);
    
}
PFNGLDELETESHADERPROC glad_debug_glDeleteShader = glad_debug_impl_glDeleteShader;
PFNGLDELETESYNCPROC glad_glDeleteSync = NULL;
static void GLAD_API_PTR glad_debug_impl_glDeleteSync(GLsync sync) {
    _pre_call_gles2_callback("glDeleteSync", (GLADapiproc) glad_glDeleteSync, 1, sync);
    glad_glDeleteSync(sync);
    _post_call_gles2_callback(NULL, "glDeleteSync", (GLADapiproc) glad_glDeleteSync, 1, sync);
    
}
PFNGLDELETESYNCPROC glad_debug_glDeleteSync = glad_debug_impl_glDeleteSync;
PFNGLDELETETEXTURESPROC glad_glDeleteTextures = NULL;
static void GLAD_API_PTR glad_debug_impl_glDeleteTextures(GLsizei n, const GLuint * textures) {
    _pre_call_gles2_callback("glDeleteTextures", (GLADapiproc) glad_glDeleteTextures, 2, n, textures);
    glad_glDeleteTextures(n, textures);
    _post_call_gles2_callback(NULL, "glDeleteTextures", (GLADapiproc) glad_glDeleteTextures, 2, n, textures);
    
}
PFNGLDELETETEXTURESPROC glad_debug_glDeleteTextures = glad_debug_impl_glDeleteTextures;
PFNGLDELETETRANSFORMFEEDBACKSPROC glad_glDeleteTransformFeedbacks = NULL;
static void GLAD_API_PTR glad_debug_impl_glDeleteTransformFeedbacks(GLsizei n, const GLuint * ids) {
    _pre_call_gles2_callback("glDeleteTransformFeedbacks", (GLADapiproc) glad_glDeleteTransformFeedbacks, 2, n, ids);
    glad_glDeleteTransformFeedbacks(n, ids);
    _post_call_gles2_callback(NULL, "glDeleteTransformFeedbacks", (GLADapiproc) glad_glDeleteTransformFeedbacks, 2, n, ids);
    
}
PFNGLDELETETRANSFORMFEEDBACKSPROC glad_debug_glDeleteTransformFeedbacks = glad_debug_impl_glDeleteTransformFeedbacks;
PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays = NULL;
static void GLAD_API_PTR glad_debug_impl_glDeleteVertexArrays(GLsizei n, const GLuint * arrays) {
    _pre_call_gles2_callback("glDeleteVertexArrays", (GLADapiproc) glad_glDeleteVertexArrays, 2, n, arrays);
    glad_glDeleteVertexArrays(n, arrays);
    _post_call_gles2_callback(NULL, "glDeleteVertexArrays", (GLADapiproc) glad_glDeleteVertexArrays, 2, n, arrays);
    
}
PFNGLDELETEVERTEXARRAYSPROC glad_debug_glDeleteVertexArrays = glad_debug_impl_glDeleteVertexArrays;
PFNGLDEPTHFUNCPROC glad_glDepthFunc = NULL;
static void GLAD_API_PTR glad_debug_impl_glDepthFunc(GLenum func) {
    _pre_call_gles2_callback("glDepthFunc", (GLADapiproc) glad_glDepthFunc, 1, func);
    glad_glDepthFunc(func);
    _post_call_gles2_callback(NULL, "glDepthFunc", (GLADapiproc) glad_glDepthFunc, 1, func);
    
}
PFNGLDEPTHFUNCPROC glad_debug_glDepthFunc = glad_debug_impl_glDepthFunc;
PFNGLDEPTHMASKPROC glad_glDepthMask = NULL;
static void GLAD_API_PTR glad_debug_impl_glDepthMask(GLboolean flag) {
    _pre_call_gles2_callback("glDepthMask", (GLADapiproc) glad_glDepthMask, 1, flag);
    glad_glDepthMask(flag);
    _post_call_gles2_callback(NULL, "glDepthMask", (GLADapiproc) glad_glDepthMask, 1, flag);
    
}
PFNGLDEPTHMASKPROC glad_debug_glDepthMask = glad_debug_impl_glDepthMask;
PFNGLDEPTHRANGEFPROC glad_glDepthRangef = NULL;
static void GLAD_API_PTR glad_debug_impl_glDepthRangef(GLfloat n, GLfloat f) {
    _pre_call_gles2_callback("glDepthRangef", (GLADapiproc) glad_glDepthRangef, 2, n, f);
    glad_glDepthRangef(n, f);
    _post_call_gles2_callback(NULL, "glDepthRangef", (GLADapiproc) glad_glDepthRangef, 2, n, f);
    
}
PFNGLDEPTHRANGEFPROC glad_debug_glDepthRangef = glad_debug_impl_glDepthRangef;
PFNGLDETACHSHADERPROC glad_glDetachShader = NULL;
static void GLAD_API_PTR glad_debug_impl_glDetachShader(GLuint program, GLuint shader) {
    _pre_call_gles2_callback("glDetachShader", (GLADapiproc) glad_glDetachShader, 2, program, shader);
    glad_glDetachShader(program, shader);
    _post_call_gles2_callback(NULL, "glDetachShader", (GLADapiproc) glad_glDetachShader, 2, program, shader);
    
}
PFNGLDETACHSHADERPROC glad_debug_glDetachShader = glad_debug_impl_glDetachShader;
PFNGLDISABLEPROC glad_glDisable = NULL;
static void GLAD_API_PTR glad_debug_impl_glDisable(GLenum cap) {
    _pre_call_gles2_callback("glDisable", (GLADapiproc) glad_glDisable, 1, cap);
    glad_glDisable(cap);
    _post_call_gles2_callback(NULL, "glDisable", (GLADapiproc) glad_glDisable, 1, cap);
    
}
PFNGLDISABLEPROC glad_debug_glDisable = glad_debug_impl_glDisable;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray = NULL;
static void GLAD_API_PTR glad_debug_impl_glDisableVertexAttribArray(GLuint index) {
    _pre_call_gles2_callback("glDisableVertexAttribArray", (GLADapiproc) glad_glDisableVertexAttribArray, 1, index);
    glad_glDisableVertexAttribArray(index);
    _post_call_gles2_callback(NULL, "glDisableVertexAttribArray", (GLADapiproc) glad_glDisableVertexAttribArray, 1, index);
    
}
PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_debug_glDisableVertexAttribArray = glad_debug_impl_glDisableVertexAttribArray;
PFNGLDISABLEIPROC glad_glDisablei = NULL;
static void GLAD_API_PTR glad_debug_impl_glDisablei(GLenum target, GLuint index) {
    _pre_call_gles2_callback("glDisablei", (GLADapiproc) glad_glDisablei, 2, target, index);
    glad_glDisablei(target, index);
    _post_call_gles2_callback(NULL, "glDisablei", (GLADapiproc) glad_glDisablei, 2, target, index);
    
}
PFNGLDISABLEIPROC glad_debug_glDisablei = glad_debug_impl_glDisablei;
PFNGLDISPATCHCOMPUTEPROC glad_glDispatchCompute = NULL;
static void GLAD_API_PTR glad_debug_impl_glDispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) {
    _pre_call_gles2_callback("glDispatchCompute", (GLADapiproc) glad_glDispatchCompute, 3, num_groups_x, num_groups_y, num_groups_z);
    glad_glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    _post_call_gles2_callback(NULL, "glDispatchCompute", (GLADapiproc) glad_glDispatchCompute, 3, num_groups_x, num_groups_y, num_groups_z);
    
}
PFNGLDISPATCHCOMPUTEPROC glad_debug_glDispatchCompute = glad_debug_impl_glDispatchCompute;
PFNGLDISPATCHCOMPUTEINDIRECTPROC glad_glDispatchComputeIndirect = NULL;
static void GLAD_API_PTR glad_debug_impl_glDispatchComputeIndirect(GLintptr indirect) {
    _pre_call_gles2_callback("glDispatchComputeIndirect", (GLADapiproc) glad_glDispatchComputeIndirect, 1, indirect);
    glad_glDispatchComputeIndirect(indirect);
    _post_call_gles2_callback(NULL, "glDispatchComputeIndirect", (GLADapiproc) glad_glDispatchComputeIndirect, 1, indirect);
    
}
PFNGLDISPATCHCOMPUTEINDIRECTPROC glad_debug_glDispatchComputeIndirect = glad_debug_impl_glDispatchComputeIndirect;
PFNGLDRAWARRAYSPROC glad_glDrawArrays = NULL;
static void GLAD_API_PTR glad_debug_impl_glDrawArrays(GLenum mode, GLint first, GLsizei count) {
    _pre_call_gles2_callback("glDrawArrays", (GLADapiproc) glad_glDrawArrays, 3, mode, first, count);
    glad_glDrawArrays(mode, first, count);
    _post_call_gles2_callback(NULL, "glDrawArrays", (GLADapiproc) glad_glDrawArrays, 3, mode, first, count);
    
}
PFNGLDRAWARRAYSPROC glad_debug_glDrawArrays = glad_debug_impl_glDrawArrays;
PFNGLDRAWARRAYSINDIRECTPROC glad_glDrawArraysIndirect = NULL;
static void GLAD_API_PTR glad_debug_impl_glDrawArraysIndirect(GLenum mode, const void * indirect) {
    _pre_call_gles2_callback("glDrawArraysIndirect", (GLADapiproc) glad_glDrawArraysIndirect, 2, mode, indirect);
    glad_glDrawArraysIndirect(mode, indirect);
    _post_call_gles2_callback(NULL, "glDrawArraysIndirect", (GLADapiproc) glad_glDrawArraysIndirect, 2, mode, indirect);
    
}
PFNGLDRAWARRAYSINDIRECTPROC glad_debug_glDrawArraysIndirect = glad_debug_impl_glDrawArraysIndirect;
PFNGLDRAWARRAYSINSTANCEDPROC glad_glDrawArraysInstanced = NULL;
static void GLAD_API_PTR glad_debug_impl_glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount) {
    _pre_call_gles2_callback("glDrawArraysInstanced", (GLADapiproc) glad_glDrawArraysInstanced, 4, mode, first, count, instancecount);
    glad_glDrawArraysInstanced(mode, first, count, instancecount);
    _post_call_gles2_callback(NULL, "glDrawArraysInstanced", (GLADapiproc) glad_glDrawArraysInstanced, 4, mode, first, count, instancecount);
    
}
PFNGLDRAWARRAYSINSTANCEDPROC glad_debug_glDrawArraysInstanced = glad_debug_impl_glDrawArraysInstanced;
PFNGLDRAWBUFFERSPROC glad_glDrawBuffers = NULL;
static void GLAD_API_PTR glad_debug_impl_glDrawBuffers(GLsizei n, const GLenum * bufs) {
    _pre_call_gles2_callback("glDrawBuffers", (GLADapiproc) glad_glDrawBuffers, 2, n, bufs);
    glad_glDrawBuffers(n, bufs);
    _post_call_gles2_callback(NULL, "glDrawBuffers", (GLADapiproc) glad_glDrawBuffers, 2, n, bufs);
    
}
PFNGLDRAWBUFFERSPROC glad_debug_glDrawBuffers = glad_debug_impl_glDrawBuffers;
PFNGLDRAWELEMENTSPROC glad_glDrawElements = NULL;
static void GLAD_API_PTR glad_debug_impl_glDrawElements(GLenum mode, GLsizei count, GLenum type, const void * indices) {
    _pre_call_gles2_callback("glDrawElements", (GLADapiproc) glad_glDrawElements, 4, mode, count, type, indices);
    glad_glDrawElements(mode, count, type, indices);
    _post_call_gles2_callback(NULL, "glDrawElements", (GLADapiproc) glad_glDrawElements, 4, mode, count, type, indices);
    
}
PFNGLDRAWELEMENTSPROC glad_debug_glDrawElements = glad_debug_impl_glDrawElements;
PFNGLDRAWELEMENTSBASEVERTEXPROC glad_glDrawElementsBaseVertex = NULL;
static void GLAD_API_PTR glad_debug_impl_glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const void * indices, GLint basevertex) {
    _pre_call_gles2_callback("glDrawElementsBaseVertex", (GLADapiproc) glad_glDrawElementsBaseVertex, 5, mode, count, type, indices, basevertex);
    glad_glDrawElementsBaseVertex(mode, count, type, indices, basevertex);
    _post_call_gles2_callback(NULL, "glDrawElementsBaseVertex", (GLADapiproc) glad_glDrawElementsBaseVertex, 5, mode, count, type, indices, basevertex);
    
}
PFNGLDRAWELEMENTSBASEVERTEXPROC glad_debug_glDrawElementsBaseVertex = glad_debug_impl_glDrawElementsBaseVertex;
PFNGLDRAWELEMENTSINDIRECTPROC glad_glDrawElementsIndirect = NULL;
static void GLAD_API_PTR glad_debug_impl_glDrawElementsIndirect(GLenum mode, GLenum type, const void * indirect) {
    _pre_call_gles2_callback("glDrawElementsIndirect", (GLADapiproc) glad_glDrawElementsIndirect, 3, mode, type, indirect);
    glad_glDrawElementsIndirect(mode, type, indirect);
    _post_call_gles2_callback(NULL, "glDrawElementsIndirect", (GLADapiproc) glad_glDrawElementsIndirect, 3, mode, type, indirect);
    
}
PFNGLDRAWELEMENTSINDIRECTPROC glad_debug_glDrawElementsIndirect = glad_debug_impl_glDrawElementsIndirect;
PFNGLDRAWELEMENTSINSTANCEDPROC glad_glDrawElementsInstanced = NULL;
static void GLAD_API_PTR glad_debug_impl_glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount) {
    _pre_call_gles2_callback("glDrawElementsInstanced", (GLADapiproc) glad_glDrawElementsInstanced, 5, mode, count, type, indices, instancecount);
    glad_glDrawElementsInstanced(mode, count, type, indices, instancecount);
    _post_call_gles2_callback(NULL, "glDrawElementsInstanced", (GLADapiproc) glad_glDrawElementsInstanced, 5, mode, count, type, indices, instancecount);
    
}
PFNGLDRAWELEMENTSINSTANCEDPROC glad_debug_glDrawElementsInstanced = glad_debug_impl_glDrawElementsInstanced;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glad_glDrawElementsInstancedBaseVertex = NULL;
static void GLAD_API_PTR glad_debug_impl_glDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount, GLint basevertex) {
    _pre_call_gles2_callback("glDrawElementsInstancedBaseVertex", (GLADapiproc) glad_glDrawElementsInstancedBaseVertex, 6, mode, count, type, indices, instancecount, basevertex);
    glad_glDrawElementsInstancedBaseVertex(mode, count, type, indices, instancecount, basevertex);
    _post_call_gles2_callback(NULL, "glDrawElementsInstancedBaseVertex", (GLADapiproc) glad_glDrawElementsInstancedBaseVertex, 6, mode, count, type, indices, instancecount, basevertex);
    
}
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glad_debug_glDrawElementsInstancedBaseVertex = glad_debug_impl_glDrawElementsInstancedBaseVertex;
PFNGLDRAWRANGEELEMENTSPROC glad_glDrawRangeElements = NULL;
static void GLAD_API_PTR glad_debug_impl_glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void * indices) {
    _pre_call_gles2_callback("glDrawRangeElements", (GLADapiproc) glad_glDrawRangeElements, 6, mode, start, end, count, type, indices);
    glad_glDrawRangeElements(mode, start, end, count, type, indices);
    _post_call_gles2_callback(NULL, "glDrawRangeElements", (GLADapiproc) glad_glDrawRangeElements, 6, mode, start, end, count, type, indices);
    
}
PFNGLDRAWRANGEELEMENTSPROC glad_debug_glDrawRangeElements = glad_debug_impl_glDrawRangeElements;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glad_glDrawRangeElementsBaseVertex = NULL;
static void GLAD_API_PTR glad_debug_impl_glDrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void * indices, GLint basevertex) {
    _pre_call_gles2_callback("glDrawRangeElementsBaseVertex", (GLADapiproc) glad_glDrawRangeElementsBaseVertex, 7, mode, start, end, count, type, indices, basevertex);
    glad_glDrawRangeElementsBaseVertex(mode, start, end, count, type, indices, basevertex);
    _post_call_gles2_callback(NULL, "glDrawRangeElementsBaseVertex", (GLADapiproc) glad_glDrawRangeElementsBaseVertex, 7, mode, start, end, count, type, indices, basevertex);
    
}
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glad_debug_glDrawRangeElementsBaseVertex = glad_debug_impl_glDrawRangeElementsBaseVertex;
PFNGLENABLEPROC glad_glEnable = NULL;
static void GLAD_API_PTR glad_debug_impl_glEnable(GLenum cap) {
    _pre_call_gles2_callback("glEnable", (GLADapiproc) glad_glEnable, 1, cap);
    glad_glEnable(cap);
    _post_call_gles2_callback(NULL, "glEnable", (GLADapiproc) glad_glEnable, 1, cap);
    
}
PFNGLENABLEPROC glad_debug_glEnable = glad_debug_impl_glEnable;
PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray = NULL;
static void GLAD_API_PTR glad_debug_impl_glEnableVertexAttribArray(GLuint index) {
    _pre_call_gles2_callback("glEnableVertexAttribArray", (GLADapiproc) glad_glEnableVertexAttribArray, 1, index);
    glad_glEnableVertexAttribArray(index);
    _post_call_gles2_callback(NULL, "glEnableVertexAttribArray", (GLADapiproc) glad_glEnableVertexAttribArray, 1, index);
    
}
PFNGLENABLEVERTEXATTRIBARRAYPROC glad_debug_glEnableVertexAttribArray = glad_debug_impl_glEnableVertexAttribArray;
PFNGLENABLEIPROC glad_glEnablei = NULL;
static void GLAD_API_PTR glad_debug_impl_glEnablei(GLenum target, GLuint index) {
    _pre_call_gles2_callback("glEnablei", (GLADapiproc) glad_glEnablei, 2, target, index);
    glad_glEnablei(target, index);
    _post_call_gles2_callback(NULL, "glEnablei", (GLADapiproc) glad_glEnablei, 2, target, index);
    
}
PFNGLENABLEIPROC glad_debug_glEnablei = glad_debug_impl_glEnablei;
PFNGLENDQUERYPROC glad_glEndQuery = NULL;
static void GLAD_API_PTR glad_debug_impl_glEndQuery(GLenum target) {
    _pre_call_gles2_callback("glEndQuery", (GLADapiproc) glad_glEndQuery, 1, target);
    glad_glEndQuery(target);
    _post_call_gles2_callback(NULL, "glEndQuery", (GLADapiproc) glad_glEndQuery, 1, target);
    
}
PFNGLENDQUERYPROC glad_debug_glEndQuery = glad_debug_impl_glEndQuery;
PFNGLENDTRANSFORMFEEDBACKPROC glad_glEndTransformFeedback = NULL;
static void GLAD_API_PTR glad_debug_impl_glEndTransformFeedback(void) {
    _pre_call_gles2_callback("glEndTransformFeedback", (GLADapiproc) glad_glEndTransformFeedback, 0);
    glad_glEndTransformFeedback();
    _post_call_gles2_callback(NULL, "glEndTransformFeedback", (GLADapiproc) glad_glEndTransformFeedback, 0);
    
}
PFNGLENDTRANSFORMFEEDBACKPROC glad_debug_glEndTransformFeedback = glad_debug_impl_glEndTransformFeedback;
PFNGLFENCESYNCPROC glad_glFenceSync = NULL;
static GLsync GLAD_API_PTR glad_debug_impl_glFenceSync(GLenum condition, GLbitfield flags) {
    GLsync ret;
    _pre_call_gles2_callback("glFenceSync", (GLADapiproc) glad_glFenceSync, 2, condition, flags);
    ret = glad_glFenceSync(condition, flags);
    _post_call_gles2_callback((void*) &ret, "glFenceSync", (GLADapiproc) glad_glFenceSync, 2, condition, flags);
    return ret;
}
PFNGLFENCESYNCPROC glad_debug_glFenceSync = glad_debug_impl_glFenceSync;
PFNGLFINISHPROC glad_glFinish = NULL;
static void GLAD_API_PTR glad_debug_impl_glFinish(void) {
    _pre_call_gles2_callback("glFinish", (GLADapiproc) glad_glFinish, 0);
    glad_glFinish();
    _post_call_gles2_callback(NULL, "glFinish", (GLADapiproc) glad_glFinish, 0);
    
}
PFNGLFINISHPROC glad_debug_glFinish = glad_debug_impl_glFinish;
PFNGLFLUSHPROC glad_glFlush = NULL;
static void GLAD_API_PTR glad_debug_impl_glFlush(void) {
    _pre_call_gles2_callback("glFlush", (GLADapiproc) glad_glFlush, 0);
    glad_glFlush();
    _post_call_gles2_callback(NULL, "glFlush", (GLADapiproc) glad_glFlush, 0);
    
}
PFNGLFLUSHPROC glad_debug_glFlush = glad_debug_impl_glFlush;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC glad_glFlushMappedBufferRange = NULL;
static void GLAD_API_PTR glad_debug_impl_glFlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length) {
    _pre_call_gles2_callback("glFlushMappedBufferRange", (GLADapiproc) glad_glFlushMappedBufferRange, 3, target, offset, length);
    glad_glFlushMappedBufferRange(target, offset, length);
    _post_call_gles2_callback(NULL, "glFlushMappedBufferRange", (GLADapiproc) glad_glFlushMappedBufferRange, 3, target, offset, length);
    
}
PFNGLFLUSHMAPPEDBUFFERRANGEPROC glad_debug_glFlushMappedBufferRange = glad_debug_impl_glFlushMappedBufferRange;
PFNGLFRAMEBUFFERPARAMETERIPROC glad_glFramebufferParameteri = NULL;
static void GLAD_API_PTR glad_debug_impl_glFramebufferParameteri(GLenum target, GLenum pname, GLint param) {
    _pre_call_gles2_callback("glFramebufferParameteri", (GLADapiproc) glad_glFramebufferParameteri, 3, target, pname, param);
    glad_glFramebufferParameteri(target, pname, param);
    _post_call_gles2_callback(NULL, "glFramebufferParameteri", (GLADapiproc) glad_glFramebufferParameteri, 3, target, pname, param);
    
}
PFNGLFRAMEBUFFERPARAMETERIPROC glad_debug_glFramebufferParameteri = glad_debug_impl_glFramebufferParameteri;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_glFramebufferRenderbuffer = NULL;
static void GLAD_API_PTR glad_debug_impl_glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
    _pre_call_gles2_callback("glFramebufferRenderbuffer", (GLADapiproc) glad_glFramebufferRenderbuffer, 4, target, attachment, renderbuffertarget, renderbuffer);
    glad_glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
    _post_call_gles2_callback(NULL, "glFramebufferRenderbuffer", (GLADapiproc) glad_glFramebufferRenderbuffer, 4, target, attachment, renderbuffertarget, renderbuffer);
    
}
PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_debug_glFramebufferRenderbuffer = glad_debug_impl_glFramebufferRenderbuffer;
PFNGLFRAMEBUFFERTEXTUREPROC glad_glFramebufferTexture = NULL;
static void GLAD_API_PTR glad_debug_impl_glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level) {
    _pre_call_gles2_callback("glFramebufferTexture", (GLADapiproc) glad_glFramebufferTexture, 4, target, attachment, texture, level);
    glad_glFramebufferTexture(target, attachment, texture, level);
    _post_call_gles2_callback(NULL, "glFramebufferTexture", (GLADapiproc) glad_glFramebufferTexture, 4, target, attachment, texture, level);
    
}
PFNGLFRAMEBUFFERTEXTUREPROC glad_debug_glFramebufferTexture = glad_debug_impl_glFramebufferTexture;
PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D = NULL;
static void GLAD_API_PTR glad_debug_impl_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
    _pre_call_gles2_callback("glFramebufferTexture2D", (GLADapiproc) glad_glFramebufferTexture2D, 5, target, attachment, textarget, texture, level);
    glad_glFramebufferTexture2D(target, attachment, textarget, texture, level);
    _post_call_gles2_callback(NULL, "glFramebufferTexture2D", (GLADapiproc) glad_glFramebufferTexture2D, 5, target, attachment, textarget, texture, level);
    
}
PFNGLFRAMEBUFFERTEXTURE2DPROC glad_debug_glFramebufferTexture2D = glad_debug_impl_glFramebufferTexture2D;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glad_glFramebufferTextureLayer = NULL;
static void GLAD_API_PTR glad_debug_impl_glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer) {
    _pre_call_gles2_callback("glFramebufferTextureLayer", (GLADapiproc) glad_glFramebufferTextureLayer, 5, target, attachment, texture, level, layer);
    glad_glFramebufferTextureLayer(target, attachment, texture, level, layer);
    _post_call_gles2_callback(NULL, "glFramebufferTextureLayer", (GLADapiproc) glad_glFramebufferTextureLayer, 5, target, attachment, texture, level, layer);
    
}
PFNGLFRAMEBUFFERTEXTURELAYERPROC glad_debug_glFramebufferTextureLayer = glad_debug_impl_glFramebufferTextureLayer;
PFNGLFRONTFACEPROC glad_glFrontFace = NULL;
static void GLAD_API_PTR glad_debug_impl_glFrontFace(GLenum mode) {
    _pre_call_gles2_callback("glFrontFace", (GLADapiproc) glad_glFrontFace, 1, mode);
    glad_glFrontFace(mode);
    _post_call_gles2_callback(NULL, "glFrontFace", (GLADapiproc) glad_glFrontFace, 1, mode);
    
}
PFNGLFRONTFACEPROC glad_debug_glFrontFace = glad_debug_impl_glFrontFace;
PFNGLGENBUFFERSPROC glad_glGenBuffers = NULL;
static void GLAD_API_PTR glad_debug_impl_glGenBuffers(GLsizei n, GLuint * buffers) {
    _pre_call_gles2_callback("glGenBuffers", (GLADapiproc) glad_glGenBuffers, 2, n, buffers);
    glad_glGenBuffers(n, buffers);
    _post_call_gles2_callback(NULL, "glGenBuffers", (GLADapiproc) glad_glGenBuffers, 2, n, buffers);
    
}
PFNGLGENBUFFERSPROC glad_debug_glGenBuffers = glad_debug_impl_glGenBuffers;
PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers = NULL;
static void GLAD_API_PTR glad_debug_impl_glGenFramebuffers(GLsizei n, GLuint * framebuffers) {
    _pre_call_gles2_callback("glGenFramebuffers", (GLADapiproc) glad_glGenFramebuffers, 2, n, framebuffers);
    glad_glGenFramebuffers(n, framebuffers);
    _post_call_gles2_callback(NULL, "glGenFramebuffers", (GLADapiproc) glad_glGenFramebuffers, 2, n, framebuffers);
    
}
PFNGLGENFRAMEBUFFERSPROC glad_debug_glGenFramebuffers = glad_debug_impl_glGenFramebuffers;
PFNGLGENPROGRAMPIPELINESPROC glad_glGenProgramPipelines = NULL;
static void GLAD_API_PTR glad_debug_impl_glGenProgramPipelines(GLsizei n, GLuint * pipelines) {
    _pre_call_gles2_callback("glGenProgramPipelines", (GLADapiproc) glad_glGenProgramPipelines, 2, n, pipelines);
    glad_glGenProgramPipelines(n, pipelines);
    _post_call_gles2_callback(NULL, "glGenProgramPipelines", (GLADapiproc) glad_glGenProgramPipelines, 2, n, pipelines);
    
}
PFNGLGENPROGRAMPIPELINESPROC glad_debug_glGenProgramPipelines = glad_debug_impl_glGenProgramPipelines;
PFNGLGENQUERIESPROC glad_glGenQueries = NULL;
static void GLAD_API_PTR glad_debug_impl_glGenQueries(GLsizei n, GLuint * ids) {
    _pre_call_gles2_callback("glGenQueries", (GLADapiproc) glad_glGenQueries, 2, n, ids);
    glad_glGenQueries(n, ids);
    _post_call_gles2_callback(NULL, "glGenQueries", (GLADapiproc) glad_glGenQueries, 2, n, ids);
    
}
PFNGLGENQUERIESPROC glad_debug_glGenQueries = glad_debug_impl_glGenQueries;
PFNGLGENRENDERBUFFERSPROC glad_glGenRenderbuffers = NULL;
static void GLAD_API_PTR glad_debug_impl_glGenRenderbuffers(GLsizei n, GLuint * renderbuffers) {
    _pre_call_gles2_callback("glGenRenderbuffers", (GLADapiproc) glad_glGenRenderbuffers, 2, n, renderbuffers);
    glad_glGenRenderbuffers(n, renderbuffers);
    _post_call_gles2_callback(NULL, "glGenRenderbuffers", (GLADapiproc) glad_glGenRenderbuffers, 2, n, renderbuffers);
    
}
PFNGLGENRENDERBUFFERSPROC glad_debug_glGenRenderbuffers = glad_debug_impl_glGenRenderbuffers;
PFNGLGENSAMPLERSPROC glad_glGenSamplers = NULL;
static void GLAD_API_PTR glad_debug_impl_glGenSamplers(GLsizei count, GLuint * samplers) {
    _pre_call_gles2_callback("glGenSamplers", (GLADapiproc) glad_glGenSamplers, 2, count, samplers);
    glad_glGenSamplers(count, samplers);
    _post_call_gles2_callback(NULL, "glGenSamplers", (GLADapiproc) glad_glGenSamplers, 2, count, samplers);
    
}
PFNGLGENSAMPLERSPROC glad_debug_glGenSamplers = glad_debug_impl_glGenSamplers;
PFNGLGENTEXTURESPROC glad_glGenTextures = NULL;
static void GLAD_API_PTR glad_debug_impl_glGenTextures(GLsizei n, GLuint * textures) {
    _pre_call_gles2_callback("glGenTextures", (GLADapiproc) glad_glGenTextures, 2, n, textures);
    glad_glGenTextures(n, textures);
    _post_call_gles2_callback(NULL, "glGenTextures", (GLADapiproc) glad_glGenTextures, 2, n, textures);
    
}
PFNGLGENTEXTURESPROC glad_debug_glGenTextures = glad_debug_impl_glGenTextures;
PFNGLGENTRANSFORMFEEDBACKSPROC glad_glGenTransformFeedbacks = NULL;
static void GLAD_API_PTR glad_debug_impl_glGenTransformFeedbacks(GLsizei n, GLuint * ids) {
    _pre_call_gles2_callback("glGenTransformFeedbacks", (GLADapiproc) glad_glGenTransformFeedbacks, 2, n, ids);
    glad_glGenTransformFeedbacks(n, ids);
    _post_call_gles2_callback(NULL, "glGenTransformFeedbacks", (GLADapiproc) glad_glGenTransformFeedbacks, 2, n, ids);
    
}
PFNGLGENTRANSFORMFEEDBACKSPROC glad_debug_glGenTransformFeedbacks = glad_debug_impl_glGenTransformFeedbacks;
PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays = NULL;
static void GLAD_API_PTR glad_debug_impl_glGenVertexArrays(GLsizei n, GLuint * arrays) {
    _pre_call_gles2_callback("glGenVertexArrays", (GLADapiproc) glad_glGenVertexArrays, 2, n, arrays);
    glad_glGenVertexArrays(n, arrays);
    _post_call_gles2_callback(NULL, "glGenVertexArrays", (GLADapiproc) glad_glGenVertexArrays, 2, n, arrays);
    
}
PFNGLGENVERTEXARRAYSPROC glad_debug_glGenVertexArrays = glad_debug_impl_glGenVertexArrays;
PFNGLGENERATEMIPMAPPROC glad_glGenerateMipmap = NULL;
static void GLAD_API_PTR glad_debug_impl_glGenerateMipmap(GLenum target) {
    _pre_call_gles2_callback("glGenerateMipmap", (GLADapiproc) glad_glGenerateMipmap, 1, target);
    glad_glGenerateMipmap(target);
    _post_call_gles2_callback(NULL, "glGenerateMipmap", (GLADapiproc) glad_glGenerateMipmap, 1, target);
    
}
PFNGLGENERATEMIPMAPPROC glad_debug_glGenerateMipmap = glad_debug_impl_glGenerateMipmap;
PFNGLGETACTIVEATTRIBPROC glad_glGetActiveAttrib = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name) {
    _pre_call_gles2_callback("glGetActiveAttrib", (GLADapiproc) glad_glGetActiveAttrib, 7, program, index, bufSize, length, size, type, name);
    glad_glGetActiveAttrib(program, index, bufSize, length, size, type, name);
    _post_call_gles2_callback(NULL, "glGetActiveAttrib", (GLADapiproc) glad_glGetActiveAttrib, 7, program, index, bufSize, length, size, type, name);
    
}
PFNGLGETACTIVEATTRIBPROC glad_debug_glGetActiveAttrib = glad_debug_impl_glGetActiveAttrib;
PFNGLGETACTIVEUNIFORMPROC glad_glGetActiveUniform = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name) {
    _pre_call_gles2_callback("glGetActiveUniform", (GLADapiproc) glad_glGetActiveUniform, 7, program, index, bufSize, length, size, type, name);
    glad_glGetActiveUniform(program, index, bufSize, length, size, type, name);
    _post_call_gles2_callback(NULL, "glGetActiveUniform", (GLADapiproc) glad_glGetActiveUniform, 7, program, index, bufSize, length, size, type, name);
    
}
PFNGLGETACTIVEUNIFORMPROC glad_debug_glGetActiveUniform = glad_debug_impl_glGetActiveUniform;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glad_glGetActiveUniformBlockName = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei * length, GLchar * uniformBlockName) {
    _pre_call_gles2_callback("glGetActiveUniformBlockName", (GLADapiproc) glad_glGetActiveUniformBlockName, 5, program, uniformBlockIndex, bufSize, length, uniformBlockName);
    glad_glGetActiveUniformBlockName(program, uniformBlockIndex, bufSize, length, uniformBlockName);
    _post_call_gles2_callback(NULL, "glGetActiveUniformBlockName", (GLADapiproc) glad_glGetActiveUniformBlockName, 5, program, uniformBlockIndex, bufSize, length, uniformBlockName);
    
}
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glad_debug_glGetActiveUniformBlockName = glad_debug_impl_glGetActiveUniformBlockName;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glad_glGetActiveUniformBlockiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetActiveUniformBlockiv", (GLADapiproc) glad_glGetActiveUniformBlockiv, 4, program, uniformBlockIndex, pname, params);
    glad_glGetActiveUniformBlockiv(program, uniformBlockIndex, pname, params);
    _post_call_gles2_callback(NULL, "glGetActiveUniformBlockiv", (GLADapiproc) glad_glGetActiveUniformBlockiv, 4, program, uniformBlockIndex, pname, params);
    
}
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glad_debug_glGetActiveUniformBlockiv = glad_debug_impl_glGetActiveUniformBlockiv;
PFNGLGETACTIVEUNIFORMSIVPROC glad_glGetActiveUniformsiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint * uniformIndices, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetActiveUniformsiv", (GLADapiproc) glad_glGetActiveUniformsiv, 5, program, uniformCount, uniformIndices, pname, params);
    glad_glGetActiveUniformsiv(program, uniformCount, uniformIndices, pname, params);
    _post_call_gles2_callback(NULL, "glGetActiveUniformsiv", (GLADapiproc) glad_glGetActiveUniformsiv, 5, program, uniformCount, uniformIndices, pname, params);
    
}
PFNGLGETACTIVEUNIFORMSIVPROC glad_debug_glGetActiveUniformsiv = glad_debug_impl_glGetActiveUniformsiv;
PFNGLGETATTACHEDSHADERSPROC glad_glGetAttachedShaders = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetAttachedShaders(GLuint program, GLsizei maxCount, GLsizei * count, GLuint * shaders) {
    _pre_call_gles2_callback("glGetAttachedShaders", (GLADapiproc) glad_glGetAttachedShaders, 4, program, maxCount, count, shaders);
    glad_glGetAttachedShaders(program, maxCount, count, shaders);
    _post_call_gles2_callback(NULL, "glGetAttachedShaders", (GLADapiproc) glad_glGetAttachedShaders, 4, program, maxCount, count, shaders);
    
}
PFNGLGETATTACHEDSHADERSPROC glad_debug_glGetAttachedShaders = glad_debug_impl_glGetAttachedShaders;
PFNGLGETATTRIBLOCATIONPROC glad_glGetAttribLocation = NULL;
static GLint GLAD_API_PTR glad_debug_impl_glGetAttribLocation(GLuint program, const GLchar * name) {
    GLint ret;
    _pre_call_gles2_callback("glGetAttribLocation", (GLADapiproc) glad_glGetAttribLocation, 2, program, name);
    ret = glad_glGetAttribLocation(program, name);
    _post_call_gles2_callback((void*) &ret, "glGetAttribLocation", (GLADapiproc) glad_glGetAttribLocation, 2, program, name);
    return ret;
}
PFNGLGETATTRIBLOCATIONPROC glad_debug_glGetAttribLocation = glad_debug_impl_glGetAttribLocation;
PFNGLGETBOOLEANI_VPROC glad_glGetBooleani_v = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetBooleani_v(GLenum target, GLuint index, GLboolean * data) {
    _pre_call_gles2_callback("glGetBooleani_v", (GLADapiproc) glad_glGetBooleani_v, 3, target, index, data);
    glad_glGetBooleani_v(target, index, data);
    _post_call_gles2_callback(NULL, "glGetBooleani_v", (GLADapiproc) glad_glGetBooleani_v, 3, target, index, data);
    
}
PFNGLGETBOOLEANI_VPROC glad_debug_glGetBooleani_v = glad_debug_impl_glGetBooleani_v;
PFNGLGETBOOLEANVPROC glad_glGetBooleanv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetBooleanv(GLenum pname, GLboolean * data) {
    _pre_call_gles2_callback("glGetBooleanv", (GLADapiproc) glad_glGetBooleanv, 2, pname, data);
    glad_glGetBooleanv(pname, data);
    _post_call_gles2_callback(NULL, "glGetBooleanv", (GLADapiproc) glad_glGetBooleanv, 2, pname, data);
    
}
PFNGLGETBOOLEANVPROC glad_debug_glGetBooleanv = glad_debug_impl_glGetBooleanv;
PFNGLGETBUFFERPARAMETERI64VPROC glad_glGetBufferParameteri64v = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetBufferParameteri64v(GLenum target, GLenum pname, GLint64 * params) {
    _pre_call_gles2_callback("glGetBufferParameteri64v", (GLADapiproc) glad_glGetBufferParameteri64v, 3, target, pname, params);
    glad_glGetBufferParameteri64v(target, pname, params);
    _post_call_gles2_callback(NULL, "glGetBufferParameteri64v", (GLADapiproc) glad_glGetBufferParameteri64v, 3, target, pname, params);
    
}
PFNGLGETBUFFERPARAMETERI64VPROC glad_debug_glGetBufferParameteri64v = glad_debug_impl_glGetBufferParameteri64v;
PFNGLGETBUFFERPARAMETERIVPROC glad_glGetBufferParameteriv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetBufferParameteriv(GLenum target, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetBufferParameteriv", (GLADapiproc) glad_glGetBufferParameteriv, 3, target, pname, params);
    glad_glGetBufferParameteriv(target, pname, params);
    _post_call_gles2_callback(NULL, "glGetBufferParameteriv", (GLADapiproc) glad_glGetBufferParameteriv, 3, target, pname, params);
    
}
PFNGLGETBUFFERPARAMETERIVPROC glad_debug_glGetBufferParameteriv = glad_debug_impl_glGetBufferParameteriv;
PFNGLGETBUFFERPOINTERVPROC glad_glGetBufferPointerv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetBufferPointerv(GLenum target, GLenum pname, void ** params) {
    _pre_call_gles2_callback("glGetBufferPointerv", (GLADapiproc) glad_glGetBufferPointerv, 3, target, pname, params);
    glad_glGetBufferPointerv(target, pname, params);
    _post_call_gles2_callback(NULL, "glGetBufferPointerv", (GLADapiproc) glad_glGetBufferPointerv, 3, target, pname, params);
    
}
PFNGLGETBUFFERPOINTERVPROC glad_debug_glGetBufferPointerv = glad_debug_impl_glGetBufferPointerv;
PFNGLGETDEBUGMESSAGELOGPROC glad_glGetDebugMessageLog = NULL;
static GLuint GLAD_API_PTR glad_debug_impl_glGetDebugMessageLog(GLuint count, GLsizei bufSize, GLenum * sources, GLenum * types, GLuint * ids, GLenum * severities, GLsizei * lengths, GLchar * messageLog) {
    GLuint ret;
    _pre_call_gles2_callback("glGetDebugMessageLog", (GLADapiproc) glad_glGetDebugMessageLog, 8, count, bufSize, sources, types, ids, severities, lengths, messageLog);
    ret = glad_glGetDebugMessageLog(count, bufSize, sources, types, ids, severities, lengths, messageLog);
    _post_call_gles2_callback((void*) &ret, "glGetDebugMessageLog", (GLADapiproc) glad_glGetDebugMessageLog, 8, count, bufSize, sources, types, ids, severities, lengths, messageLog);
    return ret;
}
PFNGLGETDEBUGMESSAGELOGPROC glad_debug_glGetDebugMessageLog = glad_debug_impl_glGetDebugMessageLog;
PFNGLGETDEBUGMESSAGELOGKHRPROC glad_glGetDebugMessageLogKHR = NULL;
static GLuint GLAD_API_PTR glad_debug_impl_glGetDebugMessageLogKHR(GLuint count, GLsizei bufSize, GLenum * sources, GLenum * types, GLuint * ids, GLenum * severities, GLsizei * lengths, GLchar * messageLog) {
    GLuint ret;
    _pre_call_gles2_callback("glGetDebugMessageLogKHR", (GLADapiproc) glad_glGetDebugMessageLogKHR, 8, count, bufSize, sources, types, ids, severities, lengths, messageLog);
    ret = glad_glGetDebugMessageLogKHR(count, bufSize, sources, types, ids, severities, lengths, messageLog);
    _post_call_gles2_callback((void*) &ret, "glGetDebugMessageLogKHR", (GLADapiproc) glad_glGetDebugMessageLogKHR, 8, count, bufSize, sources, types, ids, severities, lengths, messageLog);
    return ret;
}
PFNGLGETDEBUGMESSAGELOGKHRPROC glad_debug_glGetDebugMessageLogKHR = glad_debug_impl_glGetDebugMessageLogKHR;
PFNGLGETERRORPROC glad_glGetError = NULL;
static GLenum GLAD_API_PTR glad_debug_impl_glGetError(void) {
    GLenum ret;
    _pre_call_gles2_callback("glGetError", (GLADapiproc) glad_glGetError, 0);
    ret = glad_glGetError();
    _post_call_gles2_callback((void*) &ret, "glGetError", (GLADapiproc) glad_glGetError, 0);
    return ret;
}
PFNGLGETERRORPROC glad_debug_glGetError = glad_debug_impl_glGetError;
PFNGLGETFLOATVPROC glad_glGetFloatv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetFloatv(GLenum pname, GLfloat * data) {
    _pre_call_gles2_callback("glGetFloatv", (GLADapiproc) glad_glGetFloatv, 2, pname, data);
    glad_glGetFloatv(pname, data);
    _post_call_gles2_callback(NULL, "glGetFloatv", (GLADapiproc) glad_glGetFloatv, 2, pname, data);
    
}
PFNGLGETFLOATVPROC glad_debug_glGetFloatv = glad_debug_impl_glGetFloatv;
PFNGLGETFRAGDATALOCATIONPROC glad_glGetFragDataLocation = NULL;
static GLint GLAD_API_PTR glad_debug_impl_glGetFragDataLocation(GLuint program, const GLchar * name) {
    GLint ret;
    _pre_call_gles2_callback("glGetFragDataLocation", (GLADapiproc) glad_glGetFragDataLocation, 2, program, name);
    ret = glad_glGetFragDataLocation(program, name);
    _post_call_gles2_callback((void*) &ret, "glGetFragDataLocation", (GLADapiproc) glad_glGetFragDataLocation, 2, program, name);
    return ret;
}
PFNGLGETFRAGDATALOCATIONPROC glad_debug_glGetFragDataLocation = glad_debug_impl_glGetFragDataLocation;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glad_glGetFramebufferAttachmentParameteriv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetFramebufferAttachmentParameteriv", (GLADapiproc) glad_glGetFramebufferAttachmentParameteriv, 4, target, attachment, pname, params);
    glad_glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
    _post_call_gles2_callback(NULL, "glGetFramebufferAttachmentParameteriv", (GLADapiproc) glad_glGetFramebufferAttachmentParameteriv, 4, target, attachment, pname, params);
    
}
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glad_debug_glGetFramebufferAttachmentParameteriv = glad_debug_impl_glGetFramebufferAttachmentParameteriv;
PFNGLGETFRAMEBUFFERPARAMETERIVPROC glad_glGetFramebufferParameteriv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetFramebufferParameteriv(GLenum target, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetFramebufferParameteriv", (GLADapiproc) glad_glGetFramebufferParameteriv, 3, target, pname, params);
    glad_glGetFramebufferParameteriv(target, pname, params);
    _post_call_gles2_callback(NULL, "glGetFramebufferParameteriv", (GLADapiproc) glad_glGetFramebufferParameteriv, 3, target, pname, params);
    
}
PFNGLGETFRAMEBUFFERPARAMETERIVPROC glad_debug_glGetFramebufferParameteriv = glad_debug_impl_glGetFramebufferParameteriv;
PFNGLGETGRAPHICSRESETSTATUSPROC glad_glGetGraphicsResetStatus = NULL;
static GLenum GLAD_API_PTR glad_debug_impl_glGetGraphicsResetStatus(void) {
    GLenum ret;
    _pre_call_gles2_callback("glGetGraphicsResetStatus", (GLADapiproc) glad_glGetGraphicsResetStatus, 0);
    ret = glad_glGetGraphicsResetStatus();
    _post_call_gles2_callback((void*) &ret, "glGetGraphicsResetStatus", (GLADapiproc) glad_glGetGraphicsResetStatus, 0);
    return ret;
}
PFNGLGETGRAPHICSRESETSTATUSPROC glad_debug_glGetGraphicsResetStatus = glad_debug_impl_glGetGraphicsResetStatus;
PFNGLGETINTEGER64I_VPROC glad_glGetInteger64i_v = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetInteger64i_v(GLenum target, GLuint index, GLint64 * data) {
    _pre_call_gles2_callback("glGetInteger64i_v", (GLADapiproc) glad_glGetInteger64i_v, 3, target, index, data);
    glad_glGetInteger64i_v(target, index, data);
    _post_call_gles2_callback(NULL, "glGetInteger64i_v", (GLADapiproc) glad_glGetInteger64i_v, 3, target, index, data);
    
}
PFNGLGETINTEGER64I_VPROC glad_debug_glGetInteger64i_v = glad_debug_impl_glGetInteger64i_v;
PFNGLGETINTEGER64VPROC glad_glGetInteger64v = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetInteger64v(GLenum pname, GLint64 * data) {
    _pre_call_gles2_callback("glGetInteger64v", (GLADapiproc) glad_glGetInteger64v, 2, pname, data);
    glad_glGetInteger64v(pname, data);
    _post_call_gles2_callback(NULL, "glGetInteger64v", (GLADapiproc) glad_glGetInteger64v, 2, pname, data);
    
}
PFNGLGETINTEGER64VPROC glad_debug_glGetInteger64v = glad_debug_impl_glGetInteger64v;
PFNGLGETINTEGERI_VPROC glad_glGetIntegeri_v = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetIntegeri_v(GLenum target, GLuint index, GLint * data) {
    _pre_call_gles2_callback("glGetIntegeri_v", (GLADapiproc) glad_glGetIntegeri_v, 3, target, index, data);
    glad_glGetIntegeri_v(target, index, data);
    _post_call_gles2_callback(NULL, "glGetIntegeri_v", (GLADapiproc) glad_glGetIntegeri_v, 3, target, index, data);
    
}
PFNGLGETINTEGERI_VPROC glad_debug_glGetIntegeri_v = glad_debug_impl_glGetIntegeri_v;
PFNGLGETINTEGERVPROC glad_glGetIntegerv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetIntegerv(GLenum pname, GLint * data) {
    _pre_call_gles2_callback("glGetIntegerv", (GLADapiproc) glad_glGetIntegerv, 2, pname, data);
    glad_glGetIntegerv(pname, data);
    _post_call_gles2_callback(NULL, "glGetIntegerv", (GLADapiproc) glad_glGetIntegerv, 2, pname, data);
    
}
PFNGLGETINTEGERVPROC glad_debug_glGetIntegerv = glad_debug_impl_glGetIntegerv;
PFNGLGETINTERNALFORMATIVPROC glad_glGetInternalformativ = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint * params) {
    _pre_call_gles2_callback("glGetInternalformativ", (GLADapiproc) glad_glGetInternalformativ, 5, target, internalformat, pname, bufSize, params);
    glad_glGetInternalformativ(target, internalformat, pname, bufSize, params);
    _post_call_gles2_callback(NULL, "glGetInternalformativ", (GLADapiproc) glad_glGetInternalformativ, 5, target, internalformat, pname, bufSize, params);
    
}
PFNGLGETINTERNALFORMATIVPROC glad_debug_glGetInternalformativ = glad_debug_impl_glGetInternalformativ;
PFNGLGETMULTISAMPLEFVPROC glad_glGetMultisamplefv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetMultisamplefv(GLenum pname, GLuint index, GLfloat * val) {
    _pre_call_gles2_callback("glGetMultisamplefv", (GLADapiproc) glad_glGetMultisamplefv, 3, pname, index, val);
    glad_glGetMultisamplefv(pname, index, val);
    _post_call_gles2_callback(NULL, "glGetMultisamplefv", (GLADapiproc) glad_glGetMultisamplefv, 3, pname, index, val);
    
}
PFNGLGETMULTISAMPLEFVPROC glad_debug_glGetMultisamplefv = glad_debug_impl_glGetMultisamplefv;
PFNGLGETOBJECTLABELPROC glad_glGetObjectLabel = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetObjectLabel(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei * length, GLchar * label) {
    _pre_call_gles2_callback("glGetObjectLabel", (GLADapiproc) glad_glGetObjectLabel, 5, identifier, name, bufSize, length, label);
    glad_glGetObjectLabel(identifier, name, bufSize, length, label);
    _post_call_gles2_callback(NULL, "glGetObjectLabel", (GLADapiproc) glad_glGetObjectLabel, 5, identifier, name, bufSize, length, label);
    
}
PFNGLGETOBJECTLABELPROC glad_debug_glGetObjectLabel = glad_debug_impl_glGetObjectLabel;
PFNGLGETOBJECTLABELKHRPROC glad_glGetObjectLabelKHR = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetObjectLabelKHR(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei * length, GLchar * label) {
    _pre_call_gles2_callback("glGetObjectLabelKHR", (GLADapiproc) glad_glGetObjectLabelKHR, 5, identifier, name, bufSize, length, label);
    glad_glGetObjectLabelKHR(identifier, name, bufSize, length, label);
    _post_call_gles2_callback(NULL, "glGetObjectLabelKHR", (GLADapiproc) glad_glGetObjectLabelKHR, 5, identifier, name, bufSize, length, label);
    
}
PFNGLGETOBJECTLABELKHRPROC glad_debug_glGetObjectLabelKHR = glad_debug_impl_glGetObjectLabelKHR;
PFNGLGETOBJECTPTRLABELPROC glad_glGetObjectPtrLabel = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetObjectPtrLabel(const void * ptr, GLsizei bufSize, GLsizei * length, GLchar * label) {
    _pre_call_gles2_callback("glGetObjectPtrLabel", (GLADapiproc) glad_glGetObjectPtrLabel, 4, ptr, bufSize, length, label);
    glad_glGetObjectPtrLabel(ptr, bufSize, length, label);
    _post_call_gles2_callback(NULL, "glGetObjectPtrLabel", (GLADapiproc) glad_glGetObjectPtrLabel, 4, ptr, bufSize, length, label);
    
}
PFNGLGETOBJECTPTRLABELPROC glad_debug_glGetObjectPtrLabel = glad_debug_impl_glGetObjectPtrLabel;
PFNGLGETOBJECTPTRLABELKHRPROC glad_glGetObjectPtrLabelKHR = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetObjectPtrLabelKHR(const void * ptr, GLsizei bufSize, GLsizei * length, GLchar * label) {
    _pre_call_gles2_callback("glGetObjectPtrLabelKHR", (GLADapiproc) glad_glGetObjectPtrLabelKHR, 4, ptr, bufSize, length, label);
    glad_glGetObjectPtrLabelKHR(ptr, bufSize, length, label);
    _post_call_gles2_callback(NULL, "glGetObjectPtrLabelKHR", (GLADapiproc) glad_glGetObjectPtrLabelKHR, 4, ptr, bufSize, length, label);
    
}
PFNGLGETOBJECTPTRLABELKHRPROC glad_debug_glGetObjectPtrLabelKHR = glad_debug_impl_glGetObjectPtrLabelKHR;
PFNGLGETPOINTERVPROC glad_glGetPointerv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetPointerv(GLenum pname, void ** params) {
    _pre_call_gles2_callback("glGetPointerv", (GLADapiproc) glad_glGetPointerv, 2, pname, params);
    glad_glGetPointerv(pname, params);
    _post_call_gles2_callback(NULL, "glGetPointerv", (GLADapiproc) glad_glGetPointerv, 2, pname, params);
    
}
PFNGLGETPOINTERVPROC glad_debug_glGetPointerv = glad_debug_impl_glGetPointerv;
PFNGLGETPOINTERVKHRPROC glad_glGetPointervKHR = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetPointervKHR(GLenum pname, void ** params) {
    _pre_call_gles2_callback("glGetPointervKHR", (GLADapiproc) glad_glGetPointervKHR, 2, pname, params);
    glad_glGetPointervKHR(pname, params);
    _post_call_gles2_callback(NULL, "glGetPointervKHR", (GLADapiproc) glad_glGetPointervKHR, 2, pname, params);
    
}
PFNGLGETPOINTERVKHRPROC glad_debug_glGetPointervKHR = glad_debug_impl_glGetPointervKHR;
PFNGLGETPROGRAMBINARYPROC glad_glGetProgramBinary = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetProgramBinary(GLuint program, GLsizei bufSize, GLsizei * length, GLenum * binaryFormat, void * binary) {
    _pre_call_gles2_callback("glGetProgramBinary", (GLADapiproc) glad_glGetProgramBinary, 5, program, bufSize, length, binaryFormat, binary);
    glad_glGetProgramBinary(program, bufSize, length, binaryFormat, binary);
    _post_call_gles2_callback(NULL, "glGetProgramBinary", (GLADapiproc) glad_glGetProgramBinary, 5, program, bufSize, length, binaryFormat, binary);
    
}
PFNGLGETPROGRAMBINARYPROC glad_debug_glGetProgramBinary = glad_debug_impl_glGetProgramBinary;
PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei * length, GLchar * infoLog) {
    _pre_call_gles2_callback("glGetProgramInfoLog", (GLADapiproc) glad_glGetProgramInfoLog, 4, program, bufSize, length, infoLog);
    glad_glGetProgramInfoLog(program, bufSize, length, infoLog);
    _post_call_gles2_callback(NULL, "glGetProgramInfoLog", (GLADapiproc) glad_glGetProgramInfoLog, 4, program, bufSize, length, infoLog);
    
}
PFNGLGETPROGRAMINFOLOGPROC glad_debug_glGetProgramInfoLog = glad_debug_impl_glGetProgramInfoLog;
PFNGLGETPROGRAMINTERFACEIVPROC glad_glGetProgramInterfaceiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetProgramInterfaceiv(GLuint program, GLenum programInterface, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetProgramInterfaceiv", (GLADapiproc) glad_glGetProgramInterfaceiv, 4, program, programInterface, pname, params);
    glad_glGetProgramInterfaceiv(program, programInterface, pname, params);
    _post_call_gles2_callback(NULL, "glGetProgramInterfaceiv", (GLADapiproc) glad_glGetProgramInterfaceiv, 4, program, programInterface, pname, params);
    
}
PFNGLGETPROGRAMINTERFACEIVPROC glad_debug_glGetProgramInterfaceiv = glad_debug_impl_glGetProgramInterfaceiv;
PFNGLGETPROGRAMPIPELINEINFOLOGPROC glad_glGetProgramPipelineInfoLog = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetProgramPipelineInfoLog(GLuint pipeline, GLsizei bufSize, GLsizei * length, GLchar * infoLog) {
    _pre_call_gles2_callback("glGetProgramPipelineInfoLog", (GLADapiproc) glad_glGetProgramPipelineInfoLog, 4, pipeline, bufSize, length, infoLog);
    glad_glGetProgramPipelineInfoLog(pipeline, bufSize, length, infoLog);
    _post_call_gles2_callback(NULL, "glGetProgramPipelineInfoLog", (GLADapiproc) glad_glGetProgramPipelineInfoLog, 4, pipeline, bufSize, length, infoLog);
    
}
PFNGLGETPROGRAMPIPELINEINFOLOGPROC glad_debug_glGetProgramPipelineInfoLog = glad_debug_impl_glGetProgramPipelineInfoLog;
PFNGLGETPROGRAMPIPELINEIVPROC glad_glGetProgramPipelineiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetProgramPipelineiv(GLuint pipeline, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetProgramPipelineiv", (GLADapiproc) glad_glGetProgramPipelineiv, 3, pipeline, pname, params);
    glad_glGetProgramPipelineiv(pipeline, pname, params);
    _post_call_gles2_callback(NULL, "glGetProgramPipelineiv", (GLADapiproc) glad_glGetProgramPipelineiv, 3, pipeline, pname, params);
    
}
PFNGLGETPROGRAMPIPELINEIVPROC glad_debug_glGetProgramPipelineiv = glad_debug_impl_glGetProgramPipelineiv;
PFNGLGETPROGRAMRESOURCEINDEXPROC glad_glGetProgramResourceIndex = NULL;
static GLuint GLAD_API_PTR glad_debug_impl_glGetProgramResourceIndex(GLuint program, GLenum programInterface, const GLchar * name) {
    GLuint ret;
    _pre_call_gles2_callback("glGetProgramResourceIndex", (GLADapiproc) glad_glGetProgramResourceIndex, 3, program, programInterface, name);
    ret = glad_glGetProgramResourceIndex(program, programInterface, name);
    _post_call_gles2_callback((void*) &ret, "glGetProgramResourceIndex", (GLADapiproc) glad_glGetProgramResourceIndex, 3, program, programInterface, name);
    return ret;
}
PFNGLGETPROGRAMRESOURCEINDEXPROC glad_debug_glGetProgramResourceIndex = glad_debug_impl_glGetProgramResourceIndex;
PFNGLGETPROGRAMRESOURCELOCATIONPROC glad_glGetProgramResourceLocation = NULL;
static GLint GLAD_API_PTR glad_debug_impl_glGetProgramResourceLocation(GLuint program, GLenum programInterface, const GLchar * name) {
    GLint ret;
    _pre_call_gles2_callback("glGetProgramResourceLocation", (GLADapiproc) glad_glGetProgramResourceLocation, 3, program, programInterface, name);
    ret = glad_glGetProgramResourceLocation(program, programInterface, name);
    _post_call_gles2_callback((void*) &ret, "glGetProgramResourceLocation", (GLADapiproc) glad_glGetProgramResourceLocation, 3, program, programInterface, name);
    return ret;
}
PFNGLGETPROGRAMRESOURCELOCATIONPROC glad_debug_glGetProgramResourceLocation = glad_debug_impl_glGetProgramResourceLocation;
PFNGLGETPROGRAMRESOURCENAMEPROC glad_glGetProgramResourceName = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetProgramResourceName(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei * length, GLchar * name) {
    _pre_call_gles2_callback("glGetProgramResourceName", (GLADapiproc) glad_glGetProgramResourceName, 6, program, programInterface, index, bufSize, length, name);
    glad_glGetProgramResourceName(program, programInterface, index, bufSize, length, name);
    _post_call_gles2_callback(NULL, "glGetProgramResourceName", (GLADapiproc) glad_glGetProgramResourceName, 6, program, programInterface, index, bufSize, length, name);
    
}
PFNGLGETPROGRAMRESOURCENAMEPROC glad_debug_glGetProgramResourceName = glad_debug_impl_glGetProgramResourceName;
PFNGLGETPROGRAMRESOURCEIVPROC glad_glGetProgramResourceiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetProgramResourceiv(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum * props, GLsizei bufSize, GLsizei * length, GLint * params) {
    _pre_call_gles2_callback("glGetProgramResourceiv", (GLADapiproc) glad_glGetProgramResourceiv, 8, program, programInterface, index, propCount, props, bufSize, length, params);
    glad_glGetProgramResourceiv(program, programInterface, index, propCount, props, bufSize, length, params);
    _post_call_gles2_callback(NULL, "glGetProgramResourceiv", (GLADapiproc) glad_glGetProgramResourceiv, 8, program, programInterface, index, propCount, props, bufSize, length, params);
    
}
PFNGLGETPROGRAMRESOURCEIVPROC glad_debug_glGetProgramResourceiv = glad_debug_impl_glGetProgramResourceiv;
PFNGLGETPROGRAMIVPROC glad_glGetProgramiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetProgramiv(GLuint program, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetProgramiv", (GLADapiproc) glad_glGetProgramiv, 3, program, pname, params);
    glad_glGetProgramiv(program, pname, params);
    _post_call_gles2_callback(NULL, "glGetProgramiv", (GLADapiproc) glad_glGetProgramiv, 3, program, pname, params);
    
}
PFNGLGETPROGRAMIVPROC glad_debug_glGetProgramiv = glad_debug_impl_glGetProgramiv;
PFNGLGETQUERYOBJECTUIVPROC glad_glGetQueryObjectuiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint * params) {
    _pre_call_gles2_callback("glGetQueryObjectuiv", (GLADapiproc) glad_glGetQueryObjectuiv, 3, id, pname, params);
    glad_glGetQueryObjectuiv(id, pname, params);
    _post_call_gles2_callback(NULL, "glGetQueryObjectuiv", (GLADapiproc) glad_glGetQueryObjectuiv, 3, id, pname, params);
    
}
PFNGLGETQUERYOBJECTUIVPROC glad_debug_glGetQueryObjectuiv = glad_debug_impl_glGetQueryObjectuiv;
PFNGLGETQUERYIVPROC glad_glGetQueryiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetQueryiv(GLenum target, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetQueryiv", (GLADapiproc) glad_glGetQueryiv, 3, target, pname, params);
    glad_glGetQueryiv(target, pname, params);
    _post_call_gles2_callback(NULL, "glGetQueryiv", (GLADapiproc) glad_glGetQueryiv, 3, target, pname, params);
    
}
PFNGLGETQUERYIVPROC glad_debug_glGetQueryiv = glad_debug_impl_glGetQueryiv;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glad_glGetRenderbufferParameteriv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetRenderbufferParameteriv", (GLADapiproc) glad_glGetRenderbufferParameteriv, 3, target, pname, params);
    glad_glGetRenderbufferParameteriv(target, pname, params);
    _post_call_gles2_callback(NULL, "glGetRenderbufferParameteriv", (GLADapiproc) glad_glGetRenderbufferParameteriv, 3, target, pname, params);
    
}
PFNGLGETRENDERBUFFERPARAMETERIVPROC glad_debug_glGetRenderbufferParameteriv = glad_debug_impl_glGetRenderbufferParameteriv;
PFNGLGETSAMPLERPARAMETERIIVPROC glad_glGetSamplerParameterIiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetSamplerParameterIiv(GLuint sampler, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetSamplerParameterIiv", (GLADapiproc) glad_glGetSamplerParameterIiv, 3, sampler, pname, params);
    glad_glGetSamplerParameterIiv(sampler, pname, params);
    _post_call_gles2_callback(NULL, "glGetSamplerParameterIiv", (GLADapiproc) glad_glGetSamplerParameterIiv, 3, sampler, pname, params);
    
}
PFNGLGETSAMPLERPARAMETERIIVPROC glad_debug_glGetSamplerParameterIiv = glad_debug_impl_glGetSamplerParameterIiv;
PFNGLGETSAMPLERPARAMETERIUIVPROC glad_glGetSamplerParameterIuiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetSamplerParameterIuiv(GLuint sampler, GLenum pname, GLuint * params) {
    _pre_call_gles2_callback("glGetSamplerParameterIuiv", (GLADapiproc) glad_glGetSamplerParameterIuiv, 3, sampler, pname, params);
    glad_glGetSamplerParameterIuiv(sampler, pname, params);
    _post_call_gles2_callback(NULL, "glGetSamplerParameterIuiv", (GLADapiproc) glad_glGetSamplerParameterIuiv, 3, sampler, pname, params);
    
}
PFNGLGETSAMPLERPARAMETERIUIVPROC glad_debug_glGetSamplerParameterIuiv = glad_debug_impl_glGetSamplerParameterIuiv;
PFNGLGETSAMPLERPARAMETERFVPROC glad_glGetSamplerParameterfv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat * params) {
    _pre_call_gles2_callback("glGetSamplerParameterfv", (GLADapiproc) glad_glGetSamplerParameterfv, 3, sampler, pname, params);
    glad_glGetSamplerParameterfv(sampler, pname, params);
    _post_call_gles2_callback(NULL, "glGetSamplerParameterfv", (GLADapiproc) glad_glGetSamplerParameterfv, 3, sampler, pname, params);
    
}
PFNGLGETSAMPLERPARAMETERFVPROC glad_debug_glGetSamplerParameterfv = glad_debug_impl_glGetSamplerParameterfv;
PFNGLGETSAMPLERPARAMETERIVPROC glad_glGetSamplerParameteriv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetSamplerParameteriv(GLuint sampler, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetSamplerParameteriv", (GLADapiproc) glad_glGetSamplerParameteriv, 3, sampler, pname, params);
    glad_glGetSamplerParameteriv(sampler, pname, params);
    _post_call_gles2_callback(NULL, "glGetSamplerParameteriv", (GLADapiproc) glad_glGetSamplerParameteriv, 3, sampler, pname, params);
    
}
PFNGLGETSAMPLERPARAMETERIVPROC glad_debug_glGetSamplerParameteriv = glad_debug_impl_glGetSamplerParameteriv;
PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog) {
    _pre_call_gles2_callback("glGetShaderInfoLog", (GLADapiproc) glad_glGetShaderInfoLog, 4, shader, bufSize, length, infoLog);
    glad_glGetShaderInfoLog(shader, bufSize, length, infoLog);
    _post_call_gles2_callback(NULL, "glGetShaderInfoLog", (GLADapiproc) glad_glGetShaderInfoLog, 4, shader, bufSize, length, infoLog);
    
}
PFNGLGETSHADERINFOLOGPROC glad_debug_glGetShaderInfoLog = glad_debug_impl_glGetShaderInfoLog;
PFNGLGETSHADERPRECISIONFORMATPROC glad_glGetShaderPrecisionFormat = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint * range, GLint * precision) {
    _pre_call_gles2_callback("glGetShaderPrecisionFormat", (GLADapiproc) glad_glGetShaderPrecisionFormat, 4, shadertype, precisiontype, range, precision);
    glad_glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
    _post_call_gles2_callback(NULL, "glGetShaderPrecisionFormat", (GLADapiproc) glad_glGetShaderPrecisionFormat, 4, shadertype, precisiontype, range, precision);
    
}
PFNGLGETSHADERPRECISIONFORMATPROC glad_debug_glGetShaderPrecisionFormat = glad_debug_impl_glGetShaderPrecisionFormat;
PFNGLGETSHADERSOURCEPROC glad_glGetShaderSource = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetShaderSource(GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * source) {
    _pre_call_gles2_callback("glGetShaderSource", (GLADapiproc) glad_glGetShaderSource, 4, shader, bufSize, length, source);
    glad_glGetShaderSource(shader, bufSize, length, source);
    _post_call_gles2_callback(NULL, "glGetShaderSource", (GLADapiproc) glad_glGetShaderSource, 4, shader, bufSize, length, source);
    
}
PFNGLGETSHADERSOURCEPROC glad_debug_glGetShaderSource = glad_debug_impl_glGetShaderSource;
PFNGLGETSHADERIVPROC glad_glGetShaderiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetShaderiv(GLuint shader, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetShaderiv", (GLADapiproc) glad_glGetShaderiv, 3, shader, pname, params);
    glad_glGetShaderiv(shader, pname, params);
    _post_call_gles2_callback(NULL, "glGetShaderiv", (GLADapiproc) glad_glGetShaderiv, 3, shader, pname, params);
    
}
PFNGLGETSHADERIVPROC glad_debug_glGetShaderiv = glad_debug_impl_glGetShaderiv;
PFNGLGETSTRINGPROC glad_glGetString = NULL;
static const GLubyte * GLAD_API_PTR glad_debug_impl_glGetString(GLenum name) {
    const GLubyte * ret;
    _pre_call_gles2_callback("glGetString", (GLADapiproc) glad_glGetString, 1, name);
    ret = glad_glGetString(name);
    _post_call_gles2_callback((void*) &ret, "glGetString", (GLADapiproc) glad_glGetString, 1, name);
    return ret;
}
PFNGLGETSTRINGPROC glad_debug_glGetString = glad_debug_impl_glGetString;
PFNGLGETSTRINGIPROC glad_glGetStringi = NULL;
static const GLubyte * GLAD_API_PTR glad_debug_impl_glGetStringi(GLenum name, GLuint index) {
    const GLubyte * ret;
    _pre_call_gles2_callback("glGetStringi", (GLADapiproc) glad_glGetStringi, 2, name, index);
    ret = glad_glGetStringi(name, index);
    _post_call_gles2_callback((void*) &ret, "glGetStringi", (GLADapiproc) glad_glGetStringi, 2, name, index);
    return ret;
}
PFNGLGETSTRINGIPROC glad_debug_glGetStringi = glad_debug_impl_glGetStringi;
PFNGLGETSYNCIVPROC glad_glGetSynciv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei * length, GLint * values) {
    _pre_call_gles2_callback("glGetSynciv", (GLADapiproc) glad_glGetSynciv, 5, sync, pname, bufSize, length, values);
    glad_glGetSynciv(sync, pname, bufSize, length, values);
    _post_call_gles2_callback(NULL, "glGetSynciv", (GLADapiproc) glad_glGetSynciv, 5, sync, pname, bufSize, length, values);
    
}
PFNGLGETSYNCIVPROC glad_debug_glGetSynciv = glad_debug_impl_glGetSynciv;
PFNGLGETTEXLEVELPARAMETERFVPROC glad_glGetTexLevelParameterfv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat * params) {
    _pre_call_gles2_callback("glGetTexLevelParameterfv", (GLADapiproc) glad_glGetTexLevelParameterfv, 4, target, level, pname, params);
    glad_glGetTexLevelParameterfv(target, level, pname, params);
    _post_call_gles2_callback(NULL, "glGetTexLevelParameterfv", (GLADapiproc) glad_glGetTexLevelParameterfv, 4, target, level, pname, params);
    
}
PFNGLGETTEXLEVELPARAMETERFVPROC glad_debug_glGetTexLevelParameterfv = glad_debug_impl_glGetTexLevelParameterfv;
PFNGLGETTEXLEVELPARAMETERIVPROC glad_glGetTexLevelParameteriv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetTexLevelParameteriv", (GLADapiproc) glad_glGetTexLevelParameteriv, 4, target, level, pname, params);
    glad_glGetTexLevelParameteriv(target, level, pname, params);
    _post_call_gles2_callback(NULL, "glGetTexLevelParameteriv", (GLADapiproc) glad_glGetTexLevelParameteriv, 4, target, level, pname, params);
    
}
PFNGLGETTEXLEVELPARAMETERIVPROC glad_debug_glGetTexLevelParameteriv = glad_debug_impl_glGetTexLevelParameteriv;
PFNGLGETTEXPARAMETERIIVPROC glad_glGetTexParameterIiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetTexParameterIiv(GLenum target, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetTexParameterIiv", (GLADapiproc) glad_glGetTexParameterIiv, 3, target, pname, params);
    glad_glGetTexParameterIiv(target, pname, params);
    _post_call_gles2_callback(NULL, "glGetTexParameterIiv", (GLADapiproc) glad_glGetTexParameterIiv, 3, target, pname, params);
    
}
PFNGLGETTEXPARAMETERIIVPROC glad_debug_glGetTexParameterIiv = glad_debug_impl_glGetTexParameterIiv;
PFNGLGETTEXPARAMETERIUIVPROC glad_glGetTexParameterIuiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetTexParameterIuiv(GLenum target, GLenum pname, GLuint * params) {
    _pre_call_gles2_callback("glGetTexParameterIuiv", (GLADapiproc) glad_glGetTexParameterIuiv, 3, target, pname, params);
    glad_glGetTexParameterIuiv(target, pname, params);
    _post_call_gles2_callback(NULL, "glGetTexParameterIuiv", (GLADapiproc) glad_glGetTexParameterIuiv, 3, target, pname, params);
    
}
PFNGLGETTEXPARAMETERIUIVPROC glad_debug_glGetTexParameterIuiv = glad_debug_impl_glGetTexParameterIuiv;
PFNGLGETTEXPARAMETERFVPROC glad_glGetTexParameterfv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetTexParameterfv(GLenum target, GLenum pname, GLfloat * params) {
    _pre_call_gles2_callback("glGetTexParameterfv", (GLADapiproc) glad_glGetTexParameterfv, 3, target, pname, params);
    glad_glGetTexParameterfv(target, pname, params);
    _post_call_gles2_callback(NULL, "glGetTexParameterfv", (GLADapiproc) glad_glGetTexParameterfv, 3, target, pname, params);
    
}
PFNGLGETTEXPARAMETERFVPROC glad_debug_glGetTexParameterfv = glad_debug_impl_glGetTexParameterfv;
PFNGLGETTEXPARAMETERIVPROC glad_glGetTexParameteriv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetTexParameteriv(GLenum target, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetTexParameteriv", (GLADapiproc) glad_glGetTexParameteriv, 3, target, pname, params);
    glad_glGetTexParameteriv(target, pname, params);
    _post_call_gles2_callback(NULL, "glGetTexParameteriv", (GLADapiproc) glad_glGetTexParameteriv, 3, target, pname, params);
    
}
PFNGLGETTEXPARAMETERIVPROC glad_debug_glGetTexParameteriv = glad_debug_impl_glGetTexParameteriv;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glad_glGetTransformFeedbackVarying = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLsizei * size, GLenum * type, GLchar * name) {
    _pre_call_gles2_callback("glGetTransformFeedbackVarying", (GLADapiproc) glad_glGetTransformFeedbackVarying, 7, program, index, bufSize, length, size, type, name);
    glad_glGetTransformFeedbackVarying(program, index, bufSize, length, size, type, name);
    _post_call_gles2_callback(NULL, "glGetTransformFeedbackVarying", (GLADapiproc) glad_glGetTransformFeedbackVarying, 7, program, index, bufSize, length, size, type, name);
    
}
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glad_debug_glGetTransformFeedbackVarying = glad_debug_impl_glGetTransformFeedbackVarying;
PFNGLGETUNIFORMBLOCKINDEXPROC glad_glGetUniformBlockIndex = NULL;
static GLuint GLAD_API_PTR glad_debug_impl_glGetUniformBlockIndex(GLuint program, const GLchar * uniformBlockName) {
    GLuint ret;
    _pre_call_gles2_callback("glGetUniformBlockIndex", (GLADapiproc) glad_glGetUniformBlockIndex, 2, program, uniformBlockName);
    ret = glad_glGetUniformBlockIndex(program, uniformBlockName);
    _post_call_gles2_callback((void*) &ret, "glGetUniformBlockIndex", (GLADapiproc) glad_glGetUniformBlockIndex, 2, program, uniformBlockName);
    return ret;
}
PFNGLGETUNIFORMBLOCKINDEXPROC glad_debug_glGetUniformBlockIndex = glad_debug_impl_glGetUniformBlockIndex;
PFNGLGETUNIFORMINDICESPROC glad_glGetUniformIndices = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar *const* uniformNames, GLuint * uniformIndices) {
    _pre_call_gles2_callback("glGetUniformIndices", (GLADapiproc) glad_glGetUniformIndices, 4, program, uniformCount, uniformNames, uniformIndices);
    glad_glGetUniformIndices(program, uniformCount, uniformNames, uniformIndices);
    _post_call_gles2_callback(NULL, "glGetUniformIndices", (GLADapiproc) glad_glGetUniformIndices, 4, program, uniformCount, uniformNames, uniformIndices);
    
}
PFNGLGETUNIFORMINDICESPROC glad_debug_glGetUniformIndices = glad_debug_impl_glGetUniformIndices;
PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation = NULL;
static GLint GLAD_API_PTR glad_debug_impl_glGetUniformLocation(GLuint program, const GLchar * name) {
    GLint ret;
    _pre_call_gles2_callback("glGetUniformLocation", (GLADapiproc) glad_glGetUniformLocation, 2, program, name);
    ret = glad_glGetUniformLocation(program, name);
    _post_call_gles2_callback((void*) &ret, "glGetUniformLocation", (GLADapiproc) glad_glGetUniformLocation, 2, program, name);
    return ret;
}
PFNGLGETUNIFORMLOCATIONPROC glad_debug_glGetUniformLocation = glad_debug_impl_glGetUniformLocation;
PFNGLGETUNIFORMFVPROC glad_glGetUniformfv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetUniformfv(GLuint program, GLint location, GLfloat * params) {
    _pre_call_gles2_callback("glGetUniformfv", (GLADapiproc) glad_glGetUniformfv, 3, program, location, params);
    glad_glGetUniformfv(program, location, params);
    _post_call_gles2_callback(NULL, "glGetUniformfv", (GLADapiproc) glad_glGetUniformfv, 3, program, location, params);
    
}
PFNGLGETUNIFORMFVPROC glad_debug_glGetUniformfv = glad_debug_impl_glGetUniformfv;
PFNGLGETUNIFORMIVPROC glad_glGetUniformiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetUniformiv(GLuint program, GLint location, GLint * params) {
    _pre_call_gles2_callback("glGetUniformiv", (GLADapiproc) glad_glGetUniformiv, 3, program, location, params);
    glad_glGetUniformiv(program, location, params);
    _post_call_gles2_callback(NULL, "glGetUniformiv", (GLADapiproc) glad_glGetUniformiv, 3, program, location, params);
    
}
PFNGLGETUNIFORMIVPROC glad_debug_glGetUniformiv = glad_debug_impl_glGetUniformiv;
PFNGLGETUNIFORMUIVPROC glad_glGetUniformuiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetUniformuiv(GLuint program, GLint location, GLuint * params) {
    _pre_call_gles2_callback("glGetUniformuiv", (GLADapiproc) glad_glGetUniformuiv, 3, program, location, params);
    glad_glGetUniformuiv(program, location, params);
    _post_call_gles2_callback(NULL, "glGetUniformuiv", (GLADapiproc) glad_glGetUniformuiv, 3, program, location, params);
    
}
PFNGLGETUNIFORMUIVPROC glad_debug_glGetUniformuiv = glad_debug_impl_glGetUniformuiv;
PFNGLGETVERTEXATTRIBIIVPROC glad_glGetVertexAttribIiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetVertexAttribIiv(GLuint index, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetVertexAttribIiv", (GLADapiproc) glad_glGetVertexAttribIiv, 3, index, pname, params);
    glad_glGetVertexAttribIiv(index, pname, params);
    _post_call_gles2_callback(NULL, "glGetVertexAttribIiv", (GLADapiproc) glad_glGetVertexAttribIiv, 3, index, pname, params);
    
}
PFNGLGETVERTEXATTRIBIIVPROC glad_debug_glGetVertexAttribIiv = glad_debug_impl_glGetVertexAttribIiv;
PFNGLGETVERTEXATTRIBIUIVPROC glad_glGetVertexAttribIuiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetVertexAttribIuiv(GLuint index, GLenum pname, GLuint * params) {
    _pre_call_gles2_callback("glGetVertexAttribIuiv", (GLADapiproc) glad_glGetVertexAttribIuiv, 3, index, pname, params);
    glad_glGetVertexAttribIuiv(index, pname, params);
    _post_call_gles2_callback(NULL, "glGetVertexAttribIuiv", (GLADapiproc) glad_glGetVertexAttribIuiv, 3, index, pname, params);
    
}
PFNGLGETVERTEXATTRIBIUIVPROC glad_debug_glGetVertexAttribIuiv = glad_debug_impl_glGetVertexAttribIuiv;
PFNGLGETVERTEXATTRIBPOINTERVPROC glad_glGetVertexAttribPointerv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetVertexAttribPointerv(GLuint index, GLenum pname, void ** pointer) {
    _pre_call_gles2_callback("glGetVertexAttribPointerv", (GLADapiproc) glad_glGetVertexAttribPointerv, 3, index, pname, pointer);
    glad_glGetVertexAttribPointerv(index, pname, pointer);
    _post_call_gles2_callback(NULL, "glGetVertexAttribPointerv", (GLADapiproc) glad_glGetVertexAttribPointerv, 3, index, pname, pointer);
    
}
PFNGLGETVERTEXATTRIBPOINTERVPROC glad_debug_glGetVertexAttribPointerv = glad_debug_impl_glGetVertexAttribPointerv;
PFNGLGETVERTEXATTRIBFVPROC glad_glGetVertexAttribfv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat * params) {
    _pre_call_gles2_callback("glGetVertexAttribfv", (GLADapiproc) glad_glGetVertexAttribfv, 3, index, pname, params);
    glad_glGetVertexAttribfv(index, pname, params);
    _post_call_gles2_callback(NULL, "glGetVertexAttribfv", (GLADapiproc) glad_glGetVertexAttribfv, 3, index, pname, params);
    
}
PFNGLGETVERTEXATTRIBFVPROC glad_debug_glGetVertexAttribfv = glad_debug_impl_glGetVertexAttribfv;
PFNGLGETVERTEXATTRIBIVPROC glad_glGetVertexAttribiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetVertexAttribiv(GLuint index, GLenum pname, GLint * params) {
    _pre_call_gles2_callback("glGetVertexAttribiv", (GLADapiproc) glad_glGetVertexAttribiv, 3, index, pname, params);
    glad_glGetVertexAttribiv(index, pname, params);
    _post_call_gles2_callback(NULL, "glGetVertexAttribiv", (GLADapiproc) glad_glGetVertexAttribiv, 3, index, pname, params);
    
}
PFNGLGETVERTEXATTRIBIVPROC glad_debug_glGetVertexAttribiv = glad_debug_impl_glGetVertexAttribiv;
PFNGLGETNUNIFORMFVPROC glad_glGetnUniformfv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetnUniformfv(GLuint program, GLint location, GLsizei bufSize, GLfloat * params) {
    _pre_call_gles2_callback("glGetnUniformfv", (GLADapiproc) glad_glGetnUniformfv, 4, program, location, bufSize, params);
    glad_glGetnUniformfv(program, location, bufSize, params);
    _post_call_gles2_callback(NULL, "glGetnUniformfv", (GLADapiproc) glad_glGetnUniformfv, 4, program, location, bufSize, params);
    
}
PFNGLGETNUNIFORMFVPROC glad_debug_glGetnUniformfv = glad_debug_impl_glGetnUniformfv;
PFNGLGETNUNIFORMIVPROC glad_glGetnUniformiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetnUniformiv(GLuint program, GLint location, GLsizei bufSize, GLint * params) {
    _pre_call_gles2_callback("glGetnUniformiv", (GLADapiproc) glad_glGetnUniformiv, 4, program, location, bufSize, params);
    glad_glGetnUniformiv(program, location, bufSize, params);
    _post_call_gles2_callback(NULL, "glGetnUniformiv", (GLADapiproc) glad_glGetnUniformiv, 4, program, location, bufSize, params);
    
}
PFNGLGETNUNIFORMIVPROC glad_debug_glGetnUniformiv = glad_debug_impl_glGetnUniformiv;
PFNGLGETNUNIFORMUIVPROC glad_glGetnUniformuiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glGetnUniformuiv(GLuint program, GLint location, GLsizei bufSize, GLuint * params) {
    _pre_call_gles2_callback("glGetnUniformuiv", (GLADapiproc) glad_glGetnUniformuiv, 4, program, location, bufSize, params);
    glad_glGetnUniformuiv(program, location, bufSize, params);
    _post_call_gles2_callback(NULL, "glGetnUniformuiv", (GLADapiproc) glad_glGetnUniformuiv, 4, program, location, bufSize, params);
    
}
PFNGLGETNUNIFORMUIVPROC glad_debug_glGetnUniformuiv = glad_debug_impl_glGetnUniformuiv;
PFNGLHINTPROC glad_glHint = NULL;
static void GLAD_API_PTR glad_debug_impl_glHint(GLenum target, GLenum mode) {
    _pre_call_gles2_callback("glHint", (GLADapiproc) glad_glHint, 2, target, mode);
    glad_glHint(target, mode);
    _post_call_gles2_callback(NULL, "glHint", (GLADapiproc) glad_glHint, 2, target, mode);
    
}
PFNGLHINTPROC glad_debug_glHint = glad_debug_impl_glHint;
PFNGLINVALIDATEFRAMEBUFFERPROC glad_glInvalidateFramebuffer = NULL;
static void GLAD_API_PTR glad_debug_impl_glInvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum * attachments) {
    _pre_call_gles2_callback("glInvalidateFramebuffer", (GLADapiproc) glad_glInvalidateFramebuffer, 3, target, numAttachments, attachments);
    glad_glInvalidateFramebuffer(target, numAttachments, attachments);
    _post_call_gles2_callback(NULL, "glInvalidateFramebuffer", (GLADapiproc) glad_glInvalidateFramebuffer, 3, target, numAttachments, attachments);
    
}
PFNGLINVALIDATEFRAMEBUFFERPROC glad_debug_glInvalidateFramebuffer = glad_debug_impl_glInvalidateFramebuffer;
PFNGLINVALIDATESUBFRAMEBUFFERPROC glad_glInvalidateSubFramebuffer = NULL;
static void GLAD_API_PTR glad_debug_impl_glInvalidateSubFramebuffer(GLenum target, GLsizei numAttachments, const GLenum * attachments, GLint x, GLint y, GLsizei width, GLsizei height) {
    _pre_call_gles2_callback("glInvalidateSubFramebuffer", (GLADapiproc) glad_glInvalidateSubFramebuffer, 7, target, numAttachments, attachments, x, y, width, height);
    glad_glInvalidateSubFramebuffer(target, numAttachments, attachments, x, y, width, height);
    _post_call_gles2_callback(NULL, "glInvalidateSubFramebuffer", (GLADapiproc) glad_glInvalidateSubFramebuffer, 7, target, numAttachments, attachments, x, y, width, height);
    
}
PFNGLINVALIDATESUBFRAMEBUFFERPROC glad_debug_glInvalidateSubFramebuffer = glad_debug_impl_glInvalidateSubFramebuffer;
PFNGLISBUFFERPROC glad_glIsBuffer = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glIsBuffer(GLuint buffer) {
    GLboolean ret;
    _pre_call_gles2_callback("glIsBuffer", (GLADapiproc) glad_glIsBuffer, 1, buffer);
    ret = glad_glIsBuffer(buffer);
    _post_call_gles2_callback((void*) &ret, "glIsBuffer", (GLADapiproc) glad_glIsBuffer, 1, buffer);
    return ret;
}
PFNGLISBUFFERPROC glad_debug_glIsBuffer = glad_debug_impl_glIsBuffer;
PFNGLISENABLEDPROC glad_glIsEnabled = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glIsEnabled(GLenum cap) {
    GLboolean ret;
    _pre_call_gles2_callback("glIsEnabled", (GLADapiproc) glad_glIsEnabled, 1, cap);
    ret = glad_glIsEnabled(cap);
    _post_call_gles2_callback((void*) &ret, "glIsEnabled", (GLADapiproc) glad_glIsEnabled, 1, cap);
    return ret;
}
PFNGLISENABLEDPROC glad_debug_glIsEnabled = glad_debug_impl_glIsEnabled;
PFNGLISENABLEDIPROC glad_glIsEnabledi = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glIsEnabledi(GLenum target, GLuint index) {
    GLboolean ret;
    _pre_call_gles2_callback("glIsEnabledi", (GLADapiproc) glad_glIsEnabledi, 2, target, index);
    ret = glad_glIsEnabledi(target, index);
    _post_call_gles2_callback((void*) &ret, "glIsEnabledi", (GLADapiproc) glad_glIsEnabledi, 2, target, index);
    return ret;
}
PFNGLISENABLEDIPROC glad_debug_glIsEnabledi = glad_debug_impl_glIsEnabledi;
PFNGLISFRAMEBUFFERPROC glad_glIsFramebuffer = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glIsFramebuffer(GLuint framebuffer) {
    GLboolean ret;
    _pre_call_gles2_callback("glIsFramebuffer", (GLADapiproc) glad_glIsFramebuffer, 1, framebuffer);
    ret = glad_glIsFramebuffer(framebuffer);
    _post_call_gles2_callback((void*) &ret, "glIsFramebuffer", (GLADapiproc) glad_glIsFramebuffer, 1, framebuffer);
    return ret;
}
PFNGLISFRAMEBUFFERPROC glad_debug_glIsFramebuffer = glad_debug_impl_glIsFramebuffer;
PFNGLISPROGRAMPROC glad_glIsProgram = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glIsProgram(GLuint program) {
    GLboolean ret;
    _pre_call_gles2_callback("glIsProgram", (GLADapiproc) glad_glIsProgram, 1, program);
    ret = glad_glIsProgram(program);
    _post_call_gles2_callback((void*) &ret, "glIsProgram", (GLADapiproc) glad_glIsProgram, 1, program);
    return ret;
}
PFNGLISPROGRAMPROC glad_debug_glIsProgram = glad_debug_impl_glIsProgram;
PFNGLISPROGRAMPIPELINEPROC glad_glIsProgramPipeline = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glIsProgramPipeline(GLuint pipeline) {
    GLboolean ret;
    _pre_call_gles2_callback("glIsProgramPipeline", (GLADapiproc) glad_glIsProgramPipeline, 1, pipeline);
    ret = glad_glIsProgramPipeline(pipeline);
    _post_call_gles2_callback((void*) &ret, "glIsProgramPipeline", (GLADapiproc) glad_glIsProgramPipeline, 1, pipeline);
    return ret;
}
PFNGLISPROGRAMPIPELINEPROC glad_debug_glIsProgramPipeline = glad_debug_impl_glIsProgramPipeline;
PFNGLISQUERYPROC glad_glIsQuery = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glIsQuery(GLuint id) {
    GLboolean ret;
    _pre_call_gles2_callback("glIsQuery", (GLADapiproc) glad_glIsQuery, 1, id);
    ret = glad_glIsQuery(id);
    _post_call_gles2_callback((void*) &ret, "glIsQuery", (GLADapiproc) glad_glIsQuery, 1, id);
    return ret;
}
PFNGLISQUERYPROC glad_debug_glIsQuery = glad_debug_impl_glIsQuery;
PFNGLISRENDERBUFFERPROC glad_glIsRenderbuffer = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glIsRenderbuffer(GLuint renderbuffer) {
    GLboolean ret;
    _pre_call_gles2_callback("glIsRenderbuffer", (GLADapiproc) glad_glIsRenderbuffer, 1, renderbuffer);
    ret = glad_glIsRenderbuffer(renderbuffer);
    _post_call_gles2_callback((void*) &ret, "glIsRenderbuffer", (GLADapiproc) glad_glIsRenderbuffer, 1, renderbuffer);
    return ret;
}
PFNGLISRENDERBUFFERPROC glad_debug_glIsRenderbuffer = glad_debug_impl_glIsRenderbuffer;
PFNGLISSAMPLERPROC glad_glIsSampler = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glIsSampler(GLuint sampler) {
    GLboolean ret;
    _pre_call_gles2_callback("glIsSampler", (GLADapiproc) glad_glIsSampler, 1, sampler);
    ret = glad_glIsSampler(sampler);
    _post_call_gles2_callback((void*) &ret, "glIsSampler", (GLADapiproc) glad_glIsSampler, 1, sampler);
    return ret;
}
PFNGLISSAMPLERPROC glad_debug_glIsSampler = glad_debug_impl_glIsSampler;
PFNGLISSHADERPROC glad_glIsShader = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glIsShader(GLuint shader) {
    GLboolean ret;
    _pre_call_gles2_callback("glIsShader", (GLADapiproc) glad_glIsShader, 1, shader);
    ret = glad_glIsShader(shader);
    _post_call_gles2_callback((void*) &ret, "glIsShader", (GLADapiproc) glad_glIsShader, 1, shader);
    return ret;
}
PFNGLISSHADERPROC glad_debug_glIsShader = glad_debug_impl_glIsShader;
PFNGLISSYNCPROC glad_glIsSync = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glIsSync(GLsync sync) {
    GLboolean ret;
    _pre_call_gles2_callback("glIsSync", (GLADapiproc) glad_glIsSync, 1, sync);
    ret = glad_glIsSync(sync);
    _post_call_gles2_callback((void*) &ret, "glIsSync", (GLADapiproc) glad_glIsSync, 1, sync);
    return ret;
}
PFNGLISSYNCPROC glad_debug_glIsSync = glad_debug_impl_glIsSync;
PFNGLISTEXTUREPROC glad_glIsTexture = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glIsTexture(GLuint texture) {
    GLboolean ret;
    _pre_call_gles2_callback("glIsTexture", (GLADapiproc) glad_glIsTexture, 1, texture);
    ret = glad_glIsTexture(texture);
    _post_call_gles2_callback((void*) &ret, "glIsTexture", (GLADapiproc) glad_glIsTexture, 1, texture);
    return ret;
}
PFNGLISTEXTUREPROC glad_debug_glIsTexture = glad_debug_impl_glIsTexture;
PFNGLISTRANSFORMFEEDBACKPROC glad_glIsTransformFeedback = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glIsTransformFeedback(GLuint id) {
    GLboolean ret;
    _pre_call_gles2_callback("glIsTransformFeedback", (GLADapiproc) glad_glIsTransformFeedback, 1, id);
    ret = glad_glIsTransformFeedback(id);
    _post_call_gles2_callback((void*) &ret, "glIsTransformFeedback", (GLADapiproc) glad_glIsTransformFeedback, 1, id);
    return ret;
}
PFNGLISTRANSFORMFEEDBACKPROC glad_debug_glIsTransformFeedback = glad_debug_impl_glIsTransformFeedback;
PFNGLISVERTEXARRAYPROC glad_glIsVertexArray = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glIsVertexArray(GLuint array) {
    GLboolean ret;
    _pre_call_gles2_callback("glIsVertexArray", (GLADapiproc) glad_glIsVertexArray, 1, array);
    ret = glad_glIsVertexArray(array);
    _post_call_gles2_callback((void*) &ret, "glIsVertexArray", (GLADapiproc) glad_glIsVertexArray, 1, array);
    return ret;
}
PFNGLISVERTEXARRAYPROC glad_debug_glIsVertexArray = glad_debug_impl_glIsVertexArray;
PFNGLLINEWIDTHPROC glad_glLineWidth = NULL;
static void GLAD_API_PTR glad_debug_impl_glLineWidth(GLfloat width) {
    _pre_call_gles2_callback("glLineWidth", (GLADapiproc) glad_glLineWidth, 1, width);
    glad_glLineWidth(width);
    _post_call_gles2_callback(NULL, "glLineWidth", (GLADapiproc) glad_glLineWidth, 1, width);
    
}
PFNGLLINEWIDTHPROC glad_debug_glLineWidth = glad_debug_impl_glLineWidth;
PFNGLLINKPROGRAMPROC glad_glLinkProgram = NULL;
static void GLAD_API_PTR glad_debug_impl_glLinkProgram(GLuint program) {
    _pre_call_gles2_callback("glLinkProgram", (GLADapiproc) glad_glLinkProgram, 1, program);
    glad_glLinkProgram(program);
    _post_call_gles2_callback(NULL, "glLinkProgram", (GLADapiproc) glad_glLinkProgram, 1, program);
    
}
PFNGLLINKPROGRAMPROC glad_debug_glLinkProgram = glad_debug_impl_glLinkProgram;
PFNGLMAPBUFFERRANGEPROC glad_glMapBufferRange = NULL;
static void * GLAD_API_PTR glad_debug_impl_glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) {
    void * ret;
    _pre_call_gles2_callback("glMapBufferRange", (GLADapiproc) glad_glMapBufferRange, 4, target, offset, length, access);
    ret = glad_glMapBufferRange(target, offset, length, access);
    _post_call_gles2_callback((void*) &ret, "glMapBufferRange", (GLADapiproc) glad_glMapBufferRange, 4, target, offset, length, access);
    return ret;
}
PFNGLMAPBUFFERRANGEPROC glad_debug_glMapBufferRange = glad_debug_impl_glMapBufferRange;
PFNGLMEMORYBARRIERPROC glad_glMemoryBarrier = NULL;
static void GLAD_API_PTR glad_debug_impl_glMemoryBarrier(GLbitfield barriers) {
    _pre_call_gles2_callback("glMemoryBarrier", (GLADapiproc) glad_glMemoryBarrier, 1, barriers);
    glad_glMemoryBarrier(barriers);
    _post_call_gles2_callback(NULL, "glMemoryBarrier", (GLADapiproc) glad_glMemoryBarrier, 1, barriers);
    
}
PFNGLMEMORYBARRIERPROC glad_debug_glMemoryBarrier = glad_debug_impl_glMemoryBarrier;
PFNGLMEMORYBARRIERBYREGIONPROC glad_glMemoryBarrierByRegion = NULL;
static void GLAD_API_PTR glad_debug_impl_glMemoryBarrierByRegion(GLbitfield barriers) {
    _pre_call_gles2_callback("glMemoryBarrierByRegion", (GLADapiproc) glad_glMemoryBarrierByRegion, 1, barriers);
    glad_glMemoryBarrierByRegion(barriers);
    _post_call_gles2_callback(NULL, "glMemoryBarrierByRegion", (GLADapiproc) glad_glMemoryBarrierByRegion, 1, barriers);
    
}
PFNGLMEMORYBARRIERBYREGIONPROC glad_debug_glMemoryBarrierByRegion = glad_debug_impl_glMemoryBarrierByRegion;
PFNGLMINSAMPLESHADINGPROC glad_glMinSampleShading = NULL;
static void GLAD_API_PTR glad_debug_impl_glMinSampleShading(GLfloat value) {
    _pre_call_gles2_callback("glMinSampleShading", (GLADapiproc) glad_glMinSampleShading, 1, value);
    glad_glMinSampleShading(value);
    _post_call_gles2_callback(NULL, "glMinSampleShading", (GLADapiproc) glad_glMinSampleShading, 1, value);
    
}
PFNGLMINSAMPLESHADINGPROC glad_debug_glMinSampleShading = glad_debug_impl_glMinSampleShading;
PFNGLOBJECTLABELPROC glad_glObjectLabel = NULL;
static void GLAD_API_PTR glad_debug_impl_glObjectLabel(GLenum identifier, GLuint name, GLsizei length, const GLchar * label) {
    _pre_call_gles2_callback("glObjectLabel", (GLADapiproc) glad_glObjectLabel, 4, identifier, name, length, label);
    glad_glObjectLabel(identifier, name, length, label);
    _post_call_gles2_callback(NULL, "glObjectLabel", (GLADapiproc) glad_glObjectLabel, 4, identifier, name, length, label);
    
}
PFNGLOBJECTLABELPROC glad_debug_glObjectLabel = glad_debug_impl_glObjectLabel;
PFNGLOBJECTLABELKHRPROC glad_glObjectLabelKHR = NULL;
static void GLAD_API_PTR glad_debug_impl_glObjectLabelKHR(GLenum identifier, GLuint name, GLsizei length, const GLchar * label) {
    _pre_call_gles2_callback("glObjectLabelKHR", (GLADapiproc) glad_glObjectLabelKHR, 4, identifier, name, length, label);
    glad_glObjectLabelKHR(identifier, name, length, label);
    _post_call_gles2_callback(NULL, "glObjectLabelKHR", (GLADapiproc) glad_glObjectLabelKHR, 4, identifier, name, length, label);
    
}
PFNGLOBJECTLABELKHRPROC glad_debug_glObjectLabelKHR = glad_debug_impl_glObjectLabelKHR;
PFNGLOBJECTPTRLABELPROC glad_glObjectPtrLabel = NULL;
static void GLAD_API_PTR glad_debug_impl_glObjectPtrLabel(const void * ptr, GLsizei length, const GLchar * label) {
    _pre_call_gles2_callback("glObjectPtrLabel", (GLADapiproc) glad_glObjectPtrLabel, 3, ptr, length, label);
    glad_glObjectPtrLabel(ptr, length, label);
    _post_call_gles2_callback(NULL, "glObjectPtrLabel", (GLADapiproc) glad_glObjectPtrLabel, 3, ptr, length, label);
    
}
PFNGLOBJECTPTRLABELPROC glad_debug_glObjectPtrLabel = glad_debug_impl_glObjectPtrLabel;
PFNGLOBJECTPTRLABELKHRPROC glad_glObjectPtrLabelKHR = NULL;
static void GLAD_API_PTR glad_debug_impl_glObjectPtrLabelKHR(const void * ptr, GLsizei length, const GLchar * label) {
    _pre_call_gles2_callback("glObjectPtrLabelKHR", (GLADapiproc) glad_glObjectPtrLabelKHR, 3, ptr, length, label);
    glad_glObjectPtrLabelKHR(ptr, length, label);
    _post_call_gles2_callback(NULL, "glObjectPtrLabelKHR", (GLADapiproc) glad_glObjectPtrLabelKHR, 3, ptr, length, label);
    
}
PFNGLOBJECTPTRLABELKHRPROC glad_debug_glObjectPtrLabelKHR = glad_debug_impl_glObjectPtrLabelKHR;
PFNGLPATCHPARAMETERIPROC glad_glPatchParameteri = NULL;
static void GLAD_API_PTR glad_debug_impl_glPatchParameteri(GLenum pname, GLint value) {
    _pre_call_gles2_callback("glPatchParameteri", (GLADapiproc) glad_glPatchParameteri, 2, pname, value);
    glad_glPatchParameteri(pname, value);
    _post_call_gles2_callback(NULL, "glPatchParameteri", (GLADapiproc) glad_glPatchParameteri, 2, pname, value);
    
}
PFNGLPATCHPARAMETERIPROC glad_debug_glPatchParameteri = glad_debug_impl_glPatchParameteri;
PFNGLPAUSETRANSFORMFEEDBACKPROC glad_glPauseTransformFeedback = NULL;
static void GLAD_API_PTR glad_debug_impl_glPauseTransformFeedback(void) {
    _pre_call_gles2_callback("glPauseTransformFeedback", (GLADapiproc) glad_glPauseTransformFeedback, 0);
    glad_glPauseTransformFeedback();
    _post_call_gles2_callback(NULL, "glPauseTransformFeedback", (GLADapiproc) glad_glPauseTransformFeedback, 0);
    
}
PFNGLPAUSETRANSFORMFEEDBACKPROC glad_debug_glPauseTransformFeedback = glad_debug_impl_glPauseTransformFeedback;
PFNGLPIXELSTOREIPROC glad_glPixelStorei = NULL;
static void GLAD_API_PTR glad_debug_impl_glPixelStorei(GLenum pname, GLint param) {
    _pre_call_gles2_callback("glPixelStorei", (GLADapiproc) glad_glPixelStorei, 2, pname, param);
    glad_glPixelStorei(pname, param);
    _post_call_gles2_callback(NULL, "glPixelStorei", (GLADapiproc) glad_glPixelStorei, 2, pname, param);
    
}
PFNGLPIXELSTOREIPROC glad_debug_glPixelStorei = glad_debug_impl_glPixelStorei;
PFNGLPOLYGONOFFSETPROC glad_glPolygonOffset = NULL;
static void GLAD_API_PTR glad_debug_impl_glPolygonOffset(GLfloat factor, GLfloat units) {
    _pre_call_gles2_callback("glPolygonOffset", (GLADapiproc) glad_glPolygonOffset, 2, factor, units);
    glad_glPolygonOffset(factor, units);
    _post_call_gles2_callback(NULL, "glPolygonOffset", (GLADapiproc) glad_glPolygonOffset, 2, factor, units);
    
}
PFNGLPOLYGONOFFSETPROC glad_debug_glPolygonOffset = glad_debug_impl_glPolygonOffset;
PFNGLPOPDEBUGGROUPPROC glad_glPopDebugGroup = NULL;
static void GLAD_API_PTR glad_debug_impl_glPopDebugGroup(void) {
    _pre_call_gles2_callback("glPopDebugGroup", (GLADapiproc) glad_glPopDebugGroup, 0);
    glad_glPopDebugGroup();
    _post_call_gles2_callback(NULL, "glPopDebugGroup", (GLADapiproc) glad_glPopDebugGroup, 0);
    
}
PFNGLPOPDEBUGGROUPPROC glad_debug_glPopDebugGroup = glad_debug_impl_glPopDebugGroup;
PFNGLPOPDEBUGGROUPKHRPROC glad_glPopDebugGroupKHR = NULL;
static void GLAD_API_PTR glad_debug_impl_glPopDebugGroupKHR(void) {
    _pre_call_gles2_callback("glPopDebugGroupKHR", (GLADapiproc) glad_glPopDebugGroupKHR, 0);
    glad_glPopDebugGroupKHR();
    _post_call_gles2_callback(NULL, "glPopDebugGroupKHR", (GLADapiproc) glad_glPopDebugGroupKHR, 0);
    
}
PFNGLPOPDEBUGGROUPKHRPROC glad_debug_glPopDebugGroupKHR = glad_debug_impl_glPopDebugGroupKHR;
PFNGLPRIMITIVEBOUNDINGBOXPROC glad_glPrimitiveBoundingBox = NULL;
static void GLAD_API_PTR glad_debug_impl_glPrimitiveBoundingBox(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW) {
    _pre_call_gles2_callback("glPrimitiveBoundingBox", (GLADapiproc) glad_glPrimitiveBoundingBox, 8, minX, minY, minZ, minW, maxX, maxY, maxZ, maxW);
    glad_glPrimitiveBoundingBox(minX, minY, minZ, minW, maxX, maxY, maxZ, maxW);
    _post_call_gles2_callback(NULL, "glPrimitiveBoundingBox", (GLADapiproc) glad_glPrimitiveBoundingBox, 8, minX, minY, minZ, minW, maxX, maxY, maxZ, maxW);
    
}
PFNGLPRIMITIVEBOUNDINGBOXPROC glad_debug_glPrimitiveBoundingBox = glad_debug_impl_glPrimitiveBoundingBox;
PFNGLPROGRAMBINARYPROC glad_glProgramBinary = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramBinary(GLuint program, GLenum binaryFormat, const void * binary, GLsizei length) {
    _pre_call_gles2_callback("glProgramBinary", (GLADapiproc) glad_glProgramBinary, 4, program, binaryFormat, binary, length);
    glad_glProgramBinary(program, binaryFormat, binary, length);
    _post_call_gles2_callback(NULL, "glProgramBinary", (GLADapiproc) glad_glProgramBinary, 4, program, binaryFormat, binary, length);
    
}
PFNGLPROGRAMBINARYPROC glad_debug_glProgramBinary = glad_debug_impl_glProgramBinary;
PFNGLPROGRAMPARAMETERIPROC glad_glProgramParameteri = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramParameteri(GLuint program, GLenum pname, GLint value) {
    _pre_call_gles2_callback("glProgramParameteri", (GLADapiproc) glad_glProgramParameteri, 3, program, pname, value);
    glad_glProgramParameteri(program, pname, value);
    _post_call_gles2_callback(NULL, "glProgramParameteri", (GLADapiproc) glad_glProgramParameteri, 3, program, pname, value);
    
}
PFNGLPROGRAMPARAMETERIPROC glad_debug_glProgramParameteri = glad_debug_impl_glProgramParameteri;
PFNGLPROGRAMUNIFORM1FPROC glad_glProgramUniform1f = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform1f(GLuint program, GLint location, GLfloat v0) {
    _pre_call_gles2_callback("glProgramUniform1f", (GLADapiproc) glad_glProgramUniform1f, 3, program, location, v0);
    glad_glProgramUniform1f(program, location, v0);
    _post_call_gles2_callback(NULL, "glProgramUniform1f", (GLADapiproc) glad_glProgramUniform1f, 3, program, location, v0);
    
}
PFNGLPROGRAMUNIFORM1FPROC glad_debug_glProgramUniform1f = glad_debug_impl_glProgramUniform1f;
PFNGLPROGRAMUNIFORM1FVPROC glad_glProgramUniform1fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform1fv(GLuint program, GLint location, GLsizei count, const GLfloat * value) {
    _pre_call_gles2_callback("glProgramUniform1fv", (GLADapiproc) glad_glProgramUniform1fv, 4, program, location, count, value);
    glad_glProgramUniform1fv(program, location, count, value);
    _post_call_gles2_callback(NULL, "glProgramUniform1fv", (GLADapiproc) glad_glProgramUniform1fv, 4, program, location, count, value);
    
}
PFNGLPROGRAMUNIFORM1FVPROC glad_debug_glProgramUniform1fv = glad_debug_impl_glProgramUniform1fv;
PFNGLPROGRAMUNIFORM1IPROC glad_glProgramUniform1i = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform1i(GLuint program, GLint location, GLint v0) {
    _pre_call_gles2_callback("glProgramUniform1i", (GLADapiproc) glad_glProgramUniform1i, 3, program, location, v0);
    glad_glProgramUniform1i(program, location, v0);
    _post_call_gles2_callback(NULL, "glProgramUniform1i", (GLADapiproc) glad_glProgramUniform1i, 3, program, location, v0);
    
}
PFNGLPROGRAMUNIFORM1IPROC glad_debug_glProgramUniform1i = glad_debug_impl_glProgramUniform1i;
PFNGLPROGRAMUNIFORM1IVPROC glad_glProgramUniform1iv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform1iv(GLuint program, GLint location, GLsizei count, const GLint * value) {
    _pre_call_gles2_callback("glProgramUniform1iv", (GLADapiproc) glad_glProgramUniform1iv, 4, program, location, count, value);
    glad_glProgramUniform1iv(program, location, count, value);
    _post_call_gles2_callback(NULL, "glProgramUniform1iv", (GLADapiproc) glad_glProgramUniform1iv, 4, program, location, count, value);
    
}
PFNGLPROGRAMUNIFORM1IVPROC glad_debug_glProgramUniform1iv = glad_debug_impl_glProgramUniform1iv;
PFNGLPROGRAMUNIFORM1UIPROC glad_glProgramUniform1ui = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform1ui(GLuint program, GLint location, GLuint v0) {
    _pre_call_gles2_callback("glProgramUniform1ui", (GLADapiproc) glad_glProgramUniform1ui, 3, program, location, v0);
    glad_glProgramUniform1ui(program, location, v0);
    _post_call_gles2_callback(NULL, "glProgramUniform1ui", (GLADapiproc) glad_glProgramUniform1ui, 3, program, location, v0);
    
}
PFNGLPROGRAMUNIFORM1UIPROC glad_debug_glProgramUniform1ui = glad_debug_impl_glProgramUniform1ui;
PFNGLPROGRAMUNIFORM1UIVPROC glad_glProgramUniform1uiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform1uiv(GLuint program, GLint location, GLsizei count, const GLuint * value) {
    _pre_call_gles2_callback("glProgramUniform1uiv", (GLADapiproc) glad_glProgramUniform1uiv, 4, program, location, count, value);
    glad_glProgramUniform1uiv(program, location, count, value);
    _post_call_gles2_callback(NULL, "glProgramUniform1uiv", (GLADapiproc) glad_glProgramUniform1uiv, 4, program, location, count, value);
    
}
PFNGLPROGRAMUNIFORM1UIVPROC glad_debug_glProgramUniform1uiv = glad_debug_impl_glProgramUniform1uiv;
PFNGLPROGRAMUNIFORM2FPROC glad_glProgramUniform2f = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform2f(GLuint program, GLint location, GLfloat v0, GLfloat v1) {
    _pre_call_gles2_callback("glProgramUniform2f", (GLADapiproc) glad_glProgramUniform2f, 4, program, location, v0, v1);
    glad_glProgramUniform2f(program, location, v0, v1);
    _post_call_gles2_callback(NULL, "glProgramUniform2f", (GLADapiproc) glad_glProgramUniform2f, 4, program, location, v0, v1);
    
}
PFNGLPROGRAMUNIFORM2FPROC glad_debug_glProgramUniform2f = glad_debug_impl_glProgramUniform2f;
PFNGLPROGRAMUNIFORM2FVPROC glad_glProgramUniform2fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat * value) {
    _pre_call_gles2_callback("glProgramUniform2fv", (GLADapiproc) glad_glProgramUniform2fv, 4, program, location, count, value);
    glad_glProgramUniform2fv(program, location, count, value);
    _post_call_gles2_callback(NULL, "glProgramUniform2fv", (GLADapiproc) glad_glProgramUniform2fv, 4, program, location, count, value);
    
}
PFNGLPROGRAMUNIFORM2FVPROC glad_debug_glProgramUniform2fv = glad_debug_impl_glProgramUniform2fv;
PFNGLPROGRAMUNIFORM2IPROC glad_glProgramUniform2i = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform2i(GLuint program, GLint location, GLint v0, GLint v1) {
    _pre_call_gles2_callback("glProgramUniform2i", (GLADapiproc) glad_glProgramUniform2i, 4, program, location, v0, v1);
    glad_glProgramUniform2i(program, location, v0, v1);
    _post_call_gles2_callback(NULL, "glProgramUniform2i", (GLADapiproc) glad_glProgramUniform2i, 4, program, location, v0, v1);
    
}
PFNGLPROGRAMUNIFORM2IPROC glad_debug_glProgramUniform2i = glad_debug_impl_glProgramUniform2i;
PFNGLPROGRAMUNIFORM2IVPROC glad_glProgramUniform2iv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform2iv(GLuint program, GLint location, GLsizei count, const GLint * value) {
    _pre_call_gles2_callback("glProgramUniform2iv", (GLADapiproc) glad_glProgramUniform2iv, 4, program, location, count, value);
    glad_glProgramUniform2iv(program, location, count, value);
    _post_call_gles2_callback(NULL, "glProgramUniform2iv", (GLADapiproc) glad_glProgramUniform2iv, 4, program, location, count, value);
    
}
PFNGLPROGRAMUNIFORM2IVPROC glad_debug_glProgramUniform2iv = glad_debug_impl_glProgramUniform2iv;
PFNGLPROGRAMUNIFORM2UIPROC glad_glProgramUniform2ui = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform2ui(GLuint program, GLint location, GLuint v0, GLuint v1) {
    _pre_call_gles2_callback("glProgramUniform2ui", (GLADapiproc) glad_glProgramUniform2ui, 4, program, location, v0, v1);
    glad_glProgramUniform2ui(program, location, v0, v1);
    _post_call_gles2_callback(NULL, "glProgramUniform2ui", (GLADapiproc) glad_glProgramUniform2ui, 4, program, location, v0, v1);
    
}
PFNGLPROGRAMUNIFORM2UIPROC glad_debug_glProgramUniform2ui = glad_debug_impl_glProgramUniform2ui;
PFNGLPROGRAMUNIFORM2UIVPROC glad_glProgramUniform2uiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform2uiv(GLuint program, GLint location, GLsizei count, const GLuint * value) {
    _pre_call_gles2_callback("glProgramUniform2uiv", (GLADapiproc) glad_glProgramUniform2uiv, 4, program, location, count, value);
    glad_glProgramUniform2uiv(program, location, count, value);
    _post_call_gles2_callback(NULL, "glProgramUniform2uiv", (GLADapiproc) glad_glProgramUniform2uiv, 4, program, location, count, value);
    
}
PFNGLPROGRAMUNIFORM2UIVPROC glad_debug_glProgramUniform2uiv = glad_debug_impl_glProgramUniform2uiv;
PFNGLPROGRAMUNIFORM3FPROC glad_glProgramUniform3f = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform3f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2) {
    _pre_call_gles2_callback("glProgramUniform3f", (GLADapiproc) glad_glProgramUniform3f, 5, program, location, v0, v1, v2);
    glad_glProgramUniform3f(program, location, v0, v1, v2);
    _post_call_gles2_callback(NULL, "glProgramUniform3f", (GLADapiproc) glad_glProgramUniform3f, 5, program, location, v0, v1, v2);
    
}
PFNGLPROGRAMUNIFORM3FPROC glad_debug_glProgramUniform3f = glad_debug_impl_glProgramUniform3f;
PFNGLPROGRAMUNIFORM3FVPROC glad_glProgramUniform3fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat * value) {
    _pre_call_gles2_callback("glProgramUniform3fv", (GLADapiproc) glad_glProgramUniform3fv, 4, program, location, count, value);
    glad_glProgramUniform3fv(program, location, count, value);
    _post_call_gles2_callback(NULL, "glProgramUniform3fv", (GLADapiproc) glad_glProgramUniform3fv, 4, program, location, count, value);
    
}
PFNGLPROGRAMUNIFORM3FVPROC glad_debug_glProgramUniform3fv = glad_debug_impl_glProgramUniform3fv;
PFNGLPROGRAMUNIFORM3IPROC glad_glProgramUniform3i = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform3i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2) {
    _pre_call_gles2_callback("glProgramUniform3i", (GLADapiproc) glad_glProgramUniform3i, 5, program, location, v0, v1, v2);
    glad_glProgramUniform3i(program, location, v0, v1, v2);
    _post_call_gles2_callback(NULL, "glProgramUniform3i", (GLADapiproc) glad_glProgramUniform3i, 5, program, location, v0, v1, v2);
    
}
PFNGLPROGRAMUNIFORM3IPROC glad_debug_glProgramUniform3i = glad_debug_impl_glProgramUniform3i;
PFNGLPROGRAMUNIFORM3IVPROC glad_glProgramUniform3iv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform3iv(GLuint program, GLint location, GLsizei count, const GLint * value) {
    _pre_call_gles2_callback("glProgramUniform3iv", (GLADapiproc) glad_glProgramUniform3iv, 4, program, location, count, value);
    glad_glProgramUniform3iv(program, location, count, value);
    _post_call_gles2_callback(NULL, "glProgramUniform3iv", (GLADapiproc) glad_glProgramUniform3iv, 4, program, location, count, value);
    
}
PFNGLPROGRAMUNIFORM3IVPROC glad_debug_glProgramUniform3iv = glad_debug_impl_glProgramUniform3iv;
PFNGLPROGRAMUNIFORM3UIPROC glad_glProgramUniform3ui = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform3ui(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2) {
    _pre_call_gles2_callback("glProgramUniform3ui", (GLADapiproc) glad_glProgramUniform3ui, 5, program, location, v0, v1, v2);
    glad_glProgramUniform3ui(program, location, v0, v1, v2);
    _post_call_gles2_callback(NULL, "glProgramUniform3ui", (GLADapiproc) glad_glProgramUniform3ui, 5, program, location, v0, v1, v2);
    
}
PFNGLPROGRAMUNIFORM3UIPROC glad_debug_glProgramUniform3ui = glad_debug_impl_glProgramUniform3ui;
PFNGLPROGRAMUNIFORM3UIVPROC glad_glProgramUniform3uiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform3uiv(GLuint program, GLint location, GLsizei count, const GLuint * value) {
    _pre_call_gles2_callback("glProgramUniform3uiv", (GLADapiproc) glad_glProgramUniform3uiv, 4, program, location, count, value);
    glad_glProgramUniform3uiv(program, location, count, value);
    _post_call_gles2_callback(NULL, "glProgramUniform3uiv", (GLADapiproc) glad_glProgramUniform3uiv, 4, program, location, count, value);
    
}
PFNGLPROGRAMUNIFORM3UIVPROC glad_debug_glProgramUniform3uiv = glad_debug_impl_glProgramUniform3uiv;
PFNGLPROGRAMUNIFORM4FPROC glad_glProgramUniform4f = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform4f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
    _pre_call_gles2_callback("glProgramUniform4f", (GLADapiproc) glad_glProgramUniform4f, 6, program, location, v0, v1, v2, v3);
    glad_glProgramUniform4f(program, location, v0, v1, v2, v3);
    _post_call_gles2_callback(NULL, "glProgramUniform4f", (GLADapiproc) glad_glProgramUniform4f, 6, program, location, v0, v1, v2, v3);
    
}
PFNGLPROGRAMUNIFORM4FPROC glad_debug_glProgramUniform4f = glad_debug_impl_glProgramUniform4f;
PFNGLPROGRAMUNIFORM4FVPROC glad_glProgramUniform4fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat * value) {
    _pre_call_gles2_callback("glProgramUniform4fv", (GLADapiproc) glad_glProgramUniform4fv, 4, program, location, count, value);
    glad_glProgramUniform4fv(program, location, count, value);
    _post_call_gles2_callback(NULL, "glProgramUniform4fv", (GLADapiproc) glad_glProgramUniform4fv, 4, program, location, count, value);
    
}
PFNGLPROGRAMUNIFORM4FVPROC glad_debug_glProgramUniform4fv = glad_debug_impl_glProgramUniform4fv;
PFNGLPROGRAMUNIFORM4IPROC glad_glProgramUniform4i = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform4i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3) {
    _pre_call_gles2_callback("glProgramUniform4i", (GLADapiproc) glad_glProgramUniform4i, 6, program, location, v0, v1, v2, v3);
    glad_glProgramUniform4i(program, location, v0, v1, v2, v3);
    _post_call_gles2_callback(NULL, "glProgramUniform4i", (GLADapiproc) glad_glProgramUniform4i, 6, program, location, v0, v1, v2, v3);
    
}
PFNGLPROGRAMUNIFORM4IPROC glad_debug_glProgramUniform4i = glad_debug_impl_glProgramUniform4i;
PFNGLPROGRAMUNIFORM4IVPROC glad_glProgramUniform4iv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform4iv(GLuint program, GLint location, GLsizei count, const GLint * value) {
    _pre_call_gles2_callback("glProgramUniform4iv", (GLADapiproc) glad_glProgramUniform4iv, 4, program, location, count, value);
    glad_glProgramUniform4iv(program, location, count, value);
    _post_call_gles2_callback(NULL, "glProgramUniform4iv", (GLADapiproc) glad_glProgramUniform4iv, 4, program, location, count, value);
    
}
PFNGLPROGRAMUNIFORM4IVPROC glad_debug_glProgramUniform4iv = glad_debug_impl_glProgramUniform4iv;
PFNGLPROGRAMUNIFORM4UIPROC glad_glProgramUniform4ui = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform4ui(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) {
    _pre_call_gles2_callback("glProgramUniform4ui", (GLADapiproc) glad_glProgramUniform4ui, 6, program, location, v0, v1, v2, v3);
    glad_glProgramUniform4ui(program, location, v0, v1, v2, v3);
    _post_call_gles2_callback(NULL, "glProgramUniform4ui", (GLADapiproc) glad_glProgramUniform4ui, 6, program, location, v0, v1, v2, v3);
    
}
PFNGLPROGRAMUNIFORM4UIPROC glad_debug_glProgramUniform4ui = glad_debug_impl_glProgramUniform4ui;
PFNGLPROGRAMUNIFORM4UIVPROC glad_glProgramUniform4uiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniform4uiv(GLuint program, GLint location, GLsizei count, const GLuint * value) {
    _pre_call_gles2_callback("glProgramUniform4uiv", (GLADapiproc) glad_glProgramUniform4uiv, 4, program, location, count, value);
    glad_glProgramUniform4uiv(program, location, count, value);
    _post_call_gles2_callback(NULL, "glProgramUniform4uiv", (GLADapiproc) glad_glProgramUniform4uiv, 4, program, location, count, value);
    
}
PFNGLPROGRAMUNIFORM4UIVPROC glad_debug_glProgramUniform4uiv = glad_debug_impl_glProgramUniform4uiv;
PFNGLPROGRAMUNIFORMMATRIX2FVPROC glad_glProgramUniformMatrix2fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniformMatrix2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glProgramUniformMatrix2fv", (GLADapiproc) glad_glProgramUniformMatrix2fv, 5, program, location, count, transpose, value);
    glad_glProgramUniformMatrix2fv(program, location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glProgramUniformMatrix2fv", (GLADapiproc) glad_glProgramUniformMatrix2fv, 5, program, location, count, transpose, value);
    
}
PFNGLPROGRAMUNIFORMMATRIX2FVPROC glad_debug_glProgramUniformMatrix2fv = glad_debug_impl_glProgramUniformMatrix2fv;
PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glad_glProgramUniformMatrix2x3fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniformMatrix2x3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glProgramUniformMatrix2x3fv", (GLADapiproc) glad_glProgramUniformMatrix2x3fv, 5, program, location, count, transpose, value);
    glad_glProgramUniformMatrix2x3fv(program, location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glProgramUniformMatrix2x3fv", (GLADapiproc) glad_glProgramUniformMatrix2x3fv, 5, program, location, count, transpose, value);
    
}
PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glad_debug_glProgramUniformMatrix2x3fv = glad_debug_impl_glProgramUniformMatrix2x3fv;
PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glad_glProgramUniformMatrix2x4fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniformMatrix2x4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glProgramUniformMatrix2x4fv", (GLADapiproc) glad_glProgramUniformMatrix2x4fv, 5, program, location, count, transpose, value);
    glad_glProgramUniformMatrix2x4fv(program, location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glProgramUniformMatrix2x4fv", (GLADapiproc) glad_glProgramUniformMatrix2x4fv, 5, program, location, count, transpose, value);
    
}
PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glad_debug_glProgramUniformMatrix2x4fv = glad_debug_impl_glProgramUniformMatrix2x4fv;
PFNGLPROGRAMUNIFORMMATRIX3FVPROC glad_glProgramUniformMatrix3fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniformMatrix3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glProgramUniformMatrix3fv", (GLADapiproc) glad_glProgramUniformMatrix3fv, 5, program, location, count, transpose, value);
    glad_glProgramUniformMatrix3fv(program, location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glProgramUniformMatrix3fv", (GLADapiproc) glad_glProgramUniformMatrix3fv, 5, program, location, count, transpose, value);
    
}
PFNGLPROGRAMUNIFORMMATRIX3FVPROC glad_debug_glProgramUniformMatrix3fv = glad_debug_impl_glProgramUniformMatrix3fv;
PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glad_glProgramUniformMatrix3x2fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniformMatrix3x2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glProgramUniformMatrix3x2fv", (GLADapiproc) glad_glProgramUniformMatrix3x2fv, 5, program, location, count, transpose, value);
    glad_glProgramUniformMatrix3x2fv(program, location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glProgramUniformMatrix3x2fv", (GLADapiproc) glad_glProgramUniformMatrix3x2fv, 5, program, location, count, transpose, value);
    
}
PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glad_debug_glProgramUniformMatrix3x2fv = glad_debug_impl_glProgramUniformMatrix3x2fv;
PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glad_glProgramUniformMatrix3x4fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniformMatrix3x4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glProgramUniformMatrix3x4fv", (GLADapiproc) glad_glProgramUniformMatrix3x4fv, 5, program, location, count, transpose, value);
    glad_glProgramUniformMatrix3x4fv(program, location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glProgramUniformMatrix3x4fv", (GLADapiproc) glad_glProgramUniformMatrix3x4fv, 5, program, location, count, transpose, value);
    
}
PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glad_debug_glProgramUniformMatrix3x4fv = glad_debug_impl_glProgramUniformMatrix3x4fv;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC glad_glProgramUniformMatrix4fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glProgramUniformMatrix4fv", (GLADapiproc) glad_glProgramUniformMatrix4fv, 5, program, location, count, transpose, value);
    glad_glProgramUniformMatrix4fv(program, location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glProgramUniformMatrix4fv", (GLADapiproc) glad_glProgramUniformMatrix4fv, 5, program, location, count, transpose, value);
    
}
PFNGLPROGRAMUNIFORMMATRIX4FVPROC glad_debug_glProgramUniformMatrix4fv = glad_debug_impl_glProgramUniformMatrix4fv;
PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glad_glProgramUniformMatrix4x2fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniformMatrix4x2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glProgramUniformMatrix4x2fv", (GLADapiproc) glad_glProgramUniformMatrix4x2fv, 5, program, location, count, transpose, value);
    glad_glProgramUniformMatrix4x2fv(program, location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glProgramUniformMatrix4x2fv", (GLADapiproc) glad_glProgramUniformMatrix4x2fv, 5, program, location, count, transpose, value);
    
}
PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glad_debug_glProgramUniformMatrix4x2fv = glad_debug_impl_glProgramUniformMatrix4x2fv;
PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glad_glProgramUniformMatrix4x3fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glProgramUniformMatrix4x3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glProgramUniformMatrix4x3fv", (GLADapiproc) glad_glProgramUniformMatrix4x3fv, 5, program, location, count, transpose, value);
    glad_glProgramUniformMatrix4x3fv(program, location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glProgramUniformMatrix4x3fv", (GLADapiproc) glad_glProgramUniformMatrix4x3fv, 5, program, location, count, transpose, value);
    
}
PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glad_debug_glProgramUniformMatrix4x3fv = glad_debug_impl_glProgramUniformMatrix4x3fv;
PFNGLPUSHDEBUGGROUPPROC glad_glPushDebugGroup = NULL;
static void GLAD_API_PTR glad_debug_impl_glPushDebugGroup(GLenum source, GLuint id, GLsizei length, const GLchar * message) {
    _pre_call_gles2_callback("glPushDebugGroup", (GLADapiproc) glad_glPushDebugGroup, 4, source, id, length, message);
    glad_glPushDebugGroup(source, id, length, message);
    _post_call_gles2_callback(NULL, "glPushDebugGroup", (GLADapiproc) glad_glPushDebugGroup, 4, source, id, length, message);
    
}
PFNGLPUSHDEBUGGROUPPROC glad_debug_glPushDebugGroup = glad_debug_impl_glPushDebugGroup;
PFNGLPUSHDEBUGGROUPKHRPROC glad_glPushDebugGroupKHR = NULL;
static void GLAD_API_PTR glad_debug_impl_glPushDebugGroupKHR(GLenum source, GLuint id, GLsizei length, const GLchar * message) {
    _pre_call_gles2_callback("glPushDebugGroupKHR", (GLADapiproc) glad_glPushDebugGroupKHR, 4, source, id, length, message);
    glad_glPushDebugGroupKHR(source, id, length, message);
    _post_call_gles2_callback(NULL, "glPushDebugGroupKHR", (GLADapiproc) glad_glPushDebugGroupKHR, 4, source, id, length, message);
    
}
PFNGLPUSHDEBUGGROUPKHRPROC glad_debug_glPushDebugGroupKHR = glad_debug_impl_glPushDebugGroupKHR;
PFNGLREADBUFFERPROC glad_glReadBuffer = NULL;
static void GLAD_API_PTR glad_debug_impl_glReadBuffer(GLenum src) {
    _pre_call_gles2_callback("glReadBuffer", (GLADapiproc) glad_glReadBuffer, 1, src);
    glad_glReadBuffer(src);
    _post_call_gles2_callback(NULL, "glReadBuffer", (GLADapiproc) glad_glReadBuffer, 1, src);
    
}
PFNGLREADBUFFERPROC glad_debug_glReadBuffer = glad_debug_impl_glReadBuffer;
PFNGLREADPIXELSPROC glad_glReadPixels = NULL;
static void GLAD_API_PTR glad_debug_impl_glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void * pixels) {
    _pre_call_gles2_callback("glReadPixels", (GLADapiproc) glad_glReadPixels, 7, x, y, width, height, format, type, pixels);
    glad_glReadPixels(x, y, width, height, format, type, pixels);
    _post_call_gles2_callback(NULL, "glReadPixels", (GLADapiproc) glad_glReadPixels, 7, x, y, width, height, format, type, pixels);
    
}
PFNGLREADPIXELSPROC glad_debug_glReadPixels = glad_debug_impl_glReadPixels;
PFNGLREADNPIXELSPROC glad_glReadnPixels = NULL;
static void GLAD_API_PTR glad_debug_impl_glReadnPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void * data) {
    _pre_call_gles2_callback("glReadnPixels", (GLADapiproc) glad_glReadnPixels, 8, x, y, width, height, format, type, bufSize, data);
    glad_glReadnPixels(x, y, width, height, format, type, bufSize, data);
    _post_call_gles2_callback(NULL, "glReadnPixels", (GLADapiproc) glad_glReadnPixels, 8, x, y, width, height, format, type, bufSize, data);
    
}
PFNGLREADNPIXELSPROC glad_debug_glReadnPixels = glad_debug_impl_glReadnPixels;
PFNGLRELEASESHADERCOMPILERPROC glad_glReleaseShaderCompiler = NULL;
static void GLAD_API_PTR glad_debug_impl_glReleaseShaderCompiler(void) {
    _pre_call_gles2_callback("glReleaseShaderCompiler", (GLADapiproc) glad_glReleaseShaderCompiler, 0);
    glad_glReleaseShaderCompiler();
    _post_call_gles2_callback(NULL, "glReleaseShaderCompiler", (GLADapiproc) glad_glReleaseShaderCompiler, 0);
    
}
PFNGLRELEASESHADERCOMPILERPROC glad_debug_glReleaseShaderCompiler = glad_debug_impl_glReleaseShaderCompiler;
PFNGLRENDERBUFFERSTORAGEPROC glad_glRenderbufferStorage = NULL;
static void GLAD_API_PTR glad_debug_impl_glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) {
    _pre_call_gles2_callback("glRenderbufferStorage", (GLADapiproc) glad_glRenderbufferStorage, 4, target, internalformat, width, height);
    glad_glRenderbufferStorage(target, internalformat, width, height);
    _post_call_gles2_callback(NULL, "glRenderbufferStorage", (GLADapiproc) glad_glRenderbufferStorage, 4, target, internalformat, width, height);
    
}
PFNGLRENDERBUFFERSTORAGEPROC glad_debug_glRenderbufferStorage = glad_debug_impl_glRenderbufferStorage;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glad_glRenderbufferStorageMultisample = NULL;
static void GLAD_API_PTR glad_debug_impl_glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) {
    _pre_call_gles2_callback("glRenderbufferStorageMultisample", (GLADapiproc) glad_glRenderbufferStorageMultisample, 5, target, samples, internalformat, width, height);
    glad_glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
    _post_call_gles2_callback(NULL, "glRenderbufferStorageMultisample", (GLADapiproc) glad_glRenderbufferStorageMultisample, 5, target, samples, internalformat, width, height);
    
}
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glad_debug_glRenderbufferStorageMultisample = glad_debug_impl_glRenderbufferStorageMultisample;
PFNGLRESUMETRANSFORMFEEDBACKPROC glad_glResumeTransformFeedback = NULL;
static void GLAD_API_PTR glad_debug_impl_glResumeTransformFeedback(void) {
    _pre_call_gles2_callback("glResumeTransformFeedback", (GLADapiproc) glad_glResumeTransformFeedback, 0);
    glad_glResumeTransformFeedback();
    _post_call_gles2_callback(NULL, "glResumeTransformFeedback", (GLADapiproc) glad_glResumeTransformFeedback, 0);
    
}
PFNGLRESUMETRANSFORMFEEDBACKPROC glad_debug_glResumeTransformFeedback = glad_debug_impl_glResumeTransformFeedback;
PFNGLSAMPLECOVERAGEPROC glad_glSampleCoverage = NULL;
static void GLAD_API_PTR glad_debug_impl_glSampleCoverage(GLfloat value, GLboolean invert) {
    _pre_call_gles2_callback("glSampleCoverage", (GLADapiproc) glad_glSampleCoverage, 2, value, invert);
    glad_glSampleCoverage(value, invert);
    _post_call_gles2_callback(NULL, "glSampleCoverage", (GLADapiproc) glad_glSampleCoverage, 2, value, invert);
    
}
PFNGLSAMPLECOVERAGEPROC glad_debug_glSampleCoverage = glad_debug_impl_glSampleCoverage;
PFNGLSAMPLEMASKIPROC glad_glSampleMaski = NULL;
static void GLAD_API_PTR glad_debug_impl_glSampleMaski(GLuint maskNumber, GLbitfield mask) {
    _pre_call_gles2_callback("glSampleMaski", (GLADapiproc) glad_glSampleMaski, 2, maskNumber, mask);
    glad_glSampleMaski(maskNumber, mask);
    _post_call_gles2_callback(NULL, "glSampleMaski", (GLADapiproc) glad_glSampleMaski, 2, maskNumber, mask);
    
}
PFNGLSAMPLEMASKIPROC glad_debug_glSampleMaski = glad_debug_impl_glSampleMaski;
PFNGLSAMPLERPARAMETERIIVPROC glad_glSamplerParameterIiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glSamplerParameterIiv(GLuint sampler, GLenum pname, const GLint * param) {
    _pre_call_gles2_callback("glSamplerParameterIiv", (GLADapiproc) glad_glSamplerParameterIiv, 3, sampler, pname, param);
    glad_glSamplerParameterIiv(sampler, pname, param);
    _post_call_gles2_callback(NULL, "glSamplerParameterIiv", (GLADapiproc) glad_glSamplerParameterIiv, 3, sampler, pname, param);
    
}
PFNGLSAMPLERPARAMETERIIVPROC glad_debug_glSamplerParameterIiv = glad_debug_impl_glSamplerParameterIiv;
PFNGLSAMPLERPARAMETERIUIVPROC glad_glSamplerParameterIuiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glSamplerParameterIuiv(GLuint sampler, GLenum pname, const GLuint * param) {
    _pre_call_gles2_callback("glSamplerParameterIuiv", (GLADapiproc) glad_glSamplerParameterIuiv, 3, sampler, pname, param);
    glad_glSamplerParameterIuiv(sampler, pname, param);
    _post_call_gles2_callback(NULL, "glSamplerParameterIuiv", (GLADapiproc) glad_glSamplerParameterIuiv, 3, sampler, pname, param);
    
}
PFNGLSAMPLERPARAMETERIUIVPROC glad_debug_glSamplerParameterIuiv = glad_debug_impl_glSamplerParameterIuiv;
PFNGLSAMPLERPARAMETERFPROC glad_glSamplerParameterf = NULL;
static void GLAD_API_PTR glad_debug_impl_glSamplerParameterf(GLuint sampler, GLenum pname, GLfloat param) {
    _pre_call_gles2_callback("glSamplerParameterf", (GLADapiproc) glad_glSamplerParameterf, 3, sampler, pname, param);
    glad_glSamplerParameterf(sampler, pname, param);
    _post_call_gles2_callback(NULL, "glSamplerParameterf", (GLADapiproc) glad_glSamplerParameterf, 3, sampler, pname, param);
    
}
PFNGLSAMPLERPARAMETERFPROC glad_debug_glSamplerParameterf = glad_debug_impl_glSamplerParameterf;
PFNGLSAMPLERPARAMETERFVPROC glad_glSamplerParameterfv = NULL;
static void GLAD_API_PTR glad_debug_impl_glSamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat * param) {
    _pre_call_gles2_callback("glSamplerParameterfv", (GLADapiproc) glad_glSamplerParameterfv, 3, sampler, pname, param);
    glad_glSamplerParameterfv(sampler, pname, param);
    _post_call_gles2_callback(NULL, "glSamplerParameterfv", (GLADapiproc) glad_glSamplerParameterfv, 3, sampler, pname, param);
    
}
PFNGLSAMPLERPARAMETERFVPROC glad_debug_glSamplerParameterfv = glad_debug_impl_glSamplerParameterfv;
PFNGLSAMPLERPARAMETERIPROC glad_glSamplerParameteri = NULL;
static void GLAD_API_PTR glad_debug_impl_glSamplerParameteri(GLuint sampler, GLenum pname, GLint param) {
    _pre_call_gles2_callback("glSamplerParameteri", (GLADapiproc) glad_glSamplerParameteri, 3, sampler, pname, param);
    glad_glSamplerParameteri(sampler, pname, param);
    _post_call_gles2_callback(NULL, "glSamplerParameteri", (GLADapiproc) glad_glSamplerParameteri, 3, sampler, pname, param);
    
}
PFNGLSAMPLERPARAMETERIPROC glad_debug_glSamplerParameteri = glad_debug_impl_glSamplerParameteri;
PFNGLSAMPLERPARAMETERIVPROC glad_glSamplerParameteriv = NULL;
static void GLAD_API_PTR glad_debug_impl_glSamplerParameteriv(GLuint sampler, GLenum pname, const GLint * param) {
    _pre_call_gles2_callback("glSamplerParameteriv", (GLADapiproc) glad_glSamplerParameteriv, 3, sampler, pname, param);
    glad_glSamplerParameteriv(sampler, pname, param);
    _post_call_gles2_callback(NULL, "glSamplerParameteriv", (GLADapiproc) glad_glSamplerParameteriv, 3, sampler, pname, param);
    
}
PFNGLSAMPLERPARAMETERIVPROC glad_debug_glSamplerParameteriv = glad_debug_impl_glSamplerParameteriv;
PFNGLSCISSORPROC glad_glScissor = NULL;
static void GLAD_API_PTR glad_debug_impl_glScissor(GLint x, GLint y, GLsizei width, GLsizei height) {
    _pre_call_gles2_callback("glScissor", (GLADapiproc) glad_glScissor, 4, x, y, width, height);
    glad_glScissor(x, y, width, height);
    _post_call_gles2_callback(NULL, "glScissor", (GLADapiproc) glad_glScissor, 4, x, y, width, height);
    
}
PFNGLSCISSORPROC glad_debug_glScissor = glad_debug_impl_glScissor;
PFNGLSHADERBINARYPROC glad_glShaderBinary = NULL;
static void GLAD_API_PTR glad_debug_impl_glShaderBinary(GLsizei count, const GLuint * shaders, GLenum binaryformat, const void * binary, GLsizei length) {
    _pre_call_gles2_callback("glShaderBinary", (GLADapiproc) glad_glShaderBinary, 5, count, shaders, binaryformat, binary, length);
    glad_glShaderBinary(count, shaders, binaryformat, binary, length);
    _post_call_gles2_callback(NULL, "glShaderBinary", (GLADapiproc) glad_glShaderBinary, 5, count, shaders, binaryformat, binary, length);
    
}
PFNGLSHADERBINARYPROC glad_debug_glShaderBinary = glad_debug_impl_glShaderBinary;
PFNGLSHADERSOURCEPROC glad_glShaderSource = NULL;
static void GLAD_API_PTR glad_debug_impl_glShaderSource(GLuint shader, GLsizei count, const GLchar *const* string, const GLint * length) {
    _pre_call_gles2_callback("glShaderSource", (GLADapiproc) glad_glShaderSource, 4, shader, count, string, length);
    glad_glShaderSource(shader, count, string, length);
    _post_call_gles2_callback(NULL, "glShaderSource", (GLADapiproc) glad_glShaderSource, 4, shader, count, string, length);
    
}
PFNGLSHADERSOURCEPROC glad_debug_glShaderSource = glad_debug_impl_glShaderSource;
PFNGLSTENCILFUNCPROC glad_glStencilFunc = NULL;
static void GLAD_API_PTR glad_debug_impl_glStencilFunc(GLenum func, GLint ref, GLuint mask) {
    _pre_call_gles2_callback("glStencilFunc", (GLADapiproc) glad_glStencilFunc, 3, func, ref, mask);
    glad_glStencilFunc(func, ref, mask);
    _post_call_gles2_callback(NULL, "glStencilFunc", (GLADapiproc) glad_glStencilFunc, 3, func, ref, mask);
    
}
PFNGLSTENCILFUNCPROC glad_debug_glStencilFunc = glad_debug_impl_glStencilFunc;
PFNGLSTENCILFUNCSEPARATEPROC glad_glStencilFuncSeparate = NULL;
static void GLAD_API_PTR glad_debug_impl_glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask) {
    _pre_call_gles2_callback("glStencilFuncSeparate", (GLADapiproc) glad_glStencilFuncSeparate, 4, face, func, ref, mask);
    glad_glStencilFuncSeparate(face, func, ref, mask);
    _post_call_gles2_callback(NULL, "glStencilFuncSeparate", (GLADapiproc) glad_glStencilFuncSeparate, 4, face, func, ref, mask);
    
}
PFNGLSTENCILFUNCSEPARATEPROC glad_debug_glStencilFuncSeparate = glad_debug_impl_glStencilFuncSeparate;
PFNGLSTENCILMASKPROC glad_glStencilMask = NULL;
static void GLAD_API_PTR glad_debug_impl_glStencilMask(GLuint mask) {
    _pre_call_gles2_callback("glStencilMask", (GLADapiproc) glad_glStencilMask, 1, mask);
    glad_glStencilMask(mask);
    _post_call_gles2_callback(NULL, "glStencilMask", (GLADapiproc) glad_glStencilMask, 1, mask);
    
}
PFNGLSTENCILMASKPROC glad_debug_glStencilMask = glad_debug_impl_glStencilMask;
PFNGLSTENCILMASKSEPARATEPROC glad_glStencilMaskSeparate = NULL;
static void GLAD_API_PTR glad_debug_impl_glStencilMaskSeparate(GLenum face, GLuint mask) {
    _pre_call_gles2_callback("glStencilMaskSeparate", (GLADapiproc) glad_glStencilMaskSeparate, 2, face, mask);
    glad_glStencilMaskSeparate(face, mask);
    _post_call_gles2_callback(NULL, "glStencilMaskSeparate", (GLADapiproc) glad_glStencilMaskSeparate, 2, face, mask);
    
}
PFNGLSTENCILMASKSEPARATEPROC glad_debug_glStencilMaskSeparate = glad_debug_impl_glStencilMaskSeparate;
PFNGLSTENCILOPPROC glad_glStencilOp = NULL;
static void GLAD_API_PTR glad_debug_impl_glStencilOp(GLenum fail, GLenum zfail, GLenum zpass) {
    _pre_call_gles2_callback("glStencilOp", (GLADapiproc) glad_glStencilOp, 3, fail, zfail, zpass);
    glad_glStencilOp(fail, zfail, zpass);
    _post_call_gles2_callback(NULL, "glStencilOp", (GLADapiproc) glad_glStencilOp, 3, fail, zfail, zpass);
    
}
PFNGLSTENCILOPPROC glad_debug_glStencilOp = glad_debug_impl_glStencilOp;
PFNGLSTENCILOPSEPARATEPROC glad_glStencilOpSeparate = NULL;
static void GLAD_API_PTR glad_debug_impl_glStencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass) {
    _pre_call_gles2_callback("glStencilOpSeparate", (GLADapiproc) glad_glStencilOpSeparate, 4, face, sfail, dpfail, dppass);
    glad_glStencilOpSeparate(face, sfail, dpfail, dppass);
    _post_call_gles2_callback(NULL, "glStencilOpSeparate", (GLADapiproc) glad_glStencilOpSeparate, 4, face, sfail, dpfail, dppass);
    
}
PFNGLSTENCILOPSEPARATEPROC glad_debug_glStencilOpSeparate = glad_debug_impl_glStencilOpSeparate;
PFNGLTEXBUFFERPROC glad_glTexBuffer = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexBuffer(GLenum target, GLenum internalformat, GLuint buffer) {
    _pre_call_gles2_callback("glTexBuffer", (GLADapiproc) glad_glTexBuffer, 3, target, internalformat, buffer);
    glad_glTexBuffer(target, internalformat, buffer);
    _post_call_gles2_callback(NULL, "glTexBuffer", (GLADapiproc) glad_glTexBuffer, 3, target, internalformat, buffer);
    
}
PFNGLTEXBUFFERPROC glad_debug_glTexBuffer = glad_debug_impl_glTexBuffer;
PFNGLTEXBUFFERRANGEPROC glad_glTexBufferRange = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexBufferRange(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size) {
    _pre_call_gles2_callback("glTexBufferRange", (GLADapiproc) glad_glTexBufferRange, 5, target, internalformat, buffer, offset, size);
    glad_glTexBufferRange(target, internalformat, buffer, offset, size);
    _post_call_gles2_callback(NULL, "glTexBufferRange", (GLADapiproc) glad_glTexBufferRange, 5, target, internalformat, buffer, offset, size);
    
}
PFNGLTEXBUFFERRANGEPROC glad_debug_glTexBufferRange = glad_debug_impl_glTexBufferRange;
PFNGLTEXIMAGE2DPROC glad_glTexImage2D = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void * pixels) {
    _pre_call_gles2_callback("glTexImage2D", (GLADapiproc) glad_glTexImage2D, 9, target, level, internalformat, width, height, border, format, type, pixels);
    glad_glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
    _post_call_gles2_callback(NULL, "glTexImage2D", (GLADapiproc) glad_glTexImage2D, 9, target, level, internalformat, width, height, border, format, type, pixels);
    
}
PFNGLTEXIMAGE2DPROC glad_debug_glTexImage2D = glad_debug_impl_glTexImage2D;
PFNGLTEXIMAGE3DPROC glad_glTexImage3D = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void * pixels) {
    _pre_call_gles2_callback("glTexImage3D", (GLADapiproc) glad_glTexImage3D, 10, target, level, internalformat, width, height, depth, border, format, type, pixels);
    glad_glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
    _post_call_gles2_callback(NULL, "glTexImage3D", (GLADapiproc) glad_glTexImage3D, 10, target, level, internalformat, width, height, depth, border, format, type, pixels);
    
}
PFNGLTEXIMAGE3DPROC glad_debug_glTexImage3D = glad_debug_impl_glTexImage3D;
PFNGLTEXPARAMETERIIVPROC glad_glTexParameterIiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexParameterIiv(GLenum target, GLenum pname, const GLint * params) {
    _pre_call_gles2_callback("glTexParameterIiv", (GLADapiproc) glad_glTexParameterIiv, 3, target, pname, params);
    glad_glTexParameterIiv(target, pname, params);
    _post_call_gles2_callback(NULL, "glTexParameterIiv", (GLADapiproc) glad_glTexParameterIiv, 3, target, pname, params);
    
}
PFNGLTEXPARAMETERIIVPROC glad_debug_glTexParameterIiv = glad_debug_impl_glTexParameterIiv;
PFNGLTEXPARAMETERIUIVPROC glad_glTexParameterIuiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexParameterIuiv(GLenum target, GLenum pname, const GLuint * params) {
    _pre_call_gles2_callback("glTexParameterIuiv", (GLADapiproc) glad_glTexParameterIuiv, 3, target, pname, params);
    glad_glTexParameterIuiv(target, pname, params);
    _post_call_gles2_callback(NULL, "glTexParameterIuiv", (GLADapiproc) glad_glTexParameterIuiv, 3, target, pname, params);
    
}
PFNGLTEXPARAMETERIUIVPROC glad_debug_glTexParameterIuiv = glad_debug_impl_glTexParameterIuiv;
PFNGLTEXPARAMETERFPROC glad_glTexParameterf = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexParameterf(GLenum target, GLenum pname, GLfloat param) {
    _pre_call_gles2_callback("glTexParameterf", (GLADapiproc) glad_glTexParameterf, 3, target, pname, param);
    glad_glTexParameterf(target, pname, param);
    _post_call_gles2_callback(NULL, "glTexParameterf", (GLADapiproc) glad_glTexParameterf, 3, target, pname, param);
    
}
PFNGLTEXPARAMETERFPROC glad_debug_glTexParameterf = glad_debug_impl_glTexParameterf;
PFNGLTEXPARAMETERFVPROC glad_glTexParameterfv = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexParameterfv(GLenum target, GLenum pname, const GLfloat * params) {
    _pre_call_gles2_callback("glTexParameterfv", (GLADapiproc) glad_glTexParameterfv, 3, target, pname, params);
    glad_glTexParameterfv(target, pname, params);
    _post_call_gles2_callback(NULL, "glTexParameterfv", (GLADapiproc) glad_glTexParameterfv, 3, target, pname, params);
    
}
PFNGLTEXPARAMETERFVPROC glad_debug_glTexParameterfv = glad_debug_impl_glTexParameterfv;
PFNGLTEXPARAMETERIPROC glad_glTexParameteri = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexParameteri(GLenum target, GLenum pname, GLint param) {
    _pre_call_gles2_callback("glTexParameteri", (GLADapiproc) glad_glTexParameteri, 3, target, pname, param);
    glad_glTexParameteri(target, pname, param);
    _post_call_gles2_callback(NULL, "glTexParameteri", (GLADapiproc) glad_glTexParameteri, 3, target, pname, param);
    
}
PFNGLTEXPARAMETERIPROC glad_debug_glTexParameteri = glad_debug_impl_glTexParameteri;
PFNGLTEXPARAMETERIVPROC glad_glTexParameteriv = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexParameteriv(GLenum target, GLenum pname, const GLint * params) {
    _pre_call_gles2_callback("glTexParameteriv", (GLADapiproc) glad_glTexParameteriv, 3, target, pname, params);
    glad_glTexParameteriv(target, pname, params);
    _post_call_gles2_callback(NULL, "glTexParameteriv", (GLADapiproc) glad_glTexParameteriv, 3, target, pname, params);
    
}
PFNGLTEXPARAMETERIVPROC glad_debug_glTexParameteriv = glad_debug_impl_glTexParameteriv;
PFNGLTEXSTORAGE2DPROC glad_glTexStorage2D = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) {
    _pre_call_gles2_callback("glTexStorage2D", (GLADapiproc) glad_glTexStorage2D, 5, target, levels, internalformat, width, height);
    glad_glTexStorage2D(target, levels, internalformat, width, height);
    _post_call_gles2_callback(NULL, "glTexStorage2D", (GLADapiproc) glad_glTexStorage2D, 5, target, levels, internalformat, width, height);
    
}
PFNGLTEXSTORAGE2DPROC glad_debug_glTexStorage2D = glad_debug_impl_glTexStorage2D;
PFNGLTEXSTORAGE2DMULTISAMPLEPROC glad_glTexStorage2DMultisample = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexStorage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) {
    _pre_call_gles2_callback("glTexStorage2DMultisample", (GLADapiproc) glad_glTexStorage2DMultisample, 6, target, samples, internalformat, width, height, fixedsamplelocations);
    glad_glTexStorage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations);
    _post_call_gles2_callback(NULL, "glTexStorage2DMultisample", (GLADapiproc) glad_glTexStorage2DMultisample, 6, target, samples, internalformat, width, height, fixedsamplelocations);
    
}
PFNGLTEXSTORAGE2DMULTISAMPLEPROC glad_debug_glTexStorage2DMultisample = glad_debug_impl_glTexStorage2DMultisample;
PFNGLTEXSTORAGE3DPROC glad_glTexStorage3D = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) {
    _pre_call_gles2_callback("glTexStorage3D", (GLADapiproc) glad_glTexStorage3D, 6, target, levels, internalformat, width, height, depth);
    glad_glTexStorage3D(target, levels, internalformat, width, height, depth);
    _post_call_gles2_callback(NULL, "glTexStorage3D", (GLADapiproc) glad_glTexStorage3D, 6, target, levels, internalformat, width, height, depth);
    
}
PFNGLTEXSTORAGE3DPROC glad_debug_glTexStorage3D = glad_debug_impl_glTexStorage3D;
PFNGLTEXSTORAGE3DMULTISAMPLEPROC glad_glTexStorage3DMultisample = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexStorage3DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations) {
    _pre_call_gles2_callback("glTexStorage3DMultisample", (GLADapiproc) glad_glTexStorage3DMultisample, 7, target, samples, internalformat, width, height, depth, fixedsamplelocations);
    glad_glTexStorage3DMultisample(target, samples, internalformat, width, height, depth, fixedsamplelocations);
    _post_call_gles2_callback(NULL, "glTexStorage3DMultisample", (GLADapiproc) glad_glTexStorage3DMultisample, 7, target, samples, internalformat, width, height, depth, fixedsamplelocations);
    
}
PFNGLTEXSTORAGE3DMULTISAMPLEPROC glad_debug_glTexStorage3DMultisample = glad_debug_impl_glTexStorage3DMultisample;
PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void * pixels) {
    _pre_call_gles2_callback("glTexSubImage2D", (GLADapiproc) glad_glTexSubImage2D, 9, target, level, xoffset, yoffset, width, height, format, type, pixels);
    glad_glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
    _post_call_gles2_callback(NULL, "glTexSubImage2D", (GLADapiproc) glad_glTexSubImage2D, 9, target, level, xoffset, yoffset, width, height, format, type, pixels);
    
}
PFNGLTEXSUBIMAGE2DPROC glad_debug_glTexSubImage2D = glad_debug_impl_glTexSubImage2D;
PFNGLTEXSUBIMAGE3DPROC glad_glTexSubImage3D = NULL;
static void GLAD_API_PTR glad_debug_impl_glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void * pixels) {
    _pre_call_gles2_callback("glTexSubImage3D", (GLADapiproc) glad_glTexSubImage3D, 11, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
    glad_glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
    _post_call_gles2_callback(NULL, "glTexSubImage3D", (GLADapiproc) glad_glTexSubImage3D, 11, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
    
}
PFNGLTEXSUBIMAGE3DPROC glad_debug_glTexSubImage3D = glad_debug_impl_glTexSubImage3D;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC glad_glTransformFeedbackVaryings = NULL;
static void GLAD_API_PTR glad_debug_impl_glTransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar *const* varyings, GLenum bufferMode) {
    _pre_call_gles2_callback("glTransformFeedbackVaryings", (GLADapiproc) glad_glTransformFeedbackVaryings, 4, program, count, varyings, bufferMode);
    glad_glTransformFeedbackVaryings(program, count, varyings, bufferMode);
    _post_call_gles2_callback(NULL, "glTransformFeedbackVaryings", (GLADapiproc) glad_glTransformFeedbackVaryings, 4, program, count, varyings, bufferMode);
    
}
PFNGLTRANSFORMFEEDBACKVARYINGSPROC glad_debug_glTransformFeedbackVaryings = glad_debug_impl_glTransformFeedbackVaryings;
PFNGLUNIFORM1FPROC glad_glUniform1f = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform1f(GLint location, GLfloat v0) {
    _pre_call_gles2_callback("glUniform1f", (GLADapiproc) glad_glUniform1f, 2, location, v0);
    glad_glUniform1f(location, v0);
    _post_call_gles2_callback(NULL, "glUniform1f", (GLADapiproc) glad_glUniform1f, 2, location, v0);
    
}
PFNGLUNIFORM1FPROC glad_debug_glUniform1f = glad_debug_impl_glUniform1f;
PFNGLUNIFORM1FVPROC glad_glUniform1fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform1fv(GLint location, GLsizei count, const GLfloat * value) {
    _pre_call_gles2_callback("glUniform1fv", (GLADapiproc) glad_glUniform1fv, 3, location, count, value);
    glad_glUniform1fv(location, count, value);
    _post_call_gles2_callback(NULL, "glUniform1fv", (GLADapiproc) glad_glUniform1fv, 3, location, count, value);
    
}
PFNGLUNIFORM1FVPROC glad_debug_glUniform1fv = glad_debug_impl_glUniform1fv;
PFNGLUNIFORM1IPROC glad_glUniform1i = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform1i(GLint location, GLint v0) {
    _pre_call_gles2_callback("glUniform1i", (GLADapiproc) glad_glUniform1i, 2, location, v0);
    glad_glUniform1i(location, v0);
    _post_call_gles2_callback(NULL, "glUniform1i", (GLADapiproc) glad_glUniform1i, 2, location, v0);
    
}
PFNGLUNIFORM1IPROC glad_debug_glUniform1i = glad_debug_impl_glUniform1i;
PFNGLUNIFORM1IVPROC glad_glUniform1iv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform1iv(GLint location, GLsizei count, const GLint * value) {
    _pre_call_gles2_callback("glUniform1iv", (GLADapiproc) glad_glUniform1iv, 3, location, count, value);
    glad_glUniform1iv(location, count, value);
    _post_call_gles2_callback(NULL, "glUniform1iv", (GLADapiproc) glad_glUniform1iv, 3, location, count, value);
    
}
PFNGLUNIFORM1IVPROC glad_debug_glUniform1iv = glad_debug_impl_glUniform1iv;
PFNGLUNIFORM1UIPROC glad_glUniform1ui = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform1ui(GLint location, GLuint v0) {
    _pre_call_gles2_callback("glUniform1ui", (GLADapiproc) glad_glUniform1ui, 2, location, v0);
    glad_glUniform1ui(location, v0);
    _post_call_gles2_callback(NULL, "glUniform1ui", (GLADapiproc) glad_glUniform1ui, 2, location, v0);
    
}
PFNGLUNIFORM1UIPROC glad_debug_glUniform1ui = glad_debug_impl_glUniform1ui;
PFNGLUNIFORM1UIVPROC glad_glUniform1uiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform1uiv(GLint location, GLsizei count, const GLuint * value) {
    _pre_call_gles2_callback("glUniform1uiv", (GLADapiproc) glad_glUniform1uiv, 3, location, count, value);
    glad_glUniform1uiv(location, count, value);
    _post_call_gles2_callback(NULL, "glUniform1uiv", (GLADapiproc) glad_glUniform1uiv, 3, location, count, value);
    
}
PFNGLUNIFORM1UIVPROC glad_debug_glUniform1uiv = glad_debug_impl_glUniform1uiv;
PFNGLUNIFORM2FPROC glad_glUniform2f = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform2f(GLint location, GLfloat v0, GLfloat v1) {
    _pre_call_gles2_callback("glUniform2f", (GLADapiproc) glad_glUniform2f, 3, location, v0, v1);
    glad_glUniform2f(location, v0, v1);
    _post_call_gles2_callback(NULL, "glUniform2f", (GLADapiproc) glad_glUniform2f, 3, location, v0, v1);
    
}
PFNGLUNIFORM2FPROC glad_debug_glUniform2f = glad_debug_impl_glUniform2f;
PFNGLUNIFORM2FVPROC glad_glUniform2fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform2fv(GLint location, GLsizei count, const GLfloat * value) {
    _pre_call_gles2_callback("glUniform2fv", (GLADapiproc) glad_glUniform2fv, 3, location, count, value);
    glad_glUniform2fv(location, count, value);
    _post_call_gles2_callback(NULL, "glUniform2fv", (GLADapiproc) glad_glUniform2fv, 3, location, count, value);
    
}
PFNGLUNIFORM2FVPROC glad_debug_glUniform2fv = glad_debug_impl_glUniform2fv;
PFNGLUNIFORM2IPROC glad_glUniform2i = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform2i(GLint location, GLint v0, GLint v1) {
    _pre_call_gles2_callback("glUniform2i", (GLADapiproc) glad_glUniform2i, 3, location, v0, v1);
    glad_glUniform2i(location, v0, v1);
    _post_call_gles2_callback(NULL, "glUniform2i", (GLADapiproc) glad_glUniform2i, 3, location, v0, v1);
    
}
PFNGLUNIFORM2IPROC glad_debug_glUniform2i = glad_debug_impl_glUniform2i;
PFNGLUNIFORM2IVPROC glad_glUniform2iv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform2iv(GLint location, GLsizei count, const GLint * value) {
    _pre_call_gles2_callback("glUniform2iv", (GLADapiproc) glad_glUniform2iv, 3, location, count, value);
    glad_glUniform2iv(location, count, value);
    _post_call_gles2_callback(NULL, "glUniform2iv", (GLADapiproc) glad_glUniform2iv, 3, location, count, value);
    
}
PFNGLUNIFORM2IVPROC glad_debug_glUniform2iv = glad_debug_impl_glUniform2iv;
PFNGLUNIFORM2UIPROC glad_glUniform2ui = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform2ui(GLint location, GLuint v0, GLuint v1) {
    _pre_call_gles2_callback("glUniform2ui", (GLADapiproc) glad_glUniform2ui, 3, location, v0, v1);
    glad_glUniform2ui(location, v0, v1);
    _post_call_gles2_callback(NULL, "glUniform2ui", (GLADapiproc) glad_glUniform2ui, 3, location, v0, v1);
    
}
PFNGLUNIFORM2UIPROC glad_debug_glUniform2ui = glad_debug_impl_glUniform2ui;
PFNGLUNIFORM2UIVPROC glad_glUniform2uiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform2uiv(GLint location, GLsizei count, const GLuint * value) {
    _pre_call_gles2_callback("glUniform2uiv", (GLADapiproc) glad_glUniform2uiv, 3, location, count, value);
    glad_glUniform2uiv(location, count, value);
    _post_call_gles2_callback(NULL, "glUniform2uiv", (GLADapiproc) glad_glUniform2uiv, 3, location, count, value);
    
}
PFNGLUNIFORM2UIVPROC glad_debug_glUniform2uiv = glad_debug_impl_glUniform2uiv;
PFNGLUNIFORM3FPROC glad_glUniform3f = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) {
    _pre_call_gles2_callback("glUniform3f", (GLADapiproc) glad_glUniform3f, 4, location, v0, v1, v2);
    glad_glUniform3f(location, v0, v1, v2);
    _post_call_gles2_callback(NULL, "glUniform3f", (GLADapiproc) glad_glUniform3f, 4, location, v0, v1, v2);
    
}
PFNGLUNIFORM3FPROC glad_debug_glUniform3f = glad_debug_impl_glUniform3f;
PFNGLUNIFORM3FVPROC glad_glUniform3fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform3fv(GLint location, GLsizei count, const GLfloat * value) {
    _pre_call_gles2_callback("glUniform3fv", (GLADapiproc) glad_glUniform3fv, 3, location, count, value);
    glad_glUniform3fv(location, count, value);
    _post_call_gles2_callback(NULL, "glUniform3fv", (GLADapiproc) glad_glUniform3fv, 3, location, count, value);
    
}
PFNGLUNIFORM3FVPROC glad_debug_glUniform3fv = glad_debug_impl_glUniform3fv;
PFNGLUNIFORM3IPROC glad_glUniform3i = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform3i(GLint location, GLint v0, GLint v1, GLint v2) {
    _pre_call_gles2_callback("glUniform3i", (GLADapiproc) glad_glUniform3i, 4, location, v0, v1, v2);
    glad_glUniform3i(location, v0, v1, v2);
    _post_call_gles2_callback(NULL, "glUniform3i", (GLADapiproc) glad_glUniform3i, 4, location, v0, v1, v2);
    
}
PFNGLUNIFORM3IPROC glad_debug_glUniform3i = glad_debug_impl_glUniform3i;
PFNGLUNIFORM3IVPROC glad_glUniform3iv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform3iv(GLint location, GLsizei count, const GLint * value) {
    _pre_call_gles2_callback("glUniform3iv", (GLADapiproc) glad_glUniform3iv, 3, location, count, value);
    glad_glUniform3iv(location, count, value);
    _post_call_gles2_callback(NULL, "glUniform3iv", (GLADapiproc) glad_glUniform3iv, 3, location, count, value);
    
}
PFNGLUNIFORM3IVPROC glad_debug_glUniform3iv = glad_debug_impl_glUniform3iv;
PFNGLUNIFORM3UIPROC glad_glUniform3ui = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2) {
    _pre_call_gles2_callback("glUniform3ui", (GLADapiproc) glad_glUniform3ui, 4, location, v0, v1, v2);
    glad_glUniform3ui(location, v0, v1, v2);
    _post_call_gles2_callback(NULL, "glUniform3ui", (GLADapiproc) glad_glUniform3ui, 4, location, v0, v1, v2);
    
}
PFNGLUNIFORM3UIPROC glad_debug_glUniform3ui = glad_debug_impl_glUniform3ui;
PFNGLUNIFORM3UIVPROC glad_glUniform3uiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform3uiv(GLint location, GLsizei count, const GLuint * value) {
    _pre_call_gles2_callback("glUniform3uiv", (GLADapiproc) glad_glUniform3uiv, 3, location, count, value);
    glad_glUniform3uiv(location, count, value);
    _post_call_gles2_callback(NULL, "glUniform3uiv", (GLADapiproc) glad_glUniform3uiv, 3, location, count, value);
    
}
PFNGLUNIFORM3UIVPROC glad_debug_glUniform3uiv = glad_debug_impl_glUniform3uiv;
PFNGLUNIFORM4FPROC glad_glUniform4f = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
    _pre_call_gles2_callback("glUniform4f", (GLADapiproc) glad_glUniform4f, 5, location, v0, v1, v2, v3);
    glad_glUniform4f(location, v0, v1, v2, v3);
    _post_call_gles2_callback(NULL, "glUniform4f", (GLADapiproc) glad_glUniform4f, 5, location, v0, v1, v2, v3);
    
}
PFNGLUNIFORM4FPROC glad_debug_glUniform4f = glad_debug_impl_glUniform4f;
PFNGLUNIFORM4FVPROC glad_glUniform4fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform4fv(GLint location, GLsizei count, const GLfloat * value) {
    _pre_call_gles2_callback("glUniform4fv", (GLADapiproc) glad_glUniform4fv, 3, location, count, value);
    glad_glUniform4fv(location, count, value);
    _post_call_gles2_callback(NULL, "glUniform4fv", (GLADapiproc) glad_glUniform4fv, 3, location, count, value);
    
}
PFNGLUNIFORM4FVPROC glad_debug_glUniform4fv = glad_debug_impl_glUniform4fv;
PFNGLUNIFORM4IPROC glad_glUniform4i = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) {
    _pre_call_gles2_callback("glUniform4i", (GLADapiproc) glad_glUniform4i, 5, location, v0, v1, v2, v3);
    glad_glUniform4i(location, v0, v1, v2, v3);
    _post_call_gles2_callback(NULL, "glUniform4i", (GLADapiproc) glad_glUniform4i, 5, location, v0, v1, v2, v3);
    
}
PFNGLUNIFORM4IPROC glad_debug_glUniform4i = glad_debug_impl_glUniform4i;
PFNGLUNIFORM4IVPROC glad_glUniform4iv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform4iv(GLint location, GLsizei count, const GLint * value) {
    _pre_call_gles2_callback("glUniform4iv", (GLADapiproc) glad_glUniform4iv, 3, location, count, value);
    glad_glUniform4iv(location, count, value);
    _post_call_gles2_callback(NULL, "glUniform4iv", (GLADapiproc) glad_glUniform4iv, 3, location, count, value);
    
}
PFNGLUNIFORM4IVPROC glad_debug_glUniform4iv = glad_debug_impl_glUniform4iv;
PFNGLUNIFORM4UIPROC glad_glUniform4ui = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) {
    _pre_call_gles2_callback("glUniform4ui", (GLADapiproc) glad_glUniform4ui, 5, location, v0, v1, v2, v3);
    glad_glUniform4ui(location, v0, v1, v2, v3);
    _post_call_gles2_callback(NULL, "glUniform4ui", (GLADapiproc) glad_glUniform4ui, 5, location, v0, v1, v2, v3);
    
}
PFNGLUNIFORM4UIPROC glad_debug_glUniform4ui = glad_debug_impl_glUniform4ui;
PFNGLUNIFORM4UIVPROC glad_glUniform4uiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniform4uiv(GLint location, GLsizei count, const GLuint * value) {
    _pre_call_gles2_callback("glUniform4uiv", (GLADapiproc) glad_glUniform4uiv, 3, location, count, value);
    glad_glUniform4uiv(location, count, value);
    _post_call_gles2_callback(NULL, "glUniform4uiv", (GLADapiproc) glad_glUniform4uiv, 3, location, count, value);
    
}
PFNGLUNIFORM4UIVPROC glad_debug_glUniform4uiv = glad_debug_impl_glUniform4uiv;
PFNGLUNIFORMBLOCKBINDINGPROC glad_glUniformBlockBinding = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding) {
    _pre_call_gles2_callback("glUniformBlockBinding", (GLADapiproc) glad_glUniformBlockBinding, 3, program, uniformBlockIndex, uniformBlockBinding);
    glad_glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
    _post_call_gles2_callback(NULL, "glUniformBlockBinding", (GLADapiproc) glad_glUniformBlockBinding, 3, program, uniformBlockIndex, uniformBlockBinding);
    
}
PFNGLUNIFORMBLOCKBINDINGPROC glad_debug_glUniformBlockBinding = glad_debug_impl_glUniformBlockBinding;
PFNGLUNIFORMMATRIX2FVPROC glad_glUniformMatrix2fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glUniformMatrix2fv", (GLADapiproc) glad_glUniformMatrix2fv, 4, location, count, transpose, value);
    glad_glUniformMatrix2fv(location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glUniformMatrix2fv", (GLADapiproc) glad_glUniformMatrix2fv, 4, location, count, transpose, value);
    
}
PFNGLUNIFORMMATRIX2FVPROC glad_debug_glUniformMatrix2fv = glad_debug_impl_glUniformMatrix2fv;
PFNGLUNIFORMMATRIX2X3FVPROC glad_glUniformMatrix2x3fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glUniformMatrix2x3fv", (GLADapiproc) glad_glUniformMatrix2x3fv, 4, location, count, transpose, value);
    glad_glUniformMatrix2x3fv(location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glUniformMatrix2x3fv", (GLADapiproc) glad_glUniformMatrix2x3fv, 4, location, count, transpose, value);
    
}
PFNGLUNIFORMMATRIX2X3FVPROC glad_debug_glUniformMatrix2x3fv = glad_debug_impl_glUniformMatrix2x3fv;
PFNGLUNIFORMMATRIX2X4FVPROC glad_glUniformMatrix2x4fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glUniformMatrix2x4fv", (GLADapiproc) glad_glUniformMatrix2x4fv, 4, location, count, transpose, value);
    glad_glUniformMatrix2x4fv(location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glUniformMatrix2x4fv", (GLADapiproc) glad_glUniformMatrix2x4fv, 4, location, count, transpose, value);
    
}
PFNGLUNIFORMMATRIX2X4FVPROC glad_debug_glUniformMatrix2x4fv = glad_debug_impl_glUniformMatrix2x4fv;
PFNGLUNIFORMMATRIX3FVPROC glad_glUniformMatrix3fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glUniformMatrix3fv", (GLADapiproc) glad_glUniformMatrix3fv, 4, location, count, transpose, value);
    glad_glUniformMatrix3fv(location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glUniformMatrix3fv", (GLADapiproc) glad_glUniformMatrix3fv, 4, location, count, transpose, value);
    
}
PFNGLUNIFORMMATRIX3FVPROC glad_debug_glUniformMatrix3fv = glad_debug_impl_glUniformMatrix3fv;
PFNGLUNIFORMMATRIX3X2FVPROC glad_glUniformMatrix3x2fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glUniformMatrix3x2fv", (GLADapiproc) glad_glUniformMatrix3x2fv, 4, location, count, transpose, value);
    glad_glUniformMatrix3x2fv(location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glUniformMatrix3x2fv", (GLADapiproc) glad_glUniformMatrix3x2fv, 4, location, count, transpose, value);
    
}
PFNGLUNIFORMMATRIX3X2FVPROC glad_debug_glUniformMatrix3x2fv = glad_debug_impl_glUniformMatrix3x2fv;
PFNGLUNIFORMMATRIX3X4FVPROC glad_glUniformMatrix3x4fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glUniformMatrix3x4fv", (GLADapiproc) glad_glUniformMatrix3x4fv, 4, location, count, transpose, value);
    glad_glUniformMatrix3x4fv(location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glUniformMatrix3x4fv", (GLADapiproc) glad_glUniformMatrix3x4fv, 4, location, count, transpose, value);
    
}
PFNGLUNIFORMMATRIX3X4FVPROC glad_debug_glUniformMatrix3x4fv = glad_debug_impl_glUniformMatrix3x4fv;
PFNGLUNIFORMMATRIX4FVPROC glad_glUniformMatrix4fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glUniformMatrix4fv", (GLADapiproc) glad_glUniformMatrix4fv, 4, location, count, transpose, value);
    glad_glUniformMatrix4fv(location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glUniformMatrix4fv", (GLADapiproc) glad_glUniformMatrix4fv, 4, location, count, transpose, value);
    
}
PFNGLUNIFORMMATRIX4FVPROC glad_debug_glUniformMatrix4fv = glad_debug_impl_glUniformMatrix4fv;
PFNGLUNIFORMMATRIX4X2FVPROC glad_glUniformMatrix4x2fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glUniformMatrix4x2fv", (GLADapiproc) glad_glUniformMatrix4x2fv, 4, location, count, transpose, value);
    glad_glUniformMatrix4x2fv(location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glUniformMatrix4x2fv", (GLADapiproc) glad_glUniformMatrix4x2fv, 4, location, count, transpose, value);
    
}
PFNGLUNIFORMMATRIX4X2FVPROC glad_debug_glUniformMatrix4x2fv = glad_debug_impl_glUniformMatrix4x2fv;
PFNGLUNIFORMMATRIX4X3FVPROC glad_glUniformMatrix4x3fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glUniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value) {
    _pre_call_gles2_callback("glUniformMatrix4x3fv", (GLADapiproc) glad_glUniformMatrix4x3fv, 4, location, count, transpose, value);
    glad_glUniformMatrix4x3fv(location, count, transpose, value);
    _post_call_gles2_callback(NULL, "glUniformMatrix4x3fv", (GLADapiproc) glad_glUniformMatrix4x3fv, 4, location, count, transpose, value);
    
}
PFNGLUNIFORMMATRIX4X3FVPROC glad_debug_glUniformMatrix4x3fv = glad_debug_impl_glUniformMatrix4x3fv;
PFNGLUNMAPBUFFERPROC glad_glUnmapBuffer = NULL;
static GLboolean GLAD_API_PTR glad_debug_impl_glUnmapBuffer(GLenum target) {
    GLboolean ret;
    _pre_call_gles2_callback("glUnmapBuffer", (GLADapiproc) glad_glUnmapBuffer, 1, target);
    ret = glad_glUnmapBuffer(target);
    _post_call_gles2_callback((void*) &ret, "glUnmapBuffer", (GLADapiproc) glad_glUnmapBuffer, 1, target);
    return ret;
}
PFNGLUNMAPBUFFERPROC glad_debug_glUnmapBuffer = glad_debug_impl_glUnmapBuffer;
PFNGLUSEPROGRAMPROC glad_glUseProgram = NULL;
static void GLAD_API_PTR glad_debug_impl_glUseProgram(GLuint program) {
    _pre_call_gles2_callback("glUseProgram", (GLADapiproc) glad_glUseProgram, 1, program);
    glad_glUseProgram(program);
    _post_call_gles2_callback(NULL, "glUseProgram", (GLADapiproc) glad_glUseProgram, 1, program);
    
}
PFNGLUSEPROGRAMPROC glad_debug_glUseProgram = glad_debug_impl_glUseProgram;
PFNGLUSEPROGRAMSTAGESPROC glad_glUseProgramStages = NULL;
static void GLAD_API_PTR glad_debug_impl_glUseProgramStages(GLuint pipeline, GLbitfield stages, GLuint program) {
    _pre_call_gles2_callback("glUseProgramStages", (GLADapiproc) glad_glUseProgramStages, 3, pipeline, stages, program);
    glad_glUseProgramStages(pipeline, stages, program);
    _post_call_gles2_callback(NULL, "glUseProgramStages", (GLADapiproc) glad_glUseProgramStages, 3, pipeline, stages, program);
    
}
PFNGLUSEPROGRAMSTAGESPROC glad_debug_glUseProgramStages = glad_debug_impl_glUseProgramStages;
PFNGLVALIDATEPROGRAMPROC glad_glValidateProgram = NULL;
static void GLAD_API_PTR glad_debug_impl_glValidateProgram(GLuint program) {
    _pre_call_gles2_callback("glValidateProgram", (GLADapiproc) glad_glValidateProgram, 1, program);
    glad_glValidateProgram(program);
    _post_call_gles2_callback(NULL, "glValidateProgram", (GLADapiproc) glad_glValidateProgram, 1, program);
    
}
PFNGLVALIDATEPROGRAMPROC glad_debug_glValidateProgram = glad_debug_impl_glValidateProgram;
PFNGLVALIDATEPROGRAMPIPELINEPROC glad_glValidateProgramPipeline = NULL;
static void GLAD_API_PTR glad_debug_impl_glValidateProgramPipeline(GLuint pipeline) {
    _pre_call_gles2_callback("glValidateProgramPipeline", (GLADapiproc) glad_glValidateProgramPipeline, 1, pipeline);
    glad_glValidateProgramPipeline(pipeline);
    _post_call_gles2_callback(NULL, "glValidateProgramPipeline", (GLADapiproc) glad_glValidateProgramPipeline, 1, pipeline);
    
}
PFNGLVALIDATEPROGRAMPIPELINEPROC glad_debug_glValidateProgramPipeline = glad_debug_impl_glValidateProgramPipeline;
PFNGLVERTEXATTRIB1FPROC glad_glVertexAttrib1f = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttrib1f(GLuint index, GLfloat x) {
    _pre_call_gles2_callback("glVertexAttrib1f", (GLADapiproc) glad_glVertexAttrib1f, 2, index, x);
    glad_glVertexAttrib1f(index, x);
    _post_call_gles2_callback(NULL, "glVertexAttrib1f", (GLADapiproc) glad_glVertexAttrib1f, 2, index, x);
    
}
PFNGLVERTEXATTRIB1FPROC glad_debug_glVertexAttrib1f = glad_debug_impl_glVertexAttrib1f;
PFNGLVERTEXATTRIB1FVPROC glad_glVertexAttrib1fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttrib1fv(GLuint index, const GLfloat * v) {
    _pre_call_gles2_callback("glVertexAttrib1fv", (GLADapiproc) glad_glVertexAttrib1fv, 2, index, v);
    glad_glVertexAttrib1fv(index, v);
    _post_call_gles2_callback(NULL, "glVertexAttrib1fv", (GLADapiproc) glad_glVertexAttrib1fv, 2, index, v);
    
}
PFNGLVERTEXATTRIB1FVPROC glad_debug_glVertexAttrib1fv = glad_debug_impl_glVertexAttrib1fv;
PFNGLVERTEXATTRIB2FPROC glad_glVertexAttrib2f = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y) {
    _pre_call_gles2_callback("glVertexAttrib2f", (GLADapiproc) glad_glVertexAttrib2f, 3, index, x, y);
    glad_glVertexAttrib2f(index, x, y);
    _post_call_gles2_callback(NULL, "glVertexAttrib2f", (GLADapiproc) glad_glVertexAttrib2f, 3, index, x, y);
    
}
PFNGLVERTEXATTRIB2FPROC glad_debug_glVertexAttrib2f = glad_debug_impl_glVertexAttrib2f;
PFNGLVERTEXATTRIB2FVPROC glad_glVertexAttrib2fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttrib2fv(GLuint index, const GLfloat * v) {
    _pre_call_gles2_callback("glVertexAttrib2fv", (GLADapiproc) glad_glVertexAttrib2fv, 2, index, v);
    glad_glVertexAttrib2fv(index, v);
    _post_call_gles2_callback(NULL, "glVertexAttrib2fv", (GLADapiproc) glad_glVertexAttrib2fv, 2, index, v);
    
}
PFNGLVERTEXATTRIB2FVPROC glad_debug_glVertexAttrib2fv = glad_debug_impl_glVertexAttrib2fv;
PFNGLVERTEXATTRIB3FPROC glad_glVertexAttrib3f = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z) {
    _pre_call_gles2_callback("glVertexAttrib3f", (GLADapiproc) glad_glVertexAttrib3f, 4, index, x, y, z);
    glad_glVertexAttrib3f(index, x, y, z);
    _post_call_gles2_callback(NULL, "glVertexAttrib3f", (GLADapiproc) glad_glVertexAttrib3f, 4, index, x, y, z);
    
}
PFNGLVERTEXATTRIB3FPROC glad_debug_glVertexAttrib3f = glad_debug_impl_glVertexAttrib3f;
PFNGLVERTEXATTRIB3FVPROC glad_glVertexAttrib3fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttrib3fv(GLuint index, const GLfloat * v) {
    _pre_call_gles2_callback("glVertexAttrib3fv", (GLADapiproc) glad_glVertexAttrib3fv, 2, index, v);
    glad_glVertexAttrib3fv(index, v);
    _post_call_gles2_callback(NULL, "glVertexAttrib3fv", (GLADapiproc) glad_glVertexAttrib3fv, 2, index, v);
    
}
PFNGLVERTEXATTRIB3FVPROC glad_debug_glVertexAttrib3fv = glad_debug_impl_glVertexAttrib3fv;
PFNGLVERTEXATTRIB4FPROC glad_glVertexAttrib4f = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    _pre_call_gles2_callback("glVertexAttrib4f", (GLADapiproc) glad_glVertexAttrib4f, 5, index, x, y, z, w);
    glad_glVertexAttrib4f(index, x, y, z, w);
    _post_call_gles2_callback(NULL, "glVertexAttrib4f", (GLADapiproc) glad_glVertexAttrib4f, 5, index, x, y, z, w);
    
}
PFNGLVERTEXATTRIB4FPROC glad_debug_glVertexAttrib4f = glad_debug_impl_glVertexAttrib4f;
PFNGLVERTEXATTRIB4FVPROC glad_glVertexAttrib4fv = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttrib4fv(GLuint index, const GLfloat * v) {
    _pre_call_gles2_callback("glVertexAttrib4fv", (GLADapiproc) glad_glVertexAttrib4fv, 2, index, v);
    glad_glVertexAttrib4fv(index, v);
    _post_call_gles2_callback(NULL, "glVertexAttrib4fv", (GLADapiproc) glad_glVertexAttrib4fv, 2, index, v);
    
}
PFNGLVERTEXATTRIB4FVPROC glad_debug_glVertexAttrib4fv = glad_debug_impl_glVertexAttrib4fv;
PFNGLVERTEXATTRIBBINDINGPROC glad_glVertexAttribBinding = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttribBinding(GLuint attribindex, GLuint bindingindex) {
    _pre_call_gles2_callback("glVertexAttribBinding", (GLADapiproc) glad_glVertexAttribBinding, 2, attribindex, bindingindex);
    glad_glVertexAttribBinding(attribindex, bindingindex);
    _post_call_gles2_callback(NULL, "glVertexAttribBinding", (GLADapiproc) glad_glVertexAttribBinding, 2, attribindex, bindingindex);
    
}
PFNGLVERTEXATTRIBBINDINGPROC glad_debug_glVertexAttribBinding = glad_debug_impl_glVertexAttribBinding;
PFNGLVERTEXATTRIBDIVISORPROC glad_glVertexAttribDivisor = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttribDivisor(GLuint index, GLuint divisor) {
    _pre_call_gles2_callback("glVertexAttribDivisor", (GLADapiproc) glad_glVertexAttribDivisor, 2, index, divisor);
    glad_glVertexAttribDivisor(index, divisor);
    _post_call_gles2_callback(NULL, "glVertexAttribDivisor", (GLADapiproc) glad_glVertexAttribDivisor, 2, index, divisor);
    
}
PFNGLVERTEXATTRIBDIVISORPROC glad_debug_glVertexAttribDivisor = glad_debug_impl_glVertexAttribDivisor;
PFNGLVERTEXATTRIBFORMATPROC glad_glVertexAttribFormat = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttribFormat(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset) {
    _pre_call_gles2_callback("glVertexAttribFormat", (GLADapiproc) glad_glVertexAttribFormat, 5, attribindex, size, type, normalized, relativeoffset);
    glad_glVertexAttribFormat(attribindex, size, type, normalized, relativeoffset);
    _post_call_gles2_callback(NULL, "glVertexAttribFormat", (GLADapiproc) glad_glVertexAttribFormat, 5, attribindex, size, type, normalized, relativeoffset);
    
}
PFNGLVERTEXATTRIBFORMATPROC glad_debug_glVertexAttribFormat = glad_debug_impl_glVertexAttribFormat;
PFNGLVERTEXATTRIBI4IPROC glad_glVertexAttribI4i = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w) {
    _pre_call_gles2_callback("glVertexAttribI4i", (GLADapiproc) glad_glVertexAttribI4i, 5, index, x, y, z, w);
    glad_glVertexAttribI4i(index, x, y, z, w);
    _post_call_gles2_callback(NULL, "glVertexAttribI4i", (GLADapiproc) glad_glVertexAttribI4i, 5, index, x, y, z, w);
    
}
PFNGLVERTEXATTRIBI4IPROC glad_debug_glVertexAttribI4i = glad_debug_impl_glVertexAttribI4i;
PFNGLVERTEXATTRIBI4IVPROC glad_glVertexAttribI4iv = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttribI4iv(GLuint index, const GLint * v) {
    _pre_call_gles2_callback("glVertexAttribI4iv", (GLADapiproc) glad_glVertexAttribI4iv, 2, index, v);
    glad_glVertexAttribI4iv(index, v);
    _post_call_gles2_callback(NULL, "glVertexAttribI4iv", (GLADapiproc) glad_glVertexAttribI4iv, 2, index, v);
    
}
PFNGLVERTEXATTRIBI4IVPROC glad_debug_glVertexAttribI4iv = glad_debug_impl_glVertexAttribI4iv;
PFNGLVERTEXATTRIBI4UIPROC glad_glVertexAttribI4ui = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w) {
    _pre_call_gles2_callback("glVertexAttribI4ui", (GLADapiproc) glad_glVertexAttribI4ui, 5, index, x, y, z, w);
    glad_glVertexAttribI4ui(index, x, y, z, w);
    _post_call_gles2_callback(NULL, "glVertexAttribI4ui", (GLADapiproc) glad_glVertexAttribI4ui, 5, index, x, y, z, w);
    
}
PFNGLVERTEXATTRIBI4UIPROC glad_debug_glVertexAttribI4ui = glad_debug_impl_glVertexAttribI4ui;
PFNGLVERTEXATTRIBI4UIVPROC glad_glVertexAttribI4uiv = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttribI4uiv(GLuint index, const GLuint * v) {
    _pre_call_gles2_callback("glVertexAttribI4uiv", (GLADapiproc) glad_glVertexAttribI4uiv, 2, index, v);
    glad_glVertexAttribI4uiv(index, v);
    _post_call_gles2_callback(NULL, "glVertexAttribI4uiv", (GLADapiproc) glad_glVertexAttribI4uiv, 2, index, v);
    
}
PFNGLVERTEXATTRIBI4UIVPROC glad_debug_glVertexAttribI4uiv = glad_debug_impl_glVertexAttribI4uiv;
PFNGLVERTEXATTRIBIFORMATPROC glad_glVertexAttribIFormat = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttribIFormat(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) {
    _pre_call_gles2_callback("glVertexAttribIFormat", (GLADapiproc) glad_glVertexAttribIFormat, 4, attribindex, size, type, relativeoffset);
    glad_glVertexAttribIFormat(attribindex, size, type, relativeoffset);
    _post_call_gles2_callback(NULL, "glVertexAttribIFormat", (GLADapiproc) glad_glVertexAttribIFormat, 4, attribindex, size, type, relativeoffset);
    
}
PFNGLVERTEXATTRIBIFORMATPROC glad_debug_glVertexAttribIFormat = glad_debug_impl_glVertexAttribIFormat;
PFNGLVERTEXATTRIBIPOINTERPROC glad_glVertexAttribIPointer = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void * pointer) {
    _pre_call_gles2_callback("glVertexAttribIPointer", (GLADapiproc) glad_glVertexAttribIPointer, 5, index, size, type, stride, pointer);
    glad_glVertexAttribIPointer(index, size, type, stride, pointer);
    _post_call_gles2_callback(NULL, "glVertexAttribIPointer", (GLADapiproc) glad_glVertexAttribIPointer, 5, index, size, type, stride, pointer);
    
}
PFNGLVERTEXATTRIBIPOINTERPROC glad_debug_glVertexAttribIPointer = glad_debug_impl_glVertexAttribIPointer;
PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * pointer) {
    _pre_call_gles2_callback("glVertexAttribPointer", (GLADapiproc) glad_glVertexAttribPointer, 6, index, size, type, normalized, stride, pointer);
    glad_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    _post_call_gles2_callback(NULL, "glVertexAttribPointer", (GLADapiproc) glad_glVertexAttribPointer, 6, index, size, type, normalized, stride, pointer);
    
}
PFNGLVERTEXATTRIBPOINTERPROC glad_debug_glVertexAttribPointer = glad_debug_impl_glVertexAttribPointer;
PFNGLVERTEXBINDINGDIVISORPROC glad_glVertexBindingDivisor = NULL;
static void GLAD_API_PTR glad_debug_impl_glVertexBindingDivisor(GLuint bindingindex, GLuint divisor) {
    _pre_call_gles2_callback("glVertexBindingDivisor", (GLADapiproc) glad_glVertexBindingDivisor, 2, bindingindex, divisor);
    glad_glVertexBindingDivisor(bindingindex, divisor);
    _post_call_gles2_callback(NULL, "glVertexBindingDivisor", (GLADapiproc) glad_glVertexBindingDivisor, 2, bindingindex, divisor);
    
}
PFNGLVERTEXBINDINGDIVISORPROC glad_debug_glVertexBindingDivisor = glad_debug_impl_glVertexBindingDivisor;
PFNGLVIEWPORTPROC glad_glViewport = NULL;
static void GLAD_API_PTR glad_debug_impl_glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    _pre_call_gles2_callback("glViewport", (GLADapiproc) glad_glViewport, 4, x, y, width, height);
    glad_glViewport(x, y, width, height);
    _post_call_gles2_callback(NULL, "glViewport", (GLADapiproc) glad_glViewport, 4, x, y, width, height);
    
}
PFNGLVIEWPORTPROC glad_debug_glViewport = glad_debug_impl_glViewport;
PFNGLWAITSYNCPROC glad_glWaitSync = NULL;
static void GLAD_API_PTR glad_debug_impl_glWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout) {
    _pre_call_gles2_callback("glWaitSync", (GLADapiproc) glad_glWaitSync, 3, sync, flags, timeout);
    glad_glWaitSync(sync, flags, timeout);
    _post_call_gles2_callback(NULL, "glWaitSync", (GLADapiproc) glad_glWaitSync, 3, sync, flags, timeout);
    
}
PFNGLWAITSYNCPROC glad_debug_glWaitSync = glad_debug_impl_glWaitSync;


static void glad_gl_load_GL_ES_VERSION_2_0( GLADuserptrloadfunc load, void* userptr) {
    if(!GLAD_GL_ES_VERSION_2_0) return;
    glad_glActiveTexture = (PFNGLACTIVETEXTUREPROC) load(userptr, "glActiveTexture");
    glad_glAttachShader = (PFNGLATTACHSHADERPROC) load(userptr, "glAttachShader");
    glad_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) load(userptr, "glBindAttribLocation");
    glad_glBindBuffer = (PFNGLBINDBUFFERPROC) load(userptr, "glBindBuffer");
    glad_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) load(userptr, "glBindFramebuffer");
    glad_glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) load(userptr, "glBindRenderbuffer");
    glad_glBindTexture = (PFNGLBINDTEXTUREPROC) load(userptr, "glBindTexture");
    glad_glBlendColor = (PFNGLBLENDCOLORPROC) load(userptr, "glBlendColor");
    glad_glBlendEquation = (PFNGLBLENDEQUATIONPROC) load(userptr, "glBlendEquation");
    glad_glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC) load(userptr, "glBlendEquationSeparate");
    glad_glBlendFunc = (PFNGLBLENDFUNCPROC) load(userptr, "glBlendFunc");
    glad_glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) load(userptr, "glBlendFuncSeparate");
    glad_glBufferData = (PFNGLBUFFERDATAPROC) load(userptr, "glBufferData");
    glad_glBufferSubData = (PFNGLBUFFERSUBDATAPROC) load(userptr, "glBufferSubData");
    glad_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) load(userptr, "glCheckFramebufferStatus");
    glad_glClear = (PFNGLCLEARPROC) load(userptr, "glClear");
    glad_glClearColor = (PFNGLCLEARCOLORPROC) load(userptr, "glClearColor");
    glad_glClearDepthf = (PFNGLCLEARDEPTHFPROC) load(userptr, "glClearDepthf");
    glad_glClearStencil = (PFNGLCLEARSTENCILPROC) load(userptr, "glClearStencil");
    glad_glColorMask = (PFNGLCOLORMASKPROC) load(userptr, "glColorMask");
    glad_glCompileShader = (PFNGLCOMPILESHADERPROC) load(userptr, "glCompileShader");
    glad_glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) load(userptr, "glCompressedTexImage2D");
    glad_glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) load(userptr, "glCompressedTexSubImage2D");
    glad_glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC) load(userptr, "glCopyTexImage2D");
    glad_glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC) load(userptr, "glCopyTexSubImage2D");
    glad_glCreateProgram = (PFNGLCREATEPROGRAMPROC) load(userptr, "glCreateProgram");
    glad_glCreateShader = (PFNGLCREATESHADERPROC) load(userptr, "glCreateShader");
    glad_glCullFace = (PFNGLCULLFACEPROC) load(userptr, "glCullFace");
    glad_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) load(userptr, "glDeleteBuffers");
    glad_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) load(userptr, "glDeleteFramebuffers");
    glad_glDeleteProgram = (PFNGLDELETEPROGRAMPROC) load(userptr, "glDeleteProgram");
    glad_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) load(userptr, "glDeleteRenderbuffers");
    glad_glDeleteShader = (PFNGLDELETESHADERPROC) load(userptr, "glDeleteShader");
    glad_glDeleteTextures = (PFNGLDELETETEXTURESPROC) load(userptr, "glDeleteTextures");
    glad_glDepthFunc = (PFNGLDEPTHFUNCPROC) load(userptr, "glDepthFunc");
    glad_glDepthMask = (PFNGLDEPTHMASKPROC) load(userptr, "glDepthMask");
    glad_glDepthRangef = (PFNGLDEPTHRANGEFPROC) load(userptr, "glDepthRangef");
    glad_glDetachShader = (PFNGLDETACHSHADERPROC) load(userptr, "glDetachShader");
    glad_glDisable = (PFNGLDISABLEPROC) load(userptr, "glDisable");
    glad_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) load(userptr, "glDisableVertexAttribArray");
    glad_glDrawArrays = (PFNGLDRAWARRAYSPROC) load(userptr, "glDrawArrays");
    glad_glDrawElements = (PFNGLDRAWELEMENTSPROC) load(userptr, "glDrawElements");
    glad_glEnable = (PFNGLENABLEPROC) load(userptr, "glEnable");
    glad_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) load(userptr, "glEnableVertexAttribArray");
    glad_glFinish = (PFNGLFINISHPROC) load(userptr, "glFinish");
    glad_glFlush = (PFNGLFLUSHPROC) load(userptr, "glFlush");
    glad_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) load(userptr, "glFramebufferRenderbuffer");
    glad_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) load(userptr, "glFramebufferTexture2D");
    glad_glFrontFace = (PFNGLFRONTFACEPROC) load(userptr, "glFrontFace");
    glad_glGenBuffers = (PFNGLGENBUFFERSPROC) load(userptr, "glGenBuffers");
    glad_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) load(userptr, "glGenFramebuffers");
    glad_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) load(userptr, "glGenRenderbuffers");
    glad_glGenTextures = (PFNGLGENTEXTURESPROC) load(userptr, "glGenTextures");
    glad_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) load(userptr, "glGenerateMipmap");
    glad_glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) load(userptr, "glGetActiveAttrib");
    glad_glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC) load(userptr, "glGetActiveUniform");
    glad_glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC) load(userptr, "glGetAttachedShaders");
    glad_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) load(userptr, "glGetAttribLocation");
    glad_glGetBooleanv = (PFNGLGETBOOLEANVPROC) load(userptr, "glGetBooleanv");
    glad_glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) load(userptr, "glGetBufferParameteriv");
    glad_glGetError = (PFNGLGETERRORPROC) load(userptr, "glGetError");
    glad_glGetFloatv = (PFNGLGETFLOATVPROC) load(userptr, "glGetFloatv");
    glad_glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) load(userptr, "glGetFramebufferAttachmentParameteriv");
    glad_glGetIntegerv = (PFNGLGETINTEGERVPROC) load(userptr, "glGetIntegerv");
    glad_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) load(userptr, "glGetProgramInfoLog");
    glad_glGetProgramiv = (PFNGLGETPROGRAMIVPROC) load(userptr, "glGetProgramiv");
    glad_glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) load(userptr, "glGetRenderbufferParameteriv");
    glad_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) load(userptr, "glGetShaderInfoLog");
    glad_glGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC) load(userptr, "glGetShaderPrecisionFormat");
    glad_glGetShaderSource = (PFNGLGETSHADERSOURCEPROC) load(userptr, "glGetShaderSource");
    glad_glGetShaderiv = (PFNGLGETSHADERIVPROC) load(userptr, "glGetShaderiv");
    glad_glGetString = (PFNGLGETSTRINGPROC) load(userptr, "glGetString");
    glad_glGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC) load(userptr, "glGetTexParameterfv");
    glad_glGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC) load(userptr, "glGetTexParameteriv");
    glad_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) load(userptr, "glGetUniformLocation");
    glad_glGetUniformfv = (PFNGLGETUNIFORMFVPROC) load(userptr, "glGetUniformfv");
    glad_glGetUniformiv = (PFNGLGETUNIFORMIVPROC) load(userptr, "glGetUniformiv");
    glad_glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC) load(userptr, "glGetVertexAttribPointerv");
    glad_glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC) load(userptr, "glGetVertexAttribfv");
    glad_glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC) load(userptr, "glGetVertexAttribiv");
    glad_glHint = (PFNGLHINTPROC) load(userptr, "glHint");
    glad_glIsBuffer = (PFNGLISBUFFERPROC) load(userptr, "glIsBuffer");
    glad_glIsEnabled = (PFNGLISENABLEDPROC) load(userptr, "glIsEnabled");
    glad_glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) load(userptr, "glIsFramebuffer");
    glad_glIsProgram = (PFNGLISPROGRAMPROC) load(userptr, "glIsProgram");
    glad_glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) load(userptr, "glIsRenderbuffer");
    glad_glIsShader = (PFNGLISSHADERPROC) load(userptr, "glIsShader");
    glad_glIsTexture = (PFNGLISTEXTUREPROC) load(userptr, "glIsTexture");
    glad_glLineWidth = (PFNGLLINEWIDTHPROC) load(userptr, "glLineWidth");
    glad_glLinkProgram = (PFNGLLINKPROGRAMPROC) load(userptr, "glLinkProgram");
    glad_glPixelStorei = (PFNGLPIXELSTOREIPROC) load(userptr, "glPixelStorei");
    glad_glPolygonOffset = (PFNGLPOLYGONOFFSETPROC) load(userptr, "glPolygonOffset");
    glad_glReadPixels = (PFNGLREADPIXELSPROC) load(userptr, "glReadPixels");
    glad_glReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC) load(userptr, "glReleaseShaderCompiler");
    glad_glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) load(userptr, "glRenderbufferStorage");
    glad_glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC) load(userptr, "glSampleCoverage");
    glad_glScissor = (PFNGLSCISSORPROC) load(userptr, "glScissor");
    glad_glShaderBinary = (PFNGLSHADERBINARYPROC) load(userptr, "glShaderBinary");
    glad_glShaderSource = (PFNGLSHADERSOURCEPROC) load(userptr, "glShaderSource");
    glad_glStencilFunc = (PFNGLSTENCILFUNCPROC) load(userptr, "glStencilFunc");
    glad_glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC) load(userptr, "glStencilFuncSeparate");
    glad_glStencilMask = (PFNGLSTENCILMASKPROC) load(userptr, "glStencilMask");
    glad_glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC) load(userptr, "glStencilMaskSeparate");
    glad_glStencilOp = (PFNGLSTENCILOPPROC) load(userptr, "glStencilOp");
    glad_glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC) load(userptr, "glStencilOpSeparate");
    glad_glTexImage2D = (PFNGLTEXIMAGE2DPROC) load(userptr, "glTexImage2D");
    glad_glTexParameterf = (PFNGLTEXPARAMETERFPROC) load(userptr, "glTexParameterf");
    glad_glTexParameterfv = (PFNGLTEXPARAMETERFVPROC) load(userptr, "glTexParameterfv");
    glad_glTexParameteri = (PFNGLTEXPARAMETERIPROC) load(userptr, "glTexParameteri");
    glad_glTexParameteriv = (PFNGLTEXPARAMETERIVPROC) load(userptr, "glTexParameteriv");
    glad_glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC) load(userptr, "glTexSubImage2D");
    glad_glUniform1f = (PFNGLUNIFORM1FPROC) load(userptr, "glUniform1f");
    glad_glUniform1fv = (PFNGLUNIFORM1FVPROC) load(userptr, "glUniform1fv");
    glad_glUniform1i = (PFNGLUNIFORM1IPROC) load(userptr, "glUniform1i");
    glad_glUniform1iv = (PFNGLUNIFORM1IVPROC) load(userptr, "glUniform1iv");
    glad_glUniform2f = (PFNGLUNIFORM2FPROC) load(userptr, "glUniform2f");
    glad_glUniform2fv = (PFNGLUNIFORM2FVPROC) load(userptr, "glUniform2fv");
    glad_glUniform2i = (PFNGLUNIFORM2IPROC) load(userptr, "glUniform2i");
    glad_glUniform2iv = (PFNGLUNIFORM2IVPROC) load(userptr, "glUniform2iv");
    glad_glUniform3f = (PFNGLUNIFORM3FPROC) load(userptr, "glUniform3f");
    glad_glUniform3fv = (PFNGLUNIFORM3FVPROC) load(userptr, "glUniform3fv");
    glad_glUniform3i = (PFNGLUNIFORM3IPROC) load(userptr, "glUniform3i");
    glad_glUniform3iv = (PFNGLUNIFORM3IVPROC) load(userptr, "glUniform3iv");
    glad_glUniform4f = (PFNGLUNIFORM4FPROC) load(userptr, "glUniform4f");
    glad_glUniform4fv = (PFNGLUNIFORM4FVPROC) load(userptr, "glUniform4fv");
    glad_glUniform4i = (PFNGLUNIFORM4IPROC) load(userptr, "glUniform4i");
    glad_glUniform4iv = (PFNGLUNIFORM4IVPROC) load(userptr, "glUniform4iv");
    glad_glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC) load(userptr, "glUniformMatrix2fv");
    glad_glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC) load(userptr, "glUniformMatrix3fv");
    glad_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) load(userptr, "glUniformMatrix4fv");
    glad_glUseProgram = (PFNGLUSEPROGRAMPROC) load(userptr, "glUseProgram");
    glad_glValidateProgram = (PFNGLVALIDATEPROGRAMPROC) load(userptr, "glValidateProgram");
    glad_glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC) load(userptr, "glVertexAttrib1f");
    glad_glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC) load(userptr, "glVertexAttrib1fv");
    glad_glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC) load(userptr, "glVertexAttrib2f");
    glad_glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC) load(userptr, "glVertexAttrib2fv");
    glad_glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC) load(userptr, "glVertexAttrib3f");
    glad_glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC) load(userptr, "glVertexAttrib3fv");
    glad_glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC) load(userptr, "glVertexAttrib4f");
    glad_glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC) load(userptr, "glVertexAttrib4fv");
    glad_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) load(userptr, "glVertexAttribPointer");
    glad_glViewport = (PFNGLVIEWPORTPROC) load(userptr, "glViewport");
}
static void glad_gl_load_GL_ES_VERSION_3_0( GLADuserptrloadfunc load, void* userptr) {
    if(!GLAD_GL_ES_VERSION_3_0) return;
    glad_glBeginQuery = (PFNGLBEGINQUERYPROC) load(userptr, "glBeginQuery");
    glad_glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC) load(userptr, "glBeginTransformFeedback");
    glad_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC) load(userptr, "glBindBufferBase");
    glad_glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC) load(userptr, "glBindBufferRange");
    glad_glBindSampler = (PFNGLBINDSAMPLERPROC) load(userptr, "glBindSampler");
    glad_glBindTransformFeedback = (PFNGLBINDTRANSFORMFEEDBACKPROC) load(userptr, "glBindTransformFeedback");
    glad_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) load(userptr, "glBindVertexArray");
    glad_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC) load(userptr, "glBlitFramebuffer");
    glad_glClearBufferfi = (PFNGLCLEARBUFFERFIPROC) load(userptr, "glClearBufferfi");
    glad_glClearBufferfv = (PFNGLCLEARBUFFERFVPROC) load(userptr, "glClearBufferfv");
    glad_glClearBufferiv = (PFNGLCLEARBUFFERIVPROC) load(userptr, "glClearBufferiv");
    glad_glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC) load(userptr, "glClearBufferuiv");
    glad_glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC) load(userptr, "glClientWaitSync");
    glad_glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) load(userptr, "glCompressedTexImage3D");
    glad_glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) load(userptr, "glCompressedTexSubImage3D");
    glad_glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC) load(userptr, "glCopyBufferSubData");
    glad_glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC) load(userptr, "glCopyTexSubImage3D");
    glad_glDeleteQueries = (PFNGLDELETEQUERIESPROC) load(userptr, "glDeleteQueries");
    glad_glDeleteSamplers = (PFNGLDELETESAMPLERSPROC) load(userptr, "glDeleteSamplers");
    glad_glDeleteSync = (PFNGLDELETESYNCPROC) load(userptr, "glDeleteSync");
    glad_glDeleteTransformFeedbacks = (PFNGLDELETETRANSFORMFEEDBACKSPROC) load(userptr, "glDeleteTransformFeedbacks");
    glad_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) load(userptr, "glDeleteVertexArrays");
    glad_glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC) load(userptr, "glDrawArraysInstanced");
    glad_glDrawBuffers = (PFNGLDRAWBUFFERSPROC) load(userptr, "glDrawBuffers");
    glad_glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC) load(userptr, "glDrawElementsInstanced");
    glad_glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC) load(userptr, "glDrawRangeElements");
    glad_glEndQuery = (PFNGLENDQUERYPROC) load(userptr, "glEndQuery");
    glad_glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC) load(userptr, "glEndTransformFeedback");
    glad_glFenceSync = (PFNGLFENCESYNCPROC) load(userptr, "glFenceSync");
    glad_glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC) load(userptr, "glFlushMappedBufferRange");
    glad_glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC) load(userptr, "glFramebufferTextureLayer");
    glad_glGenQueries = (PFNGLGENQUERIESPROC) load(userptr, "glGenQueries");
    glad_glGenSamplers = (PFNGLGENSAMPLERSPROC) load(userptr, "glGenSamplers");
    glad_glGenTransformFeedbacks = (PFNGLGENTRANSFORMFEEDBACKSPROC) load(userptr, "glGenTransformFeedbacks");
    glad_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) load(userptr, "glGenVertexArrays");
    glad_glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC) load(userptr, "glGetActiveUniformBlockName");
    glad_glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC) load(userptr, "glGetActiveUniformBlockiv");
    glad_glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC) load(userptr, "glGetActiveUniformsiv");
    glad_glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC) load(userptr, "glGetBufferParameteri64v");
    glad_glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC) load(userptr, "glGetBufferPointerv");
    glad_glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC) load(userptr, "glGetFragDataLocation");
    glad_glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC) load(userptr, "glGetInteger64i_v");
    glad_glGetInteger64v = (PFNGLGETINTEGER64VPROC) load(userptr, "glGetInteger64v");
    glad_glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC) load(userptr, "glGetIntegeri_v");
    glad_glGetInternalformativ = (PFNGLGETINTERNALFORMATIVPROC) load(userptr, "glGetInternalformativ");
    glad_glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC) load(userptr, "glGetProgramBinary");
    glad_glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC) load(userptr, "glGetQueryObjectuiv");
    glad_glGetQueryiv = (PFNGLGETQUERYIVPROC) load(userptr, "glGetQueryiv");
    glad_glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC) load(userptr, "glGetSamplerParameterfv");
    glad_glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC) load(userptr, "glGetSamplerParameteriv");
    glad_glGetStringi = (PFNGLGETSTRINGIPROC) load(userptr, "glGetStringi");
    glad_glGetSynciv = (PFNGLGETSYNCIVPROC) load(userptr, "glGetSynciv");
    glad_glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) load(userptr, "glGetTransformFeedbackVarying");
    glad_glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC) load(userptr, "glGetUniformBlockIndex");
    glad_glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC) load(userptr, "glGetUniformIndices");
    glad_glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC) load(userptr, "glGetUniformuiv");
    glad_glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC) load(userptr, "glGetVertexAttribIiv");
    glad_glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC) load(userptr, "glGetVertexAttribIuiv");
    glad_glInvalidateFramebuffer = (PFNGLINVALIDATEFRAMEBUFFERPROC) load(userptr, "glInvalidateFramebuffer");
    glad_glInvalidateSubFramebuffer = (PFNGLINVALIDATESUBFRAMEBUFFERPROC) load(userptr, "glInvalidateSubFramebuffer");
    glad_glIsQuery = (PFNGLISQUERYPROC) load(userptr, "glIsQuery");
    glad_glIsSampler = (PFNGLISSAMPLERPROC) load(userptr, "glIsSampler");
    glad_glIsSync = (PFNGLISSYNCPROC) load(userptr, "glIsSync");
    glad_glIsTransformFeedback = (PFNGLISTRANSFORMFEEDBACKPROC) load(userptr, "glIsTransformFeedback");
    glad_glIsVertexArray = (PFNGLISVERTEXARRAYPROC) load(userptr, "glIsVertexArray");
    glad_glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC) load(userptr, "glMapBufferRange");
    glad_glPauseTransformFeedback = (PFNGLPAUSETRANSFORMFEEDBACKPROC) load(userptr, "glPauseTransformFeedback");
    glad_glProgramBinary = (PFNGLPROGRAMBINARYPROC) load(userptr, "glProgramBinary");
    glad_glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC) load(userptr, "glProgramParameteri");
    glad_glReadBuffer = (PFNGLREADBUFFERPROC) load(userptr, "glReadBuffer");
    glad_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) load(userptr, "glRenderbufferStorageMultisample");
    glad_glResumeTransformFeedback = (PFNGLRESUMETRANSFORMFEEDBACKPROC) load(userptr, "glResumeTransformFeedback");
    glad_glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC) load(userptr, "glSamplerParameterf");
    glad_glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC) load(userptr, "glSamplerParameterfv");
    glad_glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC) load(userptr, "glSamplerParameteri");
    glad_glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC) load(userptr, "glSamplerParameteriv");
    glad_glTexImage3D = (PFNGLTEXIMAGE3DPROC) load(userptr, "glTexImage3D");
    glad_glTexStorage2D = (PFNGLTEXSTORAGE2DPROC) load(userptr, "glTexStorage2D");
    glad_glTexStorage3D = (PFNGLTEXSTORAGE3DPROC) load(userptr, "glTexStorage3D");
    glad_glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC) load(userptr, "glTexSubImage3D");
    glad_glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC) load(userptr, "glTransformFeedbackVaryings");
    glad_glUniform1ui = (PFNGLUNIFORM1UIPROC) load(userptr, "glUniform1ui");
    glad_glUniform1uiv = (PFNGLUNIFORM1UIVPROC) load(userptr, "glUniform1uiv");
    glad_glUniform2ui = (PFNGLUNIFORM2UIPROC) load(userptr, "glUniform2ui");
    glad_glUniform2uiv = (PFNGLUNIFORM2UIVPROC) load(userptr, "glUniform2uiv");
    glad_glUniform3ui = (PFNGLUNIFORM3UIPROC) load(userptr, "glUniform3ui");
    glad_glUniform3uiv = (PFNGLUNIFORM3UIVPROC) load(userptr, "glUniform3uiv");
    glad_glUniform4ui = (PFNGLUNIFORM4UIPROC) load(userptr, "glUniform4ui");
    glad_glUniform4uiv = (PFNGLUNIFORM4UIVPROC) load(userptr, "glUniform4uiv");
    glad_glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC) load(userptr, "glUniformBlockBinding");
    glad_glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC) load(userptr, "glUniformMatrix2x3fv");
    glad_glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC) load(userptr, "glUniformMatrix2x4fv");
    glad_glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC) load(userptr, "glUniformMatrix3x2fv");
    glad_glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC) load(userptr, "glUniformMatrix3x4fv");
    glad_glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC) load(userptr, "glUniformMatrix4x2fv");
    glad_glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC) load(userptr, "glUniformMatrix4x3fv");
    glad_glUnmapBuffer = (PFNGLUNMAPBUFFERPROC) load(userptr, "glUnmapBuffer");
    glad_glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC) load(userptr, "glVertexAttribDivisor");
    glad_glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC) load(userptr, "glVertexAttribI4i");
    glad_glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC) load(userptr, "glVertexAttribI4iv");
    glad_glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC) load(userptr, "glVertexAttribI4ui");
    glad_glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC) load(userptr, "glVertexAttribI4uiv");
    glad_glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC) load(userptr, "glVertexAttribIPointer");
    glad_glWaitSync = (PFNGLWAITSYNCPROC) load(userptr, "glWaitSync");
}
static void glad_gl_load_GL_ES_VERSION_3_1( GLADuserptrloadfunc load, void* userptr) {
    if(!GLAD_GL_ES_VERSION_3_1) return;
    glad_glActiveShaderProgram = (PFNGLACTIVESHADERPROGRAMPROC) load(userptr, "glActiveShaderProgram");
    glad_glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC) load(userptr, "glBindImageTexture");
    glad_glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC) load(userptr, "glBindProgramPipeline");
    glad_glBindVertexBuffer = (PFNGLBINDVERTEXBUFFERPROC) load(userptr, "glBindVertexBuffer");
    glad_glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC) load(userptr, "glCreateShaderProgramv");
    glad_glDeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC) load(userptr, "glDeleteProgramPipelines");
    glad_glDispatchCompute = (PFNGLDISPATCHCOMPUTEPROC) load(userptr, "glDispatchCompute");
    glad_glDispatchComputeIndirect = (PFNGLDISPATCHCOMPUTEINDIRECTPROC) load(userptr, "glDispatchComputeIndirect");
    glad_glDrawArraysIndirect = (PFNGLDRAWARRAYSINDIRECTPROC) load(userptr, "glDrawArraysIndirect");
    glad_glDrawElementsIndirect = (PFNGLDRAWELEMENTSINDIRECTPROC) load(userptr, "glDrawElementsIndirect");
    glad_glFramebufferParameteri = (PFNGLFRAMEBUFFERPARAMETERIPROC) load(userptr, "glFramebufferParameteri");
    glad_glGenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC) load(userptr, "glGenProgramPipelines");
    glad_glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC) load(userptr, "glGetBooleani_v");
    glad_glGetFramebufferParameteriv = (PFNGLGETFRAMEBUFFERPARAMETERIVPROC) load(userptr, "glGetFramebufferParameteriv");
    glad_glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC) load(userptr, "glGetMultisamplefv");
    glad_glGetProgramInterfaceiv = (PFNGLGETPROGRAMINTERFACEIVPROC) load(userptr, "glGetProgramInterfaceiv");
    glad_glGetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC) load(userptr, "glGetProgramPipelineInfoLog");
    glad_glGetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC) load(userptr, "glGetProgramPipelineiv");
    glad_glGetProgramResourceIndex = (PFNGLGETPROGRAMRESOURCEINDEXPROC) load(userptr, "glGetProgramResourceIndex");
    glad_glGetProgramResourceLocation = (PFNGLGETPROGRAMRESOURCELOCATIONPROC) load(userptr, "glGetProgramResourceLocation");
    glad_glGetProgramResourceName = (PFNGLGETPROGRAMRESOURCENAMEPROC) load(userptr, "glGetProgramResourceName");
    glad_glGetProgramResourceiv = (PFNGLGETPROGRAMRESOURCEIVPROC) load(userptr, "glGetProgramResourceiv");
    glad_glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC) load(userptr, "glGetTexLevelParameterfv");
    glad_glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC) load(userptr, "glGetTexLevelParameteriv");
    glad_glIsProgramPipeline = (PFNGLISPROGRAMPIPELINEPROC) load(userptr, "glIsProgramPipeline");
    glad_glMemoryBarrier = (PFNGLMEMORYBARRIERPROC) load(userptr, "glMemoryBarrier");
    glad_glMemoryBarrierByRegion = (PFNGLMEMORYBARRIERBYREGIONPROC) load(userptr, "glMemoryBarrierByRegion");
    glad_glProgramUniform1f = (PFNGLPROGRAMUNIFORM1FPROC) load(userptr, "glProgramUniform1f");
    glad_glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC) load(userptr, "glProgramUniform1fv");
    glad_glProgramUniform1i = (PFNGLPROGRAMUNIFORM1IPROC) load(userptr, "glProgramUniform1i");
    glad_glProgramUniform1iv = (PFNGLPROGRAMUNIFORM1IVPROC) load(userptr, "glProgramUniform1iv");
    glad_glProgramUniform1ui = (PFNGLPROGRAMUNIFORM1UIPROC) load(userptr, "glProgramUniform1ui");
    glad_glProgramUniform1uiv = (PFNGLPROGRAMUNIFORM1UIVPROC) load(userptr, "glProgramUniform1uiv");
    glad_glProgramUniform2f = (PFNGLPROGRAMUNIFORM2FPROC) load(userptr, "glProgramUniform2f");
    glad_glProgramUniform2fv = (PFNGLPROGRAMUNIFORM2FVPROC) load(userptr, "glProgramUniform2fv");
    glad_glProgramUniform2i = (PFNGLPROGRAMUNIFORM2IPROC) load(userptr, "glProgramUniform2i");
    glad_glProgramUniform2iv = (PFNGLPROGRAMUNIFORM2IVPROC) load(userptr, "glProgramUniform2iv");
    glad_glProgramUniform2ui = (PFNGLPROGRAMUNIFORM2UIPROC) load(userptr, "glProgramUniform2ui");
    glad_glProgramUniform2uiv = (PFNGLPROGRAMUNIFORM2UIVPROC) load(userptr, "glProgramUniform2uiv");
    glad_glProgramUniform3f = (PFNGLPROGRAMUNIFORM3FPROC) load(userptr, "glProgramUniform3f");
    glad_glProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC) load(userptr, "glProgramUniform3fv");
    glad_glProgramUniform3i = (PFNGLPROGRAMUNIFORM3IPROC) load(userptr, "glProgramUniform3i");
    glad_glProgramUniform3iv = (PFNGLPROGRAMUNIFORM3IVPROC) load(userptr, "glProgramUniform3iv");
    glad_glProgramUniform3ui = (PFNGLPROGRAMUNIFORM3UIPROC) load(userptr, "glProgramUniform3ui");
    glad_glProgramUniform3uiv = (PFNGLPROGRAMUNIFORM3UIVPROC) load(userptr, "glProgramUniform3uiv");
    glad_glProgramUniform4f = (PFNGLPROGRAMUNIFORM4FPROC) load(userptr, "glProgramUniform4f");
    glad_glProgramUniform4fv = (PFNGLPROGRAMUNIFORM4FVPROC) load(userptr, "glProgramUniform4fv");
    glad_glProgramUniform4i = (PFNGLPROGRAMUNIFORM4IPROC) load(userptr, "glProgramUniform4i");
    glad_glProgramUniform4iv = (PFNGLPROGRAMUNIFORM4IVPROC) load(userptr, "glProgramUniform4iv");
    glad_glProgramUniform4ui = (PFNGLPROGRAMUNIFORM4UIPROC) load(userptr, "glProgramUniform4ui");
    glad_glProgramUniform4uiv = (PFNGLPROGRAMUNIFORM4UIVPROC) load(userptr, "glProgramUniform4uiv");
    glad_glProgramUniformMatrix2fv = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC) load(userptr, "glProgramUniformMatrix2fv");
    glad_glProgramUniformMatrix2x3fv = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC) load(userptr, "glProgramUniformMatrix2x3fv");
    glad_glProgramUniformMatrix2x4fv = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC) load(userptr, "glProgramUniformMatrix2x4fv");
    glad_glProgramUniformMatrix3fv = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC) load(userptr, "glProgramUniformMatrix3fv");
    glad_glProgramUniformMatrix3x2fv = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC) load(userptr, "glProgramUniformMatrix3x2fv");
    glad_glProgramUniformMatrix3x4fv = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC) load(userptr, "glProgramUniformMatrix3x4fv");
    glad_glProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC) load(userptr, "glProgramUniformMatrix4fv");
    glad_glProgramUniformMatrix4x2fv = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC) load(userptr, "glProgramUniformMatrix4x2fv");
    glad_glProgramUniformMatrix4x3fv = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC) load(userptr, "glProgramUniformMatrix4x3fv");
    glad_glSampleMaski = (PFNGLSAMPLEMASKIPROC) load(userptr, "glSampleMaski");
    glad_glTexStorage2DMultisample = (PFNGLTEXSTORAGE2DMULTISAMPLEPROC) load(userptr, "glTexStorage2DMultisample");
    glad_glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC) load(userptr, "glUseProgramStages");
    glad_glValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC) load(userptr, "glValidateProgramPipeline");
    glad_glVertexAttribBinding = (PFNGLVERTEXATTRIBBINDINGPROC) load(userptr, "glVertexAttribBinding");
    glad_glVertexAttribFormat = (PFNGLVERTEXATTRIBFORMATPROC) load(userptr, "glVertexAttribFormat");
    glad_glVertexAttribIFormat = (PFNGLVERTEXATTRIBIFORMATPROC) load(userptr, "glVertexAttribIFormat");
    glad_glVertexBindingDivisor = (PFNGLVERTEXBINDINGDIVISORPROC) load(userptr, "glVertexBindingDivisor");
}
static void glad_gl_load_GL_ES_VERSION_3_2( GLADuserptrloadfunc load, void* userptr) {
    if(!GLAD_GL_ES_VERSION_3_2) return;
    glad_glBlendBarrier = (PFNGLBLENDBARRIERPROC) load(userptr, "glBlendBarrier");
    glad_glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC) load(userptr, "glBlendEquationSeparatei");
    glad_glBlendEquationi = (PFNGLBLENDEQUATIONIPROC) load(userptr, "glBlendEquationi");
    glad_glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC) load(userptr, "glBlendFuncSeparatei");
    glad_glBlendFunci = (PFNGLBLENDFUNCIPROC) load(userptr, "glBlendFunci");
    glad_glColorMaski = (PFNGLCOLORMASKIPROC) load(userptr, "glColorMaski");
    glad_glCopyImageSubData = (PFNGLCOPYIMAGESUBDATAPROC) load(userptr, "glCopyImageSubData");
    glad_glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC) load(userptr, "glDebugMessageCallback");
    glad_glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC) load(userptr, "glDebugMessageControl");
    glad_glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC) load(userptr, "glDebugMessageInsert");
    glad_glDisablei = (PFNGLDISABLEIPROC) load(userptr, "glDisablei");
    glad_glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC) load(userptr, "glDrawElementsBaseVertex");
    glad_glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC) load(userptr, "glDrawElementsInstancedBaseVertex");
    glad_glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC) load(userptr, "glDrawRangeElementsBaseVertex");
    glad_glEnablei = (PFNGLENABLEIPROC) load(userptr, "glEnablei");
    glad_glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC) load(userptr, "glFramebufferTexture");
    glad_glGetDebugMessageLog = (PFNGLGETDEBUGMESSAGELOGPROC) load(userptr, "glGetDebugMessageLog");
    glad_glGetGraphicsResetStatus = (PFNGLGETGRAPHICSRESETSTATUSPROC) load(userptr, "glGetGraphicsResetStatus");
    glad_glGetObjectLabel = (PFNGLGETOBJECTLABELPROC) load(userptr, "glGetObjectLabel");
    glad_glGetObjectPtrLabel = (PFNGLGETOBJECTPTRLABELPROC) load(userptr, "glGetObjectPtrLabel");
    glad_glGetPointerv = (PFNGLGETPOINTERVPROC) load(userptr, "glGetPointerv");
    glad_glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC) load(userptr, "glGetSamplerParameterIiv");
    glad_glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC) load(userptr, "glGetSamplerParameterIuiv");
    glad_glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC) load(userptr, "glGetTexParameterIiv");
    glad_glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC) load(userptr, "glGetTexParameterIuiv");
    glad_glGetnUniformfv = (PFNGLGETNUNIFORMFVPROC) load(userptr, "glGetnUniformfv");
    glad_glGetnUniformiv = (PFNGLGETNUNIFORMIVPROC) load(userptr, "glGetnUniformiv");
    glad_glGetnUniformuiv = (PFNGLGETNUNIFORMUIVPROC) load(userptr, "glGetnUniformuiv");
    glad_glIsEnabledi = (PFNGLISENABLEDIPROC) load(userptr, "glIsEnabledi");
    glad_glMinSampleShading = (PFNGLMINSAMPLESHADINGPROC) load(userptr, "glMinSampleShading");
    glad_glObjectLabel = (PFNGLOBJECTLABELPROC) load(userptr, "glObjectLabel");
    glad_glObjectPtrLabel = (PFNGLOBJECTPTRLABELPROC) load(userptr, "glObjectPtrLabel");
    glad_glPatchParameteri = (PFNGLPATCHPARAMETERIPROC) load(userptr, "glPatchParameteri");
    glad_glPopDebugGroup = (PFNGLPOPDEBUGGROUPPROC) load(userptr, "glPopDebugGroup");
    glad_glPrimitiveBoundingBox = (PFNGLPRIMITIVEBOUNDINGBOXPROC) load(userptr, "glPrimitiveBoundingBox");
    glad_glPushDebugGroup = (PFNGLPUSHDEBUGGROUPPROC) load(userptr, "glPushDebugGroup");
    glad_glReadnPixels = (PFNGLREADNPIXELSPROC) load(userptr, "glReadnPixels");
    glad_glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC) load(userptr, "glSamplerParameterIiv");
    glad_glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC) load(userptr, "glSamplerParameterIuiv");
    glad_glTexBuffer = (PFNGLTEXBUFFERPROC) load(userptr, "glTexBuffer");
    glad_glTexBufferRange = (PFNGLTEXBUFFERRANGEPROC) load(userptr, "glTexBufferRange");
    glad_glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC) load(userptr, "glTexParameterIiv");
    glad_glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC) load(userptr, "glTexParameterIuiv");
    glad_glTexStorage3DMultisample = (PFNGLTEXSTORAGE3DMULTISAMPLEPROC) load(userptr, "glTexStorage3DMultisample");
}
static void glad_gl_load_GL_KHR_debug( GLADuserptrloadfunc load, void* userptr) {
    if(!GLAD_GL_KHR_debug) return;
    glad_glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC) load(userptr, "glDebugMessageCallback");
    glad_glDebugMessageCallbackKHR = (PFNGLDEBUGMESSAGECALLBACKKHRPROC) load(userptr, "glDebugMessageCallbackKHR");
    glad_glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC) load(userptr, "glDebugMessageControl");
    glad_glDebugMessageControlKHR = (PFNGLDEBUGMESSAGECONTROLKHRPROC) load(userptr, "glDebugMessageControlKHR");
    glad_glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC) load(userptr, "glDebugMessageInsert");
    glad_glDebugMessageInsertKHR = (PFNGLDEBUGMESSAGEINSERTKHRPROC) load(userptr, "glDebugMessageInsertKHR");
    glad_glGetDebugMessageLog = (PFNGLGETDEBUGMESSAGELOGPROC) load(userptr, "glGetDebugMessageLog");
    glad_glGetDebugMessageLogKHR = (PFNGLGETDEBUGMESSAGELOGKHRPROC) load(userptr, "glGetDebugMessageLogKHR");
    glad_glGetObjectLabel = (PFNGLGETOBJECTLABELPROC) load(userptr, "glGetObjectLabel");
    glad_glGetObjectLabelKHR = (PFNGLGETOBJECTLABELKHRPROC) load(userptr, "glGetObjectLabelKHR");
    glad_glGetObjectPtrLabel = (PFNGLGETOBJECTPTRLABELPROC) load(userptr, "glGetObjectPtrLabel");
    glad_glGetObjectPtrLabelKHR = (PFNGLGETOBJECTPTRLABELKHRPROC) load(userptr, "glGetObjectPtrLabelKHR");
    glad_glGetPointerv = (PFNGLGETPOINTERVPROC) load(userptr, "glGetPointerv");
    glad_glGetPointervKHR = (PFNGLGETPOINTERVKHRPROC) load(userptr, "glGetPointervKHR");
    glad_glObjectLabel = (PFNGLOBJECTLABELPROC) load(userptr, "glObjectLabel");
    glad_glObjectLabelKHR = (PFNGLOBJECTLABELKHRPROC) load(userptr, "glObjectLabelKHR");
    glad_glObjectPtrLabel = (PFNGLOBJECTPTRLABELPROC) load(userptr, "glObjectPtrLabel");
    glad_glObjectPtrLabelKHR = (PFNGLOBJECTPTRLABELKHRPROC) load(userptr, "glObjectPtrLabelKHR");
    glad_glPopDebugGroup = (PFNGLPOPDEBUGGROUPPROC) load(userptr, "glPopDebugGroup");
    glad_glPopDebugGroupKHR = (PFNGLPOPDEBUGGROUPKHRPROC) load(userptr, "glPopDebugGroupKHR");
    glad_glPushDebugGroup = (PFNGLPUSHDEBUGGROUPPROC) load(userptr, "glPushDebugGroup");
    glad_glPushDebugGroupKHR = (PFNGLPUSHDEBUGGROUPKHRPROC) load(userptr, "glPushDebugGroupKHR");
}



#if defined(GL_ES_VERSION_3_0) || defined(GL_VERSION_3_0)
#define GLAD_GL_IS_SOME_NEW_VERSION 1
#else
#define GLAD_GL_IS_SOME_NEW_VERSION 0
#endif

static int glad_gl_get_extensions( int version, const char **out_exts, unsigned int *out_num_exts_i, char ***out_exts_i) {
#if GLAD_GL_IS_SOME_NEW_VERSION
    if(GLAD_VERSION_MAJOR(version) < 3) {
#else
    (void) version;
    (void) out_num_exts_i;
    (void) out_exts_i;
#endif
        if (glad_glGetString == NULL) {
            return 0;
        }
        *out_exts = (const char *)glad_glGetString(GL_EXTENSIONS);
#if GLAD_GL_IS_SOME_NEW_VERSION
    } else {
        unsigned int index = 0;
        unsigned int num_exts_i = 0;
        char **exts_i = NULL;
        if (glad_glGetStringi == NULL || glad_glGetIntegerv == NULL) {
            return 0;
        }
        glad_glGetIntegerv(GL_NUM_EXTENSIONS, (int*) &num_exts_i);
        if (num_exts_i > 0) {
            exts_i = (char **) malloc(num_exts_i * (sizeof *exts_i));
        }
        if (exts_i == NULL) {
            return 0;
        }
        for(index = 0; index < num_exts_i; index++) {
            const char *gl_str_tmp = (const char*) glad_glGetStringi(GL_EXTENSIONS, index);
            size_t len = strlen(gl_str_tmp) + 1;

            char *local_str = (char*) malloc(len * sizeof(char));
            if(local_str != NULL) {
                memcpy(local_str, gl_str_tmp, len * sizeof(char));
            }

            exts_i[index] = local_str;
        }

        *out_num_exts_i = num_exts_i;
        *out_exts_i = exts_i;
    }
#endif
    return 1;
}
static void glad_gl_free_extensions(char **exts_i, unsigned int num_exts_i) {
    if (exts_i != NULL) {
        unsigned int index;
        for(index = 0; index < num_exts_i; index++) {
            free((void *) (exts_i[index]));
        }
        free((void *)exts_i);
        exts_i = NULL;
    }
}
static int glad_gl_has_extension(int version, const char *exts, unsigned int num_exts_i, char **exts_i, const char *ext) {
    if(GLAD_VERSION_MAJOR(version) < 3 || !GLAD_GL_IS_SOME_NEW_VERSION) {
        const char *extensions;
        const char *loc;
        const char *terminator;
        extensions = exts;
        if(extensions == NULL || ext == NULL) {
            return 0;
        }
        while(1) {
            loc = strstr(extensions, ext);
            if(loc == NULL) {
                return 0;
            }
            terminator = loc + strlen(ext);
            if((loc == extensions || *(loc - 1) == ' ') &&
                (*terminator == ' ' || *terminator == '\0')) {
                return 1;
            }
            extensions = terminator;
        }
    } else {
        unsigned int index;
        for(index = 0; index < num_exts_i; index++) {
            const char *e = exts_i[index];
            if(strcmp(e, ext) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

static GLADapiproc glad_gl_get_proc_from_userptr(void *userptr, const char* name) {
    return (GLAD_GNUC_EXTENSION (GLADapiproc (*)(const char *name)) userptr)(name);
}

static int glad_gl_find_extensions_gles2( int version) {
    const char *exts = NULL;
    unsigned int num_exts_i = 0;
    char **exts_i = NULL;
    if (!glad_gl_get_extensions(version, &exts, &num_exts_i, &exts_i)) return 0;

    GLAD_GL_KHR_debug = glad_gl_has_extension(version, exts, num_exts_i, exts_i, "GL_KHR_debug");

    glad_gl_free_extensions(exts_i, num_exts_i);

    return 1;
}

static int glad_gl_find_core_gles2(void) {
    int i, major, minor;
    const char* version;
    const char* prefixes[] = {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        NULL
    };
    version = (const char*) glad_glGetString(GL_VERSION);
    if (!version) return 0;
    for (i = 0;  prefixes[i];  i++) {
        const size_t length = strlen(prefixes[i]);
        if (strncmp(version, prefixes[i], length) == 0) {
            version += length;
            break;
        }
    }

    GLAD_IMPL_UTIL_SSCANF(version, "%d.%d", &major, &minor);

    GLAD_GL_ES_VERSION_2_0 = (major == 2 && minor >= 0) || major > 2;
    GLAD_GL_ES_VERSION_3_0 = (major == 3 && minor >= 0) || major > 3;
    GLAD_GL_ES_VERSION_3_1 = (major == 3 && minor >= 1) || major > 3;
    GLAD_GL_ES_VERSION_3_2 = (major == 3 && minor >= 2) || major > 3;

    return GLAD_MAKE_VERSION(major, minor);
}

int gladLoadGLES2UserPtr( GLADuserptrloadfunc load, void *userptr) {
    int version;

    glad_glGetString = (PFNGLGETSTRINGPROC) load(userptr, "glGetString");
    if(glad_glGetString == NULL) return 0;
    if(glad_glGetString(GL_VERSION) == NULL) return 0;
    version = glad_gl_find_core_gles2();

    glad_gl_load_GL_ES_VERSION_2_0(load, userptr);
    glad_gl_load_GL_ES_VERSION_3_0(load, userptr);
    glad_gl_load_GL_ES_VERSION_3_1(load, userptr);
    glad_gl_load_GL_ES_VERSION_3_2(load, userptr);

    if (!glad_gl_find_extensions_gles2(version)) return 0;
    glad_gl_load_GL_KHR_debug(load, userptr);



    return version;
}


int gladLoadGLES2( GLADloadfunc load) {
    return gladLoadGLES2UserPtr( glad_gl_get_proc_from_userptr, GLAD_GNUC_EXTENSION (void*) load);
}



 
