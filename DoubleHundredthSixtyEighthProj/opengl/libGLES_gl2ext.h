#ifndef __gl2ext_h_
#define __gl2ext_h_

#include <libGLES_gl2extplatform.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This document is licensed under the SGI Free Software B License Version
 * 2.0. For details, see http://oss.sgi.com/projects/FreeB/ .
*/

#ifndef GL_APIENTRYP
#   define GL_APIENTRYP GL_APIENTRY*
#endif

/*------------------------------------------------------------------------*
 * OES extension tokens
 *------------------------------------------------------------------------*/

/* GL_OES_compressed_ETC1_RGB8_texture */
#ifndef GL_OES_compressed_ETC1_RGB8_texture
#define GL_ETC1_RGB8_OES                                        0x8D64
#endif

/* GL_OES_compressed_paletted_texture */
#ifndef GL_OES_compressed_paletted_texture
#define GL_PALETTE4_RGB8_OES                                    0x8B90
#define GL_PALETTE4_RGBA8_OES                                   0x8B91
#define GL_PALETTE4_R5_G6_B5_OES                                0x8B92
#define GL_PALETTE4_RGBA4_OES                                   0x8B93
#define GL_PALETTE4_RGB5_A1_OES                                 0x8B94
#define GL_PALETTE8_RGB8_OES                                    0x8B95
#define GL_PALETTE8_RGBA8_OES                                   0x8B96
#define GL_PALETTE8_R5_G6_B5_OES                                0x8B97
#define GL_PALETTE8_RGBA4_OES                                   0x8B98
#define GL_PALETTE8_RGB5_A1_OES                                 0x8B99
#endif

/* GL_OES_depth24 */
#ifndef GL_OES_depth24
#define GL_DEPTH_COMPONENT24_OES                                0x81A6
#endif

/* GL_OES_depth32 */
#ifndef GL_OES_depth32
#define GL_DEPTH_COMPONENT32_OES                                0x81A7
#endif

/* GL_OES_depth_texture */
/* No new tokens introduced by this extension. */

/* GL_OES_EGL_image */
#ifndef GL_OES_EGL_image
typedef void* GLeglImageOES;
#endif

/* GL_OES_get_program_binary */
#ifndef GL_OES_get_program_binary
#define GL_PROGRAM_BINARY_LENGTH_OES                            0x8741
#define GL_NUM_PROGRAM_BINARY_FORMATS_OES                       0x87FE
#define GL_PROGRAM_BINARY_FORMATS_OES                           0x87FF
#endif

/* GL_OES_mapbuffer */
#ifndef GL_OES_mapbuffer
#define GL_WRITE_ONLY_OES                                       0x88B9
#define GL_BUFFER_ACCESS_OES                                    0x88BB
#define GL_BUFFER_MAPPED_OES									0x88BC
#define GL_BUFFER_MAP_POINTER_OES								0x88BD
#endif

/* GL_OES_packed_depth_stencil */
#ifndef GL_OES_packed_depth_stencil
#define GL_DEPTH_STENCIL_OES                                    0x84F9
#define GL_UNSIGNED_INT_24_8_OES                                0x84FA
#define GL_DEPTH24_STENCIL8_OES                                 0x88F0
#endif

/* GL_OES_rgb8_rgba8 */
#ifndef GL_OES_rgb8_rgba8
#define GL_RGB8_OES												0x8051
#define GL_RGBA8_OES											0x8058
#endif

/* GL_OES_standard_derivatives */
#ifndef GL_OES_standard_derivatives
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES                  0x8B8B
#endif

/* GL_OES_stencil1 */
#ifndef GL_OES_stencil1
#define GL_STENCIL_INDEX1_OES									0x8D46
#endif

/* GL_OES_stencil4 */
#ifndef GL_OES_stencil4
#define GL_STENCIL_INDEX4_OES									0x8D47
#endif

/* GL_OES_texture3D */
#ifndef GL_OES_texture3D
#define GL_TEXTURE_WRAP_R_OES                                   0x8072
#define GL_TEXTURE_3D_OES                                       0x806F
#define GL_TEXTURE_BINDING_3D_OES                               0x806A
#define GL_MAX_3D_TEXTURE_SIZE_OES                              0x8073
#define GL_SAMPLER_3D_OES                                       0x8B5F
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_OES        0x8CD4
#endif

/* GL_OES_texture_float */
/* No new tokens introduced by this extension. */

/* GL_OES_texture_float_linear */
/* No new tokens introduced by this extension. */

/* GL_OES_texture_half_float */
#ifndef GL_OES_texture_half_float
#define GL_HALF_FLOAT_OES										0x8D61
#endif

/* GL_OES_texture_half_float_linear */
/* No new tokens introduced by this extension. */

/* GL_OES_texture_npot */
/* No new tokens introduced by this extension. */

/* GL_OES_vertex_array_object */
#ifndef GL_OES_vertex_array_object
#define GL_VERTEX_ARRAY_BINDING_OES                             0x85B5
#endif

