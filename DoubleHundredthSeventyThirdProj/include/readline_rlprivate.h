/* rlprivate.h -- functions and variables global to the readline library,
		  but not intended for use by applications. */

/* Copyright (C) 1999-2004 Free Software Foundation, Inc.

   This file is part of the GNU Readline Library, a library for
   reading lines of text with interactive input and history editing.

   The GNU Readline Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2, or
   (at your option) any later version.

   The GNU Readline Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   The GNU General Public License is often shipped with GNU software, and
   is generally kept in a file called COPYING or LICENSE.  If you do not
   have a copy of the license, write to the Free Software Foundation,
   59 Temple Place, Suite 330, Boston, MA 02111 USA. */

#if !defined (_RL_PRIVATE_H_)
#define _RL_PRIVATE_H_

#include "readline_rlconf.h"	/* for VISIBLE_STATS */
#include "readline_rlstdc.h"
#include "readline_posixjmp.h"	/* defines procenv_t */

/*************************************************************************
 *									 *
 * Global functions undocumented in texinfo manual and not in readline.h *
 *									 *
 *************************************************************************/

/*************************************************************************
 *									 *
 * Global variables undocumented in texinfo manual and not in readline.h *
 *									 *
 *************************************************************************/

/* complete.c */
READLINE_EXTERN int rl_complete_with_tilde_expansion;
#if defined (VISIBLE_STATS)
READLINE_EXTERN int rl_visible_stats;
#endif /* VISIBLE_STATS */

/* readline.c */
READLINE_EXTERN int rl_line_buffer_len;
READLINE_EXTERN int rl_arg_sign;
READLINE_EXTERN int rl_visible_prompt_length;
READLINE_EXTERN int readline_echoing_p;
READLINE_EXTERN int rl_key_sequence_length;
READLINE_EXTERN int rl_byte_oriented;

/* display.c */
READLINE_EXTERN int rl_display_fixed;

/* parens.c */
READLINE_EXTERN int rl_blink_matching_paren;

/*************************************************************************
 *									 *
 * Global functions and variables unsed and undocumented		 *
 *									 *
 *************************************************************************/

/* kill.c */
READLINE_DLL_IMPEXP int rl_set_retained_kills PARAMS((int));

/* terminal.c */
READLINE_DLL_IMPEXP void _rl_set_screen_size PARAMS((int, int));

/* undo.c */
READLINE_DLL_IMPEXP int _rl_fix_last_undo_of_type PARAMS((int, int, int));

/* util.c */
READLINE_DLL_IMPEXP char *_rl_savestring PARAMS((const char *));

/*************************************************************************
 *									 *
 * Functions and variables private to the readline library		 *
 *									 *
 *************************************************************************/

/* NOTE: Functions and variables prefixed with `_rl_' are
   pseudo-global: they are global so they can be shared
   between files in the readline library, but are not intended
   to be visible to readline callers. */

/*************************************************************************
 * Undocumented private functions					 *
 *************************************************************************/

#if defined(READLINE_CALLBACKS)

/* readline.c */
READLINE_DLL_IMPEXP void readline_internal_setup PARAMS((void));
READLINE_DLL_IMPEXP char *readline_internal_teardown PARAMS((int));
READLINE_DLL_IMPEXP int readline_internal_char PARAMS((void));

#endif /* READLINE_CALLBACKS */

/* bind.c */

/* complete.c */
READLINE_DLL_IMPEXP char _rl_find_completion_word PARAMS((int *, int *));
READLINE_DLL_IMPEXP void _rl_free_match_list PARAMS((char **));

/* display.c */
READLINE_DLL_IMPEXP char *_rl_strip_prompt PARAMS((char *));
READLINE_DLL_IMPEXP void _rl_move_cursor_relative PARAMS((int, const char *));
READLINE_DLL_IMPEXP void _rl_move_vert PARAMS((int));
READLINE_DLL_IMPEXP void _rl_save_prompt PARAMS((void));
READLINE_DLL_IMPEXP void _rl_restore_prompt PARAMS((void));
READLINE_DLL_IMPEXP char *_rl_make_prompt_for_search PARAMS((int));
READLINE_DLL_IMPEXP void _rl_erase_at_end_of_line PARAMS((int));
READLINE_DLL_IMPEXP void _rl_clear_to_eol PARAMS((int));
READLINE_DLL_IMPEXP void _rl_clear_screen PARAMS((void));
READLINE_DLL_IMPEXP void _rl_update_final PARAMS((void));
READLINE_DLL_IMPEXP void _rl_redisplay_after_sigwinch PARAMS((void));
READLINE_DLL_IMPEXP void _rl_clean_up_for_exit PARAMS((void));
READLINE_DLL_IMPEXP void _rl_erase_entire_line PARAMS((void));
READLINE_DLL_IMPEXP int _rl_current_display_line PARAMS((void));

