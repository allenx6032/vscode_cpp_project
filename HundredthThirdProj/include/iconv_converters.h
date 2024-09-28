/*
 * Copyright (C) 1999-2002, 2004-2011, 2016, 2022 Free Software Foundation, Inc.
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

/* This file defines all the converters. */


/* Our own notion of wide character, as UCS-4, according to ISO-10646-1. */
typedef unsigned int ucs4_t;

/* State used by a conversion. 0 denotes the initial state. */
typedef unsigned int state_t;

/* iconv_t is an opaque type. This is the real iconv_t type. */
typedef struct conv_struct * conv_t;

/*
 * Data type for conversion multibyte -> unicode
 */
struct mbtowc_funcs {
  int (*xxx_mbtowc) (conv_t conv, ucs4_t *pwc, unsigned char const *s, size_t n);
  /*
   * int xxx_mbtowc (conv_t conv, ucs4_t *pwc, unsigned char const *s, size_t n)
   * converts the byte sequence starting at s to a wide character. Up to n bytes
   * are available at s. n is >= 1.
   * Result is number of bytes consumed (if a wide character was read),
   * or -1 if invalid, or -2 if n too small,
   * or RET_SHIFT_ILSEQ(number of bytes consumed) if invalid input after a shift
   * sequence was read,
   * or RET_TOOFEW(number of bytes consumed) if only a shift sequence was read.
   */
  int (*xxx_flushwc) (conv_t conv, ucs4_t *pwc);
  /*
   * int xxx_flushwc (conv_t conv, ucs4_t *pwc)
   * returns to the initial state and stores the pending wide character, if any.
   * Result is 1 (if a wide character was read) or 0 if none was pending.
   */
};

/* Return code if invalid input after a shift sequence of n bytes was read.
   (xxx_mbtowc) */
#define RET_SHIFT_ILSEQ(n)  (-1-2*(n))
/* Return code if invalid. (xxx_mbtowc) */
#define RET_ILSEQ           RET_SHIFT_ILSEQ(0)
/* Return code if only a shift sequence of n bytes was read. (xxx_mbtowc) */
#define RET_TOOFEW(n)       (-2-2*(n))
/* Retrieve the n from the encoded RET_... value. */
#define DECODE_SHIFT_ILSEQ(r)  ((unsigned int)(RET_SHIFT_ILSEQ(0) - (r)) / 2)
#define DECODE_TOOFEW(r)       ((unsigned int)(RET_TOOFEW(0) - (r)) / 2)
/* Maximum value of n that may be used as argument to RET_SHIFT_ILSEQ or RET_TOOFEW. */
#define RET_COUNT_MAX       ((INT_MAX / 2) - 1)

/*
 * Data type for conversion unicode -> multibyte
 */
struct wctomb_funcs {
  int (*xxx_wctomb) (conv_t conv, unsigned char *r, ucs4_t wc, size_t n);
  /*
   * int xxx_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, size_t n)
   * converts the wide character wc to the character set xxx, and stores the
   * result beginning at r. Up to n bytes may be written at r. n is >= 1.
   * Result is number of bytes written, or -1 if invalid, or -2 if n too small.
   */
  int (*xxx_reset) (conv_t conv, unsigned char *r, size_t n);
  /*
   * int xxx_reset (conv_t conv, unsigned char *r, size_t n)
   * stores a shift sequences returning to the initial state beginning at r.
   * Up to n bytes may be written at r. n is >= 0.
   * Result is number of bytes written, or -2 if n too small.
   */
};

/* Return code if invalid. (xxx_wctomb) */
#define RET_ILUNI      -1
/* Return code if output buffer is too small. (xxx_wctomb, xxx_reset) */
#define RET_TOOSMALL   -2

/*
 * Contents of a conversion descriptor.
 */
struct conv_struct {
  struct loop_funcs lfuncs;
  /* Input (conversion multibyte -> unicode) */
  int iindex;
  struct mbtowc_funcs ifuncs;
  state_t istate;
  /* Output (conversion unicode -> multibyte) */
  int oindex;
  struct wctomb_funcs ofuncs;
  int oflags;
  state_t ostate;
  /* Operation flags */
  int transliterate;
  int discard_ilseq;
  #ifndef LIBICONV_PLUG
  struct iconv_fallbacks fallbacks;
  struct iconv_hooks hooks;
  #endif
};

/*
 * Include all the converters.
 */

#include "iconv_ascii.h"

