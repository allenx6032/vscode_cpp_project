#ifndef RAYFORK_IMAGE_H
#define RAYFORK_IMAGE_H

#include "rayfork-colors.h"

typedef enum rf_desired_channels
{
    RF_ANY_CHANNELS = 0,
    RF_1BYTE_GRAYSCALE = 1,
    RF_2BYTE_GRAY_ALPHA = 2,
    RF_3BYTE_R8G8B8 = 3,
    RF_4BYTE_R8G8B8A8 = 4,
} rf_desired_channels;

typedef struct rf_image
{
    void*           data;    // image raw data
    int             width;   // image base width
    int             height;  // image base height
    rf_pixel_format format;  // Data format (rf_pixel_format type)
    rf_bool            valid;   // True if the image is valid and can be used
} rf_image;

typedef struct rf_mipmaps_stats
{
    int possible_mip_counts;
    int mipmaps_buffer_size;
} rf_mipmaps_stats;

typedef struct rf_mipmaps_image
{
    union
    {
        rf_image image;
        struct
        {
            void*           data;    // image raw data
            int             width;   // image base width
            int             height;  // image base height
            rf_pixel_format format;  // Data format (rf_pixel_format type)
            rf_bool            valid;
        };
    };

    int mipmaps; // Mipmap levels, 1 by default
} rf_mipmaps_image;

typedef struct rf_gif
{
    int frames_count;
    int* frame_delays;

    union
    {
        rf_image image;

        struct
        {
            void*           data;    // rf_image raw data
            int             width;   // rf_image base width
            int             height;  // rf_image base height
            rf_pixel_format format;  // Data format (rf_pixel_format type)
            rf_bool            valid;
        };
    };
} rf_gif;

#pragma region extract image data functions
rf_public int rf_image_size(rf_image image);
rf_public int rf_image_size_in_format(rf_image image, rf_pixel_format format);

rf_public rf_bool rf_image_get_pixels_as_rgba32_to_buffer(rf_image image, rf_color* dst, rf_int dst_size);
rf_public rf_bool rf_image_get_pixels_as_normalized_to_buffer(rf_image image, rf_vec4* dst, rf_int dst_size);

rf_public rf_color* rf_image_pixels_to_rgba32(rf_image image, rf_allocator allocator);
rf_public rf_vec4* rf_image_compute_pixels_to_normalized(rf_image image, rf_allocator allocator);

rf_public void rf_image_extract_palette_to_buffer(rf_image image, rf_color* palette_dst, rf_int palette_size);
rf_public rf_palette rf_image_extract_palette(rf_image image, rf_int palette_size, rf_allocator allocator);
rf_public rf_rec rf_image_alpha_border(rf_image image, float threshold);
#pragma endregion

#pragma region loading & unloading functions
rf_public rf_bool rf_supports_image_file_type(const char* filename);

rf_public rf_image rf_load_image_from_file_data_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size, rf_desired_channels channels, rf_allocator temp_allocator);
rf_public rf_image rf_load_image_from_file_data(const void* src, rf_int src_size, rf_desired_channels channels, rf_allocator allocator, rf_allocator temp_allocator);

rf_public rf_image rf_load_image_from_hdr_file_data_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size, rf_desired_channels channels, rf_allocator temp_allocator);
rf_public rf_image rf_load_image_from_hdr_file_data(const void* src, rf_int src_size, rf_allocator allocator, rf_allocator temp_allocator);

rf_public rf_image rf_load_image_from_format_to_buffer(const void* src, rf_int src_size, int src_width, int src_height, rf_uncompressed_pixel_format src_format, void* dst, rf_int dst_size, rf_uncompressed_pixel_format dst_format);
rf_public rf_image rf_load_image_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);

rf_public void rf_unload_image(rf_image image, rf_allocator allocator);
#pragma endregion

#pragma region mipmaps
rf_public int rf_mipmaps_image_size(rf_mipmaps_image image);
rf_public rf_mipmaps_stats rf_compute_mipmaps_stats(rf_image image, int desired_mipmaps_count);
rf_public rf_mipmaps_image rf_image_gen_mipmaps_to_buffer(rf_image image, int gen_mipmaps_count, void* dst, rf_int dst_size, rf_allocator temp_allocator);  // Generate all mipmap levels for a provided image. image.data is scaled to include mipmap levels. Mipmaps format is the same as base image
rf_public rf_mipmaps_image rf_image_gen_mipmaps(rf_image image, int desired_mipmaps_count, rf_allocator allocator, rf_allocator temp_allocator);
rf_public void rf_unload_mipmaps_image(rf_mipmaps_image image, rf_allocator allocator);
#pragma endregion

