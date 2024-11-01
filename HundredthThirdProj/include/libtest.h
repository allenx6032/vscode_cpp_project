﻿#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <LCUI_Build.h>
#include <LCUI_types.h>

void describe(const char *name, void (*func)());

void it_i(const char *name, int actual, int expected);

void it_b(const char *name, int actual, int expected);

void it_s(const char *name, const char *actual, const char *expected);

void it_rectf(const char *name, const LCUI_RectF *actual,
	      const LCUI_RectF *expected);

void it_rect(const char *name, const LCUI_Rect *actual,
	     const LCUI_Rect *expected);

void test_begin(void);

void test_end(void);

int test_result(void);

int print_test_result(void);

#endif
