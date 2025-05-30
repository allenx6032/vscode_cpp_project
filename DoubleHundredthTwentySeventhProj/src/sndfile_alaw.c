/*
** Copyright (C) 1999-2013 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include	"sndfile_sfconfig.h"

#include	<math.h>
#include	<limits.h>

#include	"sndfile.h"
#include	"sndfile_common.h"

static sf_count_t alaw_read_alaw2s (SF_PRIVATE *psf, short *ptr, sf_count_t len) ;
static sf_count_t alaw_read_alaw2i (SF_PRIVATE *psf, int *ptr, sf_count_t len) ;
static sf_count_t alaw_read_alaw2f (SF_PRIVATE *psf, float *ptr, sf_count_t len) ;
static sf_count_t alaw_read_alaw2d (SF_PRIVATE *psf, double *ptr, sf_count_t len) ;

static sf_count_t alaw_write_s2alaw (SF_PRIVATE *psf, const short *ptr, sf_count_t len) ;
static sf_count_t alaw_write_i2alaw (SF_PRIVATE *psf, const int *ptr, sf_count_t len) ;
static sf_count_t alaw_write_f2alaw (SF_PRIVATE *psf, const float *ptr, sf_count_t len) ;
static sf_count_t alaw_write_d2alaw (SF_PRIVATE *psf, const double *ptr, sf_count_t len) ;

static void alaw2s_array (unsigned char *buffer, int count, short *ptr) ;
static void alaw2i_array (unsigned char *buffer, int count, int *ptr) ;
static void alaw2f_array (unsigned char *buffer, int count, float *ptr, float normfact) ;
static void alaw2d_array (unsigned char *buffer, int count, double *ptr, double normfact) ;

static void s2alaw_array (const short *buffer, int count, unsigned char *ptr) ;
static void i2alaw_array (const int *buffer, int count, unsigned char *ptr) ;
static void f2alaw_array (const float *buffer, int count, unsigned char *ptr, float normfact) ;
static void d2alaw_array (const double *buffer, int count, unsigned char *ptr, double normfact) ;


int
alaw_init (SF_PRIVATE *psf)
{
	if (psf->file.mode == SFM_READ || psf->file.mode == SFM_RDWR)
	{	psf->read_short		= alaw_read_alaw2s ;
		psf->read_int		= alaw_read_alaw2i ;
		psf->read_float		= alaw_read_alaw2f ;
		psf->read_double	= alaw_read_alaw2d ;
		} ;

	if (psf->file.mode == SFM_WRITE || psf->file.mode == SFM_RDWR)
	{	psf->write_short	= alaw_write_s2alaw ;
		psf->write_int		= alaw_write_i2alaw ;
		psf->write_float	= alaw_write_f2alaw ;
		psf->write_double	= alaw_write_d2alaw ;
		} ;

	psf->bytewidth = 1 ;
	psf->blockwidth = psf->sf.channels ;

	if (psf->filelength > psf->dataoffset)
		psf->datalength = (psf->dataend) ? psf->dataend - psf->dataoffset : psf->filelength - psf->dataoffset ;
	else
		psf->datalength = 0 ;

	psf->sf.frames = psf->blockwidth > 0 ? psf->datalength / psf->blockwidth : 0 ;

	return 0 ;
} /* alaw_init */

/*==============================================================================
 *	Private static functions and data.
 */