/* General multi-byte encodings */
#include "iconv_utf8.h"
#include "iconv_ucs2.h"
#include "iconv_ucs2be.h"
#include "iconv_ucs2le.h"
#include "iconv_ucs4.h"
#include "iconv_ucs4be.h"
#include "iconv_ucs4le.h"
#include "iconv_utf16.h"
#include "iconv_utf16be.h"
#include "iconv_utf16le.h"
#include "iconv_utf32.h"
#include "iconv_utf32be.h"
#include "iconv_utf32le.h"
#include "iconv_utf7.h"
#include "iconv_ucs2internal.h"
#include "iconv_ucs2swapped.h"
#include "iconv_ucs4internal.h"
#include "iconv_ucs4swapped.h"
#include "iconv_c99.h"
#include "iconv_java.h"

/* 8-bit encodings */
#include "iconv_iso8859_1.h"
#include "iconv_iso8859_2.h"
#include "iconv_iso8859_3.h"
#include "iconv_iso8859_4.h"
#include "iconv_iso8859_5.h"
#include "iconv_iso8859_6.h"
#include "iconv_iso8859_7.h"
#include "iconv_iso8859_8.h"
#include "iconv_iso8859_9.h"
#include "iconv_iso8859_10.h"
#include "iconv_iso8859_11.h"
#include "iconv_iso8859_13.h"
#include "iconv_iso8859_14.h"
#include "iconv_iso8859_15.h"
#include "iconv_iso8859_16.h"
#include "iconv_koi8_r.h"
#include "iconv_koi8_u.h"
#include "iconv_koi8_ru.h"
#include "iconv_cp1250.h"
#include "iconv_cp1251.h"
#include "iconv_cp1252.h"
#include "iconv_cp1253.h"
#include "iconv_cp1254.h"
#include "iconv_cp1255.h"
#include "iconv_cp1256.h"
#include "iconv_cp1257.h"
#include "iconv_cp1258.h"
#include "iconv_cp850.h"
#include "iconv_cp862.h"
#include "iconv_cp866.h"
#include "iconv_cp1131.h"
#include "iconv_mac_roman.h"
#include "iconv_mac_centraleurope.h"
#include "iconv_mac_iceland.h"
#include "iconv_mac_croatian.h"
#include "iconv_mac_romania.h"
#include "iconv_mac_cyrillic.h"
#include "iconv_mac_ukraine.h"
#include "iconv_mac_greek.h"
#include "iconv_mac_turkish.h"
#include "iconv_mac_hebrew.h"
#include "iconv_mac_arabic.h"
#include "iconv_mac_thai.h"
#include "iconv_hp_roman8.h"
#include "iconv_nextstep.h"
#include "iconv_armscii_8.h"
#include "iconv_georgian_academy.h"
#include "iconv_georgian_ps.h"
#include "iconv_koi8_t.h"
#include "iconv_pt154.h"
#include "iconv_rk1048.h"
#include "iconv_mulelao.h"
#include "iconv_cp1133.h"
#include "iconv_tis620.h"
#include "iconv_cp874.h"
#include "iconv_viscii.h"
#include "iconv_tcvn.h"

/* CJK character sets [CCS = coded character set] [CJKV.INF chapter 3] */

typedef struct {
  unsigned short indx; /* index into big table */
  unsigned short used; /* bitmask of used entries */
} Summary16;

#include "iconv_iso646_jp.h"
#include "iconv_jisx0201.h"
#include "iconv_jisx0208.h"
#include "iconv_jisx0212.h"

#include "iconv_iso646_cn.h"
#include "iconv_gb2312.h"
#include "iconv_isoir165.h"
/*#include "iconv_gb12345.h"*/
#include "iconv_gbk.h"
#include "iconv_cns11643.h"
#include "iconv_big5.h"

#include "iconv_ksc5601.h"
#include "iconv_johab_hangul.h"

/* CJK encodings [CES = character encoding scheme] [CJKV.INF chapter 4] */

#include "iconv_euc_jp.h"
#include "iconv_sjis.h"
#include "iconv_cp932.h"
#include "iconv_iso2022_jp.h"
#include "iconv_iso2022_jp1.h"
#include "iconv_iso2022_jp2.h"
#include "iconv_iso2022_jpms.h"