/* input.c */
READLINE_DLL_IMPEXP int _rl_any_typein PARAMS((void));
READLINE_DLL_IMPEXP int _rl_input_available PARAMS((void));
READLINE_DLL_IMPEXP int _rl_input_queued PARAMS((int));
READLINE_DLL_IMPEXP void _rl_insert_typein PARAMS((int));
READLINE_DLL_IMPEXP int _rl_unget_char PARAMS((int));
READLINE_DLL_IMPEXP int _rl_pushed_input_available PARAMS((void));

/* macro.c */
READLINE_DLL_IMPEXP void _rl_with_macro_input PARAMS((char *));
READLINE_DLL_IMPEXP int _rl_next_macro_key PARAMS((void));
READLINE_DLL_IMPEXP void _rl_push_executing_macro PARAMS((void));
READLINE_DLL_IMPEXP void _rl_pop_executing_macro PARAMS((void));
READLINE_DLL_IMPEXP void _rl_add_macro_char PARAMS((int));
READLINE_DLL_IMPEXP void _rl_kill_kbd_macro PARAMS((void));

/* misc.c */
READLINE_DLL_IMPEXP int _rl_init_argument PARAMS((void));
READLINE_DLL_IMPEXP void _rl_start_using_history PARAMS((void));
READLINE_DLL_IMPEXP int _rl_free_saved_history_line PARAMS((void));
READLINE_DLL_IMPEXP void _rl_set_insert_mode PARAMS((int, int));

/* nls.c */
READLINE_DLL_IMPEXP int _rl_init_eightbit PARAMS((void));

/* parens.c */
READLINE_DLL_IMPEXP void _rl_enable_paren_matching PARAMS((int));

/* readline.c */
READLINE_DLL_IMPEXP void _rl_init_line_state PARAMS((void));
READLINE_DLL_IMPEXP void _rl_set_the_line PARAMS((void));
READLINE_DLL_IMPEXP int _rl_dispatch PARAMS((int, Keymap));
READLINE_DLL_IMPEXP int _rl_dispatch_subseq PARAMS((int, Keymap, int));

/* rltty.c */
READLINE_DLL_IMPEXP int _rl_disable_tty_signals PARAMS((void));
READLINE_DLL_IMPEXP int _rl_restore_tty_signals PARAMS((void));

/* terminal.c */
READLINE_DLL_IMPEXP void _rl_get_screen_size PARAMS((int, int));
READLINE_DLL_IMPEXP int _rl_init_terminal_io PARAMS((const char *));
#ifdef _MINIX
READLINE_DLL_IMPEXP void _rl_output_character_function PARAMS((int));
#else
READLINE_DLL_IMPEXP int _rl_output_character_function PARAMS((int));
#endif
READLINE_DLL_IMPEXP void _rl_output_some_chars PARAMS((const char *, int));
READLINE_DLL_IMPEXP int _rl_backspace PARAMS((int));
READLINE_DLL_IMPEXP void _rl_enable_meta_key PARAMS((void));
READLINE_DLL_IMPEXP void _rl_control_keypad PARAMS((int));
READLINE_DLL_IMPEXP void _rl_set_cursor PARAMS((int, int));

/* text.c */
READLINE_DLL_IMPEXP void _rl_fix_point PARAMS((int));
READLINE_DLL_IMPEXP int _rl_replace_text PARAMS((const char *, int, int));
READLINE_DLL_IMPEXP int _rl_insert_char PARAMS((int, int));
READLINE_DLL_IMPEXP int _rl_overwrite_char PARAMS((int, int));
READLINE_DLL_IMPEXP int _rl_overwrite_rubout PARAMS((int, int));
READLINE_DLL_IMPEXP int _rl_rubout_char PARAMS((int, int));
#if defined (HANDLE_MULTIBYTE)
READLINE_DLL_IMPEXP int _rl_char_search_internal PARAMS((int, int, char *, int));
#else
READLINE_DLL_IMPEXP int _rl_char_search_internal PARAMS((int, int, int));
#endif
READLINE_DLL_IMPEXP int _rl_set_mark_at_pos PARAMS((int));