static
short alaw_decode [256] =
{	-5504,	-5248,	-6016,	-5760,	-4480,	-4224,	-4992,	-4736,
	-7552,	-7296,	-8064,	-7808,	-6528,	-6272,	-7040,	-6784,
	-2752,	-2624,	-3008,	-2880,	-2240,	-2112,	-2496,	-2368,
	-3776,	-3648,	-4032,	-3904,	-3264,	-3136,	-3520,	-3392,
	-22016,	-20992,	-24064,	-23040,	-17920,	-16896,	-19968,	-18944,
	-30208,	-29184,	-32256,	-31232,	-26112,	-25088,	-28160,	-27136,
	-11008,	-10496,	-12032,	-11520,	-8960,	-8448,	-9984,	-9472,
	-15104,	-14592,	-16128,	-15616,	-13056,	-12544,	-14080,	-13568,
	-344,	-328,	-376,	-360,	-280,	-264,	-312,	-296,
	-472,	-456,	-504,	-488,	-408,	-392,	-440,	-424,
	-88,	-72,	-120,	-104,	-24,	-8,		-56,	-40,
	-216,	-200,	-248,	-232,	-152,	-136,	-184,	-168,
	-1376,	-1312,	-1504,	-1440,	-1120,	-1056,	-1248,	-1184,
	-1888,	-1824,	-2016,	-1952,	-1632,	-1568,	-1760,	-1696,
	-688,	-656,	-752,	-720,	-560,	-528,	-624,	-592,
	-944,	-912,	-1008,	-976,	-816,	-784,	-880,	-848,
	5504,	5248,	6016,	5760,	4480,	4224,	4992,	4736,
	7552,	7296,	8064,	7808,	6528,	6272,	7040,	6784,
	2752,	2624,	3008,	2880,	2240,	2112,	2496,	2368,
	3776,	3648,	4032,	3904,	3264,	3136,	3520,	3392,
	22016,	20992,	24064,	23040,	17920,	16896,	19968,	18944,
	30208,	29184,	32256,	31232,	26112,	25088,	28160,	27136,
	11008,	10496,	12032,	11520,	8960,	8448,	9984,	9472,
	15104,	14592,	16128,	15616,	13056,	12544,	14080,	13568,
	344,	328,	376,	360,	280,	264,	312,	296,
	472,	456,	504,	488,	408,	392,	440,	424,
	88,		72,		120,	104,	24,		8,		56,		40,
	216,	200,	248,	232,	152,	136,	184,	168,
	1376,	1312,	1504,	1440,	1120,	1056,	1248,	1184,
	1888,	1824,	2016,	1952,	1632,	1568,	1760,	1696,
	688,	656,	752,	720,	560,	528,	624,	592,
	944,	912,	1008,	976,	816,	784,	880,	848
} ; /* alaw_decode */