#include "iconv_euc_cn.h"
#include "iconv_ces_gbk.h"
#include "iconv_cp936.h"
#include "iconv_gb18030.h"
#include "iconv_iso2022_cn.h"
#include "iconv_iso2022_cnext.h"
#include "iconv_hz.h"
#include "iconv_euc_tw.h"
#include "iconv_ces_big5.h"
#include "iconv_cp950.h"
#include "iconv_big5hkscs1999.h"
#include "iconv_big5hkscs2001.h"
#include "iconv_big5hkscs2004.h"
#include "iconv_big5hkscs2008.h"

#include "iconv_euc_kr.h"
#include "iconv_cp949.h"
#include "iconv_johab.h"
#include "iconv_iso2022_kr.h"

/* Encodings used by system dependent locales. */

#ifdef USE_AIX
#include "iconv_cp856.h"
#include "iconv_cp922.h"
#include "iconv_cp943.h"
#include "iconv_cp1046.h"
#include "iconv_cp1124.h"
#include "iconv_cp1129.h"
#include "iconv_cp1161.h"
#include "iconv_cp1162.h"
#include "iconv_cp1163.h"
#endif

#ifdef USE_OSF1
#include "iconv_dec_kanji.h"
#include "iconv_dec_hanyu.h"
#endif

#ifdef USE_DOS
#include "iconv_cp437.h"
#include "iconv_cp737.h"
#include "iconv_cp775.h"
#include "iconv_cp852.h"
#include "iconv_cp853.h"
#include "iconv_cp855.h"
#include "iconv_cp857.h"
#include "iconv_cp858.h"
#include "iconv_cp860.h"
#include "iconv_cp861.h"
#include "iconv_cp863.h"
#include "iconv_cp864.h"
#include "iconv_cp865.h"
#include "iconv_cp869.h"
#include "iconv_cp1125.h"
#endif

#ifdef USE_ZOS
#define DEDUPLICATE_TABLES 1
#include "iconv_ebcdic037.h"
#include "iconv_ebcdic273.h"
#include "iconv_ebcdic277.h"
#include "iconv_ebcdic278.h"
#include "iconv_ebcdic280.h"
#include "iconv_ebcdic282.h"
#include "iconv_ebcdic284.h"
#include "iconv_ebcdic285.h"
#include "iconv_ebcdic297.h"
#include "iconv_ebcdic423.h"
#include "iconv_ebcdic424.h"
#include "iconv_ebcdic425.h"
#include "iconv_ebcdic500.h"
#include "iconv_ebcdic838.h"
#include "iconv_ebcdic870.h"
#include "iconv_ebcdic871.h"
#include "iconv_ebcdic875.h"
#include "iconv_ebcdic880.h"
#include "iconv_ebcdic905.h"
#include "iconv_ebcdic924.h"
#include "iconv_ebcdic1025.h"
#include "iconv_ebcdic1026.h"
#include "iconv_ebcdic1047.h"
#include "iconv_ebcdic1097.h"
#include "iconv_ebcdic1112.h"
#include "iconv_ebcdic1122.h"
#include "iconv_ebcdic1123.h"
#include "iconv_ebcdic1130.h"
#include "iconv_ebcdic1132.h"
#include "iconv_ebcdic1137.h"
#include "iconv_ebcdic1140.h"
#include "iconv_ebcdic1141.h"
#include "iconv_ebcdic1142.h"
#include "iconv_ebcdic1143.h"
#include "iconv_ebcdic1144.h"
#include "iconv_ebcdic1145.h"
#include "iconv_ebcdic1146.h"
#include "iconv_ebcdic1147.h"
#include "iconv_ebcdic1148.h"
#include "iconv_ebcdic1149.h"
#include "iconv_ebcdic1153.h"
#include "iconv_ebcdic1154.h"
#include "iconv_ebcdic1155.h"
#include "iconv_ebcdic1156.h"
#include "iconv_ebcdic1157.h"
#include "iconv_ebcdic1158.h"
#include "iconv_ebcdic1160.h"
#include "iconv_ebcdic1164.h"
#include "iconv_ebcdic1165.h"
#include "iconv_ebcdic1166.h"
#include "iconv_ebcdic4971.h"
#include "iconv_ebcdic12712.h"
#include "iconv_ebcdic16804.h"
#endif

#ifdef USE_EXTRA
#include "iconv_euc_jisx0213.h"
#include "iconv_shift_jisx0213.h"
#include "iconv_iso2022_jp3.h"
#include "iconv_big5_2003.h"
#include "iconv_tds565.h"
#include "iconv_atarist.h"
#include "iconv_riscos1.h"
#endif

