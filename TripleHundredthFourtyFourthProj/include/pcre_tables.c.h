/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/* PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

                       Written by Philip Hazel
           Copyright (c) 1997-2012 University of Cambridge

-----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the University of Cambridge nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/

#ifndef PCRE_INCLUDED

/* This module contains some fixed tables that are used by more than one of the
PCRE code modules. The tables are also #included by the pcretest program, which
uses macros to change their names from _pcre_xxx to xxxx, thereby avoiding name
clashes with the library. */


#ifdef HAVE_PCRE_CONFIG_H
#include "pcre_config.h"
#endif

#include "pcre_internal.h"

#endif /* PCRE_INCLUDED */

/* Table of sizes for the fixed-length opcodes. It's defined in a macro so that
the definition is next to the definition of the opcodes in pcre_internal.h. */

const pcre_uint8 PRIV(OP_lengths)[] = { OP_LENGTHS };



/*************************************************
*           Tables for UTF-8 support             *
*************************************************/

/* These are the breakpoints for different numbers of bytes in a UTF-8
character. */

#if (defined SUPPORT_UTF && defined COMPILE_PCRE8) \
  || (defined PCRE_INCLUDED && defined SUPPORT_PCRE16)

/* These tables are also required by pcretest in 16 bit mode. */

const int PRIV(utf8_table1)[] =
  { 0x7f, 0x7ff, 0xffff, 0x1fffff, 0x3ffffff, 0x7fffffff};

const int PRIV(utf8_table1_size) = sizeof(PRIV(utf8_table1)) / sizeof(int);

/* These are the indicator bits and the mask for the data bits to set in the
first byte of a character, indexed by the number of additional bytes. */

const int PRIV(utf8_table2)[] = { 0,    0xc0, 0xe0, 0xf0, 0xf8, 0xfc};
const int PRIV(utf8_table3)[] = { 0xff, 0x1f, 0x0f, 0x07, 0x03, 0x01};

/* Table of the number of extra bytes, indexed by the first byte masked with
0x3f. The highest number for a valid UTF-8 first byte is in fact 0x3d. */

const pcre_uint8 PRIV(utf8_table4)[] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5 };

#endif /* (SUPPORT_UTF && COMPILE_PCRE8) || (PCRE_INCLUDED && SUPPORT_PCRE16)*/

#ifdef SUPPORT_UTF

/* Table to translate from particular type value to the general value. */

const int PRIV(ucp_gentype)[] = {
  ucp_C, ucp_C, ucp_C, ucp_C, ucp_C,  /* Cc, Cf, Cn, Co, Cs */
  ucp_L, ucp_L, ucp_L, ucp_L, ucp_L,  /* Ll, Lu, Lm, Lo, Lt */
  ucp_M, ucp_M, ucp_M,                /* Mc, Me, Mn */
  ucp_N, ucp_N, ucp_N,                /* Nd, Nl, No */
  ucp_P, ucp_P, ucp_P, ucp_P, ucp_P,  /* Pc, Pd, Pe, Pf, Pi */
  ucp_P, ucp_P,                       /* Ps, Po */
  ucp_S, ucp_S, ucp_S, ucp_S,         /* Sc, Sk, Sm, So */
  ucp_Z, ucp_Z, ucp_Z                 /* Zl, Zp, Zs */
};

#ifdef SUPPORT_JIT
/* This table reverses PRIV(ucp_gentype). We can save the cost
of a memory load. */

const int PRIV(ucp_typerange)[] = {
  ucp_Cc, ucp_Cs,
  ucp_Ll, ucp_Lu,
  ucp_Mc, ucp_Mn,
  ucp_Nd, ucp_No,
  ucp_Pc, ucp_Ps,
  ucp_Sc, ucp_So,
  ucp_Zl, ucp_Zs,
};
#endif /* SUPPORT_JIT */

