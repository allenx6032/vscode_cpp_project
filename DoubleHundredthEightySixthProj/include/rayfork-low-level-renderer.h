#ifndef RAYFORK_LOW_LEVEL_RENDERER_H
#define RAYFORK_LOW_LEVEL_RENDERER_H

#include "rayfork-image.h"

#pragma region backend selection

#if !defined(RAYFORK_GRAPHICS_BACKEND_GL_33) && !defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3) && !defined(RAYFORK_GRAPHICS_BACKEND_METAL) && !defined(RAYFORK_GRAPHICS_BACKEND_DIRECTX)
    #define RAYFORK_NO_GRAPHICS_BACKEND_SELECTED_BY_THE_USER (1)
#endif

// If no graphics backend was set, choose OpenGL33 on desktop and OpenGL ES3 on mobile
#if RAYFORK_NO_GRAPHICS_BACKEND_SELECTED_BY_THE_USER
    #if defined(rayfork_platform_windows) || defined(rayfork_platform_linux) || defined(rayfork_platform_macos)
        #define RAYFORK_GRAPHICS_BACKEND_GL_33 (1)
    #else // if on mobile
        #define RAYFORK_GRAPHICS_BACKEND_GL_ES3 (1)
    #endif
#endif

// Check to make sure only one graphics backend was selected
#if (defined(RAYFORK_GRAPHICS_BACKEND_GL_33) + defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3) + defined(RAYFORK_GRAPHICS_BACKEND_METAL) + defined(RAYFORK_GRAPHICS_BACKEND_DIRECTX)) != 1
    #error rayfork error: you can only set one graphics backend but 2 or more were detected.
#endif

#pragma endregion

#pragma region constants

#ifndef RF_DEFAULT_BATCH_ELEMENTS_COUNT
    #if defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3) || defined(RAYFORK_GRAPHICS_BACKEND_METAL)
        #define RF_DEFAULT_BATCH_ELEMENTS_COUNT (2048)
    #else
        #define RF_DEFAULT_BATCH_ELEMENTS_COUNT (8192)
    #endif
#endif

#if !defined(RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT)
    #define RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT (1) // Max number of buffers for batching (multi-buffering)
#endif

#if !defined(RF_MAX_MATRIX_STACK_SIZE)
    #define RF_MAX_MATRIX_STACK_SIZE (32) // Max size of rf_mat rf__ctx->gl_ctx.stack
#endif

#if !defined(RF_DEFAULT_BATCH_DRAW_CALLS_COUNT)
    #define RF_DEFAULT_BATCH_DRAW_CALLS_COUNT (256) // Max rf__ctx->gl_ctx.draws by state changes (mode, texture)
#endif

// Shader and material limits
#if !defined(RF_MAX_SHADER_LOCATIONS)
    #define RF_MAX_SHADER_LOCATIONS (32) // Maximum number of predefined locations stored in shader struct
#endif

#if !defined(RF_MAX_MATERIAL_MAPS)
    #define RF_MAX_MATERIAL_MAPS (12) // Maximum number of texture maps stored in shader struct
#endif

#if !defined(RF_MAX_TEXT_BUFFER_LENGTH)
    #define RF_MAX_TEXT_BUFFER_LENGTH (1024) // Size of internal rf_internal buffers used on some functions:
#endif

#if !defined(RF_MAX_MESH_VBO)
    #define RF_MAX_MESH_VBO (7) // Maximum number of vbo per mesh
#endif

// Default vertex attribute names on shader to set location points
#define RF_DEFAULT_ATTRIB_POSITION_NAME  "vertex_position"    // shader-location = 0
#define RF_DEFAULT_ATTRIB_TEXCOORD_NAME  "vertex_tex_coord"   // shader-location = 1
#define RF_DEFAULT_ATTRIB_NORMAL_NAME    "vertex_normal"      // shader-location = 2
#define RF_DEFAULT_ATTRIB_COLOR_NAME     "vertex_color"       // shader-location = 3
#define RF_DEFAULT_ATTRIB_TANGENT_NAME   "vertex_tangent"     // shader-location = 4
#define RF_DEFAULT_ATTRIB_TEXCOORD2_NAME "vertex_tex_coord2"  // shader-location = 5

