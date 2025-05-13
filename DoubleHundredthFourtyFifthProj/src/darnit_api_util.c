/*
Copyright (c) 2011-2014 Steven Arnow
'darnit_util.c' - This file is part of libdarnit

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/


#include "darnit.h"


unsigned int EXPORT_THIS d_util_htonl(unsigned int arg) {
	return utilHtonl(arg);
}


unsigned int EXPORT_THIS d_util_ntohl(unsigned int arg) {
	return utilNtohl(arg);
}


char EXPORT_THIS *d_util_path_translate(const char *path) {
	return utilPathTranslate(path);
}


int EXPORT_THIS d_util_string_to_int_array(const char *str, const char *delimiter, int *dest, int max_tokens) {
	return utilStringToIntArray(str, delimiter, dest, max_tokens);
}


void EXPORT_THIS d_util_endian_convert(unsigned int *block, int elements) {
	utilBlockToHostEndian(block, elements);

	return;
}


int EXPORT_THIS d_util_sin(int angle) {
	return utilSine(angle);
}


const char EXPORT_THIS *d_str_null(const char *str) {
	if (!str)
		return NULL;
	if (!strcmp("NULL", str))
		return NULL;
	return str;
}


IMGLOAD_DATA EXPORT_THIS d_img_load_raw(const char *fname) {
	return imgloadLoad(fname);
}


int EXPORT_THIS d_util_compress(void *buffer, unsigned int data_len, void **dest) {
	return compressionCompress(buffer, data_len, dest);
}


int EXPORT_THIS d_util_decompress(void *buffer, unsigned int data_len, void **dest) {
	return compressionDecompress(buffer, data_len, dest);
}


void EXPORT_THIS *d_util_random_new(unsigned int seed) {
	return utilRandomNew(seed);
}


unsigned int EXPORT_THIS d_util_random_get(void *state) {
	return utilRandomGet(state);
}


void EXPORT_THIS *d_util_random_free(void *state) {
	return utilRandomFree(state);
}


void EXPORT_THIS *d_util_mutex_create() {
	return tpw_mutex_create();
}


void EXPORT_THIS d_util_mutex_lock(void *mutex) {
	return tpw_mutex_lock(mutex);
}


void EXPORT_THIS d_util_mutex_unlock(void *mutex) {
	return tpw_mutex_unlock(mutex);
}


void EXPORT_THIS *d_util_thread_new(void *func, void *func_arg) {
	return tpw_thread_new(func, func_arg);
}


void EXPORT_THIS d_util_thread_exit(int exit_code) {
	return tpw_thread_exit(exit_code);
}


void EXPORT_THIS d_util_thread_kill(void *thread) {
	return tpw_thread_kill(thread);
}


void EXPORT_THIS d_util_thread_kill_self(int exit_code) {
	return tpw_kill_self(exit_code);
}


void EXPORT_THIS *d_util_semaphore_new(int initial_count) {
	return tpw_semaphore_new(initial_count);
}


void EXPORT_THIS d_util_semaphore_wait(void *sem) {
	return tpw_semaphore_wait(sem);
}


void EXPORT_THIS d_util_semaphore_add(void *sem, int add) {
	return tpw_semaphore_add(sem, add);
}


void EXPORT_THIS d_util_semahpore_delete(void *sem) {
	return tpw_semaphore_delete(sem);
}