/* util.c */
READLINE_DLL_IMPEXP int _rl_abort_internal PARAMS((void));
READLINE_DLL_IMPEXP char *_rl_strindex PARAMS((const char *, const char *));
READLINE_DLL_IMPEXP int _rl_qsort_string_compare PARAMS((char **, char **));
READLINE_DLL_IMPEXP int (_rl_uppercase_p) PARAMS((int));
READLINE_DLL_IMPEXP int (_rl_lowercase_p) PARAMS((int));
READLINE_DLL_IMPEXP int (_rl_pure_alphabetic) PARAMS((int));
READLINE_DLL_IMPEXP int (_rl_digit_p) PARAMS((int));
READLINE_DLL_IMPEXP int (_rl_to_lower) PARAMS((int));
READLINE_DLL_IMPEXP int (_rl_to_upper) PARAMS((int));
READLINE_DLL_IMPEXP int (_rl_digit_value) PARAMS((int));

/* vi_mode.c */
READLINE_DLL_IMPEXP void _rl_vi_initialize_line PARAMS((void));
READLINE_DLL_IMPEXP void _rl_vi_reset_last PARAMS((void));
READLINE_DLL_IMPEXP void _rl_vi_set_last PARAMS((int, int, int));
READLINE_DLL_IMPEXP int _rl_vi_textmod_command PARAMS((int));
READLINE_DLL_IMPEXP void _rl_vi_done_inserting PARAMS((void));

/*************************************************************************
 * Undocumented private variables					 *
 *************************************************************************/

/* bind.c */
READLINE_EXTERN const char *_rl_possible_control_prefixes[];
READLINE_EXTERN const char *_rl_possible_meta_prefixes[];

/* complete.c */
READLINE_EXTERN int _rl_complete_show_all;
READLINE_EXTERN int _rl_complete_show_unmodified;
READLINE_EXTERN int _rl_complete_mark_directories;
READLINE_EXTERN int _rl_complete_mark_symlink_dirs;
READLINE_EXTERN int _rl_print_completions_horizontally;
READLINE_EXTERN int _rl_completion_case_fold;
READLINE_EXTERN int _rl_match_hidden_files;
READLINE_EXTERN int _rl_page_completions;

/* display.c */
READLINE_EXTERN int _rl_vis_botlin;
READLINE_EXTERN int _rl_last_c_pos;
READLINE_EXTERN int _rl_suppress_redisplay;
READLINE_EXTERN char *rl_display_prompt;

/* isearch.c */
READLINE_EXTERN char *_rl_isearch_terminators;

/* macro.c */
READLINE_EXTERN char *_rl_executing_macro;

/* misc.c */
READLINE_EXTERN int _rl_history_preserve_point;
READLINE_EXTERN int _rl_history_saved_point;

/* readline.c */
READLINE_EXTERN int _rl_horizontal_scroll_mode;
READLINE_EXTERN int _rl_mark_modified_lines;
READLINE_EXTERN int _rl_bell_preference;
READLINE_EXTERN int _rl_meta_flag;
READLINE_EXTERN int _rl_convert_meta_chars_to_ascii;
READLINE_EXTERN int _rl_output_meta_chars;
READLINE_EXTERN char *_rl_comment_begin;
READLINE_EXTERN unsigned char _rl_parsing_conditionalized_out;
READLINE_EXTERN Keymap _rl_keymap;
READLINE_EXTERN FILE *_rl_in_stream;
READLINE_EXTERN FILE *_rl_out_stream;
READLINE_EXTERN int _rl_last_command_was_kill;
READLINE_EXTERN int _rl_eof_char;
READLINE_EXTERN procenv_t readline_top_level;

/* terminal.c */
READLINE_EXTERN int _rl_enable_keypad;
READLINE_EXTERN int _rl_enable_meta;
READLINE_EXTERN char *_rl_term_clreol;
READLINE_EXTERN char *_rl_term_clrpag;
READLINE_EXTERN char *_rl_term_im;
READLINE_EXTERN char *_rl_term_ic;
READLINE_EXTERN char *_rl_term_ei;
READLINE_EXTERN char *_rl_term_DC;
READLINE_EXTERN char *_rl_term_up;
READLINE_EXTERN char *_rl_term_dc;
READLINE_EXTERN char *_rl_term_cr;
READLINE_EXTERN char *_rl_term_IC;
READLINE_EXTERN int _rl_screenheight;
READLINE_EXTERN int _rl_screenwidth;
READLINE_EXTERN int _rl_screenchars;
READLINE_EXTERN int _rl_terminal_can_insert;
READLINE_EXTERN int _rl_term_autowrap;

/* undo.c */
READLINE_EXTERN int _rl_doing_an_undo;
READLINE_EXTERN int _rl_undo_group_level;

/* vi_mode.c */
READLINE_EXTERN int _rl_vi_last_command;

#endif /* _RL_PRIVATE_H_ */
