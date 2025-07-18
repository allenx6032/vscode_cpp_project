/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/* PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language (but see
below for why this module is different).

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


/* This module contains the external function pcre_dfa_exec(), which is an
alternative matching function that uses a sort of DFA algorithm (not a true
FSM). This is NOT Perl- compatible, but it has advantages in certain
applications. */


/* NOTE ABOUT PERFORMANCE: A user of this function sent some code that improved
the performance of his patterns greatly. I could not use it as it stood, as it
was not thread safe, and made assumptions about pattern sizes. Also, it caused
test 7 to loop, and test 9 to crash with a segfault.

The issue is the check for duplicate states, which is done by a simple linear
search up the state list. (Grep for "duplicate" below to find the code.) For
many patterns, there will never be many states active at one time, so a simple
linear search is fine. In patterns that have many active states, it might be a
bottleneck. The suggested code used an indexing scheme to remember which states
had previously been used for each character, and avoided the linear search when
it knew there was no chance of a duplicate. This was implemented when adding
states to the state lists.

I wrote some thread-safe, not-limited code to try something similar at the time
of checking for duplicates (instead of when adding states), using index vectors
on the stack. It did give a 13% improvement with one specially constructed
pattern for certain subject strings, but on other strings and on many of the
simpler patterns in the test suite it did worse. The major problem, I think,
was the extra time to initialize the index. This had to be done for each call
of internal_dfa_exec(). (The supplied patch used a static vector, initialized
only once - I suspect this was the cause of the problems with the tests.)

Overall, I concluded that the gains in some cases did not outweigh the losses
in others, so I abandoned this code. */



#ifdef HAVE_PCRE_CONFIG_H
#include "pcre_config.h"
#endif

#define NLBLOCK md             /* Block containing newline information */
#define PSSTART start_subject  /* Field containing processed string start */
#define PSEND   end_subject    /* Field containing processed string end */

#include "pcre_internal.h"


/* For use to indent debugging output */

#define SP "                   "


/*************************************************
*      Code parameters and static tables         *
*************************************************/

/* These are offsets that are used to turn the OP_TYPESTAR and friends opcodes
into others, under special conditions. A gap of 20 between the blocks should be
enough. The resulting opcodes don't have to be less than 256 because they are
never stored, so we push them well clear of the normal opcodes. */

#define OP_PROP_EXTRA       300
#define OP_EXTUNI_EXTRA     320
#define OP_ANYNL_EXTRA      340
#define OP_HSPACE_EXTRA     360
#define OP_VSPACE_EXTRA     380


/* This table identifies those opcodes that are followed immediately by a
character that is to be tested in some way. This makes it possible to
centralize the loading of these characters. In the case of Type * etc, the
"character" is the opcode for \D, \d, \S, \s, \W, or \w, which will always be a
small value. Non-zero values in the table are the offsets from the opcode where
the character is to be found. ***NOTE*** If the start of this table is
modified, the three tables that follow must also be modified. */

static const pcre_uint8 coptable[] = {
  0,                             /* End                                    */
  0, 0, 0, 0, 0,                 /* \A, \G, \K, \B, \b                     */
  0, 0, 0, 0, 0, 0,              /* \D, \d, \S, \s, \W, \w                 */
  0, 0, 0,                       /* Any, AllAny, Anybyte                   */
  0, 0,                          /* \P, \p                                 */
  0, 0, 0, 0, 0,                 /* \R, \H, \h, \V, \v                     */
  0,                             /* \X                                     */
  0, 0, 0, 0, 0, 0,              /* \Z, \z, ^, ^M, $, $M                   */
  1,                             /* Char                                   */
  1,                             /* Chari                                  */
  1,                             /* not                                    */
  1,                             /* noti                                   */
  /* Positive single-char repeats                                          */
  1, 1, 1, 1, 1, 1,              /* *, *?, +, +?, ?, ??                    */
  1+IMM2_SIZE, 1+IMM2_SIZE,      /* upto, minupto                          */
  1+IMM2_SIZE,                   /* exact                                  */
  1, 1, 1, 1+IMM2_SIZE,          /* *+, ++, ?+, upto+                      */
  1, 1, 1, 1, 1, 1,              /* *I, *?I, +I, +?I, ?I, ??I              */
  1+IMM2_SIZE, 1+IMM2_SIZE,      /* upto I, minupto I                      */
  1+IMM2_SIZE,                   /* exact I                                */
  1, 1, 1, 1+IMM2_SIZE,          /* *+I, ++I, ?+I, upto+I                  */
  /* Negative single-char repeats - only for chars < 256                   */
  1, 1, 1, 1, 1, 1,              /* NOT *, *?, +, +?, ?, ??                */
  1+IMM2_SIZE, 1+IMM2_SIZE,      /* NOT upto, minupto                      */
  1+IMM2_SIZE,                   /* NOT exact                              */
  1, 1, 1, 1+IMM2_SIZE,          /* NOT *+, ++, ?+, upto+                  */
  1, 1, 1, 1, 1, 1,              /* NOT *I, *?I, +I, +?I, ?I, ??I          */
  1+IMM2_SIZE, 1+IMM2_SIZE,      /* NOT upto I, minupto I                  */
  1+IMM2_SIZE,                   /* NOT exact I                            */
  1, 1, 1, 1+IMM2_SIZE,          /* NOT *+I, ++I, ?+I, upto+I              */
  /* Positive type repeats                                                 */
  1, 1, 1, 1, 1, 1,              /* Type *, *?, +, +?, ?, ??               */
  1+IMM2_SIZE, 1+IMM2_SIZE,      /* Type upto, minupto                     */
  1+IMM2_SIZE,                   /* Type exact                             */
  1, 1, 1, 1+IMM2_SIZE,          /* Type *+, ++, ?+, upto+                 */
  /* Character class & ref repeats                                         */
  0, 0, 0, 0, 0, 0,              /* *, *?, +, +?, ?, ??                    */
  0, 0,                          /* CRRANGE, CRMINRANGE                    */
  0,                             /* CLASS                                  */
  0,                             /* NCLASS                                 */
  0,                             /* XCLASS - variable length               */
  0,                             /* REF                                    */
  0,                             /* REFI                                   */
  0,                             /* RECURSE                                */
  0,                             /* CALLOUT                                */
  0,                             /* Alt                                    */
  0,                             /* Ket                                    */
  0,                             /* KetRmax                                */
  0,                             /* KetRmin                                */
  0,                             /* KetRpos                                */
  0,                             /* Reverse                                */
  0,                             /* Assert                                 */
  0,                             /* Assert not                             */
  0,                             /* Assert behind                          */
  0,                             /* Assert behind not                      */
  0, 0,                          /* ONCE, ONCE_NC                          */
  0, 0, 0, 0, 0,                 /* BRA, BRAPOS, CBRA, CBRAPOS, COND       */
  0, 0, 0, 0, 0,                 /* SBRA, SBRAPOS, SCBRA, SCBRAPOS, SCOND  */
  0, 0,                          /* CREF, NCREF                            */
  0, 0,                          /* RREF, NRREF                            */
  0,                             /* DEF                                    */
  0, 0, 0,                       /* BRAZERO, BRAMINZERO, BRAPOSZERO        */
  0, 0, 0,                       /* MARK, PRUNE, PRUNE_ARG                 */
  0, 0, 0, 0,                    /* SKIP, SKIP_ARG, THEN, THEN_ARG         */
  0, 0, 0, 0,                    /* COMMIT, FAIL, ACCEPT, ASSERT_ACCEPT    */
  0, 0                           /* CLOSE, SKIPZERO  */
};

/* This table identifies those opcodes that inspect a character. It is used to
remember the fact that a character could have been inspected when the end of
the subject is reached. ***NOTE*** If the start of this table is modified, the
two tables that follow must also be modified. */

static const pcre_uint8 poptable[] = {
  0,                             /* End                                    */
  0, 0, 0, 1, 1,                 /* \A, \G, \K, \B, \b                     */
  1, 1, 1, 1, 1, 1,              /* \D, \d, \S, \s, \W, \w                 */
  1, 1, 1,                       /* Any, AllAny, Anybyte                   */
  1, 1,                          /* \P, \p                                 */
  1, 1, 1, 1, 1,                 /* \R, \H, \h, \V, \v                     */
  1,                             /* \X                                     */
  0, 0, 0, 0, 0, 0,              /* \Z, \z, ^, ^M, $, $M                   */
  1,                             /* Char                                   */
  1,                             /* Chari                                  */
  1,                             /* not                                    */
  1,                             /* noti                                   */
  /* Positive single-char repeats                                          */
  1, 1, 1, 1, 1, 1,              /* *, *?, +, +?, ?, ??                    */
  1, 1, 1,                       /* upto, minupto, exact                   */
  1, 1, 1, 1,                    /* *+, ++, ?+, upto+                      */
  1, 1, 1, 1, 1, 1,              /* *I, *?I, +I, +?I, ?I, ??I              */
  1, 1, 1,                       /* upto I, minupto I, exact I             */
  1, 1, 1, 1,                    /* *+I, ++I, ?+I, upto+I                  */
  /* Negative single-char repeats - only for chars < 256                   */
  1, 1, 1, 1, 1, 1,              /* NOT *, *?, +, +?, ?, ??                */
  1, 1, 1,                       /* NOT upto, minupto, exact               */
  1, 1, 1, 1,                    /* NOT *+, ++, ?+, upto+                  */
  1, 1, 1, 1, 1, 1,              /* NOT *I, *?I, +I, +?I, ?I, ??I          */
  1, 1, 1,                       /* NOT upto I, minupto I, exact I         */
  1, 1, 1, 1,                    /* NOT *+I, ++I, ?+I, upto+I              */
  /* Positive type repeats                                                 */
  1, 1, 1, 1, 1, 1,              /* Type *, *?, +, +?, ?, ??               */
  1, 1, 1,                       /* Type upto, minupto, exact              */
  1, 1, 1, 1,                    /* Type *+, ++, ?+, upto+                 */
  /* Character class & ref repeats                                         */
  1, 1, 1, 1, 1, 1,              /* *, *?, +, +?, ?, ??                    */
  1, 1,                          /* CRRANGE, CRMINRANGE                    */
  1,                             /* CLASS                                  */
  1,                             /* NCLASS                                 */
  1,                             /* XCLASS - variable length               */
  0,                             /* REF                                    */
  0,                             /* REFI                                   */
  0,                             /* RECURSE                                */
  0,                             /* CALLOUT                                */
  0,                             /* Alt                                    */
  0,                             /* Ket                                    */
  0,                             /* KetRmax                                */
  0,                             /* KetRmin                                */
  0,                             /* KetRpos                                */
  0,                             /* Reverse                                */
  0,                             /* Assert                                 */
  0,                             /* Assert not                             */
  0,                             /* Assert behind                          */
  0,                             /* Assert behind not                      */
  0, 0,                          /* ONCE, ONCE_NC                          */
  0, 0, 0, 0, 0,                 /* BRA, BRAPOS, CBRA, CBRAPOS, COND       */
  0, 0, 0, 0, 0,                 /* SBRA, SBRAPOS, SCBRA, SCBRAPOS, SCOND  */
  0, 0,                          /* CREF, NCREF                            */
  0, 0,                          /* RREF, NRREF                            */
  0,                             /* DEF                                    */
  0, 0, 0,                       /* BRAZERO, BRAMINZERO, BRAPOSZERO        */
  0, 0, 0,                       /* MARK, PRUNE, PRUNE_ARG                 */
  0, 0, 0, 0,                    /* SKIP, SKIP_ARG, THEN, THEN_ARG         */
  0, 0, 0, 0,                    /* COMMIT, FAIL, ACCEPT, ASSERT_ACCEPT    */
  0, 0                           /* CLOSE, SKIPZERO                        */
};

/* These 2 tables allow for compact code for testing for \D, \d, \S, \s, \W,
and \w */

static const pcre_uint8 toptable1[] = {
  0, 0, 0, 0, 0, 0,
  ctype_digit, ctype_digit,
  ctype_space, ctype_space,
  ctype_word,  ctype_word,
  0, 0                            /* OP_ANY, OP_ALLANY */
};

static const pcre_uint8 toptable2[] = {
  0, 0, 0, 0, 0, 0,
  ctype_digit, 0,
  ctype_space, 0,
  ctype_word,  0,
  1, 1                            /* OP_ANY, OP_ALLANY */
};


/* Structure for holding data about a particular state, which is in effect the
current data for an active path through the match tree. It must consist
entirely of ints because the working vector we are passed, and which we put
these structures in, is a vector of ints. */

typedef struct stateblock {
  int offset;                     /* Offset to opcode */
  int count;                      /* Count for repeats */
  int data;                       /* Some use extra data */
} stateblock;

#define INTS_PER_STATEBLOCK  (sizeof(stateblock)/sizeof(int))


#ifdef PCRE_DEBUG
/*************************************************
*             Print character string             *
*************************************************/

/* Character string printing function for debugging.

Arguments:
  p            points to string
  length       number of bytes
  f            where to print

Returns:       nothing
*/

static void
pchars(const pcre_uchar *p, int length, FILE *f)
{
int c;
while (length-- > 0)
  {
  if (isprint(c = *(p++)))
    fprintf(f, "%c", c);
  else
    fprintf(f, "\\x%02x", c);
  }
}
#endif



/*************************************************
*    Execute a Regular Expression - DFA engine   *
*************************************************/

/* This internal function applies a compiled pattern to a subject string,
starting at a given point, using a DFA engine. This function is called from the
external one, possibly multiple times if the pattern is not anchored. The
function calls itself recursively for some kinds of subpattern.

Arguments:
  md                the match_data block with fixed information
  this_start_code   the opening bracket of this subexpression's code
  current_subject   where we currently are in the subject string
  start_offset      start offset in the subject string
  offsets           vector to contain the matching string offsets
  offsetcount       size of same
  workspace         vector of workspace
  wscount           size of same
  rlevel            function call recursion level

Returns:            > 0 => number of match offset pairs placed in offsets
                    = 0 => offsets overflowed; longest matches are present
                     -1 => failed to match
                   < -1 => some kind of unexpected problem

The following macros are used for adding states to the two state vectors (one
for the current character, one for the following character). */

#define ADD_ACTIVE(x,y) \
  if (active_count++ < wscount) \
    { \
    next_active_state->offset = (x); \
    next_active_state->count  = (y); \
    next_active_state++; \
    DPRINTF(("%.*sADD_ACTIVE(%d,%d)\n", rlevel*2-2, SP, (x), (y))); \
    } \
  else return PCRE_ERROR_DFA_WSSIZE