#pragma endregion

// Matrix modes (equivalent to OpenGL)
typedef enum rf_matrix_mode
{
    RF_MODELVIEW  = 0x1700,  // GL_MODELVIEW
    RF_PROJECTION = 0x1701,  // GL_PROJECTION
    RF_TEXTURE    = 0x1702,  // GL_TEXTURE
} rf_matrix_mode;

// Drawing modes (equivalent to OpenGL)
typedef enum rf_drawing_mode
{
    RF_LINES     = 0x0001, // GL_LINES
    RF_TRIANGLES = 0x0004, // GL_TRIANGLES
    RF_QUADS     = 0x0007, // GL_QUADS
} rf_drawing_mode;

// Shader location point type
typedef enum rf_shader_location_index
{
    RF_LOC_VERTEX_POSITION   = 0,
    RF_LOC_VERTEX_TEXCOORD01 = 1,
    RF_LOC_VERTEX_TEXCOORD02 = 2,
    RF_LOC_VERTEX_NORMAL     = 3,
    RF_LOC_VERTEX_TANGENT    = 4,
    RF_LOC_VERTEX_COLOR      = 5,
    RF_LOC_MATRIX_MVP        = 6,
    RF_LOC_MATRIX_MODEL      = 7,
    RF_LOC_MATRIX_VIEW       = 8,
    RF_LOC_MATRIX_PROJECTION = 9,
    RF_LOC_VECTOR_VIEW       = 10,
    RF_LOC_COLOR_DIFFUSE     = 11,
    RF_LOC_COLOR_SPECULAR    = 12,
    RF_LOC_COLOR_AMBIENT     = 13,

    // These 2 are intentionally the same
    RF_LOC_MAP_ALBEDO        = 14,
    RF_LOC_MAP_DIFFUSE       = 14,

    // These 2 are intentionally the same
    RF_LOC_MAP_METALNESS     = 15,
    RF_LOC_MAP_SPECULAR      = 15,

    RF_LOC_MAP_NORMAL        = 16,
    RF_LOC_MAP_ROUGHNESS     = 17,
    RF_LOC_MAP_OCCLUSION     = 18,
    RF_LOC_MAP_EMISSION      = 19,
    RF_LOC_MAP_HEIGHT        = 20,
    RF_LOC_MAP_CUBEMAP       = 21,
    RF_LOC_MAP_IRRADIANCE    = 22,
    RF_LOC_MAP_PREFILTER     = 23,
    RF_LOC_MAP_BRDF          = 24,
} rf_shader_location_index;

// rf_shader uniform data types
typedef enum rf_shader_uniform_data_type
{
    RF_UNIFORM_FLOAT = 0,
    RF_UNIFORM_VEC2,
    RF_UNIFORM_VEC3,
    RF_UNIFORM_VEC4,
    RF_UNIFORM_INT,
    RF_UNIFORM_IVEC2,
    RF_UNIFORM_IVEC3,
    RF_UNIFORM_IVEC4,
    RF_UNIFORM_SAMPLER2D
} rf_shader_uniform_data_type;

// rf_texture parameters: filter mode
// NOTE 1: Filtering considers mipmaps if available in the texture
// NOTE 2: Filter is accordingly set for minification and magnification
typedef enum rf_texture_filter_mode
{
    RF_FILTER_POINT = 0, // No filter, just pixel aproximation
    RF_FILTER_BILINEAR, // Linear filtering
    RF_FILTER_TRILINEAR, // Trilinear filtering (linear with mipmaps)
    RF_FILTER_ANISOTROPIC_4x, // Anisotropic filtering 4x
    RF_FILTER_ANISOTROPIC_8x, // Anisotropic filtering 8x
    RF_FILTER_ANISOTROPIC_16x, // Anisotropic filtering 16x
} rf_texture_filter_mode;