#pragma region dds
rf_public rf_int rf_get_dds_image_size(const void* src, rf_int src_size);
rf_public rf_mipmaps_image rf_load_dds_image_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size);
rf_public rf_mipmaps_image rf_load_dds_image(const void* src, rf_int src_size, rf_allocator allocator);
rf_public rf_mipmaps_image rf_load_dds_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#pragma region pkm
rf_public rf_int rf_get_pkm_image_size(const void* src, rf_int src_size);
rf_public rf_image rf_load_pkm_image_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size);
rf_public rf_image rf_load_pkm_image(const void* src, rf_int src_size, rf_allocator allocator);
rf_public rf_image rf_load_pkm_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#pragma region ktx
rf_public rf_int rf_get_ktx_image_size(const void* src, rf_int src_size);
rf_public rf_mipmaps_image rf_load_ktx_image_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size);
rf_public rf_mipmaps_image rf_load_ktx_image(const void* src, rf_int src_size, rf_allocator allocator);
rf_public rf_mipmaps_image rf_load_ktx_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
#pragma endregion

#pragma region gif
rf_public rf_gif rf_load_animated_gif(const void* data, rf_int data_size, rf_allocator allocator, rf_allocator temp_allocator);
rf_public rf_gif rf_load_animated_gif_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io);
rf_public rf_sizei rf_gif_frame_size(rf_gif gif);
rf_public rf_image rf_get_frame_from_gif(rf_gif gif, int frame);
rf_public void rf_unload_gif(rf_gif gif, rf_allocator allocator);
#pragma endregion

#pragma region image gen
rf_public rf_vec2 rf_get_seed_for_cellular_image(int seeds_per_row, int tile_size, int i, rf_rand_proc rand);

rf_public rf_image rf_gen_image_color_to_buffer(int width, int height, rf_color color, rf_color* dst, rf_int dst_size);
rf_public rf_image rf_gen_image_color(int width, int height, rf_color color, rf_allocator allocator);
rf_public rf_image rf_gen_image_gradient_v_to_buffer(int width, int height, rf_color top, rf_color bottom, rf_color* dst, rf_int dst_size);
rf_public rf_image rf_gen_image_gradient_v(int width, int height, rf_color top, rf_color bottom, rf_allocator allocator);
rf_public rf_image rf_gen_image_gradient_h_to_buffer(int width, int height, rf_color left, rf_color right, rf_color* dst, rf_int dst_size);
rf_public rf_image rf_gen_image_gradient_h(int width, int height, rf_color left, rf_color right, rf_allocator allocator);
rf_public rf_image rf_gen_image_gradient_radial_to_buffer(int width, int height, float density, rf_color inner, rf_color outer, rf_color* dst, rf_int dst_size);
rf_public rf_image rf_gen_image_gradient_radial(int width, int height, float density, rf_color inner, rf_color outer, rf_allocator allocator);
rf_public rf_image rf_gen_image_checked_to_buffer(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2, rf_color* dst, rf_int dst_size);
rf_public rf_image rf_gen_image_checked(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2, rf_allocator allocator);
rf_public rf_image rf_gen_image_white_noise_to_buffer(int width, int height, float factor, rf_rand_proc rand, rf_color* dst, rf_int dst_size);
rf_public rf_image rf_gen_image_white_noise(int width, int height, float factor, rf_rand_proc rand, rf_allocator allocator);
rf_public rf_image rf_gen_image_perlin_noise_to_buffer(int width, int height, int offset_x, int offset_y, float scale, rf_color* dst, rf_int dst_size);
rf_public rf_image rf_gen_image_perlin_noise(int width, int height, int offset_x, int offset_y, float scale, rf_allocator allocator);
rf_public rf_image rf_gen_image_cellular_to_buffer(int width, int height, int tile_size, rf_rand_proc rand, rf_color* dst, rf_int dst_size);
rf_public rf_image rf_gen_image_cellular(int width, int height, int tile_size, rf_rand_proc rand, rf_allocator allocator);
#pragma endregion

