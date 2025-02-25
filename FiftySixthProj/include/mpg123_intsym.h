#ifndef MPG123_INTMAP_H
#define MPG123_INTMAP_H
/* Mapping of internal mpg123 symbols to something that is less likely to conflict in case of static linking. */
#define COS9 INT123_COS9
#define tfcos36 INT123_tfcos36
#define pnts INT123_pnts
#define safe_realloc INT123_safe_realloc
#define compat_open INT123_compat_open
#define compat_close INT123_compat_close
#define win32_wide_utf8 INT123_win32_wide_utf8
#define win32_utf8_wide INT123_win32_utf8_wide
#define ntom_set_ntom INT123_ntom_set_ntom
// #define synth_1to1 INT123_synth_1to1
// #define synth_1to1_dither INT123_synth_1to1_dither
// #define synth_1to1_i386 INT123_synth_1to1_i386
// #define synth_1to1_i586 INT123_synth_1to1_i586
// #define synth_1to1_i586_dither INT123_synth_1to1_i586_dither
// #define synth_1to1_mmx INT123_synth_1to1_mmx
// #define synth_1to1_3dnow INT123_synth_1to1_3dnow
// #define synth_1to1_sse INT123_synth_1to1_sse
// #define synth_1to1_stereo_sse INT123_synth_1to1_stereo_sse
// #define synth_1to1_3dnowext INT123_synth_1to1_3dnowext
// #define synth_1to1_altivec INT123_synth_1to1_altivec
// #define synth_1to1_stereo_altivec INT123_synth_1to1_stereo_altivec
// #define synth_1to1_x86_64 INT123_synth_1to1_x86_64
// #define synth_1to1_stereo_x86_64 INT123_synth_1to1_stereo_x86_64
// #define synth_1to1_avx INT123_synth_1to1_avx
// #define synth_1to1_stereo_avx INT123_synth_1to1_stereo_avx
// #define synth_1to1_arm INT123_synth_1to1_arm
// #define synth_1to1_neon INT123_synth_1to1_neon
// #define synth_1to1_stereo_neon INT123_synth_1to1_stereo_neon
#define absynth_1to1_i486 INT123_absynth_1to1_i486
// #define synth_1to1_mono INT123_synth_1to1_mono
// #define synth_1to1_m2s INT123_synth_1to1_m2s
// #define synth_2to1 INT123_synth_2to1
// #define synth_2to1_dither INT123_synth_2to1_dither
// #define synth_2to1_i386 INT123_synth_2to1_i386
// #define synth_2to1_mono INT123_synth_2to1_mono
// #define synth_2to1_m2s INT123_synth_2to1_m2s
// #define synth_4to1 INT123_synth_4to1
// #define synth_4to1_dither INT123_synth_4to1_dither
// #define synth_4to1_i386 INT123_synth_4to1_i386
// #define synth_4to1_mono INT123_synth_4to1_mono
// #define synth_4to1_m2s INT123_synth_4to1_m2s
// #define synth_ntom INT123_synth_ntom
// #define synth_ntom_mono INT123_synth_ntom_mono
// #define synth_ntom_m2s INT123_synth_ntom_m2s
// #define synth_1to1_8bit INT123_synth_1to1_8bit
// #define synth_1to1_8bit_i386 INT123_synth_1to1_8bit_i386
// #define synth_1to1_8bit_wrap INT123_synth_1to1_8bit_wrap
// #define synth_1to1_8bit_mono INT123_synth_1to1_8bit_mono
// #define synth_1to1_8bit_m2s INT123_synth_1to1_8bit_m2s
// #define synth_1to1_8bit_wrap_mono INT123_synth_1to1_8bit_wrap_mono
// #define synth_1to1_8bit_wrap_m2s INT123_synth_1to1_8bit_wrap_m2s
// #define synth_2to1_8bit INT123_synth_2to1_8bit
// #define synth_2to1_8bit_i386 INT123_synth_2to1_8bit_i386
// #define synth_2to1_8bit_mono INT123_synth_2to1_8bit_mono
// #define synth_2to1_8bit_m2s INT123_synth_2to1_8bit_m2s
// #define synth_4to1_8bit INT123_synth_4to1_8bit
// #define synth_4to1_8bit_i386 INT123_synth_4to1_8bit_i386
// #define synth_4to1_8bit_mono INT123_synth_4to1_8bit_mono
// #define synth_4to1_8bit_m2s INT123_synth_4to1_8bit_m2s
// #define synth_ntom_8bit INT123_synth_ntom_8bit
// #define synth_ntom_8bit_mono INT123_synth_ntom_8bit_mono
// #define synth_ntom_8bit_m2s INT123_synth_ntom_8bit_m2s
// #define synth_1to1_real INT123_synth_1to1_real
// #define synth_1to1_real_i386 INT123_synth_1to1_real_i386
// #define synth_1to1_real_sse INT123_synth_1to1_real_sse
// #define synth_1to1_real_stereo_sse INT123_synth_1to1_real_stereo_sse
// #define synth_1to1_real_x86_64 INT123_synth_1to1_real_x86_64
// #define synth_1to1_real_stereo_x86_64 INT123_synth_1to1_real_stereo_x86_64
// #define synth_1to1_real_avx INT123_synth_1to1_real_avx
// #define synth_1to1_real_stereo_avx INT123_synth_1to1_real_stereo_avx
// #define synth_1to1_real_altivec INT123_synth_1to1_real_altivec
// #define synth_1to1_real_stereo_altivec INT123_synth_1to1_real_stereo_altivec
// #define synth_1to1_real_neon INT123_synth_1to1_real_neon
// #define synth_1to1_real_stereo_neon INT123_synth_1to1_real_stereo_neon
// #define synth_1to1_real_mono INT123_synth_1to1_real_mono
// #define synth_1to1_real_m2s INT123_synth_1to1_real_m2s
// #define synth_2to1_real INT123_synth_2to1_real
// #define synth_2to1_real_i386 INT123_synth_2to1_real_i386
// #define synth_2to1_real_mono INT123_synth_2to1_real_mono
// #define synth_2to1_real_m2s INT123_synth_2to1_real_m2s
// #define synth_4to1_real INT123_synth_4to1_real
// #define synth_4to1_real_i386 INT123_synth_4to1_real_i386
// #define synth_4to1_real_mono INT123_synth_4to1_real_mono
// #define synth_4to1_real_m2s INT123_synth_4to1_real_m2s
// #define synth_ntom_real INT123_synth_ntom_real
// #define synth_ntom_real_mono INT123_synth_ntom_real_mono
// #define synth_ntom_real_m2s INT123_synth_ntom_real_m2s
// #define synth_1to1_s32 INT123_synth_1to1_s32
// #define synth_1to1_s32_i386 INT123_synth_1to1_s32_i386
// #define synth_1to1_s32_sse INT123_synth_1to1_s32_sse
// #define synth_1to1_s32_stereo_sse INT123_synth_1to1_s32_stereo_sse
// #define synth_1to1_s32_x86_64 INT123_synth_1to1_s32_x86_64
// #define synth_1to1_s32_stereo_x86_64 INT123_synth_1to1_s32_stereo_x86_64
// #define synth_1to1_s32_avx INT123_synth_1to1_s32_avx
// #define synth_1to1_s32_stereo_avx INT123_synth_1to1_s32_stereo_avx
// #define synth_1to1_s32_altivec INT123_synth_1to1_s32_altivec
// #define synth_1to1_s32_stereo_altivec INT123_synth_1to1_s32_stereo_altivec
// #define synth_1to1_s32_neon INT123_synth_1to1_s32_neon
// #define synth_1to1_s32_stereo_neon INT123_synth_1to1_s32_stereo_neon
// #define synth_1to1_s32_mono INT123_synth_1to1_s32_mono
// #define synth_1to1_s32_m2s INT123_synth_1to1_s32_m2s
// #define synth_2to1_s32 INT123_synth_2to1_s32
// #define synth_2to1_s32_i386 INT123_synth_2to1_s32_i386
// #define synth_2to1_s32_mono INT123_synth_2to1_s32_mono
// #define synth_2to1_s32_m2s INT123_synth_2to1_s32_m2s
// #define synth_4to1_s32 INT123_synth_4to1_s32
// #define synth_4to1_s32_i386 INT123_synth_4to1_s32_i386
// #define synth_4to1_s32_mono INT123_synth_4to1_s32_mono
// #define synth_4to1_s32_m2s INT123_synth_4to1_s32_m2s
// #define synth_ntom_s32 INT123_synth_ntom_s32
// #define synth_ntom_s32_mono INT123_synth_ntom_s32_mono
// #define synth_ntom_s32_m2s INT123_synth_ntom_s32_m2s
#define dct64 INT123_dct64
#define dct64_i386 INT123_dct64_i386
#define dct64_altivec INT123_dct64_altivec
#define dct64_i486 INT123_dct64_i486
#define dct36 INT123_dct36
#define dct36_3dnow INT123_dct36_3dnow
#define dct36_3dnowext INT123_dct36_3dnowext
#define dct36_sse INT123_dct36_sse
#define dct36_x86_64 INT123_dct36_x86_64
#define dct36_avx INT123_dct36_avx
#define synth_ntom_set_step INT123_synth_ntom_set_step
#define ntom_val INT123_ntom_val
#define ntom_frame_outsamples INT123_ntom_frame_outsamples
#define ntom_frmouts INT123_ntom_frmouts
#define ntom_ins2outs INT123_ntom_ins2outs
#define ntom_frameoff INT123_ntom_frameoff
#define init_layer3 INT123_init_layer3
#define init_layer3_gainpow2 INT123_init_layer3_gainpow2
#define init_layer3_stuff INT123_init_layer3_stuff
#define init_layer12 INT123_init_layer12
#define init_layer12_table INT123_init_layer12_table
#define init_layer12_stuff INT123_init_layer12_stuff
#define prepare_decode_tables INT123_prepare_decode_tables
#define make_decode_tables INT123_make_decode_tables
#define make_decode_tables_mmx INT123_make_decode_tables_mmx
#define init_layer3_gainpow2_mmx INT123_init_layer3_gainpow2_mmx
#define init_layer12_table_mmx INT123_init_layer12_table_mmx
#define make_conv16to8_table INT123_make_conv16to8_table
#define do_layer3 INT123_do_layer3
#define do_layer2 INT123_do_layer2
#define do_layer1 INT123_do_layer1
#define do_equalizer INT123_do_equalizer
#define dither_table_init INT123_dither_table_init
#define frame_dither_init INT123_frame_dither_init
#define invalidate_format INT123_invalidate_format
#define frame_init INT123_frame_init
#define frame_init_par INT123_frame_init_par
#define frame_outbuffer INT123_frame_outbuffer
#define frame_output_format INT123_frame_output_format
#define frame_buffers INT123_frame_buffers
#define frame_reset INT123_frame_reset
#define frame_buffers_reset INT123_frame_buffers_reset
#define frame_exit INT123_frame_exit
#define frame_index_find INT123_frame_index_find
#define frame_index_setup INT123_frame_index_setup
#define do_volume INT123_do_volume
#define do_rva INT123_do_rva
#define frame_gapless_init INT123_frame_gapless_init
#define frame_gapless_realinit INT123_frame_gapless_realinit
#define frame_gapless_update INT123_frame_gapless_update
#define frame_gapless_bytify INT123_frame_gapless_bytify
#define frame_gapless_ignore INT123_frame_gapless_ignore
#define frame_expect_outsamples INT123_frame_expect_outsamples
#define frame_skip INT123_frame_skip
#define frame_ins2outs INT123_frame_ins2outs
#define frame_outs INT123_frame_outs
#define frame_expect_outsampels INT123_frame_expect_outsampels
#define frame_offset INT123_frame_offset
#define frame_set_frameseek INT123_frame_set_frameseek
#define frame_set_seek INT123_frame_set_seek
#define frame_tell_seek INT123_frame_tell_seek
#define frame_fill_toc INT123_frame_fill_toc
#define getbits INT123_getbits
#define getcpuflags INT123_getcpuflags
#define icy2utf8 INT123_icy2utf8
#define init_icy INT123_init_icy
#define clear_icy INT123_clear_icy
#define reset_icy INT123_reset_icy
#define init_id3 INT123_init_id3
#define exit_id3 INT123_exit_id3
#define reset_id3 INT123_reset_id3
#define id3_link INT123_id3_link
#define parse_new_id3 INT123_parse_new_id3
#define id3_to_utf8 INT123_id3_to_utf8
#define fi_init INT123_fi_init
#define fi_exit INT123_fi_exit
#define fi_resize INT123_fi_resize
#define fi_add INT123_fi_add
#define fi_set INT123_fi_set
#define fi_reset INT123_fi_reset
#define double_to_long_rounded INT123_double_to_long_rounded
#define scale_rounded INT123_scale_rounded
#define decode_update INT123_decode_update
#define samples_to_bytes INT123_samples_to_bytes
#define bytes_to_samples INT123_bytes_to_samples
#define frame_cpu_opt INT123_frame_cpu_opt
#define set_synth_functions INT123_set_synth_functions
#define dectype INT123_dectype
#define defdec INT123_defdec
#define decclass INT123_decclass
#define check_decoders INT123_check_decoders
#define read_frame_init INT123_read_frame_init
#define frame_bitrate INT123_frame_bitrate
#define frame_freq INT123_frame_freq
#define read_frame_recover INT123_read_frame_recover
#define read_frame INT123_read_frame
#define set_pointer INT123_set_pointer
#define position_info INT123_position_info
#define compute_bpf INT123_compute_bpf
#define time_to_frame INT123_time_to_frame
#define get_songlen INT123_get_songlen
#define open_stream INT123_open_stream
#define open_stream_handle INT123_open_stream_handle
#define open_feed INT123_open_feed
#define feed_more INT123_feed_more
#define feed_forget INT123_feed_forget
#define feed_set_pos INT123_feed_set_pos
#define open_bad INT123_open_bad
#define dct64_3dnow INT123_dct64_3dnow
#define dct64_3dnowext INT123_dct64_3dnowext
#define dct64_mmx INT123_dct64_mmx
#define dct64_MMX INT123_dct64_MMX
#define dct64_sse INT123_dct64_sse
#define dct64_real_sse INT123_dct64_real_sse
#define dct64_x86_64 INT123_dct64_x86_64
#define dct64_real_x86_64 INT123_dct64_real_x86_64
#define dct64_avx INT123_dct64_avx
#define dct64_real_avx INT123_dct64_real_avx
#define dct64_neon INT123_dct64_neon
#define dct64_real_neon INT123_dct64_real_neon
#define do_equalizer_3dnow INT123_do_equalizer_3dnow
#define synth_1to1_3dnow_asm INT123_synth_1to1_3dnow_asm
#define synth_1to1_arm_asm INT123_synth_1to1_arm_asm
#define synth_1to1_arm_accurate_asm INT123_synth_1to1_arm_accurate_asm
#define synth_1to1_i586_asm INT123_synth_1to1_i586_asm
#define synth_1to1_i586_asm_dither INT123_synth_1to1_i586_asm_dither
#define synth_1to1_MMX INT123_synth_1to1_MMX
#define synth_1to1_sse_accurate_asm INT123_synth_1to1_sse_accurate_asm
#define synth_1to1_real_sse_asm INT123_synth_1to1_real_sse_asm
#define synth_1to1_s32_sse_asm INT123_synth_1to1_s32_sse_asm
#define synth_1to1_s_sse_accurate_asm INT123_synth_1to1_s_sse_accurate_asm
#define synth_1to1_real_s_sse_asm INT123_synth_1to1_real_s_sse_asm
#define synth_1to1_s32_s_sse_asm INT123_synth_1to1_s32_s_sse_asm
#define synth_1to1_s_x86_64_asm INT123_synth_1to1_s_x86_64_asm
#define synth_1to1_s_x86_64_accurate_asm INT123_synth_1to1_s_x86_64_accurate_asm
#define synth_1to1_real_s_x86_64_asm INT123_synth_1to1_real_s_x86_64_asm
#define synth_1to1_s32_s_x86_64_asm INT123_synth_1to1_s32_s_x86_64_asm
#define synth_1to1_x86_64_asm INT123_synth_1to1_x86_64_asm
#define synth_1to1_x86_64_accurate_asm INT123_synth_1to1_x86_64_accurate_asm
#define synth_1to1_real_x86_64_asm INT123_synth_1to1_real_x86_64_asm
#define synth_1to1_s32_x86_64_asm INT123_synth_1to1_s32_x86_64_asm
#define synth_1to1_s_avx_asm INT123_synth_1to1_s_avx_asm
#define synth_1to1_s_avx_accurate_asm INT123_synth_1to1_s_avx_accurate_asm
#define synth_1to1_real_s_avx_asm INT123_synth_1to1_real_s_avx_asm
#define synth_1to1_s32_s_avx_asm INT123_synth_1to1_s32_s_avx_asm
#define synth_1to1_neon_asm INT123_synth_1to1_neon_asm
#define synth_1to1_neon_accurate_asm INT123_synth_1to1_neon_accurate_asm
#define synth_1to1_real_neon_asm INT123_synth_1to1_real_neon_asm
#define synth_1to1_s32_neon_asm INT123_synth_1to1_s32_neon_asm
#define synth_1to1_s_neon_asm INT123_synth_1to1_s_neon_asm
#define synth_1to1_s_neon_accurate_asm INT123_synth_1to1_s_neon_accurate_asm
#define synth_1to1_real_s_neon_asm INT123_synth_1to1_real_s_neon_asm
#define synth_1to1_s32_s_neon_asm INT123_synth_1to1_s32_s_neon_asm
#define costab_mmxsse INT123_costab_mmxsse
#define make_decode_tables_mmx_asm INT123_make_decode_tables_mmx_asm
#endif