static
unsigned char alaw_encode [2048 + 1] =
{	0xd5, 0xd4, 0xd7, 0xd6, 0xd1, 0xd0, 0xd3, 0xd2, 0xdd, 0xdc, 0xdf, 0xde,
	0xd9, 0xd8, 0xdb, 0xda, 0xc5, 0xc4, 0xc7, 0xc6, 0xc1, 0xc0, 0xc3, 0xc2,
	0xcd, 0xcc, 0xcf, 0xce, 0xc9, 0xc8, 0xcb, 0xca, 0xf5, 0xf5, 0xf4, 0xf4,
	0xf7, 0xf7, 0xf6, 0xf6, 0xf1, 0xf1, 0xf0, 0xf0, 0xf3, 0xf3, 0xf2, 0xf2,
	0xfd, 0xfd, 0xfc, 0xfc, 0xff, 0xff, 0xfe, 0xfe, 0xf9, 0xf9, 0xf8, 0xf8,
	0xfb, 0xfb, 0xfa, 0xfa, 0xe5, 0xe5, 0xe5, 0xe5, 0xe4, 0xe4, 0xe4, 0xe4,
	0xe7, 0xe7, 0xe7, 0xe7, 0xe6, 0xe6, 0xe6, 0xe6, 0xe1, 0xe1, 0xe1, 0xe1,
	0xe0, 0xe0, 0xe0, 0xe0, 0xe3, 0xe3, 0xe3, 0xe3, 0xe2, 0xe2, 0xe2, 0xe2,
	0xed, 0xed, 0xed, 0xed, 0xec, 0xec, 0xec, 0xec, 0xef, 0xef, 0xef, 0xef,
	0xee, 0xee, 0xee, 0xee, 0xe9, 0xe9, 0xe9, 0xe9, 0xe8, 0xe8, 0xe8, 0xe8,
	0xeb, 0xeb, 0xeb, 0xeb, 0xea, 0xea, 0xea, 0xea, 0x95, 0x95, 0x95, 0x95,
	0x95, 0x95, 0x95, 0x95, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94,
	0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x96, 0x96, 0x96, 0x96,
	0x96, 0x96, 0x96, 0x96, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x93, 0x93, 0x93, 0x93,
	0x93, 0x93, 0x93, 0x93, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92,
	0x9d, 0x9d, 0x9d, 0x9d, 0x9d, 0x9d, 0x9d, 0x9d, 0x9c, 0x9c, 0x9c, 0x9c,
	0x9c, 0x9c, 0x9c, 0x9c, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f,
	0x9e, 0x9e, 0x9e, 0x9e, 0x9e, 0x9e, 0x9e, 0x9e, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
	0x9b, 0x9b, 0x9b, 0x9b, 0x9b, 0x9b, 0x9b, 0x9b, 0x9a, 0x9a, 0x9a, 0x9a,
	0x9a, 0x9a, 0x9a, 0x9a, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
	0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x84, 0x84, 0x84, 0x84,
	0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
	0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
	0x87, 0x87, 0x87, 0x87, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
	0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x82, 0x82, 0x82, 0x82,
	0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
	0x8d, 0x8d, 0x8d, 0x8d, 0x8d, 0x8d, 0x8d, 0x8d, 0x8d, 0x8d, 0x8d, 0x8d,
	0x8d, 0x8d, 0x8d, 0x8d, 0x8c, 0x8c, 0x8c, 0x8c, 0x8c, 0x8c, 0x8c, 0x8c,
	0x8c, 0x8c, 0x8c, 0x8c, 0x8c, 0x8c, 0x8c, 0x8c, 0x8f, 0x8f, 0x8f, 0x8f,
	0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f, 0x8f,
	0x8e, 0x8e, 0x8e, 0x8e, 0x8e, 0x8e, 0x8e, 0x8e, 0x8e, 0x8e, 0x8e, 0x8e,
	0x8e, 0x8e, 0x8e, 0x8e, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x8b, 0x8b, 0x8b, 0x8b, 0x8b, 0x8b, 0x8b, 0x8b, 0x8b, 0x8b, 0x8b, 0x8b,
	0x8b, 0x8b, 0x8b, 0x8b, 0x8a, 0x8a, 0x8a, 0x8a, 0x8a, 0x8a, 0x8a, 0x8a,
	0x8a, 0x8a, 0x8a, 0x8a, 0x8a, 0x8a, 0x8a, 0x8a, 0xb5, 0xb5, 0xb5, 0xb5,
	0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5,
	0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5,
	0xb5, 0xb5, 0xb5, 0xb5, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4,
	0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4,
	0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4,
	0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7,
	0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7,
	0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb7, 0xb6, 0xb6, 0xb6, 0xb6,
	0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6,
	0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6,
	0xb6, 0xb6, 0xb6, 0xb6, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1,
	0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1,
	0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1, 0xb1,
	0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0,
	0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0,
	0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb3, 0xb3, 0xb3, 0xb3,
	0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3,
	0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3, 0xb3,
	0xb3, 0xb3, 0xb3, 0xb3, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2,
	0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2,
	0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2, 0xb2,
	0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd,
	0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd,
	0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf,
	0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf,
	0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf,
	0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe,
	0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe,
	0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xb9, 0xb9, 0xb9, 0xb9,
	0xb9, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9,
	0xb9, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9,
	0xb9, 0xb9, 0xb9, 0xb9, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8,
	0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8,
	0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8,
	0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb,
	0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb,
	0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xba, 0xba, 0xba, 0xba,
	0xba, 0xba, 0xba, 0xba, 0xba, 0xba, 0xba, 0xba, 0xba, 0xba, 0xba, 0xba,
	0xba, 0xba, 0xba, 0xba, 0xba, 0xba, 0xba, 0xba, 0xba, 0xba, 0xba, 0xba,
	0xba, 0xba, 0xba, 0xba, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5,
	0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5,
	0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5,
	0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5,
	0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5,
	0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa4, 0xa4, 0xa4, 0xa4,
	0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4,
	0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4,
	0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4,
	0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4,
	0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4, 0xa4,
	0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7,
	0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7,
	0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7,
	0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7,
	0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xa7,
	0xa7, 0xa7, 0xa7, 0xa7, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6,
	0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6,
	0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6,
	0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6,
	0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6,
	0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa6, 0xa1, 0xa1, 0xa1, 0xa1,
	0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1,
	0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1,
	0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1,
	0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1,
	0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1,
	0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
	0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
	0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
	0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
	0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
	0xa0, 0xa0, 0xa0, 0xa0, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3,
	0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3,
	0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3,
	0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3,
	0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3,
	0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa2, 0xa2, 0xa2, 0xa2,
	0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2,
	0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2,
	0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2,
	0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2,
	0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2,
	0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad,
	0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad,
	0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad,
	0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad,
	0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad, 0xad,
	0xad, 0xad, 0xad, 0xad, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac,
	0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac,
	0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac,
	0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac,
	0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac,
	0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xac, 0xaf, 0xaf, 0xaf, 0xaf,
	0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf,
	0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf,
	0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf,
	0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf,
	0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf,
	0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae,
	0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae,
	0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae,
	0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae,
	0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae,
	0xae, 0xae, 0xae, 0xae, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9,
	0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9,
	0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9,
	0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9,
	0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9,
	0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa9, 0xa8, 0xa8, 0xa8, 0xa8,
	0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8,
	0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8,
	0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8,
	0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8,
	0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8,
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
	0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,
	0xab, 0xab, 0xab, 0xab, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
} ; /* alaw_encode */