// Cubemap layout type
typedef enum rf_cubemap_layout_type
{
    RF_CUBEMAP_AUTO_DETECT = 0, // Automatically detect layout type
    RF_CUBEMAP_LINE_VERTICAL, // Layout is defined by a vertical line with faces
    RF_CUBEMAP_LINE_HORIZONTAL, // Layout is defined by an horizontal line with faces
    RF_CUBEMAP_CROSS_THREE_BY_FOUR, // Layout is defined by a 3x4 cross with cubemap faces
    RF_CUBEMAP_CROSS_FOUR_BY_TREE, // Layout is defined by a 4x3 cross with cubemap faces
    RF_CUBEMAP_PANORAMA // Layout is defined by a panorama image (equirectangular map)
} rf_cubemap_layout_type;

// rf_texture parameters: wrap mode
typedef enum rf_texture_wrap_mode
{
    RF_WRAP_REPEAT = 0, // Repeats texture in tiled mode
    RF_WRAP_CLAMP, // Clamps texture to edge pixel in tiled mode
    RF_WRAP_MIRROR_REPEAT, // Mirrors and repeats the texture in tiled mode
    RF_WRAP_MIRROR_CLAMP // Mirrors and clamps to border the texture in tiled mode
} rf_texture_wrap_mode;

// rf_color blending modes (pre-defined)
typedef enum rf_blend_mode
{
    RF_BLEND_ALPHA = 0, // Blend textures considering alpha (default)
    RF_BLEND_ADDITIVE, // Blend textures adding colors
    RF_BLEND_MULTIPLIED // Blend textures multiplying colors
} rf_blend_mode;

typedef struct rf_shader
{
    unsigned int id; // rf_shader program id
    int locs[RF_MAX_SHADER_LOCATIONS];       // rf_shader locations array (RF_MAX_SHADER_LOCATIONS)
} rf_shader;

typedef struct rf_gfx_pixel_format
{
    unsigned int internal_format;
    unsigned int format;
    unsigned int type;
    rf_bool valid;
} rf_gfx_pixel_format;

typedef struct rf_texture2d
{
    unsigned int id;        // OpenGL texture id
    int width;              // rf_texture base width
    int height;             // rf_texture base height
    int mipmaps;            // Mipmap levels, 1 by default
    rf_pixel_format format; // Data format (rf_pixel_format type)
    rf_bool valid;
} rf_texture2d, rf_texture_cubemap;

typedef struct rf_render_texture2d
{
    unsigned int id;            // OpenGL Framebuffer Object (FBO) id
    rf_texture2d texture;       // rf_color buffer attachment texture
    rf_texture2d depth;         // Depth buffer attachment texture
    int          depth_texture; // Track if depth attachment is a texture or renderbuffer
} rf_render_texture2d;

struct rf_vertex_buffer;
struct rf_mesh;
struct rf_material;
struct rf_gfx_backend_data;

#pragma region shader
rf_public rf_shader rf_gfx_load_shader(const char* vs_code, const char* fs_code); // Load shader from code strings. If shader string is NULL, using default vertex/fragment shaders
rf_public void rf_gfx_unload_shader(rf_shader shader); // Unload shader from GPU memory (VRAM)
rf_public int rf_gfx_get_shader_location(rf_shader shader, const char* uniform_name); // Get shader uniform location
rf_public void rf_gfx_set_shader_value(rf_shader shader, int uniform_loc, const void* value, int uniform_name); // Set shader uniform value
rf_public void rf_gfx_set_shader_value_v(rf_shader shader, int uniform_loc, const void* value, int uniform_name, int count); // Set shader uniform value vector
rf_public void rf_gfx_set_shader_value_matrix(rf_shader shader, int uniform_loc, rf_mat mat); // Set shader uniform value (matrix 4x4)
rf_public void rf_gfx_set_shader_value_texture(rf_shader shader, int uniform_loc, rf_texture2d texture); // Set shader uniform value for texture
#pragma endregion