/* GL_OES_vertex_half_float */
/* GL_HALF_FLOAT_OES defined in GL_OES_texture_half_float already. */

/* GL_OES_vertex_type_10_10_10_2 */
#ifndef GL_OES_vertex_type_10_10_10_2
#define GL_UNSIGNED_INT_10_10_10_2_OES                          0x8DF6
#define GL_INT_10_10_10_2_OES                                   0x8DF7
#endif

/*------------------------------------------------------------------------*
 * AMD extension tokens
 *------------------------------------------------------------------------*/

/* GL_AMD_compressed_3DC_texture */
#ifndef GL_AMD_compressed_3DC_texture
#define GL_3DC_X_AMD                                            0x87F9
#define GL_3DC_XY_AMD                                           0x87FA
#endif

/* GL_AMD_compressed_ATC_texture */
#ifndef GL_AMD_compressed_ATC_texture
#define GL_ATC_RGB_AMD                                          0x8C92
#define GL_ATC_RGBA_EXPLICIT_ALPHA_AMD                          0x8C93
#define GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD                      0x87EE
#endif

/* GL_AMD_performance_monitor */
#ifndef GL_AMD_performance_monitor
#define GL_COUNTER_TYPE_AMD                                     0x8BC0
#define GL_COUNTER_RANGE_AMD                                    0x8BC1
#define GL_UNSIGNED_INT64_AMD                                   0x8BC2
#define GL_PERCENTAGE_AMD                                       0x8BC3
#define GL_PERFMON_RESULT_AVAILABLE_AMD                         0x8BC4
#define GL_PERFMON_RESULT_SIZE_AMD                              0x8BC5
#define GL_PERFMON_RESULT_AMD                                   0x8BC6
#endif

/* GL_AMD_program_binary_Z400 */
#ifndef GL_AMD_program_binary_Z400
#define GL_Z400_BINARY_AMD                                      0x8740
#endif

/*------------------------------------------------------------------------*
 * EXT extension tokens
 *------------------------------------------------------------------------*/

/* GL_EXT_blend_minmax */
#ifndef GL_EXT_blend_minmax
#define GL_MIN_EXT                                              0x8007
#define GL_MAX_EXT                                              0x8008
#endif

/* GL_EXT_discard_framebuffer */
#ifndef GL_EXT_discard_framebuffer
#define GL_COLOR_EXT                                            0x1800
#define GL_DEPTH_EXT                                            0x1801
#define GL_STENCIL_EXT                                          0x1802
#endif

/* GL_EXT_multi_draw_arrays */
/* No new tokens introduced by this extension. */

/* GL_EXT_read_format_bgra */
#ifndef GL_EXT_read_format_bgra
#define GL_BGRA_EXT                                             0x80E1
#define GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT                       0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT                       0x8366
#endif

/* GL_EXT_texture_filter_anisotropic */
#ifndef GL_EXT_texture_filter_anisotropic
#define GL_TEXTURE_MAX_ANISOTROPY_EXT                           0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT                       0x84FF
#endif

/* GL_EXT_texture_compression_dxt1 */
#ifndef GL_EXT_texture_compression_dxt1
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT                         0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT                        0x83F1
#endif

/* GL_EXT_texture_compression_s3tc */
#ifndef GL_EXT_texture_compression_s3tc
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT                        0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT                        0x83F3 
#endif

/* GL_EXT_texture_format_BGRA8888 */
#ifndef GL_EXT_texture_format_BGRA8888
#define GL_BGRA_EXT                                             0x80E1
#endif

/* GL_EXT_texture_type_2_10_10_10_REV */
#ifndef GL_EXT_texture_type_2_10_10_10_REV
#define GL_UNSIGNED_INT_2_10_10_10_REV_EXT                      0x8368
#endif

/*------------------------------------------------------------------------*
 * IMG extension tokens
 *------------------------------------------------------------------------*/

/* GL_IMG_program_binary */
#ifndef GL_IMG_program_binary
#define GL_SGX_PROGRAM_BINARY_IMG                               0x9130
#endif

/* GL_IMG_read_format */
#ifndef GL_IMG_read_format
#define GL_BGRA_IMG                                             0x80E1
#define GL_UNSIGNED_SHORT_4_4_4_4_REV_IMG                       0x8365
#endif

/* GL_IMG_shader_binary */
#ifndef GL_IMG_shader_binary
#define GL_SGX_BINARY_IMG                                       0x8C0A
#endif

/* GL_IMG_texture_compression_pvrtc */
#ifndef GL_IMG_texture_compression_pvrtc
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG						0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG						0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG						0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG						0x8C03
#endif

/* GL_IMG_texture_stream */
#ifndef GL_IMG_texture_stream
#define GL_TEXTURE_STREAM_IMG 									0x8C0D 	
#define GL_TEXTURE_NUM_STREAM_DEVICES_IMG 						0x8C0E 	
#define GL_TEXTURE_STREAM_DEVICE_WIDTH_IMG 						0x8C0F
#define GL_TEXTURE_STREAM_DEVICE_HEIGHT_IMG 					0x8EA0 	
#define GL_TEXTURE_STREAM_DEVICE_FORMAT_IMG 					0x8EA1 	 
#define GL_TEXTURE_STREAM_DEVICE_NUM_BUFFERS_IMG 				0x8EA2 	
#define GL_TEXTURE_BINDING_STREAM_IMG  							0x9131
#define GL_SAMPLER_STREAM_IMG  									0x9132
#endif