#pragma region image manipulation
rf_public rf_image rf_image_copy_to_buffer(rf_image image, void* dst, rf_int dst_size);
rf_public rf_image rf_image_copy(rf_image image, rf_allocator allocator);

rf_public rf_image rf_image_crop_to_buffer(rf_image image, rf_rec crop, void* dst, rf_int dst_size, rf_uncompressed_pixel_format dst_format);
rf_public rf_image rf_image_crop(rf_image image, rf_rec crop, rf_allocator allocator);

rf_public rf_image rf_image_resize_to_buffer(rf_image image, int new_width, int new_height, void* dst, rf_int dst_size, rf_allocator temp_allocator);
rf_public rf_image rf_image_resize(rf_image image, int new_width, int new_height, rf_allocator allocator, rf_allocator temp_allocator);
rf_public rf_image rf_image_resize_nn_to_buffer(rf_image image, int new_width, int new_height, void* dst, rf_int dst_size);
rf_public rf_image rf_image_resize_nn(rf_image image, int new_width, int new_height, rf_allocator allocator);

rf_public rf_image rf_image_format_to_buffer(rf_image image, rf_uncompressed_pixel_format dst_format, void* dst, rf_int dst_size);
rf_public rf_image rf_image_format(rf_image image, rf_uncompressed_pixel_format new_format, rf_allocator allocator);

rf_public rf_image rf_image_alpha_mask_to_buffer(rf_image image, rf_image alpha_mask, void* dst, rf_int dst_size);
rf_public rf_image rf_image_alpha_clear(rf_image image, rf_color color, float threshold, rf_allocator allocator, rf_allocator temp_allocator);
rf_public rf_image rf_image_alpha_premultiply(rf_image image, rf_allocator allocator, rf_allocator temp_allocator);

rf_public rf_rec rf_image_alpha_crop_rec(rf_image image, float threshold);
rf_public rf_image rf_image_alpha_crop(rf_image image, float threshold, rf_allocator allocator);

rf_public rf_image rf_image_dither(rf_image image, int r_bpp, int g_bpp, int b_bpp, int a_bpp, rf_allocator allocator, rf_allocator temp_allocator);

rf_public void rf_image_flip_vertical_in_place(rf_image* image);
rf_public rf_image rf_image_flip_vertical_to_buffer(rf_image image, void* dst, rf_int dst_size);
rf_public rf_image rf_image_flip_vertical(rf_image image, rf_allocator allocator);

rf_public void rf_image_flip_horizontal_in_place(rf_image* image);
rf_public rf_image rf_image_flip_horizontal_to_buffer(rf_image image, void* dst, rf_int dst_size);
rf_public rf_image rf_image_flip_horizontal(rf_image image, rf_allocator allocator);

rf_public rf_image rf_image_rotate_cw_to_buffer(rf_image image, void* dst, rf_int dst_size);
rf_public rf_image rf_image_rotate_cw(rf_image image);
rf_public rf_image rf_image_rotate_ccw_to_buffer(rf_image image, void* dst, rf_int dst_size);
rf_public rf_image rf_image_rotate_ccw(rf_image image);

rf_public rf_image rf_image_color_tint_to_buffer(rf_image image, rf_color color, void* dst, rf_int dst_size);
rf_public rf_image rf_image_color_tint(rf_image image, rf_color color);
rf_public rf_image rf_image_color_invert_to_buffer(rf_image image, void* dst, rf_int dst_size);
rf_public rf_image rf_image_color_invert(rf_image image);
rf_public rf_image rf_image_color_grayscale_to_buffer(rf_image image, void* dst, rf_int dst_size);
rf_public rf_image rf_image_color_grayscale(rf_image image);
rf_public rf_image rf_image_color_contrast_to_buffer(rf_image image, float contrast, void* dst, rf_int dst_size);
rf_public rf_image rf_image_color_contrast(rf_image image, int contrast);
rf_public rf_image rf_image_color_brightness_to_buffer(rf_image image, int brightness, void* dst, rf_int dst_size);
rf_public rf_image rf_image_color_brightness(rf_image image, int brightness);
rf_public rf_image rf_image_color_replace_to_buffer(rf_image image, rf_color color, rf_color replace, void* dst, rf_int dst_size);
rf_public rf_image rf_image_color_replace(rf_image image, rf_color color, rf_color replace);