#define ADD_ACTIVE_DATA(x,y,z) \
  if (active_count++ < wscount) \
    { \
    next_active_state->offset = (x); \
    next_active_state->count  = (y); \
    next_active_state->data   = (z); \
    next_active_state++; \
    DPRINTF(("%.*sADD_ACTIVE_DATA(%d,%d,%d)\n", rlevel*2-2, SP, (x), (y), (z))); \
    } \
  else return PCRE_ERROR_DFA_WSSIZE

#define ADD_NEW(x,y) \
  if (new_count++ < wscount) \
    { \
    next_new_state->offset = (x); \
    next_new_state->count  = (y); \
    next_new_state++; \
    DPRINTF(("%.*sADD_NEW(%d,%d)\n", rlevel*2-2, SP, (x), (y))); \
    } \
  else return PCRE_ERROR_DFA_WSSIZE

#define ADD_NEW_DATA(x,y,z) \
  if (new_count++ < wscount) \
    { \
    next_new_state->offset = (x); \
    next_new_state->count  = (y); \
    next_new_state->data   = (z); \
    next_new_state++; \
    DPRINTF(("%.*sADD_NEW_DATA(%d,%d,%d)\n", rlevel*2-2, SP, (x), (y), (z))); \
    } \
  else return PCRE_ERROR_DFA_WSSIZE

/* And now, here is the code */