#pragma region gfx api
rf_public rf_mat rf_gfx_get_matrix_projection(); // Return internal rf__ctx->gl_ctx.projection matrix
rf_public rf_mat rf_gfx_get_matrix_modelview(); // Return internal rf__ctx->gl_ctx.modelview matrix
rf_public void rf_gfx_set_matrix_projection(rf_mat proj); // Set a custom projection matrix (replaces internal rf__ctx->gl_ctx.projection matrix)
rf_public void rf_gfx_set_matrix_modelview(rf_mat view); // Set a custom rf__ctx->gl_ctx.modelview matrix (replaces internal rf__ctx->gl_ctx.modelview matrix)

rf_public void rf_gfx_blend_mode(rf_blend_mode mode); // Choose the blending mode (alpha, additive, multiplied)
rf_public void rf_gfx_matrix_mode(rf_matrix_mode mode); // Choose the current matrix to be transformed
rf_public void rf_gfx_push_matrix(); // Push the current matrix to rf_global_gl_stack
rf_public void rf_gfx_pop_matrix(); // Pop lattest inserted matrix from rf_global_gl_stack
rf_public void rf_gfx_load_identity(); // Reset current matrix to identity matrix
rf_public void rf_gfx_translatef(float x, float y, float z); // Multiply the current matrix by a translation matrix
rf_public void rf_gfx_rotatef(float angleDeg, float x, float y, float z); // Multiply the current matrix by a rotation matrix
rf_public void rf_gfx_scalef(float x, float y, float z); // Multiply the current matrix by a scaling matrix
rf_public void rf_gfx_mult_matrixf(float* matf); // Multiply the current matrix by another matrix
rf_public void rf_gfx_frustum(double left, double right, double bottom, double top, double znear, double zfar);
rf_public void rf_gfx_ortho(double left, double right, double bottom, double top, double znear, double zfar);
rf_public void rf_gfx_viewport(int x, int y, int width, int height); // Set the viewport area

// Functions Declaration - Vertex level operations
rf_public void rf_gfx_begin(rf_drawing_mode mode); // Initialize drawing mode (how to organize vertex)
rf_public void rf_gfx_end(); // Finish vertex providing
rf_public void rf_gfx_vertex2i(int x, int y); // Define one vertex (position) - 2 int
rf_public void rf_gfx_vertex2f(float x, float y); // Define one vertex (position) - 2 float
rf_public void rf_gfx_vertex3f(float x, float y, float z); // Define one vertex (position) - 3 float
rf_public void rf_gfx_tex_coord2f(float x, float y); // Define one vertex (texture coordinate) - 2 float
rf_public void rf_gfx_normal3f(float x, float y, float z); // Define one vertex (normal) - 3 float
rf_public void rf_gfx_color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a); // Define one vertex (color) - 4 unsigned char
rf_public void rf_gfx_color3f(float x, float y, float z); // Define one vertex (color) - 3 float
rf_public void rf_gfx_color4f(float x, float y, float z, float w); // Define one vertex (color) - 4 float

