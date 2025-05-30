/**
 * This file is part of Drystal.
 *
 * Drystal is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Drystal is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Drystal.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <stdbool.h>
#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES
#ifndef EMSCRIPTEN
#include <SDL_opengles2.h>
// #include <SDL_opengl.h>
#else
#include <SDL_opengl.h>
#endif

typedef struct Buffer Buffer;

#include "drystal_shader.h"
#include "drystal_camera.h"
#include "drystal_surface.h"

// should be multiple of 3 (GL_TRIANGLES)
#define BUFFER_DEFAULT_SIZE 3 * 4096

struct Buffer {
	unsigned int size;
	GLuint buffers[3]; // first is for positions, second for colors, third (optional) for texcoords
	GLfloat* positions;
	GLubyte* colors;
	GLfloat* tex_coords; // only if has_texture
	unsigned int current_position;
	unsigned int current_color;
	unsigned int current_tex_coord;
	bool uploaded;

	bool has_texture;
	Shader* shader;
	Camera* camera;
	bool user_buffer;

	int ref;
	const Surface* draw_on;
	const Surface* draw_from;
};

Buffer *buffer_new(bool user_buffer, unsigned int size);
void buffer_free(Buffer *b);
void buffer_allocate(Buffer *b);

void buffer_push_vertex(Buffer *b, GLfloat, GLfloat);
void buffer_push_color(Buffer *b, GLubyte, GLubyte, GLubyte, GLubyte);
void buffer_push_tex_coord(Buffer *b, GLfloat, GLfloat);

void buffer_draw(Buffer *b, float dx, float dy);

void buffer_check_empty(Buffer *b);
void buffer_check_use_texture(Buffer *b);
void buffer_check_not_use_texture(Buffer *b);
void buffer_check_not_full(Buffer *b);

void buffer_upload_and_free(Buffer *b);

static inline bool buffer_was_freed(const Buffer *b)
{
	return b->positions == NULL;
}

static inline void buffer_reset(Buffer *b)
{
	assert(b);

	b->current_position = b->current_color = b->current_tex_coord = 0;
}

static inline bool buffer_is_empty(Buffer *b)
{
	assert(b);

	return !b->current_position;
}

static inline void buffer_flush(Buffer *b)
{
	assert(b);

	buffer_draw(b, 0, 0);
	buffer_reset(b);
}

static inline void buffer_use_shader(Buffer *b, Shader *s)
{
	assert(b);
	assert(s);

	b->shader = s;
}

static inline void buffer_use_camera(Buffer *b, Camera *c)
{
	assert(b);
	assert(c);

	b->camera = c;
}

