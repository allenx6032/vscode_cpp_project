#ifndef GP_GRAPHICS_H_
#define GP_GRAPHICS_H_

#include "config.h"

/*************************************************************************
*
*************************************************************************/

typedef struct gp_canvas_t gp_canvas_t;

typedef void * (*gp_malloc_t)(gp_size_t _size, void * _ud);
typedef void * (*gp_realloc_t)(void * _ptr, gp_size_t _size, void * _ud);
typedef void (*gp_free_t)(void * _ptr, void * _ud);

/*************************************************************************
*
*************************************************************************/

gp_result_t gp_canvas_create( gp_canvas_t ** _canvas, gp_malloc_t _malloc, gp_realloc_t _realloc, gp_free_t _free, void * _ud );
gp_result_t gp_canvas_destroy( gp_canvas_t * _canvas );
gp_result_t gp_canvas_clear( gp_canvas_t * _canvas );

/*************************************************************************
*
*************************************************************************/

float gp_get_default_thickness( void );
float gp_get_default_penumbra( void );
float gp_get_default_outline_width( void );
gp_uint8_t gp_get_default_curve_quality( void );
gp_uint8_t gp_get_default_ellipse_quality( void );
gp_uint8_t gp_get_default_rect_quality( void );

/*************************************************************************
*
*************************************************************************/

gp_result_t gp_set_thickness( gp_canvas_t * _canvas, float _thickness );
gp_result_t gp_get_thickness( const gp_canvas_t * _canvas, float * _thickness );

gp_result_t gp_set_penumbra( gp_canvas_t * _canvas, float _penumbra );
gp_result_t gp_get_penumbra( const gp_canvas_t * _canvas, float * _penumbra );

gp_result_t gp_set_outline_width( gp_canvas_t * _canvas, float _width );
gp_result_t gp_get_outline_width( const gp_canvas_t * _canvas, float * _width );

gp_result_t gp_set_outline_color( gp_canvas_t * _canvas, float _r, float _g, float _b, float _a );
gp_result_t gp_get_outline_color( const gp_canvas_t * _canvas, gp_color_t * _color );

gp_result_t gp_set_color( gp_canvas_t * _canvas, float _r, float _g, float _b, float _a );
gp_result_t gp_get_color( const gp_canvas_t * _canvas, gp_color_t * _color );

gp_result_t gp_set_uv_offset( gp_canvas_t * _canvas, float _ou, float _ov, float _su, float _sv );
gp_result_t gp_get_uv_offset( const gp_canvas_t * _canvas, float * _ou, float * _ov, float * _su, float * _sv );

gp_result_t gp_set_curve_quality( gp_canvas_t * _canvas, gp_uint8_t _quality );
gp_result_t gp_get_curve_quality( const gp_canvas_t * _canvas, gp_uint8_t * _quality );

gp_result_t gp_set_ellipse_quality( gp_canvas_t * _canvas, gp_uint8_t _quality );
gp_result_t gp_get_ellipse_quality( const gp_canvas_t * _canvas, gp_uint8_t * _quality );

gp_result_t gp_set_rect_quality( gp_canvas_t * _canvas, gp_uint8_t _quality );
gp_result_t gp_get_rect_quality( const gp_canvas_t * _canvas, gp_uint8_t * _quality );

gp_result_t gp_begin_fill( gp_canvas_t * _canvas );
gp_result_t gp_end_fill( gp_canvas_t * _canvas );

gp_result_t gp_push_state( gp_canvas_t * _canvas );
gp_result_t gp_pop_state( gp_canvas_t * _canvas );

/*************************************************************************
*
*************************************************************************/

gp_result_t gp_move_to( gp_canvas_t * _canvas, float _x, float _y );
gp_result_t gp_line_to( gp_canvas_t * _canvas, float _x, float _y );
gp_result_t gp_quadratic_curve_to( gp_canvas_t * _canvas, float _p0x, float _p0y, float _x, float _y );
gp_result_t gp_bezier_curve_to( gp_canvas_t * _canvas, float _p0x, float _p0y, float _p1x, float _p1y, float _x, float _y );

gp_result_t gp_rect( gp_canvas_t * _canvas, float _x, float _y, float _width, float _height );
gp_result_t gp_rounded_rect( gp_canvas_t * _canvas, float _x, float _y, float _width, float _height, float _radius );
gp_result_t gp_circle( gp_canvas_t * _canvas, float _x, float _y, float _radius );
gp_result_t gp_ellipse( gp_canvas_t * _canvas, float _x, float _y, float _width, float _height );

/*************************************************************************
*
*************************************************************************/

typedef struct gp_mesh_t
{
    gp_uint16_t vertex_count;
    gp_uint16_t index_count;

    gp_color_t color;

    void * positions_buffer;
    gp_size_t positions_offset;
    gp_size_t positions_stride;

    void * colors_buffer;
    gp_size_t colors_offset;
    gp_size_t colors_stride;

    void * uv_buffer;
    gp_size_t uv_offset;
    gp_size_t uv_stride;

    void * indices_buffer;
    gp_size_t indices_offset;
    gp_size_t indices_stride;
} gp_mesh_t;

gp_result_t gp_calculate_mesh_size( const gp_canvas_t * _canvas, gp_mesh_t * _mesh );
gp_result_t gp_render( const gp_canvas_t * _canvas, const gp_mesh_t * _mesh );

#endif