/* The pcre_utt[] table below translates Unicode property names into type and
code values. It is searched by binary chop, so must be in collating sequence of
name. Originally, the table contained pointers to the name strings in the first
field of each entry. However, that leads to a large number of relocations when
a shared library is dynamically loaded. A significant reduction is made by
putting all the names into a single, large string and then using offsets in the
table itself. Maintenance is more error-prone, but frequent changes to this
data are unlikely.

July 2008: There is now a script called maint/GenerateUtt.py that can be used
to generate this data automatically instead of maintaining it by hand.

The script was updated in March 2009 to generate a new EBCDIC-compliant
version. Like all other character and string literals that are compared against
the regular expression pattern, we must use STR_ macros instead of literal
strings to make sure that UTF-8 support works on EBCDIC platforms. */

#define STRING_Any0 STR_A STR_n STR_y "\0"
#define STRING_Arabic0 STR_A STR_r STR_a STR_b STR_i STR_c "\0"
#define STRING_Armenian0 STR_A STR_r STR_m STR_e STR_n STR_i STR_a STR_n "\0"
#define STRING_Avestan0 STR_A STR_v STR_e STR_s STR_t STR_a STR_n "\0"
#define STRING_Balinese0 STR_B STR_a STR_l STR_i STR_n STR_e STR_s STR_e "\0"
#define STRING_Bamum0 STR_B STR_a STR_m STR_u STR_m "\0"
#define STRING_Batak0 STR_B STR_a STR_t STR_a STR_k "\0"
#define STRING_Bengali0 STR_B STR_e STR_n STR_g STR_a STR_l STR_i "\0"
#define STRING_Bopomofo0 STR_B STR_o STR_p STR_o STR_m STR_o STR_f STR_o "\0"
#define STRING_Brahmi0 STR_B STR_r STR_a STR_h STR_m STR_i "\0"
#define STRING_Braille0 STR_B STR_r STR_a STR_i STR_l STR_l STR_e "\0"
#define STRING_Buginese0 STR_B STR_u STR_g STR_i STR_n STR_e STR_s STR_e "\0"
#define STRING_Buhid0 STR_B STR_u STR_h STR_i STR_d "\0"
#define STRING_C0 STR_C "\0"
#define STRING_Canadian_Aboriginal0 STR_C STR_a STR_n STR_a STR_d STR_i STR_a STR_n STR_UNDERSCORE STR_A STR_b STR_o STR_r STR_i STR_g STR_i STR_n STR_a STR_l "\0"
#define STRING_Carian0 STR_C STR_a STR_r STR_i STR_a STR_n "\0"
#define STRING_Cc0 STR_C STR_c "\0"
#define STRING_Cf0 STR_C STR_f "\0"
#define STRING_Cham0 STR_C STR_h STR_a STR_m "\0"
#define STRING_Cherokee0 STR_C STR_h STR_e STR_r STR_o STR_k STR_e STR_e "\0"
#define STRING_Cn0 STR_C STR_n "\0"
#define STRING_Co0 STR_C STR_o "\0"
#define STRING_Common0 STR_C STR_o STR_m STR_m STR_o STR_n "\0"
#define STRING_Coptic0 STR_C STR_o STR_p STR_t STR_i STR_c "\0"
#define STRING_Cs0 STR_C STR_s "\0"
#define STRING_Cuneiform0 STR_C STR_u STR_n STR_e STR_i STR_f STR_o STR_r STR_m "\0"
#define STRING_Cypriot0 STR_C STR_y STR_p STR_r STR_i STR_o STR_t "\0"
#define STRING_Cyrillic0 STR_C STR_y STR_r STR_i STR_l STR_l STR_i STR_c "\0"
#define STRING_Deseret0 STR_D STR_e STR_s STR_e STR_r STR_e STR_t "\0"
#define STRING_Devanagari0 STR_D STR_e STR_v STR_a STR_n STR_a STR_g STR_a STR_r STR_i "\0"
#define STRING_Egyptian_Hieroglyphs0 STR_E STR_g STR_y STR_p STR_t STR_i STR_a STR_n STR_UNDERSCORE STR_H STR_i STR_e STR_r STR_o STR_g STR_l STR_y STR_p STR_h STR_s "\0"
#define STRING_Ethiopic0 STR_E STR_t STR_h STR_i STR_o STR_p STR_i STR_c "\0"
#define STRING_Georgian0 STR_G STR_e STR_o STR_r STR_g STR_i STR_a STR_n "\0"
#define STRING_Glagolitic0 STR_G STR_l STR_a STR_g STR_o STR_l STR_i STR_t STR_i STR_c "\0"
#define STRING_Gothic0 STR_G STR_o STR_t STR_h STR_i STR_c "\0"
#define STRING_Greek0 STR_G STR_r STR_e STR_e STR_k "\0"
#define STRING_Gujarati0 STR_G STR_u STR_j STR_a STR_r STR_a STR_t STR_i "\0"
#define STRING_Gurmukhi0 STR_G STR_u STR_r STR_m STR_u STR_k STR_h STR_i "\0"
#define STRING_Han0 STR_H STR_a STR_n "\0"
#define STRING_Hangul0 STR_H STR_a STR_n STR_g STR_u STR_l "\0"
#define STRING_Hanunoo0 STR_H STR_a STR_n STR_u STR_n STR_o STR_o "\0"
#define STRING_Hebrew0 STR_H STR_e STR_b STR_r STR_e STR_w "\0"
#define STRING_Hiragana0 STR_H STR_i STR_r STR_a STR_g STR_a STR_n STR_a "\0"
#define STRING_Imperial_Aramaic0 STR_I STR_m STR_p STR_e STR_r STR_i STR_a STR_l STR_UNDERSCORE STR_A STR_r STR_a STR_m STR_a STR_i STR_c "\0"
#define STRING_Inherited0 STR_I STR_n STR_h STR_e STR_r STR_i STR_t STR_e STR_d "\0"
#define STRING_Inscriptional_Pahlavi0 STR_I STR_n STR_s STR_c STR_r STR_i STR_p STR_t STR_i STR_o STR_n STR_a STR_l STR_UNDERSCORE STR_P STR_a STR_h STR_l STR_a STR_v STR_i "\0"
#define STRING_Inscriptional_Parthian0 STR_I STR_n STR_s STR_c STR_r STR_i STR_p STR_t STR_i STR_o STR_n STR_a STR_l STR_UNDERSCORE STR_P STR_a STR_r STR_t STR_h STR_i STR_a STR_n "\0"
#define STRING_Javanese0 STR_J STR_a STR_v STR_a STR_n STR_e STR_s STR_e "\0"
#define STRING_Kaithi0 STR_K STR_a STR_i STR_t STR_h STR_i "\0"
#define STRING_Kannada0 STR_K STR_a STR_n STR_n STR_a STR_d STR_a "\0"
#define STRING_Katakana0 STR_K STR_a STR_t STR_a STR_k STR_a STR_n STR_a "\0"
#define STRING_Kayah_Li0 STR_K STR_a STR_y STR_a STR_h STR_UNDERSCORE STR_L STR_i "\0"
#define STRING_Kharoshthi0 STR_K STR_h STR_a STR_r STR_o STR_s STR_h STR_t STR_h STR_i "\0"
#define STRING_Khmer0 STR_K STR_h STR_m STR_e STR_r "\0"
#define STRING_L0 STR_L "\0"
#define STRING_L_AMPERSAND0 STR_L STR_AMPERSAND "\0"
#define STRING_Lao0 STR_L STR_a STR_o "\0"
#define STRING_Latin0 STR_L STR_a STR_t STR_i STR_n "\0"
#define STRING_Lepcha0 STR_L STR_e STR_p STR_c STR_h STR_a "\0"
#define STRING_Limbu0 STR_L STR_i STR_m STR_b STR_u "\0"
#define STRING_Linear_B0 STR_L STR_i STR_n STR_e STR_a STR_r STR_UNDERSCORE STR_B "\0"
#define STRING_Lisu0 STR_L STR_i STR_s STR_u "\0"
#define STRING_Ll0 STR_L STR_l "\0"
#define STRING_Lm0 STR_L STR_m "\0"
#define STRING_Lo0 STR_L STR_o "\0"
#define STRING_Lt0 STR_L STR_t "\0"
#define STRING_Lu0 STR_L STR_u "\0"
#define STRING_Lycian0 STR_L STR_y STR_c STR_i STR_a STR_n "\0"
#define STRING_Lydian0 STR_L STR_y STR_d STR_i STR_a STR_n "\0"
#define STRING_M0 STR_M "\0"
#define STRING_Malayalam0 STR_M STR_a STR_l STR_a STR_y STR_a STR_l STR_a STR_m "\0"
#define STRING_Mandaic0 STR_M STR_a STR_n STR_d STR_a STR_i STR_c "\0"
#define STRING_Mc0 STR_M STR_c "\0"
#define STRING_Me0 STR_M STR_e "\0"
#define STRING_Meetei_Mayek0 STR_M STR_e STR_e STR_t STR_e STR_i STR_UNDERSCORE STR_M STR_a STR_y STR_e STR_k "\0"
#define STRING_Mn0 STR_M STR_n "\0"
#define STRING_Mongolian0 STR_M STR_o STR_n STR_g STR_o STR_l STR_i STR_a STR_n "\0"
#define STRING_Myanmar0 STR_M STR_y STR_a STR_n STR_m STR_a STR_r "\0"
#define STRING_N0 STR_N "\0"
#define STRING_Nd0 STR_N STR_d "\0"
#define STRING_New_Tai_Lue0 STR_N STR_e STR_w STR_UNDERSCORE STR_T STR_a STR_i STR_UNDERSCORE STR_L STR_u STR_e "\0"
#define STRING_Nko0 STR_N STR_k STR_o "\0"
#define STRING_Nl0 STR_N STR_l "\0"
#define STRING_No0 STR_N STR_o "\0"
#define STRING_Ogham0 STR_O STR_g STR_h STR_a STR_m "\0"
#define STRING_Ol_Chiki0 STR_O STR_l STR_UNDERSCORE STR_C STR_h STR_i STR_k STR_i "\0"
#define STRING_Old_Italic0 STR_O STR_l STR_d STR_UNDERSCORE STR_I STR_t STR_a STR_l STR_i STR_c "\0"
#define STRING_Old_Persian0 STR_O STR_l STR_d STR_UNDERSCORE STR_P STR_e STR_r STR_s STR_i STR_a STR_n "\0"
#define STRING_Old_South_Arabian0 STR_O STR_l STR_d STR_UNDERSCORE STR_S STR_o STR_u STR_t STR_h STR_UNDERSCORE STR_A STR_r STR_a STR_b STR_i STR_a STR_n "\0"
#define STRING_Old_Turkic0 STR_O STR_l STR_d STR_UNDERSCORE STR_T STR_u STR_r STR_k STR_i STR_c "\0"
#define STRING_Oriya0 STR_O STR_r STR_i STR_y STR_a "\0"
#define STRING_Osmanya0 STR_O STR_s STR_m STR_a STR_n STR_y STR_a "\0"
#define STRING_P0 STR_P "\0"
#define STRING_Pc0 STR_P STR_c "\0"
#define STRING_Pd0 STR_P STR_d "\0"
#define STRING_Pe0 STR_P STR_e "\0"
#define STRING_Pf0 STR_P STR_f "\0"
#define STRING_Phags_Pa0 STR_P STR_h STR_a STR_g STR_s STR_UNDERSCORE STR_P STR_a "\0"
#define STRING_Phoenician0 STR_P STR_h STR_o STR_e STR_n STR_i STR_c STR_i STR_a STR_n "\0"
#define STRING_Pi0 STR_P STR_i "\0"
#define STRING_Po0 STR_P STR_o "\0"
#define STRING_Ps0 STR_P STR_s "\0"
#define STRING_Rejang0 STR_R STR_e STR_j STR_a STR_n STR_g "\0"
#define STRING_Runic0 STR_R STR_u STR_n STR_i STR_c "\0"
#define STRING_S0 STR_S "\0"
#define STRING_Samaritan0 STR_S STR_a STR_m STR_a STR_r STR_i STR_t STR_a STR_n "\0"
#define STRING_Saurashtra0 STR_S STR_a STR_u STR_r STR_a STR_s STR_h STR_t STR_r STR_a "\0"
#define STRING_Sc0 STR_S STR_c "\0"
#define STRING_Shavian0 STR_S STR_h STR_a STR_v STR_i STR_a STR_n "\0"
#define STRING_Sinhala0 STR_S STR_i STR_n STR_h STR_a STR_l STR_a "\0"
#define STRING_Sk0 STR_S STR_k "\0"
#define STRING_Sm0 STR_S STR_m "\0"
#define STRING_So0 STR_S STR_o "\0"
#define STRING_Sundanese0 STR_S STR_u STR_n STR_d STR_a STR_n STR_e STR_s STR_e "\0"
#define STRING_Syloti_Nagri0 STR_S STR_y STR_l STR_o STR_t STR_i STR_UNDERSCORE STR_N STR_a STR_g STR_r STR_i "\0"
#define STRING_Syriac0 STR_S STR_y STR_r STR_i STR_a STR_c "\0"
#define STRING_Tagalog0 STR_T STR_a STR_g STR_a STR_l STR_o STR_g "\0"
#define STRING_Tagbanwa0 STR_T STR_a STR_g STR_b STR_a STR_n STR_w STR_a "\0"
#define STRING_Tai_Le0 STR_T STR_a STR_i STR_UNDERSCORE STR_L STR_e "\0"
#define STRING_Tai_Tham0 STR_T STR_a STR_i STR_UNDERSCORE STR_T STR_h STR_a STR_m "\0"
#define STRING_Tai_Viet0 STR_T STR_a STR_i STR_UNDERSCORE STR_V STR_i STR_e STR_t "\0"
#define STRING_Tamil0 STR_T STR_a STR_m STR_i STR_l "\0"
#define STRING_Telugu0 STR_T STR_e STR_l STR_u STR_g STR_u "\0"
#define STRING_Thaana0 STR_T STR_h STR_a STR_a STR_n STR_a "\0"
#define STRING_Thai0 STR_T STR_h STR_a STR_i "\0"
#define STRING_Tibetan0 STR_T STR_i STR_b STR_e STR_t STR_a STR_n "\0"
#define STRING_Tifinagh0 STR_T STR_i STR_f STR_i STR_n STR_a STR_g STR_h "\0"
#define STRING_Ugaritic0 STR_U STR_g STR_a STR_r STR_i STR_t STR_i STR_c "\0"
#define STRING_Vai0 STR_V STR_a STR_i "\0"
#define STRING_Xan0 STR_X STR_a STR_n "\0"
#define STRING_Xps0 STR_X STR_p STR_s "\0"
#define STRING_Xsp0 STR_X STR_s STR_p "\0"
#define STRING_Xwd0 STR_X STR_w STR_d "\0"
#define STRING_Yi0 STR_Y STR_i "\0"
#define STRING_Z0 STR_Z "\0"
#define STRING_Zl0 STR_Z STR_l "\0"
#define STRING_Zp0 STR_Z STR_p "\0"
#define STRING_Zs0 STR_Z STR_s "\0"