static int
internal_dfa_exec(
  dfa_match_data *md,
  const pcre_uchar *this_start_code,
  const pcre_uchar *current_subject,
  int start_offset,
  int *offsets,
  int offsetcount,
  int *workspace,
  int wscount,
  int  rlevel)
{
stateblock *active_states, *new_states, *temp_states;
stateblock *next_active_state, *next_new_state;

const pcre_uint8 *ctypes, *lcc, *fcc;
const pcre_uchar *ptr;
const pcre_uchar *end_code, *first_op;

dfa_recursion_info new_recursive;

int active_count, new_count, match_count;

/* Some fields in the md block are frequently referenced, so we load them into
independent variables in the hope that this will perform better. */

const pcre_uchar *start_subject = md->start_subject;
const pcre_uchar *end_subject = md->end_subject;
const pcre_uchar *start_code = md->start_code;

#ifdef SUPPORT_UTF
BOOL utf = (md->poptions & PCRE_UTF8) != 0;
#else
BOOL utf = FALSE;
#endif

rlevel++;
offsetcount &= (-2);

wscount -= 2;
wscount = (wscount - (wscount % (INTS_PER_STATEBLOCK * 2))) /
          (2 * INTS_PER_STATEBLOCK);

DPRINTF(("\n%.*s---------------------\n"
  "%.*sCall to internal_dfa_exec f=%d\n",
  rlevel*2-2, SP, rlevel*2-2, SP, rlevel));

ctypes = md->tables + ctypes_offset;
lcc = md->tables + lcc_offset;
fcc = md->tables + fcc_offset;

match_count = PCRE_ERROR_NOMATCH;   /* A negative number */

active_states = (stateblock *)(workspace + 2);
next_new_state = new_states = active_states + wscount;
new_count = 0;

first_op = this_start_code + 1 + LINK_SIZE +
  ((*this_start_code == OP_CBRA || *this_start_code == OP_SCBRA ||
    *this_start_code == OP_CBRAPOS || *this_start_code == OP_SCBRAPOS)
    ? IMM2_SIZE:0);

/* The first thing in any (sub) pattern is a bracket of some sort. Push all
the alternative states onto the list, and find out where the end is. This
makes is possible to use this function recursively, when we want to stop at a
matching internal ket rather than at the end.

If the first opcode in the first alternative is OP_REVERSE, we are dealing with
a backward assertion. In that case, we have to find out the maximum amount to
move back, and set up each alternative appropriately. */

if (*first_op == OP_REVERSE)
  {
  int max_back = 0;
  int gone_back;

  end_code = this_start_code;
  do
    {
    int back = GET(end_code, 2+LINK_SIZE);
    if (back > max_back) max_back = back;
    end_code += GET(end_code, 1);
    }
  while (*end_code == OP_ALT);

  /* If we can't go back the amount required for the longest lookbehind
  pattern, go back as far as we can; some alternatives may still be viable. */

#ifdef SUPPORT_UTF
  /* In character mode we have to step back character by character */

  if (utf)
    {
    for (gone_back = 0; gone_back < max_back; gone_back++)
      {
      if (current_subject <= start_subject) break;
      current_subject--;
      ACROSSCHAR(current_subject > start_subject, *current_subject, current_subject--);
      }
    }
  else
#endif

  /* In byte-mode we can do this quickly. */

    {
    gone_back = (current_subject - max_back < start_subject)?
      (int)(current_subject - start_subject) : max_back;
    current_subject -= gone_back;
    }

  /* Save the earliest consulted character */

  if (current_subject < md->start_used_ptr)
    md->start_used_ptr = current_subject;

  /* Now we can process the individual branches. */

  end_code = this_start_code;
  do
    {
    int back = GET(end_code, 2+LINK_SIZE);
    if (back <= gone_back)
      {
      int bstate = (int)(end_code - start_code + 2 + 2*LINK_SIZE);
      ADD_NEW_DATA(-bstate, 0, gone_back - back);
      }
    end_code += GET(end_code, 1);
    }
  while (*end_code == OP_ALT);
 }

/* This is the code for a "normal" subpattern (not a backward assertion). The
start of a whole pattern is always one of these. If we are at the top level,
we may be asked to restart matching from the same point that we reached for a
previous partial match. We still have to scan through the top-level branches to
find the end state. */

else
  {
  end_code = this_start_code;

  /* Restarting */

  if (rlevel == 1 && (md->moptions & PCRE_DFA_RESTART) != 0)
    {
    do { end_code += GET(end_code, 1); } while (*end_code == OP_ALT);
    new_count = workspace[1];
    if (!workspace[0])
      memcpy(new_states, active_states, new_count * sizeof(stateblock));
    }

  /* Not restarting */

  else
    {
    int length = 1 + LINK_SIZE +
      ((*this_start_code == OP_CBRA || *this_start_code == OP_SCBRA ||
        *this_start_code == OP_CBRAPOS || *this_start_code == OP_SCBRAPOS)
        ? IMM2_SIZE:0);
    do
      {
      ADD_NEW((int)(end_code - start_code + length), 0);
      end_code += GET(end_code, 1);
      length = 1 + LINK_SIZE;
      }
    while (*end_code == OP_ALT);
    }
  }

workspace[0] = 0;    /* Bit indicating which vector is current */

DPRINTF(("%.*sEnd state = %d\n", rlevel*2-2, SP, (int)(end_code - start_code)));

/* Loop for scanning the subject */

ptr = current_subject;
for (;;)
  {
  int i, j;
  int clen, dlen;
  unsigned int c, d;
  int forced_fail = 0;
  BOOL could_continue = FALSE;

  /* Make the new state list into the active state list and empty the
  new state list. */

  temp_states = active_states;
  active_states = new_states;
  new_states = temp_states;
  active_count = new_count;
  new_count = 0;

  workspace[0] ^= 1;              /* Remember for the restarting feature */
  workspace[1] = active_count;

#ifdef PCRE_DEBUG
  printf("%.*sNext character: rest of subject = \"", rlevel*2-2, SP);
  pchars(ptr, STRLEN_UC(ptr), stdout);
  printf("\"\n");

  printf("%.*sActive states: ", rlevel*2-2, SP);
  for (i = 0; i < active_count; i++)
    printf("%d/%d ", active_states[i].offset, active_states[i].count);
  printf("\n");
#endif

  /* Set the pointers for adding new states */

  next_active_state = active_states + active_count;
  next_new_state = new_states;

  /* Load the current character from the subject outside the loop, as many
  different states may want to look at it, and we assume that at least one
  will. */

  if (ptr < end_subject)
    {
    clen = 1;        /* Number of bytes in the character */
#ifdef SUPPORT_UTF
    if (utf) { GETCHARLEN(c, ptr, clen); } else
#endif  /* SUPPORT_UTF */
    c = *ptr;
    }
  else
    {
    clen = 0;        /* This indicates the end of the subject */
    c = NOTACHAR;    /* This value should never actually be used */
    }

  /* Scan up the active states and act on each one. The result of an action
  may be to add more states to the currently active list (e.g. on hitting a
  parenthesis) or it may be to put states on the new list, for considering
  when we move the character pointer on. */

  for (i = 0; i < active_count; i++)
    {
    stateblock *current_state = active_states + i;
    BOOL caseless = FALSE;
    const pcre_uchar *code;
    int state_offset = current_state->offset;
    int count, codevalue, rrc;

#ifdef PCRE_DEBUG
    printf ("%.*sProcessing state %d c=", rlevel*2-2, SP, state_offset);
    if (clen == 0) printf("EOL\n");
      else if (c > 32 && c < 127) printf("'%c'\n", c);
        else printf("0x%02x\n", c);
#endif

    /* A negative offset is a special case meaning "hold off going to this
    (negated) state until the number of characters in the data field have
    been skipped". */

    if (state_offset < 0)
      {
      if (current_state->data > 0)
        {
        DPRINTF(("%.*sSkipping this character\n", rlevel*2-2, SP));
        ADD_NEW_DATA(state_offset, current_state->count,
          current_state->data - 1);
        continue;
        }
      else
        {
        current_state->offset = state_offset = -state_offset;
        }
      }

    /* Check for a duplicate state with the same count, and skip if found.
    See the note at the head of this module about the possibility of improving
    performance here. */

    for (j = 0; j < i; j++)
      {
      if (active_states[j].offset == state_offset &&
          active_states[j].count == current_state->count)
        {
        DPRINTF(("%.*sDuplicate state: skipped\n", rlevel*2-2, SP));
        goto NEXT_ACTIVE_STATE;
        }
      }

    /* The state offset is the offset to the opcode */

    code = start_code + state_offset;
    codevalue = *code;

    /* If this opcode inspects a character, but we are at the end of the
    subject, remember the fact for use when testing for a partial match. */

    if (clen == 0 && poptable[codevalue] != 0)
      could_continue = TRUE;

    /* If this opcode is followed by an inline character, load it. It is
    tempting to test for the presence of a subject character here, but that
    is wrong, because sometimes zero repetitions of the subject are
    permitted.

    We also use this mechanism for opcodes such as OP_TYPEPLUS that take an
    argument that is not a data character - but is always one byte long. We
    have to take special action to deal with  \P, \p, \H, \h, \V, \v and \X in
    this case. To keep the other cases fast, convert these ones to new opcodes.
    */

    if (coptable[codevalue] > 0)
      {
      dlen = 1;
#ifdef SUPPORT_UTF
      if (utf) { GETCHARLEN(d, (code + coptable[codevalue]), dlen); } else
#endif  /* SUPPORT_UTF */
      d = code[coptable[codevalue]];
      if (codevalue >= OP_TYPESTAR)
        {
        switch(d)
          {
          case OP_ANYBYTE: return PCRE_ERROR_DFA_UITEM;
          case OP_NOTPROP:
          case OP_PROP: codevalue += OP_PROP_EXTRA; break;
          case OP_ANYNL: codevalue += OP_ANYNL_EXTRA; break;
          case OP_EXTUNI: codevalue += OP_EXTUNI_EXTRA; break;
          case OP_NOT_HSPACE:
          case OP_HSPACE: codevalue += OP_HSPACE_EXTRA; break;
          case OP_NOT_VSPACE:
          case OP_VSPACE: codevalue += OP_VSPACE_EXTRA; break;
          default: break;
          }
        }
      }
    else
      {
      dlen = 0;         /* Not strictly necessary, but compilers moan */
      d = NOTACHAR;     /* if these variables are not set. */
      }


    /* Now process the individual opcodes */

    switch (codevalue)
      {
/* ========================================================================== */
      /* These cases are never obeyed. This is a fudge that causes a compile-
      time error if the vectors coptable or poptable, which are indexed by
      opcode, are not the correct length. It seems to be the only way to do
      such a check at compile time, as the sizeof() operator does not work
      in the C preprocessor. */

      case OP_TABLE_LENGTH:
      case OP_TABLE_LENGTH +
        ((sizeof(coptable) == OP_TABLE_LENGTH) &&
         (sizeof(poptable) == OP_TABLE_LENGTH)):
      break;

/* ========================================================================== */
      /* Reached a closing bracket. If not at the end of the pattern, carry
      on with the next opcode. For repeating opcodes, also add the repeat
      state. Note that KETRPOS will always be encountered at the end of the
      subpattern, because the possessive subpattern repeats are always handled
      using recursive calls. Thus, it never adds any new states.

      At the end of the (sub)pattern, unless we have an empty string and
      PCRE_NOTEMPTY is set, or PCRE_NOTEMPTY_ATSTART is set and we are at the
      start of the subject, save the match data, shifting up all previous
      matches so we always have the longest first. */

      case OP_KET:
      case OP_KETRMIN:
      case OP_KETRMAX:
      case OP_KETRPOS:
      if (code != end_code)
        {
        ADD_ACTIVE(state_offset + 1 + LINK_SIZE, 0);
        if (codevalue != OP_KET)
          {
          ADD_ACTIVE(state_offset - GET(code, 1), 0);
          }
        }
      else
        {
        if (ptr > current_subject ||
            ((md->moptions & PCRE_NOTEMPTY) == 0 &&
              ((md->moptions & PCRE_NOTEMPTY_ATSTART) == 0 ||
                current_subject > start_subject + md->start_offset)))
          {
          if (match_count < 0) match_count = (offsetcount >= 2)? 1 : 0;
            else if (match_count > 0 && ++match_count * 2 > offsetcount)
              match_count = 0;
          count = ((match_count == 0)? offsetcount : match_count * 2) - 2;
          if (count > 0) memmove(offsets + 2, offsets, count * sizeof(int));
          if (offsetcount >= 2)
            {
            offsets[0] = (int)(current_subject - start_subject);
            offsets[1] = (int)(ptr - start_subject);
            DPRINTF(("%.*sSet matched string = \"%.*s\"\n", rlevel*2-2, SP,
              offsets[1] - offsets[0], current_subject));
            }
          if ((md->moptions & PCRE_DFA_SHORTEST) != 0)
            {
            DPRINTF(("%.*sEnd of internal_dfa_exec %d: returning %d\n"
              "%.*s---------------------\n\n", rlevel*2-2, SP, rlevel,
              match_count, rlevel*2-2, SP));
            return match_count;
            }
          }
        }
      break;

/* ========================================================================== */
      /* These opcodes add to the current list of states without looking
      at the current character. */

      /*-----------------------------------------------------------------*/
      case OP_ALT:
      do { code += GET(code, 1); } while (*code == OP_ALT);
      ADD_ACTIVE((int)(code - start_code), 0);
      break;

      /*-----------------------------------------------------------------*/
      case OP_BRA:
      case OP_SBRA:
      do
        {
        ADD_ACTIVE((int)(code - start_code + 1 + LINK_SIZE), 0);
        code += GET(code, 1);
        }
      while (*code == OP_ALT);
      break;

      /*-----------------------------------------------------------------*/
      case OP_CBRA:
      case OP_SCBRA:
      ADD_ACTIVE((int)(code - start_code + 1 + LINK_SIZE + IMM2_SIZE),  0);
      code += GET(code, 1);
      while (*code == OP_ALT)
        {
        ADD_ACTIVE((int)(code - start_code + 1 + LINK_SIZE),  0);
        code += GET(code, 1);
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_BRAZERO:
      case OP_BRAMINZERO:
      ADD_ACTIVE(state_offset + 1, 0);
      code += 1 + GET(code, 2);
      while (*code == OP_ALT) code += GET(code, 1);
      ADD_ACTIVE((int)(code - start_code + 1 + LINK_SIZE), 0);
      break;

      /*-----------------------------------------------------------------*/
      case OP_SKIPZERO:
      code += 1 + GET(code, 2);
      while (*code == OP_ALT) code += GET(code, 1);
      ADD_ACTIVE((int)(code - start_code + 1 + LINK_SIZE), 0);
      break;

      /*-----------------------------------------------------------------*/
      case OP_CIRC:
      if (ptr == start_subject && (md->moptions & PCRE_NOTBOL) == 0)
        { ADD_ACTIVE(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_CIRCM:
      if ((ptr == start_subject && (md->moptions & PCRE_NOTBOL) == 0) ||
          (ptr != end_subject && WAS_NEWLINE(ptr)))
        { ADD_ACTIVE(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_EOD:
      if (ptr >= end_subject)
        {
        if ((md->moptions & PCRE_PARTIAL_HARD) != 0)
          could_continue = TRUE;
        else { ADD_ACTIVE(state_offset + 1, 0); }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_SOD:
      if (ptr == start_subject) { ADD_ACTIVE(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_SOM:
      if (ptr == start_subject + start_offset) { ADD_ACTIVE(state_offset + 1, 0); }
      break;


/* ========================================================================== */
      /* These opcodes inspect the next subject character, and sometimes
      the previous one as well, but do not have an argument. The variable
      clen contains the length of the current character and is zero if we are
      at the end of the subject. */

      /*-----------------------------------------------------------------*/
      case OP_ANY:
      if (clen > 0 && !IS_NEWLINE(ptr))
        { ADD_NEW(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_ALLANY:
      if (clen > 0)
        { ADD_NEW(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_EODN:
      if (clen == 0 && (md->moptions & PCRE_PARTIAL_HARD) != 0)
        could_continue = TRUE;
      else if (clen == 0 || (IS_NEWLINE(ptr) && ptr == end_subject - md->nllen))
        { ADD_ACTIVE(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_DOLL:
      if ((md->moptions & PCRE_NOTEOL) == 0)
        {
        if (clen == 0 && (md->moptions & PCRE_PARTIAL_HARD) != 0)
          could_continue = TRUE;
        else if (clen == 0 ||
            ((md->poptions & PCRE_DOLLAR_ENDONLY) == 0 && IS_NEWLINE(ptr) &&
               (ptr == end_subject - md->nllen)
            ))
          { ADD_ACTIVE(state_offset + 1, 0); }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_DOLLM:
      if ((md->moptions & PCRE_NOTEOL) == 0)
        {
        if (clen == 0 && (md->moptions & PCRE_PARTIAL_HARD) != 0)
          could_continue = TRUE;
        else if (clen == 0 ||
            ((md->poptions & PCRE_DOLLAR_ENDONLY) == 0 && IS_NEWLINE(ptr)))
          { ADD_ACTIVE(state_offset + 1, 0); }
        }
      else if (IS_NEWLINE(ptr))
        { ADD_ACTIVE(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/

      case OP_DIGIT:
      case OP_WHITESPACE:
      case OP_WORDCHAR:
      if (clen > 0 && c < 256 &&
            ((ctypes[c] & toptable1[codevalue]) ^ toptable2[codevalue]) != 0)
        { ADD_NEW(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_NOT_DIGIT:
      case OP_NOT_WHITESPACE:
      case OP_NOT_WORDCHAR:
      if (clen > 0 && (c >= 256 ||
            ((ctypes[c] & toptable1[codevalue]) ^ toptable2[codevalue]) != 0))
        { ADD_NEW(state_offset + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_WORD_BOUNDARY:
      case OP_NOT_WORD_BOUNDARY:
        {
        int left_word, right_word;

        if (ptr > start_subject)
          {
          const pcre_uchar *temp = ptr - 1;
          if (temp < md->start_used_ptr) md->start_used_ptr = temp;
#ifdef SUPPORT_UTF
          if (utf) { BACKCHAR(temp); }
#endif
          GETCHARTEST(d, temp);
#ifdef SUPPORT_UCP
          if ((md->poptions & PCRE_UCP) != 0)
            {
            if (d == '_') left_word = TRUE; else
              {
              int cat = UCD_CATEGORY(d);
              left_word = (cat == ucp_L || cat == ucp_N);
              }
            }
          else
#endif
          left_word = d < 256 && (ctypes[d] & ctype_word) != 0;
          }
        else left_word = FALSE;

        if (clen > 0)
          {
#ifdef SUPPORT_UCP
          if ((md->poptions & PCRE_UCP) != 0)
            {
            if (c == '_') right_word = TRUE; else
              {
              int cat = UCD_CATEGORY(c);
              right_word = (cat == ucp_L || cat == ucp_N);
              }
            }
          else
#endif
          right_word = c < 256 && (ctypes[c] & ctype_word) != 0;
          }
        else right_word = FALSE;

        if ((left_word == right_word) == (codevalue == OP_NOT_WORD_BOUNDARY))
          { ADD_ACTIVE(state_offset + 1, 0); }
        }
      break;


      /*-----------------------------------------------------------------*/
      /* Check the next character by Unicode property. We will get here only
      if the support is in the binary; otherwise a compile-time error occurs.
      */

#ifdef SUPPORT_UCP
      case OP_PROP:
      case OP_NOTPROP:
      if (clen > 0)
        {
        BOOL OK;
        const ucd_record * prop = GET_UCD(c);
        switch(code[1])
          {
          case PT_ANY:
          OK = TRUE;
          break;

          case PT_LAMP:
          OK = prop->chartype == ucp_Lu || prop->chartype == ucp_Ll ||
               prop->chartype == ucp_Lt;
          break;

          case PT_GC:
          OK = PRIV(ucp_gentype)[prop->chartype] == code[2];
          break;

          case PT_PC:
          OK = prop->chartype == code[2];
          break;

          case PT_SC:
          OK = prop->script == code[2];
          break;

          /* These are specials for combination cases. */

          case PT_ALNUM:
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_L ||
               PRIV(ucp_gentype)[prop->chartype] == ucp_N;
          break;

          case PT_SPACE:    /* Perl space */
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_Z ||
               c == CHAR_HT || c == CHAR_NL || c == CHAR_FF || c == CHAR_CR;
          break;

          case PT_PXSPACE:  /* POSIX space */
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_Z ||
               c == CHAR_HT || c == CHAR_NL || c == CHAR_VT ||
               c == CHAR_FF || c == CHAR_CR;
          break;

          case PT_WORD:
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_L ||
               PRIV(ucp_gentype)[prop->chartype] == ucp_N ||
               c == CHAR_UNDERSCORE;
          break;

          /* Should never occur, but keep compilers from grumbling. */

          default:
          OK = codevalue != OP_PROP;
          break;
          }

        if (OK == (codevalue == OP_PROP)) { ADD_NEW(state_offset + 3, 0); }
        }
      break;
#endif



/* ========================================================================== */
      /* These opcodes likewise inspect the subject character, but have an
      argument that is not a data character. It is one of these opcodes:
      OP_ANY, OP_ALLANY, OP_DIGIT, OP_NOT_DIGIT, OP_WHITESPACE, OP_NOT_SPACE,
      OP_WORDCHAR, OP_NOT_WORDCHAR. The value is loaded into d. */

      case OP_TYPEPLUS:
      case OP_TYPEMINPLUS:
      case OP_TYPEPOSPLUS:
      count = current_state->count;  /* Already matched */
      if (count > 0) { ADD_ACTIVE(state_offset + 2, 0); }
      if (clen > 0)
        {
        if ((c >= 256 && d != OP_DIGIT && d != OP_WHITESPACE && d != OP_WORDCHAR) ||
            (c < 256 &&
              (d != OP_ANY || !IS_NEWLINE(ptr)) &&
              ((ctypes[c] & toptable1[d]) ^ toptable2[d]) != 0))
          {
          if (count > 0 && codevalue == OP_TYPEPOSPLUS)
            {
            active_count--;            /* Remove non-match possibility */
            next_active_state--;
            }
          count++;
          ADD_NEW(state_offset, count);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_TYPEQUERY:
      case OP_TYPEMINQUERY:
      case OP_TYPEPOSQUERY:
      ADD_ACTIVE(state_offset + 2, 0);
      if (clen > 0)
        {
        if ((c >= 256 && d != OP_DIGIT && d != OP_WHITESPACE && d != OP_WORDCHAR) ||
            (c < 256 &&
              (d != OP_ANY || !IS_NEWLINE(ptr)) &&
              ((ctypes[c] & toptable1[d]) ^ toptable2[d]) != 0))
          {
          if (codevalue == OP_TYPEPOSQUERY)
            {
            active_count--;            /* Remove non-match possibility */
            next_active_state--;
            }
          ADD_NEW(state_offset + 2, 0);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_TYPESTAR:
      case OP_TYPEMINSTAR:
      case OP_TYPEPOSSTAR:
      ADD_ACTIVE(state_offset + 2, 0);
      if (clen > 0)
        {
        if ((c >= 256 && d != OP_DIGIT && d != OP_WHITESPACE && d != OP_WORDCHAR) ||
            (c < 256 &&
              (d != OP_ANY || !IS_NEWLINE(ptr)) &&
              ((ctypes[c] & toptable1[d]) ^ toptable2[d]) != 0))
          {
          if (codevalue == OP_TYPEPOSSTAR)
            {
            active_count--;            /* Remove non-match possibility */
            next_active_state--;
            }
          ADD_NEW(state_offset, 0);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_TYPEEXACT:
      count = current_state->count;  /* Number already matched */
      if (clen > 0)
        {
        if ((c >= 256 && d != OP_DIGIT && d != OP_WHITESPACE && d != OP_WORDCHAR) ||
            (c < 256 &&
              (d != OP_ANY || !IS_NEWLINE(ptr)) &&
              ((ctypes[c] & toptable1[d]) ^ toptable2[d]) != 0))
          {
          if (++count >= GET2(code, 1))
            { ADD_NEW(state_offset + 1 + IMM2_SIZE + 1, 0); }
          else
            { ADD_NEW(state_offset, count); }
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_TYPEUPTO:
      case OP_TYPEMINUPTO:
      case OP_TYPEPOSUPTO:
      ADD_ACTIVE(state_offset + 2 + IMM2_SIZE, 0);
      count = current_state->count;  /* Number already matched */
      if (clen > 0)
        {
        if ((c >= 256 && d != OP_DIGIT && d != OP_WHITESPACE && d != OP_WORDCHAR) ||
            (c < 256 &&
              (d != OP_ANY || !IS_NEWLINE(ptr)) &&
              ((ctypes[c] & toptable1[d]) ^ toptable2[d]) != 0))
          {
          if (codevalue == OP_TYPEPOSUPTO)
            {
            active_count--;           /* Remove non-match possibility */
            next_active_state--;
            }
          if (++count >= GET2(code, 1))
            { ADD_NEW(state_offset + 2 + IMM2_SIZE, 0); }
          else
            { ADD_NEW(state_offset, count); }
          }
        }
      break;

/* ========================================================================== */
      /* These are virtual opcodes that are used when something like
      OP_TYPEPLUS has OP_PROP, OP_NOTPROP, OP_ANYNL, or OP_EXTUNI as its
      argument. It keeps the code above fast for the other cases. The argument
      is in the d variable. */

#ifdef SUPPORT_UCP
      case OP_PROP_EXTRA + OP_TYPEPLUS:
      case OP_PROP_EXTRA + OP_TYPEMINPLUS:
      case OP_PROP_EXTRA + OP_TYPEPOSPLUS:
      count = current_state->count;           /* Already matched */
      if (count > 0) { ADD_ACTIVE(state_offset + 4, 0); }
      if (clen > 0)
        {
        BOOL OK;
        const ucd_record * prop = GET_UCD(c);
        switch(code[2])
          {
          case PT_ANY:
          OK = TRUE;
          break;

          case PT_LAMP:
          OK = prop->chartype == ucp_Lu || prop->chartype == ucp_Ll ||
            prop->chartype == ucp_Lt;
          break;

          case PT_GC:
          OK = PRIV(ucp_gentype)[prop->chartype] == code[3];
          break;

          case PT_PC:
          OK = prop->chartype == code[3];
          break;

          case PT_SC:
          OK = prop->script == code[3];
          break;

          /* These are specials for combination cases. */

          case PT_ALNUM:
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_L ||
               PRIV(ucp_gentype)[prop->chartype] == ucp_N;
          break;

          case PT_SPACE:    /* Perl space */
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_Z ||
               c == CHAR_HT || c == CHAR_NL || c == CHAR_FF || c == CHAR_CR;
          break;

          case PT_PXSPACE:  /* POSIX space */
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_Z ||
               c == CHAR_HT || c == CHAR_NL || c == CHAR_VT ||
               c == CHAR_FF || c == CHAR_CR;
          break;

          case PT_WORD:
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_L ||
               PRIV(ucp_gentype)[prop->chartype] == ucp_N ||
               c == CHAR_UNDERSCORE;
          break;

          /* Should never occur, but keep compilers from grumbling. */

          default:
          OK = codevalue != OP_PROP;
          break;
          }

        if (OK == (d == OP_PROP))
          {
          if (count > 0 && codevalue == OP_PROP_EXTRA + OP_TYPEPOSPLUS)
            {
            active_count--;           /* Remove non-match possibility */
            next_active_state--;
            }
          count++;
          ADD_NEW(state_offset, count);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_EXTUNI_EXTRA + OP_TYPEPLUS:
      case OP_EXTUNI_EXTRA + OP_TYPEMINPLUS:
      case OP_EXTUNI_EXTRA + OP_TYPEPOSPLUS:
      count = current_state->count;  /* Already matched */
      if (count > 0) { ADD_ACTIVE(state_offset + 2, 0); }
      if (clen > 0 && UCD_CATEGORY(c) != ucp_M)
        {
        const pcre_uchar *nptr = ptr + clen;
        int ncount = 0;
        if (count > 0 && codevalue == OP_EXTUNI_EXTRA + OP_TYPEPOSPLUS)
          {
          active_count--;           /* Remove non-match possibility */
          next_active_state--;
          }
        while (nptr < end_subject)
          {
          int nd;
          int ndlen = 1;
          GETCHARLEN(nd, nptr, ndlen);
          if (UCD_CATEGORY(nd) != ucp_M) break;
          ncount++;
          nptr += ndlen;
          }
        count++;
        ADD_NEW_DATA(-state_offset, count, ncount);
        }
      break;
#endif

      /*-----------------------------------------------------------------*/
      case OP_ANYNL_EXTRA + OP_TYPEPLUS:
      case OP_ANYNL_EXTRA + OP_TYPEMINPLUS:
      case OP_ANYNL_EXTRA + OP_TYPEPOSPLUS:
      count = current_state->count;  /* Already matched */
      if (count > 0) { ADD_ACTIVE(state_offset + 2, 0); }
      if (clen > 0)
        {
        int ncount = 0;
        switch (c)
          {
          case 0x000b:
          case 0x000c:
          case 0x0085:
          case 0x2028:
          case 0x2029:
          if ((md->moptions & PCRE_BSR_ANYCRLF) != 0) break;
          goto ANYNL01;

          case 0x000d:
          if (ptr + 1 < end_subject && ptr[1] == 0x0a) ncount = 1;
          /* Fall through */

          ANYNL01:
          case 0x000a:
          if (count > 0 && codevalue == OP_ANYNL_EXTRA + OP_TYPEPOSPLUS)
            {
            active_count--;           /* Remove non-match possibility */
            next_active_state--;
            }
          count++;
          ADD_NEW_DATA(-state_offset, count, ncount);
          break;

          default:
          break;
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_VSPACE_EXTRA + OP_TYPEPLUS:
      case OP_VSPACE_EXTRA + OP_TYPEMINPLUS:
      case OP_VSPACE_EXTRA + OP_TYPEPOSPLUS:
      count = current_state->count;  /* Already matched */
      if (count > 0) { ADD_ACTIVE(state_offset + 2, 0); }
      if (clen > 0)
        {
        BOOL OK;
        switch (c)
          {
          case 0x000a:
          case 0x000b:
          case 0x000c:
          case 0x000d:
          case 0x0085:
          case 0x2028:
          case 0x2029:
          OK = TRUE;
          break;

          default:
          OK = FALSE;
          break;
          }

        if (OK == (d == OP_VSPACE))
          {
          if (count > 0 && codevalue == OP_VSPACE_EXTRA + OP_TYPEPOSPLUS)
            {
            active_count--;           /* Remove non-match possibility */
            next_active_state--;
            }
          count++;
          ADD_NEW_DATA(-state_offset, count, 0);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_HSPACE_EXTRA + OP_TYPEPLUS:
      case OP_HSPACE_EXTRA + OP_TYPEMINPLUS:
      case OP_HSPACE_EXTRA + OP_TYPEPOSPLUS:
      count = current_state->count;  /* Already matched */
      if (count > 0) { ADD_ACTIVE(state_offset + 2, 0); }
      if (clen > 0)
        {
        BOOL OK;
        switch (c)
          {
          case 0x09:      /* HT */
          case 0x20:      /* SPACE */
          case 0xa0:      /* NBSP */
          case 0x1680:    /* OGHAM SPACE MARK */
          case 0x180e:    /* MONGOLIAN VOWEL SEPARATOR */
          case 0x2000:    /* EN QUAD */
          case 0x2001:    /* EM QUAD */
          case 0x2002:    /* EN SPACE */
          case 0x2003:    /* EM SPACE */
          case 0x2004:    /* THREE-PER-EM SPACE */
          case 0x2005:    /* FOUR-PER-EM SPACE */
          case 0x2006:    /* SIX-PER-EM SPACE */
          case 0x2007:    /* FIGURE SPACE */
          case 0x2008:    /* PUNCTUATION SPACE */
          case 0x2009:    /* THIN SPACE */
          case 0x200A:    /* HAIR SPACE */
          case 0x202f:    /* NARROW NO-BREAK SPACE */
          case 0x205f:    /* MEDIUM MATHEMATICAL SPACE */
          case 0x3000:    /* IDEOGRAPHIC SPACE */
          OK = TRUE;
          break;

          default:
          OK = FALSE;
          break;
          }

        if (OK == (d == OP_HSPACE))
          {
          if (count > 0 && codevalue == OP_HSPACE_EXTRA + OP_TYPEPOSPLUS)
            {
            active_count--;           /* Remove non-match possibility */
            next_active_state--;
            }
          count++;
          ADD_NEW_DATA(-state_offset, count, 0);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
#ifdef SUPPORT_UCP
      case OP_PROP_EXTRA + OP_TYPEQUERY:
      case OP_PROP_EXTRA + OP_TYPEMINQUERY:
      case OP_PROP_EXTRA + OP_TYPEPOSQUERY:
      count = 4;
      goto QS1;

      case OP_PROP_EXTRA + OP_TYPESTAR:
      case OP_PROP_EXTRA + OP_TYPEMINSTAR:
      case OP_PROP_EXTRA + OP_TYPEPOSSTAR:
      count = 0;

      QS1:

      ADD_ACTIVE(state_offset + 4, 0);
      if (clen > 0)
        {
        BOOL OK;
        const ucd_record * prop = GET_UCD(c);
        switch(code[2])
          {
          case PT_ANY:
          OK = TRUE;
          break;

          case PT_LAMP:
          OK = prop->chartype == ucp_Lu || prop->chartype == ucp_Ll ||
            prop->chartype == ucp_Lt;
          break;

          case PT_GC:
          OK = PRIV(ucp_gentype)[prop->chartype] == code[3];
          break;

          case PT_PC:
          OK = prop->chartype == code[3];
          break;

          case PT_SC:
          OK = prop->script == code[3];
          break;

          /* These are specials for combination cases. */

          case PT_ALNUM:
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_L ||
               PRIV(ucp_gentype)[prop->chartype] == ucp_N;
          break;

          case PT_SPACE:    /* Perl space */
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_Z ||
               c == CHAR_HT || c == CHAR_NL || c == CHAR_FF || c == CHAR_CR;
          break;

          case PT_PXSPACE:  /* POSIX space */
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_Z ||
               c == CHAR_HT || c == CHAR_NL || c == CHAR_VT ||
               c == CHAR_FF || c == CHAR_CR;
          break;

          case PT_WORD:
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_L ||
               PRIV(ucp_gentype)[prop->chartype] == ucp_N ||
               c == CHAR_UNDERSCORE;
          break;

          /* Should never occur, but keep compilers from grumbling. */

          default:
          OK = codevalue != OP_PROP;
          break;
          }

        if (OK == (d == OP_PROP))
          {
          if (codevalue == OP_PROP_EXTRA + OP_TYPEPOSSTAR ||
              codevalue == OP_PROP_EXTRA + OP_TYPEPOSQUERY)
            {
            active_count--;           /* Remove non-match possibility */
            next_active_state--;
            }
          ADD_NEW(state_offset + count, 0);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_EXTUNI_EXTRA + OP_TYPEQUERY:
      case OP_EXTUNI_EXTRA + OP_TYPEMINQUERY:
      case OP_EXTUNI_EXTRA + OP_TYPEPOSQUERY:
      count = 2;
      goto QS2;

      case OP_EXTUNI_EXTRA + OP_TYPESTAR:
      case OP_EXTUNI_EXTRA + OP_TYPEMINSTAR:
      case OP_EXTUNI_EXTRA + OP_TYPEPOSSTAR:
      count = 0;

      QS2:

      ADD_ACTIVE(state_offset + 2, 0);
      if (clen > 0 && UCD_CATEGORY(c) != ucp_M)
        {
        const pcre_uchar *nptr = ptr + clen;
        int ncount = 0;
        if (codevalue == OP_EXTUNI_EXTRA + OP_TYPEPOSSTAR ||
            codevalue == OP_EXTUNI_EXTRA + OP_TYPEPOSQUERY)
          {
          active_count--;           /* Remove non-match possibility */
          next_active_state--;
          }
        while (nptr < end_subject)
          {
          int nd;
          int ndlen = 1;
          GETCHARLEN(nd, nptr, ndlen);
          if (UCD_CATEGORY(nd) != ucp_M) break;
          ncount++;
          nptr += ndlen;
          }
        ADD_NEW_DATA(-(state_offset + count), 0, ncount);
        }
      break;
#endif

      /*-----------------------------------------------------------------*/
      case OP_ANYNL_EXTRA + OP_TYPEQUERY:
      case OP_ANYNL_EXTRA + OP_TYPEMINQUERY:
      case OP_ANYNL_EXTRA + OP_TYPEPOSQUERY:
      count = 2;
      goto QS3;

      case OP_ANYNL_EXTRA + OP_TYPESTAR:
      case OP_ANYNL_EXTRA + OP_TYPEMINSTAR:
      case OP_ANYNL_EXTRA + OP_TYPEPOSSTAR:
      count = 0;

      QS3:
      ADD_ACTIVE(state_offset + 2, 0);
      if (clen > 0)
        {
        int ncount = 0;
        switch (c)
          {
          case 0x000b:
          case 0x000c:
          case 0x0085:
          case 0x2028:
          case 0x2029:
          if ((md->moptions & PCRE_BSR_ANYCRLF) != 0) break;
          goto ANYNL02;

          case 0x000d:
          if (ptr + 1 < end_subject && ptr[1] == 0x0a) ncount = 1;
          /* Fall through */

          ANYNL02:
          case 0x000a:
          if (codevalue == OP_ANYNL_EXTRA + OP_TYPEPOSSTAR ||
              codevalue == OP_ANYNL_EXTRA + OP_TYPEPOSQUERY)
            {
            active_count--;           /* Remove non-match possibility */
            next_active_state--;
            }
          ADD_NEW_DATA(-(state_offset + count), 0, ncount);
          break;

          default:
          break;
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_VSPACE_EXTRA + OP_TYPEQUERY:
      case OP_VSPACE_EXTRA + OP_TYPEMINQUERY:
      case OP_VSPACE_EXTRA + OP_TYPEPOSQUERY:
      count = 2;
      goto QS4;

      case OP_VSPACE_EXTRA + OP_TYPESTAR:
      case OP_VSPACE_EXTRA + OP_TYPEMINSTAR:
      case OP_VSPACE_EXTRA + OP_TYPEPOSSTAR:
      count = 0;

      QS4:
      ADD_ACTIVE(state_offset + 2, 0);
      if (clen > 0)
        {
        BOOL OK;
        switch (c)
          {
          case 0x000a:
          case 0x000b:
          case 0x000c:
          case 0x000d:
          case 0x0085:
          case 0x2028:
          case 0x2029:
          OK = TRUE;
          break;

          default:
          OK = FALSE;
          break;
          }
        if (OK == (d == OP_VSPACE))
          {
          if (codevalue == OP_VSPACE_EXTRA + OP_TYPEPOSSTAR ||
              codevalue == OP_VSPACE_EXTRA + OP_TYPEPOSQUERY)
            {
            active_count--;           /* Remove non-match possibility */
            next_active_state--;
            }
          ADD_NEW_DATA(-(state_offset + count), 0, 0);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_HSPACE_EXTRA + OP_TYPEQUERY:
      case OP_HSPACE_EXTRA + OP_TYPEMINQUERY:
      case OP_HSPACE_EXTRA + OP_TYPEPOSQUERY:
      count = 2;
      goto QS5;

      case OP_HSPACE_EXTRA + OP_TYPESTAR:
      case OP_HSPACE_EXTRA + OP_TYPEMINSTAR:
      case OP_HSPACE_EXTRA + OP_TYPEPOSSTAR:
      count = 0;

      QS5:
      ADD_ACTIVE(state_offset + 2, 0);
      if (clen > 0)
        {
        BOOL OK;
        switch (c)
          {
          case 0x09:      /* HT */
          case 0x20:      /* SPACE */
          case 0xa0:      /* NBSP */
          case 0x1680:    /* OGHAM SPACE MARK */
          case 0x180e:    /* MONGOLIAN VOWEL SEPARATOR */
          case 0x2000:    /* EN QUAD */
          case 0x2001:    /* EM QUAD */
          case 0x2002:    /* EN SPACE */
          case 0x2003:    /* EM SPACE */
          case 0x2004:    /* THREE-PER-EM SPACE */
          case 0x2005:    /* FOUR-PER-EM SPACE */
          case 0x2006:    /* SIX-PER-EM SPACE */
          case 0x2007:    /* FIGURE SPACE */
          case 0x2008:    /* PUNCTUATION SPACE */
          case 0x2009:    /* THIN SPACE */
          case 0x200A:    /* HAIR SPACE */
          case 0x202f:    /* NARROW NO-BREAK SPACE */
          case 0x205f:    /* MEDIUM MATHEMATICAL SPACE */
          case 0x3000:    /* IDEOGRAPHIC SPACE */
          OK = TRUE;
          break;

          default:
          OK = FALSE;
          break;
          }

        if (OK == (d == OP_HSPACE))
          {
          if (codevalue == OP_HSPACE_EXTRA + OP_TYPEPOSSTAR ||
              codevalue == OP_HSPACE_EXTRA + OP_TYPEPOSQUERY)
            {
            active_count--;           /* Remove non-match possibility */
            next_active_state--;
            }
          ADD_NEW_DATA(-(state_offset + count), 0, 0);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
#ifdef SUPPORT_UCP
      case OP_PROP_EXTRA + OP_TYPEEXACT:
      case OP_PROP_EXTRA + OP_TYPEUPTO:
      case OP_PROP_EXTRA + OP_TYPEMINUPTO:
      case OP_PROP_EXTRA + OP_TYPEPOSUPTO:
      if (codevalue != OP_PROP_EXTRA + OP_TYPEEXACT)
        { ADD_ACTIVE(state_offset + 1 + IMM2_SIZE + 3, 0); }
      count = current_state->count;  /* Number already matched */
      if (clen > 0)
        {
        BOOL OK;
        const ucd_record * prop = GET_UCD(c);
        switch(code[1 + IMM2_SIZE + 1])
          {
          case PT_ANY:
          OK = TRUE;
          break;

          case PT_LAMP:
          OK = prop->chartype == ucp_Lu || prop->chartype == ucp_Ll ||
            prop->chartype == ucp_Lt;
          break;

          case PT_GC:
          OK = PRIV(ucp_gentype)[prop->chartype] == code[1 + IMM2_SIZE + 2];
          break;

          case PT_PC:
          OK = prop->chartype == code[1 + IMM2_SIZE + 2];
          break;

          case PT_SC:
          OK = prop->script == code[1 + IMM2_SIZE + 2];
          break;

          /* These are specials for combination cases. */

          case PT_ALNUM:
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_L ||
               PRIV(ucp_gentype)[prop->chartype] == ucp_N;
          break;

          case PT_SPACE:    /* Perl space */
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_Z ||
               c == CHAR_HT || c == CHAR_NL || c == CHAR_FF || c == CHAR_CR;
          break;

          case PT_PXSPACE:  /* POSIX space */
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_Z ||
               c == CHAR_HT || c == CHAR_NL || c == CHAR_VT ||
               c == CHAR_FF || c == CHAR_CR;
          break;

          case PT_WORD:
          OK = PRIV(ucp_gentype)[prop->chartype] == ucp_L ||
               PRIV(ucp_gentype)[prop->chartype] == ucp_N ||
               c == CHAR_UNDERSCORE;
          break;

          /* Should never occur, but keep compilers from grumbling. */

          default:
          OK = codevalue != OP_PROP;
          break;
          }

        if (OK == (d == OP_PROP))
          {
          if (codevalue == OP_PROP_EXTRA + OP_TYPEPOSUPTO)
            {
            active_count--;           /* Remove non-match possibility */
            next_active_state--;
            }
          if (++count >= GET2(code, 1))
            { ADD_NEW(state_offset + 1 + IMM2_SIZE + 3, 0); }
          else
            { ADD_NEW(state_offset, count); }
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_EXTUNI_EXTRA + OP_TYPEEXACT:
      case OP_EXTUNI_EXTRA + OP_TYPEUPTO:
      case OP_EXTUNI_EXTRA + OP_TYPEMINUPTO:
      case OP_EXTUNI_EXTRA + OP_TYPEPOSUPTO:
      if (codevalue != OP_EXTUNI_EXTRA + OP_TYPEEXACT)
        { ADD_ACTIVE(state_offset + 2 + IMM2_SIZE, 0); }
      count = current_state->count;  /* Number already matched */
      if (clen > 0 && UCD_CATEGORY(c) != ucp_M)
        {
        const pcre_uchar *nptr = ptr + clen;
        int ncount = 0;
        if (codevalue == OP_EXTUNI_EXTRA + OP_TYPEPOSUPTO)
          {
          active_count--;           /* Remove non-match possibility */
          next_active_state--;
          }
        while (nptr < end_subject)
          {
          int nd;
          int ndlen = 1;
          GETCHARLEN(nd, nptr, ndlen);
          if (UCD_CATEGORY(nd) != ucp_M) break;
          ncount++;
          nptr += ndlen;
          }
        if (++count >= GET2(code, 1))
          { ADD_NEW_DATA(-(state_offset + 2 + IMM2_SIZE), 0, ncount); }
        else
          { ADD_NEW_DATA(-state_offset, count, ncount); }
        }
      break;
#endif

      /*-----------------------------------------------------------------*/
      case OP_ANYNL_EXTRA + OP_TYPEEXACT:
      case OP_ANYNL_EXTRA + OP_TYPEUPTO:
      case OP_ANYNL_EXTRA + OP_TYPEMINUPTO:
      case OP_ANYNL_EXTRA + OP_TYPEPOSUPTO:
      if (codevalue != OP_ANYNL_EXTRA + OP_TYPEEXACT)
        { ADD_ACTIVE(state_offset + 2 + IMM2_SIZE, 0); }
      count = current_state->count;  /* Number already matched */
      if (clen > 0)
        {
        int ncount = 0;
        switch (c)
          {
          case 0x000b:
          case 0x000c:
          case 0x0085:
          case 0x2028:
          case 0x2029:
          if ((md->moptions & PCRE_BSR_ANYCRLF) != 0) break;
          goto ANYNL03;

          case 0x000d:
          if (ptr + 1 < end_subject && ptr[1] == 0x0a) ncount = 1;
          /* Fall through */

          ANYNL03:
          case 0x000a:
          if (codevalue == OP_ANYNL_EXTRA + OP_TYPEPOSUPTO)
            {
            active_count--;           /* Remove non-match possibility */
            next_active_state--;
            }
          if (++count >= GET2(code, 1))
            { ADD_NEW_DATA(-(state_offset + 2 + IMM2_SIZE), 0, ncount); }
          else
            { ADD_NEW_DATA(-state_offset, count, ncount); }
          break;

          default:
          break;
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_VSPACE_EXTRA + OP_TYPEEXACT:
      case OP_VSPACE_EXTRA + OP_TYPEUPTO:
      case OP_VSPACE_EXTRA + OP_TYPEMINUPTO:
      case OP_VSPACE_EXTRA + OP_TYPEPOSUPTO:
      if (codevalue != OP_VSPACE_EXTRA + OP_TYPEEXACT)
        { ADD_ACTIVE(state_offset + 2 + IMM2_SIZE, 0); }
      count = current_state->count;  /* Number already matched */
      if (clen > 0)
        {
        BOOL OK;
        switch (c)
          {
          case 0x000a:
          case 0x000b:
          case 0x000c:
          case 0x000d:
          case 0x0085:
          case 0x2028:
          case 0x2029:
          OK = TRUE;
          break;

          default:
          OK = FALSE;
          }

        if (OK == (d == OP_VSPACE))
          {
          if (codevalue == OP_VSPACE_EXTRA + OP_TYPEPOSUPTO)
            {
            active_count--;           /* Remove non-match possibility */
            next_active_state--;
            }
          if (++count >= GET2(code, 1))
            { ADD_NEW_DATA(-(state_offset + 2 + IMM2_SIZE), 0, 0); }
          else
            { ADD_NEW_DATA(-state_offset, count, 0); }
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_HSPACE_EXTRA + OP_TYPEEXACT:
      case OP_HSPACE_EXTRA + OP_TYPEUPTO:
      case OP_HSPACE_EXTRA + OP_TYPEMINUPTO:
      case OP_HSPACE_EXTRA + OP_TYPEPOSUPTO:
      if (codevalue != OP_HSPACE_EXTRA + OP_TYPEEXACT)
        { ADD_ACTIVE(state_offset + 2 + IMM2_SIZE, 0); }
      count = current_state->count;  /* Number already matched */
      if (clen > 0)
        {
        BOOL OK;
        switch (c)
          {
          case 0x09:      /* HT */
          case 0x20:      /* SPACE */
          case 0xa0:      /* NBSP */
          case 0x1680:    /* OGHAM SPACE MARK */
          case 0x180e:    /* MONGOLIAN VOWEL SEPARATOR */
          case 0x2000:    /* EN QUAD */
          case 0x2001:    /* EM QUAD */
          case 0x2002:    /* EN SPACE */
          case 0x2003:    /* EM SPACE */
          case 0x2004:    /* THREE-PER-EM SPACE */
          case 0x2005:    /* FOUR-PER-EM SPACE */
          case 0x2006:    /* SIX-PER-EM SPACE */
          case 0x2007:    /* FIGURE SPACE */
          case 0x2008:    /* PUNCTUATION SPACE */
          case 0x2009:    /* THIN SPACE */
          case 0x200A:    /* HAIR SPACE */
          case 0x202f:    /* NARROW NO-BREAK SPACE */
          case 0x205f:    /* MEDIUM MATHEMATICAL SPACE */
          case 0x3000:    /* IDEOGRAPHIC SPACE */
          OK = TRUE;
          break;

          default:
          OK = FALSE;
          break;
          }

        if (OK == (d == OP_HSPACE))
          {
          if (codevalue == OP_HSPACE_EXTRA + OP_TYPEPOSUPTO)
            {
            active_count--;           /* Remove non-match possibility */
            next_active_state--;
            }
          if (++count >= GET2(code, 1))
            { ADD_NEW_DATA(-(state_offset + 2 + IMM2_SIZE), 0, 0); }
          else
            { ADD_NEW_DATA(-state_offset, count, 0); }
          }
        }
      break;

/* ========================================================================== */
      /* These opcodes are followed by a character that is usually compared
      to the current subject character; it is loaded into d. We still get
      here even if there is no subject character, because in some cases zero
      repetitions are permitted. */

      /*-----------------------------------------------------------------*/
      case OP_CHAR:
      if (clen > 0 && c == d) { ADD_NEW(state_offset + dlen + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_CHARI:
      if (clen == 0) break;

#ifdef SUPPORT_UTF
      if (utf)
        {
        if (c == d) { ADD_NEW(state_offset + dlen + 1, 0); } else
          {
          unsigned int othercase;
          if (c < 128)
            othercase = fcc[c];
          else
            /* If we have Unicode property support, we can use it to test the
            other case of the character. */
#ifdef SUPPORT_UCP
            othercase = UCD_OTHERCASE(c);
#else
            othercase = NOTACHAR;
#endif

          if (d == othercase) { ADD_NEW(state_offset + dlen + 1, 0); }
          }
        }
      else
#endif  /* SUPPORT_UTF */
      /* Not UTF mode */
        {
        if (TABLE_GET(c, lcc, c) == TABLE_GET(d, lcc, d))
          { ADD_NEW(state_offset + 2, 0); }
        }
      break;


#ifdef SUPPORT_UCP
      /*-----------------------------------------------------------------*/
      /* This is a tricky one because it can match more than one character.
      Find out how many characters to skip, and then set up a negative state
      to wait for them to pass before continuing. */

      case OP_EXTUNI:
      if (clen > 0 && UCD_CATEGORY(c) != ucp_M)
        {
        const pcre_uchar *nptr = ptr + clen;
        int ncount = 0;
        while (nptr < end_subject)
          {
          int nclen = 1;
          GETCHARLEN(c, nptr, nclen);
          if (UCD_CATEGORY(c) != ucp_M) break;
          ncount++;
          nptr += nclen;
          }
        ADD_NEW_DATA(-(state_offset + 1), 0, ncount);
        }
      break;
#endif

      /*-----------------------------------------------------------------*/
      /* This is a tricky like EXTUNI because it too can match more than one
      character (when CR is followed by LF). In this case, set up a negative
      state to wait for one character to pass before continuing. */

      case OP_ANYNL:
      if (clen > 0) switch(c)
        {
        case 0x000b:
        case 0x000c:
        case 0x0085:
        case 0x2028:
        case 0x2029:
        if ((md->moptions & PCRE_BSR_ANYCRLF) != 0) break;

        case 0x000a:
        ADD_NEW(state_offset + 1, 0);
        break;

        case 0x000d:
        if (ptr + 1 < end_subject && ptr[1] == 0x0a)
          {
          ADD_NEW_DATA(-(state_offset + 1), 0, 1);
          }
        else
          {
          ADD_NEW(state_offset + 1, 0);
          }
        break;
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_NOT_VSPACE:
      if (clen > 0) switch(c)
        {
        case 0x000a:
        case 0x000b:
        case 0x000c:
        case 0x000d:
        case 0x0085:
        case 0x2028:
        case 0x2029:
        break;

        default:
        ADD_NEW(state_offset + 1, 0);
        break;
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_VSPACE:
      if (clen > 0) switch(c)
        {
        case 0x000a:
        case 0x000b:
        case 0x000c:
        case 0x000d:
        case 0x0085:
        case 0x2028:
        case 0x2029:
        ADD_NEW(state_offset + 1, 0);
        break;

        default: break;
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_NOT_HSPACE:
      if (clen > 0) switch(c)
        {
        case 0x09:      /* HT */
        case 0x20:      /* SPACE */
        case 0xa0:      /* NBSP */
        case 0x1680:    /* OGHAM SPACE MARK */
        case 0x180e:    /* MONGOLIAN VOWEL SEPARATOR */
        case 0x2000:    /* EN QUAD */
        case 0x2001:    /* EM QUAD */
        case 0x2002:    /* EN SPACE */
        case 0x2003:    /* EM SPACE */
        case 0x2004:    /* THREE-PER-EM SPACE */
        case 0x2005:    /* FOUR-PER-EM SPACE */
        case 0x2006:    /* SIX-PER-EM SPACE */
        case 0x2007:    /* FIGURE SPACE */
        case 0x2008:    /* PUNCTUATION SPACE */
        case 0x2009:    /* THIN SPACE */
        case 0x200A:    /* HAIR SPACE */
        case 0x202f:    /* NARROW NO-BREAK SPACE */
        case 0x205f:    /* MEDIUM MATHEMATICAL SPACE */
        case 0x3000:    /* IDEOGRAPHIC SPACE */
        break;

        default:
        ADD_NEW(state_offset + 1, 0);
        break;
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_HSPACE:
      if (clen > 0) switch(c)
        {
        case 0x09:      /* HT */
        case 0x20:      /* SPACE */
        case 0xa0:      /* NBSP */
        case 0x1680:    /* OGHAM SPACE MARK */
        case 0x180e:    /* MONGOLIAN VOWEL SEPARATOR */
        case 0x2000:    /* EN QUAD */
        case 0x2001:    /* EM QUAD */
        case 0x2002:    /* EN SPACE */
        case 0x2003:    /* EM SPACE */
        case 0x2004:    /* THREE-PER-EM SPACE */
        case 0x2005:    /* FOUR-PER-EM SPACE */
        case 0x2006:    /* SIX-PER-EM SPACE */
        case 0x2007:    /* FIGURE SPACE */
        case 0x2008:    /* PUNCTUATION SPACE */
        case 0x2009:    /* THIN SPACE */
        case 0x200A:    /* HAIR SPACE */
        case 0x202f:    /* NARROW NO-BREAK SPACE */
        case 0x205f:    /* MEDIUM MATHEMATICAL SPACE */
        case 0x3000:    /* IDEOGRAPHIC SPACE */
        ADD_NEW(state_offset + 1, 0);
        break;
        }
      break;

      /*-----------------------------------------------------------------*/
      /* Match a negated single character casefully. This is only used for
      one-byte characters, that is, we know that d < 256. The character we are
      checking (c) can be multibyte. */

      case OP_NOT:
      if (clen > 0 && c != d) { ADD_NEW(state_offset + dlen + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      /* Match a negated single character caselessly. This is only used for
      one-byte characters, that is, we know that d < 256. The character we are
      checking (c) can be multibyte. */

      case OP_NOTI:
      if (clen > 0 && c != d && c != fcc[d])
        { ADD_NEW(state_offset + dlen + 1, 0); }
      break;

      /*-----------------------------------------------------------------*/
      case OP_PLUSI:
      case OP_MINPLUSI:
      case OP_POSPLUSI:
      case OP_NOTPLUSI:
      case OP_NOTMINPLUSI:
      case OP_NOTPOSPLUSI:
      caseless = TRUE;
      codevalue -= OP_STARI - OP_STAR;

      /* Fall through */
      case OP_PLUS:
      case OP_MINPLUS:
      case OP_POSPLUS:
      case OP_NOTPLUS:
      case OP_NOTMINPLUS:
      case OP_NOTPOSPLUS:
      count = current_state->count;  /* Already matched */
      if (count > 0) { ADD_ACTIVE(state_offset + dlen + 1, 0); }
      if (clen > 0)
        {
        unsigned int otherd = NOTACHAR;
        if (caseless)
          {
#ifdef SUPPORT_UTF
          if (utf && d >= 128)
            {
#ifdef SUPPORT_UCP
            otherd = UCD_OTHERCASE(d);
#endif  /* SUPPORT_UCP */
            }
          else
#endif  /* SUPPORT_UTF */
          otherd = TABLE_GET(d, fcc, d);
          }
        if ((c == d || c == otherd) == (codevalue < OP_NOTSTAR))
          {
          if (count > 0 &&
              (codevalue == OP_POSPLUS || codevalue == OP_NOTPOSPLUS))
            {
            active_count--;             /* Remove non-match possibility */
            next_active_state--;
            }
          count++;
          ADD_NEW(state_offset, count);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_QUERYI:
      case OP_MINQUERYI:
      case OP_POSQUERYI:
      case OP_NOTQUERYI:
      case OP_NOTMINQUERYI:
      case OP_NOTPOSQUERYI:
      caseless = TRUE;
      codevalue -= OP_STARI - OP_STAR;
      /* Fall through */
      case OP_QUERY:
      case OP_MINQUERY:
      case OP_POSQUERY:
      case OP_NOTQUERY:
      case OP_NOTMINQUERY:
      case OP_NOTPOSQUERY:
      ADD_ACTIVE(state_offset + dlen + 1, 0);
      if (clen > 0)
        {
        unsigned int otherd = NOTACHAR;
        if (caseless)
          {
#ifdef SUPPORT_UTF
          if (utf && d >= 128)
            {
#ifdef SUPPORT_UCP
            otherd = UCD_OTHERCASE(d);
#endif  /* SUPPORT_UCP */
            }
          else
#endif  /* SUPPORT_UTF */
          otherd = TABLE_GET(d, fcc, d);
          }
        if ((c == d || c == otherd) == (codevalue < OP_NOTSTAR))
          {
          if (codevalue == OP_POSQUERY || codevalue == OP_NOTPOSQUERY)
            {
            active_count--;            /* Remove non-match possibility */
            next_active_state--;
            }
          ADD_NEW(state_offset + dlen + 1, 0);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_STARI:
      case OP_MINSTARI:
      case OP_POSSTARI:
      case OP_NOTSTARI:
      case OP_NOTMINSTARI:
      case OP_NOTPOSSTARI:
      caseless = TRUE;
      codevalue -= OP_STARI - OP_STAR;
      /* Fall through */
      case OP_STAR:
      case OP_MINSTAR:
      case OP_POSSTAR:
      case OP_NOTSTAR:
      case OP_NOTMINSTAR:
      case OP_NOTPOSSTAR:
      ADD_ACTIVE(state_offset + dlen + 1, 0);
      if (clen > 0)
        {
        unsigned int otherd = NOTACHAR;
        if (caseless)
          {
#ifdef SUPPORT_UTF
          if (utf && d >= 128)
            {
#ifdef SUPPORT_UCP
            otherd = UCD_OTHERCASE(d);
#endif  /* SUPPORT_UCP */
            }
          else
#endif  /* SUPPORT_UTF */
          otherd = TABLE_GET(d, fcc, d);
          }
        if ((c == d || c == otherd) == (codevalue < OP_NOTSTAR))
          {
          if (codevalue == OP_POSSTAR || codevalue == OP_NOTPOSSTAR)
            {
            active_count--;            /* Remove non-match possibility */
            next_active_state--;
            }
          ADD_NEW(state_offset, 0);
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_EXACTI:
      case OP_NOTEXACTI:
      caseless = TRUE;
      codevalue -= OP_STARI - OP_STAR;
      /* Fall through */
      case OP_EXACT:
      case OP_NOTEXACT:
      count = current_state->count;  /* Number already matched */
      if (clen > 0)
        {
        unsigned int otherd = NOTACHAR;
        if (caseless)
          {
#ifdef SUPPORT_UTF
          if (utf && d >= 128)
            {
#ifdef SUPPORT_UCP
            otherd = UCD_OTHERCASE(d);
#endif  /* SUPPORT_UCP */
            }
          else
#endif  /* SUPPORT_UTF */
          otherd = TABLE_GET(d, fcc, d);
          }
        if ((c == d || c == otherd) == (codevalue < OP_NOTSTAR))
          {
          if (++count >= GET2(code, 1))
            { ADD_NEW(state_offset + dlen + 1 + IMM2_SIZE, 0); }
          else
            { ADD_NEW(state_offset, count); }
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_UPTOI:
      case OP_MINUPTOI:
      case OP_POSUPTOI:
      case OP_NOTUPTOI:
      case OP_NOTMINUPTOI:
      case OP_NOTPOSUPTOI:
      caseless = TRUE;
      codevalue -= OP_STARI - OP_STAR;
      /* Fall through */
      case OP_UPTO:
      case OP_MINUPTO:
      case OP_POSUPTO:
      case OP_NOTUPTO:
      case OP_NOTMINUPTO:
      case OP_NOTPOSUPTO:
      ADD_ACTIVE(state_offset + dlen + 1 + IMM2_SIZE, 0);
      count = current_state->count;  /* Number already matched */
      if (clen > 0)
        {
        unsigned int otherd = NOTACHAR;
        if (caseless)
          {
#ifdef SUPPORT_UTF
          if (utf && d >= 128)
            {
#ifdef SUPPORT_UCP
            otherd = UCD_OTHERCASE(d);
#endif  /* SUPPORT_UCP */
            }
          else
#endif  /* SUPPORT_UTF */
          otherd = TABLE_GET(d, fcc, d);
          }
        if ((c == d || c == otherd) == (codevalue < OP_NOTSTAR))
          {
          if (codevalue == OP_POSUPTO || codevalue == OP_NOTPOSUPTO)
            {
            active_count--;             /* Remove non-match possibility */
            next_active_state--;
            }
          if (++count >= GET2(code, 1))
            { ADD_NEW(state_offset + dlen + 1 + IMM2_SIZE, 0); }
          else
            { ADD_NEW(state_offset, count); }
          }
        }
      break;


/* ========================================================================== */
      /* These are the class-handling opcodes */

      case OP_CLASS:
      case OP_NCLASS:
      case OP_XCLASS:
        {
        BOOL isinclass = FALSE;
        int next_state_offset;
        const pcre_uchar *ecode;

        /* For a simple class, there is always just a 32-byte table, and we
        can set isinclass from it. */

        if (codevalue != OP_XCLASS)
          {
          ecode = code + 1 + (32 / sizeof(pcre_uchar));
          if (clen > 0)
            {
            isinclass = (c > 255)? (codevalue == OP_NCLASS) :
              ((((pcre_uint8 *)(code + 1))[c/8] & (1 << (c&7))) != 0);
            }
          }

        /* An extended class may have a table or a list of single characters,
        ranges, or both, and it may be positive or negative. There's a
        function that sorts all this out. */

        else
         {
         ecode = code + GET(code, 1);
         if (clen > 0) isinclass = PRIV(xclass)(c, code + 1 + LINK_SIZE, utf);
         }

        /* At this point, isinclass is set for all kinds of class, and ecode
        points to the byte after the end of the class. If there is a
        quantifier, this is where it will be. */

        next_state_offset = (int)(ecode - start_code);

        switch (*ecode)
          {
          case OP_CRSTAR:
          case OP_CRMINSTAR:
          ADD_ACTIVE(next_state_offset + 1, 0);
          if (isinclass) { ADD_NEW(state_offset, 0); }
          break;

          case OP_CRPLUS:
          case OP_CRMINPLUS:
          count = current_state->count;  /* Already matched */
          if (count > 0) { ADD_ACTIVE(next_state_offset + 1, 0); }
          if (isinclass) { count++; ADD_NEW(state_offset, count); }
          break;

          case OP_CRQUERY:
          case OP_CRMINQUERY:
          ADD_ACTIVE(next_state_offset + 1, 0);
          if (isinclass) { ADD_NEW(next_state_offset + 1, 0); }
          break;

          case OP_CRRANGE:
          case OP_CRMINRANGE:
          count = current_state->count;  /* Already matched */
          if (count >= GET2(ecode, 1))
            { ADD_ACTIVE(next_state_offset + 1 + 2 * IMM2_SIZE, 0); }
          if (isinclass)
            {
            int max = GET2(ecode, 1 + IMM2_SIZE);
            if (++count >= max && max != 0)   /* Max 0 => no limit */
              { ADD_NEW(next_state_offset + 1 + 2 * IMM2_SIZE, 0); }
            else
              { ADD_NEW(state_offset, count); }
            }
          break;

          default:
          if (isinclass) { ADD_NEW(next_state_offset, 0); }
          break;
          }
        }
      break;

/* ========================================================================== */
      /* These are the opcodes for fancy brackets of various kinds. We have
      to use recursion in order to handle them. The "always failing" assertion
      (?!) is optimised to OP_FAIL when compiling, so we have to support that,
      though the other "backtracking verbs" are not supported. */

      case OP_FAIL:
      forced_fail++;    /* Count FAILs for multiple states */
      break;

      case OP_ASSERT:
      case OP_ASSERT_NOT:
      case OP_ASSERTBACK:
      case OP_ASSERTBACK_NOT:
        {
        int rc;
        int local_offsets[2];
        int local_workspace[1000];
        const pcre_uchar *endasscode = code + GET(code, 1);

        while (*endasscode == OP_ALT) endasscode += GET(endasscode, 1);

        rc = internal_dfa_exec(
          md,                                   /* static match data */
          code,                                 /* this subexpression's code */
          ptr,                                  /* where we currently are */
          (int)(ptr - start_subject),           /* start offset */
          local_offsets,                        /* offset vector */
          sizeof(local_offsets)/sizeof(int),    /* size of same */
          local_workspace,                      /* workspace vector */
          sizeof(local_workspace)/sizeof(int),  /* size of same */
          rlevel);                              /* function recursion level */

        if (rc == PCRE_ERROR_DFA_UITEM) return rc;
        if ((rc >= 0) == (codevalue == OP_ASSERT || codevalue == OP_ASSERTBACK))
            { ADD_ACTIVE((int)(endasscode + LINK_SIZE + 1 - start_code), 0); }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_COND:
      case OP_SCOND:
        {
        int local_offsets[1000];
        int local_workspace[1000];
        int codelink = GET(code, 1);
        int condcode;

        /* Because of the way auto-callout works during compile, a callout item
        is inserted between OP_COND and an assertion condition. This does not
        happen for the other conditions. */

        if (code[LINK_SIZE+1] == OP_CALLOUT)
          {
          rrc = 0;
          if (PUBL(callout) != NULL)
            {
            PUBL(callout_block) cb;
            cb.version          = 1;   /* Version 1 of the callout block */
            cb.callout_number   = code[LINK_SIZE+2];
            cb.offset_vector    = offsets;
#ifdef COMPILE_PCRE8
            cb.subject          = (PCRE_SPTR)start_subject;
#else
            cb.subject          = (PCRE_SPTR16)start_subject;
#endif
            cb.subject_length   = (int)(end_subject - start_subject);
            cb.start_match      = (int)(current_subject - start_subject);
            cb.current_position = (int)(ptr - start_subject);
            cb.pattern_position = GET(code, LINK_SIZE + 3);
            cb.next_item_length = GET(code, 3 + 2*LINK_SIZE);
            cb.capture_top      = 1;
            cb.capture_last     = -1;
            cb.callout_data     = md->callout_data;
            cb.mark             = NULL;   /* No (*MARK) support */
            if ((rrc = (*PUBL(callout))(&cb)) < 0) return rrc;   /* Abandon */
            }
          if (rrc > 0) break;                      /* Fail this thread */
          code += PRIV(OP_lengths)[OP_CALLOUT];    /* Skip callout data */
          }

        condcode = code[LINK_SIZE+1];

        /* Back reference conditions are not supported */

        if (condcode == OP_CREF || condcode == OP_NCREF)
          return PCRE_ERROR_DFA_UCOND;

        /* The DEFINE condition is always false */

        if (condcode == OP_DEF)
          { ADD_ACTIVE(state_offset + codelink + LINK_SIZE + 1, 0); }

        /* The only supported version of OP_RREF is for the value RREF_ANY,
        which means "test if in any recursion". We can't test for specifically
        recursed groups. */

        else if (condcode == OP_RREF || condcode == OP_NRREF)
          {
          int value = GET2(code, LINK_SIZE + 2);
          if (value != RREF_ANY) return PCRE_ERROR_DFA_UCOND;
          if (md->recursive != NULL)
            { ADD_ACTIVE(state_offset + LINK_SIZE + 2 + IMM2_SIZE, 0); }
          else { ADD_ACTIVE(state_offset + codelink + LINK_SIZE + 1, 0); }
          }

        /* Otherwise, the condition is an assertion */

        else
          {
          int rc;
          const pcre_uchar *asscode = code + LINK_SIZE + 1;
          const pcre_uchar *endasscode = asscode + GET(asscode, 1);

          while (*endasscode == OP_ALT) endasscode += GET(endasscode, 1);

          rc = internal_dfa_exec(
            md,                                   /* fixed match data */
            asscode,                              /* this subexpression's code */
            ptr,                                  /* where we currently are */
            (int)(ptr - start_subject),           /* start offset */
            local_offsets,                        /* offset vector */
            sizeof(local_offsets)/sizeof(int),    /* size of same */
            local_workspace,                      /* workspace vector */
            sizeof(local_workspace)/sizeof(int),  /* size of same */
            rlevel);                              /* function recursion level */

          if (rc == PCRE_ERROR_DFA_UITEM) return rc;
          if ((rc >= 0) ==
                (condcode == OP_ASSERT || condcode == OP_ASSERTBACK))
            { ADD_ACTIVE((int)(endasscode + LINK_SIZE + 1 - start_code), 0); }
          else
            { ADD_ACTIVE(state_offset + codelink + LINK_SIZE + 1, 0); }
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_RECURSE:
        {
        dfa_recursion_info *ri;
        int local_offsets[1000];
        int local_workspace[1000];
        const pcre_uchar *callpat = start_code + GET(code, 1);
        int recno = (callpat == md->start_code)? 0 :
          GET2(callpat, 1 + LINK_SIZE);
        int rc;

        DPRINTF(("%.*sStarting regex recursion\n", rlevel*2-2, SP));

        /* Check for repeating a recursion without advancing the subject
        pointer. This should catch convoluted mutual recursions. (Some simple
        cases are caught at compile time.) */

        for (ri = md->recursive; ri != NULL; ri = ri->prevrec)
          if (recno == ri->group_num && ptr == ri->subject_position)
            return PCRE_ERROR_RECURSELOOP;

        /* Remember this recursion and where we started it so as to
        catch infinite loops. */

        new_recursive.group_num = recno;
        new_recursive.subject_position = ptr;
        new_recursive.prevrec = md->recursive;
        md->recursive = &new_recursive;

        rc = internal_dfa_exec(
          md,                                   /* fixed match data */
          callpat,                              /* this subexpression's code */
          ptr,                                  /* where we currently are */
          (int)(ptr - start_subject),           /* start offset */
          local_offsets,                        /* offset vector */
          sizeof(local_offsets)/sizeof(int),    /* size of same */
          local_workspace,                      /* workspace vector */
          sizeof(local_workspace)/sizeof(int),  /* size of same */
          rlevel);                              /* function recursion level */

        md->recursive = new_recursive.prevrec;  /* Done this recursion */

        DPRINTF(("%.*sReturn from regex recursion: rc=%d\n", rlevel*2-2, SP,
          rc));

        /* Ran out of internal offsets */

        if (rc == 0) return PCRE_ERROR_DFA_RECURSE;

        /* For each successful matched substring, set up the next state with a
        count of characters to skip before trying it. Note that the count is in
        characters, not bytes. */

        if (rc > 0)
          {
          for (rc = rc*2 - 2; rc >= 0; rc -= 2)
            {
            int charcount = local_offsets[rc+1] - local_offsets[rc];
#ifdef SUPPORT_UTF
            const pcre_uchar *p = start_subject + local_offsets[rc];
            const pcre_uchar *pp = start_subject + local_offsets[rc+1];
            while (p < pp) if (NOT_FIRSTCHAR(*p++)) charcount--;
#endif
            if (charcount > 0)
              {
              ADD_NEW_DATA(-(state_offset + LINK_SIZE + 1), 0, (charcount - 1));
              }
            else
              {
              ADD_ACTIVE(state_offset + LINK_SIZE + 1, 0);
              }
            }
          }
        else if (rc != PCRE_ERROR_NOMATCH) return rc;
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_BRAPOS:
      case OP_SBRAPOS:
      case OP_CBRAPOS:
      case OP_SCBRAPOS:
      case OP_BRAPOSZERO:
        {
        int charcount, matched_count;
        const pcre_uchar *local_ptr = ptr;
        BOOL allow_zero;

        if (codevalue == OP_BRAPOSZERO)
          {
          allow_zero = TRUE;
          codevalue = *(++code);  /* Codevalue will be one of above BRAs */
          }
        else allow_zero = FALSE;

        /* Loop to match the subpattern as many times as possible as if it were
        a complete pattern. */

        for (matched_count = 0;; matched_count++)
          {
          int local_offsets[2];
          int local_workspace[1000];

          int rc = internal_dfa_exec(
            md,                                   /* fixed match data */
            code,                                 /* this subexpression's code */
            local_ptr,                            /* where we currently are */
            (int)(ptr - start_subject),           /* start offset */
            local_offsets,                        /* offset vector */
            sizeof(local_offsets)/sizeof(int),    /* size of same */
            local_workspace,                      /* workspace vector */
            sizeof(local_workspace)/sizeof(int),  /* size of same */
            rlevel);                              /* function recursion level */

          /* Failed to match */

          if (rc < 0)
            {
            if (rc != PCRE_ERROR_NOMATCH) return rc;
            break;
            }

          /* Matched: break the loop if zero characters matched. */

          charcount = local_offsets[1] - local_offsets[0];
          if (charcount == 0) break;
          local_ptr += charcount;    /* Advance temporary position ptr */
          }

        /* At this point we have matched the subpattern matched_count
        times, and local_ptr is pointing to the character after the end of the
        last match. */

        if (matched_count > 0 || allow_zero)
          {
          const pcre_uchar *end_subpattern = code;
          int next_state_offset;

          do { end_subpattern += GET(end_subpattern, 1); }
            while (*end_subpattern == OP_ALT);
          next_state_offset =
            (int)(end_subpattern - start_code + LINK_SIZE + 1);

          /* Optimization: if there are no more active states, and there
          are no new states yet set up, then skip over the subject string
          right here, to save looping. Otherwise, set up the new state to swing
          into action when the end of the matched substring is reached. */

          if (i + 1 >= active_count && new_count == 0)
            {
            ptr = local_ptr;
            clen = 0;
            ADD_NEW(next_state_offset, 0);
            }
          else
            {
            const pcre_uchar *p = ptr;
            const pcre_uchar *pp = local_ptr;
            charcount = (int)(pp - p);
#ifdef SUPPORT_UTF
            while (p < pp) if (NOT_FIRSTCHAR(*p++)) charcount--;
#endif
            ADD_NEW_DATA(-next_state_offset, 0, (charcount - 1));
            }
          }
        }
      break;

      /*-----------------------------------------------------------------*/
      case OP_ONCE:
      case OP_ONCE_NC:
        {
        int local_offsets[2];
        int local_workspace[1000];

        int rc = internal_dfa_exec(
          md,                                   /* fixed match data */
          code,                                 /* this subexpression's code */
          ptr,                                  /* where we currently are */
          (int)(ptr - start_subject),           /* start offset */
          local_offsets,                        /* offset vector */
          sizeof(local_offsets)/sizeof(int),    /* size of same */
          local_workspace,                      /* workspace vector */
          sizeof(local_workspace)/sizeof(int),  /* size of same */
          rlevel);                              /* function recursion level */

        if (rc >= 0)
          {
          const pcre_uchar *end_subpattern = code;
          int charcount = local_offsets[1] - local_offsets[0];
          int next_state_offset, repeat_state_offset;

          do { end_subpattern += GET(end_subpattern, 1); }
            while (*end_subpattern == OP_ALT);
          next_state_offset =
            (int)(end_subpattern - start_code + LINK_SIZE + 1);

          /* If the end of this subpattern is KETRMAX or KETRMIN, we must
          arrange for the repeat state also to be added to the relevant list.
          Calculate the offset, or set -1 for no repeat. */

          repeat_state_offset = (*end_subpattern == OP_KETRMAX ||
                                 *end_subpattern == OP_KETRMIN)?
            (int)(end_subpattern - start_code - GET(end_subpattern, 1)) : -1;

          /* If we have matched an empty string, add the next state at the
          current character pointer. This is important so that the duplicate
          checking kicks in, which is what breaks infinite loops that match an
          empty string. */

          if (charcount == 0)
            {
            ADD_ACTIVE(next_state_offset, 0);
            }

          /* Optimization: if there are no more active states, and there
          are no new states yet set up, then skip over the subject string
          right here, to save looping. Otherwise, set up the new state to swing
          into action when the end of the matched substring is reached. */

          else if (i + 1 >= active_count && new_count == 0)
            {
            ptr += charcount;
            clen = 0;
            ADD_NEW(next_state_offset, 0);

            /* If we are adding a repeat state at the new character position,
            we must fudge things so that it is the only current state.
            Otherwise, it might be a duplicate of one we processed before, and
            that would cause it to be skipped. */

            if (repeat_state_offset >= 0)
              {
              next_active_state = active_states;
              active_count = 0;
              i = -1;
              ADD_ACTIVE(repeat_state_offset, 0);
              }
            }
          else
            {
#ifdef SUPPORT_UTF
            const pcre_uchar *p = start_subject + local_offsets[0];
            const pcre_uchar *pp = start_subject + local_offsets[1];
            while (p < pp) if (NOT_FIRSTCHAR(*p++)) charcount--;
#endif
            ADD_NEW_DATA(-next_state_offset, 0, (charcount - 1));
            if (repeat_state_offset >= 0)
              { ADD_NEW_DATA(-repeat_state_offset, 0, (charcount - 1)); }
            }
          }
        else if (rc != PCRE_ERROR_NOMATCH) return rc;
        }
      break;


/* ========================================================================== */
      /* Handle callouts */

      case OP_CALLOUT:
      rrc = 0;
      if (PUBL(callout) != NULL)
        {
        PUBL(callout_block) cb;
        cb.version          = 1;   /* Version 1 of the callout block */
        cb.callout_number   = code[1];
        cb.offset_vector    = offsets;
#ifdef COMPILE_PCRE8
        cb.subject          = (PCRE_SPTR)start_subject;
#else
        cb.subject          = (PCRE_SPTR16)start_subject;
#endif
        cb.subject_length   = (int)(end_subject - start_subject);
        cb.start_match      = (int)(current_subject - start_subject);
        cb.current_position = (int)(ptr - start_subject);
        cb.pattern_position = GET(code, 2);
        cb.next_item_length = GET(code, 2 + LINK_SIZE);
        cb.capture_top      = 1;
        cb.capture_last     = -1;
        cb.callout_data     = md->callout_data;
        cb.mark             = NULL;   /* No (*MARK) support */
        if ((rrc = (*PUBL(callout))(&cb)) < 0) return rrc;   /* Abandon */
        }
      if (rrc == 0)
        { ADD_ACTIVE(state_offset + PRIV(OP_lengths)[OP_CALLOUT], 0); }
      break;


/* ========================================================================== */
      default:        /* Unsupported opcode */
      return PCRE_ERROR_DFA_UITEM;
      }

    NEXT_ACTIVE_STATE: continue;

    }      /* End of loop scanning active states */

  /* We have finished the processing at the current subject character. If no
  new states have been set for the next character, we have found all the
  matches that we are going to find. If we are at the top level and partial
  matching has been requested, check for appropriate conditions.

  The "forced_ fail" variable counts the number of (*F) encountered for the
  character. If it is equal to the original active_count (saved in
  workspace[1]) it means that (*F) was found on every active state. In this
  case we don't want to give a partial match.

  The "could_continue" variable is true if a state could have continued but
  for the fact that the end of the subject was reached. */

  if (new_count <= 0)
    {
    if (rlevel == 1 &&                               /* Top level, and */
        could_continue &&                            /* Some could go on */
        forced_fail != workspace[1] &&               /* Not all forced fail & */
        (                                            /* either... */
        (md->moptions & PCRE_PARTIAL_HARD) != 0      /* Hard partial */
        ||                                           /* or... */
        ((md->moptions & PCRE_PARTIAL_SOFT) != 0 &&  /* Soft partial and */
         match_count < 0)                            /* no matches */
        ) &&                                         /* And... */
        ptr >= end_subject &&                  /* Reached end of subject */
        ptr > md->start_used_ptr)              /* Inspected non-empty string */
      {
      if (offsetcount >= 2)
        {
        offsets[0] = (int)(md->start_used_ptr - start_subject);
        offsets[1] = (int)(end_subject - start_subject);
        }
      match_count = PCRE_ERROR_PARTIAL;
      }

    DPRINTF(("%.*sEnd of internal_dfa_exec %d: returning %d\n"
      "%.*s---------------------\n\n", rlevel*2-2, SP, rlevel, match_count,
      rlevel*2-2, SP));
    break;        /* In effect, "return", but see the comment below */
    }

  /* One or more states are active for the next character. */

  ptr += clen;    /* Advance to next subject character */
  }               /* Loop to move along the subject string */

/* Control gets here from "break" a few lines above. We do it this way because
if we use "return" above, we have compiler trouble. Some compilers warn if
there's nothing here because they think the function doesn't return a value. On
the other hand, if we put a dummy statement here, some more clever compilers
complain that it can't be reached. Sigh. */

return match_count;
}




/*************************************************
*    Execute a Regular Expression - DFA engine   *
*************************************************/

/* This external function applies a compiled re to a subject string using a DFA
engine. This function calls the internal function multiple times if the pattern
is not anchored.

Arguments:
  argument_re     points to the compiled expression
  extra_data      points to extra data or is NULL
  subject         points to the subject string
  length          length of subject string (may contain binary zeros)
  start_offset    where to start in the subject string
  options         option bits
  offsets         vector of match offsets
  offsetcount     size of same
  workspace       workspace vector
  wscount         size of same

Returns:          > 0 => number of match offset pairs placed in offsets
                  = 0 => offsets overflowed; longest matches are present
                   -1 => failed to match
                 < -1 => some kind of unexpected problem
*/

#ifdef COMPILE_PCRE8
PCRE_EXP_DEFN int PCRE_CALL_CONVENTION
pcre_dfa_exec(const pcre *argument_re, const pcre_extra *extra_data,
  const char *subject, int length, int start_offset, int options, int *offsets,
  int offsetcount, int *workspace, int wscount)
#else
PCRE_EXP_DEFN int PCRE_CALL_CONVENTION
pcre16_dfa_exec(const pcre16 *argument_re, const pcre16_extra *extra_data,
  PCRE_SPTR16 subject, int length, int start_offset, int options, int *offsets,
  int offsetcount, int *workspace, int wscount)
#endif
{
REAL_PCRE *re = (REAL_PCRE *)argument_re;
dfa_match_data match_block;
dfa_match_data *md = &match_block;
BOOL utf, anchored, startline, firstline;
const pcre_uchar *current_subject, *end_subject;
const pcre_study_data *study = NULL;

const pcre_uchar *req_char_ptr;
const pcre_uint8 *start_bits = NULL;
BOOL has_first_char = FALSE;
BOOL has_req_char = FALSE;
pcre_uchar first_char = 0;
pcre_uchar first_char2 = 0;
pcre_uchar req_char = 0;
pcre_uchar req_char2 = 0;
int newline;

/* Plausibility checks */

if ((options & ~PUBLIC_DFA_EXEC_OPTIONS) != 0) return PCRE_ERROR_BADOPTION;
if (re == NULL || subject == NULL || workspace == NULL ||
   (offsets == NULL && offsetcount > 0)) return PCRE_ERROR_NULL;
if (offsetcount < 0) return PCRE_ERROR_BADCOUNT;
if (wscount < 20) return PCRE_ERROR_DFA_WSSIZE;
if (start_offset < 0 || start_offset > length) return PCRE_ERROR_BADOFFSET;

/* We need to find the pointer to any study data before we test for byte
flipping, so we scan the extra_data block first. This may set two fields in the
match block, so we must initialize them beforehand. However, the other fields
in the match block must not be set until after the byte flipping. */

md->tables = re->tables;
md->callout_data = NULL;

if (extra_data != NULL)
  {
  unsigned int flags = extra_data->flags;
  if ((flags & PCRE_EXTRA_STUDY_DATA) != 0)
    study = (const pcre_study_data *)extra_data->study_data;
  if ((flags & PCRE_EXTRA_MATCH_LIMIT) != 0) return PCRE_ERROR_DFA_UMLIMIT;
  if ((flags & PCRE_EXTRA_MATCH_LIMIT_RECURSION) != 0)
    return PCRE_ERROR_DFA_UMLIMIT;
  if ((flags & PCRE_EXTRA_CALLOUT_DATA) != 0)
    md->callout_data = extra_data->callout_data;
  if ((flags & PCRE_EXTRA_TABLES) != 0)
    md->tables = extra_data->tables;
  }

/* Check that the first field in the block is the magic number. If it is not,
return with PCRE_ERROR_BADMAGIC. However, if the magic number is equal to
REVERSED_MAGIC_NUMBER we return with PCRE_ERROR_BADENDIANNESS, which
means that the pattern is likely compiled with different endianness. */

if (re->magic_number != MAGIC_NUMBER)
  return re->magic_number == REVERSED_MAGIC_NUMBER?
    PCRE_ERROR_BADENDIANNESS:PCRE_ERROR_BADMAGIC;
if ((re->flags & PCRE_MODE) == 0) return PCRE_ERROR_BADMODE;

/* Set some local values */

current_subject = (const pcre_uchar *)subject + start_offset;
end_subject = (const pcre_uchar *)subject + length;
req_char_ptr = current_subject - 1;

#ifdef SUPPORT_UTF
/* PCRE_UTF16 has the same value as PCRE_UTF8. */
utf = (re->options & PCRE_UTF8) != 0;
#else
utf = FALSE;
#endif

anchored = (options & (PCRE_ANCHORED|PCRE_DFA_RESTART)) != 0 ||
  (re->options & PCRE_ANCHORED) != 0;

/* The remaining fixed data for passing around. */

md->start_code = (const pcre_uchar *)argument_re +
    re->name_table_offset + re->name_count * re->name_entry_size;
md->start_subject = (const pcre_uchar *)subject;
md->end_subject = end_subject;
md->start_offset = start_offset;
md->moptions = options;
md->poptions = re->options;

/* If the BSR option is not set at match time, copy what was set
at compile time. */

if ((md->moptions & (PCRE_BSR_ANYCRLF|PCRE_BSR_UNICODE)) == 0)
  {
  if ((re->options & (PCRE_BSR_ANYCRLF|PCRE_BSR_UNICODE)) != 0)
    md->moptions |= re->options & (PCRE_BSR_ANYCRLF|PCRE_BSR_UNICODE);
#ifdef BSR_ANYCRLF
  else md->moptions |= PCRE_BSR_ANYCRLF;
#endif
  }

/* Handle different types of newline. The three bits give eight cases. If
nothing is set at run time, whatever was used at compile time applies. */

switch ((((options & PCRE_NEWLINE_BITS) == 0)? re->options : (pcre_uint32)options) &
         PCRE_NEWLINE_BITS)
  {
  case 0: newline = NEWLINE; break;   /* Compile-time default */
  case PCRE_NEWLINE_CR: newline = CHAR_CR; break;
  case PCRE_NEWLINE_LF: newline = CHAR_NL; break;
  case PCRE_NEWLINE_CR+
       PCRE_NEWLINE_LF: newline = (CHAR_CR << 8) | CHAR_NL; break;
  case PCRE_NEWLINE_ANY: newline = -1; break;
  case PCRE_NEWLINE_ANYCRLF: newline = -2; break;
  default: return PCRE_ERROR_BADNEWLINE;
  }

if (newline == -2)
  {
  md->nltype = NLTYPE_ANYCRLF;
  }
else if (newline < 0)
  {
  md->nltype = NLTYPE_ANY;
  }
else
  {
  md->nltype = NLTYPE_FIXED;
  if (newline > 255)
    {
    md->nllen = 2;
    md->nl[0] = (newline >> 8) & 255;
    md->nl[1] = newline & 255;
    }
  else
    {
    md->nllen = 1;
    md->nl[0] = newline;
    }
  }

/* Check a UTF-8 string if required. Unfortunately there's no way of passing
back the character offset. */

#ifdef SUPPORT_UTF
if (utf && (options & PCRE_NO_UTF8_CHECK) == 0)
  {
  int erroroffset;
  int errorcode = PRIV(valid_utf)((pcre_uchar *)subject, length, &erroroffset);
  if (errorcode != 0)
    {
    if (offsetcount >= 2)
      {
      offsets[0] = erroroffset;
      offsets[1] = errorcode;
      }
    return (errorcode <= PCRE_UTF8_ERR5 && (options & PCRE_PARTIAL_HARD) != 0)?
      PCRE_ERROR_SHORTUTF8 : PCRE_ERROR_BADUTF8;
    }
  if (start_offset > 0 && start_offset < length &&
        NOT_FIRSTCHAR(((PCRE_PUCHAR)subject)[start_offset]))
    return PCRE_ERROR_BADUTF8_OFFSET;
  }
#endif

/* If the exec call supplied NULL for tables, use the inbuilt ones. This
is a feature that makes it possible to save compiled regex and re-use them
in other programs later. */

if (md->tables == NULL) md->tables = PRIV(default_tables);

/* The "must be at the start of a line" flags are used in a loop when finding
where to start. */

startline = (re->flags & PCRE_STARTLINE) != 0;
firstline = (re->options & PCRE_FIRSTLINE) != 0;

/* Set up the first character to match, if available. The first_byte value is
never set for an anchored regular expression, but the anchoring may be forced
at run time, so we have to test for anchoring. The first char may be unset for
an unanchored pattern, of course. If there's no first char and the pattern was
studied, there may be a bitmap of possible first characters. */

if (!anchored)
  {
  if ((re->flags & PCRE_FIRSTSET) != 0)
    {
    has_first_char = TRUE;
    first_char = first_char2 = (pcre_uchar)(re->first_char);
    if ((re->flags & PCRE_FCH_CASELESS) != 0)
      {
      first_char2 = TABLE_GET(first_char, md->tables + fcc_offset, first_char);
#if defined SUPPORT_UCP && !(defined COMPILE_PCRE8)
      if (utf && first_char > 127)
        first_char2 = UCD_OTHERCASE(first_char);
#endif
      }
    }
  else
    {
    if (!startline && study != NULL &&
         (study->flags & PCRE_STUDY_MAPPED) != 0)
      start_bits = study->start_bits;
    }
  }

/* For anchored or unanchored matches, there may be a "last known required
character" set. */

if ((re->flags & PCRE_REQCHSET) != 0)
  {
  has_req_char = TRUE;
  req_char = req_char2 = (pcre_uchar)(re->req_char);
  if ((re->flags & PCRE_RCH_CASELESS) != 0)
    {
    req_char2 = TABLE_GET(req_char, md->tables + fcc_offset, req_char);
#if defined SUPPORT_UCP && !(defined COMPILE_PCRE8)
    if (utf && req_char > 127)
      req_char2 = UCD_OTHERCASE(req_char);
#endif
    }
  }

/* Call the main matching function, looping for a non-anchored regex after a
failed match. If not restarting, perform certain optimizations at the start of
a match. */

for (;;)
  {
  int rc;

  if ((options & PCRE_DFA_RESTART) == 0)
    {
    const pcre_uchar *save_end_subject = end_subject;

    /* If firstline is TRUE, the start of the match is constrained to the first
    line of a multiline string. Implement this by temporarily adjusting
    end_subject so that we stop scanning at a newline. If the match fails at
    the newline, later code breaks this loop. */

    if (firstline)
      {
      PCRE_PUCHAR t = current_subject;
#ifdef SUPPORT_UTF
      if (utf)
        {
        while (t < md->end_subject && !IS_NEWLINE(t))
          {
          t++;
          ACROSSCHAR(t < end_subject, *t, t++);
          }
        }
      else
#endif
      while (t < md->end_subject && !IS_NEWLINE(t)) t++;
      end_subject = t;
      }

    /* There are some optimizations that avoid running the match if a known
    starting point is not found. However, there is an option that disables
    these, for testing and for ensuring that all callouts do actually occur.
    The option can be set in the regex by (*NO_START_OPT) or passed in
    match-time options. */

    if (((options | re->options) & PCRE_NO_START_OPTIMIZE) == 0)
      {
      /* Advance to a known first char. */

      if (has_first_char)
        {
        if (first_char != first_char2)
          while (current_subject < end_subject &&
              *current_subject != first_char && *current_subject != first_char2)
            current_subject++;
        else
          while (current_subject < end_subject &&
                 *current_subject != first_char)
            current_subject++;
        }

      /* Or to just after a linebreak for a multiline match if possible */

      else if (startline)
        {
        if (current_subject > md->start_subject + start_offset)
          {
#ifdef SUPPORT_UTF
          if (utf)
            {
            while (current_subject < end_subject &&
                   !WAS_NEWLINE(current_subject))
              {
              current_subject++;
              ACROSSCHAR(current_subject < end_subject, *current_subject,
                current_subject++);
              }
            }
          else
#endif
          while (current_subject < end_subject && !WAS_NEWLINE(current_subject))
            current_subject++;

          /* If we have just passed a CR and the newline option is ANY or
          ANYCRLF, and we are now at a LF, advance the match position by one
          more character. */

          if (current_subject[-1] == CHAR_CR &&
               (md->nltype == NLTYPE_ANY || md->nltype == NLTYPE_ANYCRLF) &&
               current_subject < end_subject &&
               *current_subject == CHAR_NL)
            current_subject++;
          }
        }

      /* Or to a non-unique first char after study */

      else if (start_bits != NULL)
        {
        while (current_subject < end_subject)
          {
          register unsigned int c = *current_subject;
#ifndef COMPILE_PCRE8
          if (c > 255) c = 255;
#endif
          if ((start_bits[c/8] & (1 << (c&7))) == 0)
            {
            current_subject++;
#if defined SUPPORT_UTF && defined COMPILE_PCRE8
            /* In non 8-bit mode, the iteration will stop for
            characters > 255 at the beginning or not stop at all. */
            if (utf)
              ACROSSCHAR(current_subject < end_subject, *current_subject,
                current_subject++);
#endif
            }
          else break;
          }
        }
      }

    /* Restore fudged end_subject */

    end_subject = save_end_subject;

    /* The following two optimizations are disabled for partial matching or if
    disabling is explicitly requested (and of course, by the test above, this
    code is not obeyed when restarting after a partial match). */

    if (((options | re->options) & PCRE_NO_START_OPTIMIZE) == 0 &&
        (options & (PCRE_PARTIAL_HARD|PCRE_PARTIAL_SOFT)) == 0)
      {
      /* If the pattern was studied, a minimum subject length may be set. This
      is a lower bound; no actual string of that length may actually match the
      pattern. Although the value is, strictly, in characters, we treat it as
      bytes to avoid spending too much time in this optimization. */

      if (study != NULL && (study->flags & PCRE_STUDY_MINLEN) != 0 &&
          (pcre_uint32)(end_subject - current_subject) < study->minlength)
        return PCRE_ERROR_NOMATCH;

      /* If req_char is set, we know that that character must appear in the
      subject for the match to succeed. If the first character is set, req_char
      must be later in the subject; otherwise the test starts at the match
      point. This optimization can save a huge amount of work in patterns with
      nested unlimited repeats that aren't going to match. Writing separate
      code for cased/caseless versions makes it go faster, as does using an
      autoincrement and backing off on a match.

      HOWEVER: when the subject string is very, very long, searching to its end
      can take a long time, and give bad performance on quite ordinary
      patterns. This showed up when somebody was matching /^C/ on a 32-megabyte
      string... so we don't do this when the string is sufficiently long. */

      if (has_req_char && end_subject - current_subject < REQ_BYTE_MAX)
        {
        register PCRE_PUCHAR p = current_subject + (has_first_char? 1:0);

        /* We don't need to repeat the search if we haven't yet reached the
        place we found it at last time. */

        if (p > req_char_ptr)
          {
          if (req_char != req_char2)
            {
            while (p < end_subject)
              {
              register int pp = *p++;
              if (pp == req_char || pp == req_char2) { p--; break; }
              }
            }
          else
            {
            while (p < end_subject)
              {
              if (*p++ == req_char) { p--; break; }
              }
            }

          /* If we can't find the required character, break the matching loop,
          which will cause a return or PCRE_ERROR_NOMATCH. */

          if (p >= end_subject) break;

          /* If we have found the required character, save the point where we
          found it, so that we don't search again next time round the loop if
          the start hasn't passed this character yet. */

          req_char_ptr = p;
          }
        }
      }
    }   /* End of optimizations that are done when not restarting */

  /* OK, now we can do the business */

  md->start_used_ptr = current_subject;
  md->recursive = NULL;

  rc = internal_dfa_exec(
    md,                                /* fixed match data */
    md->start_code,                    /* this subexpression's code */
    current_subject,                   /* where we currently are */
    start_offset,                      /* start offset in subject */
    offsets,                           /* offset vector */
    offsetcount,                       /* size of same */
    workspace,                         /* workspace vector */
    wscount,                           /* size of same */
    0);                                /* function recurse level */

  /* Anything other than "no match" means we are done, always; otherwise, carry
  on only if not anchored. */

  if (rc != PCRE_ERROR_NOMATCH || anchored) return rc;

  /* Advance to the next subject character unless we are at the end of a line
  and firstline is set. */

  if (firstline && IS_NEWLINE(current_subject)) break;
  current_subject++;
#ifdef SUPPORT_UTF
  if (utf)
    {
    ACROSSCHAR(current_subject < end_subject, *current_subject,
      current_subject++);
    }
#endif
  if (current_subject > end_subject) break;

  /* If we have just passed a CR and we are now at a LF, and the pattern does
  not contain any explicit matches for \r or \n, and the newline option is CRLF
  or ANY or ANYCRLF, advance the match position by one more character. */

  if (current_subject[-1] == CHAR_CR &&
      current_subject < end_subject &&
      *current_subject == CHAR_NL &&
      (re->flags & PCRE_HASCRORLF) == 0 &&
        (md->nltype == NLTYPE_ANY ||
         md->nltype == NLTYPE_ANYCRLF ||
         md->nllen == 2))
    current_subject++;

  }   /* "Bumpalong" loop */

return PCRE_ERROR_NOMATCH;
}

/* End of pcre_dfa_exec.c */