static inline void
alaw2s_array (unsigned char *buffer, int count, short *ptr)
{	for (int i = 0 ; i < count ; i++)
		ptr [i] = alaw_decode [(int) buffer [i]] ;
} /* alaw2s_array */

static inline void
alaw2i_array (unsigned char *buffer, int count, int *ptr)
{	for (int i = 0 ; i < count ; i++)
		ptr [i] = ((uint32_t) alaw_decode [(int) buffer [i]]) << 16 ;
} /* alaw2i_array */

static inline void
alaw2f_array (unsigned char *buffer, int count, float *ptr, float normfact)
{	for (int i = 0 ; i < count ; i++)
		ptr [i] = normfact * alaw_decode [(int) buffer [i]] ;
} /* alaw2f_array */

static inline void
alaw2d_array (unsigned char *buffer, int count, double *ptr, double normfact)
{	for (int i = 0 ; i < count ; i++)
		ptr [i] = normfact * alaw_decode [(int) buffer [i]] ;
} /* alaw2d_array */

static inline void
s2alaw_array (const short *ptr, int count, unsigned char *buffer)
{	for (int i = 0 ; i < count ; i++)
	{	if (ptr [i] >= 0)
			buffer [i] = alaw_encode [ptr [i] / 16] ;
		else
			buffer [i] = 0x7F & alaw_encode [ptr [i] / -16] ;
		} ;
} /* s2alaw_array */

static inline void
i2alaw_array (const int *ptr, int count, unsigned char *buffer)
{	for (int i = 0 ; i < count ; i++)
	{	if (ptr [i] == INT_MIN)
			buffer [i] = alaw_encode [INT_MAX >> (16 + 4)] ;
		else if (ptr [i] >= 0)
			buffer [i] = alaw_encode [ptr [i] >> (16 + 4)] ;
		else
			buffer [i] = 0x7F & alaw_encode [- ptr [i] >> (16 + 4)] ;
		} ;
} /* i2alaw_array */

static inline void
f2alaw_array (const float *ptr, int count, unsigned char *buffer, float normfact)
{	for (int i = 0 ; i < count ; i++)
	{	if (ptr [i] >= 0)
			buffer [i] = alaw_encode [psf_lrintf (normfact * ptr [i])] ;
		else
			buffer [i] = 0x7F & alaw_encode [- psf_lrintf (normfact * ptr [i])] ;
		} ;
} /* f2alaw_array */

static inline void
d2alaw_array (const double *ptr, int count, unsigned char *buffer, double normfact)
{	for (int i = 0 ; i < count ; i++)
	{	if (!isfinite (ptr [i]))
			buffer [i] = 0 ;
		else if (ptr [i] >= 0)
			buffer [i] = alaw_encode [psf_lrint (normfact * ptr [i])] ;
		else
			buffer [i] = 0x7F & alaw_encode [- psf_lrint (normfact * ptr [i])] ;
		} ;
} /* d2alaw_array */

/*==============================================================================
*/

static sf_count_t
alaw_read_alaw2s (SF_PRIVATE *psf, short *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	int			bufferlen, readcount ;
	sf_count_t	total = 0 ;

	bufferlen = ARRAY_LEN (ubuf.ucbuf) ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;
		readcount = (int) psf_fread (ubuf.ucbuf, 1, bufferlen, psf) ;
		alaw2s_array (ubuf.ucbuf, readcount, ptr + total) ;
		total += readcount ;
		if (readcount < bufferlen)
			break ;
		len -= readcount ;
		} ;

	return total ;
} /* alaw_read_alaw2s */

static sf_count_t
alaw_read_alaw2i (SF_PRIVATE *psf, int *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	int			bufferlen, readcount ;
	sf_count_t	total = 0 ;

	bufferlen = ARRAY_LEN (ubuf.ucbuf) ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;
		readcount = (int) psf_fread (ubuf.ucbuf, 1, bufferlen, psf) ;
		alaw2i_array (ubuf.ucbuf, readcount, ptr + total) ;
		total += readcount ;
		if (readcount < bufferlen)
			break ;
		len -= readcount ;
		} ;

	return total ;
} /* alaw_read_alaw2i */