rf_public void rf_gfx_enable_texture(unsigned int id); // Enable texture usage
rf_public void rf_gfx_disable_texture(); // Disable texture usage
rf_public void rf_gfx_set_texture_wrap(rf_texture2d texture, rf_texture_wrap_mode wrap_mode); // Set texture parameters (wrap mode/filter mode)
rf_public void rf_gfx_set_texture_filter(rf_texture2d texture, rf_texture_filter_mode filter_mode); // Set filter for texture
rf_public void rf_gfx_enable_render_texture(unsigned int id); // Enable render texture (fbo)
rf_public void rf_gfx_disable_render_texture(void); // Disable render texture (fbo), return to default framebuffer
rf_public void rf_gfx_enable_depth_test(void); // Enable depth test
rf_public void rf_gfx_disable_depth_test(void); // Disable depth test
rf_public void rf_gfx_enable_backface_culling(void); // Enable backface culling
rf_public void rf_gfx_disable_backface_culling(void); // Disable backface culling
rf_public void rf_gfx_enable_scissor_test(void); // Enable scissor test
rf_public void rf_gfx_disable_scissor_test(void); // Disable scissor test
rf_public void rf_gfx_scissor(int x, int y, int width, int height); // Scissor test
rf_public void rf_gfx_enable_wire_mode(void); // Enable wire mode
rf_public void rf_gfx_disable_wire_mode(void); // Disable wire mode
rf_public void rf_gfx_delete_textures(unsigned int id); // Delete OpenGL texture from GPU
rf_public void rf_gfx_delete_render_textures(rf_render_texture2d target); // Delete render textures (fbo) from GPU
rf_public void rf_gfx_delete_shader(unsigned int id); // Delete OpenGL shader program from GPU
rf_public void rf_gfx_delete_vertex_arrays(unsigned int id); // Unload vertex data (VAO) from GPU memory
rf_public void rf_gfx_delete_buffers(unsigned int id); // Unload vertex data (VBO) from GPU memory
rf_public void rf_gfx_clear_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a); // Clear color buffer with color
rf_public void rf_gfx_clear_screen_buffers(void); // Clear used screen buffers (color and depth)
rf_public void rf_gfx_update_buffer(int buffer_id, void* data, int data_size); // Update GPU buffer with new data
rf_public unsigned int rf_gfx_load_attrib_buffer(unsigned int vao_id, int shader_loc, void* buffer, int size, rf_bool dynamic); // Load a new attributes buffer
rf_public void rf_gfx_init_vertex_buffer(struct rf_vertex_buffer* vertex_buffer);

rf_public void rf_gfx_close(); // De-inititialize rf gfx (buffers, shaders, textures)
rf_public void rf_gfx_draw(); // Update and draw default internal buffers

rf_public rf_bool rf_gfx_check_buffer_limit(int v_count); // Check internal buffer overflow for a given number of vertex
rf_public void rf_gfx_set_debug_marker(const char* text); // Set debug marker for analysis

// Textures data management
rf_public unsigned int rf_gfx_load_texture(void* data, int width, int height, rf_pixel_format format, int mipmap_count); // Load texture in GPU
rf_public unsigned int rf_gfx_load_texture_depth(int width, int height, int bits, rf_bool use_render_buffer); // Load depth texture/renderbuffer (to be attached to fbo)
rf_public unsigned int rf_gfx_load_texture_cubemap(void* data, int size, rf_pixel_format format); // Load texture cubemap
rf_public void rf_gfx_update_texture(unsigned int id, int width, int height, rf_pixel_format format, const void* pixels, int pixels_size); // Update GPU texture with new data
rf_public rf_gfx_pixel_format rf_gfx_get_internal_texture_formats(rf_pixel_format format); // Get OpenGL internal formats
rf_public void rf_gfx_unload_texture(unsigned int id); // Unload texture from GPU memory

rf_public void rf_gfx_generate_mipmaps(rf_texture2d* texture); // Generate mipmap data for selected texture
rf_public rf_image rf_gfx_read_texture_pixels_to_buffer(rf_texture2d texture, void* dst, int dst_size);
rf_public rf_image rf_gfx_read_texture_pixels(rf_texture2d texture, rf_allocator allocator);
rf_public void rf_gfx_read_screen_pixels(rf_color* dst, int width, int height); // Read screen pixel data (color buffer)

// Render texture management (fbo)
rf_public rf_render_texture2d rf_gfx_load_render_texture(int width, int height, rf_pixel_format format, int depth_bits, rf_bool use_depth_texture); // Load a render texture (with color and depth attachments)
rf_public void rf_gfx_render_texture_attach(rf_render_texture2d target, unsigned int id, int attach_type); // Attach texture/renderbuffer to an fbo
rf_public rf_bool rf_gfx_render_texture_complete(rf_render_texture2d target); // Verify render texture is complete