const char PRIV(utt_names)[] =
  STRING_Any0
  STRING_Arabic0
  STRING_Armenian0
  STRING_Avestan0
  STRING_Balinese0
  STRING_Bamum0
  STRING_Batak0
  STRING_Bengali0
  STRING_Bopomofo0
  STRING_Brahmi0
  STRING_Braille0
  STRING_Buginese0
  STRING_Buhid0
  STRING_C0
  STRING_Canadian_Aboriginal0
  STRING_Carian0
  STRING_Cc0
  STRING_Cf0
  STRING_Cham0
  STRING_Cherokee0
  STRING_Cn0
  STRING_Co0
  STRING_Common0
  STRING_Coptic0
  STRING_Cs0
  STRING_Cuneiform0
  STRING_Cypriot0
  STRING_Cyrillic0
  STRING_Deseret0
  STRING_Devanagari0
  STRING_Egyptian_Hieroglyphs0
  STRING_Ethiopic0
  STRING_Georgian0
  STRING_Glagolitic0
  STRING_Gothic0
  STRING_Greek0
  STRING_Gujarati0
  STRING_Gurmukhi0
  STRING_Han0
  STRING_Hangul0
  STRING_Hanunoo0
  STRING_Hebrew0
  STRING_Hiragana0
  STRING_Imperial_Aramaic0
  STRING_Inherited0
  STRING_Inscriptional_Pahlavi0
  STRING_Inscriptional_Parthian0
  STRING_Javanese0
  STRING_Kaithi0
  STRING_Kannada0
  STRING_Katakana0
  STRING_Kayah_Li0
  STRING_Kharoshthi0
  STRING_Khmer0
  STRING_L0
  STRING_L_AMPERSAND0
  STRING_Lao0
  STRING_Latin0
  STRING_Lepcha0
  STRING_Limbu0
  STRING_Linear_B0
  STRING_Lisu0
  STRING_Ll0
  STRING_Lm0
  STRING_Lo0
  STRING_Lt0
  STRING_Lu0
  STRING_Lycian0
  STRING_Lydian0
  STRING_M0
  STRING_Malayalam0
  STRING_Mandaic0
  STRING_Mc0
  STRING_Me0
  STRING_Meetei_Mayek0
  STRING_Mn0
  STRING_Mongolian0
  STRING_Myanmar0
  STRING_N0
  STRING_Nd0
  STRING_New_Tai_Lue0
  STRING_Nko0
  STRING_Nl0
  STRING_No0
  STRING_Ogham0
  STRING_Ol_Chiki0
  STRING_Old_Italic0
  STRING_Old_Persian0
  STRING_Old_South_Arabian0
  STRING_Old_Turkic0
  STRING_Oriya0
  STRING_Osmanya0
  STRING_P0
  STRING_Pc0
  STRING_Pd0
  STRING_Pe0
  STRING_Pf0
  STRING_Phags_Pa0
  STRING_Phoenician0
  STRING_Pi0
  STRING_Po0
  STRING_Ps0
  STRING_Rejang0
  STRING_Runic0
  STRING_S0
  STRING_Samaritan0
  STRING_Saurashtra0
  STRING_Sc0
  STRING_Shavian0
  STRING_Sinhala0
  STRING_Sk0
  STRING_Sm0
  STRING_So0
  STRING_Sundanese0
  STRING_Syloti_Nagri0
  STRING_Syriac0
  STRING_Tagalog0
  STRING_Tagbanwa0
  STRING_Tai_Le0
  STRING_Tai_Tham0
  STRING_Tai_Viet0
  STRING_Tamil0
  STRING_Telugu0
  STRING_Thaana0
  STRING_Thai0
  STRING_Tibetan0
  STRING_Tifinagh0
  STRING_Ugaritic0
  STRING_Vai0
  STRING_Xan0
  STRING_Xps0
  STRING_Xsp0
  STRING_Xwd0
  STRING_Yi0
  STRING_Z0
  STRING_Zl0
  STRING_Zp0
  STRING_Zs0;