rf_public void rf_image_draw(rf_image* dst, rf_image src, rf_rec src_rec, rf_rec dst_rec, rf_color tint, rf_allocator temp_allocator);
rf_public void rf_image_draw_rectangle(rf_image* dst, rf_rec rec, rf_color color, rf_allocator temp_allocator);
rf_public void rf_image_draw_rectangle_lines(rf_image* dst, rf_rec rec, int thick, rf_color color, rf_allocator temp_allocator);
#pragma endregion

#pragma region ez
#ifdef RAYFORK_EZ

#pragma region extract image data functions
rf_public rf_color* rf_image_pixels_to_rgba32_ez(rf_image image);
rf_public rf_vec4* rf_image_compute_pixels_to_normalized_ez(rf_image image);
rf_public rf_palette rf_image_extract_palette_ez(rf_image image, int palette_size);
#pragma endregion

#pragma region loading & unloading functions
rf_public rf_image rf_load_image_from_file_data_ez(const void* src, int src_size);
rf_public rf_image rf_load_image_from_hdr_file_data_ez(const void* src, int src_size);
rf_public rf_image rf_load_image_from_file_ez(const char* filename);
rf_public void rf_unload_image_ez(rf_image image);
#pragma endregion

#pragma region image manipulation
rf_public rf_image rf_image_copy_ez(rf_image image);

rf_public rf_image rf_image_crop_ez(rf_image image, rf_rec crop);

rf_public rf_image rf_image_resize_ez(rf_image image, int new_width, int new_height);
rf_public rf_image rf_image_resize_nn_ez(rf_image image, int new_width, int new_height);

rf_public rf_image rf_image_format_ez(rf_image image, rf_uncompressed_pixel_format new_format);

rf_public rf_image rf_image_alpha_clear_ez(rf_image image, rf_color color, float threshold);
rf_public rf_image rf_image_alpha_premultiply_ez(rf_image image);
rf_public rf_image rf_image_alpha_crop_ez(rf_image image, float threshold);
rf_public rf_image rf_image_dither_ez(rf_image image, int r_bpp, int g_bpp, int b_bpp, int a_bpp);

rf_public rf_image rf_image_flip_vertical_ez(rf_image image);
rf_public rf_image rf_image_flip_horizontal_ez(rf_image image);

rf_public rf_vec2 rf_get_seed_for_cellular_image_ez(int seeds_per_row, int tile_size, int i);

rf_public rf_image rf_gen_image_color_ez(int width, int height, rf_color color);
rf_public rf_image rf_gen_image_gradient_v_ez(int width, int height, rf_color top, rf_color bottom);
rf_public rf_image rf_gen_image_gradient_h_ez(int width, int height, rf_color left, rf_color right);
rf_public rf_image rf_gen_image_gradient_radial_ez(int width, int height, float density, rf_color inner, rf_color outer);
rf_public rf_image rf_gen_image_checked_ez(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2);
rf_public rf_image rf_gen_image_white_noise_ez(int width, int height, float factor);
rf_public rf_image rf_gen_image_perlin_noise_ez(int width, int height, int offset_x, int offset_y, float scale);
rf_public rf_image rf_gen_image_cellular_ez(int width, int height, int tile_size);
#pragma endregion

#pragma region mipmaps
rf_public rf_mipmaps_image rf_image_gen_mipmaps_ez(rf_image image, int gen_mipmaps_count);
rf_public void rf_unload_mipmaps_image_ez(rf_mipmaps_image image);
#pragma endregion

#pragma region dds
rf_public rf_mipmaps_image rf_load_dds_image_ez(const void* src, int src_size);
rf_public rf_mipmaps_image rf_load_dds_image_from_file_ez(const char* file);
#pragma endregion

#pragma region pkm
rf_public rf_image rf_load_pkm_image_ez(const void* src, int src_size);
rf_public rf_image rf_load_pkm_image_from_file_ez(const char* file);
#pragma endregion

#pragma region ktx
rf_public rf_mipmaps_image rf_load_ktx_image_ez(const void* src, int src_size);
rf_public rf_mipmaps_image rf_load_ktx_image_from_file_ez(const char* file);
#pragma endregion

#pragma region gif
rf_public rf_gif rf_load_animated_gif_ez(const void* data, int data_size);
rf_public rf_gif rf_load_animated_gif_file_ez(const char* filename);
rf_public void rf_unload_gif_ez(rf_gif gif);
#pragma endregion

#endif // RAYFORK_EZ
#pragma endregion

#endif // RAYFORK_IMAGE_H