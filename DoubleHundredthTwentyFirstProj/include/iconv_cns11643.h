/*
 * Copyright (C) 1999-2001 Free Software Foundation, Inc.
 * This file is part of the GNU LIBICONV Library.
 *
 * The GNU LIBICONV Library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * The GNU LIBICONV Library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU LIBICONV Library; see the file COPYING.LIB.
 * If not, see <https://www.gnu.org/licenses/>.
 */

/*
 * CNS 11643-1992
 */

/* ISO-2022-CN and EUC-TW use CNS 11643-1992 planes 1 to 7. We also
 * have a table for the older plane 15. We use a trick to keep the
 * Unicode -> CNS 11643 table as small as possible (see cns11643_inv.h).
 */

#include "iconv_cns11643_1.h"
#include "iconv_cns11643_2.h"
#include "iconv_cns11643_3.h"
#include "iconv_cns11643_4.h"
#include "iconv_cns11643_5.h"
#include "iconv_cns11643_6.h"
#include "iconv_cns11643_7.h"
#include "iconv_cns11643_15.h"
#include "iconv_cns11643_inv.h"

/* Returns the plane number (1,...,7,15) in r[0], the two bytes in r[1],r[2]. */
#define cns11643_wctomb cns11643_inv_wctomb