// Vertex data management
rf_public void rf_gfx_load_mesh(struct rf_mesh* mesh, rf_bool dynamic); // Upload vertex data into GPU and provided VAO/VBO ids
rf_public void rf_gfx_update_mesh(struct rf_mesh mesh, int buffer, int num); // Update vertex or index data on GPU (upload new data to one buffer)
rf_public void rf_gfx_update_mesh_at(struct rf_mesh mesh, int buffer, int num, int index); // Update vertex or index data on GPU, at index
rf_public void rf_gfx_draw_mesh(struct rf_mesh mesh, struct rf_material material, rf_mat transform); // Draw a 3d mesh with material and transform
rf_public void rf_gfx_unload_mesh(struct rf_mesh mesh); // Unload mesh data from CPU and GPU
#pragma endregion

#pragma region gfx backends

#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33) || defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
    #include "rayfork-backend-opengl.h"
#endif

#if defined(RAYFORK_GRAPHICS_BACKEND_METAL) || defined(RAYFORK_GRAPHICS_BACKEND_DIRECTX)
    #include "rayfork-backend-sokol.h"
#endif

#pragma endregion

// The render batch must come after the gfx backends since it depends on the definitions of the backend
#pragma region render batch
typedef struct rf_render_batch
{
    rf_int vertex_buffers_count;
    rf_int current_buffer;
    rf_vertex_buffer* vertex_buffers;

    rf_int draw_calls_size;
    rf_int draw_calls_counter;
    rf_draw_call* draw_calls;
    float current_depth; // Current depth value for next draw

    rf_bool valid;
} rf_render_batch;

typedef struct rf_one_element_vertex_buffer
{
    rf_gfx_vertex_data_type       vertices  [1 * RF_GFX_VERTEX_COMPONENT_COUNT      ];
    rf_gfx_texcoord_data_type     texcoords [1 * RF_GFX_TEXCOORD_COMPONENT_COUNT    ];
    rf_gfx_color_data_type        colors    [1 * RF_GFX_COLOR_COMPONENT_COUNT       ];
    rf_gfx_vertex_index_data_type indices   [1 * RF_GFX_VERTEX_INDEX_COMPONENT_COUNT];
} rf_one_element_vertex_buffer;

typedef struct rf_default_vertex_buffer
{
    rf_gfx_vertex_data_type       vertices  [RF_DEFAULT_BATCH_ELEMENTS_COUNT * RF_GFX_VERTEX_COMPONENT_COUNT      ];
    rf_gfx_texcoord_data_type     texcoords [RF_DEFAULT_BATCH_ELEMENTS_COUNT * RF_GFX_TEXCOORD_COMPONENT_COUNT    ];
    rf_gfx_color_data_type        colors    [RF_DEFAULT_BATCH_ELEMENTS_COUNT * RF_GFX_COLOR_COMPONENT_COUNT       ];
    rf_gfx_vertex_index_data_type indices   [RF_DEFAULT_BATCH_ELEMENTS_COUNT * RF_GFX_VERTEX_INDEX_COMPONENT_COUNT];
} rf_default_vertex_buffer;

typedef struct rf_default_render_batch
{
    rf_vertex_buffer         vertex_buffers        [RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT];
    rf_draw_call             draw_calls            [RF_DEFAULT_BATCH_DRAW_CALLS_COUNT    ];
    rf_default_vertex_buffer vertex_buffers_memory [RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT];
} rf_default_render_batch;

rf_public rf_render_batch rf_create_custom_render_batch_from_buffers(rf_vertex_buffer* vertex_buffers, rf_int vertex_buffers_count, rf_draw_call* draw_calls, rf_int draw_calls_count);
rf_public rf_render_batch rf_create_custom_render_batch(rf_int vertex_buffers_count, rf_int draw_calls_count, rf_int vertex_buffer_elements_count, rf_allocator allocator);
rf_public rf_render_batch rf_create_default_render_batch(rf_allocator allocator);

rf_public void rf_set_active_render_batch(rf_render_batch* batch);
rf_public void rf_unload_render_batch(rf_render_batch batch, rf_allocator allocator);
#pragma endregion

#endif // RAYFORK_RAYFORK_LOW_LEVEL_RENDERER_H