static sf_count_t
alaw_read_alaw2f (SF_PRIVATE *psf, float *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	int			bufferlen, readcount ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? 1.0 / ((float) 0x8000) : 1.0 ;

	bufferlen = ARRAY_LEN (ubuf.ucbuf) ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;
		readcount = (int) psf_fread (ubuf.ucbuf, 1, bufferlen, psf) ;
		alaw2f_array (ubuf.ucbuf, readcount, ptr + total, normfact) ;
		total += readcount ;
		if (readcount < bufferlen)
			break ;
		len -= readcount ;
		} ;

	return total ;
} /* alaw_read_alaw2f */

static sf_count_t
alaw_read_alaw2d (SF_PRIVATE *psf, double *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	int			bufferlen, readcount ;
	sf_count_t	total = 0 ;
	double	normfact ;

	normfact = (psf->norm_double) ? 1.0 / ((double) 0x8000) : 1.0 ;
	bufferlen = ARRAY_LEN (ubuf.ucbuf) ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;
		readcount = (int) psf_fread (ubuf.ucbuf, 1, bufferlen, psf) ;
		alaw2d_array (ubuf.ucbuf, readcount, ptr + total, normfact) ;
		total += readcount ;
		if (readcount < bufferlen)
			break ;
		len -= readcount ;
		} ;

	return total ;
} /* alaw_read_alaw2d */

/*=============================================================================================
*/

static sf_count_t
alaw_write_s2alaw	(SF_PRIVATE *psf, const short *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	int			bufferlen, writecount ;
	sf_count_t	total = 0 ;

	bufferlen = ARRAY_LEN (ubuf.ucbuf) ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;
		s2alaw_array (ptr + total, bufferlen, ubuf.ucbuf) ;
		writecount = (int) psf_fwrite (ubuf.ucbuf, 1, bufferlen, psf) ;
		total += writecount ;
		if (writecount < bufferlen)
			break ;
		len -= writecount ;
		} ;

	return total ;
} /* alaw_write_s2alaw */

static sf_count_t
alaw_write_i2alaw	(SF_PRIVATE *psf, const int *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	int			bufferlen, writecount ;
	sf_count_t	total = 0 ;

	bufferlen = ARRAY_LEN (ubuf.ucbuf) ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;
		i2alaw_array (ptr + total, bufferlen, ubuf.ucbuf) ;
		writecount = (int) psf_fwrite (ubuf.ucbuf, 1, bufferlen, psf) ;
		total += writecount ;
		if (writecount < bufferlen)
			break ;
		len -= writecount ;
		} ;

	return total ;
} /* alaw_write_i2alaw */

static sf_count_t
alaw_write_f2alaw	(SF_PRIVATE *psf, const float *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	int			bufferlen, writecount ;
	sf_count_t	total = 0 ;
	float	normfact ;

	normfact = (psf->norm_float == SF_TRUE) ? (1.0 * 0x7FFF) / 16.0 : 1.0 / 16 ;

	bufferlen = ARRAY_LEN (ubuf.ucbuf) ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;
		f2alaw_array (ptr + total, bufferlen, ubuf.ucbuf, normfact) ;
		writecount = (int) psf_fwrite (ubuf.ucbuf, 1, bufferlen, psf) ;
		total += writecount ;
		if (writecount < bufferlen)
			break ;
		len -= writecount ;
		} ;

	return total ;
} /* alaw_write_f2alaw */

static sf_count_t
alaw_write_d2alaw	(SF_PRIVATE *psf, const double *ptr, sf_count_t len)
{	BUF_UNION	ubuf ;
	int			bufferlen, writecount ;
	sf_count_t	total = 0 ;
	double	normfact ;

	normfact = (psf->norm_double) ? (1.0 * 0x7FFF) / 16.0 : 1.0 / 16.0 ;

	bufferlen = ARRAY_LEN (ubuf.ucbuf) ;

	while (len > 0)
	{	if (len < bufferlen)
			bufferlen = (int) len ;
		d2alaw_array (ptr + total, bufferlen, ubuf.ucbuf, normfact) ;
		writecount = (int) psf_fwrite (ubuf.ucbuf, 1, bufferlen, psf) ;
		total += writecount ;
		if (writecount < bufferlen)
			break ;
		len -= writecount ;
		} ;

	return total ;
} /* alaw_write_d2alaw */

