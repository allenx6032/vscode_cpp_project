#ifndef GP_ROUNDED_RECT_H_
#define GP_ROUNDED_RECT_H_

#include "graphics.h"

gp_result_t gp_calculate_mesh_rounded_rect_size( const gp_canvas_t * _canvas, gp_mesh_t * _mesh );
gp_result_t gp_render_rounded_rect( const gp_canvas_t * _canvas, const gp_mesh_t * _mesh, gp_uint16_t * _vertex_iterator, gp_uint16_t * _index_iterator );

#endif