/* IMG_vertex_array_object */
#ifndef GL_IMG_vertex_array_object 
#define GL_VERTEX_ARRAY_BINDING_IMG                             0x85B5
#endif

/*------------------------------------------------------------------------*
 * NV extension tokens
 *------------------------------------------------------------------------*/

/* GL_NV_fence */
#ifndef GL_NV_fence
#define GL_ALL_COMPLETED_NV                                     0x84F2
#define GL_FENCE_STATUS_NV                                      0x84F3
#define GL_FENCE_CONDITION_NV                                   0x84F4
#endif

/*------------------------------------------------------------------------*
 * QCOM extension tokens
 *------------------------------------------------------------------------*/

/* GL_QCOM_driver_control */
/* No new tokens introduced by this extension. */

/* GL_QCOM_extended_get */
#ifndef GL_QCOM_extended_get
#define GL_TEXTURE_WIDTH_QCOM                                   0x8BD2
#define GL_TEXTURE_HEIGHT_QCOM                                  0x8BD3
#define GL_TEXTURE_DEPTH_QCOM                                   0x8BD4
#define GL_TEXTURE_INTERNAL_FORMAT_QCOM                         0x8BD5
#define GL_TEXTURE_FORMAT_QCOM                                  0x8BD6
#define GL_TEXTURE_TYPE_QCOM                                    0x8BD7
#define GL_TEXTURE_IMAGE_VALID_QCOM                             0x8BD8
#define GL_TEXTURE_NUM_LEVELS_QCOM                              0x8BD9
#define GL_TEXTURE_TARGET_QCOM                                  0x8BDA
#define GL_TEXTURE_OBJECT_VALID_QCOM                            0x8BDB
#define GL_STATE_RESTORE                                        0x8BDC
#endif

/* GL_QCOM_extended_get2 */
/* No new tokens introduced by this extension. */

/* GL_QCOM_perfmon_global_mode */
#ifndef GL_QCOM_perfmon_global_mode
#define GL_PERFMON_GLOBAL_MODE_QCOM                             0x8FA0
#endif

/* GL_QCOM_writeonly_rendering */
#ifndef GL_QCOM_writeonly_rendering
#define GL_WRITEONLY_RENDERING_QCOM                             0x8823
#endif

/* GL_QCOM_tiled_rendering */
#ifndef GL_QCOM_tiled_rendering
#define GL_COLOR_BUFFER_BIT0_QCOM                               0x00000001
#define GL_COLOR_BUFFER_BIT1_QCOM                               0x00000002
#define GL_COLOR_BUFFER_BIT2_QCOM                               0x00000004
#define GL_COLOR_BUFFER_BIT3_QCOM                               0x00000008
#define GL_COLOR_BUFFER_BIT4_QCOM                               0x00000010
#define GL_COLOR_BUFFER_BIT5_QCOM                               0x00000020
#define GL_COLOR_BUFFER_BIT6_QCOM                               0x00000040
#define GL_COLOR_BUFFER_BIT7_QCOM                               0x00000080
#define GL_DEPTH_BUFFER_BIT0_QCOM                               0x00000100
#define GL_DEPTH_BUFFER_BIT1_QCOM                               0x00000200
#define GL_DEPTH_BUFFER_BIT2_QCOM                               0x00000400
#define GL_DEPTH_BUFFER_BIT3_QCOM                               0x00000800
#define GL_DEPTH_BUFFER_BIT4_QCOM                               0x00001000
#define GL_DEPTH_BUFFER_BIT5_QCOM                               0x00002000
#define GL_DEPTH_BUFFER_BIT6_QCOM                               0x00004000
#define GL_DEPTH_BUFFER_BIT7_QCOM                               0x00008000
#define GL_STENCIL_BUFFER_BIT0_QCOM                             0x00010000
#define GL_STENCIL_BUFFER_BIT1_QCOM                             0x00020000
#define GL_STENCIL_BUFFER_BIT2_QCOM                             0x00040000
#define GL_STENCIL_BUFFER_BIT3_QCOM                             0x00080000
#define GL_STENCIL_BUFFER_BIT4_QCOM                             0x00100000
#define GL_STENCIL_BUFFER_BIT5_QCOM                             0x00200000
#define GL_STENCIL_BUFFER_BIT6_QCOM                             0x00400000
#define GL_STENCIL_BUFFER_BIT7_QCOM                             0x00800000
#define GL_MULTISAMPLE_BUFFER_BIT0_QCOM                         0x01000000
#define GL_MULTISAMPLE_BUFFER_BIT1_QCOM                         0x02000000
#define GL_MULTISAMPLE_BUFFER_BIT2_QCOM                         0x04000000
#define GL_MULTISAMPLE_BUFFER_BIT3_QCOM                         0x08000000
#define GL_MULTISAMPLE_BUFFER_BIT4_QCOM                         0x10000000
#define GL_MULTISAMPLE_BUFFER_BIT5_QCOM                         0x20000000
#define GL_MULTISAMPLE_BUFFER_BIT6_QCOM                         0x40000000
#define GL_MULTISAMPLE_BUFFER_BIT7_QCOM                         0x80000000
#endif