const ucp_type_table PRIV(utt)[] = {
  {   0, PT_ANY, 0 },
  {   4, PT_SC, ucp_Arabic },
  {  11, PT_SC, ucp_Armenian },
  {  20, PT_SC, ucp_Avestan },
  {  28, PT_SC, ucp_Balinese },
  {  37, PT_SC, ucp_Bamum },
  {  43, PT_SC, ucp_Batak },
  {  49, PT_SC, ucp_Bengali },
  {  57, PT_SC, ucp_Bopomofo },
  {  66, PT_SC, ucp_Brahmi },
  {  73, PT_SC, ucp_Braille },
  {  81, PT_SC, ucp_Buginese },
  {  90, PT_SC, ucp_Buhid },
  {  96, PT_GC, ucp_C },
  {  98, PT_SC, ucp_Canadian_Aboriginal },
  { 118, PT_SC, ucp_Carian },
  { 125, PT_PC, ucp_Cc },
  { 128, PT_PC, ucp_Cf },
  { 131, PT_SC, ucp_Cham },
  { 136, PT_SC, ucp_Cherokee },
  { 145, PT_PC, ucp_Cn },
  { 148, PT_PC, ucp_Co },
  { 151, PT_SC, ucp_Common },
  { 158, PT_SC, ucp_Coptic },
  { 165, PT_PC, ucp_Cs },
  { 168, PT_SC, ucp_Cuneiform },
  { 178, PT_SC, ucp_Cypriot },
  { 186, PT_SC, ucp_Cyrillic },
  { 195, PT_SC, ucp_Deseret },
  { 203, PT_SC, ucp_Devanagari },
  { 214, PT_SC, ucp_Egyptian_Hieroglyphs },
  { 235, PT_SC, ucp_Ethiopic },
  { 244, PT_SC, ucp_Georgian },
  { 253, PT_SC, ucp_Glagolitic },
  { 264, PT_SC, ucp_Gothic },
  { 271, PT_SC, ucp_Greek },
  { 277, PT_SC, ucp_Gujarati },
  { 286, PT_SC, ucp_Gurmukhi },
  { 295, PT_SC, ucp_Han },
  { 299, PT_SC, ucp_Hangul },
  { 306, PT_SC, ucp_Hanunoo },
  { 314, PT_SC, ucp_Hebrew },
  { 321, PT_SC, ucp_Hiragana },
  { 330, PT_SC, ucp_Imperial_Aramaic },
  { 347, PT_SC, ucp_Inherited },
  { 357, PT_SC, ucp_Inscriptional_Pahlavi },
  { 379, PT_SC, ucp_Inscriptional_Parthian },
  { 402, PT_SC, ucp_Javanese },
  { 411, PT_SC, ucp_Kaithi },
  { 418, PT_SC, ucp_Kannada },
  { 426, PT_SC, ucp_Katakana },
  { 435, PT_SC, ucp_Kayah_Li },
  { 444, PT_SC, ucp_Kharoshthi },
  { 455, PT_SC, ucp_Khmer },
  { 461, PT_GC, ucp_L },
  { 463, PT_LAMP, 0 },
  { 466, PT_SC, ucp_Lao },
  { 470, PT_SC, ucp_Latin },
  { 476, PT_SC, ucp_Lepcha },
  { 483, PT_SC, ucp_Limbu },
  { 489, PT_SC, ucp_Linear_B },
  { 498, PT_SC, ucp_Lisu },
  { 503, PT_PC, ucp_Ll },
  { 506, PT_PC, ucp_Lm },
  { 509, PT_PC, ucp_Lo },
  { 512, PT_PC, ucp_Lt },
  { 515, PT_PC, ucp_Lu },
  { 518, PT_SC, ucp_Lycian },
  { 525, PT_SC, ucp_Lydian },
  { 532, PT_GC, ucp_M },
  { 534, PT_SC, ucp_Malayalam },
  { 544, PT_SC, ucp_Mandaic },
  { 552, PT_PC, ucp_Mc },
  { 555, PT_PC, ucp_Me },
  { 558, PT_SC, ucp_Meetei_Mayek },
  { 571, PT_PC, ucp_Mn },
  { 574, PT_SC, ucp_Mongolian },
  { 584, PT_SC, ucp_Myanmar },
  { 592, PT_GC, ucp_N },
  { 594, PT_PC, ucp_Nd },
  { 597, PT_SC, ucp_New_Tai_Lue },
  { 609, PT_SC, ucp_Nko },
  { 613, PT_PC, ucp_Nl },
  { 616, PT_PC, ucp_No },
  { 619, PT_SC, ucp_Ogham },
  { 625, PT_SC, ucp_Ol_Chiki },
  { 634, PT_SC, ucp_Old_Italic },
  { 645, PT_SC, ucp_Old_Persian },
  { 657, PT_SC, ucp_Old_South_Arabian },
  { 675, PT_SC, ucp_Old_Turkic },
  { 686, PT_SC, ucp_Oriya },
  { 692, PT_SC, ucp_Osmanya },
  { 700, PT_GC, ucp_P },
  { 702, PT_PC, ucp_Pc },
  { 705, PT_PC, ucp_Pd },
  { 708, PT_PC, ucp_Pe },
  { 711, PT_PC, ucp_Pf },
  { 714, PT_SC, ucp_Phags_Pa },
  { 723, PT_SC, ucp_Phoenician },
  { 734, PT_PC, ucp_Pi },
  { 737, PT_PC, ucp_Po },
  { 740, PT_PC, ucp_Ps },
  { 743, PT_SC, ucp_Rejang },
  { 750, PT_SC, ucp_Runic },
  { 756, PT_GC, ucp_S },
  { 758, PT_SC, ucp_Samaritan },
  { 768, PT_SC, ucp_Saurashtra },
  { 779, PT_PC, ucp_Sc },
  { 782, PT_SC, ucp_Shavian },
  { 790, PT_SC, ucp_Sinhala },
  { 798, PT_PC, ucp_Sk },
  { 801, PT_PC, ucp_Sm },
  { 804, PT_PC, ucp_So },
  { 807, PT_SC, ucp_Sundanese },
  { 817, PT_SC, ucp_Syloti_Nagri },
  { 830, PT_SC, ucp_Syriac },
  { 837, PT_SC, ucp_Tagalog },
  { 845, PT_SC, ucp_Tagbanwa },
  { 854, PT_SC, ucp_Tai_Le },
  { 861, PT_SC, ucp_Tai_Tham },
  { 870, PT_SC, ucp_Tai_Viet },
  { 879, PT_SC, ucp_Tamil },
  { 885, PT_SC, ucp_Telugu },
  { 892, PT_SC, ucp_Thaana },
  { 899, PT_SC, ucp_Thai },
  { 904, PT_SC, ucp_Tibetan },
  { 912, PT_SC, ucp_Tifinagh },
  { 921, PT_SC, ucp_Ugaritic },
  { 930, PT_SC, ucp_Vai },
  { 934, PT_ALNUM, 0 },
  { 938, PT_PXSPACE, 0 },
  { 942, PT_SPACE, 0 },
  { 946, PT_WORD, 0 },
  { 950, PT_SC, ucp_Yi },
  { 953, PT_GC, ucp_Z },
  { 955, PT_PC, ucp_Zl },
  { 958, PT_PC, ucp_Zp },
  { 961, PT_PC, ucp_Zs }
};

const int PRIV(utt_size) = sizeof(PRIV(utt)) / sizeof(ucp_type_table);

#endif /* SUPPORT_UTF */

/* End of pcre_tables.c */