/*------------------------------------------------------------------------*
 * End of extension tokens, start of corresponding extension functions
 *------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*
 * OES extension functions
 *------------------------------------------------------------------------*/

/* GL_OES_compressed_ETC1_RGB8_texture */
#ifndef GL_OES_compressed_ETC1_RGB8_texture
#define GL_OES_compressed_ETC1_RGB8_texture 1
#endif

/* GL_OES_compressed_paletted_texture */
#ifndef GL_OES_compressed_paletted_texture
#define GL_OES_compressed_paletted_texture 1
#endif

/* GL_OES_depth24 */
#ifndef GL_OES_depth24
#define GL_OES_depth24 1
#endif

/* GL_OES_depth32 */
#ifndef GL_OES_depth32
#define GL_OES_depth32 1
#endif

/* GL_OES_depth_texture */
#ifndef GL_OES_depth_texture
#define GL_OES_depth_texture 1
#endif

/* GL_OES_EGL_image */
#ifndef GL_OES_EGL_image
#define GL_OES_EGL_image 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glEGLImageTargetTexture2DOES (GLenum target, GLeglImageOES image);
GL_APICALL void GL_APIENTRY glEGLImageTargetRenderbufferStorageOES (GLenum target, GLeglImageOES image);
#endif
typedef void (GL_APIENTRYP PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) (GLenum target, GLeglImageOES image);
typedef void (GL_APIENTRYP PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC) (GLenum target, GLeglImageOES image);
#endif

/* GL_OES_element_index_uint */
#ifndef GL_OES_element_index_uint
#define GL_OES_element_index_uint 1
#endif

/* GL_OES_fbo_render_mipmap */
#ifndef GL_OES_fbo_render_mipmap
#define GL_OES_fbo_render_mipmap 1
#endif

/* GL_OES_fragment_precision_high */
#ifndef GL_OES_fragment_precision_high
#define GL_OES_fragment_precision_high 1
#endif

/* GL_OES_get_program_binary */
#ifndef GL_OES_get_program_binary
#define GL_OES_get_program_binary 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glGetProgramBinaryOES (GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary);
GL_APICALL void GL_APIENTRY glProgramBinaryOES (GLuint program, GLenum binaryFormat, const void *binary, GLint length);
#endif
typedef void (GL_APIENTRYP PFNGLGETPROGRAMBINARYOESPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary);
typedef void (GL_APIENTRYP PFNGLPROGRAMBINARYOESPROC) (GLuint program, GLenum binaryFormat, const void *binary, GLint length);
#endif

/* GL_OES_mapbuffer */
#ifndef GL_OES_mapbuffer
#define GL_OES_mapbuffer 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void* GL_APIENTRY glMapBufferOES (GLenum target, GLenum access);
GL_APICALL GLboolean GL_APIENTRY glUnmapBufferOES (GLenum target);
GL_APICALL void GL_APIENTRY glGetBufferPointervOES (GLenum target, GLenum pname, void** params);
#endif
typedef void* (GL_APIENTRYP PFNGLMAPBUFFEROESPROC) (GLenum target, GLenum access);
typedef GLboolean (GL_APIENTRYP PFNGLUNMAPBUFFEROESPROC) (GLenum target);
typedef void (GL_APIENTRYP PFNGLGETBUFFERPOINTERVOESPROC) (GLenum target, GLenum pname, void** params);
#endif

/* GL_OES_packed_depth_stencil */
#ifndef GL_OES_packed_depth_stencil
#define GL_OES_packed_depth_stencil 1
#endif

/* GL_OES_rgb8_rgba8 */
#ifndef GL_OES_rgb8_rgba8
#define GL_OES_rgb8_rgba8 1
#endif

/* GL_OES_standard_derivatives */
#ifndef GL_OES_standard_derivatives
#define GL_OES_standard_derivatives 1
#endif

/* GL_OES_stencil1 */
#ifndef GL_OES_stencil1
#define GL_OES_stencil1 1
#endif

/* GL_OES_stencil4 */
#ifndef GL_OES_stencil4
#define GL_OES_stencil4 1
#endif

/* GL_OES_texture_3D */
#ifndef GL_OES_texture_3D
#define GL_OES_texture_3D 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glTexImage3DOES (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels);
GL_APICALL void GL_APIENTRY glTexSubImage3DOES (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
GL_APICALL void GL_APIENTRY glCopyTexSubImage3DOES (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GL_APICALL void GL_APIENTRY glCompressedTexImage3DOES (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data);
GL_APICALL void GL_APIENTRY glCompressedTexSubImage3DOES (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data);
GL_APICALL void GL_APIENTRY glFramebufferTexture3DOES (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
#endif
typedef void (GL_APIENTRYP PFNGLTEXIMAGE3DOESPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
typedef void (GL_APIENTRYP PFNGLTEXSUBIMAGE3DOESPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
typedef void (GL_APIENTRYP PFNGLCOPYTEXSUBIMAGE3DOESPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (GL_APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DOESPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data);
typedef void (GL_APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data);
typedef void (GL_APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DOES) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
#endif

/* GL_OES_texture_float */
#ifndef GL_OES_texture_float
#define GL_OES_texture_float 1
#endif

/* GL_OES_texture_float_linear */
#ifndef GL_OES_texture_float_linear
#define GL_OES_texture_float_linear 1
#endif

/* GL_OES_texture_half_float */
#ifndef GL_OES_texture_half_float
#define GL_OES_texture_half_float 1
#endif

/* GL_OES_texture_half_float_linear */
#ifndef GL_OES_texture_half_float_linear
#define GL_OES_texture_half_float_linear 1
#endif

/* GL_OES_texture_npot */
#ifndef GL_OES_texture_npot
#define GL_OES_texture_npot 1
#endif

/* GL_OES_vertex_array_object */
#ifndef GL_OES_vertex_array_object
#define GL_OES_vertex_array_object 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glBindVertexArrayOES (GLuint array);
GL_APICALL void GL_APIENTRY glDeleteVertexArraysOES (GLsizei n, const GLuint *arrays);
GL_APICALL void GL_APIENTRY glGenVertexArraysOES (GLsizei n, GLuint *arrays);
GL_APICALL GLboolean GL_APIENTRY glIsVertexArrayOES (GLuint array);
#endif
typedef void (GL_APIENTRYP PFNGLBINDVERTEXARRAYOESPROC) (GLuint array);
typedef void (GL_APIENTRYP PFNGLDELETEVERTEXARRAYSOESPROC) (GLsizei n, const GLuint *arrays);
typedef void (GL_APIENTRYP PFNGLGENVERTEXARRAYSOESPROC) (GLsizei n, GLuint *arrays);
typedef GLboolean (GL_APIENTRYP PFNGLISVERTEXARRAYOESPROC) (GLuint array);
#endif

/* GL_OES_vertex_half_float */
#ifndef GL_OES_vertex_half_float
#define GL_OES_vertex_half_float 1
#endif

/* GL_OES_vertex_type_10_10_10_2 */
#ifndef GL_OES_vertex_type_10_10_10_2
#define GL_OES_vertex_type_10_10_10_2 1
#endif

/*------------------------------------------------------------------------*
 * AMD extension functions
 *------------------------------------------------------------------------*/

/* GL_AMD_compressed_3DC_texture */
#ifndef GL_AMD_compressed_3DC_texture
#define GL_AMD_compressed_3DC_texture 1
#endif

/* GL_AMD_compressed_ATC_texture */
#ifndef GL_AMD_compressed_ATC_texture
#define GL_AMD_compressed_ATC_texture 1
#endif

/* AMD_performance_monitor */
#ifndef GL_AMD_performance_monitor
#define GL_AMD_performance_monitor 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glGetPerfMonitorGroupsAMD (GLint *numGroups, GLsizei groupsSize, GLuint *groups);
GL_APICALL void GL_APIENTRY glGetPerfMonitorCountersAMD (GLuint group, GLint *numCounters, GLint *maxActiveCounters, GLsizei counterSize, GLuint *counters);
GL_APICALL void GL_APIENTRY glGetPerfMonitorGroupStringAMD (GLuint group, GLsizei bufSize, GLsizei *length, char *groupString);
GL_APICALL void GL_APIENTRY glGetPerfMonitorCounterStringAMD (GLuint group, GLuint counter, GLsizei bufSize, GLsizei *length, char *counterString);
GL_APICALL void GL_APIENTRY glGetPerfMonitorCounterInfoAMD (GLuint group, GLuint counter, GLenum pname, void *data);
GL_APICALL void GL_APIENTRY glGenPerfMonitorsAMD (GLsizei n, GLuint *monitors);
GL_APICALL void GL_APIENTRY glDeletePerfMonitorsAMD (GLsizei n, GLuint *monitors);
GL_APICALL void GL_APIENTRY glSelectPerfMonitorCountersAMD (GLuint monitor, GLboolean enable, GLuint group, GLint numCounters, GLuint *countersList);
GL_APICALL void GL_APIENTRY glBeginPerfMonitorAMD (GLuint monitor);
GL_APICALL void GL_APIENTRY glEndPerfMonitorAMD (GLuint monitor);
GL_APICALL void GL_APIENTRY glGetPerfMonitorCounterDataAMD (GLuint monitor, GLenum pname, GLsizei dataSize, GLuint *data, GLint *bytesWritten);
#endif
typedef void (GL_APIENTRYP PFNGLGETPERFMONITORGROUPSAMDPROC) (GLint *numGroups, GLsizei groupsSize, GLuint *groups);
typedef void (GL_APIENTRYP PFNGLGETPERFMONITORCOUNTERSAMDPROC) (GLuint group, GLint *numCounters, GLint *maxActiveCounters, GLsizei counterSize, GLuint *counters);
typedef void (GL_APIENTRYP PFNGLGETPERFMONITORGROUPSTRINGAMDPROC) (GLuint group, GLsizei bufSize, GLsizei *length, char *groupString);
typedef void (GL_APIENTRYP PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC) (GLuint group, GLuint counter, GLsizei bufSize, GLsizei *length, char *counterString);
typedef void (GL_APIENTRYP PFNGLGETPERFMONITORCOUNTERINFOAMDPROC) (GLuint group, GLuint counter, GLenum pname, void *data);
typedef void (GL_APIENTRYP PFNGLGENPERFMONITORSAMDPROC) (GLsizei n, GLuint *monitors);
typedef void (GL_APIENTRYP PFNGLDELETEPERFMONITORSAMDPROC) (GLsizei n, GLuint *monitors);
typedef void (GL_APIENTRYP PFNGLSELECTPERFMONITORCOUNTERSAMDPROC) (GLuint monitor, GLboolean enable, GLuint group, GLint numCounters, GLuint *countersList);
typedef void (GL_APIENTRYP PFNGLBEGINPERFMONITORAMDPROC) (GLuint monitor);
typedef void (GL_APIENTRYP PFNGLENDPERFMONITORAMDPROC) (GLuint monitor);
typedef void (GL_APIENTRYP PFNGLGETPERFMONITORCOUNTERDATAAMDPROC) (GLuint monitor, GLenum pname, GLsizei dataSize, GLuint *data, GLint *bytesWritten);
#endif

/* GL_AMD_program_binary_Z400 */
#ifndef GL_AMD_program_binary_Z400
#define GL_AMD_program_binary_Z400 1
#endif

/*------------------------------------------------------------------------*
 * EXT extension functions
 *------------------------------------------------------------------------*/

/* GL_EXT_blend_minmax */
#ifndef GL_EXT_blend_minmax
#define GL_EXT_blend_minmax 1
#endif

/* GL_EXT_discard_framebuffer */
#ifndef GL_EXT_discard_framebuffer
#define GL_EXT_discard_framebuffer 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glDiscardFramebufferEXT (GLenum target, GLsizei numAttachments, const GLenum *attachments);
#endif
typedef void (GL_APIENTRYP PFNGLDISCARDFRAMEBUFFEREXTPROC) (GLenum target, GLsizei numAttachments, const GLenum *attachments);
#endif

#ifndef GL_EXT_multi_draw_arrays
#define GL_EXT_multi_draw_arrays 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glMultiDrawArraysEXT (GLenum, GLint *, GLsizei *, GLsizei);
GL_APICALL void GL_APIENTRY glMultiDrawElementsEXT (GLenum, const GLsizei *, GLenum, const GLvoid* *, GLsizei);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (GL_APIENTRYP PFNGLMULTIDRAWARRAYSEXTPROC) (GLenum mode, GLint *first, GLsizei *count, GLsizei primcount);
typedef void (GL_APIENTRYP PFNGLMULTIDRAWELEMENTSEXTPROC) (GLenum mode, const GLsizei *count, GLenum type, const GLvoid* *indices, GLsizei primcount);
#endif

/* GL_EXT_read_format_bgra */
#ifndef GL_EXT_read_format_bgra
#define GL_EXT_read_format_bgra 1
#endif

/* GL_EXT_texture_filter_anisotropic */
#ifndef GL_EXT_texture_filter_anisotropic
#define GL_EXT_texture_filter_anisotropic 1
#endif

/* GL_EXT_texture_format_BGRA8888 */
#ifndef GL_EXT_texture_format_BGRA8888
#define GL_EXT_texture_format_BGRA8888 1
#endif

/* GL_EXT_texture_type_2_10_10_10_REV */
#ifndef GL_EXT_texture_type_2_10_10_10_REV
#define GL_EXT_texture_type_2_10_10_10_REV 1
#endif

/*------------------------------------------------------------------------*
 * IMG extension functions
 *------------------------------------------------------------------------*/

/* GL_IMG_program_binary */
#ifndef GL_IMG_program_binary
#define GL_IMG_program_binary 1
#endif

/* GL_IMG_read_format */
#ifndef GL_IMG_read_format
#define GL_IMG_read_format 1
#endif

/* GL_IMG_shader_binary */
#ifndef GL_IMG_shader_binary
#define GL_IMG_shader_binary 1
#endif

/* GL_IMG_texture_compression_pvrtc */
#ifndef GL_IMG_texture_compression_pvrtc
#define GL_IMG_texture_compression_pvrtc 1
#endif


/* GL_IMG_texture_stream */
#ifndef GL_IMG_texture_stream2
#define GL_IMG_texture_stream2 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glTexBindStreamIMG(GLint device, GLint deviceoffset);
GL_APICALL void GL_APIENTRY glGetTexStreamDeviceAttributeivIMG(GLint device, GLenum pname, GLint *params);
GL_APICALL const GLubyte * GL_APIENTRY glGetTexStreamDeviceNameIMG(GLint device);
#endif

typedef void (GL_APIENTRYP PFNGLTEXBINDSTREAMIMGPROC) (GLint device, GLint deviceoffset);
typedef const GLubyte *(GL_APIENTRYP PFNGLGETTEXSTREAMDEVICENAMEIMGPROC) (GLenum target);
typedef void (GL_APIENTRYP PFNGLGETTEXSTREAMDEVICEATTRIBUTEIVIMGPROC) (GLenum target, GLenum pname, GLint *params);

#endif

/* IMG_vertex_array_object  */
#ifndef GL_IMG_vertex_array_object 
#define GL_IMG_vertex_array_object 1

#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glBindVertexArrayIMG(GLuint vertexarray);
GL_APICALL void GL_APIENTRY glDeleteVertexArraysIMG(GLsizei n, const GLuint *vertexarrays);
GL_APICALL void GL_APIENTRY glGenVertexArraysIMG(GLsizei n, const GLuint *vertexarrays);
GL_APICALL GLboolean GL_APIENTRY glIsVertexArrayIMG (GLuint vertexarray);
#endif

typedef void (GL_APIENTRYP PFNGLBINDVERTEXARRAYIMGPROC) (GLuint vertexarray);
typedef void (GL_APIENTRYP PFNGLDELETEVERTEXARRAYSIMGPROC) (GLsizei n, const GLuint *vertexarrays);
typedef void (GL_APIENTRYP PFNGLGENVERTEXARRAYSIMGPROC) (GLsizei n, const GLuint *vertexarrays);
typedef GLboolean (GL_APIENTRYP PFNGLISVERTEXARRAYIMGPROC) (GLuint vertexarray);

#endif

/*------------------------------------------------------------------------*
 * NV extension functions
 *------------------------------------------------------------------------*/

/* GL_NV_fence */
#ifndef GL_NV_fence
#define GL_NV_fence 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glDeleteFencesNV (GLsizei, const GLuint *);
GL_APICALL void GL_APIENTRY glGenFencesNV (GLsizei, GLuint *);
GL_APICALL GLboolean GL_APIENTRY glIsFenceNV (GLuint);
GL_APICALL GLboolean GL_APIENTRY glTestFenceNV (GLuint);
GL_APICALL void GL_APIENTRY glGetFenceivNV (GLuint, GLenum, GLint *);
GL_APICALL void GL_APIENTRY glFinishFenceNV (GLuint);
GL_APICALL void GL_APIENTRY glSetFenceNV (GLuint, GLenum);
#endif
typedef void (GL_APIENTRYP PFNGLDELETEFENCESNVPROC) (GLsizei n, const GLuint *fences);
typedef void (GL_APIENTRYP PFNGLGENFENCESNVPROC) (GLsizei n, GLuint *fences);
typedef GLboolean (GL_APIENTRYP PFNGLISFENCENVPROC) (GLuint fence);
typedef GLboolean (GL_APIENTRYP PFNGLTESTFENCENVPROC) (GLuint fence);
typedef void (GL_APIENTRYP PFNGLGETFENCEIVNVPROC) (GLuint fence, GLenum pname, GLint *params);
typedef void (GL_APIENTRYP PFNGLFINISHFENCENVPROC) (GLuint fence);
typedef void (GL_APIENTRYP PFNGLSETFENCENVPROC) (GLuint fence, GLenum condition);
#endif

/*------------------------------------------------------------------------*
 * QCOM extension functions
 *------------------------------------------------------------------------*/

/* GL_QCOM_driver_control */
#ifndef GL_QCOM_driver_control
#define GL_QCOM_driver_control 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glGetDriverControlsQCOM (GLint *num, GLsizei size, GLuint *driverControls);
GL_APICALL void GL_APIENTRY glGetDriverControlStringQCOM (GLuint driverControl, GLsizei bufSize, GLsizei *length, char *driverControlString);
GL_APICALL void GL_APIENTRY glEnableDriverControlQCOM (GLuint driverControl);
GL_APICALL void GL_APIENTRY glDisableDriverControlQCOM (GLuint driverControl);
#endif
typedef void (GL_APIENTRYP PFNGLGETDRIVERCONTROLSQCOMPROC) (GLint *num, GLsizei size, GLuint *driverControls);
typedef void (GL_APIENTRYP PFNGLGETDRIVERCONTROLSTRINGQCOMPROC) (GLuint driverControl, GLsizei bufSize, GLsizei *length, char *driverControlString);
typedef void (GL_APIENTRYP PFNGLENABLEDRIVERCONTROLQCOMPROC) (GLuint driverControl);
typedef void (GL_APIENTRYP PFNGLDISABLEDRIVERCONTROLQCOMPROC) (GLuint driverControl);
#endif

/* GL_QCOM_extended_get */
#ifndef GL_QCOM_extended_get
#define GL_QCOM_extended_get 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glExtGetTexturesQCOM (GLuint *textures, GLint maxTextures, GLint *numTextures);
GL_APICALL void GL_APIENTRY glExtGetBuffersQCOM (GLuint *buffers, GLint maxBuffers, GLint *numBuffers);
GL_APICALL void GL_APIENTRY glExtGetRenderbuffersQCOM (GLuint *renderbuffers, GLint maxRenderbuffers, GLint *numRenderbuffers);
GL_APICALL void GL_APIENTRY glExtGetFramebuffersQCOM (GLuint *framebuffers, GLint maxFramebuffers, GLint *numFramebuffers);
GL_APICALL void GL_APIENTRY glExtGetTexLevelParameterivQCOM (GLuint texture, GLenum face, GLint level, GLenum pname, GLint *params);
GL_APICALL void GL_APIENTRY glExtTexObjectStateOverrideiQCOM (GLenum target, GLenum pname, GLint param);
GL_APICALL void GL_APIENTRY glExtGetTexSubImageQCOM (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, void *texels);
GL_APICALL void GL_APIENTRY glExtGetBufferPointervQCOM (GLenum target, void **params);
#endif
typedef void (GL_APIENTRYP PFNGLEXTGETTEXTURESQCOMPROC) (GLuint *textures, GLint maxTextures, GLint *numTextures);
typedef void (GL_APIENTRYP PFNGLEXTGETBUFFERSQCOMPROC) (GLuint *buffers, GLint maxBuffers, GLint *numBuffers);
typedef void (GL_APIENTRYP PFNGLEXTGETRENDERBUFFERSQCOMPROC) (GLuint *renderbuffers, GLint maxRenderbuffers, GLint *numRenderbuffers);
typedef void (GL_APIENTRYP PFNGLEXTGETFRAMEBUFFERSQCOMPROC) (GLuint *framebuffers, GLint maxFramebuffers, GLint *numFramebuffers);
typedef void (GL_APIENTRYP PFNGLEXTGETTEXLEVELPARAMETERIVQCOMPROC) (GLuint texture, GLenum face, GLint level, GLenum pname, GLint *params);
typedef void (GL_APIENTRYP PFNGLEXTTEXOBJECTSTATEOVERRIDEIQCOMPROC) (GLenum target, GLenum pname, GLint param);
typedef void (GL_APIENTRYP PFNGLEXTGETTEXSUBIMAGEQCOMPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, void *texels);
typedef void (GL_APIENTRYP PFNGLEXTGETBUFFERPOINTERVQCOMPROC) (GLenum target, void **params);
#endif

/* GL_QCOM_extended_get2 */
#ifndef GL_QCOM_extended_get2
#define GL_QCOM_extended_get2 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glExtGetShadersQCOM (GLuint *shaders, GLint maxShaders, GLint *numShaders);
GL_APICALL void GL_APIENTRY glExtGetProgramsQCOM (GLuint *programs, GLint maxPrograms, GLint *numPrograms);
GL_APICALL void GL_APIENTRY glExtIsProgramBinaryQCOM (GLuint program);
GL_APICALL void GL_APIENTRY glExtGetProgramBinarySourceQCOM (GLuint program, GLenum shadertype, char *source, GLint *length);
#endif
typedef void (GL_APIENTRYP PFNGLEXTGETSHADERSQCOMPROC) (GLuint *shaders, GLint maxShaders, GLint *numShaders);
typedef void (GL_APIENTRYP PFNGLEXTGETPROGRAMSQCOMPROC) (GLuint *programs, GLint maxPrograms, GLint *numPrograms);
typedef void (GL_APIENTRYP PFNGLEXTISPROGRAMBINARYQCOMPROC) (GLuint program);
typedef void (GL_APIENTRYP PFNGLEXTGETPROGRAMBINARYSOURCEQCOMPROC) (GLuint program, GLenum shadertype, char *source, GLint *length);
#endif

/* GL_QCOM_perfmon_global_mode */
#ifndef GL_QCOM_perfmon_global_mode
#define GL_QCOM_perfmon_global_mode 1
#endif

/* GL_QCOM_writeonly_rendering */
#ifndef GL_QCOM_writeonly_rendering
#define GL_QCOM_writeonly_rendering 1
#endif

/* GL_QCOM_tiled_rendering */
#ifndef GL_QCOM_tiled_rendering
#define GL_QCOM_tiled_rendering 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glStartTilingQCOM (GLuint x, GLuint y, GLuint width, GLuint height, GLbitfield preserveMask);
GL_APICALL void GL_APIENTRY glEndTilingQCOM (GLbitfield preserveMask);
#endif
typedef void (GL_APIENTRYP PFNGLSTARTTILINGQCOMPROC) (GLuint x, GLuint y, GLuint width, GLuint height, GLbitfield preserveMask);
typedef void (GL_APIENTRYP PFNGLENDTILINGQCOMPROC) (GLbitfield preserveMask);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __gl2ext_h_ */
