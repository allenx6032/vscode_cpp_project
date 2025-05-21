#include "rayfork.h"


#include "string.h"
#include "malloc.h"
#include "stdio.h"

#pragma region error

rf_thread_local rf_recorded_error rf__last_error;

rf_public rf_recorded_error rf_get_last_recorded_error()
{
    return rf__last_error;
}

#pragma endregion

#pragma region allocator

rf_thread_local rf_allocator rf__global_allocator_for_dependencies;

rf_public void* rf_calloc_wrapper(rf_allocator allocator, rf_int amount, rf_int size)
{
    void* ptr = rf_alloc(allocator, amount * size);
    memset(ptr, 0, amount * size);
    return ptr;
}

rf_public void* rf_default_realloc(rf_allocator allocator, void* source, int old_size, int new_size)
{
    void* new_alloc = rf_alloc(allocator, new_size);
    if (new_alloc && source && old_size) { memcpy(new_alloc, source, old_size); }
    if (source) { rf_free(allocator, source); }
    return new_alloc;
}

rf_public void* rf_libc_allocator_wrapper(struct rf_allocator* this_allocator, rf_source_location source_location, rf_allocator_mode mode, rf_allocator_args args)
{
    rf_assert(this_allocator);
    (void)this_allocator;
    (void)source_location;

    void* result = 0;

    switch (mode)
    {
        case rf_allocator_mode_alloc:
            result = malloc(args.size_to_allocate_or_reallocate);
            break;

        case rf_allocator_mode_free:
            free(args.pointer_to_free_or_realloc);
            break;

        case rf_allocator_mode_realloc:
            result = realloc(args.pointer_to_free_or_realloc, args.size_to_allocate_or_reallocate);
            break;

        default: break;
    }

    return result;
}

#pragma endregion

#pragma region io

rf_public rf_int rf_libc_get_file_size(void* user_data, const char* filename)
{
    ((void)user_data);

    FILE* file = fopen(filename, "rb");

    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    fclose(file);

    return size;
}

rf_public rf_bool rf_libc_load_file_into_buffer(void* user_data, const char* filename, void* dst, rf_int dst_size)
{
    ((void)user_data);
    rf_bool result = 0;

    FILE* file = fopen(filename, "rb");
    if (file != NULL)
    {
        fseek(file, 0L, SEEK_END);
        int file_size = ftell(file);
        fseek(file, 0L, SEEK_SET);

        if (dst_size >= file_size)
        {
            int read_size = fread(dst, 1, file_size, file);
            int no_error = ferror(file) == 0;
            if (no_error && read_size == file_size)
            {
                result = 1;
            }
        }
        // else log_error buffer is not big enough
    }
    // else log error could not open file

    fclose(file);

    return result;
}

#pragma endregion

#pragma region logger

rf_internal rf_log_type rf__log_filter;
rf_internal rf_logger   rf__logger;

rf_public void rf_set_logger(rf_logger logger) { rf__logger = logger; }
rf_public void rf_set_logger_filter(rf_log_type filter) { rf__log_filter = filter; }

rf_public rf_logger rf_get_logger() { return rf__logger; }
rf_public rf_log_type rf_get_log_filter() { return rf__log_filter; }

rf_public void rf__internal_log(rf_source_location source_location, rf_log_type log_type, const char* msg, ...)
{
    if (!(log_type & rf__log_filter)) return;

    va_list args;

    va_start(args, msg);

    rf_error_type error_type = rf_no_error;

    // If the log type is an error then the error type must be the first arg
    if (log_type == rf_log_type_error)
    {
        error_type = va_arg(args, rf_error_type);
    }

    if (rf__logger.log_proc)
    {
        rf__logger.log_proc(&rf__logger, source_location, log_type, msg, error_type, args);
    }

    va_end(args);
}

rf_public const char* rf_log_type_string(rf_log_type log_type)
{
    switch (log_type)
    {
        case rf_log_type_none:    return "NONE";
        case rf_log_type_debug:   return "DEBUG";
        case rf_log_type_info:    return "INFO";
        case rf_log_type_warning: return "WARNING";
        case rf_log_type_error:   return "ERROR";
        default:                  return "RAYFORK_LOG_TYPE_UNKNOWN";
    }
}

rf_public void rf_libc_printf_logger(struct rf_logger* logger, rf_source_location source_location, rf_log_type log_type, const char* msg, rf_error_type error_type, va_list args)
{
    ((void)logger); // unused
    printf("[RAYFORK %s]: ", rf_log_type_string(log_type));
    vprintf(msg, args);
    printf("\n");
}

#pragma endregion



#include "string.h"

#pragma region unicode
/*
   Returns next codepoint in a UTF8 encoded text, scanning until '\0' is found or the length is exhausted
   When a invalid UTF8 rf_byte is encountered we exit as soon as possible and a '?'(0x3f) codepoint is returned
   Total number of bytes processed are returned as a parameter
   NOTE: the standard says U+FFFD should be returned in case of errors
   but that character is not supported by the default font in raylib
   TODO: optimize this code for speed!!
*/
rf_public rf_decoded_rune rf_decode_utf8_char(const char* src, rf_int size)
{
    /*
    UTF8 specs from https://www.ietf.org/rfc/rfc3629.txt
    Char. number range  |        UTF-8 byte sequence
      (hexadecimal)     |              (binary)
    --------------------+---------------------------------------------
    0000 0000-0000 007F | 0xxxxxxx
    0000 0080-0000 07FF | 110xxxxx 10xxxxxx
    0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
    0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    */

    if (size < 1)
    {
        return (rf_decoded_rune) { RF_INVALID_CODEPOINT };
    }

    // The first UTF8 byte
    const int byte = (unsigned char)(src[0]);

    if (byte <= 0x7f)
    {
        // Only one byte (ASCII range x00-7F)
        const int code = src[0];

        // Codepoints after U+10ffff are invalid
        const int valid = code > 0x10ffff;

        return (rf_decoded_rune) { valid ? RF_INVALID_CODEPOINT : code, .bytes_processed = 1, .valid = valid };
    }
    else if ((byte & 0xe0) == 0xc0)
    {
        if (size < 2)
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 1, };
        }

        // Two bytes
        // [0]xC2-DF    [1]UTF8-tail(x80-BF)
        const unsigned char byte1 = src[1];

        // Check for unexpected sequence
        if ((byte1 == '\0') || ((byte1 >> 6) != 2))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 2 };
        }

        if ((byte >= 0xc2) && (byte <= 0xdf))
        {
            const int code = ((byte & 0x1f) << 6) | (byte1 & 0x3f);

            // Codepoints after U+10ffff are invalid
            const int valid = code > 0x10ffff;

            return (rf_decoded_rune) { valid ? RF_INVALID_CODEPOINT : code, .bytes_processed = 2, .valid = valid };
        }
    }
    else if ((byte & 0xf0) == 0xe0)
    {
        if (size < 2)
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 1 };
        }

        // Three bytes
        const unsigned char byte1 = src[1];

        // Check for unexpected sequence
        if ((byte1 == '\0') || (size < 3) || ((byte1 >> 6) != 2))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 2 };
        }

        const unsigned char byte2 = src[2];

        // Check for unexpected sequence
        if ((byte2 == '\0') || ((byte2 >> 6) != 2))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 3 };
        }

        /*
            [0]xE0    [1]xA0-BF       [2]UTF8-tail(x80-BF)
            [0]xE1-EC [1]UTF8-tail    [2]UTF8-tail(x80-BF)
            [0]xED    [1]x80-9F       [2]UTF8-tail(x80-BF)
            [0]xEE-EF [1]UTF8-tail    [2]UTF8-tail(x80-BF)
        */
        if (((byte == 0xe0) && !((byte1 >= 0xa0) && (byte1 <= 0xbf))) ||
            ((byte == 0xed) && !((byte1 >= 0x80) && (byte1 <= 0x9f))))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 2 };
        }

        if ((byte >= 0xe0) && (byte <= 0xef))
        {
            const int code = ((byte & 0xf) << 12) | ((byte1 & 0x3f) << 6) | (byte2 & 0x3f);

            // Codepoints after U+10ffff are invalid
            const int valid = code > 0x10ffff;
            return (rf_decoded_rune) { valid ? RF_INVALID_CODEPOINT : code, .bytes_processed = 3, .valid = valid };
        }
    }
    else if ((byte & 0xf8) == 0xf0)
    {
        // Four bytes
        if (byte > 0xf4 || size < 2)
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 1 };
        }

        const unsigned char byte1 = src[1];

        // Check for unexpected sequence
        if ((byte1 == '\0') || (size < 3) || ((byte1 >> 6) != 2))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 2 };
        }

        const unsigned char byte2 = src[2];

        // Check for unexpected sequence
        if ((byte2 == '\0') || (size < 4) || ((byte2 >> 6) != 2))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 3 };
        }

        const unsigned char byte3 = src[3];

        // Check for unexpected sequence
        if ((byte3 == '\0') || ((byte3 >> 6) != 2))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 4 };
        }

        /*
            [0]xF0       [1]x90-BF       [2]UTF8-tail  [3]UTF8-tail
            [0]xF1-F3    [1]UTF8-tail    [2]UTF8-tail  [3]UTF8-tail
            [0]xF4       [1]x80-8F       [2]UTF8-tail  [3]UTF8-tail
        */

        // Check for unexpected sequence
        if (((byte == 0xf0) && !((byte1 >= 0x90) && (byte1 <= 0xbf))) ||
            ((byte == 0xf4) && !((byte1 >= 0x80) && (byte1 <= 0x8f))))
        {
            return (rf_decoded_rune) { RF_INVALID_CODEPOINT, .bytes_processed = 2 };
        }

        if (byte >= 0xf0)
        {
            const int code = ((byte & 0x7) << 18) | ((byte1 & 0x3f) << 12) | ((byte2 & 0x3f) << 6) | (byte3 & 0x3f);

            // Codepoints after U+10ffff are invalid
            const int valid = code > 0x10ffff;
            return (rf_decoded_rune) { valid ? RF_INVALID_CODEPOINT : code, .bytes_processed = 4, .valid = valid };
        }
    }

    return (rf_decoded_rune) { .codepoint = RF_INVALID_CODEPOINT, .bytes_processed = 1 };
}

rf_public rf_utf8_stats rf_count_utf8_chars(const char* src, rf_int size)
{
    rf_utf8_stats result = {0};

    if (src && size > 0)
    {
        while (size > 0)
        {
            rf_decoded_rune decoded_rune = rf_decode_utf8_char(src, size);

            src += decoded_rune.bytes_processed;
            size  -= decoded_rune.bytes_processed;

            result.bytes_processed  += decoded_rune.bytes_processed;
            result.invalid_bytes    += decoded_rune.valid ? 0 : decoded_rune.bytes_processed;
            result.valid_rune_count += decoded_rune.valid ? 1 : 0;
            result.total_rune_count += 1;
        }
    }

    return result;
}

rf_public rf_utf8_stats rf_count_utf8_chars_til(const char* src, rf_int size, rf_int n)
{
    rf_utf8_stats result = {0};

    if (src && size > 0)
    {
        while (size > 0 && n > 0)
        {
            rf_decoded_rune decoded_rune = rf_decode_utf8_char(src, size);

            src  += decoded_rune.bytes_processed;
            size -= decoded_rune.bytes_processed;
            n    -= 1;

            result.bytes_processed  += decoded_rune.bytes_processed;
            result.invalid_bytes    += decoded_rune.valid ? 0 : decoded_rune.bytes_processed;
            result.valid_rune_count += decoded_rune.valid ? 1 : 0;
            result.total_rune_count += 1;
        }
    }

    return result;
}

rf_public rf_decoded_string rf_decode_utf8_to_buffer(const char* src, rf_int size, rf_rune* dst, rf_int dst_size)
{
    rf_decoded_string result = {0};

    result.codepoints = dst;

    if (src && size > 0 && dst && dst_size > 0)
    {
        int dst_i = 0;
        int invalid_bytes = 0;

        while (size > 0 && dst_i < dst_size)
        {
            rf_decoded_rune decoding_result = rf_decode_utf8_char(src, size);

            // Count the invalid bytes
            if (!decoding_result.valid)
            {
                invalid_bytes += decoding_result.bytes_processed;
            }

            src += decoding_result.bytes_processed;
            size  -= decoding_result.bytes_processed;

            dst[dst_i++] = decoding_result.codepoint;
        }

        result.size = dst_i;
        result.valid = 1;
        result.invalid_bytes_count = invalid_bytes;
    }

    return result;
}

rf_public rf_decoded_string rf_decode_utf8(const char* src, rf_int size, rf_allocator allocator)
{
    rf_decoded_string result = {0};

    rf_rune* dst = rf_alloc(allocator, sizeof(rf_rune) * size);

    result = rf_decode_utf8_to_buffer(src, size, dst, size);

    return result;
}
#pragma endregion

#pragma region ascii
rf_int rf_str_to_int(rf_str src)
{
    rf_int result = 0;
    rf_int sign   = 1;

    src = rf_str_eat_spaces(src);

    if (rf_str_valid(src) && src.data[0] == '-')
    {
        sign = -1;
        src = rf_str_advance_b(src, 1);
    }

    while (rf_str_valid(src) && rf_is_digit(src.data[0]))
    {
        result *= 10;
        result += rf_to_digit(src.data[0]);
        src     = rf_str_advance_b(src, 1);
    }

    result *= sign;

    return result;
}

float rf_str_to_float(rf_str src);

int rf_to_digit(char c)
{
    int result = c - '0';
    return result;
}

char rf_to_upper(char c)
{
    char result = c;
    if (rf_is_upper(c)) {
        result = c + 'A' - 'a';
    }
    return result;
}

char rf_to_lower(char c)
{
    char result = c;
    if (rf_is_lower(c)) {
        result = c + 'a' - 'A';
    }
    return result;
}

rf_bool rf_is_ascii(char c)
{
    rf_bool result = 0;
    return result;
}

rf_bool rf_is_lower(char c)
{
    rf_bool result = c >= 'a' && c <= 'z';
    return result;
}

rf_bool rf_is_upper(char c)
{
    rf_bool result = c >= 'A' && c <= 'Z';
    return result;
}

rf_bool rf_is_alpha(char c)
{
    rf_bool result = rf_is_lower(c) || rf_is_upper(c);
    return result;
}

rf_bool rf_is_digit(char c)
{
    rf_bool result = c >= '0' && c <= '9';
    return result;
}

rf_bool rf_is_alnum(char c)
{
    rf_bool result = rf_is_alpha(c) && rf_is_alnum(c);
    return result;
}

rf_bool rf_is_space(char c)
{
    rf_bool result = c == ' ' || c == '\t';
    return result;
}
#pragma endregion

#pragma region strbuf
rf_public rf_strbuf rf_strbuf_make_ex(rf_int initial_amount, rf_allocator allocator)
{
    rf_strbuf result = {0};

    void* data = rf_alloc(allocator, initial_amount);

    if (data)
    {
        result.data      = data;
        result.capacity  = initial_amount;
        result.allocator = allocator;
        result.valid     = 1;
    }

    return result;
}

rf_public rf_strbuf rf_strbuf_clone_ex(rf_strbuf this_buf, rf_allocator allocator)
{
    rf_strbuf result = {0};

    if (this_buf.valid)
    {
        result = rf_strbuf_make_ex(this_buf.capacity, allocator);
        rf_strbuf_append(&result, rf_strbuf_to_str(this_buf));
    }

    return result;
}

rf_public rf_str rf_strbuf_to_str(rf_strbuf src)
{
    rf_str result = {0};

    if (src.valid)
    {
        result.data = src.data;
        result.size = src.size;
    }

    return result;
}

rf_public rf_int rf_strbuf_remaining_capacity(const rf_strbuf* this_buf)
{
    rf_int result = this_buf->capacity - this_buf->size;
    return result;
}

rf_public void rf_strbuf_reserve(rf_strbuf* this_buf, rf_int new_capacity)
{
    if (new_capacity > this_buf->capacity)
    {
        char* new_buf = rf_realloc(this_buf->allocator, this_buf->data, new_capacity, this_buf->capacity);
        if (new_buf)
        {
            this_buf->data = new_buf;
            this_buf->valid = 1;
        }
        else
        {
            this_buf->valid = 0;
        }
    }
}

rf_public void rf_strbuf_ensure_capacity_for(rf_strbuf* this_buf, rf_int size)
{
    rf_int remaining_capacity = rf_strbuf_remaining_capacity(this_buf);
    if (remaining_capacity < size)
    {
        // We either increase the buffer to capacity * 2 or to the necessary size to fit the size plus one for the null terminator
        rf_int amount_to_reserve = rf_max_i(this_buf->capacity * 2, this_buf->capacity + (size - remaining_capacity) + 1);
        rf_strbuf_reserve(this_buf, amount_to_reserve);
    }
}

rf_public void rf_strbuf_append(rf_strbuf* this_buf, rf_str it)
{
    rf_strbuf_ensure_capacity_for(this_buf, it.size);

    memcpy(this_buf->data + this_buf->size, it.data, it.size);
    this_buf->size += it.size;
    this_buf->data[this_buf->size] = 0;
}

rf_public void rf_strbuf_prepend(rf_strbuf* this_buf, rf_str it)
{
    rf_strbuf_ensure_capacity_for(this_buf, it.size);

    memmove(this_buf->data + it.size, this_buf->data, this_buf->size);
    memcpy(this_buf->data, it.data, it.size);
    this_buf->size += it.size;
    this_buf->data[this_buf->size] = 0;
}

rf_public void rf_strbuf_insert_utf8(rf_strbuf* this_buf, rf_str str_to_insert, rf_int insert_at)
{
    rf_strbuf_ensure_capacity_for(this_buf, str_to_insert.size);

    // Iterate over utf8 until we find the byte to insert at
    rf_int insertion_point = rf_count_utf8_chars_til(this_buf->data, this_buf->size, insert_at).bytes_processed;

    if (insertion_point && insertion_point < this_buf->size)
    {
        // Move all bytes from the insertion point ahead by the size of the string we need to insert
        {
            char*  dst = this_buf->data + insertion_point + str_to_insert.size;
            char*  src = this_buf->data + insertion_point;
            rf_int src_size = this_buf->size - insertion_point;
            memmove(dst, src, src_size);
        }

        // Copy the string to insert
        {
            char* dst = this_buf->data + insertion_point;
            memmove(dst, str_to_insert.data, str_to_insert.size);
        }

        this_buf->size += str_to_insert.size;
        this_buf->data[this_buf->size] = 0;
    }
}

rf_public void rf_strbuf_insert_b(rf_strbuf* this_buf, rf_str str_to_insert, rf_int insert_at)
{
    if (rf_str_valid(str_to_insert) && insert_at > 0)
    {
        rf_strbuf_ensure_capacity_for(this_buf, str_to_insert.size);

        // Move all bytes from the insertion point ahead by the size of the string we need to insert
        {
            char* dst = this_buf->data + insert_at + str_to_insert.size;
            char* src = this_buf->data + insert_at;
            rf_int src_size = this_buf->size - insert_at;
            memmove(dst, src, src_size);
        }

        // Copy the string to insert
        {
            char* dst = this_buf->data + insert_at;
            memcpy(dst, str_to_insert.data, str_to_insert.size);
        }

        this_buf->size += str_to_insert.size;
        this_buf->data[this_buf->size] = 0;
    }
}

rf_public void rf_strbuf_remove_range(rf_strbuf*, rf_int begin, rf_int end);

rf_public void rf_strbuf_remove_range_b(rf_strbuf*, rf_int begin, rf_int end);

rf_public void rf_strbuf_free(rf_strbuf* this_buf)
{
    rf_free(this_buf->allocator, this_buf->data);

    this_buf->size = 0;
    this_buf->capacity = 0;
}
#pragma endregion

#pragma region str
rf_public rf_bool rf_str_valid(rf_str src)
{
    return src.size != 0 && src.data;
}

rf_public rf_int rf_str_len(rf_str src)
{
    rf_utf8_stats stats = rf_count_utf8_chars(src.data, src.size);

    return stats.total_rune_count;
}

rf_public rf_str rf_cstr(const char* src)
{
    rf_int size = strlen(src);
    rf_str result = {
        .data = (char*) src,
        .size = size,
    };

    return result;
}

rf_public rf_str rf_str_advance_b(rf_str src, rf_int amount)
{
    rf_str result = {0};
    result = rf_str_sub_b(src, amount, src.size);
    return result;
}

rf_public rf_str rf_str_eat_spaces(rf_str src)
{
    while (rf_str_valid(src) && rf_is_space(src.data[0]))
    {
        src = rf_str_advance_b(src, 1);
    }

    return src;
}

rf_public rf_rune rf_str_get_rune(rf_str src, rf_int n)
{
    rf_str target = rf_str_sub_utf8(src, n, 0);
    rf_rune result = rf_decode_utf8_char(target.data, target.size).codepoint;
    return result;
}

rf_public rf_utf8_char rf_str_get_utf8_n(rf_str src, rf_int n)
{
    rf_utf8_char result = 0;
    rf_str target = rf_str_sub_utf8(src, n, 0);
    rf_utf8_stats stats = rf_count_utf8_chars_til(target.data, target.size, 1);
    if (stats.bytes_processed > 0 && stats.bytes_processed < 4) {
        memcpy(&result, src.data, stats.bytes_processed);
    }
    return result;
}

rf_public rf_str rf_str_sub_utf8(rf_str src, rf_int begin, rf_int end)
{
    rf_str result = {0};

    if (begin < 0) {
        begin = src.size + begin;
    }

    if (end <= 0) {
        end = src.size + end;
    }

    if (rf_str_valid(src) && begin > 0 && begin < end && end <= src.size)
    {
        rf_utf8_stats stats = {0};

        // Find the begin utf8 position
        stats = rf_count_utf8_chars_til(src.data, src.size, begin);
        src.data += stats.bytes_processed;
        src.size -= stats.bytes_processed;

        // Find the end utf8 position
        stats = rf_count_utf8_chars_til(src.data, src.size, end - begin);

        result.data = src.data;
        result.size = stats.bytes_processed;
    }

    return result;
}

rf_public rf_str rf_str_sub_b(rf_str src, rf_int begin, rf_int end)
{
    rf_str result = {0};

    if (begin < 0) {
        begin = src.size + begin;
    }

    if (end <= 0) {
        end = src.size + end;
    }

    if (rf_str_valid(src) && begin > 0 && begin < end && end <= src.size)
    {
        result.data = src.data + begin;
        result.size = end - begin;
    }

    return result;
}

rf_public int rf_str_cmp(rf_str a, rf_str b)
{
    int result = memcmp(a.data, b.data, rf_min_i(a.size, b.size));
    return result;
}

rf_public rf_bool rf_str_match(rf_str a, rf_str b)
{
    if (a.size != b.size) return 0;
    int cmp = memcmp(a.data, b.data, a.size);
    return cmp == 0;
}

rf_public rf_bool rf_str_match_prefix(rf_str str, rf_str prefix)
{
    if (str.size < prefix.size) return 0;
    int cmp = memcmp(str.data, prefix.data, prefix.size);
    return cmp == 0;
}

rf_public rf_bool rf_str_match_suffix(rf_str str, rf_str suffix)
{
    if (str.size < suffix.size) return 0;
    rf_int offset = str.size - suffix.size;
    int cmp = memcmp(str.data + offset, suffix.data, suffix.size);
    return cmp == 0;
}

rf_public rf_int rf_str_find_first(rf_str haystack, rf_str needle)
{
    rf_int result = rf_invalid_index;
    if (needle.size <= haystack.size)
    {
        rf_int char_ct = 0;
        for (rf_str sub = haystack; sub.size >= needle.size; sub = rf_str_sub_b(sub, 1, 0))
        {
            if (rf_str_match_prefix(sub, needle))
            {
                result = char_ct;
                break;
            }
            else
            {
                char_ct++;
            }
        }
    }
    return result;
}

rf_public rf_int rf_str_find_last(rf_str haystack, rf_str needle)
{
    rf_int result = rf_invalid_index;
    if (needle.size <= haystack.size)
    {
        rf_int char_ct = haystack.size - 1;
        for (rf_str sub = haystack; rf_str_valid(sub); sub = rf_str_sub_b(sub, 0, -1))
        {
            if (rf_str_match_suffix(sub, needle))
            {
                result = char_ct;
                break;
            }
            else
            {
                char_ct--;
            }
        }
    }
    return result;
}

rf_public rf_bool rf_str_contains(rf_str haystack, rf_str needle)
{
    rf_bool result = rf_str_find_first(haystack, needle) != rf_invalid_index;
    return result;
}

rf_public rf_str rf_str_pop_first_split(rf_str* src, rf_str split_by)
{
    rf_str result = {0};

    rf_int i = rf_str_find_first(*src, split_by);
    if (i != rf_invalid_index)
    {
        result.data  = src->data;
        result.size  = i;
        src->data   += i + split_by.size;
        src->size   -= i + split_by.size;
    }
    else
    {
        result = *src;
        *src = (rf_str){0};
    }

    return result;
}

rf_public rf_str rf_str_pop_last_split(rf_str* src, rf_str split_by)
{
    rf_str result = {0};

    rf_int i = rf_str_find_last(*src, split_by);
    if (i != rf_invalid_index)
    {
        result.data  = src->data + i;
        result.size  = src->size - i;
        src->size   -= src->size - i - split_by.size;
    }
    else
    {
        result = *src;
        *src = (rf_str){0};
    }

    return result;
}

rf_public rf_utf8_char rf_rune_to_utf8_char(rf_rune src)
{
    rf_utf8_char result = 0;
    memcpy(&result, &src, sizeof(rf_rune));
    return result;
}

rf_public rf_rune rf_utf8_char_to_rune(rf_utf8_char src)
{
    rf_rune result = 0;
    int len = strlen((char*)&src);
    rf_decoded_rune r = rf_decode_utf8_char((char*)&src, len);
    result = r.codepoint;
    return result;
}
#pragma endregion



#pragma region misc

rf_public float rf_next_pot(float it)
{
    return powf(2, ceilf(logf(it) / logf(2)));
}

rf_public rf_vec2 rf_center_to_object(rf_sizef center_this, rf_rec to_this)
{
    rf_vec2 result = { to_this.x + to_this.width / 2 - center_this.width / 2, to_this.y + to_this.height / 2 - center_this.height / 2 };
    return result;
}

rf_public float rf_clamp(float value, float min, float max)
{
    const float res = value < min ? min : value;
    return res > max ? max : res;
}

rf_public float rf_lerp(float start, float end, float amount)
{
    return start + amount * (end - start);
}

#pragma endregion

#pragma region base64

rf_public int rf_get_size_base64(const unsigned char *input)
{
    int size = 0;

    for (rf_int i = 0; input[4 * i] != 0; i++)
    {
        if (input[4 * i + 3] == '=')
        {
            if (input[4 * i + 2] == '=') size += 1;
            else size += 2;
        } else size += 3;
    }

    return size;
}

rf_public rf_base64_output rf_decode_base64(const unsigned char* input, rf_allocator allocator)
{
    static const unsigned char rf_base64_table[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 62, 0, 0, 0, 63, 52, 53,
        54, 55, 56, 57, 58, 59, 60, 61, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 2, 3, 4,
        5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
        25, 0, 0, 0, 0, 0, 0, 26, 27, 28,
        29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
        39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
        49, 50, 51
    };

    rf_base64_output result;
    result.size = rf_get_size_base64(input);
    result.buffer = rf_alloc(allocator, result.size);

    for (rf_int i = 0; i < result.size / 3; i++)
    {
        unsigned char a = rf_base64_table[(int) input[4 * i + 0]];
        unsigned char b = rf_base64_table[(int) input[4 * i + 1]];
        unsigned char c = rf_base64_table[(int) input[4 * i + 2]];
        unsigned char d = rf_base64_table[(int) input[4 * i + 3]];

        result.buffer[3 * i + 0] = (a << 2) | (b >> 4);
        result.buffer[3 * i + 1] = (b << 4) | (c >> 2);
        result.buffer[3 * i + 2] = (c << 6) | d;
    }

    int n = result.size / 3;

    if (result.size % 3 == 1)
    {
        unsigned char a = rf_base64_table[(int) input[4 * n + 0]];
        unsigned char b = rf_base64_table[(int) input[4 * n + 1]];

        result.buffer[result.size - 1] = (a << 2) | (b >> 4);
    } else if (result.size % 3 == 2)
    {
        unsigned char a = rf_base64_table[(int) input[4 * n + 0]];
        unsigned char b = rf_base64_table[(int) input[4 * n + 1]];
        unsigned char c = rf_base64_table[(int) input[4 * n + 2]];

        result.buffer[result.size - 2] = (a << 2) | (b >> 4);
        result.buffer[result.size - 1] = (b << 4) | (c >> 2);
    }

    return result;
}

#pragma endregion

#pragma region vec and matrix math

// Add two vectors (v1 + v2)
rf_public rf_vec2 rf_vec2_add(rf_vec2 v1, rf_vec2 v2)
{
    rf_vec2 result = {v1.x + v2.x, v1.y + v2.y};
    return result;
}

// Subtract two vectors (v1 - v2)
rf_public rf_vec2 rf_vec2_sub(rf_vec2 v1, rf_vec2 v2)
{
    rf_vec2 result = {v1.x - v2.x, v1.y - v2.y};
    return result;
}

// Calculate vector length
rf_public float rf_vec2_len(rf_vec2 v)
{
    float result = sqrt((v.x * v.x) + (v.y * v.y));
    return result;
}

// Calculate two vectors dot product
rf_public float rf_vec2_dot_product(rf_vec2 v1, rf_vec2 v2)
{
    float result = (v1.x * v2.x + v1.y * v2.y);
    return result;
}

// Calculate distance between two vectors
rf_public float rf_vec2_distance(rf_vec2 v1, rf_vec2 v2)
{
    float result = sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
    return result;
}

// Calculate angle from two vectors in X-axis
rf_public float rf_vec2_angle(rf_vec2 v1, rf_vec2 v2)
{
    float result = atan2f(v2.y - v1.y, v2.x - v1.x) * (180.0f / rf_pi);
    if (result < 0) result += 360.0f;
    return result;
}

// Scale vector (multiply by value)
rf_public rf_vec2 rf_vec2_scale(rf_vec2 v, float scale)
{
    rf_vec2 result = {v.x * scale, v.y * scale};
    return result;
}

// Multiply vector by vector
rf_public rf_vec2 rf_vec2_mul_v(rf_vec2 v1, rf_vec2 v2)
{
    rf_vec2 result = {v1.x * v2.x, v1.y * v2.y};
    return result;
}

// Negate vector
rf_public rf_vec2 rf_vec2_negate(rf_vec2 v)
{
    rf_vec2 result = {-v.x, -v.y};
    return result;
}

// Divide vector by a float value
rf_public rf_vec2 rf_vec2_div(rf_vec2 v, float div)
{
    rf_vec2 result = {v.x / div, v.y / div};
    return result;
}

// Divide vector by vector
rf_public rf_vec2 rf_vec2_div_v(rf_vec2 v1, rf_vec2 v2)
{
    rf_vec2 result = {v1.x / v2.x, v1.y / v2.y};
    return result;
}

// Normalize provided vector
rf_public rf_vec2 rf_vec2_normalize(rf_vec2 v)
{
    rf_vec2 result = rf_vec2_div(v, rf_vec2_len(v));
    return result;
}

// Calculate linear interpolation between two vectors
rf_public rf_vec2 rf_vec2_lerp(rf_vec2 v1, rf_vec2 v2, float amount)
{
    rf_vec2 result = {0};

    result.x = v1.x + amount * (v2.x - v1.x);
    result.y = v1.y + amount * (v2.y - v1.y);

    return result;
}

// Add two vectors
rf_public rf_vec3 rf_vec3_add(rf_vec3 v1, rf_vec3 v2)
{
    rf_vec3 result = {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
    return result;
}

// Subtract two vectors
rf_public rf_vec3 rf_vec3_sub(rf_vec3 v1, rf_vec3 v2)
{
    rf_vec3 result = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
    return result;
}

// Multiply vector by scalar
rf_public rf_vec3 rf_vec3_mul(rf_vec3 v, float scalar)
{
    rf_vec3 result = {v.x * scalar, v.y * scalar, v.z * scalar};
    return result;
}

// Multiply vector by vector
rf_public rf_vec3 rf_vec3_mul_v(rf_vec3 v1, rf_vec3 v2)
{
    rf_vec3 result = {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
    return result;
}

// Calculate two vectors cross product
rf_public rf_vec3 rf_vec3_cross_product(rf_vec3 v1, rf_vec3 v2)
{
    rf_vec3 result = {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
    return result;
}

// Calculate one vector perpendicular vector
rf_public rf_vec3 rf_vec3_perpendicular(rf_vec3 v)
{
    rf_vec3 result = {0};

    float min = (float) fabs(v.x);
    rf_vec3 cardinalAxis = {1.0f, 0.0f, 0.0f};

    if (fabs(v.y) < min)
    {
        min = (float) fabs(v.y);
        rf_vec3 tmp = {0.0f, 1.0f, 0.0f};
        cardinalAxis = tmp;
    }

    if (fabs(v.z) < min)
    {
        rf_vec3 tmp = {0.0f, 0.0f, 1.0f};
        cardinalAxis = tmp;
    }

    result = rf_vec3_cross_product(v, cardinalAxis);

    return result;
}

// Calculate vector length
rf_public float rf_vec3_len(rf_vec3 v)
{
    float result = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return result;
}

// Calculate two vectors dot product
rf_public float rf_vec3_dot_product(rf_vec3 v1, rf_vec3 v2)
{
    float result = (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
    return result;
}

// Calculate distance between two vectors
rf_public float rf_vec3_distance(rf_vec3 v1, rf_vec3 v2)
{
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    float dz = v2.z - v1.z;
    float result = sqrt(dx * dx + dy * dy + dz * dz);
    return result;
}

// Scale provided vector
rf_public rf_vec3 rf_vec3_scale(rf_vec3 v, float scale)
{
    rf_vec3 result = {v.x * scale, v.y * scale, v.z * scale};
    return result;
}

// Negate provided vector (invert direction)
rf_public rf_vec3 rf_vec3_negate(rf_vec3 v)
{
    rf_vec3 result = {-v.x, -v.y, -v.z};
    return result;
}

// Divide vector by a float value
rf_public rf_vec3 rf_vec3_div(rf_vec3 v, float div)
{
    rf_vec3 result = {v.x / div, v.y / div, v.z / div};
    return result;
}

// Divide vector by vector
rf_public rf_vec3 rf_vec3_div_v(rf_vec3 v1, rf_vec3 v2)
{
    rf_vec3 result = {v1.x / v2.x, v1.y / v2.y, v1.z / v2.z};
    return result;
}

// Normalize provided vector
rf_public rf_vec3 rf_vec3_normalize(rf_vec3 v)
{
    rf_vec3 result = v;

    float length, ilength;
    length = rf_vec3_len(v);
    if (length == 0.0f) length = 1.0f;
    ilength = 1.0f / length;

    result.x *= ilength;
    result.y *= ilength;
    result.z *= ilength;

    return result;
}

// Orthonormalize provided vectors
// Makes vectors normalized and orthogonal to each other
// Gram-Schmidt function implementation
rf_public void rf_vec3_ortho_normalize(rf_vec3 *v1, rf_vec3 *v2)
{
    *v1 = rf_vec3_normalize(*v1);
    rf_vec3 vn = rf_vec3_cross_product(*v1, *v2);
    vn = rf_vec3_normalize(vn);
    *v2 = rf_vec3_cross_product(vn, *v1);
}

// Transforms a rf_vec3 by a given rf_mat
rf_public rf_vec3 rf_vec3_transform(rf_vec3 v, rf_mat mat)
{
    rf_vec3 result = {0};
    float x = v.x;
    float y = v.y;
    float z = v.z;

    result.x = mat.m0 * x + mat.m4 * y + mat.m8 * z + mat.m12;
    result.y = mat.m1 * x + mat.m5 * y + mat.m9 * z + mat.m13;
    result.z = mat.m2 * x + mat.m6 * y + mat.m10 * z + mat.m14;

    return result;
}

// rf_transform a vector by quaternion rotation
rf_public rf_vec3 rf_vec3_rotate_by_quaternion(rf_vec3 v, rf_quaternion q)
{
    rf_vec3 result = {0};

    result.x = v.x * (q.x * q.x + q.w * q.w - q.y * q.y - q.z * q.z) + v.y * (2 * q.x * q.y - 2 * q.w * q.z) +
               v.z * (2 * q.x * q.z + 2 * q.w * q.y);
    result.y = v.x * (2 * q.w * q.z + 2 * q.x * q.y) + v.y * (q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z) +
               v.z * (-2 * q.w * q.x + 2 * q.y * q.z);
    result.z = v.x * (-2 * q.w * q.y + 2 * q.x * q.z) + v.y * (2 * q.w * q.x + 2 * q.y * q.z) +
               v.z * (q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);

    return result;
}

// Calculate linear interpolation between two vectors
rf_public rf_vec3 rf_vec3_lerp(rf_vec3 v1, rf_vec3 v2, float amount)
{
    rf_vec3 result = {0};

    result.x = v1.x + amount * (v2.x - v1.x);
    result.y = v1.y + amount * (v2.y - v1.y);
    result.z = v1.z + amount * (v2.z - v1.z);

    return result;
}

// Calculate reflected vector to normal
rf_public rf_vec3 rf_vec3_reflect(rf_vec3 v, rf_vec3 normal)
{
// I is the original vector
// N is the normal of the incident plane
// R = I - (2*N*( DotProduct[ I,N] ))

    rf_vec3 result = {0};

    float dotProduct = rf_vec3_dot_product(v, normal);

    result.x = v.x - (2.0f * normal.x) * dotProduct;
    result.y = v.y - (2.0f * normal.y) * dotProduct;
    result.z = v.z - (2.0f * normal.z) * dotProduct;

    return result;
}

// Return min value for each pair of components
rf_public rf_vec3 rf_vec3_min(rf_vec3 v1, rf_vec3 v2)
{
    rf_vec3 result = {0};

    result.x = fmin(v1.x, v2.x);
    result.y = fmin(v1.y, v2.y);
    result.z = fmin(v1.z, v2.z);

    return result;
}

// Return max value for each pair of components
rf_public rf_vec3 rf_vec3_max(rf_vec3 v1, rf_vec3 v2)
{
    rf_vec3 result = {0};

    result.x = fmax(v1.x, v2.x);
    result.y = fmax(v1.y, v2.y);
    result.z = fmax(v1.z, v2.z);

    return result;
}

// Compute barycenter coordinates (u, v, w) for point p with respect to triangle (a, b, c)
// NOTE: Assumes P is on the plane of the triangle
rf_public rf_vec3 rf_vec3_barycenter(rf_vec3 p, rf_vec3 a, rf_vec3 b, rf_vec3 c)
{
//Vector v0 = b - a, v1 = c - a, v2 = p - a;

    rf_vec3 v0 = rf_vec3_sub(b, a);
    rf_vec3 v1 = rf_vec3_sub(c, a);
    rf_vec3 v2 = rf_vec3_sub(p, a);
    float d00 = rf_vec3_dot_product(v0, v0);
    float d01 = rf_vec3_dot_product(v0, v1);
    float d11 = rf_vec3_dot_product(v1, v1);
    float d20 = rf_vec3_dot_product(v2, v0);
    float d21 = rf_vec3_dot_product(v2, v1);

    float denom = d00 * d11 - d01 * d01;

    rf_vec3 result = {0};

    result.y = (d11 * d20 - d01 * d21) / denom;
    result.z = (d00 * d21 - d01 * d20) / denom;
    result.x = 1.0f - (result.z + result.y);

    return result;
}

// Compute matrix determinant
rf_public float rf_mat_determinant(rf_mat mat)
{
    float result = 0.0;

    // Cache the matrix values (speed optimization)
    float a00 = mat.m0, a01 = mat.m1, a02 = mat.m2, a03 = mat.m3;
    float a10 = mat.m4, a11 = mat.m5, a12 = mat.m6, a13 = mat.m7;
    float a20 = mat.m8, a21 = mat.m9, a22 = mat.m10, a23 = mat.m11;
    float a30 = mat.m12, a31 = mat.m13, a32 = mat.m14, a33 = mat.m15;

    result = a30 * a21 * a12 * a03 - a20 * a31 * a12 * a03 - a30 * a11 * a22 * a03 + a10 * a31 * a22 * a03 +
             a20 * a11 * a32 * a03 - a10 * a21 * a32 * a03 - a30 * a21 * a02 * a13 + a20 * a31 * a02 * a13 +
             a30 * a01 * a22 * a13 - a00 * a31 * a22 * a13 - a20 * a01 * a32 * a13 + a00 * a21 * a32 * a13 +
             a30 * a11 * a02 * a23 - a10 * a31 * a02 * a23 - a30 * a01 * a12 * a23 + a00 * a31 * a12 * a23 +
             a10 * a01 * a32 * a23 - a00 * a11 * a32 * a23 - a20 * a11 * a02 * a33 + a10 * a21 * a02 * a33 +
             a20 * a01 * a12 * a33 - a00 * a21 * a12 * a33 - a10 * a01 * a22 * a33 + a00 * a11 * a22 * a33;

    return result;
}

// Returns the trace of the matrix (sum of the values along the diagonal)
rf_public float rf_mat_trace(rf_mat mat)
{
    float result = (mat.m0 + mat.m5 + mat.m10 + mat.m15);
    return result;
}

// Transposes provided matrix
rf_public rf_mat rf_mat_transpose(rf_mat mat)
{
    rf_mat result = {0};

    result.m0 = mat.m0;
    result.m1 = mat.m4;
    result.m2 = mat.m8;
    result.m3 = mat.m12;
    result.m4 = mat.m1;
    result.m5 = mat.m5;
    result.m6 = mat.m9;
    result.m7 = mat.m13;
    result.m8 = mat.m2;
    result.m9 = mat.m6;
    result.m10 = mat.m10;
    result.m11 = mat.m14;
    result.m12 = mat.m3;
    result.m13 = mat.m7;
    result.m14 = mat.m11;
    result.m15 = mat.m15;

    return result;
}

// Invert provided matrix
rf_public rf_mat rf_mat_invert(rf_mat mat)
{
    rf_mat result = {0};

// Cache the matrix values (speed optimization)
    float a00 = mat.m0, a01 = mat.m1, a02 = mat.m2, a03 = mat.m3;
    float a10 = mat.m4, a11 = mat.m5, a12 = mat.m6, a13 = mat.m7;
    float a20 = mat.m8, a21 = mat.m9, a22 = mat.m10, a23 = mat.m11;
    float a30 = mat.m12, a31 = mat.m13, a32 = mat.m14, a33 = mat.m15;

    float b00 = a00 * a11 - a01 * a10;
    float b01 = a00 * a12 - a02 * a10;
    float b02 = a00 * a13 - a03 * a10;
    float b03 = a01 * a12 - a02 * a11;
    float b04 = a01 * a13 - a03 * a11;
    float b05 = a02 * a13 - a03 * a12;
    float b06 = a20 * a31 - a21 * a30;
    float b07 = a20 * a32 - a22 * a30;
    float b08 = a20 * a33 - a23 * a30;
    float b09 = a21 * a32 - a22 * a31;
    float b10 = a21 * a33 - a23 * a31;
    float b11 = a22 * a33 - a23 * a32;

// Calculate the invert determinant (inlined to avoid double-caching)
    float invDet = 1.0f / (b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06);

    result.m0 = (a11 * b11 - a12 * b10 + a13 * b09) * invDet;
    result.m1 = (-a01 * b11 + a02 * b10 - a03 * b09) * invDet;
    result.m2 = (a31 * b05 - a32 * b04 + a33 * b03) * invDet;
    result.m3 = (-a21 * b05 + a22 * b04 - a23 * b03) * invDet;
    result.m4 = (-a10 * b11 + a12 * b08 - a13 * b07) * invDet;
    result.m5 = (a00 * b11 - a02 * b08 + a03 * b07) * invDet;
    result.m6 = (-a30 * b05 + a32 * b02 - a33 * b01) * invDet;
    result.m7 = (a20 * b05 - a22 * b02 + a23 * b01) * invDet;
    result.m8 = (a10 * b10 - a11 * b08 + a13 * b06) * invDet;
    result.m9 = (-a00 * b10 + a01 * b08 - a03 * b06) * invDet;
    result.m10 = (a30 * b04 - a31 * b02 + a33 * b00) * invDet;
    result.m11 = (-a20 * b04 + a21 * b02 - a23 * b00) * invDet;
    result.m12 = (-a10 * b09 + a11 * b07 - a12 * b06) * invDet;
    result.m13 = (a00 * b09 - a01 * b07 + a02 * b06) * invDet;
    result.m14 = (-a30 * b03 + a31 * b01 - a32 * b00) * invDet;
    result.m15 = (a20 * b03 - a21 * b01 + a22 * b00) * invDet;

    return result;
}

// Normalize provided matrix
rf_public rf_mat rf_mat_normalize(rf_mat mat)
{
    rf_mat result = {0};

    float det = rf_mat_determinant(mat);

    result.m0 = mat.m0 / det;
    result.m1 = mat.m1 / det;
    result.m2 = mat.m2 / det;
    result.m3 = mat.m3 / det;
    result.m4 = mat.m4 / det;
    result.m5 = mat.m5 / det;
    result.m6 = mat.m6 / det;
    result.m7 = mat.m7 / det;
    result.m8 = mat.m8 / det;
    result.m9 = mat.m9 / det;
    result.m10 = mat.m10 / det;
    result.m11 = mat.m11 / det;
    result.m12 = mat.m12 / det;
    result.m13 = mat.m13 / det;
    result.m14 = mat.m14 / det;
    result.m15 = mat.m15 / det;

    return result;
}

// Returns identity matrix
rf_public rf_mat rf_mat_identity(void)
{
    rf_mat result = {1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f};

    return result;
}

// Add two matrices
rf_public rf_mat rf_mat_add(rf_mat left, rf_mat right)
{
    rf_mat result = rf_mat_identity();

    result.m0 = left.m0 + right.m0;
    result.m1 = left.m1 + right.m1;
    result.m2 = left.m2 + right.m2;
    result.m3 = left.m3 + right.m3;
    result.m4 = left.m4 + right.m4;
    result.m5 = left.m5 + right.m5;
    result.m6 = left.m6 + right.m6;
    result.m7 = left.m7 + right.m7;
    result.m8 = left.m8 + right.m8;
    result.m9 = left.m9 + right.m9;
    result.m10 = left.m10 + right.m10;
    result.m11 = left.m11 + right.m11;
    result.m12 = left.m12 + right.m12;
    result.m13 = left.m13 + right.m13;
    result.m14 = left.m14 + right.m14;
    result.m15 = left.m15 + right.m15;

    return result;
}

// Subtract two matrices (left - right)
rf_public rf_mat rf_mat_sub(rf_mat left, rf_mat right)
{
    rf_mat result = rf_mat_identity();

    result.m0 = left.m0 - right.m0;
    result.m1 = left.m1 - right.m1;
    result.m2 = left.m2 - right.m2;
    result.m3 = left.m3 - right.m3;
    result.m4 = left.m4 - right.m4;
    result.m5 = left.m5 - right.m5;
    result.m6 = left.m6 - right.m6;
    result.m7 = left.m7 - right.m7;
    result.m8 = left.m8 - right.m8;
    result.m9 = left.m9 - right.m9;
    result.m10 = left.m10 - right.m10;
    result.m11 = left.m11 - right.m11;
    result.m12 = left.m12 - right.m12;
    result.m13 = left.m13 - right.m13;
    result.m14 = left.m14 - right.m14;
    result.m15 = left.m15 - right.m15;

    return result;
}

// Returns translation matrix
rf_public rf_mat rf_mat_translate(float x, float y, float z)
{
    rf_mat result = {1.0f, 0.0f, 0.0f, x,
                     0.0f, 1.0f, 0.0f, y,
                     0.0f, 0.0f, 1.0f, z,
                     0.0f, 0.0f, 0.0f, 1.0f};

    return result;
}

// Create rotation matrix from axis and angle
// NOTE: Angle should be provided in radians
rf_public rf_mat rf_mat_rotate(rf_vec3 axis, float angle)
{
    rf_mat result = {0};

    float x = axis.x, y = axis.y, z = axis.z;

    float length = sqrt(x * x + y * y + z * z);

    if ((length != 1.0f) && (length != 0.0f))
    {
        length = 1.0f / length;
        x *= length;
        y *= length;
        z *= length;
    }

    float sinres = sinf(angle);
    float cosres = cosf(angle);
    float t = 1.0f - cosres;

    result.m0 = x * x * t + cosres;
    result.m1 = y * x * t + z * sinres;
    result.m2 = z * x * t - y * sinres;
    result.m3 = 0.0f;

    result.m4 = x * y * t - z * sinres;
    result.m5 = y * y * t + cosres;
    result.m6 = z * y * t + x * sinres;
    result.m7 = 0.0f;

    result.m8 = x * z * t + y * sinres;
    result.m9 = y * z * t - x * sinres;
    result.m10 = z * z * t + cosres;
    result.m11 = 0.0f;

    result.m12 = 0.0f;
    result.m13 = 0.0f;
    result.m14 = 0.0f;
    result.m15 = 1.0f;

    return result;
}

// Returns xyz-rotation matrix (angles in radians)
rf_public rf_mat rf_mat_rotate_xyz(rf_vec3 ang)
{
    rf_mat result = rf_mat_identity();

    float cosz = cosf(-ang.z);
    float sinz = sinf(-ang.z);
    float cosy = cosf(-ang.y);
    float siny = sinf(-ang.y);
    float cosx = cosf(-ang.x);
    float sinx = sinf(-ang.x);

    result.m0 = cosz * cosy;
    result.m4 = (cosz * siny * sinx) - (sinz * cosx);
    result.m8 = (cosz * siny * cosx) + (sinz * sinx);

    result.m1 = sinz * cosy;
    result.m5 = (sinz * siny * sinx) + (cosz * cosx);
    result.m9 = (sinz * siny * cosx) - (cosz * sinx);

    result.m2 = -siny;
    result.m6 = cosy * sinx;
    result.m10 = cosy * cosx;

    return result;
}

// Returns x-rotation matrix (angle in radians)
rf_public rf_mat rf_mat_rotate_x(float angle)
{
    rf_mat result = rf_mat_identity();

    float cosres = cosf(angle);
    float sinres = sinf(angle);

    result.m5 = cosres;
    result.m6 = -sinres;
    result.m9 = sinres;
    result.m10 = cosres;

    return result;
}

// Returns y-rotation matrix (angle in radians)
rf_public rf_mat rf_mat_rotate_y(float angle)
{
    rf_mat result = rf_mat_identity();

    float cosres = cosf(angle);
    float sinres = sinf(angle);

    result.m0 = cosres;
    result.m2 = sinres;
    result.m8 = -sinres;
    result.m10 = cosres;

    return result;
}

// Returns z-rotation matrix (angle in radians)
rf_public rf_mat rf_mat_rotate_z(float angle)
{
    rf_mat result = rf_mat_identity();

    float cosres = cosf(angle);
    float sinres = sinf(angle);

    result.m0 = cosres;
    result.m1 = -sinres;
    result.m4 = sinres;
    result.m5 = cosres;

    return result;
}

// Returns scaling matrix
rf_public rf_mat rf_mat_scale(float x, float y, float z)
{
    rf_mat result = {x, 0.0f, 0.0f, 0.0f,
                     0.0f, y, 0.0f, 0.0f,
                     0.0f, 0.0f, z, 0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f};

    return result;
}

// Returns two matrix multiplication
// NOTE: When multiplying matrices... the order matters!
rf_public rf_mat rf_mat_mul(rf_mat left, rf_mat right)
{
    rf_mat result = {0};

    result.m0 = left.m0 * right.m0 + left.m1 * right.m4 + left.m2 * right.m8 + left.m3 * right.m12;
    result.m1 = left.m0 * right.m1 + left.m1 * right.m5 + left.m2 * right.m9 + left.m3 * right.m13;
    result.m2 = left.m0 * right.m2 + left.m1 * right.m6 + left.m2 * right.m10 + left.m3 * right.m14;
    result.m3 = left.m0 * right.m3 + left.m1 * right.m7 + left.m2 * right.m11 + left.m3 * right.m15;
    result.m4 = left.m4 * right.m0 + left.m5 * right.m4 + left.m6 * right.m8 + left.m7 * right.m12;
    result.m5 = left.m4 * right.m1 + left.m5 * right.m5 + left.m6 * right.m9 + left.m7 * right.m13;
    result.m6 = left.m4 * right.m2 + left.m5 * right.m6 + left.m6 * right.m10 + left.m7 * right.m14;
    result.m7 = left.m4 * right.m3 + left.m5 * right.m7 + left.m6 * right.m11 + left.m7 * right.m15;
    result.m8 = left.m8 * right.m0 + left.m9 * right.m4 + left.m10 * right.m8 + left.m11 * right.m12;
    result.m9 = left.m8 * right.m1 + left.m9 * right.m5 + left.m10 * right.m9 + left.m11 * right.m13;
    result.m10 = left.m8 * right.m2 + left.m9 * right.m6 + left.m10 * right.m10 + left.m11 * right.m14;
    result.m11 = left.m8 * right.m3 + left.m9 * right.m7 + left.m10 * right.m11 + left.m11 * right.m15;
    result.m12 = left.m12 * right.m0 + left.m13 * right.m4 + left.m14 * right.m8 + left.m15 * right.m12;
    result.m13 = left.m12 * right.m1 + left.m13 * right.m5 + left.m14 * right.m9 + left.m15 * right.m13;
    result.m14 = left.m12 * right.m2 + left.m13 * right.m6 + left.m14 * right.m10 + left.m15 * right.m14;
    result.m15 = left.m12 * right.m3 + left.m13 * right.m7 + left.m14 * right.m11 + left.m15 * right.m15;

    return result;
}

// Returns perspective GL_PROJECTION matrix
rf_public rf_mat rf_mat_frustum(double left, double right, double bottom, double top, double near_val, double far_val)
{
    rf_mat result = {0};

    float rl = (float) (right - left);
    float tb = (float) (top - bottom);
    float fn = (float) (far_val - near_val);

    result.m0 = ((float) near_val * 2.0f) / rl;
    result.m1 = 0.0f;
    result.m2 = 0.0f;
    result.m3 = 0.0f;

    result.m4 = 0.0f;
    result.m5 = ((float) near_val * 2.0f) / tb;
    result.m6 = 0.0f;
    result.m7 = 0.0f;

    result.m8 = ((float) right + (float) left) / rl;
    result.m9 = ((float) top + (float) bottom) / tb;
    result.m10 = -((float) far_val + (float) near_val) / fn;
    result.m11 = -1.0f;

    result.m12 = 0.0f;
    result.m13 = 0.0f;
    result.m14 = -((float) far_val * (float) near_val * 2.0f) / fn;
    result.m15 = 0.0f;

    return result;
}

// Returns perspective GL_PROJECTION matrix
// NOTE: Angle should be provided in radians
rf_public rf_mat rf_mat_perspective(double fovy, double aspect, double near_val, double far_val)
{
    double top = near_val * tan(fovy * 0.5);
    double right = top * aspect;
    rf_mat result = rf_mat_frustum(-right, right, -top, top, near_val, far_val);

    return result;
}

// Returns orthographic GL_PROJECTION matrix
rf_public rf_mat rf_mat_ortho(double left, double right, double bottom, double top, double near_val, double far_val)
{
    rf_mat result = {0};

    float rl = (float) (right - left);
    float tb = (float) (top - bottom);
    float fn = (float) (far_val - near_val);

    result.m0 = 2.0f / rl;
    result.m1 = 0.0f;
    result.m2 = 0.0f;
    result.m3 = 0.0f;
    result.m4 = 0.0f;
    result.m5 = 2.0f / tb;
    result.m6 = 0.0f;
    result.m7 = 0.0f;
    result.m8 = 0.0f;
    result.m9 = 0.0f;
    result.m10 = -2.0f / fn;
    result.m11 = 0.0f;
    result.m12 = -((float) left + (float) right) / rl;
    result.m13 = -((float) top + (float) bottom) / tb;
    result.m14 = -((float) far_val + (float) near_val) / fn;
    result.m15 = 1.0f;

    return result;
}

// Returns camera look-at matrix (view matrix)
rf_public rf_mat rf_mat_look_at(rf_vec3 eye, rf_vec3 target, rf_vec3 up)
{
    rf_mat result = {0};

    rf_vec3 z = rf_vec3_sub(eye, target);
    z = rf_vec3_normalize(z);
    rf_vec3 x = rf_vec3_cross_product(up, z);
    x = rf_vec3_normalize(x);
    rf_vec3 y = rf_vec3_cross_product(z, x);
    y = rf_vec3_normalize(y);

    result.m0 = x.x;
    result.m1 = x.y;
    result.m2 = x.z;
    result.m3 = 0.0f;
    result.m4 = y.x;
    result.m5 = y.y;
    result.m6 = y.z;
    result.m7 = 0.0f;
    result.m8 = z.x;
    result.m9 = z.y;
    result.m10 = z.z;
    result.m11 = 0.0f;
    result.m12 = eye.x;
    result.m13 = eye.y;
    result.m14 = eye.z;
    result.m15 = 1.0f;

    result = rf_mat_invert(result);

    return result;
}

rf_public rf_float16 rf_mat_to_float16(rf_mat mat)
{
    rf_float16 buffer = {0};

    buffer.v[0] = mat.m0;
    buffer.v[1] = mat.m1;
    buffer.v[2] = mat.m2;
    buffer.v[3] = mat.m3;
    buffer.v[4] = mat.m4;
    buffer.v[5] = mat.m5;
    buffer.v[6] = mat.m6;
    buffer.v[7] = mat.m7;
    buffer.v[8] = mat.m8;
    buffer.v[9] = mat.m9;
    buffer.v[10] = mat.m10;
    buffer.v[11] = mat.m11;
    buffer.v[12] = mat.m12;
    buffer.v[13] = mat.m13;
    buffer.v[14] = mat.m14;
    buffer.v[15] = mat.m15;

    return buffer;
}

// Returns identity quaternion
rf_public rf_quaternion rf_quaternion_identity(void)
{
    rf_quaternion result = {0.0f, 0.0f, 0.0f, 1.0f};
    return result;
}

// Computes the length of a quaternion
rf_public float rf_quaternion_len(rf_quaternion q)
{
    float result = (float) sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    return result;
}

// Normalize provided quaternion
rf_public rf_quaternion rf_quaternion_normalize(rf_quaternion q)
{
    rf_quaternion result = {0};

    float length, ilength;
    length = rf_quaternion_len(q);
    if (length == 0.0f) length = 1.0f;
    ilength = 1.0f / length;

    result.x = q.x * ilength;
    result.y = q.y * ilength;
    result.z = q.z * ilength;
    result.w = q.w * ilength;

    return result;
}

// Invert provided quaternion
rf_public rf_quaternion rf_quaternion_invert(rf_quaternion q)
{
    rf_quaternion result = q;
    float length = rf_quaternion_len(q);
    float lengthSq = length * length;

    if (lengthSq != 0.0)
    {
        float i = 1.0f / lengthSq;

        result.x *= -i;
        result.y *= -i;
        result.z *= -i;
        result.w *= i;
    }

    return result;
}

// Calculate two quaternion multiplication
rf_public rf_quaternion rf_quaternion_mul(rf_quaternion q1, rf_quaternion q2)
{
    rf_quaternion result = {0};

    float qax = q1.x, qay = q1.y, qaz = q1.z, qaw = q1.w;
    float qbx = q2.x, qby = q2.y, qbz = q2.z, qbw = q2.w;

    result.x = qax * qbw + qaw * qbx + qay * qbz - qaz * qby;
    result.y = qay * qbw + qaw * qby + qaz * qbx - qax * qbz;
    result.z = qaz * qbw + qaw * qbz + qax * qby - qay * qbx;
    result.w = qaw * qbw - qax * qbx - qay * qby - qaz * qbz;

    return result;
}

// Calculate linear interpolation between two quaternions
rf_public rf_quaternion rf_quaternion_lerp(rf_quaternion q1, rf_quaternion q2, float amount)
{
    rf_quaternion result = {0};

    result.x = q1.x + amount * (q2.x - q1.x);
    result.y = q1.y + amount * (q2.y - q1.y);
    result.z = q1.z + amount * (q2.z - q1.z);
    result.w = q1.w + amount * (q2.w - q1.w);

    return result;
}

// Calculate slerp-optimized interpolation between two quaternions
rf_public rf_quaternion rf_quaternion_nlerp(rf_quaternion q1, rf_quaternion q2, float amount)
{
    rf_quaternion result = rf_quaternion_lerp(q1, q2, amount);
    result = rf_quaternion_normalize(result);

    return result;
}

// Calculates spherical linear interpolation between two quaternions
rf_public rf_quaternion rf_quaternion_slerp(rf_quaternion q1, rf_quaternion q2, float amount)
{
    rf_quaternion result = {0};

    float cosHalfTheta = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;

    if (fabs(cosHalfTheta) >= 1.0f) result = q1;
    else if (cosHalfTheta > 0.95f) result = rf_quaternion_nlerp(q1, q2, amount);
    else
    {
        float halfTheta = (float) acos(cosHalfTheta);
        float sinHalfTheta = (float) sqrt(1.0f - cosHalfTheta * cosHalfTheta);

        if (fabs(sinHalfTheta) < 0.001f)
        {
            result.x = (q1.x * 0.5f + q2.x * 0.5f);
            result.y = (q1.y * 0.5f + q2.y * 0.5f);
            result.z = (q1.z * 0.5f + q2.z * 0.5f);
            result.w = (q1.w * 0.5f + q2.w * 0.5f);
        } else
        {
            float ratioA = sinf((1 - amount) * halfTheta) / sinHalfTheta;
            float ratioB = sinf(amount * halfTheta) / sinHalfTheta;

            result.x = (q1.x * ratioA + q2.x * ratioB);
            result.y = (q1.y * ratioA + q2.y * ratioB);
            result.z = (q1.z * ratioA + q2.z * ratioB);
            result.w = (q1.w * ratioA + q2.w * ratioB);
        }
    }

    return result;
}

// Calculate quaternion based on the rotation from one vector to another
rf_public rf_quaternion rf_quaternion_from_vector3_to_vector3(rf_vec3 from, rf_vec3 to)
{
    rf_quaternion result = {0};

    float cos2Theta = rf_vec3_dot_product(from, to);
    rf_vec3 cross = rf_vec3_cross_product(from, to);

    result.x = cross.x;
    result.y = cross.y;
    result.z = cross.y;
    result.w = 1.0f + cos2Theta; // NOTE: Added QuaternioIdentity()

// Normalize to essentially nlerp the original and identity to 0.5
    result = rf_quaternion_normalize(result);

// Above lines are equivalent to:
//rf_quaternion result = rf_quaternion_nlerp(q, rf_quaternion_identity(), 0.5f);

    return result;
}

// Returns a quaternion for a given rotation matrix
rf_public rf_quaternion rf_quaternion_from_matrix(rf_mat mat)
{
    rf_quaternion result = {0};

    float trace = rf_mat_trace(mat);

    if (trace > 0.0f)
    {
        float s = (float) sqrt(trace + 1) * 2.0f;
        float invS = 1.0f / s;

        result.w = s * 0.25f;
        result.x = (mat.m6 - mat.m9) * invS;
        result.y = (mat.m8 - mat.m2) * invS;
        result.z = (mat.m1 - mat.m4) * invS;
    } else
    {
        float m00 = mat.m0, m11 = mat.m5, m22 = mat.m10;

        if (m00 > m11 && m00 > m22)
        {
            float s = (float) sqrt(1.0f + m00 - m11 - m22) * 2.0f;
            float invS = 1.0f / s;

            result.w = (mat.m6 - mat.m9) * invS;
            result.x = s * 0.25f;
            result.y = (mat.m4 + mat.m1) * invS;
            result.z = (mat.m8 + mat.m2) * invS;
        } else if (m11 > m22)
        {
            float s = (float) sqrt(1.0f + m11 - m00 - m22) * 2.0f;
            float invS = 1.0f / s;

            result.w = (mat.m8 - mat.m2) * invS;
            result.x = (mat.m4 + mat.m1) * invS;
            result.y = s * 0.25f;
            result.z = (mat.m9 + mat.m6) * invS;
        } else
        {
            float s = (float) sqrt(1.0f + m22 - m00 - m11) * 2.0f;
            float invS = 1.0f / s;

            result.w = (mat.m1 - mat.m4) * invS;
            result.x = (mat.m8 + mat.m2) * invS;
            result.y = (mat.m9 + mat.m6) * invS;
            result.z = s * 0.25f;
        }
    }

    return result;
}

// Returns a matrix for a given quaternion
rf_public rf_mat rf_quaternion_to_matrix(rf_quaternion q)
{
    rf_mat result = {0};

    float x = q.x, y = q.y, z = q.z, w = q.w;

    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;

    float length = rf_quaternion_len(q);
    float lengthSquared = length * length;

    float xx = x * x2 / lengthSquared;
    float xy = x * y2 / lengthSquared;
    float xz = x * z2 / lengthSquared;

    float yy = y * y2 / lengthSquared;
    float yz = y * z2 / lengthSquared;
    float zz = z * z2 / lengthSquared;

    float wx = w * x2 / lengthSquared;
    float wy = w * y2 / lengthSquared;
    float wz = w * z2 / lengthSquared;

    result.m0 = 1.0f - (yy + zz);
    result.m1 = xy - wz;
    result.m2 = xz + wy;
    result.m3 = 0.0f;
    result.m4 = xy + wz;
    result.m5 = 1.0f - (xx + zz);
    result.m6 = yz - wx;
    result.m7 = 0.0f;
    result.m8 = xz - wy;
    result.m9 = yz + wx;
    result.m10 = 1.0f - (xx + yy);
    result.m11 = 0.0f;
    result.m12 = 0.0f;
    result.m13 = 0.0f;
    result.m14 = 0.0f;
    result.m15 = 1.0f;

    return result;
}

// Returns rotation quaternion for an angle and axis
// NOTE: angle must be provided in radians
rf_public rf_quaternion rf_quaternion_from_axis_angle(rf_vec3 axis, float angle)
{
    rf_quaternion result = {0.0f, 0.0f, 0.0f, 1.0f};

    if (rf_vec3_len(axis) != 0.0f)

        angle *= 0.5f;

    axis = rf_vec3_normalize(axis);

    float sinres = sinf(angle);
    float cosres = cosf(angle);

    result.x = axis.x * sinres;
    result.y = axis.y * sinres;
    result.z = axis.z * sinres;
    result.w = cosres;

    result = rf_quaternion_normalize(result);

    return result;
}

// Returns the rotation angle and axis for a given quaternion
rf_public void rf_quaternion_to_axis_angle(rf_quaternion q, rf_vec3 *outAxis, float *outAngle)
{
    if (fabs(q.w) > 1.0f) q = rf_quaternion_normalize(q);

    rf_vec3 resAxis = {0.0f, 0.0f, 0.0f};
    float resAngle = 0.0f;

    resAngle = 2.0f * (float) acos(q.w);
    float den = (float) sqrt(1.0f - q.w * q.w);

    if (den > 0.0001f)
    {
        resAxis.x = q.x / den;
        resAxis.y = q.y / den;
        resAxis.z = q.z / den;
    } else
    {
        // This occurs when the angle is zero.
        // Not a problem: just set an arbitrary normalized axis.
        resAxis.x = 1.0f;
    }

    *outAxis = resAxis;
    *outAngle = resAngle;
}

// Returns he quaternion equivalent to Euler angles
rf_public rf_quaternion rf_quaternion_from_euler(float roll, float pitch, float yaw)
{
    rf_quaternion q = {0};

    float x0 = cosf(roll * 0.5f);
    float x1 = sinf(roll * 0.5f);
    float y0 = cosf(pitch * 0.5f);
    float y1 = sinf(pitch * 0.5f);
    float z0 = cosf(yaw * 0.5f);
    float z1 = sinf(yaw * 0.5f);

    q.x = x1 * y0 * z0 - x0 * y1 * z1;
    q.y = x0 * y1 * z0 + x1 * y0 * z1;
    q.z = x0 * y0 * z1 - x1 * y1 * z0;
    q.w = x0 * y0 * z0 + x1 * y1 * z1;

    return q;
}

// Return the Euler angles equivalent to quaternion (roll, pitch, yaw)
// NOTE: Angles are returned in a rf_vec3 struct in degrees
rf_public rf_vec3 rf_quaternion_to_euler(rf_quaternion q)
{
    rf_vec3 result = {0};

// roll (x-axis rotation)
    float x0 = 2.0f * (q.w * q.x + q.y * q.z);
    float x1 = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    result.x = atan2f(x0, x1) * rf_rad2deg;

// pitch (y-axis rotation)
    float y0 = 2.0f * (q.w * q.y - q.z * q.x);
    y0 = y0 > 1.0f ? 1.0f : y0;
    y0 = y0 < -1.0f ? -1.0f : y0;
    result.y = sinf(y0) * rf_rad2deg;

// yaw (z-axis rotation)
    float z0 = 2.0f * (q.w * q.z + q.x * q.y);
    float z1 = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
    result.z = atan2f(z0, z1) * rf_rad2deg;

    return result;
}

// rf_transform a quaternion given a transformation matrix
rf_public rf_quaternion rf_quaternion_transform(rf_quaternion q, rf_mat mat)
{
    rf_quaternion result = {0};

    result.x = mat.m0 * q.x + mat.m4 * q.y + mat.m8 * q.z + mat.m12 * q.w;
    result.y = mat.m1 * q.x + mat.m5 * q.y + mat.m9 * q.z + mat.m13 * q.w;
    result.z = mat.m2 * q.x + mat.m6 * q.y + mat.m10 * q.z + mat.m14 * q.w;
    result.w = mat.m3 * q.x + mat.m7 * q.y + mat.m11 * q.z + mat.m15 * q.w;

    return result;
}

#pragma endregion

#pragma region collision detection

// Check if point is inside rectangle
rf_bool rf_check_collision_point_rec(rf_vec2 point, rf_rec rec)
{
    rf_bool collision = 0;

    if ((point.x >= rec.x) && (point.x <= (rec.x + rec.width)) && (point.y >= rec.y) &&
        (point.y <= (rec.y + rec.height)))
        collision = 1;

    return collision;
}

// Check if point is inside circle
rf_bool rf_check_collision_point_circle(rf_vec2 point, rf_vec2 center, float radius)
{
    return rf_check_collision_circles(point, 0, center, radius);
}

// Check if point is inside a triangle defined by three points (p1, p2, p3)
rf_bool rf_check_collision_point_triangle(rf_vec2 point, rf_vec2 p1, rf_vec2 p2, rf_vec2 p3)
{
    rf_bool collision = 0;

    float alpha = ((p2.y - p3.y) * (point.x - p3.x) + (p3.x - p2.x) * (point.y - p3.y)) /
                  ((p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y));

    float beta = ((p3.y - p1.y) * (point.x - p3.x) + (p1.x - p3.x) * (point.y - p3.y)) /
                 ((p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y));

    float gamma = 1.0f - alpha - beta;

    if ((alpha > 0) && (beta > 0) & (gamma > 0)) collision = 1;

    return collision;
}

// Check collision between two rectangles
rf_bool rf_check_collision_recs(rf_rec rec1, rf_rec rec2)
{
    rf_bool collision = 0;

    if ((rec1.x < (rec2.x + rec2.width) && (rec1.x + rec1.width) > rec2.x) &&
        (rec1.y < (rec2.y + rec2.height) && (rec1.y + rec1.height) > rec2.y))
        collision = 1;

    return collision;
}

// Check collision between two circles
rf_bool rf_check_collision_circles(rf_vec2 center1, float radius1, rf_vec2 center2, float radius2)
{
    rf_bool collision = 0;

    float dx = center2.x - center1.x; // X distance between centers
    float dy = center2.y - center1.y; // Y distance between centers

    float distance = sqrt(dx * dx + dy * dy); // Distance between centers

    if (distance <= (radius1 + radius2)) collision = 1;

    return collision;
}

// Check collision between circle and rectangle
// NOTE: Reviewed version to take into account corner limit case
rf_bool rf_check_collision_circle_rec(rf_vec2 center, float radius, rf_rec rec)
{
    int recCenterX = (int) (rec.x + rec.width / 2.0f);
    int recCenterY = (int) (rec.y + rec.height / 2.0f);

    float dx = (float) fabs(center.x - recCenterX);
    float dy = (float) fabs(center.y - recCenterY);

    if (dx > (rec.width / 2.0f + radius))
    { return 0; }
    if (dy > (rec.height / 2.0f + radius))
    { return 0; }

    if (dx <= (rec.width / 2.0f))
    { return 1; }
    if (dy <= (rec.height / 2.0f))
    { return 1; }

    float cornerDistanceSq = (dx - rec.width / 2.0f) * (dx - rec.width / 2.0f) +
                             (dy - rec.height / 2.0f) * (dy - rec.height / 2.0f);

    return (cornerDistanceSq <= (radius * radius));
}

// Get collision rectangle for two rectangles collision
rf_rec rf_get_collision_rec(rf_rec rec1, rf_rec rec2)
{
    rf_rec retRec = {0, 0, 0, 0};

    if (rf_check_collision_recs(rec1, rec2))
    {
        float dxx = (float) fabs(rec1.x - rec2.x);
        float dyy = (float) fabs(rec1.y - rec2.y);

        if (rec1.x <= rec2.x)
        {
            if (rec1.y <= rec2.y)
            {
                retRec.x = rec2.x;
                retRec.y = rec2.y;
                retRec.width = rec1.width - dxx;
                retRec.height = rec1.height - dyy;
            } else
            {
                retRec.x = rec2.x;
                retRec.y = rec1.y;
                retRec.width = rec1.width - dxx;
                retRec.height = rec2.height - dyy;
            }
        } else
        {
            if (rec1.y <= rec2.y)
            {
                retRec.x = rec1.x;
                retRec.y = rec2.y;
                retRec.width = rec2.width - dxx;
                retRec.height = rec1.height - dyy;
            } else
            {
                retRec.x = rec1.x;
                retRec.y = rec1.y;
                retRec.width = rec2.width - dxx;
                retRec.height = rec2.height - dyy;
            }
        }

        if (rec1.width > rec2.width)
        {
            if (retRec.width >= rec2.width) retRec.width = rec2.width;
        } else
        {
            if (retRec.width >= rec1.width) retRec.width = rec1.width;
        }

        if (rec1.height > rec2.height)
        {
            if (retRec.height >= rec2.height) retRec.height = rec2.height;
        } else
        {
            if (retRec.height >= rec1.height) retRec.height = rec1.height;
        }
    }

    return retRec;
}

// Detect collision between two spheres
rf_public rf_bool rf_check_collision_spheres(rf_vec3 center_a, float radius_a, rf_vec3 center_b, float radius_b)
{
    rf_bool collision = 0;

// Simple way to check for collision, just checking distance between two points
// Unfortunately, rf_sqrtf() is a costly operation, so we avoid it with following solution
/*
float dx = centerA.x - centerB.x;      // X distance between centers
float dy = centerA.y - centerB.y;      // Y distance between centers
float dz = centerA.z - centerB.z;      // Z distance between centers

float distance = rf_sqrtf(dx*dx + dy*dy + dz*dz);  // Distance between centers

if (distance <= (radiusA + radiusB)) collision = 1;
*/
// Check for distances squared to avoid rf_sqrtf()
    if (rf_vec3_dot_product(rf_vec3_sub(center_b, center_a), rf_vec3_sub(center_b, center_a)) <=
        (radius_a + radius_b) * (radius_a + radius_b))
        collision = 1;

    return collision;
}

// Detect collision between two boxes. Note: Boxes are defined by two points minimum and maximum
rf_public rf_bool rf_check_collision_boxes(rf_bounding_box box1, rf_bounding_box box2)
{
    rf_bool collision = 1;

    if ((box1.max.x >= box2.min.x) && (box1.min.x <= box2.max.x))
    {
        if ((box1.max.y < box2.min.y) || (box1.min.y > box2.max.y)) collision = 0;
        if ((box1.max.z < box2.min.z) || (box1.min.z > box2.max.z)) collision = 0;
    } else collision = 0;

    return collision;
}

// Detect collision between box and sphere
rf_public rf_bool rf_check_collision_box_sphere(rf_bounding_box box, rf_vec3 center, float radius)
{
    rf_bool collision = 0;

    float dmin = 0;

    if (center.x < box.min.x) dmin += powf(center.x - box.min.x, 2);
    else if (center.x > box.max.x) dmin += powf(center.x - box.max.x, 2);

    if (center.y < box.min.y) dmin += powf(center.y - box.min.y, 2);
    else if (center.y > box.max.y) dmin += powf(center.y - box.max.y, 2);

    if (center.z < box.min.z) dmin += powf(center.z - box.min.z, 2);
    else if (center.z > box.max.z) dmin += powf(center.z - box.max.z, 2);

    if (dmin <= (radius * radius)) collision = 1;

    return collision;
}

// Detect collision between ray and sphere
rf_public rf_bool rf_check_collision_ray_sphere(rf_ray ray, rf_vec3 center, float radius)
{
    rf_bool collision = 0;

    rf_vec3 ray_sphere_pos = rf_vec3_sub(center, ray.position);
    float distance = rf_vec3_len(ray_sphere_pos);
    float vector = rf_vec3_dot_product(ray_sphere_pos, ray.direction);
    float d = radius * radius - (distance * distance - vector * vector);

    if (d >= 0.0f) collision = 1;

    return collision;
}

// Detect collision between ray and sphere with extended parameters and collision point detection
rf_public rf_bool rf_check_collision_ray_sphere_ex(rf_ray ray, rf_vec3 center, float radius, rf_vec3 *collision_point)
{
    rf_bool collision = 0;

    rf_vec3 ray_sphere_pos = rf_vec3_sub(center, ray.position);
    float distance = rf_vec3_len(ray_sphere_pos);
    float vector = rf_vec3_dot_product(ray_sphere_pos, ray.direction);
    float d = radius * radius - (distance * distance - vector * vector);

    if (d >= 0.0f) collision = 1;

// Check if ray origin is inside the sphere to calculate the correct collision point
    float collision_distance = 0;

    if (distance < radius) collision_distance = vector + sqrt(d);
    else collision_distance = vector - sqrt(d);

// Calculate collision point
    rf_vec3 c_point = rf_vec3_add(ray.position, rf_vec3_scale(ray.direction, collision_distance));

    collision_point->x = c_point.x;
    collision_point->y = c_point.y;
    collision_point->z = c_point.z;

    return collision;
}

// Detect collision between ray and bounding box
rf_public rf_bool rf_check_collision_ray_box(rf_ray ray, rf_bounding_box box)
{
    rf_bool collision = 0;

    float t[8];
    t[0] = (box.min.x - ray.position.x) / ray.direction.x;
    t[1] = (box.max.x - ray.position.x) / ray.direction.x;
    t[2] = (box.min.y - ray.position.y) / ray.direction.y;
    t[3] = (box.max.y - ray.position.y) / ray.direction.y;
    t[4] = (box.min.z - ray.position.z) / ray.direction.z;
    t[5] = (box.max.z - ray.position.z) / ray.direction.z;
    t[6] = (float) fmax(fmax(fmin(t[0], t[1]), fmin(t[2], t[3])), fmin(t[4], t[5]));
    t[7] = (float) fmin(fmin(fmax(t[0], t[1]), fmax(t[2], t[3])), fmax(t[4], t[5]));

    collision = !(t[7] < 0 || t[6] > t[7]);

    return collision;
}

// Get collision info between ray and triangle. Note: Based on https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
rf_public rf_ray_hit_info rf_collision_ray_triangle(rf_ray ray, rf_vec3 p1, rf_vec3 p2, rf_vec3 p3)
{
    rf_ray_hit_info result = {0};
    rf_vec3 edge1  = {0};
    rf_vec3 edge2  = {0};
    rf_vec3 p      = {0};
    rf_vec3 q      = {0};
    rf_vec3 tv     = {0};
    float det      = 0;
    float inv_det  = 0;
    float u        = 0;
    float v        = 0;
    float t        = 0;
    double epsilon = 0.000001; // Just a small number

    // Find vectors for two edges sharing V1
    edge1 = rf_vec3_sub(p2, p1);
    edge2 = rf_vec3_sub(p3, p1);

    // Begin calculating determinant - also used to calculate u parameter
    p = rf_vec3_cross_product(ray.direction, edge2);

    // If determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
    det = rf_vec3_dot_product(edge1, p);

    // Avoid culling!
    if ((det > -epsilon) && (det < epsilon)) return result;

    inv_det = 1.0f / det;

    // Calculate distance from V1 to ray origin
    tv = rf_vec3_sub(ray.position, p1);

    // Calculate u parameter and test bound
    u = rf_vec3_dot_product(tv, p) * inv_det;

    // The intersection lies outside of the triangle
    if ((u < 0.0f) || (u > 1.0f)) return result;

    // Prepare to test v parameter
    q = rf_vec3_cross_product(tv, edge1);

    // Calculate V parameter and test bound
    v = rf_vec3_dot_product(ray.direction, q) * inv_det;

    // The intersection lies outside of the triangle
    if ((v < 0.0f) || ((u + v) > 1.0f)) return result;

    t = rf_vec3_dot_product(edge2, q) * inv_det;

    if (t > epsilon)
    {
        // rf_ray hit, get hit point and normal
        result.hit = 1;
        result.distance = t;
        result.hit = 1;
        result.normal = rf_vec3_normalize(rf_vec3_cross_product(edge1, edge2));
        result.position = rf_vec3_add(ray.position, rf_vec3_scale(ray.direction, t));
    }

    return result;
}

// Get collision info between ray and ground plane (Y-normal plane)
rf_public rf_ray_hit_info rf_collision_ray_ground(rf_ray ray, float ground_height)
{
    rf_ray_hit_info result = {0};
    double epsilon = 0.000001; // Just a small number

    if (fabs(ray.direction.y) > epsilon)
    {
        float distance = (ray.position.y - ground_height) / -ray.direction.y;

        if (distance >= 0.0)
        {
            result.hit = 1;
            result.distance = distance;
            result.normal = (rf_vec3) {0.0, 1.0, 0.0};
            result.position = rf_vec3_add(ray.position, rf_vec3_scale(ray.direction, distance));
        }
    }

    return result;
}

#pragma endregion



#if !defined(RAYFORK_NO_GFX)

#ifndef RAYFORK_GFX_INTERNAL_STRING_UTILS_H
#define RAYFORK_GFX_INTERNAL_STRING_UTILS_H
#include "string.h"

#ifndef RF_MAX_FILEPATH_LEN
    #define RF_MAX_FILEPATH_LEN (1024)
#endif

rf_internal inline rf_bool rf_match_str_n(const char* a, int a_len, const char* b, int b_len)
{
    return a_len == b_len && strncmp(a, b, a_len) == 0;
}

rf_internal inline rf_bool rf_match_str_cstr(const char* a, int a_len, const char* b)
{
    return rf_match_str_n(a, a_len, b, strlen(b));
}

rf_internal inline rf_bool rf_is_file_extension(const char* filename, const char* ext)
{
    int filename_len = strlen(filename);
    int ext_len      = strlen(ext);

    if (filename_len < ext_len)
    {
        return 0;
    }

    return rf_match_str_n(filename + filename_len - ext_len, ext_len, ext, ext_len);
}

rf_internal inline const char* rf__str_find_last(const char* s, const char* charset)
{
    const char* latest_match = NULL;
    for (; s = strpbrk(s, charset), s != NULL; latest_match = s++) { }
    return latest_match;
}

rf_internal inline const char* rf_get_directory_path_from_file_path(const char* file_path)
{
    static rf_thread_local char rf_global_dir_path[RF_MAX_FILEPATH_LEN];

    const char* last_slash = NULL;
    memset(rf_global_dir_path, 0, RF_MAX_FILEPATH_LEN);

    last_slash = rf__str_find_last(file_path, "\\/");
    if (!last_slash) { return NULL; }

    // NOTE: Be careful, strncpy() is not safe, it does not care about '\0'
    strncpy(rf_global_dir_path, file_path, strlen(file_path) - (strlen(last_slash) - 1));
    rf_global_dir_path[strlen(file_path) - strlen(last_slash)] = '\0'; // Add '\0' manually

    return rf_global_dir_path;
}

#endif // RAYFORK_GFX_INTERNAL_STRING_UTILS_H

rf_internal rf_gfx_context* rf__global_gfx_context_ptr;

#define rf_ctx   (*rf__global_gfx_context_ptr)
#define rf_gfx   (rf_ctx.gfx_backend_data)
#define rf_gl    (rf_gfx.gl)
#define rf_batch (*(rf_ctx.current_batch))

#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33) || defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)

#pragma region macros

// Use this for glClearDepth
#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
    #define rf_gl_ClearDepth rf_gl.ClearDepth
#else
    #define rf_gl_ClearDepth rf_gl.ClearDepthf
#endif

#pragma endregion

#pragma region gl constants

// Since we don't have an opengl header to include I just pasted all the constants we might need
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_LOOP 0x0002
#define GL_LINE_STRIP 0x0003
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_NEVER 0x0200
#define GL_LESS 0x0201
#define GL_EQUAL 0x0202
#define GL_LEQUAL 0x0203
#define GL_GREATER 0x0204
#define GL_NOTEQUAL 0x0205
#define GL_GEQUAL 0x0206
#define GL_ALWAYS 0x0207
#define GL_ZERO 0
#define GL_ONE 1
#define GL_SRC_COLOR 0x0300
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_DST_ALPHA 0x0304
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_DST_COLOR 0x0306
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_NONE 0
#define GL_FRONT_LEFT 0x0400
#define GL_FRONT_RIGHT 0x0401
#define GL_BACK_LEFT 0x0402
#define GL_BACK_RIGHT 0x0403
#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_LEFT 0x0406
#define GL_RIGHT 0x0407
#define GL_FRONT_AND_BACK 0x0408
#define GL_NO_ERROR 0
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_OUT_OF_MEMORY 0x0505
#define GL_CW 0x0900
#define GL_CCW 0x0901
#define GL_POINT_SIZE 0x0B11
#define GL_POINT_SIZE_RANGE 0x0B12
#define GL_POINT_SIZE_GRANULARITY 0x0B13
#define GL_LINE_SMOOTH 0x0B20
#define GL_LINE_WIDTH 0x0B21
#define GL_LINE_WIDTH_RANGE 0x0B22
#define GL_LINE_WIDTH_GRANULARITY 0x0B23
#define GL_POLYGON_MODE 0x0B40
#define GL_POLYGON_SMOOTH 0x0B41
#define GL_CULL_FACE 0x0B44
#define GL_CULL_FACE_MODE 0x0B45
#define GL_FRONT_FACE 0x0B46
#define GL_DEPTH_RANGE 0x0B70
#define GL_DEPTH_TEST 0x0B71
#define GL_DEPTH_WRITEMASK 0x0B72
#define GL_DEPTH_CLEAR_VALUE 0x0B73
#define GL_DEPTH_FUNC 0x0B74
#define GL_STENCIL_TEST 0x0B90
#define GL_STENCIL_CLEAR_VALUE 0x0B91
#define GL_STENCIL_FUNC 0x0B92
#define GL_STENCIL_VALUE_MASK 0x0B93
#define GL_STENCIL_FAIL 0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL 0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS 0x0B96
#define GL_STENCIL_REF 0x0B97
#define GL_STENCIL_WRITEMASK 0x0B98
#define GL_VIEWPORT 0x0BA2
#define GL_DITHER 0x0BD0
#define GL_BLEND_DST 0x0BE0
#define GL_BLEND_SRC 0x0BE1
#define GL_BLEND 0x0BE2
#define GL_LOGIC_OP_MODE 0x0BF0
#define GL_COLOR_LOGIC_OP 0x0BF2
#define GL_DRAW_BUFFER 0x0C01
#define GL_READ_BUFFER 0x0C02
#define GL_SCISSOR_BOX 0x0C10
#define GL_SCISSOR_TEST 0x0C11
#define GL_COLOR_CLEAR_VALUE 0x0C22
#define GL_COLOR_WRITEMASK 0x0C23
#define GL_DOUBLEBUFFER 0x0C32
#define GL_STEREO 0x0C33
#define GL_LINE_SMOOTH_HINT 0x0C52
#define GL_POLYGON_SMOOTH_HINT 0x0C53
#define GL_UNPACK_SWAP_BYTES 0x0CF0
#define GL_UNPACK_LSB_FIRST 0x0CF1
#define GL_UNPACK_ROW_LENGTH 0x0CF2
#define GL_UNPACK_SKIP_ROWS 0x0CF3
#define GL_UNPACK_SKIP_PIXELS 0x0CF4
#define GL_UNPACK_ALIGNMENT 0x0CF5
#define GL_PACK_SWAP_BYTES 0x0D00
#define GL_PACK_LSB_FIRST 0x0D01
#define GL_PACK_ROW_LENGTH 0x0D02
#define GL_PACK_SKIP_ROWS 0x0D03
#define GL_PACK_SKIP_PIXELS 0x0D04
#define GL_PACK_ALIGNMENT 0x0D05
#define GL_MAX_TEXTURE_SIZE 0x0D33
#define GL_MAX_VIEWPORT_DIMS 0x0D3A
#define GL_SUBPIXEL_BITS 0x0D50
#define GL_TEXTURE_1D 0x0DE0
#define GL_TEXTURE_2D 0x0DE1
#define GL_POLYGON_OFFSET_UNITS 0x2A00
#define GL_POLYGON_OFFSET_POINT 0x2A01
#define GL_POLYGON_OFFSET_LINE 0x2A02
#define GL_POLYGON_OFFSET_FILL 0x8037
#define GL_POLYGON_OFFSET_FACTOR 0x8038
#define GL_TEXTURE_BINDING_1D 0x8068
#define GL_TEXTURE_BINDING_2D 0x8069
#define GL_TEXTURE_WIDTH 0x1000
#define GL_TEXTURE_HEIGHT 0x1001
#define GL_TEXTURE_INTERNAL_FORMAT 0x1003
#define GL_TEXTURE_BORDER_COLOR 0x1004
#define GL_TEXTURE_RED_SIZE 0x805C
#define GL_TEXTURE_GREEN_SIZE 0x805D
#define GL_TEXTURE_BLUE_SIZE 0x805E
#define GL_TEXTURE_ALPHA_SIZE 0x805F
#define GL_DONT_CARE 0x1100
#define GL_FASTEST 0x1101
#define GL_NICEST 0x1102
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_DOUBLE 0x140A
#define GL_CLEAR 0x1500
#define GL_AND 0x1501
#define GL_AND_REVERSE 0x1502
#define GL_COPY 0x1503
#define GL_AND_INVERTED 0x1504
#define GL_NOOP 0x1505
#define GL_XOR 0x1506
#define GL_OR 0x1507
#define GL_NOR 0x1508
#define GL_EQUIV 0x1509
#define GL_INVERT 0x150A
#define GL_OR_REVERSE 0x150B
#define GL_COPY_INVERTED 0x150C
#define GL_OR_INVERTED 0x150D
#define GL_NAND 0x150E
#define GL_SET 0x150F
#define GL_TEXTURE 0x1702
#define GL_COLOR 0x1800
#define GL_DEPTH 0x1801
#define GL_STENCIL 0x1802
#define GL_STENCIL_INDEX 0x1901
#define GL_DEPTH_COMPONENT 0x1902
#define GL_RED 0x1903
#define GL_GREEN 0x1904
#define GL_BLUE 0x1905
#define GL_ALPHA 0x1906
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_POINT 0x1B00
#define GL_LINE 0x1B01
#define GL_FILL 0x1B02
#define GL_KEEP 0x1E00
#define GL_REPLACE 0x1E01
#define GL_INCR 0x1E02
#define GL_DECR 0x1E03
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_EXTENSIONS 0x1F03
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_PROXY_TEXTURE_1D 0x8063
#define GL_PROXY_TEXTURE_2D 0x8064
#define GL_REPEAT 0x2901
#define GL_R3_G3_B2 0x2A10
#define GL_RGB4 0x804F
#define GL_RGB5 0x8050
#define GL_RGB8 0x8051
#define GL_RGB10 0x8052
#define GL_RGB12 0x8053
#define GL_RGB16 0x8054
#define GL_RGBA2 0x8055
#define GL_RGBA4 0x8056
#define GL_RGB5_A1 0x8057
#define GL_RGBA8 0x8058
#define GL_RGB10_A2 0x8059
#define GL_RGBA12 0x805A
#define GL_RGBA16 0x805B
#define GL_UNSIGNED_BYTE_3_3_2 0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
#define GL_UNSIGNED_INT_8_8_8_8 0x8035
#define GL_UNSIGNED_INT_10_10_10_2 0x8036
#define GL_TEXTURE_BINDING_3D 0x806A
#define GL_PACK_SKIP_IMAGES 0x806B
#define GL_PACK_IMAGE_HEIGHT 0x806C
#define GL_UNPACK_SKIP_IMAGES 0x806D
#define GL_UNPACK_IMAGE_HEIGHT 0x806E
#define GL_TEXTURE_3D 0x806F
#define GL_PROXY_TEXTURE_3D 0x8070
#define GL_TEXTURE_DEPTH 0x8071
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_MAX_3D_TEXTURE_SIZE 0x8073
#define GL_UNSIGNED_BYTE_2_3_3_REV 0x8362
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV 0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4_REV 0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV 0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#define GL_UNSIGNED_INT_2_10_10_10_REV 0x8368
#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1
#define GL_MAX_ELEMENTS_VERTICES 0x80E8
#define GL_MAX_ELEMENTS_INDICES 0x80E9
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_TEXTURE_MIN_LOD 0x813A
#define GL_TEXTURE_MAX_LOD 0x813B
#define GL_TEXTURE_BASE_LEVEL 0x813C
#define GL_TEXTURE_MAX_LEVEL 0x813D
#define GL_SMOOTH_POINT_SIZE_RANGE 0x0B12
#define GL_SMOOTH_POINT_SIZE_GRANULARITY 0x0B13
#define GL_SMOOTH_LINE_WIDTH_RANGE 0x0B22
#define GL_SMOOTH_LINE_WIDTH_GRANULARITY 0x0B23
#define GL_ALIASED_LINE_WIDTH_RANGE 0x846E
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7
#define GL_TEXTURE8 0x84C8
#define GL_TEXTURE9 0x84C9
#define GL_TEXTURE10 0x84CA
#define GL_TEXTURE11 0x84CB
#define GL_TEXTURE12 0x84CC
#define GL_TEXTURE13 0x84CD
#define GL_TEXTURE14 0x84CE
#define GL_TEXTURE15 0x84CF
#define GL_TEXTURE16 0x84D0
#define GL_TEXTURE17 0x84D1
#define GL_TEXTURE18 0x84D2
#define GL_TEXTURE19 0x84D3
#define GL_TEXTURE20 0x84D4
#define GL_TEXTURE21 0x84D5
#define GL_TEXTURE22 0x84D6
#define GL_TEXTURE23 0x84D7
#define GL_TEXTURE24 0x84D8
#define GL_TEXTURE25 0x84D9
#define GL_TEXTURE26 0x84DA
#define GL_TEXTURE27 0x84DB
#define GL_TEXTURE28 0x84DC
#define GL_TEXTURE29 0x84DD
#define GL_TEXTURE30 0x84DE
#define GL_TEXTURE31 0x84DF
#define GL_ACTIVE_TEXTURE 0x84E0
#define GL_MULTISAMPLE 0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
#define GL_SAMPLE_ALPHA_TO_ONE 0x809F
#define GL_SAMPLE_COVERAGE 0x80A0
#define GL_SAMPLE_BUFFERS 0x80A8
#define GL_SAMPLES 0x80A9
#define GL_SAMPLE_COVERAGE_VALUE 0x80AA
#define GL_SAMPLE_COVERAGE_INVERT 0x80AB
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP 0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP 0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE 0x851C
#define GL_COMPRESSED_RGB 0x84ED
#define GL_COMPRESSED_RGBA 0x84EE
#define GL_TEXTURE_COMPRESSION_HINT 0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE 0x86A0
#define GL_TEXTURE_COMPRESSED 0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS 0x86A3
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_BLEND_DST_RGB 0x80C8
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_BLEND_DST_ALPHA 0x80CA
#define GL_BLEND_SRC_ALPHA 0x80CB
#define GL_POINT_FADE_THRESHOLD_SIZE 0x8128
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_DEPTH_COMPONENT24 0x81A6
#define GL_DEPTH_COMPONENT32 0x81A7
#define GL_MIRRORED_REPEAT 0x8370
#define GL_MAX_TEXTURE_LOD_BIAS 0x84FD
#define GL_TEXTURE_LOD_BIAS 0x8501
#define GL_INCR_WRAP 0x8507
#define GL_DECR_WRAP 0x8508
#define GL_TEXTURE_DEPTH_SIZE 0x884A
#define GL_TEXTURE_COMPARE_MODE 0x884C
#define GL_TEXTURE_COMPARE_FUNC 0x884D
#define GL_FUNC_ADD 0x8006
#define GL_FUNC_SUBTRACT 0x800A
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_MIN 0x8007
#define GL_MAX 0x8008
#define GL_CONSTANT_COLOR 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_CONSTANT_ALPHA 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_BUFFER_SIZE 0x8764
#define GL_BUFFER_USAGE 0x8765
#define GL_QUERY_COUNTER_BITS 0x8864
#define GL_CURRENT_QUERY 0x8865
#define GL_QUERY_RESULT 0x8866
#define GL_QUERY_RESULT_AVAILABLE 0x8867
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_ARRAY_BUFFER_BINDING 0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_READ_ONLY 0x88B8
#define GL_WRITE_ONLY 0x88B9
#define GL_READ_WRITE 0x88BA
#define GL_BUFFER_ACCESS 0x88BB
#define GL_BUFFER_MAPPED 0x88BC
#define GL_BUFFER_MAP_POINTER 0x88BD
#define GL_STREAM_DRAW 0x88E0
#define GL_STREAM_READ 0x88E1
#define GL_STREAM_COPY 0x88E2
#define GL_STATIC_DRAW 0x88E4
#define GL_STATIC_READ 0x88E5
#define GL_STATIC_COPY 0x88E6
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_DYNAMIC_READ 0x88E9
#define GL_DYNAMIC_COPY 0x88EA
#define GL_SAMPLES_PASSED 0x8914
#define GL_SRC1_ALPHA 0x8589
#define GL_BLEND_EQUATION_RGB 0x8009
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED 0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE 0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE 0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE 0x8625
#define GL_CURRENT_VERTEX_ATTRIB 0x8626
#define GL_VERTEX_PROGRAM_POINT_SIZE 0x8642
#define GL_VERTEX_ATTRIB_ARRAY_POINTER 0x8645
#define GL_STENCIL_BACK_FUNC 0x8800
#define GL_STENCIL_BACK_FAIL 0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL 0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS 0x8803
#define GL_MAX_DRAW_BUFFERS 0x8824
#define GL_DRAW_BUFFER0 0x8825
#define GL_DRAW_BUFFER1 0x8826
#define GL_DRAW_BUFFER2 0x8827
#define GL_DRAW_BUFFER3 0x8828
#define GL_DRAW_BUFFER4 0x8829
#define GL_DRAW_BUFFER5 0x882A
#define GL_DRAW_BUFFER6 0x882B
#define GL_DRAW_BUFFER7 0x882C
#define GL_DRAW_BUFFER8 0x882D
#define GL_DRAW_BUFFER9 0x882E
#define GL_DRAW_BUFFER10 0x882F
#define GL_DRAW_BUFFER11 0x8830
#define GL_DRAW_BUFFER12 0x8831
#define GL_DRAW_BUFFER13 0x8832
#define GL_DRAW_BUFFER14 0x8833
#define GL_DRAW_BUFFER15 0x8834
#define GL_BLEND_EQUATION_ALPHA 0x883D
#define GL_MAX_VERTEX_ATTRIBS 0x8869
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
#define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS 0x8B49
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS 0x8B4A
#define GL_MAX_VARYING_FLOATS 0x8B4B
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_SHADER_TYPE 0x8B4F
#define GL_FLOAT_VEC2 0x8B50
#define GL_FLOAT_VEC3 0x8B51
#define GL_FLOAT_VEC4 0x8B52
#define GL_INT_VEC2 0x8B53
#define GL_INT_VEC3 0x8B54
#define GL_INT_VEC4 0x8B55
#define GL_BOOL 0x8B56
#define GL_BOOL_VEC2 0x8B57
#define GL_BOOL_VEC3 0x8B58
#define GL_BOOL_VEC4 0x8B59
#define GL_FLOAT_MAT2 0x8B5A
#define GL_FLOAT_MAT3 0x8B5B
#define GL_FLOAT_MAT4 0x8B5C
#define GL_SAMPLER_1D 0x8B5D
#define GL_SAMPLER_2D 0x8B5E
#define GL_SAMPLER_3D 0x8B5F
#define GL_SAMPLER_CUBE 0x8B60
#define GL_SAMPLER_1D_SHADOW 0x8B61
#define GL_SAMPLER_2D_SHADOW 0x8B62
#define GL_DELETE_STATUS 0x8B80
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_VALIDATE_STATUS 0x8B83
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_ATTACHED_SHADERS 0x8B85
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87
#define GL_SHADER_SOURCE_LENGTH 0x8B88
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH 0x8B8A
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT 0x8B8B
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#define GL_CURRENT_PROGRAM 0x8B8D
#define GL_POINT_SPRITE_COORD_ORIGIN 0x8CA0
#define GL_LOWER_LEFT 0x8CA1
#define GL_UPPER_LEFT 0x8CA2
#define GL_STENCIL_BACK_REF 0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK 0x8CA4
#define GL_STENCIL_BACK_WRITEMASK 0x8CA5
#define GL_PIXEL_PACK_BUFFER 0x88EB
#define GL_PIXEL_UNPACK_BUFFER 0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING 0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING 0x88EF
#define GL_FLOAT_MAT2x3 0x8B65
#define GL_FLOAT_MAT2x4 0x8B66
#define GL_FLOAT_MAT3x2 0x8B67
#define GL_FLOAT_MAT3x4 0x8B68
#define GL_FLOAT_MAT4x2 0x8B69
#define GL_FLOAT_MAT4x3 0x8B6A
#define GL_SRGB 0x8C40
#define GL_SRGB8 0x8C41
#define GL_SRGB_ALPHA 0x8C42
#define GL_SRGB8_ALPHA8 0x8C43
#define GL_COMPRESSED_SRGB 0x8C48
#define GL_COMPRESSED_SRGB_ALPHA 0x8C49
#define GL_COMPARE_REF_TO_TEXTURE 0x884E
#define GL_CLIP_DISTANCE0 0x3000
#define GL_CLIP_DISTANCE1 0x3001
#define GL_CLIP_DISTANCE2 0x3002
#define GL_CLIP_DISTANCE3 0x3003
#define GL_CLIP_DISTANCE4 0x3004
#define GL_CLIP_DISTANCE5 0x3005
#define GL_CLIP_DISTANCE6 0x3006
#define GL_CLIP_DISTANCE7 0x3007
#define GL_MAX_CLIP_DISTANCES 0x0D32
#define GL_MAJOR_VERSION 0x821B
#define GL_MINOR_VERSION 0x821C
#define GL_NUM_EXTENSIONS 0x821D
#define GL_CONTEXT_FLAGS 0x821E
#define GL_COMPRESSED_RED 0x8225
#define GL_COMPRESSED_RG 0x8226
#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x00000001
#define GL_RGBA32F 0x8814
#define GL_RGB32F 0x8815
#define GL_RGBA16F 0x881A
#define GL_RGB16F 0x881B
#define GL_VERTEX_ATTRIB_ARRAY_INTEGER 0x88FD
#define GL_MAX_ARRAY_TEXTURE_LAYERS 0x88FF
#define GL_MIN_PROGRAM_TEXEL_OFFSET 0x8904
#define GL_MAX_PROGRAM_TEXEL_OFFSET 0x8905
#define GL_CLAMP_READ_COLOR 0x891C
#define GL_FIXED_ONLY 0x891D
#define GL_MAX_VARYING_COMPONENTS 0x8B4B
#define GL_TEXTURE_1D_ARRAY 0x8C18
#define GL_PROXY_TEXTURE_1D_ARRAY 0x8C19
#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_PROXY_TEXTURE_2D_ARRAY 0x8C1B
#define GL_TEXTURE_BINDING_1D_ARRAY 0x8C1C
#define GL_TEXTURE_BINDING_2D_ARRAY 0x8C1D
#define GL_R11F_G11F_B10F 0x8C3A
#define GL_UNSIGNED_INT_10F_11F_11F_REV 0x8C3B
#define GL_RGB9_E5 0x8C3D
#define GL_UNSIGNED_INT_5_9_9_9_REV 0x8C3E
#define GL_TEXTURE_SHARED_SIZE 0x8C3F
#define GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH 0x8C76
#define GL_TRANSFORM_FEEDBACK_BUFFER_MODE 0x8C7F
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS 0x8C80
#define GL_TRANSFORM_FEEDBACK_VARYINGS 0x8C83
#define GL_TRANSFORM_FEEDBACK_BUFFER_START 0x8C84
#define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE 0x8C85
#define GL_PRIMITIVES_GENERATED 0x8C87
#define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN 0x8C88
#define GL_RASTERIZER_DISCARD 0x8C89
#define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS 0x8C8A
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS 0x8C8B
#define GL_INTERLEAVED_ATTRIBS 0x8C8C
#define GL_SEPARATE_ATTRIBS 0x8C8D
#define GL_TRANSFORM_FEEDBACK_BUFFER 0x8C8E
#define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING 0x8C8F
#define GL_RGBA32UI 0x8D70
#define GL_RGB32UI 0x8D71
#define GL_RGBA16UI 0x8D76
#define GL_RGB16UI 0x8D77
#define GL_RGBA8UI 0x8D7C
#define GL_RGB8UI 0x8D7D
#define GL_RGBA32I 0x8D82
#define GL_RGB32I 0x8D83
#define GL_RGBA16I 0x8D88
#define GL_RGB16I 0x8D89
#define GL_RGBA8I 0x8D8E
#define GL_RGB8I 0x8D8F
#define GL_RED_INTEGER 0x8D94
#define GL_GREEN_INTEGER 0x8D95
#define GL_BLUE_INTEGER 0x8D96
#define GL_RGB_INTEGER 0x8D98
#define GL_RGBA_INTEGER 0x8D99
#define GL_BGR_INTEGER 0x8D9A
#define GL_BGRA_INTEGER 0x8D9B
#define GL_SAMPLER_1D_ARRAY 0x8DC0
#define GL_SAMPLER_2D_ARRAY 0x8DC1
#define GL_SAMPLER_1D_ARRAY_SHADOW 0x8DC3
#define GL_SAMPLER_2D_ARRAY_SHADOW 0x8DC4
#define GL_SAMPLER_CUBE_SHADOW 0x8DC5
#define GL_UNSIGNED_INT_VEC2 0x8DC6
#define GL_UNSIGNED_INT_VEC3 0x8DC7
#define GL_UNSIGNED_INT_VEC4 0x8DC8
#define GL_INT_SAMPLER_1D 0x8DC9
#define GL_INT_SAMPLER_2D 0x8DCA
#define GL_INT_SAMPLER_3D 0x8DCB
#define GL_INT_SAMPLER_CUBE 0x8DCC
#define GL_INT_SAMPLER_1D_ARRAY 0x8DCE
#define GL_INT_SAMPLER_2D_ARRAY 0x8DCF
#define GL_UNSIGNED_INT_SAMPLER_1D 0x8DD1
#define GL_UNSIGNED_INT_SAMPLER_2D 0x8DD2
#define GL_UNSIGNED_INT_SAMPLER_3D 0x8DD3
#define GL_UNSIGNED_INT_SAMPLER_CUBE 0x8DD4
#define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY 0x8DD6
#define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY 0x8DD7
#define GL_QUERY_WAIT 0x8E13
#define GL_QUERY_NO_WAIT 0x8E14
#define GL_QUERY_BY_REGION_WAIT 0x8E15
#define GL_QUERY_BY_REGION_NO_WAIT 0x8E16
#define GL_BUFFER_ACCESS_FLAGS 0x911F
#define GL_BUFFER_MAP_LENGTH 0x9120
#define GL_BUFFER_MAP_OFFSET 0x9121
#define GL_DEPTH_COMPONENT32F 0x8CAC
#define GL_DEPTH32F_STENCIL8 0x8CAD
#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV 0x8DAD
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING 0x8210
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE 0x8211
#define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE 0x8212
#define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 0x8213
#define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 0x8214
#define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 0x8215
#define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 0x8216
#define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 0x8217
#define GL_FRAMEBUFFER_DEFAULT 0x8218
#define GL_FRAMEBUFFER_UNDEFINED 0x8219
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_MAX_RENDERBUFFER_SIZE 0x84E8
#define GL_DEPTH_STENCIL 0x84F9
#define GL_UNSIGNED_INT_24_8 0x84FA
#define GL_DEPTH24_STENCIL8 0x88F0
#define GL_TEXTURE_STENCIL_SIZE 0x88F1
#define GL_TEXTURE_RED_TYPE 0x8C10
#define GL_TEXTURE_GREEN_TYPE 0x8C11
#define GL_TEXTURE_BLUE_TYPE 0x8C12
#define GL_TEXTURE_ALPHA_TYPE 0x8C13
#define GL_TEXTURE_DEPTH_TYPE 0x8C16
#define GL_UNSIGNED_NORMALIZED 0x8C17
#define GL_FRAMEBUFFER_BINDING 0x8CA6
#define GL_DRAW_FRAMEBUFFER_BINDING 0x8CA6
#define GL_RENDERBUFFER_BINDING 0x8CA7
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#define GL_READ_FRAMEBUFFER_BINDING 0x8CAA
#define GL_RENDERBUFFER_SAMPLES 0x8CAB
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER 0x8CD4
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD
#define GL_MAX_COLOR_ATTACHMENTS 0x8CDF
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_COLOR_ATTACHMENT4 0x8CE4
#define GL_COLOR_ATTACHMENT5 0x8CE5
#define GL_COLOR_ATTACHMENT6 0x8CE6
#define GL_COLOR_ATTACHMENT7 0x8CE7
#define GL_COLOR_ATTACHMENT8 0x8CE8
#define GL_COLOR_ATTACHMENT9 0x8CE9
#define GL_COLOR_ATTACHMENT10 0x8CEA
#define GL_COLOR_ATTACHMENT11 0x8CEB
#define GL_COLOR_ATTACHMENT12 0x8CEC
#define GL_COLOR_ATTACHMENT13 0x8CED
#define GL_COLOR_ATTACHMENT14 0x8CEE
#define GL_COLOR_ATTACHMENT15 0x8CEF
#define GL_COLOR_ATTACHMENT16 0x8CF0
#define GL_COLOR_ATTACHMENT17 0x8CF1
#define GL_COLOR_ATTACHMENT18 0x8CF2
#define GL_COLOR_ATTACHMENT19 0x8CF3
#define GL_COLOR_ATTACHMENT20 0x8CF4
#define GL_COLOR_ATTACHMENT21 0x8CF5
#define GL_COLOR_ATTACHMENT22 0x8CF6
#define GL_COLOR_ATTACHMENT23 0x8CF7
#define GL_COLOR_ATTACHMENT24 0x8CF8
#define GL_COLOR_ATTACHMENT25 0x8CF9
#define GL_COLOR_ATTACHMENT26 0x8CFA
#define GL_COLOR_ATTACHMENT27 0x8CFB
#define GL_COLOR_ATTACHMENT28 0x8CFC
#define GL_COLOR_ATTACHMENT29 0x8CFD
#define GL_COLOR_ATTACHMENT30 0x8CFE
#define GL_COLOR_ATTACHMENT31 0x8CFF
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_FRAMEBUFFER 0x8D40
#define GL_RENDERBUFFER 0x8D41
#define GL_RENDERBUFFER_WIDTH 0x8D42
#define GL_RENDERBUFFER_HEIGHT 0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT 0x8D44
#define GL_STENCIL_INDEX1 0x8D46
#define GL_STENCIL_INDEX4 0x8D47
#define GL_STENCIL_INDEX8 0x8D48
#define GL_STENCIL_INDEX16 0x8D49
#define GL_RENDERBUFFER_RED_SIZE 0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE 0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE 0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE 0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE 0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE 0x8D55
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
#define GL_MAX_SAMPLES 0x8D57
#define GL_INDEX 0x8222
#define GL_FRAMEBUFFER_SRGB 0x8DB9
#define GL_HALF_FLOAT 0x140B
#define GL_MAP_READ_BIT 0x0001
#define GL_MAP_WRITE_BIT 0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT 0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT 0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT 0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT 0x0020
#define GL_COMPRESSED_RED_RGTC1 0x8DBB
#define GL_COMPRESSED_SIGNED_RED_RGTC1 0x8DBC
#define GL_COMPRESSED_RG_RGTC2 0x8DBD
#define GL_COMPRESSED_SIGNED_RG_RGTC2 0x8DBE
#define GL_RG 0x8227
#define GL_RG_INTEGER 0x8228
#define GL_R8 0x8229
#define GL_R16 0x822A
#define GL_RG8 0x822B
#define GL_RG16 0x822C
#define GL_R16F 0x822D
#define GL_R32F 0x822E
#define GL_RG16F 0x822F
#define GL_RG32F 0x8230
#define GL_R8I 0x8231
#define GL_R8UI 0x8232
#define GL_R16I 0x8233
#define GL_R16UI 0x8234
#define GL_R32I 0x8235
#define GL_R32UI 0x8236
#define GL_RG8I 0x8237
#define GL_RG8UI 0x8238
#define GL_RG16I 0x8239
#define GL_RG16UI 0x823A
#define GL_RG32I 0x823B
#define GL_RG32UI 0x823C
#define GL_VERTEX_ARRAY_BINDING 0x85B5
#define GL_SAMPLER_2D_RECT 0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW 0x8B64
#define GL_SAMPLER_BUFFER 0x8DC2
#define GL_INT_SAMPLER_2D_RECT 0x8DCD
#define GL_INT_SAMPLER_BUFFER 0x8DD0
#define GL_UNSIGNED_INT_SAMPLER_2D_RECT 0x8DD5
#define GL_UNSIGNED_INT_SAMPLER_BUFFER 0x8DD8
#define GL_TEXTURE_BUFFER 0x8C2A
#define GL_MAX_TEXTURE_BUFFER_SIZE 0x8C2B
#define GL_TEXTURE_BINDING_BUFFER 0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING 0x8C2D
#define GL_TEXTURE_RECTANGLE 0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE 0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE 0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE 0x84F8
#define GL_R8_SNORM 0x8F94
#define GL_RG8_SNORM 0x8F95
#define GL_RGB8_SNORM 0x8F96
#define GL_RGBA8_SNORM 0x8F97
#define GL_R16_SNORM 0x8F98
#define GL_RG16_SNORM 0x8F99
#define GL_RGB16_SNORM 0x8F9A
#define GL_RGBA16_SNORM 0x8F9B
#define GL_SIGNED_NORMALIZED 0x8F9C
#define GL_PRIMITIVE_RESTART 0x8F9D
#define GL_PRIMITIVE_RESTART_INDEX 0x8F9E
#define GL_COPY_READ_BUFFER 0x8F36
#define GL_COPY_WRITE_BUFFER 0x8F37
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_UNIFORM_BUFFER_BINDING 0x8A28
#define GL_UNIFORM_BUFFER_START 0x8A29
#define GL_UNIFORM_BUFFER_SIZE 0x8A2A
#define GL_MAX_VERTEX_UNIFORM_BLOCKS 0x8A2B
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS 0x8A2C
#define GL_MAX_FRAGMENT_UNIFORM_BLOCKS 0x8A2D
#define GL_MAX_COMBINED_UNIFORM_BLOCKS 0x8A2E
#define GL_MAX_UNIFORM_BUFFER_BINDINGS 0x8A2F
#define GL_MAX_UNIFORM_BLOCK_SIZE 0x8A30
#define GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS 0x8A31
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS 0x8A32
#define GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS 0x8A33
#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT 0x8A34
#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH 0x8A35
#define GL_ACTIVE_UNIFORM_BLOCKS 0x8A36
#define GL_UNIFORM_TYPE 0x8A37
#define GL_UNIFORM_SIZE 0x8A38
#define GL_UNIFORM_NAME_LENGTH 0x8A39
#define GL_UNIFORM_BLOCK_INDEX 0x8A3A
#define GL_UNIFORM_OFFSET 0x8A3B
#define GL_UNIFORM_ARRAY_STRIDE 0x8A3C
#define GL_UNIFORM_MATRIX_STRIDE 0x8A3D
#define GL_UNIFORM_IS_ROW_MAJOR 0x8A3E
#define GL_UNIFORM_BLOCK_BINDING 0x8A3F
#define GL_UNIFORM_BLOCK_DATA_SIZE 0x8A40
#define GL_UNIFORM_BLOCK_NAME_LENGTH 0x8A41
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS 0x8A42
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES 0x8A43
#define GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER 0x8A44
#define GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER 0x8A45
#define GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER 0x8A46
#define GL_INVALID_INDEX 0xFFFFFFFF
#define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_LINES_ADJACENCY 0x000A
#define GL_LINE_STRIP_ADJACENCY 0x000B
#define GL_TRIANGLES_ADJACENCY 0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY 0x000D
#define GL_PROGRAM_POINT_SIZE 0x8642
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS 0x8C29
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED 0x8DA7
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
#define GL_GEOMETRY_SHADER 0x8DD9
#define GL_GEOMETRY_VERTICES_OUT 0x8916
#define GL_GEOMETRY_INPUT_TYPE 0x8917
#define GL_GEOMETRY_OUTPUT_TYPE 0x8918
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS 0x8DDF
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES 0x8DE0
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS 0x8DE1
#define GL_MAX_VERTEX_OUTPUT_COMPONENTS 0x9122
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS 0x9123
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS 0x9124
#define GL_MAX_FRAGMENT_INPUT_COMPONENTS 0x9125
#define GL_CONTEXT_PROFILE_MASK 0x9126
#define GL_DEPTH_CLAMP 0x864F
#define GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION 0x8E4C
#define GL_FIRST_VERTEX_CONVENTION 0x8E4D
#define GL_LAST_VERTEX_CONVENTION 0x8E4E
#define GL_PROVOKING_VERTEX 0x8E4F
#define GL_TEXTURE_CUBE_MAP_SEAMLESS 0x884F
#define GL_MAX_SERVER_WAIT_TIMEOUT 0x9111
#define GL_OBJECT_TYPE 0x9112
#define GL_SYNC_CONDITION 0x9113
#define GL_SYNC_STATUS 0x9114
#define GL_SYNC_FLAGS 0x9115
#define GL_SYNC_FENCE 0x9116
#define GL_SYNC_GPU_COMMANDS_COMPLETE 0x9117
#define GL_UNSIGNALED 0x9118
#define GL_SIGNALED 0x9119
#define GL_ALREADY_SIGNALED 0x911A
#define GL_TIMEOUT_EXPIRED 0x911B
#define GL_CONDITION_SATISFIED 0x911C
#define GL_WAIT_FAILED 0x911D
#define GL_TIMEOUT_IGNORED 0xFFFFFFFFFFFFFFFF
#define GL_SYNC_FLUSH_COMMANDS_BIT 0x00000001
#define GL_SAMPLE_POSITION 0x8E50
#define GL_SAMPLE_MASK 0x8E51
#define GL_SAMPLE_MASK_VALUE 0x8E52
#define GL_MAX_SAMPLE_MASK_WORDS 0x8E59
#define GL_TEXTURE_2D_MULTISAMPLE 0x9100
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE 0x9101
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9102
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9103
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE 0x9104
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY 0x9105
#define GL_TEXTURE_SAMPLES 0x9106
#define GL_TEXTURE_FIXED_SAMPLE_LOCATIONS 0x9107
#define GL_SAMPLER_2D_MULTISAMPLE 0x9108
#define GL_INT_SAMPLER_2D_MULTISAMPLE 0x9109
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE 0x910A
#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910B
#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910C
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910D
#define GL_MAX_COLOR_TEXTURE_SAMPLES 0x910E
#define GL_MAX_DEPTH_TEXTURE_SAMPLES 0x910F
#define GL_MAX_INTEGER_SAMPLES 0x9110
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR 0x88FE
#define GL_SRC1_COLOR 0x88F9
#define GL_ONE_MINUS_SRC1_COLOR 0x88FA
#define GL_ONE_MINUS_SRC1_ALPHA 0x88FB
#define GL_MAX_DUAL_SOURCE_DRAW_BUFFERS 0x88FC
#define GL_ANY_SAMPLES_PASSED 0x8C2F
#define GL_SAMPLER_BINDING 0x8919
#define GL_RGB10_A2UI 0x906F
#define GL_TEXTURE_SWIZZLE_R 0x8E42
#define GL_TEXTURE_SWIZZLE_G 0x8E43
#define GL_TEXTURE_SWIZZLE_B 0x8E44
#define GL_TEXTURE_SWIZZLE_A 0x8E45
#define GL_TEXTURE_SWIZZLE_RGBA 0x8E46
#define GL_TIME_ELAPSED 0x88BF
#define GL_TIMESTAMP 0x8E28
#define GL_INT_2_10_10_10_REV 0x8D9F
#define GL_LUMINANCE 0x1909
#define GL_LUMINANCE_ALPHA 0x190A
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 0x8CD9

#define GL_TEXTURE_ANISOTROPIC_FILTER 0x3000 // Anisotropic filter (custom identifier)
#define GL_MIRROR_CLAMP_EXT           0x8742 // GL_MIRROR_CLAMP_EXT
#define GL_MODELVIEW                  0x1700 // GL_MODELVIEW
#define GL_PROJECTION                 0x1701 // GL_PROJECTION
#define GL_QUADS                      0x0007 // GL_QUADS

#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#define GL_ETC1_RGB8_OES 0x8D64
#define GL_COMPRESSED_RGB8_ETC2 0x9274
#define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#define GL_COMPRESSED_RGBA_ASTC_4x4_KHR 0x93b0
#define GL_COMPRESSED_RGBA_ASTC_8x8_KHR 0x93b7
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE

#pragma endregion

#pragma region internal renderer functions

// Compile custom shader and return shader id
rf_internal unsigned int rf_compile_shader(const char* shader_str, int type)
{
    unsigned int shader = rf_gl.CreateShader(type);
    rf_gl.ShaderSource(shader, 1, &shader_str, NULL);

    int success = 0;
    rf_gl.CompileShader(shader);
    rf_gl.GetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success != GL_TRUE)
    {
        rf_log(rf_log_type_warning, "[SHDR ID %i] Failed to compile shader...", shader);
        int max_len = 0;
        int length;
        rf_gl.GetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_len);

        //@Note: Buffer may not be big enough for some messages
        char log[1024];

        rf_gl.GetShaderInfoLog(shader, 1024, &length, log);

        rf_log(rf_log_type_info, "%s", log);
    }
    else rf_log(rf_log_type_info, "[SHDR ID %i] rf_shader compiled successfully", shader);

    return shader;
}

// Load custom shader strings and return program id
rf_internal unsigned int rf_load_shader_program(unsigned int v_shader_id, unsigned int f_shader_id)
{
    unsigned int program = 0;

    int success = 0;
    program = rf_gl.CreateProgram();

    rf_gl.AttachShader(program, v_shader_id);
    rf_gl.AttachShader(program, f_shader_id);

    // NOTE: Default attribute shader locations must be binded before linking
    rf_gl.BindAttribLocation(program, 0, RF_DEFAULT_ATTRIB_POSITION_NAME);
    rf_gl.BindAttribLocation(program, 1, RF_DEFAULT_ATTRIB_TEXCOORD_NAME);
    rf_gl.BindAttribLocation(program, 2, RF_DEFAULT_ATTRIB_NORMAL_NAME);
    rf_gl.BindAttribLocation(program, 3, RF_DEFAULT_ATTRIB_COLOR_NAME);
    rf_gl.BindAttribLocation(program, 4, RF_DEFAULT_ATTRIB_TANGENT_NAME);
    rf_gl.BindAttribLocation(program, 5, RF_DEFAULT_ATTRIB_TEXCOORD2_NAME);

    // NOTE: If some attrib name is no found on the shader, it locations becomes -1

    rf_gl.LinkProgram(program);

    // NOTE: All uniform variables are intitialised to 0 when a program links

    rf_gl.GetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        rf_log(rf_log_type_warning, "[SHDR ID %i] Failed to link shader program...", program);

        int max_len = 0;
        int length;

        rf_gl.GetProgramiv(program, GL_INFO_LOG_LENGTH, &max_len);

        char log[1024];

        rf_gl.GetProgramInfoLog(program, 1024, &length, log);

        rf_log(rf_log_type_info, "%s", log);

        rf_gl.DeleteProgram(program);

        program = 0;
    }
    else rf_log(rf_log_type_info, "[SHDR ID %i] rf_shader program loaded successfully", program);

    return program;
}

// Get location handlers to for shader attributes and uniforms. Note: If any location is not found, loc point becomes -1
rf_internal void rf_set_shader_default_locations(rf_shader* shader)
{
    // NOTE: Default shader attrib locations have been fixed before linking:
    //          vertex position location    = 0
    //          vertex texcoord location    = 1
    //          vertex normal location      = 2
    //          vertex color location       = 3
    //          vertex tangent location     = 4
    //          vertex texcoord2 location   = 5

    // Get handles to GLSL input attibute locations
    shader->locs[RF_LOC_VERTEX_POSITION] = rf_gl.GetAttribLocation(shader->id, RF_DEFAULT_ATTRIB_POSITION_NAME);
    shader->locs[RF_LOC_VERTEX_TEXCOORD01] = rf_gl.GetAttribLocation(shader->id, RF_DEFAULT_ATTRIB_TEXCOORD_NAME);
    shader->locs[RF_LOC_VERTEX_TEXCOORD02] = rf_gl.GetAttribLocation(shader->id, RF_DEFAULT_ATTRIB_TEXCOORD2_NAME);
    shader->locs[RF_LOC_VERTEX_NORMAL] = rf_gl.GetAttribLocation(shader->id, RF_DEFAULT_ATTRIB_NORMAL_NAME);
    shader->locs[RF_LOC_VERTEX_TANGENT] = rf_gl.GetAttribLocation(shader->id, RF_DEFAULT_ATTRIB_TANGENT_NAME);
    shader->locs[RF_LOC_VERTEX_COLOR] = rf_gl.GetAttribLocation(shader->id, RF_DEFAULT_ATTRIB_COLOR_NAME);

    // Get handles to GLSL uniform locations (vertex shader)
    shader->locs[RF_LOC_MATRIX_MVP]  = rf_gl.GetUniformLocation(shader->id, "mvp");
    shader->locs[RF_LOC_MATRIX_PROJECTION]  = rf_gl.GetUniformLocation(shader->id, "projection");
    shader->locs[RF_LOC_MATRIX_VIEW]  = rf_gl.GetUniformLocation(shader->id, "view");

    // Get handles to GLSL uniform locations (fragment shader)
    shader->locs[RF_LOC_COLOR_DIFFUSE] = rf_gl.GetUniformLocation(shader->id, "col_diffuse");
    shader->locs[RF_LOC_MAP_DIFFUSE] = rf_gl.GetUniformLocation(shader->id, "texture0");
    shader->locs[RF_LOC_MAP_SPECULAR] = rf_gl.GetUniformLocation(shader->id, "texture1");
    shader->locs[RF_LOC_MAP_NORMAL] = rf_gl.GetUniformLocation(shader->id, "texture2");
}

// Unload default shader
rf_internal void rf_unlock_shader_default()
{
    rf_gl.UseProgram(0);

    rf_gl.DetachShader(rf_ctx.default_shader.id, rf_ctx.default_vertex_shader_id);
    rf_gl.DetachShader(rf_ctx.default_shader.id, rf_ctx.default_frag_shader_id);
    rf_gl.DeleteShader(rf_ctx.default_vertex_shader_id);
    rf_gl.DeleteShader(rf_ctx.default_frag_shader_id);

    rf_gl.DeleteProgram(rf_ctx.default_shader.id);
}

// Draw default internal buffers vertex data
rf_internal void rf_draw_buffers_default()
{

}

// Unload default internal buffers vertex data from CPU and GPU
rf_internal void rf_unload_buffers_default()
{
    // Unbind everything
    rf_gl.BindVertexArray(0);
    rf_gl.DisableVertexAttribArray(0);
    rf_gl.DisableVertexAttribArray(1);
    rf_gl.DisableVertexAttribArray(2);
    rf_gl.DisableVertexAttribArray(3);
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, 0);
    rf_gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    for (rf_int i = 0; i < RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT; i++)
    {
        // Delete VBOs from GPU (VRAM)
        rf_gl.DeleteBuffers(1, &rf_batch.vertex_buffers[i].vbo_id[0]);
        rf_gl.DeleteBuffers(1, &rf_batch.vertex_buffers[i].vbo_id[1]);
        rf_gl.DeleteBuffers(1, &rf_batch.vertex_buffers[i].vbo_id[2]);
        rf_gl.DeleteBuffers(1, &rf_batch.vertex_buffers[i].vbo_id[3]);

        // Delete VAOs from GPU (VRAM)
        rf_gl.DeleteVertexArrays(1, &rf_batch.vertex_buffers[i].vao_id);
    }
}

// Renders a 1x1 XY quad in NDC
rf_internal void rf_gen_draw_quad(void)
{
    unsigned int quad_vao = 0;
    unsigned int quad_vbo = 0;

    float vertices[] = {
            // Positions        // rf_texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    // Set up plane VAO
    rf_gl.GenVertexArrays(1, &quad_vao);
    rf_gl.GenBuffers(1, &quad_vbo);
    rf_gl.BindVertexArray(quad_vao);

    // Fill buffer
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // Link vertex attributes
    rf_gl.EnableVertexAttribArray(0);
    rf_gl.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void* )0);
    rf_gl.EnableVertexAttribArray(1);
    rf_gl.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void* )(3 * sizeof(float)));

    // Draw quad
    rf_gl.BindVertexArray(quad_vao);
    rf_gl.DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    rf_gl.BindVertexArray(0);

    rf_gl.DeleteBuffers(1, &quad_vbo);
    rf_gl.DeleteVertexArrays(1, &quad_vao);
}

// Renders a 1x1 3D cube in NDC
rf_internal void rf_gen_draw_cube(void)
{
    unsigned int cube_vao = 0;
    unsigned int cube_vbo = 0;

    static float vertices[] = {
            -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f , 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };

    // Set up cube VAO
    rf_gl.GenVertexArrays(1, &cube_vao);
    rf_gl.GenBuffers(1, &cube_vbo);

    // Fill buffer
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Link vertex attributes
    rf_gl.BindVertexArray(cube_vao);
    rf_gl.EnableVertexAttribArray(0);
    rf_gl.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void* )0);
    rf_gl.EnableVertexAttribArray(1);
    rf_gl.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void* )(3 * sizeof(float)));
    rf_gl.EnableVertexAttribArray(2);
    rf_gl.VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void* )(6 * sizeof(float)));
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, 0);
    rf_gl.BindVertexArray(0);

    // Draw cube
    rf_gl.BindVertexArray(cube_vao);
    rf_gl.DrawArrays(GL_TRIANGLES, 0, 36);
    rf_gl.BindVertexArray(0);

    rf_gl.DeleteBuffers(1, &cube_vbo);
    rf_gl.DeleteVertexArrays(1, &cube_vao);
}

rf_internal void rf_set_gl_extension_if_available(const char* gl_ext, int len)
{
    #if defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
        // Check NPOT textures support
        // NOTE: Only check on OpenGL ES, OpenGL 3.3 has NPOT textures full support as core feature
        if (rf_match_str_cstr(gl_ext, len, "GL_OES_texture_npot")) {
            rf_gfx.extensions.tex_npot_supported = 1;
        }

        // Check texture float support
        if (rf_match_str_cstr(gl_ext, len, "GL_OES_texture_float")) {
            rf_gfx.extensions.tex_float_supported = 1;
        }

        // Check depth texture support
        if ((rf_match_str_cstr(gl_ext, len, "GL_OES_depth_texture")) ||
            (rf_match_str_cstr(gl_ext, len, "GL_WEBGL_depth_texture"))) {
            rf_gfx.extensions.tex_depth_supported = 1;
        }

        if (rf_match_str_cstr(gl_ext, len, "GL_OES_depth24")) {
            rf_gfx.extensions.max_depth_bits = 24;
        }

        if (rf_match_str_cstr(gl_ext, len, "GL_OES_depth32")) {
            rf_gfx.extensions.max_depth_bits = 32;
        }
    #endif

    // DDS texture compression support
    if (rf_match_str_cstr(gl_ext, len, "GL_EXT_texture_compression_s3tc") ||
        rf_match_str_cstr(gl_ext, len, "GL_WEBGL_compressed_texture_s3tc") ||
        rf_match_str_cstr(gl_ext, len, "GL_WEBKIT_WEBGL_compressed_texture_s3tc")) {
        rf_gfx.extensions.tex_comp_dxt_supported = 1;
    }

    // ETC1 texture compression support
    if (rf_match_str_cstr(gl_ext, len, "GL_OES_compressed_ETC1_RGB8_texture") ||
        rf_match_str_cstr(gl_ext, len, "GL_WEBGL_compressed_texture_etc1")) {
        rf_gfx.extensions.tex_comp_etc1_supported = 1;
    }

    // ETC2/EAC texture compression support
    if (rf_match_str_cstr(gl_ext, len, "GL_ARB_ES3_compatibility")) {
        rf_gfx.extensions.tex_comp_etc2_supported = 1;
    }

    // PVR texture compression support
    if (rf_match_str_cstr(gl_ext, len, "GL_IMG_texture_compression_pvrtc")) {
        rf_gfx.extensions.tex_comp_pvrt_supported = 1;
    }

    // ASTC texture compression support
    if (rf_match_str_cstr(gl_ext, len, "GL_KHR_texture_compression_astc_hdr")) {
        rf_gfx.extensions.tex_comp_astc_supported = 1;
    }

    // Anisotropic texture filter support
    if (rf_match_str_cstr(gl_ext, len, "GL_EXT_texture_filter_anisotropic")) {
        rf_gfx.extensions.tex_anisotropic_filter_supported = 1;
        rf_gl.GetFloatv(0x84FF, &rf_gfx.extensions.max_anisotropic_level); // GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
    }

    // Clamp mirror wrap mode supported
    if (rf_match_str_cstr(gl_ext, len, "GL_EXT_texture_mirror_clamp")) {
        rf_gfx.extensions.tex_mirror_clamp_supported = 1;
    }

    // Debug marker support
    if (rf_match_str_cstr(gl_ext, len, "GL_EXT_debug_marker")) {
        rf_gfx.extensions.debug_marker_supported = 1;
    }
}

#pragma endregion

#pragma region init

rf_internal void rf__gfx_backend_internal_init(rf_gfx_backend_data* gfx_data)
{
    rf_gfx.gl = *((rf_opengl_procs*) gfx_data);
    rf_gfx.extensions.max_depth_bits = 16;

    // Check for extensions
    {
        int num_ext = 0;

        #if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
        {
            // Multiple texture extensions supported by default
            rf_gfx.extensions.tex_npot_supported  = 1;
            rf_gfx.extensions.tex_float_supported = 1;
            rf_gfx.extensions.tex_depth_supported = 1;

            rf_gl.GetIntegerv(GL_NUM_EXTENSIONS, &num_ext);

            for (rf_int i = 0; i < num_ext; i++)
            {
                const char *ext = (const char *) rf_gl.GetStringi(GL_EXTENSIONS, i);
                rf_set_gl_extension_if_available(ext, strlen(ext));
            }
        }
        #elif defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
        {
            //Note: How this works is that we get one big string formated like "gl_ext_func_name1 gl_ext_func_name2 ..."
            //All function names are separated by a space, so we just take a pointer to the begin and advance until we hit a space
            //At which point we get the length, do the comparison, and set the ptr of the next extension to the next element.
            const char* extensions_str = (const char*) rf_gl.GetString(GL_EXTENSIONS);  // One big const string
            if (extensions_str)
            {
                const char* curr_ext = extensions_str;

                while (*extensions_str)
                {
                    //If we get to a space that means we got a new extension name
                    if (*extensions_str == ' ')
                    {
                        num_ext++;
                        const int curr_ext_len = (int) (extensions_str - curr_ext);
                        rf_set_gl_extension_if_available(curr_ext, curr_ext_len);
                        curr_ext = extensions_str + 1;
                    }

                    extensions_str++;
                }

                if (rf_gfx.extensions.tex_npot_supported) rf_log(rf_log_type_info, "[EXTENSION] NPOT textures extension detected, full NPOT textures supported");
                else rf_log(rf_log_type_warning, "[EXTENSION] NPOT textures extension not found, limited NPOT support (no-mipmaps, no-repeat)");
            }
        }
        #endif

        rf_log(rf_log_type_info, "Number of supported extensions: %i.", num_ext);

        if (rf_gfx.extensions.tex_comp_dxt_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] DXT compressed textures supported");
        }

        if (rf_gfx.extensions.tex_comp_etc1_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] ETC1 compressed textures supported");
        }

        if (rf_gfx.extensions.tex_comp_etc2_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] ETC2/EAC compressed textures supported");
        }

        if (rf_gfx.extensions.tex_comp_pvrt_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] PVRT compressed textures supported");
        }

        if (rf_gfx.extensions.tex_comp_astc_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] ASTC compressed textures supported");
        }

        if (rf_gfx.extensions.tex_anisotropic_filter_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] Anisotropic textures filtering supported (max: %.0fX)", rf_gfx.extensions.max_anisotropic_level);
        }

        if (rf_gfx.extensions.tex_mirror_clamp_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] Mirror clamp wrap texture mode supported");
        }

        if (rf_gfx.extensions.debug_marker_supported)
        {
            rf_log(rf_log_type_info, "[GL EXTENSION] Debug Marker supported");
        }
    }

    // Initialize OpenGL default states
    {
        // Init state: Depth test
        rf_gl.DepthFunc(GL_LEQUAL);                                 // Type of depth testing to apply
        rf_gl.Disable(GL_DEPTH_TEST);                               // Disable depth testing for 2D (only used for 3D)

        // Init state: Blending mode
        rf_gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // rf_color blending function (how colors are mixed)
        rf_gl.Enable(GL_BLEND);                                     // Enable color blending (required to work with transparencies)

        // Init state: Culling
        // NOTE: All shapes/models triangles are drawn CCW
        rf_gl.CullFace(GL_BACK);                                    // Cull the back face (default)
        rf_gl.FrontFace(GL_CCW);                                    // Front face are defined counter clockwise (default)
        rf_gl.Enable(GL_CULL_FACE);                                 // Enable backface culling

        // Init state: rf_color/Depth buffers clear
        rf_gl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // Set clear color (black)
        rf_gl_ClearDepth(1.0f);                                     // Set clear depth value (default)
        rf_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear color and depth buffers (depth buffer required for 3D)
    }
}

#pragma endregion

#pragma region shader

// Load default shader (just vertex positioning and texture coloring). Note: This shader program is used for internal buffers
rf_public rf_shader rf_load_default_shader()
{
    rf_shader shader = { 0 };
    memset(shader.locs, 0, sizeof(shader.locs));

    // NOTE: All locations must be reseted to -1 (no location)
    for (rf_int i = 0; i < RF_MAX_SHADER_LOCATIONS; i++) shader.locs[i] = -1;

    // Vertex shader directly defined, no external file required
    const char* default_vertex_shader_str =
            #if defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
            "#version 100\n"
            "attribute vec3 vertex_position;"
            "attribute vec2 vertex_tex_coord;"
            "attribute vec4 vertex_color;"
            "varying vec2 frag_tex_coord;"
            "varying vec4 frag_color;"
            #elif defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
            "#version 330\n"
            "in vec3 vertex_position;"
            "in vec2 vertex_tex_coord;"
            "in vec4 vertex_color;"
            "out vec2 frag_tex_coord;"
            "out vec4 frag_color;"
            #endif
            "uniform mat4 mvp;"
            "void main()"
            "{"
            "    frag_tex_coord = vertex_tex_coord;"
            "    frag_color = vertex_color;"
            "    gl_Position = mvp*vec4(vertex_position, 1.0);"
            "}";

    // Fragment shader directly defined, no external file required
    const char* default_fragment_shader_str =
            #if defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
            "#version 100\n"
            "precision mediump float;"
            "varying vec2 frag_tex_coord;"
            "varying vec4 frag_color;"
            #elif defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
            "#version 330\n"
            "precision mediump float;"
            "in vec2 frag_tex_coord;"
            "in vec4 frag_color;"
            "out vec4 final_color;"
            #endif
            "uniform sampler2D texture0;"
            "uniform vec4 col_diffuse;"
            "void main()"
            "{"
            #if defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
            "    vec4 texel_color = texture2D(texture0, frag_tex_coord);" // NOTE: texture2D() is deprecated on OpenGL 3.3 and ES 3.0
            "    frag_color = texel_color*col_diffuse*frag_color;"
            #elif defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
            "    vec4 texel_color = texture(texture0, frag_tex_coord);"
            "    final_color = texel_color*col_diffuse*frag_color;"
            #endif
            "}";

    // NOTE: Compiled vertex/fragment shaders are kept for re-use
    rf_ctx.default_vertex_shader_id = rf_compile_shader(default_vertex_shader_str, GL_VERTEX_SHADER);     // Compile default vertex shader
    rf_ctx.default_frag_shader_id = rf_compile_shader(default_fragment_shader_str, GL_FRAGMENT_SHADER);   // Compile default fragment shader

    shader.id = rf_load_shader_program(rf_ctx.default_vertex_shader_id, rf_ctx.default_frag_shader_id);

    if (shader.id > 0)
    {
        rf_log(rf_log_type_info, "[SHDR ID %i] Default shader loaded successfully", shader.id);

        // Set default shader locations: attributes locations
        shader.locs[RF_LOC_VERTEX_POSITION] = rf_gl.GetAttribLocation(shader.id, "vertex_position");
        shader.locs[RF_LOC_VERTEX_TEXCOORD01] = rf_gl.GetAttribLocation(shader.id, "vertex_tex_coord");
        shader.locs[RF_LOC_VERTEX_COLOR] = rf_gl.GetAttribLocation(shader.id, "vertex_color");

        // Set default shader locations: uniform locations
        shader.locs[RF_LOC_MATRIX_MVP]  = rf_gl.GetUniformLocation(shader.id, "mvp");
        shader.locs[RF_LOC_COLOR_DIFFUSE] = rf_gl.GetUniformLocation(shader.id, "col_diffuse");
        shader.locs[RF_LOC_MAP_DIFFUSE] = rf_gl.GetUniformLocation(shader.id, "texture0");

        // NOTE: We could also use below function but in case RF_DEFAULT_ATTRIB_* points are
        // changed for external custom shaders, we just use direct bindings above
        //rf_set_shader_default_locations(&shader);
    }
    else rf_log(rf_log_type_warning, "[SHDR ID %i] Default shader could not be loaded", shader.id);

    return shader;
}

// Load shader from code strings. If shader string is NULL, using default vertex/fragment shaders
rf_public rf_shader rf_gfx_load_shader(const char* vs_code, const char* fs_code)
{
    rf_shader shader = { 0 };
    memset(shader.locs, -1, sizeof(shader.locs));

    unsigned int vertex_shader_id   = rf_ctx.default_vertex_shader_id;
    unsigned int fragment_shader_id = rf_ctx.default_frag_shader_id;

    if (vs_code != NULL) vertex_shader_id   = rf_compile_shader(vs_code, GL_VERTEX_SHADER);
    if (fs_code != NULL) fragment_shader_id = rf_compile_shader(fs_code, GL_FRAGMENT_SHADER);

    if ((vertex_shader_id == rf_ctx.default_vertex_shader_id) && (fragment_shader_id == rf_ctx.default_frag_shader_id)) shader = rf_ctx.default_shader;
    else
    {
        shader.id = rf_load_shader_program(vertex_shader_id, fragment_shader_id);

        if (vertex_shader_id != rf_ctx.default_vertex_shader_id) rf_gl.DeleteShader(vertex_shader_id);
        if (fragment_shader_id != rf_ctx.default_frag_shader_id) rf_gl.DeleteShader(fragment_shader_id);

        if (shader.id == 0)
        {
            rf_log(rf_log_type_warning, "Custom shader could not be loaded");
            shader = rf_ctx.default_shader;
        }

        // After shader loading, we TRY to set default location names
        if (shader.id > 0) rf_set_shader_default_locations(&shader);
    }

    // Get available shader uniforms
    // NOTE: This information is useful for debug...
    int uniform_count = -1;

    rf_gl.GetProgramiv(shader.id, GL_ACTIVE_UNIFORMS, &uniform_count);

    for (rf_int i = 0; i < uniform_count; i++)
    {
        int namelen = -1;
        int num = -1;
        char name[256]; // Assume no variable names longer than 256
        unsigned int type = GL_ZERO;

        // Get the name of the uniforms
        rf_gl.GetActiveUniform(shader.id, i,sizeof(name) - 1, &namelen, &num, &type, name);

        name[namelen] = 0;

        // Get the location of the named uniform
        unsigned int location = rf_gl.GetUniformLocation(shader.id, name);

        rf_log(rf_log_type_debug, "[SHDR ID %i] Active uniform [%s] set at location: %i", shader.id, name, location);
    }

    return shader;
}

// Unload shader from GPU memory (VRAM)
rf_public void rf_gfx_unload_shader(rf_shader shader)
{
    if (shader.id > 0)
    {
        rf_gfx_delete_shader(shader.id);
        rf_log(rf_log_type_info, "[SHDR ID %i] Unloaded shader program data", shader.id);
    }
}

// Get shader uniform location
rf_public int rf_gfx_get_shader_location(rf_shader shader, const char* uniform_name)
{
    int location = -1;
    location = rf_gl.GetUniformLocation(shader.id, uniform_name);

    if (location == -1) rf_log(rf_log_type_warning, "[SHDR ID %i][%s] rf_shader uniform could not be found", shader.id, uniform_name);
    else rf_log(rf_log_type_info, "[SHDR ID %i][%s] rf_shader uniform set at location: %i", shader.id, uniform_name, location);

    return location;
}

// Set shader uniform value
rf_public void rf_gfx_set_shader_value(rf_shader shader, int uniform_loc, const void* value, int uniform_name)
{
    rf_gfx_set_shader_value_v(shader, uniform_loc, value, uniform_name, 1);
}

// Set shader uniform value vector
rf_public void rf_gfx_set_shader_value_v(rf_shader shader, int uniform_loc, const void* value, int uniform_name, int count)
{
    rf_gl.UseProgram(shader.id);

    switch (uniform_name)
    {
        case RF_UNIFORM_FLOAT: rf_gl.Uniform1fv(uniform_loc, count, (float* )value); break;
        case RF_UNIFORM_VEC2: rf_gl.Uniform2fv(uniform_loc, count, (float* )value); break;
        case RF_UNIFORM_VEC3: rf_gl.Uniform3fv(uniform_loc, count, (float* )value); break;
        case RF_UNIFORM_VEC4: rf_gl.Uniform4fv(uniform_loc, count, (float* )value); break;
        case RF_UNIFORM_INT: rf_gl.Uniform1iv(uniform_loc, count, (int* )value); break;
        case RF_UNIFORM_IVEC2: rf_gl.Uniform2iv(uniform_loc, count, (int* )value); break;
        case RF_UNIFORM_IVEC3: rf_gl.Uniform3iv(uniform_loc, count, (int* )value); break;
        case RF_UNIFORM_IVEC4: rf_gl.Uniform4iv(uniform_loc, count, (int* )value); break;
        case RF_UNIFORM_SAMPLER2D: rf_gl.Uniform1iv(uniform_loc, count, (int* )value); break;
        default: rf_log(rf_log_type_warning, "rf_shader uniform could not be set data type not recognized");
    }

    //rf_gl.UseProgram(0);      // Avoid reseting current shader program, in case other uniforms are set
}

// Set shader uniform value (matrix 4x4)
rf_public void rf_gfx_set_shader_value_matrix(rf_shader shader, int uniform_loc, rf_mat mat)
{
    rf_gl.UseProgram(shader.id);

    rf_gl.UniformMatrix4fv(uniform_loc, 1, 0, rf_mat_to_float16(mat).v);

    //rf_gl.UseProgram(0);
}

// Set shader uniform value for texture
rf_public void rf_gfx_set_shader_value_texture(rf_shader shader, int uniform_loc, rf_texture2d texture)
{
    rf_gl.UseProgram(shader.id);

    rf_gl.Uniform1i(uniform_loc, texture.id);

    //rf_gl.UseProgram(0);
}

// Return internal rf_ctx->gl_ctx.projection matrix
rf_public rf_mat rf_gfx_get_matrix_projection() {
    return rf_ctx.projection;
}

// Return internal rf_ctx->gl_ctx.modelview matrix
rf_public rf_mat rf_gfx_get_matrix_modelview()
{
    rf_mat matrix = rf_mat_identity();
    matrix = rf_ctx.modelview;
    return matrix;
}

// Set a custom projection matrix (replaces internal rf_ctx->gl_ctx.projection matrix)
rf_public void rf_gfx_set_matrix_projection(rf_mat proj)
{
    rf_ctx.projection = proj;
}

// Set a custom rf_ctx->gl_ctx.modelview matrix (replaces internal rf_ctx->gl_ctx.modelview matrix)
rf_public void rf_gfx_set_matrix_modelview(rf_mat view)
{
    rf_ctx.modelview = view;
}

#pragma endregion

#pragma region rf_gfx

// Choose the blending mode (alpha, additive, multiplied)
rf_public void rf_gfx_blend_mode(rf_blend_mode mode)
{
    rf_gfx_draw();

    switch (mode)
    {
        case RF_BLEND_ALPHA: rf_gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
        case RF_BLEND_ADDITIVE: rf_gl.BlendFunc(GL_SRC_ALPHA, GL_ONE); break; // Alternative: glBlendFunc(GL_ONE, GL_ONE);
        case RF_BLEND_MULTIPLIED: rf_gl.BlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA); break;
        default: break;
    }

    rf_ctx.blend_mode = mode;
}

// Choose the current matrix to be transformed
rf_public void rf_gfx_matrix_mode(rf_matrix_mode mode)
{
    if (mode == GL_PROJECTION) rf_ctx.current_matrix = &rf_ctx.projection;
    else if (mode == GL_MODELVIEW) rf_ctx.current_matrix = &rf_ctx.modelview;
    //else if (mode == GL_TEXTURE) // Not supported

    rf_ctx.current_matrix_mode = mode;
}

// Push the current matrix into rf_ctx->gl_ctx.stack
rf_public void rf_gfx_push_matrix()
{
    if (rf_ctx.stack_counter >= RF_MAX_MATRIX_STACK_SIZE) rf_log_error(rf_limit_reached, "Matrix stack limit reached.");

    if (rf_ctx.current_matrix_mode == GL_MODELVIEW)
    {
        rf_ctx.transform_matrix_required = 1;
        rf_ctx.current_matrix = &rf_ctx.transform;
    }

    rf_ctx.stack[rf_ctx.stack_counter] = *rf_ctx.current_matrix;
    rf_ctx.stack_counter++;
}

// Pop lattest inserted matrix from rf_ctx->gl_ctx.stack
rf_public void rf_gfx_pop_matrix()
{
    if (rf_ctx.stack_counter > 0)
    {
        rf_mat mat = rf_ctx.stack[rf_ctx.stack_counter - 1];
        *rf_ctx.current_matrix = mat;
        rf_ctx.stack_counter--;
    }

    if ((rf_ctx.stack_counter == 0) && (rf_ctx.current_matrix_mode == GL_MODELVIEW))
    {
        rf_ctx.current_matrix = &rf_ctx.modelview;
        rf_ctx.transform_matrix_required = 0;
    }
}

// Reset current matrix to identity matrix
rf_public void rf_gfx_load_identity()
{
    *rf_ctx.current_matrix = rf_mat_identity();
}

// Multiply the current matrix by a translation matrix
rf_public void rf_gfx_translatef(float x, float y, float z)
{
    rf_mat mat_translation = rf_mat_translate(x, y, z);

    // NOTE: We transpose matrix with multiplication order
    *rf_ctx.current_matrix = rf_mat_mul(mat_translation, *rf_ctx.current_matrix);
}

// Multiply the current matrix by a rotation matrix
rf_public void rf_gfx_rotatef(float angleDeg, float x, float y, float z)
{
    rf_mat mat_rotation = rf_mat_identity();

    rf_vec3 axis = (rf_vec3){x, y, z };
    mat_rotation = rf_mat_rotate(rf_vec3_normalize(axis), angleDeg * rf_deg2rad);

    // NOTE: We transpose matrix with multiplication order
    *rf_ctx.current_matrix = rf_mat_mul(mat_rotation, *rf_ctx.current_matrix);
}

// Multiply the current matrix by a scaling matrix
rf_public void rf_gfx_scalef(float x, float y, float z)
{
    rf_mat mat_scale = rf_mat_scale(x, y, z);

    // NOTE: We transpose matrix with multiplication order
    *rf_ctx.current_matrix = rf_mat_mul(mat_scale, *rf_ctx.current_matrix);
}

// Multiply the current matrix by another matrix
rf_public void rf_gfx_mult_matrixf(float* matf)
{
    // rf_mat creation from array
    rf_mat mat = {matf[0], matf[4], matf[8], matf[12],
                  matf[1], matf[5], matf[9], matf[13],
                  matf[2], matf[6], matf[10], matf[14],
                  matf[3], matf[7], matf[11], matf[15] };

    *rf_ctx.current_matrix = rf_mat_mul(*rf_ctx.current_matrix, mat);
}

// Multiply the current matrix by a perspective matrix generated by parameters
rf_public void rf_gfx_frustum(double left, double right, double bottom, double top, double znear, double zfar)
{
    rf_mat mat_perps = rf_mat_frustum(left, right, bottom, top, znear, zfar);

    *rf_ctx.current_matrix = rf_mat_mul(*rf_ctx.current_matrix, mat_perps);
}

// Multiply the current matrix by an orthographic matrix generated by parameters
rf_public void rf_gfx_ortho(double left, double right, double bottom, double top, double znear, double zfar)
{
    rf_mat mat_ortho = rf_mat_ortho(left, right, bottom, top, znear, zfar);

    *rf_ctx.current_matrix = rf_mat_mul(*rf_ctx.current_matrix, mat_ortho);
}

// Set the viewport area (transformation from normalized device coordinates to window coordinates)
void rf_gfx_viewport(int x, int y, int width, int height)
{
    rf_gl.Viewport(x, y, width, height);
}

// Initialize drawing mode (how to organize vertex)
rf_public void rf_gfx_begin(rf_drawing_mode mode)
{
    // Draw mode can be GL_LINES, GL_TRIANGLES and GL_QUADS
    // NOTE: In all three cases, vertex are accumulated over default internal vertex buffer
    if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].mode != mode)
    {
        if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count > 0)
        {
            // Make sure current rf_ctx->gl_ctx.draws[i].vertex_count is aligned a multiple of 4,
            // that way, following QUADS drawing will keep aligned with index processing
            // It implies adding some extra alignment vertex at the end of the draw,
            // those vertex are not processed but they are considered as an additional offset
            // for the next set of vertex to be drawn
            if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].mode == GL_LINES) rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment = ((rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count < 4) ? rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count : rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count % 4);
            else if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].mode == GL_TRIANGLES) rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment = ((rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count < 4) ? 1 : (4 - (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count % 4)));

            else rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment = 0;

            if (rf_gfx_check_buffer_limit(rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment)) rf_gfx_draw();
            else
            {
                rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter += rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment;
                rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter += rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment;
                rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter += rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment;

                rf_batch.draw_calls_counter++;
            }
        }

        if (rf_batch.draw_calls_counter >= RF_DEFAULT_BATCH_DRAW_CALLS_COUNT) rf_gfx_draw();

        rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].mode = mode;
        rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count = 0;
        rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].texture_id = rf_ctx.default_texture_id;
    }
}

// Finish vertex providing
rf_public void rf_gfx_end()
{
    // Make sure vertex_count is the same for vertices, texcoords, colors and normals
    // NOTE: In OpenGL 1.1, one glColor call can be made for all the subsequent glVertex calls

    // Make sure colors count match vertex count
    if (rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter != rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter)
    {
        int add_colors = rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter - rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter;

        for (rf_int i = 0; i < add_colors; i++)
        {
            rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter] = rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter - 4];
            rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter + 1] = rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter - 3];
            rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter + 2] = rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter - 2];
            rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter + 3] = rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter - 1];
            rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter++;
        }
    }

    // Make sure texcoords count match vertex count
    if (rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter != rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter)
    {
        int add_tex_coords = rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter - rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter;

        for (rf_int i = 0; i < add_tex_coords; i++)
        {
            rf_batch.vertex_buffers[rf_batch.current_buffer].texcoords[2*rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter] = 0.0f;
            rf_batch.vertex_buffers[rf_batch.current_buffer].texcoords[2*rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter + 1] = 0.0f;
            rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter++;
        }
    }

    // TODO: Make sure normals count match vertex count... if normals support is added in a future... :P

    // NOTE: Depth increment is dependant on rf_gfx_ortho(): z-near and z-far values,
    // as well as depth buffer bit-depth (16bit or 24bit or 32bit)
    // Correct increment formula would be: depthInc = (zfar - znear)/pow(2, bits)
    rf_batch.current_depth += (1.0f/20000.0f);

    // Verify internal buffers limits
    // NOTE: This check is combined with usage of rf_gfx_check_buffer_limit()
    if ((rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter) >= (rf_batch.vertex_buffers[rf_batch.current_buffer].elements_count * 4 - 4))
    {
        // WARNING: If we are between rf_gfx_push_matrix() and rf_gfx_pop_matrix() and we need to force a rf_gfx_draw(),
        // we need to call rf_gfx_pop_matrix() before to recover *rf_ctx->gl_ctx.current_matrix (rf_ctx->gl_ctx.modelview) for the next forced draw call!
        // If we have multiple matrix pushed, it will require "rf_ctx->gl_ctx.stack_counter" pops before launching the draw
        for (rf_int i = rf_ctx.stack_counter; i >= 0; i--) rf_gfx_pop_matrix();
        rf_gfx_draw();
    }
}

// Define one vertex (position)
rf_public void rf_gfx_vertex2i(int x, int y)
{
    rf_gfx_vertex3f((float)x, (float)y, rf_batch.current_depth);
}

// Define one vertex (position)
rf_public void rf_gfx_vertex2f(float x, float y)
{
    rf_gfx_vertex3f(x, y, rf_batch.current_depth);
}

// Define one vertex (position)
// NOTE: Vertex position data is the basic information required for drawing
rf_public void rf_gfx_vertex3f(float x, float y, float z)
{
    rf_vec3 vec = {x, y, z };

    // rf_transform provided vector if required
    if (rf_ctx.transform_matrix_required) vec = rf_vec3_transform(vec, rf_ctx.transform);

    // Verify that rf_max_batch_elements limit not reached
    if (rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter < (rf_batch.vertex_buffers[rf_batch.current_buffer].elements_count * 4))
    {
        rf_batch.vertex_buffers[rf_batch.current_buffer].vertices[3*rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter] = vec.x;
        rf_batch.vertex_buffers[rf_batch.current_buffer].vertices[3*rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter + 1] = vec.y;
        rf_batch.vertex_buffers[rf_batch.current_buffer].vertices[3*rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter + 2] = vec.z;
        rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter++;

        rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count++;
    }
    else rf_log_error(rf_limit_reached, "Render batch elements limit reached. Max bacht elements: %d", rf_batch.vertex_buffers[rf_batch.current_buffer].elements_count * 4);
}

// Define one vertex (texture coordinate)
// NOTE: rf_texture coordinates are limited to QUADS only
rf_public void rf_gfx_tex_coord2f(float x, float y)
{
    rf_batch.vertex_buffers[rf_batch.current_buffer].texcoords[2*rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter] = x;
    rf_batch.vertex_buffers[rf_batch.current_buffer].texcoords[2*rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter + 1] = y;
    rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter++;
}

// Define one vertex (normal)
// NOTE: Normals limited to TRIANGLES only?
rf_public void rf_gfx_normal3f(float x, float y, float z)
{
    // TODO: Normals usage...
}

// Define one vertex (color)
rf_public void rf_gfx_color3f(float x, float y, float z)
{
    rf_gfx_color4ub((unsigned char)(x*255), (unsigned char)(y*255), (unsigned char)(z*255), 255);
}

// Define one vertex (color)
rf_public void rf_gfx_color4ub(unsigned char x, unsigned char y, unsigned char z, unsigned char w)
{
    rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter] = x;
    rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter + 1] = y;
    rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter + 2] = z;
    rf_batch.vertex_buffers[rf_batch.current_buffer].colors[4 * rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter + 3] = w;
    rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter++;
}

// Define one vertex (color)
rf_public void rf_gfx_color4f(float r, float g, float b, float a)
{
    rf_gfx_color4ub((unsigned char)(r*255), (unsigned char)(g*255), (unsigned char)(b*255), (unsigned char)(a*255));
}

// Enable texture usage
rf_public void rf_gfx_enable_texture(unsigned int id)
{
    if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].texture_id != id)
    {
        if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count > 0)
        {
            // Make sure current rf_ctx->gl_ctx.draws[i].vertex_count is aligned a multiple of 4,
            // that way, following QUADS drawing will keep aligned with index processing
            // It implies adding some extra alignment vertex at the end of the draw,
            // those vertex are not processed but they are considered as an additional offset
            // for the next set of vertex to be drawn
            if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].mode == RF_LINES)
            {
                rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment = ((rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count < 4) ? rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count : rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count % 4);
            }
            else if (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].mode == RF_TRIANGLES)
            {
                rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment = ((rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count < 4) ? 1 : (4 - (rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count % 4)));
            }
            else
            {
                rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment = 0;
            }

            if (rf_gfx_check_buffer_limit(rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment))
            {
                rf_gfx_draw();
            }
            else
            {
                rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter += rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment;
                rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter += rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment;
                rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter += rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_alignment;

                rf_batch.draw_calls_counter++;
            }
        }

        if (rf_batch.draw_calls_counter >= RF_DEFAULT_BATCH_DRAW_CALLS_COUNT)
        {
            rf_gfx_draw();
        }

        rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].texture_id = id;
        rf_batch.draw_calls[rf_batch.draw_calls_counter - 1].vertex_count = 0;
    }
}

// Disable texture usage
rf_public void rf_gfx_disable_texture()
{
    // NOTE: If quads batch limit is reached,
    // we force a draw call and next batch starts
    if (rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter >= (rf_batch.vertex_buffers[rf_batch.current_buffer].elements_count * 4))
    {
        rf_gfx_draw();
    }
}

// Set texture parameters (wrap mode/filter mode)
rf_public void rf_gfx_set_texture_wrap(rf_texture2d texture, rf_texture_wrap_mode wrap_mode)
{
    rf_gl.BindTexture(GL_TEXTURE_2D, texture.id);

    switch (wrap_mode)
    {
        case RF_WRAP_REPEAT:
        {
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        } break;

        case RF_WRAP_CLAMP:
        {
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        } break;

        case RF_WRAP_MIRROR_REPEAT:
        {
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        } break;

        case RF_WRAP_MIRROR_CLAMP:
        {
            if (rf_gfx.extensions.tex_mirror_clamp_supported)
            {
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_EXT);
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_EXT);
            }
            else
            {
                rf_log(rf_log_type_warning, "Clamp mirror wrap mode not supported");
            }
        } break;

        default: break;
    }

    rf_gl.BindTexture(GL_TEXTURE_2D, 0);
}

// Set filter for texture
rf_public void rf_gfx_set_texture_filter(rf_texture2d texture, rf_texture_filter_mode filter_mode)
{
    rf_gl.BindTexture(GL_TEXTURE_2D, texture.id);

    // Used only in case of anisotropic filters
    float anisotropic_value = 0;

    switch (filter_mode)
    {
        case RF_FILTER_POINT:
        {
            if (texture.mipmaps > 1)
            {
                // GL_NEAREST_MIPMAP_NEAREST - tex filter: POINT, mipmaps filter: POINT (sharp switching between mipmaps)
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

                // GL_NEAREST - tex filter: POINT (no filter), no mipmaps
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }
            else
            {
                // GL_NEAREST - tex filter: POINT (no filter), no mipmaps
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            }
        } break;

        case RF_FILTER_BILINEAR:
        {
            if (texture.mipmaps > 1)
            {
                // GL_LINEAR_MIPMAP_NEAREST - tex filter: BILINEAR, mipmaps filter: POINT (sharp switching between mipmaps)
                // Alternative: GL_NEAREST_MIPMAP_LINEAR - tex filter: POINT, mipmaps filter: BILINEAR (smooth transition between mipmaps)
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

                // GL_LINEAR - tex filter: BILINEAR, no mipmaps
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else
            {
                // GL_LINEAR - tex filter: BILINEAR, no mipmaps
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
        } break;

        case RF_FILTER_TRILINEAR:
        {
            if (texture.mipmaps > 1)
            {
                // GL_LINEAR_MIPMAP_LINEAR - tex filter: BILINEAR, mipmaps filter: BILINEAR (smooth transition between mipmaps)
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

                // GL_LINEAR - tex filter: BILINEAR, no mipmaps
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else
            {
                rf_log(rf_log_type_warning, "No mipmaps available for TRILINEAR texture filtering. Texture id: %d", texture.id);

                // GL_LINEAR - tex filter: BILINEAR, no mipmaps
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
        } break;

        case RF_FILTER_ANISOTROPIC_4x:  anisotropic_value = 4;
        case RF_FILTER_ANISOTROPIC_8x:  anisotropic_value = 8;
        case RF_FILTER_ANISOTROPIC_16x: anisotropic_value = 16;
        {
            if (anisotropic_value <= rf_gfx.extensions.max_anisotropic_level)
            {
                rf_gl.TexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropic_value);
            }
            else if (rf_gfx.extensions.max_anisotropic_level > 0.0f)
            {
                rf_log(rf_log_type_warning, "Maximum anisotropic filter level supported is %i_x. Texture id: %d", rf_gfx.extensions.max_anisotropic_level, texture.id);
                rf_gl.TexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropic_value);
            }
            else
            {
                rf_log(rf_log_type_warning, "Anisotropic filtering not supported");
            }
        } break;
    }

    rf_gl.BindTexture(GL_TEXTURE_2D, 0);
}

// Enable rendering to texture (fbo)
rf_public void rf_gfx_enable_render_texture(unsigned int id)
{
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, id);

    //rf_gl.Disable(GL_CULL_FACE);    // Allow double side drawing for texture flipping
    //glCullFace(GL_FRONT);
}

// Disable rendering to texture
rf_public void rf_gfx_disable_render_texture(void)
{
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    //rf_gl.Enable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
}

// Enable depth test
rf_public void rf_gfx_enable_depth_test(void) { rf_gl.Enable(GL_DEPTH_TEST); }

// Disable depth test
rf_public void rf_gfx_disable_depth_test(void) { rf_gl.Disable(GL_DEPTH_TEST); }

// Enable backface culling
rf_public void rf_gfx_enable_backface_culling(void) { rf_gl.Enable(GL_CULL_FACE); }

// Disable backface culling
rf_public void rf_gfx_disable_backface_culling(void) { rf_gl.Disable(GL_CULL_FACE); }

// Enable scissor test
rf_public void rf_gfx_enable_scissor_test(void) { rf_gl.Enable(GL_SCISSOR_TEST); }

// Disable scissor test
rf_public void rf_gfx_disable_scissor_test(void) { rf_gl.Disable(GL_SCISSOR_TEST); }

// Scissor test
rf_public void rf_gfx_scissor(int x, int y, int width, int height) { rf_gl.Scissor(x, y, width, height); }

// Enable wire mode
rf_public void rf_gfx_enable_wire_mode(void)
{
#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
    // NOTE: glPolygonMode() not available on OpenGL ES
    rf_gl.PolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
}

// Disable wire mode
rf_public void rf_gfx_disable_wire_mode(void)
{
#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
    // NOTE: glPolygonMode() not available on OpenGL ES
    rf_gl.PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}

// Unload texture from GPU memory
rf_public void rf_gfx_delete_textures(unsigned int id)
{
    if (id > 0) rf_gl.DeleteTextures(1, &id);
}

// Unload render texture from GPU memory
rf_public void rf_gfx_delete_render_textures(rf_render_texture2d target)
{
    if (target.texture.id > 0) rf_gl.DeleteTextures(1, &target.texture.id);
    if (target.depth.id > 0)
    {
        if (target.depth_texture) rf_gl.DeleteTextures(1, &target.depth.id);
        else rf_gl.DeleteRenderbuffers(1, &target.depth.id);
    }

    if (target.id > 0) rf_gl.DeleteFramebuffers(1, &target.id);

    rf_log(rf_log_type_info, "[FBO ID %i] Unloaded render texture data from VRAM (GPU)", target.id);
}

// Unload shader from GPU memory
rf_public void rf_gfx_delete_shader(unsigned int id)
{
    if (id != 0) rf_gl.DeleteProgram(id);
}

// Unload vertex data (VAO) from GPU memory
rf_public void rf_gfx_delete_vertex_arrays(unsigned int id)
{
    if (id != 0) rf_gl.DeleteVertexArrays(1, &id);
    rf_log(rf_log_type_info, "[VAO ID %i] Unloaded model data from VRAM (GPU)", id);
}

// Unload vertex data (VBO) from GPU memory
rf_public void rf_gfx_delete_buffers(unsigned int id)
{
    if (id != 0)
    {
        rf_gl.DeleteBuffers(1, &id);
        rf_log(rf_log_type_info, "[VBO ID %i] Unloaded model vertex data from VRAM (GPU)", id);
    }
}

// Clear color buffer with color
rf_public void rf_gfx_clear_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    // rf_color values clamp to 0.0f(0) and 1.0f(255)
    float cr = (float)r/255;
    float cg = (float)g/255;
    float cb = (float)b/255;
    float ca = (float)a/255;

    rf_gl.ClearColor(cr, cg, cb, ca);
}

// Clear used screen buffers (color and depth)
rf_public void rf_gfx_clear_screen_buffers(void)
{
    rf_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear used buffers: rf_color and Depth (Depth is used for 3D)
    //rf_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);     // Stencil buffer not used...
}

// Update GPU buffer with new data
rf_public void rf_gfx_update_buffer(int buffer_id, void* data, int data_size)
{
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, buffer_id);
    rf_gl.BufferSubData(GL_ARRAY_BUFFER, 0, data_size, data);
}

// Load a new attributes buffer
rf_public unsigned int rf_gfx_load_attrib_buffer(unsigned int vao_id, int shader_loc, void* buffer, int size, rf_bool dynamic)
{
    unsigned int id = 0;

    int draw_hint = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

    rf_gl.BindVertexArray(vao_id);

    rf_gl.GenBuffers(1, &id);
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, id);
    rf_gl.BufferData(GL_ARRAY_BUFFER, size, buffer, draw_hint);
    rf_gl.VertexAttribPointer(shader_loc, 2, GL_FLOAT, 0, 0, 0);
    rf_gl.EnableVertexAttribArray(shader_loc);

    rf_gl.BindVertexArray(0);

    return id;
}

rf_public void rf_gfx_init_vertex_buffer(rf_vertex_buffer* vertex_buffer)
{
    int elements_count = vertex_buffer->elements_count;

    // Initialize Quads VAO
    rf_gl.GenVertexArrays(1, &vertex_buffer->vao_id);
    rf_gl.BindVertexArray(vertex_buffer->vao_id);

    // Quads - Vertex buffers binding and attributes enable
    // Vertex position buffer (shader-location = 0)
    rf_gl.GenBuffers(1, &vertex_buffer->vbo_id[0]);
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, vertex_buffer->vbo_id[0]);
    rf_gl.BufferData(GL_ARRAY_BUFFER, elements_count * RF_GFX_VERTEX_COMPONENT_COUNT, vertex_buffer->vertices, GL_DYNAMIC_DRAW);
    rf_gl.EnableVertexAttribArray(rf_ctx.current_shader.locs[RF_LOC_VERTEX_POSITION]);
    rf_gl.VertexAttribPointer(rf_ctx.current_shader.locs[RF_LOC_VERTEX_POSITION], 3, GL_FLOAT, 0, 0, 0);

    // Vertex texcoord buffer (shader-location = 1)
    rf_gl.GenBuffers(1, &vertex_buffer->vbo_id[1]);
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, vertex_buffer->vbo_id[1]);
    rf_gl.BufferData(GL_ARRAY_BUFFER, elements_count * RF_GFX_TEXCOORD_COMPONENT_COUNT, vertex_buffer->texcoords, GL_DYNAMIC_DRAW);
    rf_gl.EnableVertexAttribArray(rf_ctx.current_shader.locs[RF_LOC_VERTEX_TEXCOORD01]);
    rf_gl.VertexAttribPointer(rf_ctx.current_shader.locs[RF_LOC_VERTEX_TEXCOORD01], 2, GL_FLOAT, 0, 0, 0);

    // Vertex color buffer (shader-location = 3)
    rf_gl.GenBuffers(1, &vertex_buffer->vbo_id[2]);
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, vertex_buffer->vbo_id[2]);
    rf_gl.BufferData(GL_ARRAY_BUFFER, elements_count * RF_GFX_COLOR_COMPONENT_COUNT, vertex_buffer->colors, GL_DYNAMIC_DRAW);
    rf_gl.EnableVertexAttribArray(rf_ctx.current_shader.locs[RF_LOC_VERTEX_COLOR]);
    rf_gl.VertexAttribPointer(rf_ctx.current_shader.locs[RF_LOC_VERTEX_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);

    // Fill index buffer
    rf_gl.GenBuffers(1, &vertex_buffer->vbo_id[3]);
    rf_gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer->vbo_id[3]);
    rf_gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, elements_count * RF_GFX_VERTEX_INDEX_COMPONENT_COUNT, vertex_buffer->indices, GL_STATIC_DRAW);

    // Unbind
    rf_gl.BindVertexArray(0);
}

// Vertex Buffer Object deinitialization (memory free)
rf_public void rf_gfx_close()
{
    rf_unlock_shader_default();              // Unload default shader
    rf_unload_buffers_default();             // Unload default buffers

    rf_gl.DeleteTextures(1, &rf_ctx.default_texture_id); // Unload default texture

    rf_log(rf_log_type_info, "[TEX ID %i] Unloaded texture data (base white texture) from VRAM", rf_ctx.default_texture_id);
}

// Update and draw internal buffers
rf_public void rf_gfx_draw()
{
    // Only process data if we have data to process
    if (rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter > 0)
    {
        // Update default internal buffers (VAOs/VBOs) with vertex array data
        // NOTE: If there is not vertex data, buffers doesn't need to be updated (vertex_count > 0)
        // TODO: If no data changed on the CPU arrays --> No need to re-update GPU arrays (change flag required)
        {
            // Update vertex buffers data
            // Activate elements VAO
            rf_gl.BindVertexArray(rf_batch.vertex_buffers[rf_batch.current_buffer].vao_id);

            // Vertex positions buffer
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, rf_batch.vertex_buffers[rf_batch.current_buffer].vbo_id[0]);
            rf_gl.BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter, rf_batch.vertex_buffers[rf_batch.current_buffer].vertices);
            //rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 4 * rf_max_batch_elements, rf_ctx->gl_ctx.memory->vertex_buffers[rf_ctx->gl_ctx.current_buffer].vertices, GL_DYNAMIC_DRAW);  // Update all buffer

            // rf_texture coordinates buffer
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, rf_batch.vertex_buffers[rf_batch.current_buffer].vbo_id[1]);
            rf_gl.BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 2 * rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter, rf_batch.vertex_buffers[rf_batch.current_buffer].texcoords);
            //rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 4 * rf_max_batch_elements, rf_ctx->gl_ctx.memory->vertex_buffers[rf_ctx->gl_ctx.current_buffer].texcoords, GL_DYNAMIC_DRAW); // Update all buffer

            // Colors buffer
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, rf_batch.vertex_buffers[rf_batch.current_buffer].vbo_id[2]);
            rf_gl.BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned char) * 4 * rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter, rf_batch.vertex_buffers[rf_batch.current_buffer].colors);
            //rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4 * rf_max_batch_elements, rf_ctx->gl_ctx.memory->vertex_buffers[rf_ctx->gl_ctx.current_buffer].colors, GL_DYNAMIC_DRAW);    // Update all buffer

            // NOTE: glMap_buffer() causes sync issue.
            // If GPU is working with this buffer, glMap_buffer() will wait(stall) until GPU to finish its job.
            // To avoid waiting (idle), you can call first rf_gl.BufferData() with NULL pointer before glMap_buffer().
            // If you do that, the previous data in PBO will be discarded and glMap_buffer() returns a new
            // allocated pointer immediately even if GPU is still working with the previous data.

            // Another option: map the buffer object into client's memory
            // Probably this code could be moved somewhere else...
            // rf_ctx->gl_ctx.memory->vertex_buffers[rf_ctx->gl_ctx.current_buffer].vertices = (float* )glMap_buffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
            // if (rf_ctx->gl_ctx.memory->vertex_buffers[rf_ctx->gl_ctx.current_buffer].vertices)
            // {
            // Update vertex data
            // }
            // glUnmap_buffer(GL_ARRAY_BUFFER);

            // Unbind the current VAO
            rf_gl.BindVertexArray(0);
        }

        // NOTE: Stereo rendering is checked inside
        //rf_draw_buffers_default();
        {
            // Draw default internal buffers vertex data
            rf_mat mat_projection = rf_ctx.projection;
            rf_mat mat_model_view = rf_ctx.modelview;

            // Draw buffers
            if (rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter > 0)
            {
                // Set current shader and upload current MVP matrix
                rf_gl.UseProgram(rf_ctx.current_shader.id);

                // Create rf_ctx->gl_ctx.modelview-rf_ctx->gl_ctx.projection matrix
                rf_mat mat_mvp = rf_mat_mul(rf_ctx.modelview, rf_ctx.projection);

                rf_gl.UniformMatrix4fv(rf_ctx.current_shader.locs[RF_LOC_MATRIX_MVP], 1, 0, rf_mat_to_float16(mat_mvp).v);
                rf_gl.Uniform4f(rf_ctx.current_shader.locs[RF_LOC_COLOR_DIFFUSE], 1.0f, 1.0f, 1.0f, 1.0f);
                rf_gl.Uniform1i(rf_ctx.current_shader.locs[RF_LOC_MAP_DIFFUSE], 0);    // Provided value refers to the texture unit (active)

                // TODO: Support additional texture units on custom shader
                //if (rf_ctx->gl_ctx.current_shader->locs[RF_LOC_MAP_SPECULAR] > 0) rf_gl.Uniform1i(rf_ctx->gl_ctx.current_shader.locs[RF_LOC_MAP_SPECULAR], 1);
                //if (rf_ctx->gl_ctx.current_shader->locs[RF_LOC_MAP_NORMAL] > 0) rf_gl.Uniform1i(rf_ctx->gl_ctx.current_shader.locs[RF_LOC_MAP_NORMAL], 2);

                // NOTE: Right now additional map textures not considered for default buffers drawing

                int vertex_offset = 0;

                rf_gl.BindVertexArray(rf_batch.vertex_buffers[rf_batch.current_buffer].vao_id);

                rf_gl.ActiveTexture(GL_TEXTURE0);

                for (rf_int i = 0; i < rf_batch.draw_calls_counter; i++)
                {
                    rf_gl.BindTexture(GL_TEXTURE_2D, rf_batch.draw_calls[i].texture_id);

                    // TODO: Find some way to bind additional textures --> Use global texture IDs? Register them on draw[i]?
                    //if (rf_ctx->gl_ctx.current_shader->locs[RF_LOC_MAP_SPECULAR] > 0) { rf_gl.ActiveTexture(GL_TEXTURE1); rf_gl.BindTexture(GL_TEXTURE_2D, textureUnit1_id); }
                    //if (rf_ctx->gl_ctx.current_shader->locs[RF_LOC_MAP_SPECULAR] > 0) { rf_gl.ActiveTexture(GL_TEXTURE2); rf_gl.BindTexture(GL_TEXTURE_2D, textureUnit2_id); }

                    if ((rf_batch.draw_calls[i].mode == RF_LINES) || (rf_batch.draw_calls[i].mode == RF_TRIANGLES))
                    {
                        rf_gl.DrawArrays(rf_batch.draw_calls[i].mode, vertex_offset, rf_batch.draw_calls[i].vertex_count);
                    }
                    else
                    {
                        #if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
                        // We need to define the number of indices to be processed: quadsCount*6
                        // NOTE: The final parameter tells the GPU the offset in bytes from the
                        // start of the index buffer to the location of the first index to process
                        rf_gl.DrawElements(GL_TRIANGLES, rf_batch.draw_calls[i].vertex_count / 4 * 6, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * vertex_offset / 4 * 6));
                        #elif defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
                        rf_gl.DrawElements(GL_TRIANGLES, rf_batch.draw_calls[i].vertex_count / 4 * 6, GL_UNSIGNED_SHORT, (void*)(sizeof(unsigned short) * vertex_offset / 4 * 6));
                        #endif
                    }

                    vertex_offset += (rf_batch.draw_calls[i].vertex_count + rf_batch.draw_calls[i].vertex_alignment);
                }

                rf_gl.BindTexture(GL_TEXTURE_2D, 0);
            }

            rf_gl.BindVertexArray(0);
            rf_gl.UseProgram(0);

            // Reset vertex counters for next frame
            rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter  = 0;
            rf_batch.vertex_buffers[rf_batch.current_buffer].tc_counter = 0;
            rf_batch.vertex_buffers[rf_batch.current_buffer].c_counter  = 0;

            // Reset depth for next draw
            rf_batch.current_depth = -1.0f;

            // Restore rf_ctx->gl_ctx.projection/rf_ctx->gl_ctx.modelview matrices
            rf_ctx.projection = mat_projection;
            rf_ctx.modelview  = mat_model_view;

            // Reset rf_ctx->gl_ctx.draws array
            for (rf_int i = 0; i < RF_DEFAULT_BATCH_DRAW_CALLS_COUNT; i++)
            {
                rf_batch.draw_calls[i].mode = RF_QUADS;
                rf_batch.draw_calls[i].vertex_count = 0;
                rf_batch.draw_calls[i].texture_id = rf_ctx.default_texture_id;
            }

            rf_batch.draw_calls_counter = 1;

            // Change to next buffer in the list
            rf_batch.current_buffer++;
            if (rf_batch.current_buffer >= RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT) rf_batch.current_buffer = 0;
        }
    }
}

// Check internal buffer overflow for a given number of vertex
rf_public rf_bool rf_gfx_check_buffer_limit(int v_count)
{
    return (rf_batch.vertex_buffers[rf_batch.current_buffer].v_counter + v_count) >= (rf_batch.vertex_buffers[rf_batch.current_buffer].elements_count * 4);
}

// Set debug marker
rf_public void rf_gfx_set_debug_marker(const char* text)
{
#if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
    //if (rf_ctx->gl_ctx.debug_marker_supported) glInsertEventMarkerEXT(0, text);
#endif
}

// Convert image data to OpenGL texture (returns OpenGL valid Id)
rf_public unsigned int rf_gfx_load_texture(void* data, int width, int height, rf_pixel_format format, int mipmap_count)
{
    rf_gl.BindTexture(GL_TEXTURE_2D, 0); // Free any old binding

    unsigned int id = 0;

    // Check texture format support by OpenGL 1.1 (compressed textures not supported)
    if ((!rf_gfx.extensions.tex_comp_dxt_supported) && ((format == rf_pixel_format_dxt1_rgb) || (format == rf_pixel_format_dxt1_rgba) || (format == rf_pixel_format_dxt3_rgba) || (format == rf_pixel_format_dxt5_rgba)))
    {
        rf_log(rf_log_type_warning, "DXT compressed texture format not supported");
        return id;
    }

    if ((!rf_gfx.extensions.tex_comp_etc1_supported) && (format == rf_pixel_format_etc1_rgb))
    {
        rf_log(rf_log_type_warning, "ETC1 compressed texture format not supported");
        return id;
    }

    if ((!rf_gfx.extensions.tex_comp_etc2_supported) && ((format == rf_pixel_format_etc2_rgb) || (format == rf_pixel_format_etc2_eac_rgba)))
    {
        rf_log(rf_log_type_warning, "ETC2 compressed texture format not supported");
        return id;
    }

    if ((!rf_gfx.extensions.tex_comp_pvrt_supported) && ((format == rf_pixel_format_pvrt_rgb) || (format == rf_pixel_format_prvt_rgba)))
    {
        rf_log(rf_log_type_warning, "PVRT compressed texture format not supported");
        return id;
    }

    if ((!rf_gfx.extensions.tex_comp_astc_supported) && ((format == rf_pixel_format_astc_4x4_rgba) || (format == rf_pixel_format_astc_8x8_rgba)))
    {
        rf_log(rf_log_type_warning, "ASTC compressed texture format not supported");
        return id;
    }

    rf_gl.PixelStorei(GL_UNPACK_ALIGNMENT, 1);

    rf_gl.GenTextures(1, &id);              // Generate texture id

    //rf_gl.ActiveTexture(GL_TEXTURE0);     // If not defined, using GL_TEXTURE0 by default (shader texture)

    rf_gl.BindTexture(GL_TEXTURE_2D, id);

    int mip_width = width;
    int mip_height = height;
    int mip_offset = 0;          // Mipmap data offset

    rf_log(rf_log_type_debug, "Load texture from data memory address: 0x%x", data);

    // Load the different mipmap levels
    for (rf_int i = 0; i < mipmap_count; i++)
    {
        int mip_size = rf_pixel_buffer_size(mip_width, mip_height, format);

        rf_gfx_pixel_format glformat = rf_gfx_get_internal_texture_formats(format);

        rf_log(rf_log_type_debug, "Load mipmap level %i (%i x %i), size: %i, offset: %i", i, mip_width, mip_height, mip_size, mip_offset);

        if (glformat.valid)
        {
            if (rf_is_uncompressed_format(format))
            {
                rf_gl.TexImage2D(GL_TEXTURE_2D, i, glformat.internal_format, mip_width, mip_height, 0, glformat.format, glformat.type, (unsigned char* )data + mip_offset);
            }
            else
            {
                rf_gl.CompressedTexImage2D(GL_TEXTURE_2D, i, glformat.internal_format, mip_width, mip_height, 0, mip_size, (unsigned char *)data + mip_offset);
            }

            #if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
                if (format == rf_pixel_format_grayscale)
                {
                    int swizzle_mask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
                    rf_gl.TexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
                }
                else if (format == rf_pixel_format_gray_alpha)
                {
                    int swizzle_mask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
                    rf_gl.TexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
                }
            #endif
        }

        mip_width /= 2;
        mip_height /= 2;
        mip_offset += mip_size;

        // Security check for NPOT textures
        if (mip_width < 1) mip_width = 1;
        if (mip_height < 1) mip_height = 1;
    }

    // rf_texture parameters configuration
    // NOTE: rf_gl.TexParameteri does NOT affect texture uploading, just the way it's used
    #if defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
        // NOTE: OpenGL ES 2.0 with no GL_OES_texture_npot support (i.e. WebGL) has limited NPOT support, so CLAMP_TO_EDGE must be used
        if (rf_gfx.extensions.tex_npot_supported)
        {
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // Set texture to repeat on x-axis
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // Set texture to repeat on y-axis
        }
        else
        {
            // NOTE: If using negative texture coordinates (LoadOBJ()), it does not work!
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);       // Set texture to clamp on x-axis
            rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);       // Set texture to clamp on y-axis
        }
    #else
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // Set texture to repeat on x-axis
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // Set texture to repeat on y-axis
    #endif

    // Magnification and minification filters
    rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR
    rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // Alternative: GL_LINEAR

    #if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
    if (mipmap_count > 1)
    {
        // Activate Trilinear filtering if mipmaps are available
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    #endif

    // At this point we have the texture loaded in GPU and texture parameters configured

    // NOTE: If mipmaps were not in data, they are not generated automatically

    // Unbind current texture
    rf_gl.BindTexture(GL_TEXTURE_2D, 0);

    if (id > 0) rf_log(rf_log_type_info, "[TEX ID %i] rf_texture created successfully (%ix%i - %i mipmaps)", id, width, height, mipmap_count);
    else rf_log(rf_log_type_warning, "rf_texture could not be created");

    return id;
}

// Load depth texture/renderbuffer (to be attached to fbo)
// WARNING: OpenGL ES 2.0 requires GL_OES_depth_texture/WEBGL_depth_texture extensions
rf_public unsigned int rf_gfx_load_texture_depth(int width, int height, int bits, rf_bool use_render_buffer)
{
    unsigned int id = 0;

    unsigned int glInternalFormat = GL_DEPTH_COMPONENT16;

    if ((bits != 16) && (bits != 24) && (bits != 32)) bits = 16;

    if (bits == 24)
    {
        glInternalFormat = GL_DEPTH_COMPONENT24;
    }

    if (bits == 32 && rf_gfx.extensions.max_depth_bits == 32) // Check max_depth_bits to make sure its ok on mobile
    {
        glInternalFormat = GL_DEPTH_COMPONENT32;
    }

    if (!use_render_buffer && rf_gfx.extensions.tex_depth_supported)
    {
        rf_gl.GenTextures(1, &id);
        rf_gl.BindTexture(GL_TEXTURE_2D, id);
        rf_gl.TexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        rf_gl.BindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        // Create the renderbuffer that will serve as the depth attachment for the framebuffer
        // NOTE: A renderbuffer is simpler than a texture and could offer better performance on embedded devices
        rf_gl.GenRenderbuffers(1, &id);
        rf_gl.BindRenderbuffer(GL_RENDERBUFFER, id);
        rf_gl.RenderbufferStorage(GL_RENDERBUFFER, glInternalFormat, width, height);

        rf_gl.BindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    return id;
}

// Load texture cubemap
// NOTE: Cubemap data is expected to be 6 images in a single column,
// expected the following convention: +X, -X, +Y, -Y, +Z, -Z
rf_public unsigned int rf_gfx_load_texture_cubemap(void* data, int size, rf_pixel_format format)
{
    unsigned int cubemap_id = 0;
    unsigned int data_size = size * size * rf_bytes_per_pixel(format);

    rf_gl.GenTextures(1, &cubemap_id);
    rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);

    rf_gfx_pixel_format glformat = rf_gfx_get_internal_texture_formats(format);

    if (glformat.valid)
    {
        // Load cubemap faces
        for (unsigned int i = 0; i < 6; i++)
        {
            if (rf_is_uncompressed_format(format)) rf_gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glformat.internal_format, size, size, 0, glformat.format, glformat.type, (unsigned char* )data + i * data_size);
            else rf_gl.CompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glformat.internal_format, size, size, 0, data_size, (unsigned char* )data + i*data_size);

            #if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
                if (format == rf_pixel_format_grayscale)
                {
                    int swizzle_mask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
                    rf_gl.TexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
                }
                else if (format == rf_pixel_format_gray_alpha)
                {
                    int swizzle_mask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
                    rf_gl.TexParameteriv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
                }
            #endif
        }
    }

    // Set cubemap texture sampling parameters
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    #if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  // Flag not supported on OpenGL ES 2.0
    #endif // defined(RAYFORK_GRAPHICS_BACKEND_GL_33)

    rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return cubemap_id;
}

// Update already loaded texture in GPU with new data
// NOTE: We don't know safely if internal texture format is the expected one...
rf_public void rf_gfx_update_texture(unsigned int id, int width, int height, rf_pixel_format format, const void* pixels, int pixels_size)
{
    if (width * height * rf_bytes_per_pixel(format) <= pixels_size) return;

    rf_gl.BindTexture(GL_TEXTURE_2D, id);

    rf_gfx_pixel_format gfx_format = rf_gfx_get_internal_texture_formats(format);

    if (gfx_format.valid && rf_is_uncompressed_format(format))
    {
        rf_gl.TexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, gfx_format.internal_format, gfx_format.type, (unsigned char*) pixels);
    }
    else rf_log(rf_log_type_warning, "rf_texture format updating not supported");
}

// Get OpenGL internal formats and data type from raylib rf_pixel_format
rf_public rf_gfx_pixel_format rf_gfx_get_internal_texture_formats(rf_pixel_format format)
{
    rf_gfx_pixel_format result = {.valid = 1};

    switch (format)
    {
        #if defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
        // NOTE: on OpenGL ES 2.0 (WebGL), internalFormat must match format and options allowed are: GL_LUMINANCE, GL_RGB, GL_RGBA
        case rf_pixel_format_grayscale: result.internal_format = GL_LUMINANCE; result.format = GL_LUMINANCE; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_gray_alpha: result.internal_format = GL_LUMINANCE_ALPHA; result.format = GL_LUMINANCE_ALPHA; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_r5g6b5: result.internal_format = GL_RGB; result.format = GL_RGB; result.type = GL_UNSIGNED_SHORT_5_6_5; break;
        case rf_pixel_format_r8g8b8: result.internal_format = GL_RGB; result.format = GL_RGB; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_r5g5b5a1: result.internal_format = GL_RGBA; result.format = GL_RGBA; result.type = GL_UNSIGNED_SHORT_5_5_5_1; break;
        case rf_pixel_format_r4g4b4a4: result.internal_format = GL_RGBA; result.format = GL_RGBA; result.type = GL_UNSIGNED_SHORT_4_4_4_4; break;
        case rf_pixel_format_r8g8b8a8: result.internal_format = GL_RGBA; result.format = GL_RGBA; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_r32:          if (rf_gfx.extensions.tex_float_supported) result.internal_format = GL_LUMINANCE; result.format = GL_LUMINANCE; result.type = GL_FLOAT; break;   // NOTE: Requires extension OES_texture_float
        case rf_pixel_format_r32g32b32:    if (rf_gfx.extensions.tex_float_supported) result.internal_format = GL_RGB; result.format = GL_RGB; result.type = GL_FLOAT; break;         // NOTE: Requires extension OES_texture_float
        case rf_pixel_format_r32g32b32a32: if (rf_gfx.extensions.tex_float_supported) result.internal_format = GL_RGBA; result.format = GL_RGBA; result.type = GL_FLOAT; break;    // NOTE: Requires extension OES_texture_float

        #elif defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
        //case rf_pixel_format_r5g6b5:       result.internal_format = GL_RGB565; result.format = GL_RGB; result.type = GL_UNSIGNED_SHORT_5_6_5; break;
        case rf_pixel_format_grayscale:    result.internal_format = GL_R8; result.format = GL_RED; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_gray_alpha:   result.internal_format = GL_RG8; result.format = GL_RG; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_r8g8b8:       result.internal_format = GL_RGB8; result.format = GL_RGB; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_r5g5b5a1:     result.internal_format = GL_RGB5_A1; result.format = GL_RGBA; result.type = GL_UNSIGNED_SHORT_5_5_5_1; break;
        case rf_pixel_format_r4g4b4a4:     result.internal_format = GL_RGBA4; result.format = GL_RGBA; result.type = GL_UNSIGNED_SHORT_4_4_4_4; break;
        case rf_pixel_format_r8g8b8a8:     result.internal_format = GL_RGBA8; result.format = GL_RGBA; result.type = GL_UNSIGNED_BYTE; break;
        case rf_pixel_format_r32:          if (rf_gfx.extensions.tex_float_supported) result.internal_format = GL_R32F; result.format = GL_RED; result.type = GL_FLOAT; break;
        case rf_pixel_format_r32g32b32:    if (rf_gfx.extensions.tex_float_supported) result.internal_format = GL_RGB32F; result.format = GL_RGB; result.type = GL_FLOAT; break;
        case rf_pixel_format_r32g32b32a32: if (rf_gfx.extensions.tex_float_supported) result.internal_format = GL_RGBA32F; result.format = GL_RGBA; result.type = GL_FLOAT; break;
        #endif

        case rf_pixel_format_dxt1_rgb:      if (rf_gfx.extensions.tex_comp_dxt_supported) result.internal_format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT; break;
        case rf_pixel_format_dxt1_rgba:     if (rf_gfx.extensions.tex_comp_dxt_supported) result.internal_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
        case rf_pixel_format_dxt3_rgba:     if (rf_gfx.extensions.tex_comp_dxt_supported) result.internal_format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
        case rf_pixel_format_dxt5_rgba:     if (rf_gfx.extensions.tex_comp_dxt_supported) result.internal_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
        case rf_pixel_format_etc1_rgb:      if (rf_gfx.extensions.tex_comp_etc1_supported) result.internal_format = GL_ETC1_RGB8_OES; break;                      // NOTE: Requires OpenGL ES 2.0 or OpenGL 4.3
        case rf_pixel_format_etc2_rgb:      if (rf_gfx.extensions.tex_comp_etc2_supported) result.internal_format = GL_COMPRESSED_RGB8_ETC2; break;               // NOTE: Requires OpenGL ES 3.0 or OpenGL 4.3
        case rf_pixel_format_etc2_eac_rgba: if (rf_gfx.extensions.tex_comp_etc2_supported) result.internal_format = GL_COMPRESSED_RGBA8_ETC2_EAC; break;     // NOTE: Requires OpenGL ES 3.0 or OpenGL 4.3
        case rf_pixel_format_pvrt_rgb:      if (rf_gfx.extensions.tex_comp_pvrt_supported) result.internal_format = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG; break;    // NOTE: Requires PowerVR GPU
        case rf_pixel_format_prvt_rgba:     if (rf_gfx.extensions.tex_comp_pvrt_supported) result.internal_format = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG; break;  // NOTE: Requires PowerVR GPU
        case rf_pixel_format_astc_4x4_rgba: if (rf_gfx.extensions.tex_comp_astc_supported) result.internal_format = GL_COMPRESSED_RGBA_ASTC_4x4_KHR; break;  // NOTE: Requires OpenGL ES 3.1 or OpenGL 4.3
        case rf_pixel_format_astc_8x8_rgba: if (rf_gfx.extensions.tex_comp_astc_supported) result.internal_format = GL_COMPRESSED_RGBA_ASTC_8x8_KHR; break;  // NOTE: Requires OpenGL ES 3.1 or OpenGL 4.3

        default:
            rf_log(rf_log_type_warning, "rf_texture format not supported");
            result.valid = 0;
            break;
    }

    return result;
}

// Unload texture from GPU memory
rf_public void rf_gfx_unload_texture(unsigned int id)
{
    if (id > 0) rf_gl.DeleteTextures(1, &id);
}

// Generate mipmap data for selected texture
rf_public void rf_gfx_generate_mipmaps(rf_texture2d* texture)
{
    rf_gl.BindTexture(GL_TEXTURE_2D, texture->id);

    // Check if texture is power-of-two (POT)
    rf_bool tex_is_pot = 0;

    if (((texture->width > 0) && ((texture->width & (texture->width - 1)) == 0)) &&
        ((texture->height > 0) && ((texture->height & (texture->height - 1)) == 0))) tex_is_pot = 1;

    if ((tex_is_pot) || (rf_gfx.extensions.tex_npot_supported))
    {
        //glHint(GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE);   // Hint for mipmaps generation algorythm: GL_FASTEST, GL_NICEST, GL_DONT_CARE
        rf_gl.GenerateMipmap(GL_TEXTURE_2D);    // Generate mipmaps automatically
        rf_log(rf_log_type_info, "[TEX ID %i] Mipmaps generated automatically", texture->id);

        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);   // Activate Trilinear filtering for mipmaps

        texture->mipmaps =  1 + (int)floor(log(texture->width > texture->height ? texture->width : texture->height)/log(2));
    }
    else rf_log(rf_log_type_warning, "[TEX ID %i] Mipmaps can not be generated", texture->id);

    rf_gl.BindTexture(GL_TEXTURE_2D, 0);
}

// Read texture pixel data
rf_public rf_image rf_gfx_read_texture_pixels_to_buffer(rf_texture2d texture, void* dst, int dst_size)
{
    if (!texture.valid || !dst || !dst_size) return (rf_image) {0};

    rf_image result = {0};

    #if defined(RAYFORK_GRAPHICS_BACKEND_GL_33)
    {
        rf_gl.BindTexture(GL_TEXTURE_2D, texture.id);

        /*
        NOTE: Using texture.id, we can retrieve some texture info (but not on OpenGL ES 2.0)
        Possible texture info: GL_TEXTURE_RED_SIZE, GL_TEXTURE_GREEN_SIZE, GL_TEXTURE_BLUE_SIZE, GL_TEXTURE_ALPHA_SIZE
        int width, height, format;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);

        NOTE: Each row written to or read from by OpenGL pixel operations like rf_gl.GetTexImage are aligned to a 4 unsigned char boundary by default, which may add some padding.
        Use rf_gl.PixelStorei to modify padding with the GL_[UN]PACK_ALIGNMENT setting.
        GL_PACK_ALIGNMENT affects operations that read from OpenGL memory (rf_gl.ReadPixels, rf_gl.GetTexImage, etc.)
        GL_UNPACK_ALIGNMENT affects operations that write to OpenGL memory (glTexImage, etc.)
        */

        rf_gl.PixelStorei(GL_PACK_ALIGNMENT, 1);

        rf_gfx_pixel_format format = rf_gfx_get_internal_texture_formats(texture.format);
        int size = texture.width * texture.height * rf_bytes_per_pixel(texture.format);

        if (format.valid && rf_is_uncompressed_format(texture.format) && size <= dst_size)
        {
            rf_gl.GetTexImage(GL_TEXTURE_2D, 0, format.format, format.type, dst);

            result = (rf_image) {
                .width = texture.width,
                .height = texture.height,
                .format = texture.format,
                .data = dst,
                .valid = 1,
            };
        }
        else rf_log(rf_log_type_warning, "rf_texture data retrieval not suported for pixel format");

        rf_gl.BindTexture(GL_TEXTURE_2D, 0);
    }
    #elif defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
    {
        /*
        rf_gl.GetTexImage() is not available on OpenGL ES 2.0
        rf_texture2d width and height are required on OpenGL ES 2.0. There is no way to get it from texture id.
        Two possible Options:
        1 - Bind texture to color fbo attachment and rf_gl.ReadPixels()
        2 - Create an fbo, activate it, render quad with texture, rf_gl.ReadPixels()
        We are using Option 1, just need to care for texture format on retrieval
        NOTE: This behaviour could be conditioned by graphic driver...
        */
        rf_render_texture2d fbo = rf_gfx_load_render_texture(texture.width, texture.height, rf_pixel_format_r8g8b8a8, 16, 0);

        rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo.id);
        rf_gl.BindTexture(GL_TEXTURE_2D, 0);

        // Attach our texture to FBO
        // NOTE: Previoust attached texture is automatically detached
        rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.id, 0);

        // We read data as RGBA because FBO texture is configured as RGBA, despite binding another texture format
        if (texture.width * texture.height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8) <= dst_size)
        {
            rf_gl.ReadPixels(0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, dst);

            result = (rf_image) {
                .width = texture.width,
                .height = texture.height,
                .format = texture.format,
                .data = dst,
                .valid = 1,
            };
        }

        // Re-attach internal FBO color texture before deleting it
        rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.texture.id, 0);

        rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

        // Clean up temporal fbo
        rf_gfx_delete_render_textures(fbo);
    }
    #endif

    return result;
}

rf_public rf_image rf_gfx_read_texture_pixels(rf_texture2d texture, rf_allocator allocator)
{
    if (!texture.valid) return (rf_image) {0};

    int size = texture.width * texture.height * rf_bytes_per_pixel(texture.format);
    void* dst = rf_alloc(allocator, size);
    rf_image result = rf_gfx_read_texture_pixels_to_buffer(texture, dst, size);

    if (!result.valid) rf_free(allocator, dst);

    return result;
}

// Read screen pixel data (color buffer)
rf_public void rf_gfx_read_screen_pixels(rf_color* dst, int width, int height)
{
    // NOTE 1: glReadPixels returns image flipped vertically -> (0,0) is the bottom left corner of the framebuffer
    // NOTE 2: We are getting alpha channel! Be careful, it can be transparent if not cleared properly!
    rf_gl.ReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, dst);

    for (rf_int y = height - 1; y >= 0; y--)
    {
        for (rf_int x = 0; x < width; x++)
        {
            dst[((height - 1) - y) * width + x] = dst[(y * width) + x]; // Flip line

            // Set alpha component value to 255 (no trasparent image retrieval)
            // NOTE: Alpha value has already been applied to RGB in framebuffer, we don't need it!
            if (((x + 1) % 4) == 0) dst[((height - 1) - y) * width + x].a = 255;
        }
    }
}

// Load a texture to be used for rendering (fbo with default color and depth attachments)
// NOTE: If colorFormat or depth_bits are no supported, no attachment is done
rf_public rf_render_texture2d rf_gfx_load_render_texture(int width, int height, rf_pixel_format format, int depth_bits, rf_bool use_depth_texture)
{
    rf_render_texture2d target = { 0 };

    if (use_depth_texture && rf_gfx.extensions.tex_depth_supported) target.depth_texture = 1;

    // Create the framebuffer object
    rf_gl.GenFramebuffers(1, &target.id);
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, target.id);

    // Create fbo color texture attachment
    //-----------------------------------------------------------------------------------------------------
    if (rf_is_uncompressed_format(format))
    {
        // WARNING: Some texture formats are not supported for fbo color attachment
        target.texture.id = rf_gfx_load_texture(NULL, width, height, format, 1);
        target.texture.width = width;
        target.texture.height = height;
        target.texture.format = format;
        target.texture.mipmaps = 1;
    }
    //-----------------------------------------------------------------------------------------------------

    // Create fbo depth renderbuffer/texture
    //-----------------------------------------------------------------------------------------------------
    if (depth_bits > 0)
    {
        target.depth.id = rf_gfx_load_texture_depth(width, height, depth_bits, !use_depth_texture);
        target.depth.width = width;
        target.depth.height = height;
        target.depth.format = 19;       //DEPTH_COMPONENT_24BIT?
        target.depth.mipmaps = 1;
    }
    //-----------------------------------------------------------------------------------------------------

    // Attach color texture and depth renderbuffer to FBO
    //-----------------------------------------------------------------------------------------------------
    rf_gfx_render_texture_attach(target, target.texture.id, 0);    // COLOR attachment
    rf_gfx_render_texture_attach(target, target.depth.id, 1);      // DEPTH attachment
    //-----------------------------------------------------------------------------------------------------

    // Check if fbo is complete with attachments (valid)
    //-----------------------------------------------------------------------------------------------------
    if (rf_gfx_render_texture_complete(target)) rf_log(rf_log_type_info, "[FBO ID %i] Framebuffer object created successfully", target.id);
    //-----------------------------------------------------------------------------------------------------

    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    return target;
}

// Attach color buffer texture to an fbo (unloads previous attachment)
// NOTE: Attach type: 0-rf_color, 1-Depth renderbuffer, 2-Depth texture
rf_public void rf_gfx_render_texture_attach(rf_render_texture2d target, unsigned int id, int attach_type)
{
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, target.id);

    if (attach_type == 0) rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);
    else if (attach_type == 1)
    {
        if (target.depth_texture) rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);
        else rf_gl.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id);
    }

    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Verify render texture is complete
rf_public rf_bool rf_gfx_render_texture_complete(rf_render_texture2d target)
{
    rf_bool result = 0;

    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, target.id);

    unsigned int status = rf_gl.CheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        switch (status)
        {
            case GL_FRAMEBUFFER_UNSUPPORTED: rf_log(rf_log_type_warning, "Framebuffer is unsupported"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: rf_log(rf_log_type_warning, "Framebuffer has incomplete attachment"); break;

            #if defined(RAYFORK_GRAPHICS_BACKEND_GL_ES3)
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: rf_log(rf_log_type_warning, "Framebuffer has incomplete dimensions"); break;
            #endif

            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: rf_log(rf_log_type_warning, "Framebuffer has a missing attachment"); break;
            default: break;
        }
    }

    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    result = (status == GL_FRAMEBUFFER_COMPLETE);

    return result;
}

// Upload vertex data into a VAO (if supported) and VBO
rf_public void rf_gfx_load_mesh(rf_mesh* mesh, rf_bool dynamic)
{
    if (mesh->vao_id > 0)
    {
        // Check if mesh has already been loaded in GPU
        rf_log(rf_log_type_warning, "Trying to re-load an already loaded mesh");
        return;
    }

    mesh->vao_id = 0;        // Vertex Array Object
    mesh->vbo_id[0] = 0;     // Vertex positions VBO
    mesh->vbo_id[1] = 0;     // Vertex texcoords VBO
    mesh->vbo_id[2] = 0;     // Vertex normals VBO
    mesh->vbo_id[3] = 0;     // Vertex colors VBO
    mesh->vbo_id[4] = 0;     // Vertex tangents VBO
    mesh->vbo_id[5] = 0;     // Vertex texcoords2 VBO
    mesh->vbo_id[6] = 0;     // Vertex indices VBO

    int draw_hint = GL_STATIC_DRAW;
    if (dynamic) draw_hint = GL_DYNAMIC_DRAW;

    // Initialize Quads VAO (Buffer A)
    rf_gl.GenVertexArrays(1, &mesh->vao_id);
    rf_gl.BindVertexArray(mesh->vao_id);


    // NOTE: Attributes must be uploaded considering default locations points

    // Enable vertex attributes: position (shader-location = 0)
    rf_gl.GenBuffers(1, &mesh->vbo_id[0]);
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[0]);
    rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 3*mesh->vertex_count, mesh->vertices, draw_hint);
    rf_gl.VertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
    rf_gl.EnableVertexAttribArray(0);

    // Enable vertex attributes: texcoords (shader-location = 1)
    rf_gl.GenBuffers(1, &mesh->vbo_id[1]);
    rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[1]);
    rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 2*mesh->vertex_count, mesh->texcoords, draw_hint);
    rf_gl.VertexAttribPointer(1, 2, GL_FLOAT, 0, 0, 0);
    rf_gl.EnableVertexAttribArray(1);

    // Enable vertex attributes: normals (shader-location = 2)
    if (mesh->normals != NULL)
    {
        rf_gl.GenBuffers(1, &mesh->vbo_id[2]);
        rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[2]);
        rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 3*mesh->vertex_count, mesh->normals, draw_hint);
        rf_gl.VertexAttribPointer(2, 3, GL_FLOAT, 0, 0, 0);
        rf_gl.EnableVertexAttribArray(2);
    }
    else
    {
        // Default color vertex attribute set to RF_WHITE
        rf_gl.VertexAttrib3f(2, 1.0f, 1.0f, 1.0f);
        rf_gl.DisableVertexAttribArray(2);
    }

    // Default color vertex attribute (shader-location = 3)
    if (mesh->colors != NULL)
    {
        rf_gl.GenBuffers(1, &mesh->vbo_id[3]);
        rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[3]);
        rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(unsigned char) * 4 * mesh->vertex_count, mesh->colors, draw_hint);
        rf_gl.VertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
        rf_gl.EnableVertexAttribArray(3);
    }
    else
    {
        // Default color vertex attribute set to RF_WHITE
        rf_gl.VertexAttrib4f(3, 1.0f, 1.0f, 1.0f, 1.0f);
        rf_gl.DisableVertexAttribArray(3);
    }

    // Default tangent vertex attribute (shader-location = 4)
    if (mesh->tangents != NULL)
    {
        rf_gl.GenBuffers(1, &mesh->vbo_id[4]);
        rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[4]);
        rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * mesh->vertex_count, mesh->tangents, draw_hint);
        rf_gl.VertexAttribPointer(4, 4, GL_FLOAT, 0, 0, 0);
        rf_gl.EnableVertexAttribArray(4);
    }
    else
    {
        // Default tangents vertex attribute
        rf_gl.VertexAttrib4f(4, 0.0f, 0.0f, 0.0f, 0.0f);
        rf_gl.DisableVertexAttribArray(4);
    }

    // Default texcoord2 vertex attribute (shader-location = 5)
    if (mesh->texcoords2 != NULL)
    {
        rf_gl.GenBuffers(1, &mesh->vbo_id[5]);
        rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh->vbo_id[5]);
        rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 2*mesh->vertex_count, mesh->texcoords2, draw_hint);
        rf_gl.VertexAttribPointer(5, 2, GL_FLOAT, 0, 0, 0);
        rf_gl.EnableVertexAttribArray(5);
    }
    else
    {
        // Default texcoord2 vertex attribute
        rf_gl.VertexAttrib2f(5, 0.0f, 0.0f);
        rf_gl.DisableVertexAttribArray(5);
    }

    if (mesh->indices != NULL)
    {
        rf_gl.GenBuffers(1, &mesh->vbo_id[6]);
        rf_gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbo_id[6]);
        rf_gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*mesh->triangle_count * 3, mesh->indices, draw_hint);
    }

    if (mesh->vao_id > 0) rf_log(rf_log_type_info, "[VAO ID %i] rf_mesh uploaded successfully to VRAM (GPU)", mesh->vao_id);
    else rf_log(rf_log_type_warning, "rf_mesh could not be uploaded to VRAM (GPU)");
}

// Update vertex or index data on GPU (upload new data to one buffer)
rf_public void rf_gfx_update_mesh(rf_mesh mesh, int buffer, int num)
{
    rf_gfx_update_mesh_at(mesh, buffer, num, 0);
}

// Update vertex or index data on GPU, at index
// WARNING: error checking is in place that will cause the data to not be
//          updated if offset + size exceeds what the buffer can hold
rf_public void rf_gfx_update_mesh_at(rf_mesh mesh, int buffer, int num, int index)
{
    // Activate mesh VAO
    rf_gl.BindVertexArray(mesh.vao_id);

    switch (buffer)
    {
        case 0:     // Update vertices (vertex position)
        {
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh.vbo_id[0]);
            if (index == 0 && num >= mesh.vertex_count) rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 3*num, mesh.vertices, GL_DYNAMIC_DRAW);
            else if (index + num >= mesh.vertex_count) break;
            else rf_gl.BufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 3*index, sizeof(float) * 3*num, mesh.vertices);

        } break;
        case 1:     // Update texcoords (vertex texture coordinates)
        {
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh.vbo_id[1]);
            if (index == 0 && num >= mesh.vertex_count) rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 2*num, mesh.texcoords, GL_DYNAMIC_DRAW);
            else if (index + num >= mesh.vertex_count) break;
            else rf_gl.BufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 2*index, sizeof(float) * 2*num, mesh.texcoords);

        } break;
        case 2:     // Update normals (vertex normals)
        {
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh.vbo_id[2]);
            if (index == 0 && num >= mesh.vertex_count) rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 3*num, mesh.normals, GL_DYNAMIC_DRAW);
            else if (index + num >= mesh.vertex_count) break;
            else rf_gl.BufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 3*index, sizeof(float) * 3*num, mesh.normals);

        } break;
        case 3:     // Update colors (vertex colors)
        {
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh.vbo_id[3]);
            if (index == 0 && num >= mesh.vertex_count) rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * num, mesh.colors, GL_DYNAMIC_DRAW);
            else if (index + num >= mesh.vertex_count) break;
            else rf_gl.BufferSubData(GL_ARRAY_BUFFER, sizeof(unsigned char) * 4 * index, sizeof(unsigned char) * 4 * num, mesh.colors);

        } break;
        case 4:     // Update tangents (vertex tangents)
        {
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh.vbo_id[4]);
            if (index == 0 && num >= mesh.vertex_count) rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * num, mesh.tangents, GL_DYNAMIC_DRAW);
            else if (index + num >= mesh.vertex_count) break;
            else rf_gl.BufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 4 * index, sizeof(float) * 4 * num, mesh.tangents);
        } break;
        case 5:     // Update texcoords2 (vertex second texture coordinates)
        {
            rf_gl.BindBuffer(GL_ARRAY_BUFFER, mesh.vbo_id[5]);
            if (index == 0 && num >= mesh.vertex_count) rf_gl.BufferData(GL_ARRAY_BUFFER, sizeof(float) * 2*num, mesh.texcoords2, GL_DYNAMIC_DRAW);
            else if (index + num >= mesh.vertex_count) break;
            else rf_gl.BufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 2*index, sizeof(float) * 2*num, mesh.texcoords2);
        } break;
        case 6:     // Update indices (triangle index buffer)
        {
            // the * 3 is because each triangle has 3 indices
            unsigned short *indices = mesh.indices;
            rf_gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbo_id[6]);
            if (index == 0 && num >= mesh.triangle_count)
                rf_gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices)*num * 3, indices, GL_DYNAMIC_DRAW);
            else if (index + num >= mesh.triangle_count)
                break;
            else
                rf_gl.BufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices)*index * 3, sizeof(*indices)*num * 3, indices);
        } break;
        default: break;
    }

    // Unbind the current VAO
    rf_gl.BindVertexArray(0);

    // Another option would be using buffer mapping...
    //mesh.vertices = glMap_buffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
    // Now we can modify vertices
    //glUnmap_buffer(GL_ARRAY_BUFFER);
}

// Draw a 3d mesh with material and transform
rf_public void rf_gfx_draw_mesh(rf_mesh mesh, rf_material material, rf_mat transform)
{
    // Bind shader program
    rf_gl.UseProgram(material.shader.id);

    // Matrices and other values required by shader
    //-----------------------------------------------------
    // Calculate and send to shader model matrix (used by PBR shader)
    if (material.shader.locs[RF_LOC_MATRIX_MODEL] != -1)
        rf_gfx_set_shader_value_matrix(material.shader, material.shader.locs[RF_LOC_MATRIX_MODEL], transform);

    // Upload to shader material.col_diffuse
    if (material.shader.locs[RF_LOC_COLOR_DIFFUSE] != -1)
        rf_gl.Uniform4f(material.shader.locs[RF_LOC_COLOR_DIFFUSE], (float)material.maps[RF_MAP_DIFFUSE].color.r / 255.0f,
                    (float)material.maps[RF_MAP_DIFFUSE].color.g / 255.0f,
                    (float)material.maps[RF_MAP_DIFFUSE].color.b / 255.0f,
                    (float)material.maps[RF_MAP_DIFFUSE].color.a / 255.0f);

    // Upload to shader material.colSpecular (if available)
    if (material.shader.locs[RF_LOC_COLOR_SPECULAR] != -1)
        rf_gl.Uniform4f(material.shader.locs[RF_LOC_COLOR_SPECULAR], (float)material.maps[RF_MAP_SPECULAR].color.r / 255.0f,
                    (float)material.maps[RF_MAP_SPECULAR].color.g / 255.0f,
                    (float)material.maps[RF_MAP_SPECULAR].color.b / 255.0f,
                    (float)material.maps[RF_MAP_SPECULAR].color.a / 255.0f);

    if (material.shader.locs[RF_LOC_MATRIX_VIEW] != -1)
        rf_gfx_set_shader_value_matrix(material.shader, material.shader.locs[RF_LOC_MATRIX_VIEW],
                                       rf_ctx.modelview);
    if (material.shader.locs[RF_LOC_MATRIX_PROJECTION] != -1)
        rf_gfx_set_shader_value_matrix(material.shader, material.shader.locs[RF_LOC_MATRIX_PROJECTION],
                                       rf_ctx.projection);

    // At this point the rf_ctx->gl_ctx.modelview matrix just contains the view matrix (camera)
    // That's because rf_begin_mode3d() sets it an no model-drawing function modifies it, all use rf_gfx_push_matrix() and rf_gfx_pop_matrix()
    rf_mat mat_view = rf_ctx.modelview;         // View matrix (camera)
    rf_mat mat_projection = rf_ctx.projection;  // Projection matrix (perspective)

    // TODO: Consider possible transform matrices in the rf_ctx->gl_ctx.stack
    // Is this the right order? or should we start with the first stored matrix instead of the last one?
    //rf_mat matStackTransform = rf_mat_identity();
    //for (rf_int i = rf_ctx->gl_ctx.stack_counter; i > 0; i--) matStackTransform = rf_mat_mul(rf_ctx->gl_ctx.stack[i], matStackTransform);

    // rf_transform to camera-space coordinates
    rf_mat mat_model_view = rf_mat_mul(transform, rf_mat_mul(rf_ctx.transform, mat_view));
    //-----------------------------------------------------

    // Bind active texture maps (if available)
    for (rf_int i = 0; i < RF_MAX_MATERIAL_MAPS; i++)
    {
        if (material.maps[i].texture.id > 0)
        {
            rf_gl.ActiveTexture(GL_TEXTURE0 + i);
            if ((i == RF_MAP_IRRADIANCE) || (i == RF_MAP_PREFILTER) || (i == RF_MAP_CUBEMAP)) rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, material.maps[i].texture.id);
            else rf_gl.BindTexture(GL_TEXTURE_2D, material.maps[i].texture.id);

            rf_gl.Uniform1i(material.shader.locs[RF_LOC_MAP_DIFFUSE + i], i);
        }
    }

    // Bind vertex array objects (or VBOs)
    rf_gl.BindVertexArray(mesh.vao_id);

    rf_ctx.modelview = mat_model_view;

    // Calculate model-view-rf_ctx->gl_ctx.projection matrix (MVP)
    rf_mat mat_mvp = rf_mat_mul(rf_ctx.modelview, rf_ctx.projection); // rf_transform to screen-space coordinates

    // Send combined model-view-rf_ctx->gl_ctx.projection matrix to shader
    rf_gl.UniformMatrix4fv(material.shader.locs[RF_LOC_MATRIX_MVP], 1, 0, rf_mat_to_float16(mat_mvp).v);

    // Draw call!
    if (mesh.indices != NULL) rf_gl.DrawElements(GL_TRIANGLES, mesh.triangle_count * 3, GL_UNSIGNED_SHORT, 0); // Indexed vertices draw
    else rf_gl.DrawArrays(GL_TRIANGLES, 0, mesh.vertex_count);

    // Unbind all binded texture maps
    for (rf_int i = 0; i < RF_MAX_MATERIAL_MAPS; i++)
    {
        rf_gl.ActiveTexture(GL_TEXTURE0 + i);       // Set shader active texture
        if ((i == RF_MAP_IRRADIANCE) || (i == RF_MAP_PREFILTER) || (i == RF_MAP_CUBEMAP)) rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, 0);
        else rf_gl.BindTexture(GL_TEXTURE_2D, 0);   // Unbind current active texture
    }

    // Unind vertex array objects (or VBOs)
    rf_gl.BindVertexArray(0);

    // Unbind shader program
    rf_gl.UseProgram(0);

    // Restore rf_ctx->gl_ctx.projection/rf_ctx->gl_ctx.modelview matrices
    // NOTE: In stereo rendering matrices are being modified to fit every eye
    rf_ctx.projection = mat_projection;
    rf_ctx.modelview = mat_view;
}

// Unload mesh data from the GPU
rf_public void rf_gfx_unload_mesh(rf_mesh mesh)
{
    rf_gfx_delete_buffers(mesh.vbo_id[0]);   // vertex
    rf_gfx_delete_buffers(mesh.vbo_id[1]);   // texcoords
    rf_gfx_delete_buffers(mesh.vbo_id[2]);   // normals
    rf_gfx_delete_buffers(mesh.vbo_id[3]);   // colors
    rf_gfx_delete_buffers(mesh.vbo_id[4]);   // tangents
    rf_gfx_delete_buffers(mesh.vbo_id[5]);   // texcoords2
    rf_gfx_delete_buffers(mesh.vbo_id[6]);   // indices

    rf_gfx_delete_vertex_arrays(mesh.vao_id);
}

#pragma endregion

#pragma region gen textures
// Generate cubemap texture from HDR texture
rf_public rf_texture2d rf_gen_texture_cubemap(rf_shader shader, rf_texture2d sky_hdr, rf_int size)
{
    rf_texture2d cubemap = { 0 };
    // NOTE: rf_set_shader_default_locations() already setups locations for rf_ctx->gl_ctx.projection and view rf_mat in shader
    // Other locations should be setup externally in shader before calling the function

    // Set up depth face culling and cubemap seamless
    rf_gl.Disable(GL_CULL_FACE);
    rf_gl.Enable(GL_TEXTURE_CUBE_MAP_SEAMLESS);     // Flag not supported on OpenGL ES 2.0

    // Setup framebuffer
    unsigned int fbo, rbo;
    rf_gl.GenFramebuffers(1, &fbo);
    rf_gl.GenRenderbuffers(1, &rbo);
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo);
    rf_gl.BindRenderbuffer(GL_RENDERBUFFER, rbo);
    rf_gl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    rf_gl.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Set up cubemap to render and attach to framebuffer
    // NOTE: Faces are stored as 32 bit floating point values
    rf_gl.GenTextures(1, &cubemap.id);
    rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);
    for (unsigned int i = 0; i < 6; i++)
    {
        rf_gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, NULL);
    }

    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create rf_ctx->gl_ctx.projection and different views for each face
    rf_mat fbo_projection = rf_mat_perspective(90.0 * rf_deg2rad, 1.0, 0.01, 1000.0);
    rf_mat fbo_views[6] = {
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {1.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {-1.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 1.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, 1.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, -1.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, 1.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, -1.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f})
    };

    // Convert HDR equirectangular environment map to cubemap equivalent
    rf_gl.UseProgram(shader.id);
    rf_gl.ActiveTexture(GL_TEXTURE0);
    rf_gl.BindTexture(GL_TEXTURE_2D, sky_hdr.id);
    rf_gfx_set_shader_value_matrix(shader, shader.locs[RF_LOC_MATRIX_PROJECTION], fbo_projection);

    // Note: don't forget to configure the viewport to the capture dimensions
    rf_gl.Viewport(0, 0, size, size);
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo);

    for (rf_int i = 0; i < 6; i++)
    {
        rf_gfx_set_shader_value_matrix(shader, shader.locs[RF_LOC_MATRIX_VIEW], fbo_views[i]);
        rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap.id, 0);
        rf_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        rf_gen_draw_cube();
    }

    // Unbind framebuffer and textures
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset viewport dimensions to default
    rf_gl.Viewport(0, 0, rf_ctx.framebuffer_width, rf_ctx.framebuffer_height);
    //glEnable(GL_CULL_FACE);

    // NOTE: rf_texture2d is a GL_TEXTURE_CUBE_MAP, not a GL_TEXTURE_2D!
    cubemap.width = size;
    cubemap.height = size;
    cubemap.mipmaps = 1;
    cubemap.format = rf_pixel_format_r32g32b32;

    return cubemap;
}

// Generate irradiance texture using cubemap data
rf_public rf_texture2d rf_gen_texture_irradiance(rf_shader shader, rf_texture2d cubemap, rf_int size)
{
    rf_texture2d irradiance = { 0 };

    // NOTE: rf_set_shader_default_locations() already setups locations for rf_ctx->gl_ctx.projection and view rf_mat in shader
    // Other locations should be setup externally in shader before calling the function

    // Setup framebuffer
    unsigned int fbo, rbo;
    rf_gl.GenFramebuffers(1, &fbo);
    rf_gl.GenRenderbuffers(1, &rbo);
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo);
    rf_gl.BindRenderbuffer(GL_RENDERBUFFER, rbo);
    rf_gl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    rf_gl.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Create an irradiance cubemap, and re-scale capture FBO to irradiance scale
    rf_gl.GenTextures(1, &irradiance.id);
    rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, irradiance.id);
    for (unsigned int i = 0; i < 6; i++)
    {
        rf_gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, NULL);
    }

    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create rf_ctx->gl_ctx.projection (transposed) and different views for each face
    rf_mat fbo_projection = rf_mat_perspective(90.0 * rf_deg2rad, 1.0, 0.01, 1000.0);
    rf_mat fbo_views[6] = {
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {1.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {-1.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 1.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, 1.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, -1.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, 1.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, -1.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f})
    };

    // Solve diffuse integral by convolution to create an irradiance cubemap
    rf_gl.UseProgram(shader.id);
    rf_gl.ActiveTexture(GL_TEXTURE0);
    rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);
    rf_gfx_set_shader_value_matrix(shader, shader.locs[RF_LOC_MATRIX_PROJECTION], fbo_projection);

    // Note: don't forget to configure the viewport to the capture dimensions
    rf_gl.Viewport(0, 0, size, size);
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo);

    for (rf_int i = 0; i < 6; i++)
    {
        rf_gfx_set_shader_value_matrix(shader, shader.locs[RF_LOC_MATRIX_VIEW], fbo_views[i]);
        rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradiance.id, 0);
        rf_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        rf_gen_draw_cube();
    }

    // Unbind framebuffer and textures
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset viewport dimensions to default
    rf_gl.Viewport(0, 0, rf_ctx.framebuffer_width, rf_ctx.framebuffer_height);

    irradiance.width = size;
    irradiance.height = size;
    irradiance.mipmaps = 1;
    //irradiance.format = UNCOMPRESSED_R16G16B16;

    return irradiance;
}

// Generate prefilter texture using cubemap data
rf_public rf_texture2d rf_gen_texture_prefilter(rf_shader shader, rf_texture2d cubemap, rf_int size)
{
    rf_texture2d prefilter = { 0 };

    // NOTE: rf_set_shader_default_locations() already setups locations for projection and view rf_mat in shader
    // Other locations should be setup externally in shader before calling the function
    // TODO: Locations should be taken out of this function... too shader dependant...
    int roughness_loc = rf_gfx_get_shader_location(shader, "roughness");

    // Setup framebuffer
    unsigned int fbo, rbo;
    rf_gl.GenFramebuffers(1, &fbo);
    rf_gl.GenRenderbuffers(1, &rbo);
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo);
    rf_gl.BindRenderbuffer(GL_RENDERBUFFER, rbo);
    rf_gl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    rf_gl.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Create a prefiltered HDR environment map
    rf_gl.GenTextures(1, &prefilter.id);
    rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, prefilter.id);
    for (unsigned int i = 0; i < 6; i++)
    {
        rf_gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, NULL);
    }

    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    rf_gl.TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Generate mipmaps for the prefiltered HDR texture
    rf_gl.GenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // Create rf_ctx->gl_ctx.projection (transposed) and different views for each face
    rf_mat fbo_projection = rf_mat_perspective(90.0 * rf_deg2rad, 1.0, 0.01, 1000.0);
    rf_mat fbo_views[6] = {
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {1.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {-1.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 1.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, 1.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, -1.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, 1.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f}),
            rf_mat_look_at((rf_vec3) {0.0f, 0.0f, 0.0f}, (rf_vec3) {0.0f, 0.0f, -1.0f}, (rf_vec3) {0.0f, -1.0f, 0.0f})
    };

    // Prefilter HDR and store data into mipmap levels
    rf_gl.UseProgram(shader.id);
    rf_gl.ActiveTexture(GL_TEXTURE0);
    rf_gl.BindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);
    rf_gfx_set_shader_value_matrix(shader, shader.locs[RF_LOC_MATRIX_PROJECTION], fbo_projection);

    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo);

#define MAX_MIPMAP_LEVELS   5   // Max number of prefilter texture mipmaps

    for (rf_int mip = 0; mip < MAX_MIPMAP_LEVELS; mip++)
    {
        // Resize framebuffer according to mip-level size.
        unsigned int mip_width  = size*(int)powf(0.5f, (float)mip);
        unsigned int mip_height = size*(int)powf(0.5f, (float)mip);

        rf_gl.BindRenderbuffer(GL_RENDERBUFFER, rbo);
        rf_gl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mip_width, mip_height);
        rf_gl.Viewport(0, 0, mip_width, mip_height);

        float roughness = (float)mip/(float)(MAX_MIPMAP_LEVELS - 1);
        rf_gl.Uniform1f(roughness_loc, roughness);

        for (rf_int i = 0; i < 6; i++)
        {
            rf_gfx_set_shader_value_matrix(shader, shader.locs[RF_LOC_MATRIX_VIEW], fbo_views[i]);
            rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilter.id, mip);
            rf_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            rf_gen_draw_cube();
        }
    }

    // Unbind framebuffer and textures
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset viewport dimensions to default
    rf_gl.Viewport(0, 0, rf_ctx.framebuffer_width, rf_ctx.framebuffer_height);

    prefilter.width = size;
    prefilter.height = size;
    //prefilter.mipmaps = 1 + (int)floor(log(size)/log(2));
    //prefilter.format = UNCOMPRESSED_R16G16B16;

    return prefilter;
}

// Generate BRDF texture using cubemap data. Todo: Review implementation: https://github.com/HectorMF/BRDFGenerator
rf_public rf_texture2d rf_gen_texture_brdf(rf_shader shader, rf_int size)
{
    rf_texture2d brdf = { 0 };
    // Generate BRDF convolution texture
    rf_gl.GenTextures(1, &brdf.id);
    rf_gl.BindTexture(GL_TEXTURE_2D, brdf.id);
    rf_gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, NULL);

    rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    rf_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Render BRDF LUT into a quad using FBO
    unsigned int fbo, rbo;
    rf_gl.GenFramebuffers(1, &fbo);
    rf_gl.GenRenderbuffers(1, &rbo);
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, fbo);
    rf_gl.BindRenderbuffer(GL_RENDERBUFFER, rbo);
    rf_gl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    rf_gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdf.id, 0);

    rf_gl.Viewport(0, 0, size, size);
    rf_gl.UseProgram(shader.id);
    rf_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    rf_gen_draw_quad();

    // Unbind framebuffer and textures
    rf_gl.BindFramebuffer(GL_FRAMEBUFFER, 0);

    // Unload framebuffer but keep color texture
    rf_gl.DeleteRenderbuffers(1, &rbo);
    rf_gl.DeleteFramebuffers(1, &fbo);

    // Reset viewport dimensions to default
    rf_gl.Viewport(0, 0, rf_ctx.framebuffer_width, rf_ctx.framebuffer_height);

    brdf.width = size;
    brdf.height = size;
    brdf.mipmaps = 1;
    brdf.format = rf_pixel_format_r32g32b32;

    return brdf;
}

#pragma endregion


#endif

#if defined(RAYFORK_GRAPHICS_BACKEND_METAL) || defined(RAYFORK_GRAPHICS_BACKEND_DIRECTX)


#define SOKOL_IMPL
#if defined(RAYFORK_GRAPHICS_BACKEND_DIRECTX)
#define SOKOL_D3D11
#elif defined(RAYFORK_GRAPHICS_BACKEND_METAL)
#define SOKOL_METAL
#endif
#include "sokol_gfx.h"

rf_internal void rf__gfx_backend_internal_init(rf_gfx_backend_data* gfx_data);

#pragma region shader

// Load default shader (just vertex positioning and texture coloring). Note: This shader program is used for internal buffers
RF_API rf_shader rf_load_default_shader();

// Load shader from code strings. If shader string is NULL, using default vertex/fragment shaders
RF_API rf_shader rf_gfx_load_shader(const char* vs_code, const char* fs_code);

// Unload shader from GPU memory (VRAM)
RF_API void rf_gfx_unload_shader(rf_shader shader);

// Get shader uniform location
RF_API int rf_gfx_get_shader_location(rf_shader shader, const char* uniform_name);

// Set shader uniform value
RF_API void rf_gfx_set_shader_value(rf_shader shader, int uniform_loc, const void* value, int uniform_name);

// Set shader uniform value vector
RF_API void rf_gfx_set_shader_value_v(rf_shader shader, int uniform_loc, const void* value, int uniform_name, int count);

// Set shader uniform value (matrix 4x4)
RF_API void rf_gfx_set_shader_value_matrix(rf_shader shader, int uniform_loc, rf_mat mat);

// Set shader uniform value for texture
RF_API void rf_gfx_set_shader_value_texture(rf_shader shader, int uniform_loc, rf_texture2d texture);

// Return internal rf_ctx->gl_ctx.projection matrix
RF_API rf_mat rf_gfx_get_matrix_projection();

// Return internal rf_ctx->gl_ctx.modelview matrix
RF_API rf_mat rf_gfx_get_matrix_modelview();

// Set a custom projection matrix (replaces internal rf_ctx->gl_ctx.projection matrix)
RF_API void rf_gfx_set_matrix_projection(rf_mat proj);

// Set a custom rf_ctx->gl_ctx.modelview matrix (replaces internal rf_ctx->gl_ctx.modelview matrix)
RF_API void rf_gfx_set_matrix_modelview(rf_mat view);

#pragma endregion

#pragma region rf_gfx

// Choose the blending mode (alpha, additive, multiplied)
RF_API void rf_gfx_blend_mode(rf_blend_mode mode);

// Choose the current matrix to be transformed
RF_API void rf_gfx_matrix_mode(rf_matrix_mode mode);

// Push the current matrix into rf_ctx->gl_ctx.stack
RF_API void rf_gfx_push_matrix();

// Pop lattest inserted matrix from rf_ctx->gl_ctx.stack
RF_API void rf_gfx_pop_matrix();

// Reset current matrix to identity matrix
RF_API void rf_gfx_load_identity();

// Multiply the current matrix by a translation matrix
RF_API void rf_gfx_translatef(float x, float y, float z);

// Multiply the current matrix by a rotation matrix
RF_API void rf_gfx_rotatef(float angleDeg, float x, float y, float z);

// Multiply the current matrix by a scaling matrix
RF_API void rf_gfx_scalef(float x, float y, float z);

// Multiply the current matrix by another matrix
RF_API void rf_gfx_mult_matrixf(float* matf);

// Multiply the current matrix by a perspective matrix generated by parameters
RF_API void rf_gfx_frustum(double left, double right, double bottom, double top, double znear, double zfar);

// Multiply the current matrix by an orthographic matrix generated by parameters
RF_API void rf_gfx_ortho(double left, double right, double bottom, double top, double znear, double zfar);

// Set the viewport area (transformation from normalized device coordinates to window coordinates)
void rf_gfx_viewport(int x, int y, int width, int height);

// Initialize drawing mode (how to organize vertex)
RF_API void rf_gfx_begin(rf_drawing_mode mode);

// Finish vertex providing
RF_API void rf_gfx_end();

// Define one vertex (position)
RF_API void rf_gfx_vertex2i(int x, int y);

// Define one vertex (position)
RF_API void rf_gfx_vertex2f(float x, float y);

// Define one vertex (position)
// NOTE: Vertex position data is the basic information required for drawing
RF_API void rf_gfx_vertex3f(float x, float y, float z);

// Define one vertex (texture coordinate)
// NOTE: rf_texture coordinates are limited to QUADS only
RF_API void rf_gfx_tex_coord2f(float x, float y);

// Define one vertex (normal)
// NOTE: Normals limited to TRIANGLES only?
RF_API void rf_gfx_normal3f(float x, float y, float z);

// Define one vertex (color)
RF_API void rf_gfx_color3f(float x, float y, float z);

// Define one vertex (color)
RF_API void rf_gfx_color4ub(unsigned char x, unsigned char y, unsigned char z, unsigned char w);

// Define one vertex (color)
RF_API void rf_gfx_color4f(float r, float g, float b, float a);

// Enable texture usage
RF_API void rf_gfx_enable_texture(unsigned int id);

// Disable texture usage
RF_API void rf_gfx_disable_texture();

// Set texture parameters (wrap mode/filter mode)
RF_API void rf_gfx_set_texture_wrap(rf_texture2d texture, rf_texture_wrap_mode wrap_mode);

// Set filter for texture
RF_API void rf_gfx_set_texture_filter(rf_texture2d texture, rf_texture_filter_mode filter_mode);

// Enable rendering to texture (fbo)
RF_API void rf_gfx_enable_render_texture(unsigned int id);

// Disable rendering to texture
RF_API void rf_gfx_disable_render_texture(void);

// Enable depth test
RF_API void rf_gfx_enable_depth_test(void);

// Disable depth test
RF_API void rf_gfx_disable_depth_test(void);

// Enable backface culling
RF_API void rf_gfx_enable_backface_culling(void);

// Disable backface culling
RF_API void rf_gfx_disable_backface_culling(void);

// Enable scissor test
RF_API void rf_gfx_enable_scissor_test(void);

// Disable scissor test
RF_API void rf_gfx_disable_scissor_test(void);

// Scissor test
RF_API void rf_gfx_scissor(int x, int y, int width, int height);

// Enable wire mode
RF_API void rf_gfx_enable_wire_mode(void);

// Disable wire mode
RF_API void rf_gfx_disable_wire_mode(void);

// Unload texture from GPU memory
RF_API void rf_gfx_delete_textures(unsigned int id);

// Unload render texture from GPU memory
RF_API void rf_gfx_delete_render_textures(rf_render_texture2d target);

// Unload shader from GPU memory
RF_API void rf_gfx_delete_shader(unsigned int id);

// Unload vertex data (VAO) from GPU memory
RF_API void rf_gfx_delete_vertex_arrays(unsigned int id);

// Unload vertex data (VBO) from GPU memory
RF_API void rf_gfx_delete_buffers(unsigned int id);

// Clear color buffer with color
RF_API void rf_gfx_clear_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

// Clear used screen buffers (color and depth)
RF_API void rf_gfx_clear_screen_buffers(void);

// Update GPU buffer with new data
RF_API void rf_gfx_update_buffer(int buffer_id, void* data, int data_size);

// Load a new attributes buffer
RF_API unsigned int rf_gfx_load_attrib_buffer(unsigned int vao_id, int shader_loc, void* buffer, int size, rf_bool dynamic);

RF_API void rf_gfx_init_vertex_buffer(rf_vertex_buffer* vertex_buffer);

// Vertex Buffer Object deinitialization (memory free)
RF_API void rf_gfx_close();

// Update and draw internal buffers
RF_API void rf_gfx_draw();

// Check internal buffer overflow for a given number of vertex
RF_API rf_bool rf_gfx_check_buffer_limit(int v_count);

// Set debug marker
RF_API void rf_gfx_set_debug_marker(const char* text);

// Convert image data to OpenGL texture (returns OpenGL valid Id)
RF_API unsigned int rf_gfx_load_texture(void* data, int width, int height, rf_pixel_format format, int mipmap_count);

// Load depth texture/renderbuffer (to be attached to fbo)
// WARNING: OpenGL ES 2.0 requires GL_OES_depth_texture/WEBGL_depth_texture extensions
RF_API unsigned int rf_gfx_load_texture_depth(int width, int height, int bits, rf_bool use_render_buffer);

// Load texture cubemap
// NOTE: Cubemap data is expected to be 6 images in a single column,
// expected the following convention: +X, -X, +Y, -Y, +Z, -Z
RF_API unsigned int rf_gfx_load_texture_cubemap(void* data, int size, rf_pixel_format format);

// Update already loaded texture in GPU with new data
// NOTE: We don't know safely if internal texture format is the expected one...
RF_API void rf_gfx_update_texture(unsigned int id, int width, int height, rf_pixel_format format, const void* pixels, int pixels_size);

// Get OpenGL internal formats and data type from raylib rf_pixel_format
RF_API rf_gfx_pixel_format rf_gfx_get_internal_texture_formats(rf_pixel_format format);

// Unload texture from GPU memory
RF_API void rf_gfx_unload_texture(unsigned int id);

// Generate mipmap data for selected texture
RF_API void rf_gfx_generate_mipmaps(rf_texture2d* texture);

// Read texture pixel data
RF_API rf_image rf_gfx_read_texture_pixels_to_buffer(rf_texture2d texture, void* dst, int dst_size);

RF_API rf_image rf_gfx_read_texture_pixels(rf_texture2d texture, rf_allocator allocator);

// Read screen pixel data (color buffer)
RF_API void rf_gfx_read_screen_pixels(rf_color* dst, int width, int height);

// Load a texture to be used for rendering (fbo with default color and depth attachments)
// NOTE: If colorFormat or depth_bits are no supported, no attachment is done
RF_API rf_render_texture2d rf_gfx_load_render_texture(int width, int height, rf_pixel_format format, int depth_bits, rf_bool use_depth_texture);

// Attach color buffer texture to an fbo (unloads previous attachment)
// NOTE: Attach type: 0-rf_color, 1-Depth renderbuffer, 2-Depth texture
RF_API void rf_gfx_render_texture_attach(rf_render_texture2d target, unsigned int id, int attach_type);

// Verify render texture is complete
RF_API rf_bool rf_gfx_render_texture_complete(rf_render_texture2d target);

// Upload vertex data into a VAO (if supported) and VBO
RF_API void rf_gfx_load_mesh(rf_mesh* mesh, rf_bool dynamic);

// Update vertex or index data on GPU (upload new data to one buffer)
RF_API void rf_gfx_update_mesh(rf_mesh mesh, int buffer, int num);

// Update vertex or index data on GPU, at index
// WARNING: error checking is in place that will cause the data to not be
//          updated if offset + size exceeds what the buffer can hold
RF_API void rf_gfx_update_mesh_at(rf_mesh mesh, int buffer, int num, int index);

// Draw a 3d mesh with material and transform
RF_API void rf_gfx_draw_mesh(rf_mesh mesh, rf_material material, rf_mat transform);

// Unload mesh data from the GPU
RF_API void rf_gfx_unload_mesh(rf_mesh mesh);

#pragma endregion

#pragma region gen textures

// Generate cubemap texture from HDR texture
RF_API rf_texture2d rf_gen_texture_cubemap(rf_shader shader, rf_texture2d sky_hdr, rf_int size);

// Generate irradiance texture using cubemap data
RF_API rf_texture2d rf_gen_texture_irradiance(rf_shader shader, rf_texture2d cubemap, rf_int size);

// Generate prefilter texture using cubemap data
RF_API rf_texture2d rf_gen_texture_prefilter(rf_shader shader, rf_texture2d cubemap, rf_int size);

// Generate BRDF texture using cubemap data. Todo: Review implementation: https://github.com/HectorMF/BRDFGenerator
RF_API rf_texture2d rf_gen_texture_brdf(rf_shader shader, rf_int size);

#pragma endregion

#endif


// Get world coordinates from screen coordinates
rf_public rf_vec3 rf_unproject(rf_vec3 source, rf_mat proj, rf_mat view)
{
    rf_vec3 result = {0.0f, 0.0f, 0.0f};

    // Calculate unproject matrix (multiply view patrix by rf_ctx->gl_ctx.projection matrix) and invert it
    rf_mat mat_viewProj = rf_mat_mul(view, proj);
    mat_viewProj = rf_mat_invert(mat_viewProj);

    // Create quaternion from source point
    rf_quaternion quat = {source.x, source.y, source.z, 1.0f};

    // Multiply quat point by unproject matrix
    quat = rf_quaternion_transform(quat, mat_viewProj);

    // Normalized world points in vectors
    result.x = quat.x / quat.w;
    result.y = quat.y / quat.w;
    result.z = quat.z / quat.w;

    return result;
}

// Returns a ray trace from mouse position
rf_public rf_ray rf_get_mouse_ray(rf_sizei screen_size, rf_vec2 mouse_position, rf_camera3d camera)
{
    rf_ray ray = {0};

    // Calculate normalized device coordinates
    // NOTE: y value is negative
    float x = (2.0f * mouse_position.x) / (float) screen_size.width - 1.0f;
    float y = 1.0f - (2.0f * mouse_position.y) / (float) screen_size.height;
    float z = 1.0f;

    // Store values in a vector
    rf_vec3 device_coords = { x, y, z };

    // Calculate view matrix from camera look at
    rf_mat mat_view = rf_mat_look_at(camera.position, camera.target, camera.up);

    rf_mat mat_proj = rf_mat_identity();

    if (camera.type == RF_CAMERA_PERSPECTIVE)
    {
        // Calculate projection matrix from perspective
        mat_proj = rf_mat_perspective(camera.fovy * rf_deg2rad, ((double) screen_size.width / (double) screen_size.height), 0.01, 1000.0);
    }
    else if (camera.type == RF_CAMERA_ORTHOGRAPHIC)
    {
        float aspect = (float) screen_size.width / (float) screen_size.height;
        double top = camera.fovy / 2.0;
        double right = top * aspect;

        // Calculate projection matrix from orthographic
        mat_proj = rf_mat_ortho(-right, right, -top, top, 0.01, 1000.0);
    }

    // Unproject far/near points
    rf_vec3 near_point = rf_unproject((rf_vec3) {device_coords.x, device_coords.y, 0.0f}, mat_proj, mat_view);
    rf_vec3 far_point  = rf_unproject((rf_vec3) {device_coords.x, device_coords.y, 1.0f}, mat_proj, mat_view);

    // Unproject the mouse cursor in the near plane.
    // We need this as the source position because orthographic projects, compared to perspect doesn't have a
    // convergence point, meaning that the "eye" of the camera is more like a plane than a point.
    rf_vec3 camera_plane_pointer_pos = rf_unproject((rf_vec3) {device_coords.x, device_coords.y, -1.0f}, mat_proj,
                                                    mat_view);

    // Calculate normalized direction vector
    rf_vec3 direction = rf_vec3_normalize(rf_vec3_sub(far_point, near_point));

    if (camera.type == RF_CAMERA_PERSPECTIVE)
    {
        ray.position = camera.position;
    }
    else if (camera.type == RF_CAMERA_ORTHOGRAPHIC)
    {
        ray.position = camera_plane_pointer_pos;
    }

    // Apply calculated vectors to ray
    ray.direction = direction;

    return ray;
}

// Get transform matrix for camera
rf_public rf_mat rf_get_camera_matrix(rf_camera3d camera)
{
    return rf_mat_look_at(camera.position, camera.target, camera.up);
}

// Returns camera 2d transform matrix
rf_public rf_mat rf_get_camera_matrix2d(rf_camera2d camera)
{
    rf_mat mat_transform = {0};
// The camera in world-space is set by
//   1. Move it to target
//   2. Rotate by -rotation and scale by (1/zoom)
//      When setting higher scale, it's more intuitive for the world to become bigger (= camera become smaller),
//      not for the camera getting bigger, hence the invert. Same deal with rotation.
//   3. Move it by (-offset);
//      Offset defines target transform relative to screen, but since we're effectively "moving" screen (camera)
//      we need to do it into opposite direction (inverse transform)

// Having camera transform in world-space, inverse of it gives the rf_gfxobal_model_view transform.
// Since (A*B*C)' = C'*B'*A', the rf_gfxobal_model_view is
//   1. Move to offset
//   2. Rotate and Scale
//   3. Move by -target
    rf_mat mat_origin = rf_mat_translate(-camera.target.x, -camera.target.y, 0.0f);
    rf_mat mat_rotation = rf_mat_rotate((rf_vec3) {0.0f, 0.0f, 1.0f}, camera.rotation * rf_deg2rad);
    rf_mat mat_scale = rf_mat_scale(camera.zoom, camera.zoom, 1.0f);
    rf_mat mat_translation = rf_mat_translate(camera.offset.x, camera.offset.y, 0.0f);

    mat_transform = rf_mat_mul(rf_mat_mul(mat_origin, rf_mat_mul(mat_scale, mat_rotation)), mat_translation);

    return mat_transform;
}

// Returns the screen space position from a 3d world space position
rf_public rf_vec2 rf_get_world_to_screen(rf_sizei screen_size, rf_vec3 position, rf_camera3d camera)
{
    // Calculate projection matrix from perspective instead of frustum
    rf_mat mat_proj = rf_mat_identity();

    if (camera.type == RF_CAMERA_PERSPECTIVE)
    {
        // Calculate projection matrix from perspective
        mat_proj = rf_mat_perspective(camera.fovy * rf_deg2rad, ((double) screen_size.width / (double) screen_size.height), 0.01, 1000.0);
    }
    else if (camera.type == RF_CAMERA_ORTHOGRAPHIC)
    {
        float aspect = (float) screen_size.width / (float) screen_size.height;
        double top = camera.fovy / 2.0;
        double right = top * aspect;

    // Calculate projection matrix from orthographic
        mat_proj = rf_mat_ortho(-right, right, -top, top, 0.01, 1000.0);
    }

    // Calculate view matrix from camera look at (and transpose it)
    rf_mat mat_view = rf_mat_look_at(camera.position, camera.target, camera.up);

    // Convert world position vector to quaternion
    rf_quaternion world_pos = { position.x, position.y, position.z, 1.0f };

    // Transform world position to view
    world_pos = rf_quaternion_transform(world_pos, mat_view);

    // Transform result to projection (clip space position)
    world_pos = rf_quaternion_transform(world_pos, mat_proj);

    // Calculate normalized device coordinates (inverted y)
    rf_vec3 ndc_pos = {world_pos.x / world_pos.w, -world_pos.y / world_pos.w, world_pos.z / world_pos.w};

    // Calculate 2d screen position vector
    rf_vec2 screen_position = {(ndc_pos.x + 1.0f) / 2.0f * (float) screen_size.width,
                               (ndc_pos.y + 1.0f) / 2.0f * (float) screen_size.height};

    return screen_position;
}

// Returns the screen space position for a 2d camera world space position
rf_public rf_vec2 rf_get_world_to_screen2d(rf_vec2 position, rf_camera2d camera)
{
    rf_mat mat_camera = rf_get_camera_matrix2d(camera);
    rf_vec3 transform = rf_vec3_transform((rf_vec3) {position.x, position.y, 0}, mat_camera);

    return (rf_vec2) {transform.x, transform.y};
}

// Returns the world space position for a 2d camera screen space position
rf_public rf_vec2 rf_get_screen_to_world2d(rf_vec2 position, rf_camera2d camera)
{
    rf_mat inv_mat_camera = rf_mat_invert(rf_get_camera_matrix2d(camera));
    rf_vec3 transform = rf_vec3_transform((rf_vec3) {position.x, position.y, 0}, inv_mat_camera);

    return (rf_vec2) {transform.x, transform.y};
}

// Select camera mode (multiple camera modes available)
rf_public void rf_set_camera3d_mode(rf_camera3d_state* state, rf_camera3d camera, rf_builtin_camera3d_mode mode)
{
    rf_vec3 v1 = camera.position;
    rf_vec3 v2 = camera.target;

    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    float dz = v2.z - v1.z;

    state->camera_target_distance = sqrtf(dx*dx + dy*dy + dz*dz);

    rf_vec2 distance = { 0.0f, 0.0f };
    distance.x = sqrtf(dx*dx + dz*dz);
    distance.y = sqrtf(dx*dx + dy*dy);

    // rf_camera3d angle calculation
    state->camera_angle.x = asinf((float)fabs(dx)/distance.x); // rf_camera3d angle in plane XZ (0 aligned with Z, move positive CCW)
    state->camera_angle.y = -asinf((float)fabs(dy)/distance.y); // rf_camera3d angle in plane XY (0 aligned with X, move positive CW)

    state->player_eyes_position = camera.position.y;

    // Lock cursor for first person and third person cameras
    // if ((mode == rf_camera_first_person) || (mode == rf_camera_third_person)) DisableCursor();
    // else EnableCursor();

    state->camera_mode = mode;
}

// Update camera depending on selected mode
// NOTE: rf_camera3d controls depend on some raylib functions:
//       System: EnableCursor(), DisableCursor()
//       Mouse: IsMouseButtonDown(), GetMousePosition(), GetMouseWheelMove()
//       Keys:  IsKeyDown()
// TODO: Port to quaternion-based camera
rf_public void rf_update_camera3d(rf_camera3d* camera, rf_camera3d_state* state, rf_input_state_for_update_camera input_state)
{
    // rf_camera3d mouse movement sensitivity
    #define RF_CAMERA_MOUSE_MOVE_SENSITIVITY 0.003f
    #define RF_CAMERA_MOUSE_SCROLL_SENSITIVITY 1.5f

    // FREE_CAMERA
    #define RF_CAMERA_FREE_MOUSE_SENSITIVITY 0.01f
    #define RF_CAMERA_FREE_DISTANCE_MIN_CLAMP 0.3f
    #define RF_CAMERA_FREE_DISTANCE_MAX_CLAMP 120.0f
    #define RF_CAMERA_FREE_MIN_CLAMP 85.0f
    #define RF_CAMERA_FREE_MAX_CLAMP -85.0f
    #define RF_CAMERA_FREE_SMOOTH_ZOOM_SENSITIVITY 0.05f
    #define RF_CAMERA_FREE_PANNING_DIVIDER 5.1f

    // ORBITAL_CAMERA
    #define RF_CAMERA_ORBITAL_SPEED 0.01f // Radians per frame

    // FIRST_PERSON
    //#define CAMERA_FIRST_PERSON_MOUSE_SENSITIVITY           0.003f
    #define RF_CAMERA_FIRST_PERSON_FOCUS_DISTANCE 25.0f
    #define RF_CAMERA_FIRST_PERSON_MIN_CLAMP 85.0f
    #define RF_CAMERA_FIRST_PERSON_MAX_CLAMP -85.0f

    #define RF_CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER 5.0f
    #define RF_CAMERA_FIRST_PERSON_STEP_DIVIDER 30.0f
    #define RF_CAMERA_FIRST_PERSON_WAVING_DIVIDER 200.0f

    // THIRD_PERSON
    //#define CAMERA_THIRD_PERSON_MOUSE_SENSITIVITY           0.003f
    #define RF_CAMERA_THIRD_PERSON_DISTANCE_CLAMP 1.2f
    #define RF_CAMERA_THIRD_PERSON_MIN_CLAMP 5.0f
    #define RF_CAMERA_THIRD_PERSON_MAX_CLAMP -85.0f
    #define RF_CAMERA_THIRD_PERSON_OFFSET (rf_vec3) { 0.4f, 0.0f, 0.0f }

    // PLAYER (used by camera)
    #define RF_PLAYER_MOVEMENT_SENSITIVITY 20.0f

    // rf_camera3d move modes (first person and third person cameras)
    typedef enum rf_camera_move
    {
        rf_move_front = 0,
        rf_move_back,
        rf_move_right,
        rf_move_left,
        rf_move_up,
        rf_move_down
    } rf_camera_move;

    // rf_internal float player_eyes_position = 1.85f;

    // TODO: CRF_INTERNAL rf_ctx->gl_ctx.camera_target_distance and rf_ctx->gl_ctx.camera_angle here

    // Mouse movement detection
    rf_vec2 mouse_position_delta = { 0.0f, 0.0f };
    rf_vec2 mouse_position = input_state.mouse_position;
    int mouse_wheel_move = input_state.mouse_wheel_move;

    // Keys input detection
    rf_bool pan_key = input_state.is_camera_pan_control_key_down;
    rf_bool alt_key = input_state.is_camera_alt_control_key_down;
    rf_bool szoom_key = input_state.is_camera_smooth_zoom_control_key;

    rf_bool direction[6];
    direction[0] = input_state.direction_keys[0];
    direction[1] = input_state.direction_keys[1];
    direction[2] = input_state.direction_keys[2];
    direction[3] = input_state.direction_keys[3];
    direction[4] = input_state.direction_keys[4];
    direction[5] = input_state.direction_keys[5];

    // TODO: Consider touch inputs for camera

    if (state->camera_mode != RF_CAMERA_CUSTOM)
    {
        mouse_position_delta.x = mouse_position.x - state->previous_mouse_position.x;
        mouse_position_delta.y = mouse_position.y - state->previous_mouse_position.y;

        state->previous_mouse_position = mouse_position;
    }

    // Support for multiple automatic camera modes
    switch (state->camera_mode)
    {
        case RF_CAMERA_FREE:
        {
            // Camera zoom
            if ((state->camera_target_distance < RF_CAMERA_FREE_DISTANCE_MAX_CLAMP) && (mouse_wheel_move < 0))
            {
                state->camera_target_distance -= (mouse_wheel_move * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY);

                if (state->camera_target_distance > RF_CAMERA_FREE_DISTANCE_MAX_CLAMP) {
                    state->camera_target_distance = RF_CAMERA_FREE_DISTANCE_MAX_CLAMP;
                }
            }
            // Camera looking down
            // TODO: Review, weird comparisson of rf_ctx->gl_ctx.camera_target_distance == 120.0f?
            else if ((camera->position.y > camera->target.y) && (state->camera_target_distance == RF_CAMERA_FREE_DISTANCE_MAX_CLAMP) && (mouse_wheel_move < 0))
            {
                camera->target.x += mouse_wheel_move * (camera->target.x - camera->position.x) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.y += mouse_wheel_move * (camera->target.y - camera->position.y) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.z += mouse_wheel_move * (camera->target.z - camera->position.z) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
            }
            else if ((camera->position.y > camera->target.y) && (camera->target.y >= 0))
            {
                camera->target.x += mouse_wheel_move * (camera->target.x - camera->position.x) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.y += mouse_wheel_move * (camera->target.y - camera->position.y) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.z += mouse_wheel_move * (camera->target.z - camera->position.z) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;

                // if (camera->target.y < 0) camera->target.y = -0.001;
            }
            else if ((camera->position.y > camera->target.y) && (camera->target.y < 0) && (mouse_wheel_move > 0))
            {
                state->camera_target_distance -= (mouse_wheel_move * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY);
                if (state->camera_target_distance < RF_CAMERA_FREE_DISTANCE_MIN_CLAMP) {
                    state->camera_target_distance = RF_CAMERA_FREE_DISTANCE_MIN_CLAMP;
                }
            }
            // Camera looking up
            // TODO: Review, weird comparisson of rf_ctx->gl_ctx.camera_target_distance == 120.0f?
            else if ((camera->position.y < camera->target.y) && (state->camera_target_distance == RF_CAMERA_FREE_DISTANCE_MAX_CLAMP) && (mouse_wheel_move < 0))
            {
                camera->target.x += mouse_wheel_move * (camera->target.x - camera->position.x) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.y += mouse_wheel_move * (camera->target.y - camera->position.y) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.z += mouse_wheel_move * (camera->target.z - camera->position.z) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
            }
            else if ((camera->position.y < camera->target.y) && (camera->target.y <= 0))
            {
                camera->target.x += mouse_wheel_move * (camera->target.x - camera->position.x) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.y += mouse_wheel_move * (camera->target.y - camera->position.y) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;
                camera->target.z += mouse_wheel_move * (camera->target.z - camera->position.z) * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY / state->camera_target_distance;

                // if (camera->target.y > 0) camera->target.y = 0.001;
            }
            else if ((camera->position.y < camera->target.y) && (camera->target.y > 0) && (mouse_wheel_move > 0))
            {
                state->camera_target_distance -= (mouse_wheel_move * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY);
                if (state->camera_target_distance < RF_CAMERA_FREE_DISTANCE_MIN_CLAMP) {
                    state->camera_target_distance = RF_CAMERA_FREE_DISTANCE_MIN_CLAMP;
                }
            }

            // Input keys checks
            if (pan_key)
            {
                if (alt_key) // Alternative key behaviour
                {
                    if (szoom_key)
                    {
                        // Camera smooth zoom
                        state->camera_target_distance += (mouse_position_delta.y * RF_CAMERA_FREE_SMOOTH_ZOOM_SENSITIVITY);
                    }
                    else
                    {
                        // Camera rotation
                        state->camera_angle.x += mouse_position_delta.x*-RF_CAMERA_FREE_MOUSE_SENSITIVITY;
                        state->camera_angle.y += mouse_position_delta.y*-RF_CAMERA_FREE_MOUSE_SENSITIVITY;

                        // Angle clamp
                        if (state->camera_angle.y > RF_CAMERA_FREE_MIN_CLAMP * rf_deg2rad) {
                            state->camera_angle.y = RF_CAMERA_FREE_MIN_CLAMP * rf_deg2rad;
                        }
                        else if (state->camera_angle.y < RF_CAMERA_FREE_MAX_CLAMP * rf_deg2rad) {
                            state->camera_angle.y = RF_CAMERA_FREE_MAX_CLAMP * rf_deg2rad;
                        }
                    }
                }
                else
                {
                    // Camera panning
                    camera->target.x += ((mouse_position_delta.x*-RF_CAMERA_FREE_MOUSE_SENSITIVITY) * cosf(state->camera_angle.x) + (mouse_position_delta.y * RF_CAMERA_FREE_MOUSE_SENSITIVITY) * sinf(state->camera_angle.x) * sinf(state->camera_angle.y)) * (state->camera_target_distance / RF_CAMERA_FREE_PANNING_DIVIDER);
                    camera->target.y += ((mouse_position_delta.y * RF_CAMERA_FREE_MOUSE_SENSITIVITY) * cosf(state->camera_angle.y)) * (state->camera_target_distance / RF_CAMERA_FREE_PANNING_DIVIDER);
                    camera->target.z += ((mouse_position_delta.x * RF_CAMERA_FREE_MOUSE_SENSITIVITY) * sinf(state->camera_angle.x) + (mouse_position_delta.y * RF_CAMERA_FREE_MOUSE_SENSITIVITY) * cosf(state->camera_angle.x) * sinf(state->camera_angle.y)) * (state->camera_target_distance / RF_CAMERA_FREE_PANNING_DIVIDER);
                }
            }

            // Update camera position with changes
            camera->position.x = sinf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.x;
            camera->position.y = ((state->camera_angle.y <= 0.0f)? 1 : -1)*sinf(state->camera_angle.y)*state->camera_target_distance*sinf(state->camera_angle.y) + camera->target.y;
            camera->position.z = cosf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.z;

        } break;

        case RF_CAMERA_ORBITAL:
        {
            state->camera_angle.x += RF_CAMERA_ORBITAL_SPEED; // Camera orbit angle
            state->camera_target_distance -= (mouse_wheel_move * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY); // Camera zoom

            // Camera distance clamp
            if (state->camera_target_distance < RF_CAMERA_THIRD_PERSON_DISTANCE_CLAMP) {
                state->camera_target_distance = RF_CAMERA_THIRD_PERSON_DISTANCE_CLAMP;
            }

            // Update camera position with changes
            camera->position.x = sinf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.x;
            camera->position.y = ((state->camera_angle.y <= 0.0f)? 1 : -1)*sinf(state->camera_angle.y)*state->camera_target_distance*sinf(state->camera_angle.y) + camera->target.y;
            camera->position.z = cosf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.z;

        } break;

        case RF_CAMERA_FIRST_PERSON:
        {
            camera->position.x += (sinf(state->camera_angle.x)*direction[rf_move_back] -
                                   sinf(state->camera_angle.x)*direction[rf_move_front] -
                                   cosf(state->camera_angle.x)*direction[rf_move_left] +
                                   cosf(state->camera_angle.x)*direction[rf_move_right]) / RF_PLAYER_MOVEMENT_SENSITIVITY;

            camera->position.y += (sinf(state->camera_angle.y)*direction[rf_move_front] -
                                   sinf(state->camera_angle.y)*direction[rf_move_back] +
                                   1.0f*direction[rf_move_up] - 1.0f*direction[rf_move_down]) / RF_PLAYER_MOVEMENT_SENSITIVITY;

            camera->position.z += (cosf(state->camera_angle.x)*direction[rf_move_back] -
                                   cosf(state->camera_angle.x)*direction[rf_move_front] +
                                   sinf(state->camera_angle.x)*direction[rf_move_left] -
                                   sinf(state->camera_angle.x)*direction[rf_move_right]) / RF_PLAYER_MOVEMENT_SENSITIVITY;

            rf_bool is_moving = 0; // Required for swinging

            for (rf_int i = 0; i < 6; i++) if (direction[i]) { is_moving = 1; break; }

            // Camera orientation calculation
            state->camera_angle.x += (mouse_position_delta.x*-RF_CAMERA_MOUSE_MOVE_SENSITIVITY);
            state->camera_angle.y += (mouse_position_delta.y*-RF_CAMERA_MOUSE_MOVE_SENSITIVITY);

            // Angle clamp
            if (state->camera_angle.y > RF_CAMERA_FIRST_PERSON_MIN_CLAMP * rf_deg2rad) {
                state->camera_angle.y = RF_CAMERA_FIRST_PERSON_MIN_CLAMP * rf_deg2rad;
            }
            else if (state->camera_angle.y < RF_CAMERA_FIRST_PERSON_MAX_CLAMP * rf_deg2rad) {
                state->camera_angle.y = RF_CAMERA_FIRST_PERSON_MAX_CLAMP * rf_deg2rad;
            }

            // Camera is always looking at player
            camera->target.x = camera->position.x - sinf(state->camera_angle.x) * RF_CAMERA_FIRST_PERSON_FOCUS_DISTANCE;
            camera->target.y = camera->position.y + sinf(state->camera_angle.y) * RF_CAMERA_FIRST_PERSON_FOCUS_DISTANCE;
            camera->target.z = camera->position.z - cosf(state->camera_angle.x) * RF_CAMERA_FIRST_PERSON_FOCUS_DISTANCE;

            if (is_moving) {
                state->swing_counter++;
            }

            // Camera position update
            // NOTE: On RF_CAMERA_FIRST_PERSON player Y-movement is limited to player 'eyes position'
            camera->position.y = state->player_eyes_position - sinf(state->swing_counter / RF_CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER) / RF_CAMERA_FIRST_PERSON_STEP_DIVIDER;

            camera->up.x = sinf(state->swing_counter/(RF_CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER * 2)) / RF_CAMERA_FIRST_PERSON_WAVING_DIVIDER;
            camera->up.z = -sinf(state->swing_counter/(RF_CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER * 2)) / RF_CAMERA_FIRST_PERSON_WAVING_DIVIDER;


        } break;

        case RF_CAMERA_THIRD_PERSON:
        {
            camera->position.x += (sinf(state->camera_angle.x)*direction[rf_move_back] -
                                   sinf(state->camera_angle.x)*direction[rf_move_front] -
                                   cosf(state->camera_angle.x)*direction[rf_move_left] +
                                   cosf(state->camera_angle.x)*direction[rf_move_right]) / RF_PLAYER_MOVEMENT_SENSITIVITY;

            camera->position.y += (sinf(state->camera_angle.y)*direction[rf_move_front] -
                                   sinf(state->camera_angle.y)*direction[rf_move_back] +
                                   1.0f*direction[rf_move_up] - 1.0f*direction[rf_move_down]) / RF_PLAYER_MOVEMENT_SENSITIVITY;

            camera->position.z += (cosf(state->camera_angle.x)*direction[rf_move_back] -
                                   cosf(state->camera_angle.x)*direction[rf_move_front] +
                                   sinf(state->camera_angle.x)*direction[rf_move_left] -
                                   sinf(state->camera_angle.x)*direction[rf_move_right]) / RF_PLAYER_MOVEMENT_SENSITIVITY;

            // Camera orientation calculation
            state->camera_angle.x += (mouse_position_delta.x*-RF_CAMERA_MOUSE_MOVE_SENSITIVITY);
            state->camera_angle.y += (mouse_position_delta.y*-RF_CAMERA_MOUSE_MOVE_SENSITIVITY);

            // Angle clamp
            if (state->camera_angle.y > RF_CAMERA_THIRD_PERSON_MIN_CLAMP * rf_deg2rad)
            {
                state->camera_angle.y = RF_CAMERA_THIRD_PERSON_MIN_CLAMP * rf_deg2rad;
            }
            else if (state->camera_angle.y < RF_CAMERA_THIRD_PERSON_MAX_CLAMP * rf_deg2rad)
            {
                state->camera_angle.y = RF_CAMERA_THIRD_PERSON_MAX_CLAMP * rf_deg2rad;
            }

            // Camera zoom
            state->camera_target_distance -= (mouse_wheel_move * RF_CAMERA_MOUSE_SCROLL_SENSITIVITY);

            // Camera distance clamp
            if (state->camera_target_distance < RF_CAMERA_THIRD_PERSON_DISTANCE_CLAMP)
            {
                state->camera_target_distance = RF_CAMERA_THIRD_PERSON_DISTANCE_CLAMP;
            }

            // TODO: It seems camera->position is not correctly updated or some rounding issue makes the camera move straight to camera->target...
            camera->position.x = sinf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.x;

            if (state->camera_angle.y <= 0.0f)
            {
                camera->position.y = sinf(state->camera_angle.y)*state->camera_target_distance*sinf(state->camera_angle.y) + camera->target.y;
            }
            else
            {
                camera->position.y = -sinf(state->camera_angle.y)*state->camera_target_distance*sinf(state->camera_angle.y) + camera->target.y;
            }

            camera->position.z = cosf(state->camera_angle.x)*state->camera_target_distance*cosf(state->camera_angle.y) + camera->target.z;

        } break;

        default: break;
    }
}



#pragma region pixel format

rf_public const char* rf_pixel_format_string(rf_pixel_format format)
{
    switch (format)
    {
        case rf_pixel_format_grayscale: return "RF_UNCOMPRESSED_GRAYSCALE";
        case rf_pixel_format_gray_alpha: return "RF_UNCOMPRESSED_GRAY_ALPHA";
        case rf_pixel_format_r5g6b5: return "RF_UNCOMPRESSED_R5G6B5";
        case rf_pixel_format_r8g8b8: return "RF_UNCOMPRESSED_R8G8B8";
        case rf_pixel_format_r5g5b5a1: return "RF_UNCOMPRESSED_R5G5B5A1";
        case rf_pixel_format_r4g4b4a4: return "RF_UNCOMPRESSED_R4G4B4A4";
        case rf_pixel_format_r8g8b8a8: return "RF_UNCOMPRESSED_R8G8B8A8";
        case rf_pixel_format_r32: return "RF_UNCOMPRESSED_R32";
        case rf_pixel_format_r32g32b32: return "RF_UNCOMPRESSED_R32G32B32";
        case rf_pixel_format_r32g32b32a32: return "RF_UNCOMPRESSED_R32G32B32A32";
        case rf_pixel_format_dxt1_rgb: return "RF_COMPRESSED_DXT1_RGB";
        case rf_pixel_format_dxt1_rgba: return "RF_COMPRESSED_DXT1_RGBA";
        case rf_pixel_format_dxt3_rgba: return "RF_COMPRESSED_DXT3_RGBA";
        case rf_pixel_format_dxt5_rgba: return "RF_COMPRESSED_DXT5_RGBA";
        case rf_pixel_format_etc1_rgb: return "RF_COMPRESSED_ETC1_RGB";
        case rf_pixel_format_etc2_rgb: return "RF_COMPRESSED_ETC2_RGB";
        case rf_pixel_format_etc2_eac_rgba: return "RF_COMPRESSED_ETC2_EAC_RGBA";
        case rf_pixel_format_pvrt_rgb: return "RF_COMPRESSED_PVRT_RGB";
        case rf_pixel_format_prvt_rgba: return "RF_COMPRESSED_PVRT_RGBA";
        case rf_pixel_format_astc_4x4_rgba: return "RF_COMPRESSED_ASTC_4x4_RGBA";
        case rf_pixel_format_astc_8x8_rgba: return "RF_COMPRESSED_ASTC_8x8_RGBA";
        default: return NULL;
    }
}

rf_public rf_bool rf_is_uncompressed_format(rf_pixel_format format)
{
    return format >= rf_pixel_format_grayscale && format <= rf_pixel_format_r32g32b32a32;
}

rf_public rf_bool rf_is_compressed_format(rf_pixel_format format)
{
    return format >= rf_pixel_format_dxt1_rgb && format <= rf_pixel_format_astc_8x8_rgba;
}

rf_public int rf_bits_per_pixel(rf_pixel_format format)
{
    switch (format)
    {
        case rf_pixel_format_grayscale: return 8; // 8 bit per pixel (no alpha)
        case rf_pixel_format_gray_alpha: return 8 * 2; // 8 * 2 bpp (2 channels)
        case rf_pixel_format_r5g6b5: return 16; // 16 bpp
        case rf_pixel_format_r8g8b8: return 24; // 24 bpp
        case rf_pixel_format_r5g5b5a1: return 16; // 16 bpp (1 bit alpha)
        case rf_pixel_format_r4g4b4a4: return 16; // 16 bpp (4 bit alpha)
        case rf_pixel_format_r8g8b8a8: return 32; // 32 bpp
        case rf_pixel_format_r32: return 32; // 32 bpp (1 channel - float)
        case rf_pixel_format_r32g32b32: return 32 * 3; // 32 * 3 bpp (3 channels - float)
        case rf_pixel_format_r32g32b32a32: return 32 * 4; // 32 * 4 bpp (4 channels - float)
        case rf_pixel_format_dxt1_rgb: return 4; // 4 bpp (no alpha)
        case rf_pixel_format_dxt1_rgba: return 4; // 4 bpp (1 bit alpha)
        case rf_pixel_format_dxt3_rgba: return 8; // 8 bpp
        case rf_pixel_format_dxt5_rgba: return 8; // 8 bpp
        case rf_pixel_format_etc1_rgb: return 4; // 4 bpp
        case rf_pixel_format_etc2_rgb: return 4; // 4 bpp
        case rf_pixel_format_etc2_eac_rgba: return 8; // 8 bpp
        case rf_pixel_format_pvrt_rgb: return 4; // 4 bpp
        case rf_pixel_format_prvt_rgba: return 4; // 4 bpp
        case rf_pixel_format_astc_4x4_rgba: return 8; // 8 bpp
        case rf_pixel_format_astc_8x8_rgba: return 2; // 2 bpp
        default: return 0;
    }
}

rf_public int rf_bytes_per_pixel(rf_uncompressed_pixel_format format)
{
    switch (format)
    {
        case rf_pixel_format_grayscale: return 1;
        case rf_pixel_format_gray_alpha: return 2;
        case rf_pixel_format_r5g5b5a1: return 2;
        case rf_pixel_format_r5g6b5: return 2;
        case rf_pixel_format_r4g4b4a4: return 2;
        case rf_pixel_format_r8g8b8a8: return 4;
        case rf_pixel_format_r8g8b8: return 3;
        case rf_pixel_format_r32: return 4;
        case rf_pixel_format_r32g32b32: return 12;
        case rf_pixel_format_r32g32b32a32: return 16;
        default: return 0;
    }
}

rf_public int rf_pixel_buffer_size(int width, int height, rf_pixel_format format)
{
    return width * height * rf_bits_per_pixel(format) / 8;
}

rf_public rf_bool rf_format_pixels_to_normalized(const void* src, rf_int src_size, rf_uncompressed_pixel_format src_format, rf_vec4* dst, rf_int dst_size)
{
    rf_bool success = 0;

    rf_int src_bpp = rf_bytes_per_pixel(src_format);
    rf_int src_pixel_count = src_size / src_bpp;
    rf_int dst_pixel_count = dst_size / sizeof(rf_vec4);

    if (dst_pixel_count >= src_pixel_count)
    {
        if (src_format == rf_pixel_format_r32g32b32a32)
        {
            success = 1;
            memcpy(dst, src, src_size);
        }
        else
        {
            success = 1;

            #define RF_FOR_EACH_PIXEL for (rf_int dst_iter = 0, src_iter = 0; src_iter < src_size && dst_iter < dst_size; dst_iter++, src_iter += src_bpp)
            switch (src_format)
            {
                case rf_pixel_format_grayscale:
                    RF_FOR_EACH_PIXEL
                    {
                        float value = ((unsigned char*)src)[src_iter] / 255.0f;

                        dst[dst_iter].x = value;
                        dst[dst_iter].y = value;
                        dst[dst_iter].z = value;
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case rf_pixel_format_gray_alpha:
                    RF_FOR_EACH_PIXEL
                    {
                        float value0 = (float)((unsigned char*)src)[src_iter + 0] / 255.0f;
                        float value1 = (float)((unsigned char*)src)[src_iter + 1] / 255.0f;

                        dst[dst_iter].x = value0;
                        dst[dst_iter].y = value0;
                        dst[dst_iter].z = value0;
                        dst[dst_iter].w = value1;
                    }
                    break;

                case rf_pixel_format_r5g5b5a1:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned short pixel = ((unsigned short*) src)[src_iter];

                        dst[dst_iter].x = (float)((pixel & 0b1111100000000000) >> 11) * (1.0f/31);
                        dst[dst_iter].y = (float)((pixel & 0b0000011111000000) >>  6) * (1.0f/31);
                        dst[dst_iter].z = (float)((pixel & 0b0000000000111110) >>  1) * (1.0f/31);
                        dst[dst_iter].w = ((pixel & 0b0000000000000001) == 0) ? 0.0f : 1.0f;
                    }
                    break;

                case rf_pixel_format_r5g6b5:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned short pixel = ((unsigned short*)src)[src_iter];

                        dst[dst_iter].x = (float)((pixel & 0b1111100000000000) >> 11) * (1.0f / 31);
                        dst[dst_iter].y = (float)((pixel & 0b0000011111100000) >>  5) * (1.0f / 63);
                        dst[dst_iter].z = (float) (pixel & 0b0000000000011111)        * (1.0f / 31);
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case rf_pixel_format_r4g4b4a4:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned short pixel = ((unsigned short*)src)[src_iter];

                        dst[dst_iter].x = (float)((pixel & 0b1111000000000000) >> 12) * (1.0f / 15);
                        dst[dst_iter].y = (float)((pixel & 0b0000111100000000) >> 8)  * (1.0f / 15);
                        dst[dst_iter].z = (float)((pixel & 0b0000000011110000) >> 4)  * (1.0f / 15);
                        dst[dst_iter].w = (float) (pixel & 0b0000000000001111)        * (1.0f / 15);
                    }
                    break;

                case rf_pixel_format_r8g8b8a8:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].x = (float)((unsigned char*)src)[src_iter + 0] / 255.0f;
                        dst[dst_iter].y = (float)((unsigned char*)src)[src_iter + 1] / 255.0f;
                        dst[dst_iter].z = (float)((unsigned char*)src)[src_iter + 2] / 255.0f;
                        dst[dst_iter].w = (float)((unsigned char*)src)[src_iter + 3] / 255.0f;
                    }
                    break;

                case rf_pixel_format_r8g8b8:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].x = (float)((unsigned char*)src)[src_iter + 0] / 255.0f;
                        dst[dst_iter].y = (float)((unsigned char*)src)[src_iter + 1] / 255.0f;
                        dst[dst_iter].z = (float)((unsigned char*)src)[src_iter + 2] / 255.0f;
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case rf_pixel_format_r32:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].x = ((float*)src)[src_iter];
                        dst[dst_iter].y = 0.0f;
                        dst[dst_iter].z = 0.0f;
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case rf_pixel_format_r32g32b32:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].x = ((float*)src)[src_iter + 0];
                        dst[dst_iter].y = ((float*)src)[src_iter + 1];
                        dst[dst_iter].z = ((float*)src)[src_iter + 2];
                        dst[dst_iter].w = 1.0f;
                    }
                    break;

                case rf_pixel_format_r32g32b32a32:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].x = ((float*)src)[src_iter + 0];
                        dst[dst_iter].y = ((float*)src)[src_iter + 1];
                        dst[dst_iter].z = ((float*)src)[src_iter + 2];
                        dst[dst_iter].w = ((float*)src)[src_iter + 3];
                    }
                    break;

                default: break;
            }
            #undef RF_FOR_EACH_PIXEL
        }
    }
    else rf_log_error(rf_bad_buffer_size, "Buffer is size %d but function expected a size of at least %d.", dst_size, src_pixel_count * sizeof(rf_vec4));

    return success;
}

rf_public rf_bool rf_format_pixels_to_rgba32(const void* src, rf_int src_size, rf_uncompressed_pixel_format src_format, rf_color* dst, rf_int dst_size)
{
    rf_bool success = 0;

    rf_int src_bpp = rf_bytes_per_pixel(src_format);
    rf_int src_pixel_count = src_size / src_bpp;
    rf_int dst_pixel_count = dst_size / sizeof(rf_color);

    if (dst_pixel_count >= src_pixel_count)
    {
        if (src_format == rf_pixel_format_r8g8b8a8)
        {
            success = 1;
            memcpy(dst, src, src_size);
        }
        else
        {
            success = 1;
            #define RF_FOR_EACH_PIXEL for (rf_int dst_iter = 0, src_iter = 0; src_iter < src_size && dst_iter < dst_size; dst_iter++, src_iter += src_bpp)
            switch (src_format)
            {
                case rf_pixel_format_grayscale:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned char value = ((unsigned char*) src)[src_iter];
                        dst[dst_iter].r = value;
                        dst[dst_iter].g = value;
                        dst[dst_iter].b = value;
                        dst[dst_iter].a = 255;
                    }
                    break;

                case rf_pixel_format_gray_alpha:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned char value0 = ((unsigned char*) src)[src_iter + 0];
                        unsigned char value1 = ((unsigned char*) src)[src_iter + 1];

                        dst[dst_iter].r = value0;
                        dst[dst_iter].g = value0;
                        dst[dst_iter].b = value0;
                        dst[dst_iter].a = value1;
                    }
                    break;

                case rf_pixel_format_r5g5b5a1:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned short pixel = ((unsigned short*) src)[src_iter];

                        dst[dst_iter].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11) * (255 / 31));
                        dst[dst_iter].g = (unsigned char)((float)((pixel & 0b0000011111000000) >>  6) * (255 / 31));
                        dst[dst_iter].b = (unsigned char)((float)((pixel & 0b0000000000111110) >>  1) * (255 / 31));
                        dst[dst_iter].a = (unsigned char)        ((pixel & 0b0000000000000001)        *  255);
                    }
                    break;

                case rf_pixel_format_r5g6b5:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned short pixel = ((unsigned short*) src)[src_iter];

                        dst[dst_iter].r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)* (255 / 31));
                        dst[dst_iter].g = (unsigned char)((float)((pixel & 0b0000011111100000) >>  5)* (255 / 63));
                        dst[dst_iter].b = (unsigned char)((float) (pixel & 0b0000000000011111)       * (255 / 31));
                        dst[dst_iter].a = 255;
                    }
                    break;

                case rf_pixel_format_r4g4b4a4:
                    RF_FOR_EACH_PIXEL
                    {
                        unsigned short pixel = ((unsigned short*) src)[src_iter];

                        dst[dst_iter].r = (unsigned char)((float)((pixel & 0b1111000000000000) >> 12) * (255 / 15));
                        dst[dst_iter].g = (unsigned char)((float)((pixel & 0b0000111100000000) >> 8)  * (255 / 15));
                        dst[dst_iter].b = (unsigned char)((float)((pixel & 0b0000000011110000) >> 4)  * (255 / 15));
                        dst[dst_iter].a = (unsigned char)((float) (pixel & 0b0000000000001111)        * (255 / 15));
                    }
                    break;

                case rf_pixel_format_r8g8b8a8:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].r = ((unsigned char*) src)[src_iter + 0];
                        dst[dst_iter].g = ((unsigned char*) src)[src_iter + 1];
                        dst[dst_iter].b = ((unsigned char*) src)[src_iter + 2];
                        dst[dst_iter].a = ((unsigned char*) src)[src_iter + 3];
                    }
                    break;

                case rf_pixel_format_r8g8b8:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].r = (unsigned char)((unsigned char*) src)[src_iter + 0];
                        dst[dst_iter].g = (unsigned char)((unsigned char*) src)[src_iter + 1];
                        dst[dst_iter].b = (unsigned char)((unsigned char*) src)[src_iter + 2];
                        dst[dst_iter].a = 255;
                    }
                    break;

                case rf_pixel_format_r32:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].r = (unsigned char)(((float*) src)[src_iter + 0] * 255.0f);
                        dst[dst_iter].g = 0;
                        dst[dst_iter].b = 0;
                        dst[dst_iter].a = 255;
                    }
                    break;

                case rf_pixel_format_r32g32b32:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].r = (unsigned char)(((float*) src)[src_iter + 0] * 255.0f);
                        dst[dst_iter].g = (unsigned char)(((float*) src)[src_iter + 1] * 255.0f);
                        dst[dst_iter].b = (unsigned char)(((float*) src)[src_iter + 2] * 255.0f);
                        dst[dst_iter].a = 255;
                    }
                    break;

                case rf_pixel_format_r32g32b32a32:
                    RF_FOR_EACH_PIXEL
                    {
                        dst[dst_iter].r = (unsigned char)(((float*) src)[src_iter + 0] * 255.0f);
                        dst[dst_iter].g = (unsigned char)(((float*) src)[src_iter + 1] * 255.0f);
                        dst[dst_iter].b = (unsigned char)(((float*) src)[src_iter + 2] * 255.0f);
                        dst[dst_iter].a = (unsigned char)(((float*) src)[src_iter + 3] * 255.0f);
                    }
                    break;

                default: break;
            }
            #undef RF_FOR_EACH_PIXEL
        }
    }
    else rf_log_error(rf_bad_buffer_size, "Buffer is size %d but function expected a size of at least %d", dst_size, src_pixel_count * sizeof(rf_color));

    return success;
}

rf_public rf_bool rf_format_pixels(const void* src, rf_int src_size, rf_uncompressed_pixel_format src_format, void* dst, rf_int dst_size, rf_uncompressed_pixel_format dst_format)
{
    rf_bool success = 0;

    if (rf_is_uncompressed_format(src_format) && dst_format == rf_pixel_format_r32g32b32a32)
    {
        success = rf_format_pixels_to_normalized(src, src_size, src_format, dst, dst_size);
    }
    else if (rf_is_uncompressed_format(src_format) && dst_format == rf_pixel_format_r8g8b8a8)
    {
        success = rf_format_pixels_to_rgba32(src, src_size, src_format, dst, dst_size);
    }
    else if (rf_is_uncompressed_format(src_format) && rf_is_uncompressed_format(dst_format))
    {
        rf_int src_bpp = rf_bytes_per_pixel(src_format);
        rf_int dst_bpp = rf_bytes_per_pixel(dst_format);

        rf_int src_pixel_count = src_size / src_bpp;
        rf_int dst_pixel_count = dst_size / dst_bpp;

        if (dst_pixel_count >= src_pixel_count)
        {
            success = 1;

            //Loop over both src and dst
            #define RF_FOR_EACH_PIXEL for (rf_int src_iter = 0, dst_iter = 0; src_iter < src_size && dst_iter < dst_size; src_iter += src_bpp, dst_iter += dst_bpp)
            #define RF_COMPUTE_NORMALIZED_PIXEL() rf_format_one_pixel_to_normalized(((unsigned char*) src) + src_iter, src_format);
            if (src_format == dst_format)
            {
                memcpy(dst, src, src_size);
            }
            else
            {
                switch (dst_format)
                {
                    case rf_pixel_format_grayscale:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();
                            ((unsigned char*)dst)[dst_iter] = (unsigned char)((normalized.x * 0.299f + normalized.y * 0.587f + normalized.z * 0.114f) * 255.0f);
                        }
                        break;

                    case rf_pixel_format_gray_alpha:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();

                            ((unsigned char*)dst)[dst_iter    ] = (unsigned char)((normalized.x * 0.299f + (float)normalized.y * 0.587f + (float)normalized.z * 0.114f) * 255.0f);
                            ((unsigned char*)dst)[dst_iter + 1] = (unsigned char) (normalized.w * 255.0f);
                        }
                        break;

                    case rf_pixel_format_r5g6b5:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();

                            unsigned char r = (unsigned char)(round(normalized.x * 31.0f));
                            unsigned char g = (unsigned char)(round(normalized.y * 63.0f));
                            unsigned char b = (unsigned char)(round(normalized.z * 31.0f));

                            ((unsigned short*)dst)[dst_iter] = (unsigned short)r << 11 | (unsigned short)g << 5 | (unsigned short)b;
                        }
                        break;

                    case rf_pixel_format_r8g8b8:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();

                            ((unsigned char*)dst)[dst_iter    ] = (unsigned char)(normalized.x * 255.0f);
                            ((unsigned char*)dst)[dst_iter + 1] = (unsigned char)(normalized.y * 255.0f);
                            ((unsigned char*)dst)[dst_iter + 2] = (unsigned char)(normalized.z * 255.0f);
                        }
                        break;

                    case rf_pixel_format_r5g5b5a1:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();

                            int ALPHA_THRESHOLD = 50;
                            unsigned char r = (unsigned char)(round(normalized.x * 31.0f));
                            unsigned char g = (unsigned char)(round(normalized.y * 31.0f));
                            unsigned char b = (unsigned char)(round(normalized.z * 31.0f));
                            unsigned char a = (normalized.w > ((float)ALPHA_THRESHOLD / 255.0f)) ? 1 : 0;

                            ((unsigned short*)dst)[dst_iter] = (unsigned short)r << 11 | (unsigned short)g << 6 | (unsigned short)b << 1 | (unsigned short)a;
                        }
                        break;

                    case rf_pixel_format_r4g4b4a4:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();

                            unsigned char r = (unsigned char)(round(normalized.x * 15.0f));
                            unsigned char g = (unsigned char)(round(normalized.y * 15.0f));
                            unsigned char b = (unsigned char)(round(normalized.z * 15.0f));
                            unsigned char a = (unsigned char)(round(normalized.w * 15.0f));

                            ((unsigned short*)dst)[dst_iter] = (unsigned short)r << 12 | (unsigned short)g << 8 | (unsigned short)b << 4 | (unsigned short)a;
                        }
                        break;

                    case rf_pixel_format_r32:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();

                            ((float*)dst)[dst_iter] = (float)(normalized.x * 0.299f + normalized.y * 0.587f + normalized.z * 0.114f);
                        }
                        break;

                    case rf_pixel_format_r32g32b32:
                        RF_FOR_EACH_PIXEL
                        {
                            rf_vec4 normalized = RF_COMPUTE_NORMALIZED_PIXEL();

                            ((float*)dst)[dst_iter    ] = normalized.x;
                            ((float*)dst)[dst_iter + 1] = normalized.y;
                            ((float*)dst)[dst_iter + 2] = normalized.z;
                        }
                        break;

                    default: break;
                }
            }
            #undef RF_FOR_EACH_PIXEL
            #undef RF_COMPUTE_NORMALIZED_PIXEL
        }
        else rf_log_error(rf_bad_buffer_size, "Buffer is size %d but function expected a size of at least %d.", dst_size, src_pixel_count * dst_bpp);
    }
    else rf_log_error(rf_bad_argument, "Function expected uncompressed pixel formats. Source format: %d, Destination format: %d.", src_format, dst_format);

    return success;
}

rf_public rf_vec4 rf_format_one_pixel_to_normalized(const void* src, rf_uncompressed_pixel_format src_format)
{
    rf_vec4 result = {0};

    switch (src_format)
    {
        case rf_pixel_format_grayscale:
        {
            float value = ((unsigned char*)src)[0] / 255.0f;

            result.x = value;
            result.y = value;
            result.z = value;
            result.w = 1.0f;
        }
        break;

        case rf_pixel_format_gray_alpha:
        {
            float value0 = (float)((unsigned char*)src)[0] / 255.0f;
            float value1 = (float)((unsigned char*)src)[1] / 255.0f;

            result.x = value0;
            result.y = value0;
            result.z = value0;
            result.w = value1;
        }
        break;

        case rf_pixel_format_r5g5b5a1:
        {
            unsigned short pixel = ((unsigned short*) src)[0];

            result.x = (float)((pixel & 0b1111100000000000) >> 11) * (1.0f/31);
            result.y = (float)((pixel & 0b0000011111000000) >>  6) * (1.0f/31);
            result.z = (float)((pixel & 0b0000000000111110) >>  1) * (1.0f/31);
            result.w = ((pixel & 0b0000000000000001) == 0) ? 0.0f : 1.0f;
        }
        break;

        case rf_pixel_format_r5g6b5:
        {
            unsigned short pixel = ((unsigned short*)src)[0];

            result.x = (float)((pixel & 0b1111100000000000) >> 11) * (1.0f / 31);
            result.y = (float)((pixel & 0b0000011111100000) >>  5) * (1.0f / 63);
            result.z = (float) (pixel & 0b0000000000011111)        * (1.0f / 31);
            result.w = 1.0f;
        }
        break;

        case rf_pixel_format_r4g4b4a4:
        {
            unsigned short pixel = ((unsigned short*)src)[0];

            result.x = (float)((pixel & 0b1111000000000000) >> 12) * (1.0f / 15);
            result.y = (float)((pixel & 0b0000111100000000) >> 8)  * (1.0f / 15);
            result.z = (float)((pixel & 0b0000000011110000) >> 4)  * (1.0f / 15);
            result.w = (float) (pixel & 0b0000000000001111)        * (1.0f / 15);
        }
        break;

        case rf_pixel_format_r8g8b8a8:
        {
            result.x = (float)((unsigned char*)src)[0] / 255.0f;
            result.y = (float)((unsigned char*)src)[1] / 255.0f;
            result.z = (float)((unsigned char*)src)[2] / 255.0f;
            result.w = (float)((unsigned char*)src)[3] / 255.0f;
        }
        break;

        case rf_pixel_format_r8g8b8:
        {
            result.x = (float)((unsigned char*)src)[0] / 255.0f;
            result.y = (float)((unsigned char*)src)[1] / 255.0f;
            result.z = (float)((unsigned char*)src)[2] / 255.0f;
            result.w = 1.0f;
        }
        break;

        case rf_pixel_format_r32:
        {
            result.x = ((float*)src)[0];
            result.y = 0.0f;
            result.z = 0.0f;
            result.w = 1.0f;
        }
        break;

        case rf_pixel_format_r32g32b32:
        {
            result.x = ((float*)src)[0];
            result.y = ((float*)src)[1];
            result.z = ((float*)src)[2];
            result.w = 1.0f;
        }
        break;

        case rf_pixel_format_r32g32b32a32:
        {
            result.x = ((float*)src)[0];
            result.y = ((float*)src)[1];
            result.z = ((float*)src)[2];
            result.w = ((float*)src)[3];
        }
        break;

        default: break;
    }

    return result;
}

rf_public rf_color rf_format_one_pixel_to_rgba32(const void* src, rf_uncompressed_pixel_format src_format)
{
    rf_color result = {0};

    switch (src_format)
    {
        case rf_pixel_format_grayscale:
        {
            unsigned char value = ((unsigned char*) src)[0];
            result.r = value;
            result.g = value;
            result.b = value;
            result.a = 255;
        }
        break;

        case rf_pixel_format_gray_alpha:
        {
            unsigned char value0 = ((unsigned char*) src)[0];
            unsigned char value1 = ((unsigned char*) src)[1];

            result.r = value0;
            result.g = value0;
            result.b = value0;
            result.a = value1;
        }
        break;

        case rf_pixel_format_r5g5b5a1:
        {
            unsigned short pixel = ((unsigned short*) src)[0];

            result.r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11) * (255 / 31));
            result.g = (unsigned char)((float)((pixel & 0b0000011111000000) >>  6) * (255 / 31));
            result.b = (unsigned char)((float)((pixel & 0b0000000000111110) >>  1) * (255 / 31));
            result.a = (unsigned char)        ((pixel & 0b0000000000000001)        *  255);
        }
        break;

        case rf_pixel_format_r5g6b5:
        {
            unsigned short pixel = ((unsigned short*) src)[0];

            result.r = (unsigned char)((float)((pixel & 0b1111100000000000) >> 11)* (255 / 31));
            result.g = (unsigned char)((float)((pixel & 0b0000011111100000) >>  5)* (255 / 63));
            result.b = (unsigned char)((float) (pixel & 0b0000000000011111)       * (255 / 31));
            result.a = 255;
        }
        break;

        case rf_pixel_format_r4g4b4a4:
        {
            unsigned short pixel = ((unsigned short*) src)[0];

            result.r = (unsigned char)((float)((pixel & 0b1111000000000000) >> 12) * (255 / 15));
            result.g = (unsigned char)((float)((pixel & 0b0000111100000000) >> 8)  * (255 / 15));
            result.b = (unsigned char)((float)((pixel & 0b0000000011110000) >> 4)  * (255 / 15));
            result.a = (unsigned char)((float) (pixel & 0b0000000000001111)        * (255 / 15));
        }
        break;

        case rf_pixel_format_r8g8b8a8:
        {
            result.r = ((unsigned char*) src)[0];
            result.g = ((unsigned char*) src)[1];
            result.b = ((unsigned char*) src)[2];
            result.a = ((unsigned char*) src)[3];
        }
        break;

        case rf_pixel_format_r8g8b8:
        {
            result.r = (unsigned char)((unsigned char*) src)[0];
            result.g = (unsigned char)((unsigned char*) src)[1];
            result.b = (unsigned char)((unsigned char*) src)[2];
            result.a = 255;
        }
        break;

        case rf_pixel_format_r32:
        {
            result.r = (unsigned char)(((float*) src)[0] * 255.0f);
            result.g = 0;
            result.b = 0;
            result.a = 255;
        }
        break;

        case rf_pixel_format_r32g32b32:
        {
            result.r = (unsigned char)(((float*) src)[0] * 255.0f);
            result.g = (unsigned char)(((float*) src)[1] * 255.0f);
            result.b = (unsigned char)(((float*) src)[2] * 255.0f);
            result.a = 255;
        }
        break;

        case rf_pixel_format_r32g32b32a32:
        {
            result.r = (unsigned char)(((float*) src)[0] * 255.0f);
            result.g = (unsigned char)(((float*) src)[1] * 255.0f);
            result.b = (unsigned char)(((float*) src)[2] * 255.0f);
            result.a = (unsigned char)(((float*) src)[3] * 255.0f);
        }
        break;

        default: break;
    }

    return result;
}

rf_public void rf_format_one_pixel(const void* src, rf_uncompressed_pixel_format src_format, void* dst, rf_uncompressed_pixel_format dst_format)
{
    if (src_format == dst_format && rf_is_uncompressed_format(src_format) && rf_is_uncompressed_format(dst_format))
    {
        memcpy(dst, src, rf_bytes_per_pixel(src_format));
    }
    else if (rf_is_uncompressed_format(src_format) && dst_format == rf_pixel_format_r32g32b32a32)
    {
        *((rf_vec4*)dst) = rf_format_one_pixel_to_normalized(src, src_format);
    }
    else if (rf_is_uncompressed_format(src_format) && dst_format == rf_pixel_format_r8g8b8a8)
    {
        *((rf_color*)dst) = rf_format_one_pixel_to_rgba32(src, src_format);
    }
    else if (rf_is_uncompressed_format(src_format) && rf_is_uncompressed_format(dst_format))
    {
        switch (dst_format)
        {
            case rf_pixel_format_grayscale:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((unsigned char*)dst)[0] = (unsigned char)((normalized.x * 0.299f + normalized.y * 0.587f + normalized.z * 0.114f) * 255.0f);
            }
            break;

            case rf_pixel_format_gray_alpha:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((unsigned char*)dst)[0    ] = (unsigned char)((normalized.x * 0.299f + (float)normalized.y * 0.587f + (float)normalized.z * 0.114f) * 255.0f);
                ((unsigned char*)dst)[0 + 1] = (unsigned char) (normalized.w * 255.0f);
            }
            break;

            case rf_pixel_format_r5g6b5:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                unsigned char r = (unsigned char)(round(normalized.x * 31.0f));
                unsigned char g = (unsigned char)(round(normalized.y * 63.0f));
                unsigned char b = (unsigned char)(round(normalized.z * 31.0f));

                ((unsigned short*)dst)[0] = (unsigned short)r << 11 | (unsigned short)g << 5 | (unsigned short)b;
            }
            break;

            case rf_pixel_format_r8g8b8:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((unsigned char*)dst)[0    ] = (unsigned char)(normalized.x * 255.0f);
                ((unsigned char*)dst)[0 + 1] = (unsigned char)(normalized.y * 255.0f);
                ((unsigned char*)dst)[0 + 2] = (unsigned char)(normalized.z * 255.0f);
            }
            break;

            case rf_pixel_format_r5g5b5a1:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                int ALPHA_THRESHOLD = 50;
                unsigned char r = (unsigned char)(round(normalized.x * 31.0f));
                unsigned char g = (unsigned char)(round(normalized.y * 31.0f));
                unsigned char b = (unsigned char)(round(normalized.z * 31.0f));
                unsigned char a = (normalized.w > ((float)ALPHA_THRESHOLD / 255.0f)) ? 1 : 0;

                ((unsigned short*)dst)[0] = (unsigned short)r << 11 | (unsigned short)g << 6 | (unsigned short)b << 1 | (unsigned short)a;
            }
            break;

            case rf_pixel_format_r4g4b4a4:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                unsigned char r = (unsigned char)(round(normalized.x * 15.0f));
                unsigned char g = (unsigned char)(round(normalized.y * 15.0f));
                unsigned char b = (unsigned char)(round(normalized.z * 15.0f));
                unsigned char a = (unsigned char)(round(normalized.w * 15.0f));

                ((unsigned short*)dst)[0] = (unsigned short)r << 12 | (unsigned short)g << 8 | (unsigned short)b << 4 | (unsigned short)a;
            }
            break;

            case rf_pixel_format_r8g8b8a8:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((unsigned char*)dst)[0    ] = (unsigned char)(normalized.x * 255.0f);
                ((unsigned char*)dst)[0 + 1] = (unsigned char)(normalized.y * 255.0f);
                ((unsigned char*)dst)[0 + 2] = (unsigned char)(normalized.z * 255.0f);
                ((unsigned char*)dst)[0 + 3] = (unsigned char)(normalized.w * 255.0f);
            }
            break;

            case rf_pixel_format_r32:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((float*)dst)[0] = (float)(normalized.x * 0.299f + normalized.y * 0.587f + normalized.z * 0.114f);
            }
            break;

            case rf_pixel_format_r32g32b32:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((float*)dst)[0    ] = normalized.x;
                ((float*)dst)[0 + 1] = normalized.y;
                ((float*)dst)[0 + 2] = normalized.z;
            }
            break;

            case rf_pixel_format_r32g32b32a32:
            {
                rf_vec4 normalized = rf_format_one_pixel_to_normalized(src, src_format);
                ((float*)dst)[0    ] = normalized.x;
                ((float*)dst)[0 + 1] = normalized.y;
                ((float*)dst)[0 + 2] = normalized.z;
                ((float*)dst)[0 + 3] = normalized.w;
            }
            break;

            default: break;
        }
    }
}

#pragma endregion

#pragma region color

// Returns 1 if the two colors have the same values for the rgb components
rf_public rf_bool rf_color_match_rgb(rf_color a, rf_color b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

// Returns 1 if the two colors have the same values
rf_public rf_bool rf_color_match(rf_color a, rf_color b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

// Returns hexadecimal value for a rf_color
rf_public int rf_color_to_int(rf_color color)
{
    return (((int) color.r << 24) | ((int) color.g << 16) | ((int) color.b << 8) | (int) color.a);
}

// Returns color normalized as float [0..1]
rf_public rf_vec4 rf_color_normalize(rf_color color)
{
    rf_vec4 result;

    result.x = (float) color.r / 255.0f;
    result.y = (float) color.g / 255.0f;
    result.z = (float) color.b / 255.0f;
    result.w = (float) color.a / 255.0f;

    return result;
}

// Returns color from normalized values [0..1]
rf_public rf_color rf_color_from_normalized(rf_vec4 normalized)
{
    rf_color result;

    result.r = normalized.x * 255.0f;
    result.g = normalized.y * 255.0f;
    result.b = normalized.z * 255.0f;
    result.a = normalized.w * 255.0f;

    return result;
}

// Returns HSV values for a rf_color. Hue is returned as degrees [0..360]
rf_public rf_vec3 rf_color_to_hsv(rf_color color)
{
    rf_vec3 rgb = { (float) color.r / 255.0f, (float) color.g / 255.0f, (float) color.b / 255.0f };
    rf_vec3 hsv = { 0.0f, 0.0f, 0.0f };
    float min, max, delta;

    min = rgb.x < rgb.y ? rgb.x : rgb.y;
    min = min < rgb.z ? min : rgb.z;

    max = rgb.x > rgb.y ? rgb.x : rgb.y;
    max = max > rgb.z ? max : rgb.z;

    hsv.z = max; // Value
    delta = max - min;

    if (delta < 0.00001f)
    {
        hsv.y = 0.0f;
        hsv.x = 0.0f; // Undefined, maybe NAN?
        return hsv;
    }

    if (max > 0.0f)
    {
        // NOTE: If max is 0, this divide would cause a crash
        hsv.y = (delta / max); // Saturation
    }
    else
    {
        // NOTE: If max is 0, then r = g = b = 0, s = 0, h is undefined
        hsv.y = 0.0f;
        hsv.x = NAN; // Undefined
        return hsv;
    }

    // NOTE: Comparing float values could not work properly
    if (rgb.x >= max) hsv.x = (rgb.y - rgb.z) / delta; // Between yellow & magenta
    else
    {
        if (rgb.y >= max) hsv.x = 2.0f + (rgb.z - rgb.x) / delta; // Between cyan & yellow
        else hsv.x = 4.0f + (rgb.x - rgb.y) / delta; // Between magenta & cyan
    }

    hsv.x *= 60.0f; // Convert to degrees

    if (hsv.x < 0.0f) hsv.x += 360.0f;

    return hsv;
}

// Returns a rf_color from HSV values. rf_color->HSV->rf_color conversion will not yield exactly the same color due to rounding errors. Implementation reference: https://en.wikipedia.org/wiki/HSL_and_HSV#Alternative_HSV_conversion
rf_public rf_color rf_color_from_hsv(rf_vec3 hsv)
{
    rf_color color = {0, 0, 0, 255};
    float h = hsv.x, s = hsv.y, v = hsv.z;

// Red channel
    float k = fmod((5.0f + h / 60.0f), 6);
    float t = 4.0f - k;
    k = (t < k) ? t : k;
    k = (k < 1) ? k : 1;
    k = (k > 0) ? k : 0;
    color.r = (v - v * s * k) * 255;

// Green channel
    k = fmod((3.0f + h / 60.0f), 6);
    t = 4.0f - k;
    k = (t < k) ? t : k;
    k = (k < 1) ? k : 1;
    k = (k > 0) ? k : 0;
    color.g = (v - v * s * k) * 255;

// Blue channel
    k = fmod((1.0f + h / 60.0f), 6);
    t = 4.0f - k;
    k = (t < k) ? t : k;
    k = (k < 1) ? k : 1;
    k = (k > 0) ? k : 0;
    color.b = (v - v * s * k) * 255;

    return color;
}

// Returns a rf_color struct from hexadecimal value
rf_public rf_color rf_color_from_int(int hex_value)
{
    rf_color color;

    color.r = (unsigned char) (hex_value >> 24) & 0xFF;
    color.g = (unsigned char) (hex_value >> 16) & 0xFF;
    color.b = (unsigned char) (hex_value >> 8) & 0xFF;
    color.a = (unsigned char) hex_value & 0xFF;

    return color;
}

// rf_color fade-in or fade-out, alpha goes from 0.0f to 1.0f
rf_public rf_color rf_fade(rf_color color, float alpha)
{
    if (alpha < 0.0f) alpha = 0.0f;
    else if (alpha > 1.0f) alpha = 1.0f;

    return (rf_color) {color.r, color.g, color.b, (unsigned char) (255.0f * alpha)};
}

#pragma endregion


#ifndef RAYFORK_GFX_INTERNAL_STRING_UTILS_H
#define RAYFORK_GFX_INTERNAL_STRING_UTILS_H

#include "string.h"

#ifndef RF_MAX_FILEPATH_LEN
    #define RF_MAX_FILEPATH_LEN (1024)
#endif

rf_internal inline rf_bool rf_match_str_n(const char* a, int a_len, const char* b, int b_len)
{
    return a_len == b_len && strncmp(a, b, a_len) == 0;
}

rf_internal inline rf_bool rf_match_str_cstr(const char* a, int a_len, const char* b)
{
    return rf_match_str_n(a, a_len, b, strlen(b));
}

rf_internal inline rf_bool rf_is_file_extension(const char* filename, const char* ext)
{
    int filename_len = strlen(filename);
    int ext_len      = strlen(ext);

    if (filename_len < ext_len)
    {
        return 0;
    }

    return rf_match_str_n(filename + filename_len - ext_len, ext_len, ext, ext_len);
}

rf_internal inline const char* rf__str_find_last(const char* s, const char* charset)
{
    const char* latest_match = NULL;
    for (; s = strpbrk(s, charset), s != NULL; latest_match = s++) { }
    return latest_match;
}

rf_internal inline const char* rf_get_directory_path_from_file_path(const char* file_path)
{
    static rf_thread_local char rf_global_dir_path[RF_MAX_FILEPATH_LEN];

    const char* last_slash = NULL;
    memset(rf_global_dir_path, 0, RF_MAX_FILEPATH_LEN);

    last_slash = rf__str_find_last(file_path, "\\/");
    if (!last_slash) { return NULL; }

    // NOTE: Be careful, strncpy() is not safe, it does not care about '\0'
    strncpy(rf_global_dir_path, file_path, strlen(file_path) - (strlen(last_slash) - 1));
    rf_global_dir_path[strlen(file_path) - strlen(last_slash)] = '\0'; // Add '\0' manually

    return rf_global_dir_path;
}

#endif // RAYFORK_GFX_INTERNAL_STRING_UTILS_H

#pragma region dependencies

#pragma region stb_image
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC(sz)                     rf_alloc(rf__global_allocator_for_dependencies, sz)
#define STBI_FREE(p)                        rf_free(rf__global_allocator_for_dependencies, p)
#define STBI_REALLOC_SIZED(p, oldsz, newsz) rf_default_realloc(rf__global_allocator_for_dependencies, p, oldsz, newsz)
#define STBI_ASSERT(it)                     rf_assert(it)
#define STBIDEF                             rf_internal
#include "stb_image.h"
#pragma endregion

#pragma region stb_image_resize
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STBIR_MALLOC(sz,c)   ((void)(c), rf_alloc(rf__global_allocator_for_dependencies, sz))
#define STBIR_FREE(p,c)      ((void)(c), rf_free(rf__global_allocator_for_dependencies, p))
#define STBIR_ASSERT(it)     rf_assert(it)
#define STBIRDEF rf_internal
#include "stb_image_resize.h"
#pragma endregion

#pragma region stb_rect_pack
#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_ASSERT RF_ASSERT
#define STBRP_STATIC
#include "stb_rect_pack.h"
#pragma endregion

#pragma region stb_perlin
#define STB_PERLIN_IMPLEMENTATION
#define STBPDEF rf_internal
#include "stb_perlin.h"
#pragma endregion

#pragma endregion

#pragma region extract image data functions

rf_public int rf_image_size(rf_image image)
{
    return rf_pixel_buffer_size(image.width, image.height, image.format);
}

rf_public int rf_image_size_in_format(rf_image image, rf_pixel_format format)
{
    return image.width * image.height * rf_bytes_per_pixel(format);
}

rf_public rf_bool rf_image_get_pixels_as_rgba32_to_buffer(rf_image image, rf_color* dst, rf_int dst_size)
{
    rf_bool success = 0;

    if (rf_is_uncompressed_format(image.format))
    {
        if (image.format == rf_pixel_format_r32 || image.format == rf_pixel_format_r32g32b32 || image.format == rf_pixel_format_r32g32b32a32)
        {
            rf_log(rf_log_type_warning, "32bit pixel format converted to 8bit per channel.");
        }

        success = rf_format_pixels_to_rgba32(image.data, rf_image_size(image), image.format, dst, dst_size);
    }
    else rf_log_error(rf_bad_argument, "Function only works for uncompressed formats but was called with format %d.", image.format);

    return success;
}

rf_public rf_bool rf_image_get_pixels_as_normalized_to_buffer(rf_image image, rf_vec4* dst, rf_int dst_size)
{
    rf_bool success = 0;

    if (rf_is_uncompressed_format(image.format))
    {
        if ((image.format == rf_pixel_format_r32) || (image.format == rf_pixel_format_r32g32b32) || (image.format == rf_pixel_format_r32g32b32a32))
        {
            rf_log(rf_log_type_warning, "32bit pixel format converted to 8bit per channel");
        }

        success = rf_format_pixels_to_normalized(image.data, rf_image_size(image), rf_pixel_format_r32g32b32a32, dst, dst_size);
    }
    else rf_log_error(rf_bad_argument, "Function only works for uncompressed formats but was called with format %d.", image.format);

    return success;
}

// Get pixel data from image in the form of rf_color struct array
rf_public rf_color* rf_image_pixels_to_rgba32(rf_image image, rf_allocator allocator)
{
    rf_color* result = NULL;

    if (rf_is_uncompressed_format(image.format))
    {
        int size = image.width * image.height * sizeof(rf_color);
        result = rf_alloc(allocator, size);

        if (result)
        {
            rf_bool success = rf_image_get_pixels_as_rgba32_to_buffer(image, result, size);
            rf_assert(success);
        }
        else rf_log_error(rf_bad_alloc, "Allocation of size %d failed.", size);
    }
    else rf_log_error(rf_bad_argument, "Function only works for uncompressed formats but was called with format %d.", image.format);

    return result;
}

// Get pixel data from image as rf_vec4 array (float normalized)
rf_public rf_vec4* rf_image_compute_pixels_to_normalized(rf_image image, rf_allocator allocator)
{
    rf_vec4* result = NULL;

    if (rf_is_compressed_format(image.format))
    {
        int size = image.width * image.height * sizeof(rf_color);
        result = rf_alloc(allocator, size);

        if (result)
        {
            rf_bool success = rf_image_get_pixels_as_normalized_to_buffer(image, result, size);
            rf_assert(success);
        }
        else rf_log_error(rf_bad_alloc, "Allocation of size %d failed.", size);
    }
    else rf_log_error(rf_bad_argument, "Function only works for uncompressed formats but was called with format %d.", image.format);

    return result;
}

// Extract color palette from image to maximum size
rf_public void rf_image_extract_palette_to_buffer(rf_image image, rf_color* palette_dst, rf_int palette_size)
{
    if (rf_is_uncompressed_format(image.format))
    {
        if (palette_size > 0)
        {
            int img_size = rf_image_size(image);
            int img_bpp  = rf_bytes_per_pixel(image.format);
            const unsigned char* img_data = (unsigned char*) image.data;

            for (rf_int img_iter = 0, palette_iter = 0; img_iter < img_size && palette_iter < palette_size; img_iter += img_bpp)
            {
                rf_color color = rf_format_one_pixel_to_rgba32(img_data, image.format);

                rf_bool color_found = 0;

                for (rf_int i = 0; i < palette_iter; i++)
                {
                    if (rf_color_match(palette_dst[i], color))
                    {
                        color_found = 1;
                        break;
                    }
                }

                if (!color_found)
                {
                    palette_dst[palette_iter] = color;
                    palette_iter++;
                }
            }
        }
        else rf_log(rf_log_type_warning, "Palette size was 0.");
    }
    else rf_log_error(rf_bad_argument, "Function only works for uncompressed formats but was called with format %d.", image.format);
}

rf_public rf_palette rf_image_extract_palette(rf_image image, rf_int palette_size, rf_allocator allocator)
{
    rf_palette result = {0};

    if (rf_is_uncompressed_format(image.format))
    {
        rf_color* dst = rf_alloc(allocator, sizeof(rf_color) * palette_size);
        rf_image_extract_palette_to_buffer(image, dst, palette_size);

        result.colors = dst;
        result.count = palette_size;
    }

    return result;
}

// Get image alpha border rectangle
rf_public rf_rec rf_image_alpha_border(rf_image image, float threshold)
{
    rf_rec crop = {0};

    if (rf_is_uncompressed_format(image.format))
    {
        int x_min = 65536; // Define a big enough number
        int x_max = 0;
        int y_min = 65536;
        int y_max = 0;

        int src_bpp = rf_bytes_per_pixel(image.format);
        int src_size = rf_image_size(image);
        unsigned char* src = image.data;

        for (rf_int y = 0; y < image.height; y++)
        {
            for (rf_int x = 0; x < image.width; x++)
            {
                int src_pos = (y * image.width + x) * src_bpp;

                rf_color rgba32_pixel = rf_format_one_pixel_to_rgba32(&src[src_pos], image.format);

                if (rgba32_pixel.a > (unsigned char)(threshold * 255.0f))
                {
                    if (x < x_min) x_min = x;
                    if (x > x_max) x_max = x;
                    if (y < y_min) y_min = y;
                    if (y > y_max) y_max = y;
                }
            }
        }

        crop = (rf_rec) { x_min, y_min, (x_max + 1) - x_min, (y_max + 1) - y_min };
    }
    else rf_log_error(rf_bad_argument, "Function only works for uncompressed formats but was called with format %d.", image.format);

    return crop;
}

#pragma endregion

#pragma region loading and unloading functions

rf_public rf_bool rf_supports_image_file_type(const char* filename)
{
    return       rf_is_file_extension(filename, ".png")
              || rf_is_file_extension(filename, ".bmp")
              || rf_is_file_extension(filename, ".tga")
              || rf_is_file_extension(filename, ".pic")
              || rf_is_file_extension(filename, ".psd")
              || rf_is_file_extension(filename, ".hdr");
}

rf_public rf_image rf_load_image_from_file_data_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size, rf_desired_channels channels, rf_allocator temp_allocator)
{
    if (src == NULL || src_size <= 0) { rf_log_error(rf_bad_argument, "Argument `image` was invalid."); return (rf_image){0}; }

    rf_image result = {0};

    int img_width  = 0;
    int img_height = 0;
    int img_bpp    = 0;

    rf_set_global_dependencies_allocator(temp_allocator);
    void* output_buffer = stbi_load_from_memory(src, src_size, &img_width, &img_height, &img_bpp, channels);
    rf_set_global_dependencies_allocator((rf_allocator) {0});

    if (output_buffer)
    {
        int output_buffer_size = img_width * img_height * img_bpp;

        if (dst_size >= output_buffer_size)
        {
            result.data   = dst;
            result.width  = img_width;
            result.height = img_height;
            result.valid  = 1;

            switch (img_bpp)
            {
                case 1: result.format = rf_pixel_format_grayscale; break;
                case 2: result.format = rf_pixel_format_gray_alpha; break;
                case 3: result.format = rf_pixel_format_r8g8b8; break;
                case 4: result.format = rf_pixel_format_r8g8b8a8; break;
                default: break;
            }

            memcpy(dst, output_buffer, output_buffer_size);
        }
        else rf_log_error(rf_bad_argument, "Buffer is not big enough", img_width, img_height, img_bpp);

        rf_free(temp_allocator, output_buffer);
    }
    else rf_log_error(rf_stbi_failed, "File format not supported or could not be loaded. STB Image returned { x: %d, y: %d, channels: %d }", img_width, img_height, img_bpp);

    return result;
}

rf_public rf_image rf_load_image_from_file_data(const void* src, rf_int src_size, rf_desired_channels desired_channels, rf_allocator allocator, rf_allocator temp_allocator)
{
    // Preconditions
    if (!src || src_size <= 0)
    {
        rf_log_error(rf_bad_argument, "Argument `src` was null.");
        return (rf_image) {0};
    }

    // Compute the result
    rf_image result = {0};

    // Use stb image with the `temp_allocator` to decompress the image and get it's data
    int width = 0, height = 0, channels = 0;
    rf_set_global_dependencies_allocator(temp_allocator);
    void* stbi_result = stbi_load_from_memory(src, src_size, &width, &height, &channels, desired_channels);
    rf_set_global_dependencies_allocator((rf_allocator) {0});

    if (stbi_result && channels)
    {
        // Allocate a result buffer using the `allocator` and copy the data to it
        int stbi_result_size = width * height * channels;
        void* result_buffer = rf_alloc(allocator, stbi_result_size);

        if (result_buffer)
        {
            result.data   = result_buffer;
            result.width  = width;
            result.height = height;
            result.valid  = 1;

            // Set the format appropriately depending on the `channels` count
            switch (channels)
            {
                case 1: result.format = rf_pixel_format_grayscale; break;
                case 2: result.format = rf_pixel_format_gray_alpha; break;
                case 3: result.format = rf_pixel_format_r8g8b8; break;
                case 4: result.format = rf_pixel_format_r8g8b8a8; break;
                default: break;
            }

            memcpy(result_buffer, stbi_result, stbi_result_size);
        }
        else rf_log_error(rf_bad_alloc, "Buffer is not big enough", width, height, channels);

        // Free the temp buffer allocated by stbi
        rf_free(temp_allocator, stbi_result);
    }
    else rf_log_error(rf_stbi_failed, "File format not supported or could not be loaded. STB Image returned { x: %d, y: %d, channels: %d }", width, height, channels);

    return result;
}

rf_public rf_image rf_load_image_from_hdr_file_data_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size, rf_desired_channels channels, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (src && src_size > 0)
    {
        int img_width = 0, img_height = 0, img_bpp = 0;

        // NOTE: Using stb_image to load images (Supports multiple image formats)
        rf_set_global_dependencies_allocator(temp_allocator);
        void* output_buffer = stbi_load_from_memory(src, src_size, &img_width, &img_height, &img_bpp, channels);
        rf_set_global_dependencies_allocator((rf_allocator) {0});

        if (output_buffer)
        {
            int output_buffer_size = img_width * img_height * img_bpp;

            if (dst_size >= output_buffer_size)
            {
                result.data   = dst;
                result.width  = img_width;
                result.height = img_height;
                result.valid  = 1;

                     if (img_bpp == 1) result.format = rf_pixel_format_r32;
                else if (img_bpp == 3) result.format = rf_pixel_format_r32g32b32;
                else if (img_bpp == 4) result.format = rf_pixel_format_r32g32b32a32;

                memcpy(dst, output_buffer, output_buffer_size);
            }
            else rf_log_error(rf_bad_argument, "Buffer is not big enough", img_width, img_height, img_bpp);

            rf_free(temp_allocator, output_buffer);
        }
        else rf_log_error(rf_stbi_failed, "File format not supported or could not be loaded. STB Image returned { x: %d, y: %d, channels: %d }", img_width, img_height, img_bpp);
    }
    else rf_log_error(rf_bad_argument, "Argument `image` was invalid.");

    return result;
}

rf_public rf_image rf_load_image_from_hdr_file_data(const void* src, rf_int src_size, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (src && src_size)
    {
        int width = 0, height = 0, bpp = 0;

        // NOTE: Using stb_image to load images (Supports multiple image formats)
        rf_set_global_dependencies_allocator(temp_allocator);
        void* stbi_result = stbi_load_from_memory(src, src_size, &width, &height, &bpp, RF_ANY_CHANNELS);
        rf_set_global_dependencies_allocator((rf_allocator) {0});

        if (stbi_result && bpp)
        {
            int stbi_result_size = width * height * bpp;
            void* result_buffer = rf_alloc(allocator, stbi_result_size);

            if (result_buffer)
            {
                result.data   = result_buffer;
                result.width  = width;
                result.height = height;
                result.valid  = 1;

                     if (bpp == 1) result.format = rf_pixel_format_r32;
                else if (bpp == 3) result.format = rf_pixel_format_r32g32b32;
                else if (bpp == 4) result.format = rf_pixel_format_r32g32b32a32;

                memcpy(result_buffer, stbi_result, stbi_result_size);
            }
            else rf_log_error(rf_bad_alloc, "Buffer is not big enough", width, height, bpp);

            rf_free(temp_allocator, stbi_result);
        }
        else rf_log_error(rf_stbi_failed, "File format not supported or could not be loaded. STB Image returned { x: %d, y: %d, channels: %d }", width, height, bpp);
    }
    else rf_log_error(rf_bad_argument, "Argument `image` was invalid.");

    return result;
}

rf_public rf_image rf_load_image_from_format_to_buffer(const void* src, rf_int src_size, int src_width, int src_height, rf_uncompressed_pixel_format src_format, void* dst, rf_int dst_size, rf_uncompressed_pixel_format dst_format)
{
    rf_image result = {0};

    if (rf_is_uncompressed_format(dst_format))
    {
        result = (rf_image) {
            .data   = dst,
            .width  = src_width,
            .height = src_height,
            .format = dst_format,
        };

        rf_bool success = rf_format_pixels(src, src_size, src_format, dst, dst_size, dst_format);
        rf_assert(success);
    }

    return result;
}

rf_public rf_image rf_load_image_from_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_image image = {0};

    if (rf_supports_image_file_type(filename))
    {
        int file_size = rf_file_size(io, filename);

        if (file_size > 0)
        {
            unsigned char* image_file_buffer = rf_alloc(temp_allocator, file_size);

            if (image_file_buffer)
            {
                if (rf_read_file(io, filename, image_file_buffer, file_size))
                {
                    if (rf_is_file_extension(filename, ".hdr"))
                    {
                        image = rf_load_image_from_hdr_file_data(image_file_buffer, file_size, allocator, temp_allocator);
                    }
                    else
                    {
                        image = rf_load_image_from_file_data(image_file_buffer, file_size, RF_ANY_CHANNELS, allocator, temp_allocator);
                    }
                }
                else rf_log_error(rf_bad_io, "File size for %s is 0", filename);

                rf_free(temp_allocator, image_file_buffer);
            }
            else rf_log_error(rf_bad_alloc, "Temporary allocation of size %d failed", file_size);
        }
        else rf_log_error(rf_bad_io, "File size for %s is 0", filename);
    }
    else rf_log_error(rf_unsupported, "Image fileformat not supported", filename);

    return image;
}

rf_public void rf_unload_image(rf_image image, rf_allocator allocator)
{
    rf_free(allocator, image.data);
}

#pragma endregion

#pragma region image manipulation

/**
 * Copy an existing image into a buffer.
 * @param image a valid image to copy from.
 * @param dst a buffer for the resulting image.
 * @param dst_size size of the `dst` buffer.
 * @return a deep copy of the image into the provided `dst` buffer.
 */
rf_public rf_image rf_image_copy_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        int width  = image.width;
        int height = image.height;
        int size   = width * height * rf_bytes_per_pixel(image.format);

        if (dst_size >= size)
        {
            memcpy(dst, image.data, size);

            result.data    = dst;
            result.width   = image.width;
            result.height  = image.height;
            result.format  = image.format;
            result.valid   = 1;
        }
        else rf_log_error(rf_bad_argument, "Destination buffer is too small. Expected at least %d bytes but was %d", size, dst_size);
    }
    else rf_log_error(rf_bad_argument, "Image was invalid.");

    return result;
}

/**
 * Copy an existing image.
 * @param image a valid image to copy from.
 * @param allocator used to allocate the new buffer for the resulting image.
 * @return a deep copy of the image.
 */
rf_public rf_image rf_image_copy(rf_image image, rf_allocator allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        int size  = image.width * image.height * rf_bytes_per_pixel(image.format);
        void* dst = rf_alloc(allocator, size);

        if (dst)
        {
            result = rf_image_copy_to_buffer(image, dst, size);
        }
        else rf_log_error(rf_bad_alloc, "Failed to allocate %d bytes", size);
    }
    else rf_log_error(rf_bad_argument, "Image was invalid.");

    return result;
}

/**
 * Crop an image and store the result in a provided buffer.
 * @param image a valid image that we crop from.
 * @param crop a rectangle representing which part of the image to crop.
 * @param dst a buffer for the resulting image. Must be of size at least `rf_pixel_buffer_size(RF_UNCOMPRESSED_R8G8B8A8, crop.width, crop.height)`.
 * @param dst_size size of the `dst` buffer.
 * @return a cropped image using the `dst` buffer in the same format as `image`.
 */
rf_public rf_image rf_image_crop_to_buffer(rf_image image, rf_rec crop, void* dst, rf_int dst_size, rf_uncompressed_pixel_format dst_format)
{
    rf_image result = {0};

    if (image.valid)
    {
        // Security checks to validate crop rectangle
        if (crop.x < 0) { crop.width += crop.x; crop.x = 0; }
        if (crop.y < 0) { crop.height += crop.y; crop.y = 0; }
        if ((crop.x + crop.width) > image.width) { crop.width = image.width - crop.x; }
        if ((crop.y + crop.height) > image.height) { crop.height = image.height - crop.y; }

        if ((crop.x < image.width) && (crop.y < image.height))
        {
            int expected_size = rf_pixel_buffer_size(crop.width, crop.height, dst_format);
            if (dst_size >= expected_size)
            {
                rf_pixel_format src_format = image.format;
                int src_size = rf_image_size(image);

                unsigned char* src_ptr = image.data;
                unsigned char* dst_ptr = dst;

                int src_bpp = rf_bytes_per_pixel(image.format);
                int dst_bpp = rf_bytes_per_pixel(dst_format);

                int crop_y = crop.y;
                int crop_h = crop.height;
                int crop_x = crop.x;
                int crop_w = crop.width;

                for (rf_int y = 0; y < crop_h; y++)
                {
                    for (rf_int x = 0; x < crop_w; x++)
                    {
                        int src_x = x + crop_x;
                        int src_y = y + crop_y;

                        int src_pixel = (src_y * image.width + src_x) * src_bpp;
                        int dst_pixel = (y * crop_w + x) * src_bpp;
                        rf_assert(src_pixel < src_size);
                        rf_assert(dst_pixel < dst_size);

                        rf_format_one_pixel(&src_ptr[src_pixel], src_format, &dst_ptr[dst_pixel], dst_format);
                    }
                }

                result.data   = dst;
                result.format = dst_format;
                result.width  = crop.width;
                result.height = crop.height;
                result.valid  = 1;
            }
            else rf_log_error(rf_bad_argument, "Destination buffer is too small. Expected at least %d bytes but was %d", expected_size, dst_size);
        }
        else rf_log_error(rf_bad_argument, "Image can not be cropped, crop rectangle out of bounds.");
    }
    else rf_log_error(rf_bad_argument, "Image is invalid.");

    return result;
}

/**
 * Crop an image and store the result in a provided buffer.
 * @param image a valid image that we crop from.
 * @param crop a rectangle representing which part of the image to crop.
 * @param dst a buffer for the resulting image. Must be of size at least `rf_pixel_buffer_size(RF_UNCOMPRESSED_R8G8B8A8, crop.width, crop.height)`.
 * @param dst_size size of the `dst` buffer.
 * @return a cropped image using the `dst` buffer in the same format as `image`.
 */
rf_public rf_image rf_image_crop(rf_image image, rf_rec crop, rf_allocator allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        int size = rf_pixel_buffer_size(crop.width, crop.height, image.format);
        void* dst = rf_alloc(allocator, size);

        if (dst)
        {
            result = rf_image_crop_to_buffer(image, crop, dst, size, image.format);
        }
        else rf_log_error(rf_bad_alloc, "Allocation of size %d failed.", size);
    }
    else rf_log_error(rf_bad_argument, "Image is invalid.");

    return result;
}

rf_internal int rf_format_to_stb_channel_count(rf_pixel_format format)
{
    switch (format)
    {
        case rf_pixel_format_grayscale: return 1;
        case rf_pixel_format_gray_alpha: return 2;
        case rf_pixel_format_r8g8b8: return 3;
        case rf_pixel_format_r8g8b8a8: return 4;
        default: return 0;
    }
}

// Resize and image to new size.
// Note: Uses stb default scaling filters (both bicubic): STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_CATMULLROM STBIR_DEFAULT_FILTER_DOWNSAMPLE STBIR_FILTER_MITCHELL (high-quality Catmull-Rom)
rf_public rf_image rf_image_resize_to_buffer(rf_image image, int new_width, int new_height, void* dst, rf_int dst_size, rf_allocator temp_allocator)
{
    if (!image.valid || dst_size < new_width * new_height * rf_bytes_per_pixel(image.format)) return (rf_image){0};

    rf_image result = {0};

    int stb_format = rf_format_to_stb_channel_count(image.format);

    if (stb_format)
    {
        rf_set_global_dependencies_allocator(temp_allocator);
        stbir_resize_uint8((unsigned char*) image.data, image.width, image.height, 0, (unsigned char*) dst, new_width, new_height, 0, stb_format);
        rf_set_global_dependencies_allocator((rf_allocator) {0});

        result.data   = dst;
        result.width  = new_width;
        result.height = new_height;
        result.format = image.format;
        result.valid  = 1;
    }
    else // if the format of the image is not supported by stbir
    {
        int pixels_size = image.width * image.height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
        rf_color* pixels = rf_alloc(temp_allocator, pixels_size);

        if (pixels)
        {
            rf_bool format_success = rf_format_pixels_to_rgba32(image.data, rf_image_size(image), image.format, pixels, pixels_size);
            rf_assert(format_success);

            rf_set_global_dependencies_allocator(temp_allocator);
            stbir_resize_uint8((unsigned char*)pixels, image.width, image.height, 0, (unsigned char*) dst, new_width, new_height, 0, 4);
            rf_set_global_dependencies_allocator((rf_allocator) {0});

            format_success = rf_format_pixels(pixels, pixels_size, rf_pixel_format_r8g8b8a8, dst, dst_size, image.format);
            rf_assert(format_success);

            result.data   = dst;
            result.width  = new_width;
            result.height = new_height;
            result.format = image.format;
            result.valid  = 1;
        }
        else rf_log_error(rf_bad_alloc, "Allocation of size %d failed.", image.width * image.height * sizeof(rf_color));

        rf_free(temp_allocator, pixels);
    }

    return result;
}

rf_public rf_image rf_image_resize(rf_image image, int new_width, int new_height, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        int dst_size = new_width * new_height * rf_bytes_per_pixel(image.format);
        void* dst = rf_alloc(allocator, dst_size);

        if (dst)
        {
            result = rf_image_resize_to_buffer(image, new_width, new_height, dst, dst_size, temp_allocator);
        }
        else rf_log_error(rf_bad_alloc, "Allocation of size %d failed.", dst_size);
    }
    else rf_log_error(rf_bad_argument, "Image is invalid.");

    return result;
}

/**
 * Resize and image to new size using Nearest-Neighbor scaling algorithm
 * @param image
 * @param new_width
 * @param new_height
 * @param dst
 * @param dst_size
 * @return a resized version of the `image`, with the `dst` buffer, in the same format.
 */
rf_public rf_image rf_image_resize_nn_to_buffer(rf_image image, int new_width, int new_height, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        int bpp = rf_bytes_per_pixel(image.format);
        int expected_size = new_width * new_height * bpp;

        if (dst_size >= expected_size)
        {
            // EDIT: added +1 to account for an early rounding problem
            int x_ratio = (int)((image.width  << 16) / new_width ) + 1;
            int y_ratio = (int)((image.height << 16) / new_height) + 1;

            unsigned char* src = image.data;

            int x2, y2;
            for (rf_int y = 0; y < new_height; y++)
            {
                for (rf_int x = 0; x < new_width; x++)
                {
                    x2 = ((x * x_ratio) >> 16);
                    y2 = ((y * y_ratio) >> 16);

                    rf_format_one_pixel(src + ((y2 * image.width) + x2) * bpp, image.format,
                                        ((unsigned char *) dst) + ((y * new_width) + x) * bpp, image.format);
                }
            }

            result.data   = dst;
            result.width  = new_width;
            result.height = new_height;
            result.format = image.format;
            result.valid  = 1;
        }
        else rf_log_error(rf_bad_buffer_size, "Expected `dst` to be at least %d bytes but was %d bytes", expected_size, dst_size);
    }
    else rf_log_error(rf_bad_argument, "Image is invalid.");

    return result;
}

rf_public rf_image rf_image_resize_nn(rf_image image, int new_width, int new_height, rf_allocator allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        int dst_size = new_width * new_height * rf_bytes_per_pixel(image.format);
        void* dst = rf_alloc(allocator, dst_size);

        if (dst)
        {
            result = rf_image_resize_nn_to_buffer(image, new_width, new_height, dst, dst_size);
        }
        else rf_log_error(rf_bad_alloc, "Allocation of size %d failed.", dst_size);
    }
    else rf_log_error(rf_bad_argument, "Image is invalid.");

    return result;
}

// Convert image data to desired format
rf_public rf_image rf_image_format_to_buffer(rf_image image, rf_uncompressed_pixel_format dst_format, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (rf_is_uncompressed_format(dst_format) && rf_is_uncompressed_format(image.format))
        {
            rf_bool success = rf_format_pixels(image.data, rf_image_size(image), image.format, dst, dst_size, dst_format);
            rf_assert(success);

            result = (rf_image)
            {
                .data = dst,
                .width = image.width,
                .height = image.height,
                .format = dst_format,
                .valid = 1,
            };
        }
        else rf_log_error(rf_bad_argument, "Cannot format compressed pixel formats. Image format: %d, Destination format: %d.", image.format, dst_format);
    }
    else rf_log_error(rf_bad_argument, "Image is invalid.");

    return result;
}

rf_public rf_image rf_image_format(rf_image image, rf_uncompressed_pixel_format new_format, rf_allocator allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (rf_is_uncompressed_format(new_format) && rf_is_uncompressed_format(image.format))
        {
            int dst_size = image.width * image.height * rf_bytes_per_pixel(image.format);
            void* dst = rf_alloc(allocator, dst_size);

            if (dst)
            {
                rf_bool format_success = rf_format_pixels(image.data, rf_image_size(image), image.format, dst, dst_size, new_format);
                rf_assert(format_success);

                result = (rf_image)
                {
                    .data = dst,
                    .width = image.width,
                    .height = image.height,
                    .format = new_format,
                    .valid = 1,
                };
            }
            else rf_log_error(rf_bad_alloc, "Allocation of size %d failed.", dst_size);
        }
        else rf_log_error(rf_bad_argument, "Cannot format compressed pixel formats. `image.format`: %d, `dst_format`: %d", image.format, new_format);
    }
    else rf_log_error(rf_bad_argument, "Image is invalid.");

    return result;
}

// Apply alpha mask to image. Note 1: Returned image is GRAY_ALPHA (16bit) or RGBA (32bit). Note 2: alphaMask should be same size as image
rf_public rf_image rf_image_alpha_mask_to_buffer(rf_image image, rf_image alpha_mask, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid && alpha_mask.valid)
    {
        if (image.width == alpha_mask.width && image.height == alpha_mask.height)
        {
            if (rf_is_compressed_format(image.format) && alpha_mask.format == rf_pixel_format_grayscale)
            {
                if (dst_size >= rf_image_size_in_format(image, rf_pixel_format_gray_alpha))
                {
                    // Apply alpha mask to alpha channel
                    for (rf_int i = 0; i < image.width * image.height; i++)
                    {
                        unsigned char mask_pixel = 0;
                        rf_format_one_pixel(((unsigned char*)alpha_mask.data) + i, alpha_mask.format, &mask_pixel, rf_pixel_format_grayscale);

                        // Todo: Finish implementing this function
                        //((unsigned char*)dst)[k] = mask_pixel;
                    }

                    result.data   = dst;
                    result.width  = image.width;
                    result.height = image.height;
                    result.format = rf_pixel_format_gray_alpha;
                    result.valid  = 1;
                }
            } else rf_log_error(rf_bad_argument, "Expected compressed pixel formats. `image.format`: %d, `alpha_mask.format`: %d", image.format, alpha_mask.format);
        } else rf_log_error(rf_bad_argument, "Alpha mask must be same size as image but was w: %d, h: %d", alpha_mask.width, alpha_mask.height);
    } else rf_log_error(rf_bad_argument, "One image was invalid. `image.valid`: %d, `alpha_mask.valid`: %d", image.valid, alpha_mask.valid);

    return result;
}

// Clear alpha channel to desired color. Note: Threshold defines the alpha limit, 0.0f to 1.0f
rf_public rf_image rf_image_alpha_clear(rf_image image, rf_color color, float threshold, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        rf_color* pixels = rf_image_pixels_to_rgba32(image, temp_allocator);

        if (pixels)
        {
            for (rf_int i = 0; i < image.width * image.height; i++)
            {
                if (pixels[i].a <= (unsigned char)(threshold * 255.0f))
                {
                    pixels[i] = color;
                }
            }

            rf_image temp_image = {
                .data = pixels,
                .width = image.width,
                .height = image.height,
                .format = rf_pixel_format_r8g8b8a8,
                .valid = 1
            };

            result = rf_image_format(temp_image, image.format, allocator);
        }

        rf_free(temp_allocator, pixels);
    }
    else rf_log_error(rf_bad_argument, "Image is invalid.");

    return result;
}

// Premultiply alpha channel
rf_public rf_image rf_image_alpha_premultiply(rf_image image, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        float alpha = 0.0f;
        rf_color* pixels = rf_image_pixels_to_rgba32(image, temp_allocator);

        if (pixels)
        {
            for (rf_int i = 0; i < image.width * image.height; i++)
            {
                alpha = (float)pixels[i].a / 255.0f;
                pixels[i].r = (unsigned char)((float)pixels[i].r*alpha);
                pixels[i].g = (unsigned char)((float)pixels[i].g*alpha);
                pixels[i].b = (unsigned char)((float)pixels[i].b*alpha);
            }

            rf_image temp_image = {
                .data = pixels,
                .width = image.width,
                .height = image.height,
                .format = rf_pixel_format_r8g8b8a8,
                .valid = 1,
            };

            result = rf_image_format(temp_image, image.format, allocator);
        }

        rf_free(temp_allocator, pixels);
    }
    else rf_log_error(rf_bad_argument, "Image is invalid.");

    return result;
}

rf_public rf_rec rf_image_alpha_crop_rec(rf_image image, float threshold)
{
    if (!image.valid) return (rf_rec){0};

    int bpp = rf_bytes_per_pixel(image.format);

    int x_min = INT_MAX;
    int x_max = 0;
    int y_min = INT_MAX;
    int y_max = 0;

    char* src = image.data;

    for (rf_int y = 0; y < image.height; y++)
    {
        for (rf_int x = 0; x < image.width; x++)
        {
            int pixel = (y * image.width + x) * bpp;
            rf_color pixel_rgba32 = rf_format_one_pixel_to_rgba32(&src[pixel], image.format);

            if (pixel_rgba32.a > (unsigned char)(threshold * 255.0f))
            {
                if (x < x_min) x_min = x;
                if (x > x_max) x_max = x;
                if (y < y_min) y_min = y;
                if (y > y_max) y_max = y;
            }
        }
    }

    return (rf_rec) { x_min, y_min, (x_max + 1) - x_min, (y_max + 1) - y_min };
}

// Crop image depending on alpha value
rf_public rf_image rf_image_alpha_crop(rf_image image, float threshold, rf_allocator allocator)
{
    rf_rec crop = rf_image_alpha_crop_rec(image, threshold);

    return rf_image_crop(image, crop, allocator);
}

// Dither image data to 16bpp or lower (Floyd-Steinberg dithering) Note: In case selected bpp do not represent an known 16bit format, dithered data is stored in the LSB part of the unsigned short
rf_public rf_image rf_image_dither(const rf_image image, int r_bpp, int g_bpp, int b_bpp, int a_bpp, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (image.format == rf_pixel_format_r8g8b8)
        {
            if (image.format == rf_pixel_format_r8g8b8 && (r_bpp + g_bpp + b_bpp + a_bpp) < 16)
            {
                rf_color* pixels = rf_image_pixels_to_rgba32(image, temp_allocator);

                if ((image.format != rf_pixel_format_r8g8b8) && (image.format != rf_pixel_format_r8g8b8a8))
                {
                    rf_log(rf_log_type_warning, "rf_image format is already 16bpp or lower, dithering could have no effect");
                }

                // Todo: Finish implementing this function
                // // Define new image format, check if desired bpp match internal known format
                // if ((r_bpp == 5) && (g_bpp == 6) && (b_bpp == 5) && (a_bpp == 0)) image.format = RF_UNCOMPRESSED_R5G6B5;
                // else if ((r_bpp == 5) && (g_bpp == 5) && (b_bpp == 5) && (a_bpp == 1)) image.format = RF_UNCOMPRESSED_R5G5B5A1;
                // else if ((r_bpp == 4) && (g_bpp == 4) && (b_bpp == 4) && (a_bpp == 4)) image.format = RF_UNCOMPRESSED_R4G4B4A4;
                // else
                // {
                //     image.format = 0;
                //     RF_LOG(RF_LOG_TYPE_WARNING, "Unsupported dithered OpenGL internal format: %ibpp (R%i_g%i_b%i_a%i)", (r_bpp + g_bpp + b_bpp + a_bpp), r_bpp, g_bpp, b_bpp, a_bpp);
                // }
                //
                // // NOTE: We will store the dithered data as unsigned short (16bpp)
                // image.data = (unsigned short*) RF_ALLOC(image.allocator, image.width * image.height * sizeof(unsigned short));

                rf_color old_pixel = rf_white;
                rf_color new_pixel  = rf_white;

                int r_error, g_error, b_error;
                unsigned short r_pixel, g_pixel, b_pixel, a_pixel; // Used for 16bit pixel composition

                for (rf_int y = 0; y < image.height; y++)
                {
                    for (rf_int x = 0; x < image.width; x++)
                    {
                        old_pixel = pixels[y * image.width + x];

                        // NOTE: New pixel obtained by bits truncate, it would be better to round values (check rf_image_format())
                        new_pixel.r = old_pixel.r >> (8 - r_bpp); // R bits
                        new_pixel.g = old_pixel.g >> (8 - g_bpp); // G bits
                        new_pixel.b = old_pixel.b >> (8 - b_bpp); // B bits
                        new_pixel.a = old_pixel.a >> (8 - a_bpp); // A bits (not used on dithering)

                        // NOTE: Error must be computed between new and old pixel but using same number of bits!
                        // We want to know how much color precision we have lost...
                        r_error = (int)old_pixel.r - (int)(new_pixel.r << (8 - r_bpp));
                        g_error = (int)old_pixel.g - (int)(new_pixel.g << (8 - g_bpp));
                        b_error = (int)old_pixel.b - (int)(new_pixel.b << (8 - b_bpp));

                        pixels[y*image.width + x] = new_pixel;

                        // NOTE: Some cases are out of the array and should be ignored
                        if (x < (image.width - 1))
                        {
                            pixels[y*image.width + x+1].r = rf_min_i((int)pixels[y * image.width + x + 1].r + (int)((float)r_error * 7.0f / 16), 0xff);
                            pixels[y*image.width + x+1].g = rf_min_i((int)pixels[y * image.width + x + 1].g + (int)((float)g_error * 7.0f / 16), 0xff);
                            pixels[y*image.width + x+1].b = rf_min_i((int)pixels[y * image.width + x + 1].b + (int)((float)b_error * 7.0f / 16), 0xff);
                        }

                        if ((x > 0) && (y < (image.height - 1)))
                        {
                            pixels[(y+1)*image.width + x-1].r = rf_min_i((int)pixels[(y + 1) * image.width + x - 1].r + (int)((float)r_error * 3.0f / 16), 0xff);
                            pixels[(y+1)*image.width + x-1].g = rf_min_i((int)pixels[(y + 1) * image.width + x - 1].g + (int)((float)g_error * 3.0f / 16), 0xff);
                            pixels[(y+1)*image.width + x-1].b = rf_min_i((int)pixels[(y + 1) * image.width + x - 1].b + (int)((float)b_error * 3.0f / 16), 0xff);
                        }

                        if (y < (image.height - 1))
                        {
                            pixels[(y+1)*image.width + x].r = rf_min_i((int)pixels[(y+1)*image.width + x].r + (int)((float)r_error*5.0f/16), 0xff);
                            pixels[(y+1)*image.width + x].g = rf_min_i((int)pixels[(y+1)*image.width + x].g + (int)((float)g_error*5.0f/16), 0xff);
                            pixels[(y+1)*image.width + x].b = rf_min_i((int)pixels[(y+1)*image.width + x].b + (int)((float)b_error*5.0f/16), 0xff);
                        }

                        if ((x < (image.width - 1)) && (y < (image.height - 1)))
                        {
                            pixels[(y+1)*image.width + x+1].r = rf_min_i((int)pixels[(y+1)*image.width + x+1].r + (int)((float)r_error*1.0f/16), 0xff);
                            pixels[(y+1)*image.width + x+1].g = rf_min_i((int)pixels[(y+1)*image.width + x+1].g + (int)((float)g_error*1.0f/16), 0xff);
                            pixels[(y+1)*image.width + x+1].b = rf_min_i((int)pixels[(y+1)*image.width + x+1].b + (int)((float)b_error*1.0f/16), 0xff);
                        }

                        r_pixel = (unsigned short)new_pixel.r;
                        g_pixel = (unsigned short)new_pixel.g;
                        b_pixel = (unsigned short)new_pixel.b;
                        a_pixel = (unsigned short)new_pixel.a;

                        ((unsigned short *)image.data)[y*image.width + x] = (r_pixel << (g_bpp + b_bpp + a_bpp)) | (g_pixel << (b_bpp + a_bpp)) | (b_pixel << a_bpp) | a_pixel;
                    }
                }

                rf_free(temp_allocator, pixels);
            }
            else rf_log_error(rf_bad_argument, "Unsupported dithering bpps (%ibpp), only 16bpp or lower modes supported", (r_bpp + g_bpp + b_bpp + a_bpp));
        }
    }

    return result;
}

// Flip image vertically
rf_public rf_image rf_image_flip_vertical_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            int bpp = rf_bytes_per_pixel(image.format);

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    void* dst_pixel = ((unsigned char*)dst) + (y * image.width + x) * bpp;
                    void* src_pixel = ((unsigned char*)image.data) + ((image.height - 1 - y) * image.width + x) * bpp;

                    memcpy(dst_pixel, src_pixel, bpp);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_public rf_image rf_image_flip_vertical(rf_image image, rf_allocator allocator)
{
    if (!image.valid) return (rf_image) {0};

    int size = rf_image_size(image);
    void* dst = rf_alloc(allocator, size);

    rf_image result = rf_image_flip_vertical_to_buffer(image, dst, size);
    if (!result.valid) rf_free(allocator, dst);

    return result;
}

// Flip image horizontally
rf_public rf_image rf_image_flip_horizontal_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            int bpp = rf_bytes_per_pixel(image.format);

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    void* dst_pixel = ((unsigned char*)dst) + (y * image.width + x) * bpp;
                    void* src_pixel = ((unsigned char*)image.data) + (y * image.width + (image.width - 1 - x)) * bpp;

                    memcpy(dst_pixel, src_pixel, bpp);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_public rf_image rf_image_flip_horizontal(rf_image image, rf_allocator allocator)
{
    if (!image.valid) return (rf_image) {0};

    int size = rf_image_size(image);
    void* dst = rf_alloc(allocator, size);

    rf_image result = rf_image_flip_horizontal_to_buffer(image, dst, size);
    if (!result.valid) rf_free(allocator, dst);

    return result;
}

// Rotate image clockwise 90deg
rf_public rf_image rf_image_rotate_cw_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            int bpp = rf_bytes_per_pixel(image.format);

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    void* dst_pixel = ((unsigned char*)dst) + (x * image.height + (image.height - y - 1)) * bpp;
                    void* src_pixel = ((unsigned char*)image.data) + (y * image.width + x) * bpp;

                    memcpy(dst_pixel, src_pixel, bpp);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_public rf_image rf_image_rotate_cw(rf_image image)
{
    return rf_image_rotate_cw_to_buffer(image, image.data, rf_image_size(image));
}

// Rotate image counter-clockwise 90deg
rf_public rf_image rf_image_rotate_ccw_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            int bpp = rf_bytes_per_pixel(image.format);

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    void* dst_pixel = ((unsigned char*)dst) + (x * image.height + y) * bpp;
                    void* src_pixel = ((unsigned char*)image.data) + (y * image.width + (image.width - x - 1)) * bpp;

                    memcpy(dst_pixel, src_pixel, bpp);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_public rf_image rf_image_rotate_ccw(rf_image image)
{
    return rf_image_rotate_ccw_to_buffer(image, image.data, rf_image_size(image));
}

// Modify image color: tint
rf_public rf_image rf_image_color_tint_to_buffer(rf_image image, rf_color color, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            int bpp = rf_bytes_per_pixel(image.format);

            float c_r = ((float) color.r) / 255.0f;
            float c_g = ((float) color.g) / 255.0f;
            float c_b = ((float) color.b) / 255.0f;
            float c_a = ((float) color.a) / 255.0f;

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    int   index = y * image.width + x;
                    void* src_pixel = ((unsigned char*)image.data) + index * bpp;
                    void* dst_pixel = ((unsigned char*)image.data) + index * bpp;

                    rf_color pixel_rgba32 = rf_format_one_pixel_to_rgba32(src_pixel, image.format);

                    pixel_rgba32.r = (unsigned char) (255.f * (((float)pixel_rgba32.r) / 255.f * c_r));
                    pixel_rgba32.g = (unsigned char) (255.f * (((float)pixel_rgba32.g) / 255.f * c_g));
                    pixel_rgba32.b = (unsigned char) (255.f * (((float)pixel_rgba32.b) / 255.f * c_b));
                    pixel_rgba32.a = (unsigned char) (255.f * (((float)pixel_rgba32.a) / 255.f * c_a));

                    rf_format_one_pixel(&pixel_rgba32, rf_pixel_format_r8g8b8a8, dst_pixel, image.format);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_public rf_image rf_image_color_tint(rf_image image, rf_color color)
{
    return rf_image_color_tint_to_buffer(image, color, image.data, rf_image_size(image));
}

// Modify image color: invert
rf_public rf_image rf_image_color_invert_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            int bpp = rf_bytes_per_pixel(image.format);

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    int   index = y * image.width + x;
                    void* src_pixel = ((unsigned char*)image.data) + index * bpp;
                    void* dst_pixel = ((unsigned char*)dst) + index * bpp;

                    rf_color pixel_rgba32 = rf_format_one_pixel_to_rgba32(src_pixel, image.format);
                    pixel_rgba32.r = 255 - pixel_rgba32.r;
                    pixel_rgba32.g = 255 - pixel_rgba32.g;
                    pixel_rgba32.b = 255 - pixel_rgba32.b;

                    rf_format_one_pixel(&pixel_rgba32, rf_pixel_format_r8g8b8a8, dst_pixel, image.format);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_public rf_image rf_image_color_invert(rf_image image)
{
    return rf_image_color_invert_to_buffer(image, image.data, rf_image_size(image));
}

// Modify image color: grayscale
rf_public rf_image rf_image_color_grayscale_to_buffer(rf_image image, void* dst, rf_int dst_size)
{
    return rf_image_format_to_buffer(image, rf_pixel_format_grayscale, dst, dst_size);
}

rf_public rf_image rf_image_color_grayscale(rf_image image)
{
    rf_image result = {0};

    if (image.valid)
    {
        result = rf_image_color_grayscale_to_buffer(image, image.data, rf_image_size(image));
    }

    return result;
}

// Modify image color: contrast
// NOTE: Contrast values between -100 and 100
rf_public rf_image rf_image_color_contrast_to_buffer(rf_image image, float contrast, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            if (contrast < -100) contrast = -100;
            if (contrast > +100) contrast = +100;

            contrast = (100.0f + contrast) / 100.0f;
            contrast *= contrast;

            int bpp = rf_bytes_per_pixel(image.format);

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    int index = y * image.width + x;
                    void* src_pixel = ((unsigned char*)image.data) + index * bpp;
                    void* dst_pixel = ((unsigned char*)dst) + index * bpp;

                    rf_color src_pixel_rgba32 = rf_format_one_pixel_to_rgba32(src_pixel, image.format);

                    float p_r = ((float)src_pixel_rgba32.r) / 255.0f;
                    p_r -= 0.5;
                    p_r *= contrast;
                    p_r += 0.5;
                    p_r *= 255;
                    if (p_r < 0) p_r = 0;
                    if (p_r > 255) p_r = 255;

                    float p_g = ((float)src_pixel_rgba32.g) / 255.0f;
                    p_g -= 0.5;
                    p_g *= contrast;
                    p_g += 0.5;
                    p_g *= 255;
                    if (p_g < 0) p_g = 0;
                    if (p_g > 255) p_g = 255;

                    float p_b = ((float)src_pixel_rgba32.b) / 255.0f;
                    p_b -= 0.5;
                    p_b *= contrast;
                    p_b += 0.5;
                    p_b *= 255;
                    if (p_b < 0) p_b = 0;
                    if (p_b > 255) p_b = 255;

                    src_pixel_rgba32.r = (unsigned char)p_r;
                    src_pixel_rgba32.g = (unsigned char)p_g;
                    src_pixel_rgba32.b = (unsigned char)p_b;

                    rf_format_one_pixel(&src_pixel_rgba32, rf_pixel_format_r8g8b8a8, dst_pixel, image.format);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_public rf_image rf_image_color_contrast(rf_image image, int contrast)
{
    return rf_image_color_contrast_to_buffer(image, contrast, image.data, rf_image_size(image));
}

// Modify image color: brightness
// NOTE: Brightness values between -255 and 255
rf_public rf_image rf_image_color_brightness_to_buffer(rf_image image, int brightness, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (image.valid)
    {
        if (dst_size >= rf_image_size(image))
        {
            if (brightness < -255) brightness = -255;
            if (brightness > +255) brightness = +255;

            int bpp = rf_bytes_per_pixel(image.format);

            for (rf_int y = 0; y < image.height; y++)
            {
                for (rf_int x = 0; x < image.width; x++)
                {
                    int index = y * image.width + x;

                    void* src_pixel = ((unsigned char*)image.data) + index * bpp;
                    void* dst_pixel = ((unsigned char*)dst) + index * bpp;

                    rf_color pixel_rgba32 = rf_format_one_pixel_to_rgba32(src_pixel, image.format);

                    int c_r = pixel_rgba32.r + brightness;
                    int c_g = pixel_rgba32.g + brightness;
                    int c_b = pixel_rgba32.b + brightness;

                    if (c_r < 0) c_r = 1;
                    if (c_r > 255) c_r = 255;

                    if (c_g < 0) c_g = 1;
                    if (c_g > 255) c_g = 255;

                    if (c_b < 0) c_b = 1;
                    if (c_b > 255) c_b = 255;

                    pixel_rgba32.r = (unsigned char) c_r;
                    pixel_rgba32.g = (unsigned char) c_g;
                    pixel_rgba32.b = (unsigned char) c_b;

                    rf_format_one_pixel(&pixel_rgba32, rf_pixel_format_r8g8b8a8, dst_pixel, image.format);
                }
            }

            result = image;
            result.data = dst;
        }
    }

    return result;
}

rf_public rf_image rf_image_color_brightness(rf_image image, int brightness)
{
    return rf_image_color_brightness_to_buffer(image, brightness, image.data, rf_image_size(image));
}

// Modify image color: replace color
rf_public rf_image rf_image_color_replace_to_buffer(rf_image image, rf_color color, rf_color replace, void* dst, rf_int dst_size)
{
    if (image.valid && dst_size >= rf_image_size(image)) return (rf_image) {0};

    rf_image result = {0};

    int bpp = rf_bytes_per_pixel(image.format);

    for (rf_int y = 0; y < image.height; y++)
    {
        for (rf_int x = 0; x < image.width; x++)
        {
            int index = y * image.width + x;

            void* src_pixel = ((unsigned char*)image.data) + index * bpp;
            void* dst_pixel = ((unsigned char*)dst) + index * bpp;

            rf_color pixel_rgba32 = rf_format_one_pixel_to_rgba32(src_pixel, image.format);

            if (rf_color_match(pixel_rgba32, color))
            {
                rf_format_one_pixel(&replace, rf_pixel_format_r8g8b8a8, dst_pixel, image.format);
            }
        }
    }

    result = image;
    result.data = dst;

    return result;
}

rf_public rf_image rf_image_color_replace(rf_image image, rf_color color, rf_color replace)
{
    return rf_image_color_replace_to_buffer(image, color, replace, image.data, rf_image_size(image));
}

// Generate image: plain color
rf_public rf_image rf_gen_image_color_to_buffer(int width, int height, rf_color color, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    for (rf_int i = 0; i < dst_size; i++)
    {
        dst[i] = color;
    }

    return (rf_image) {
        .data   = dst,
        .width  = width,
        .height = height,
        .format = rf_pixel_format_r8g8b8a8,
        .valid  = 1,
    };
}

rf_public rf_image rf_gen_image_color(int width, int height, rf_color color, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
    void* dst = rf_alloc(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_color_to_buffer(width, height, color, dst, width * height);
    }

    return result;
}

// Generate image: vertical gradient
rf_public rf_image rf_gen_image_gradient_v_to_buffer(int width, int height, rf_color top, rf_color bottom, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8))
    {
        for (rf_int j = 0; j < height; j++)
        {
            float factor = ((float)j) / ((float)height);

            for (rf_int i = 0; i < width; i++)
            {
                ((rf_color*)dst)[j * width + i].r = (int)((float)bottom.r * factor + (float)top.r * (1.f - factor));
                ((rf_color*)dst)[j * width + i].g = (int)((float)bottom.g * factor + (float)top.g * (1.f - factor));
                ((rf_color*)dst)[j * width + i].b = (int)((float)bottom.b * factor + (float)top.b * (1.f - factor));
                ((rf_color*)dst)[j * width + i].a = (int)((float)bottom.a * factor + (float)top.a * (1.f - factor));
            }
        }

        result = (rf_image)
        {
            .data = dst,
            .width = width,
            .height = height,
            .format = rf_pixel_format_r8g8b8a8,
            .valid = 1
        };
    }

    return result;
}

rf_public rf_image rf_gen_image_gradient_v(int width, int height, rf_color top, rf_color bottom, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
    rf_color* dst = rf_alloc(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_gradient_v_to_buffer(width, height, top, bottom, dst, dst_size);
    }

    return result;
}

// Generate image: horizontal gradient
rf_public rf_image rf_gen_image_gradient_h_to_buffer(int width, int height, rf_color left, rf_color right, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8))
    {
        for (rf_int i = 0; i < width; i++)
        {
            float factor = ((float)i) / ((float)width);

            for (rf_int j = 0; j < height; j++)
            {
                ((rf_color*)dst)[j * width + i].r = (int)((float)right.r * factor + (float)left.r * (1.f - factor));
                ((rf_color*)dst)[j * width + i].g = (int)((float)right.g * factor + (float)left.g * (1.f - factor));
                ((rf_color*)dst)[j * width + i].b = (int)((float)right.b * factor + (float)left.b * (1.f - factor));
                ((rf_color*)dst)[j * width + i].a = (int)((float)right.a * factor + (float)left.a * (1.f - factor));
            }
        }

        result = (rf_image)
        {
            .data = dst,
            .width = width,
            .height = height,
            .format = rf_pixel_format_r8g8b8a8,
            .valid = 1
        };
    }

    return result;
}

rf_public rf_image rf_gen_image_gradient_h(int width, int height, rf_color left, rf_color right, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
    rf_color* dst = rf_alloc(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_gradient_h_to_buffer(width, height, left, right, dst, dst_size);
    }

    return result;
}

// Generate image: radial gradient
rf_public rf_image rf_gen_image_gradient_radial_to_buffer(int width, int height, float density, rf_color inner, rf_color outer, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8))
    {
        float radius = (width < height) ? ((float)width) / 2.0f : ((float)height) / 2.0f;

        float center_x = ((float)width ) / 2.0f;
        float center_y = ((float)height) / 2.0f;

        for (rf_int y = 0; y < height; y++)
        {
            for (rf_int x = 0; x < width; x++)
            {
                float dist   = hypotf((float)x - center_x, (float)y - center_y);
                float factor = (dist - radius * density) / (radius * (1.0f - density));

                factor = (float)fmax(factor, 0.f);
                factor = (float)fmin(factor, 1.f); // dist can be bigger than radius so we have to check

                dst[y * width + x].r = (int)((float)outer.r * factor + (float)inner.r * (1.0f - factor));
                dst[y * width + x].g = (int)((float)outer.g * factor + (float)inner.g * (1.0f - factor));
                dst[y * width + x].b = (int)((float)outer.b * factor + (float)inner.b * (1.0f - factor));
                dst[y * width + x].a = (int)((float)outer.a * factor + (float)inner.a * (1.0f - factor));
            }
        }

        result = (rf_image)
        {
            .data = dst,
            .width = width,
            .height = height,
            .format = rf_pixel_format_r8g8b8a8,
            .valid = 1,
        };
    }

    return result;
}

rf_public rf_image rf_gen_image_gradient_radial(int width, int height, float density, rf_color inner, rf_color outer, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
    rf_color* dst = rf_alloc(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_gradient_radial_to_buffer(width, height, density, inner, outer, dst, dst_size);
    }

    return result;
}

// Generate image: checked
rf_public rf_image rf_gen_image_checked_to_buffer(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8))
    {
        float radius = (width < height) ? ((float)width) / 2.0f : ((float)height) / 2.0f;

        float center_x = ((float)width ) / 2.0f;
        float center_y = ((float)height) / 2.0f;

        for (rf_int y = 0; y < height; y++)
        {
            for (rf_int x = 0; x < width; x++)
            {
                if ((x / checks_x + y / checks_y) % 2 == 0) dst[y * width + x] = col1;
                else dst[y * width + x] = col2;
            }
        }

        result = (rf_image)
        {
            .data = dst,
            .width = width,
            .height = height,
            .format = rf_pixel_format_r8g8b8a8,
            .valid = 1,
        };
    }

    return result;
}

rf_public rf_image rf_gen_image_checked(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
    rf_color* dst = rf_alloc(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_checked_to_buffer(width, height, checks_x, checks_y, col1, col2, dst, dst_size);
    }

    return result;
}

// Generate image: white noise
rf_public rf_image rf_gen_image_white_noise_to_buffer(int width, int height, float factor, rf_rand_proc rand, rf_color* dst, rf_int dst_size)
{
    int result_image_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
    rf_image result = {0};

    if (dst_size < result_image_size || !rand || result_image_size <= 0) return result;

    for (rf_int i = 0; i < width * height; i++)
    {
        if (rand(0, 99) < (int)(factor * 100.0f))
        {
            dst[i] = rf_white;
        }
        else
        {
            dst[i] = rf_black;
        }
    }

    result = (rf_image)
    {
        .data = dst,
        .width = width,
        .height = height,
        .format = rf_pixel_format_r8g8b8a8,
        .valid = 1,
    };

    return result;
}

rf_public rf_image rf_gen_image_white_noise(int width, int height, float factor, rf_rand_proc rand, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);

    if (!rand || dst_size <= 0) return result;

    rf_color* dst = rf_alloc(allocator, dst_size);
    result = rf_gen_image_white_noise_to_buffer(width, height, factor, rand, dst, dst_size);

    return result;
}

// Generate image: perlin noise
rf_public rf_image rf_gen_image_perlin_noise_to_buffer(int width, int height, int offset_x, int offset_y, float scale, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (dst_size >= width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8))
    {
        for (rf_int y = 0; y < height; y++)
        {
            for (rf_int x = 0; x < width; x++)
            {
                float nx = (float)(x + offset_x)*scale/(float)width;
                float ny = (float)(y + offset_y)*scale/(float)height;

                // Typical values to start playing with:
                //   lacunarity = ~2.0   -- spacing between successive octaves (use exactly 2.0 for wrapping output)
                //   gain       =  0.5   -- relative weighting applied to each successive octave
                //   octaves    =  6     -- number of "octaves" of noise3() to sum

                // NOTE: We need to translate the data from [-1..1] to [0..1]
                float p = (stb_perlin_fbm_noise3(nx, ny, 1.0f, 2.0f, 0.5f, 6) + 1.0f) / 2.0f;

                int intensity = (int)(p * 255.0f);
                dst[y * width + x] = (rf_color){ intensity, intensity, intensity, 255 };
            }
        }

        result = (rf_image)
        {
            .data = dst,
            .width = width,
            .height = height,
            .format = rf_pixel_format_r8g8b8a8,
            .valid = 1,
        };
    }

    return result;
}

rf_public rf_image rf_gen_image_perlin_noise(int width, int height, int offset_x, int offset_y, float scale, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
    rf_color* dst = rf_alloc(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_perlin_noise_to_buffer(width, height, offset_x, offset_y, scale, dst, dst_size);
    }

    return result;
}

rf_public rf_vec2 rf_get_seed_for_cellular_image(int seeds_per_row, int tile_size, int i, rf_rand_proc rand)
{
    rf_vec2 result = {0};

    int y = (i / seeds_per_row) * tile_size + rand(0, tile_size - 1);
    int x = (i % seeds_per_row) * tile_size + rand(0, tile_size - 1);
    result = (rf_vec2) { (float) x, (float) y };

    return result;
}

// Generate image: cellular algorithm. Bigger tileSize means bigger cells
rf_public rf_image rf_gen_image_cellular_to_buffer(int width, int height, int tile_size, rf_rand_proc rand, rf_color* dst, rf_int dst_size)
{
    rf_image result = {0};

    int seeds_per_row = width / tile_size;
    int seeds_per_col = height / tile_size;
    int seeds_count = seeds_per_row * seeds_per_col;

    if (dst_size >= width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8))
    {
        for (rf_int y = 0; y < height; y++)
        {
            int tile_y = y / tile_size;

            for (rf_int x = 0; x < width; x++)
            {
                int tile_x = x / tile_size;

                float min_distance = INFINITY;

                // Check all adjacent tiles
                for (rf_int i = -1; i < 2; i++)
                {
                    if ((tile_x + i < 0) || (tile_x + i >= seeds_per_row)) continue;

                    for (rf_int j = -1; j < 2; j++)
                    {
                        if ((tile_y + j < 0) || (tile_y + j >= seeds_per_col)) continue;

                        rf_vec2 neighbor_seed = rf_get_seed_for_cellular_image(seeds_per_row, tile_size, (tile_y + j) * seeds_per_row + tile_x + i, rand);

                        float dist = (float)hypot(x - (int)neighbor_seed.x, y - (int)neighbor_seed.y);
                        min_distance = (float)fmin(min_distance, dist);
                    }
                }

                // I made this up but it seems to give good results at all tile sizes
                int intensity = (int)(min_distance * 256.0f / tile_size);
                if (intensity > 255) intensity = 255;

                dst[y * width + x] = (rf_color) { intensity, intensity, intensity, 255 };
            }
        }

        result = (rf_image)
        {
            .data = dst,
            .width = width,
            .height = height,
            .format = rf_pixel_format_r8g8b8a8,
            .valid = 1,
        };
    }

    return result;
}

rf_public rf_image rf_gen_image_cellular(int width, int height, int tile_size, rf_rand_proc rand, rf_allocator allocator)
{
    rf_image result = {0};

    int dst_size = width * height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);

    rf_color* dst = rf_alloc(allocator, dst_size);

    if (dst)
    {
        result = rf_gen_image_cellular_to_buffer(width, height, tile_size, rand, dst, dst_size);
    }

    return result;
}

// Draw an image (source) within an image (destination)
// NOTE: rf_color tint is applied to source image
rf_public void rf_image_draw(rf_image* dst, rf_image src, rf_rec src_rec, rf_rec dst_rec, rf_color tint, rf_allocator temp_allocator)
{
    if (src.valid && dst->valid)
    {
        dst->valid = 0;

        if (src_rec.x < 0) src_rec.x = 0;
        if (src_rec.y < 0) src_rec.y = 0;

        if ((src_rec.x + src_rec.width) > src.width)
        {
            src_rec.width = src.width - src_rec.x;
            rf_log(rf_log_type_warning, "Source rectangle width out of bounds, rescaled width: %i", src_rec.width);
        }

        if ((src_rec.y + src_rec.height) > src.height)
        {
            src_rec.height = src.height - src_rec.y;
            rf_log(rf_log_type_warning, "Source rectangle height out of bounds, rescaled height: %i", src_rec.height);
        }

        rf_image src_copy = rf_image_copy(src, temp_allocator); // Make a copy of source src to work with it

        // Crop source image to desired source rectangle (if required)
        if ((src.width != (int)src_rec.width) && (src.height != (int)src_rec.height))
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, src_rec, temp_allocator);
            rf_unload_image(old_src_copy, temp_allocator);
        }

        // Scale source image in case destination rec size is different than source rec size
        if (((int)dst_rec.width != (int)src_rec.width) || ((int)dst_rec.height != (int)src_rec.height))
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_resize(src_copy, (int)dst_rec.width, (int)dst_rec.height, temp_allocator, temp_allocator);
            rf_unload_image(old_src_copy, temp_allocator);
        }

        // Check that dstRec is inside dst image
        // Allow negative position within destination with cropping
        if (dst_rec.x < 0)
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, (rf_rec) { -dst_rec.x, 0, dst_rec.width + dst_rec.x, dst_rec.height }, temp_allocator);
            dst_rec.width = dst_rec.width + dst_rec.x;
            dst_rec.x = 0;
            rf_unload_image(old_src_copy, temp_allocator);
        }

        if ((dst_rec.x + dst_rec.width) > dst->width)
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, (rf_rec) { 0, 0, dst->width - dst_rec.x, dst_rec.height }, temp_allocator);
            dst_rec.width = dst->width - dst_rec.x;
            rf_unload_image(old_src_copy, temp_allocator);
        }

        if (dst_rec.y < 0)
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, (rf_rec) { 0, -dst_rec.y, dst_rec.width, dst_rec.height + dst_rec.y }, temp_allocator);
            dst_rec.height = dst_rec.height + dst_rec.y;
            dst_rec.y = 0;
            rf_unload_image(old_src_copy, temp_allocator);
        }

        if ((dst_rec.y + dst_rec.height) > dst->height)
        {
            rf_image old_src_copy = src_copy;
            src_copy = rf_image_crop(src_copy, (rf_rec) { 0, 0, dst_rec.width, dst->height - dst_rec.y }, temp_allocator);
            dst_rec.height = dst->height - dst_rec.y;
            rf_unload_image(old_src_copy, temp_allocator);
        }

        if (src_copy.valid)
        {
            // Get image data as rf_color pixels array to work with it
            rf_color* dst_pixels = rf_image_pixels_to_rgba32(*dst, temp_allocator);
            rf_color* src_pixels = rf_image_pixels_to_rgba32(src_copy, temp_allocator);

            rf_unload_image(src_copy, temp_allocator); // Source copy not required any more

            rf_vec4 fsrc, fdst, fout; // Normalized pixel data (ready for operation)
            rf_vec4 ftint = rf_color_normalize(tint); // Normalized color tint

            // Blit pixels, copy source image into destination
            // TODO: Maybe out-of-bounds blitting could be considered here instead of so much cropping
            for (rf_int j = (int)dst_rec.y; j < (int)(dst_rec.y + dst_rec.height); j++)
            {
                for (rf_int i = (int)dst_rec.x; i < (int)(dst_rec.x + dst_rec.width); i++)
                {
                    // Alpha blending (https://en.wikipedia.org/wiki/Alpha_compositing)

                    fdst = rf_color_normalize(dst_pixels[j * (int)dst->width + i]);
                    fsrc = rf_color_normalize(src_pixels[(j - (int)dst_rec.y) * (int)dst_rec.width + (i - (int)dst_rec.x)]);

                    // Apply color tint to source image
                    fsrc.x *= ftint.x; fsrc.y *= ftint.y; fsrc.z *= ftint.z; fsrc.w *= ftint.w;

                    fout.w = fsrc.w + fdst.w * (1.0f - fsrc.w);

                    if (fout.w <= 0.0f)
                    {
                        fout.x = 0.0f;
                        fout.y = 0.0f;
                        fout.z = 0.0f;
                    }
                    else
                    {
                        fout.x = (fsrc.x*fsrc.w + fdst.x*fdst.w*(1 - fsrc.w))/fout.w;
                        fout.y = (fsrc.y*fsrc.w + fdst.y*fdst.w*(1 - fsrc.w))/fout.w;
                        fout.z = (fsrc.z*fsrc.w + fdst.z*fdst.w*(1 - fsrc.w))/fout.w;
                    }

                    dst_pixels[j*(int)dst->width + i] = (rf_color) { (unsigned char)(fout.x * 255.0f),
                                                                    (unsigned char)(fout.y * 255.0f),
                                                                    (unsigned char)(fout.z * 255.0f),
                                                                    (unsigned char)(fout.w * 255.0f) };

                    // TODO: Support other blending options
                }
            }

            rf_bool format_success = rf_format_pixels(dst_pixels, rf_image_size_in_format(*dst, rf_pixel_format_r8g8b8a8), rf_pixel_format_r8g8b8a8,
                                                      dst->data, rf_image_size(*dst), dst->format);
            rf_assert(format_success);

            rf_free(temp_allocator, src_pixels);
            rf_free(temp_allocator, dst_pixels);

            dst->valid = 1;
        }
    }
}

// Draw rectangle within an image
rf_public void rf_image_draw_rectangle(rf_image* dst, rf_rec rec, rf_color color, rf_allocator temp_allocator)
{
    if (dst->valid)
    {
        dst->valid = 0;

        rf_image src = rf_gen_image_color((int)rec.width, (int)rec.height, color, temp_allocator);

        if (src.valid)
        {
            rf_rec src_rec = (rf_rec){ 0, 0, rec.width, rec.height };

            rf_image_draw(dst, src, src_rec, rec, rf_white, temp_allocator);

            rf_unload_image(src, temp_allocator);
        }
    }
}

// Draw rectangle lines within an image
rf_public void rf_image_draw_rectangle_lines(rf_image* dst, rf_rec rec, int thick, rf_color color, rf_allocator temp_allocator)
{
    rf_image_draw_rectangle(dst, (rf_rec) { rec.x, rec.y, rec.width, thick }, color, temp_allocator);
    rf_image_draw_rectangle(dst, (rf_rec) { rec.x, rec.y + thick, thick, rec.height - thick * 2 }, color, temp_allocator);
    rf_image_draw_rectangle(dst, (rf_rec) { rec.x + rec.width - thick, rec.y + thick, thick, rec.height - thick * 2 }, color, temp_allocator);
    rf_image_draw_rectangle(dst, (rf_rec) { rec.x, rec.y + rec.height - thick, rec.width, thick }, color, temp_allocator);
}

#pragma endregion

#pragma region mipmaps

rf_public int rf_mipmaps_image_size(rf_mipmaps_image image)
{
    int size   = 0;
    int width  = image.width;
    int height = image.height;

    for (rf_int i = 0; i < image.mipmaps; i++)
    {
        size += width * height * rf_bytes_per_pixel(image.format);

        width  /= 2;
        height /= 2;

        // Security check for NPOT textures
        if (width  < 1) width  = 1;
        if (height < 1) height = 1;
    }

    return size;
}

rf_public rf_mipmaps_stats rf_compute_mipmaps_stats(rf_image image, int desired_mipmaps_count)
{
    if (!image.valid) return (rf_mipmaps_stats) {0};

    int possible_mip_count = 1;
    int mipmaps_size = rf_image_size(image);

    int mip_width = image.width;
    int mip_height = image.height;

    while (mip_width != 1 || mip_height != 1 || possible_mip_count == desired_mipmaps_count)
    {
        if (mip_width  != 1) mip_width  /= 2;
        if (mip_height != 1) mip_height /= 2;

        // Safety check for NPOT textures
        if (mip_width  < 1) mip_width  = 1;
        if (mip_height < 1) mip_height = 1;

        mipmaps_size += mip_width * mip_height * rf_bytes_per_pixel(image.format);

        possible_mip_count++;
    }

    return (rf_mipmaps_stats) { possible_mip_count, mipmaps_size };
}

// Generate all mipmap levels for a provided image. image.data is scaled to include mipmap levels. Mipmaps format is the same as base image
rf_public rf_mipmaps_image rf_image_gen_mipmaps_to_buffer(rf_image image, int gen_mipmaps_count, void* dst, rf_int dst_size, rf_allocator temp_allocator)
{
    if (image.valid) return (rf_mipmaps_image) {0};

    rf_mipmaps_image result = {0};
    rf_mipmaps_stats mipmap_stats = rf_compute_mipmaps_stats(image, gen_mipmaps_count);

    if (mipmap_stats.possible_mip_counts <= gen_mipmaps_count)
    {
        if (dst_size == mipmap_stats.mipmaps_buffer_size)
        {
            // Pointer to current mip location in the dst buffer
            unsigned char* dst_iter = dst;

            // Copy the image to the dst as the first mipmap level
            memcpy(dst_iter, image.data, rf_image_size(image));
            dst_iter += rf_image_size(image);

            // Create a rgba32 buffer for the mipmap result, half the image size is enough for any mipmap level
            int temp_mipmap_buffer_size = (image.width / 2) * (image.height / 2) * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
            void* temp_mipmap_buffer = rf_alloc(temp_allocator, temp_mipmap_buffer_size);

            if (temp_mipmap_buffer)
            {
                int mip_width  = image.width  / 2;
                int mip_height = image.height / 2;
                int mip_count = 1;
                for (; mip_count < gen_mipmaps_count; mip_count++)
                {
                    rf_image mipmap = rf_image_resize_to_buffer(image, mip_width, mip_height, temp_mipmap_buffer, temp_mipmap_buffer_size, temp_allocator);

                    if (mipmap.valid)
                    {
                        int dst_iter_size = dst_size - ((int)(dst_iter - ((unsigned char*)(dst))));

                        rf_bool success = rf_format_pixels(mipmap.data, rf_image_size(mipmap), mipmap.format, dst_iter, dst_iter_size, image.format);
                        rf_assert(success);
                    }
                    else break;

                    mip_width  /= 2;
                    mip_height /= 2;

                    // Security check for NPOT textures
                    if (mip_width  < 1) mip_width  = 1;
                    if (mip_height < 1) mip_height = 1;

                    // Compute next mipmap location in the dst buffer
                    dst_iter += mip_width * mip_height * rf_bytes_per_pixel(image.format);
                }

                if (mip_count == gen_mipmaps_count)
                {
                    result = (rf_mipmaps_image){
                        .data = dst,
                        .width = image.width,
                        .height = image.height,
                        .mipmaps = gen_mipmaps_count,
                        .format = image.format,
                        .valid = 1
                    };
                }
            }

            rf_free(temp_allocator, temp_mipmap_buffer);
        }
    }
    else rf_log(rf_log_type_warning, "rf_image mipmaps already available");

    return result;
}

rf_public rf_mipmaps_image rf_image_gen_mipmaps(rf_image image, int desired_mipmaps_count, rf_allocator allocator, rf_allocator temp_allocator)
{
    if (!image.valid) return (rf_mipmaps_image) {0};

    rf_mipmaps_image result = {0};
    rf_mipmaps_stats mipmap_stats = rf_compute_mipmaps_stats(image, desired_mipmaps_count);

    if (mipmap_stats.possible_mip_counts <= desired_mipmaps_count)
    {
        void* dst = rf_alloc(allocator, mipmap_stats.mipmaps_buffer_size);

        if (dst)
        {
            result = rf_image_gen_mipmaps_to_buffer(image, desired_mipmaps_count, dst, mipmap_stats.mipmaps_buffer_size, temp_allocator);
            if (!result.valid)
            {
                rf_free(allocator, dst);
            }
        }
    }

    return result;
}

rf_public void rf_unload_mipmaps_image(rf_mipmaps_image image, rf_allocator allocator)
{
    rf_free(allocator, image.data);
}

#pragma endregion

#pragma region dds

/*
 Required extension:
 GL_EXT_texture_compression_s3tc

 Supported tokens (defined by extensions)
 GL_COMPRESSED_RGB_S3TC_DXT1_EXT      0x83F0
 GL_COMPRESSED_RGBA_S3TC_DXT1_EXT     0x83F1
 GL_COMPRESSED_RGBA_S3TC_DXT3_EXT     0x83F2
 GL_COMPRESSED_RGBA_S3TC_DXT5_EXT     0x83F3
*/

#define RF_FOURCC_DXT1 (0x31545844)  // Equivalent to "DXT1" in ASCII
#define RF_FOURCC_DXT3 (0x33545844)  // Equivalent to "DXT3" in ASCII
#define RF_FOURCC_DXT5 (0x35545844)  // Equivalent to "DXT5" in ASCII

typedef struct rf_dds_pixel_format rf_dds_pixel_format;
struct rf_dds_pixel_format
{
    unsigned int size;
    unsigned int flags;
    unsigned int four_cc;
    unsigned int rgb_bit_count;
    unsigned int r_bit_mask;
    unsigned int g_bit_mask;
    unsigned int b_bit_mask;
    unsigned int a_bit_mask;
};

// DDS Header (must be 124 bytes)
typedef struct rf_dds_header rf_dds_header;
struct rf_dds_header
{
    char id[4];
    unsigned int size;
    unsigned int flags;
    unsigned int height;
    unsigned int width;
    unsigned int pitch_or_linear_size;
    unsigned int depth;
    unsigned int mipmap_count;
    unsigned int reserved_1[11];
    rf_dds_pixel_format ddspf;
    unsigned int caps;
    unsigned int caps_2;
    unsigned int caps_3;
    unsigned int caps_4;
    unsigned int reserved_2;
};

rf_public rf_int rf_get_dds_image_size(const void* src, rf_int src_size)
{
    int result = 0;

    if (src && src_size >= sizeof(rf_dds_header))
    {
        rf_dds_header header = *(rf_dds_header*)src;

        // Verify the type of file
        if (rf_match_str_cstr(header.id, sizeof(header.id), "DDS "))
        {
            if (header.ddspf.rgb_bit_count == 16) // 16bit mode, no compressed
            {
                if (header.ddspf.flags == 0x40) // no alpha channel
                {
                    result = header.width * header.height * sizeof(unsigned short);
                }
                else if (header.ddspf.flags == 0x41) // with alpha channel
                {
                    if (header.ddspf.a_bit_mask == 0x8000) // 1bit alpha
                    {
                        result = header.width * header.height * sizeof(unsigned short);
                    }
                    else if (header.ddspf.a_bit_mask == 0xf000) // 4bit alpha
                    {
                        result = header.width * header.height * sizeof(unsigned short);
                    }
                }
            }
            else if (header.ddspf.flags == 0x40 && header.ddspf.rgb_bit_count == 24) // DDS_RGB, no compressed
            {
                // Not sure if this case exists...
                result = header.width * header.height * 3 * sizeof(unsigned char);
            }
            else if (header.ddspf.flags == 0x41 && header.ddspf.rgb_bit_count == 32) // DDS_RGBA, no compressed
            {
                result = header.width * header.height * 4 * sizeof(unsigned char);
            }
            else if (((header.ddspf.flags == 0x04) || (header.ddspf.flags == 0x05)) && (header.ddspf.four_cc > 0)) // Compressed
            {
                int size;       // DDS result data size

                // Calculate data size, including all mipmaps
                if (header.mipmap_count > 1) size = header.pitch_or_linear_size * 2;
                else size = header.pitch_or_linear_size;

                result = size * sizeof(unsigned char);
            }
        }
    }

    return result;
}

rf_public rf_mipmaps_image rf_load_dds_image_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size)
{
    rf_mipmaps_image result = { 0 };

    if (src && dst && dst_size > 0 && src_size >= sizeof(rf_dds_header))
    {
        rf_dds_header header = *(rf_dds_header*)src;

        src = ((char*)src) + sizeof(rf_dds_header);
        src_size -= sizeof(rf_dds_header);

        // Verify the type of file
        if (rf_match_str_cstr(header.id, sizeof(header.id), "DDS "))
        {
            result.width   = header.width;
            result.height  = header.height;
            result.mipmaps = (header.mipmap_count == 0) ? 1 : header.mipmap_count;

            if (header.ddspf.rgb_bit_count == 16) // 16bit mode, no compressed
            {
                if (header.ddspf.flags == 0x40) // no alpha channel
                {
                    int dds_result_size = header.width * header.height * sizeof(unsigned short);

                    if (src_size >= dds_result_size && dst_size >= dds_result_size)
                    {
                        memcpy(dst, src, dds_result_size);
                        result.format = rf_pixel_format_r5g6b5;
                        result.data   = dst;
                        result.valid  = 1;
                    }
                }
                else if (header.ddspf.flags == 0x41) // with alpha channel
                {
                    if (header.ddspf.a_bit_mask == 0x8000) // 1bit alpha
                    {
                        int dds_result_size = header.width * header.height * sizeof(unsigned short);

                        if (src_size >= dds_result_size && dst_size >= dds_result_size)
                        {
                            memcpy(dst, src, dds_result_size);

                            unsigned char alpha = 0;

                            // Data comes as A1R5G5B5, it must be reordered to R5G5B5A1
                            for (rf_int i = 0; i < result.width * result.height; i++)
                            {
                                alpha = ((unsigned short *)result.data)[i] >> 15;
                                ((unsigned short*)result.data)[i] = ((unsigned short *)result.data)[i] << 1;
                                ((unsigned short*)result.data)[i] += alpha;
                            }

                            result.format = rf_pixel_format_r5g5b5a1;
                            result.data   = dst;
                            result.valid  = 1;
                        }
                    }
                    else if (header.ddspf.a_bit_mask == 0xf000) // 4bit alpha
                    {
                        int dds_result_size = header.width * header.height * sizeof(unsigned short);

                        if (src_size >= dds_result_size && dst_size >= dds_result_size)
                        {
                            memcpy(dst, src, dds_result_size);

                            unsigned char alpha = 0;

                            // Data comes as A4R4G4B4, it must be reordered R4G4B4A4
                            for (rf_int i = 0; i < result.width * result.height; i++)
                            {
                                alpha = ((unsigned short*)result.data)[i] >> 12;
                                ((unsigned short*)result.data)[i] = ((unsigned short*)result.data)[i] << 4;
                                ((unsigned short*)result.data)[i] += alpha;
                            }

                            result.format = rf_pixel_format_r4g4b4a4;
                            result.data   = dst;
                            result.valid  = 1;
                        }
                    }
                }
            }
            else if (header.ddspf.flags == 0x40 && header.ddspf.rgb_bit_count == 24) // DDS_RGB, no compressed, not sure if this case exists...
            {
                int dds_result_size = header.width * header.height * 3;

                if (src_size >= dds_result_size && dst_size >= dds_result_size)
                {
                    memcpy(dst, src, dds_result_size);
                    result.format = rf_pixel_format_r8g8b8;
                    result.data   = dst;
                    result.valid  = 1;
                }
            }
            else if (header.ddspf.flags == 0x41 && header.ddspf.rgb_bit_count == 32) // DDS_RGBA, no compressed
            {
                int dds_result_size = header.width * header.height * 4;

                if (src_size >= dds_result_size && dst_size >= dds_result_size)
                {
                    memcpy(dst, src, dds_result_size);

                    unsigned char blue = 0;

                    // Data comes as A8R8G8B8, it must be reordered R8G8B8A8 (view next comment)
                    // DirecX understand ARGB as a 32bit DWORD but the actual memory byte alignment is BGRA
                    // So, we must realign B8G8R8A8 to R8G8B8A8
                    for (rf_int i = 0; i < header.width * header.height * 4; i += 4)
                    {
                        blue = ((unsigned char*)dst)[i];
                        ((unsigned char*)dst)[i + 0] = ((unsigned char*)dst)[i + 2];
                        ((unsigned char*)dst)[i + 2] = blue;
                    }

                    result.format = rf_pixel_format_r8g8b8a8;
                    result.data   = dst;
                    result.valid  = 1;
                }
            }
            else if (((header.ddspf.flags == 0x04) || (header.ddspf.flags == 0x05)) && (header.ddspf.four_cc > 0)) // Compressed
            {
                int dds_result_size = (header.mipmap_count > 1) ? (header.pitch_or_linear_size * 2) : header.pitch_or_linear_size;

                if (src_size >= dds_result_size && dst_size >= dds_result_size)
                {
                    memcpy(dst, src, dds_result_size);

                    switch (header.ddspf.four_cc)
                    {
                        case RF_FOURCC_DXT1: result.format = header.ddspf.flags == 0x04 ? rf_pixel_format_dxt1_rgb : rf_pixel_format_dxt1_rgba; break;
                        case RF_FOURCC_DXT3: result.format = rf_pixel_format_dxt3_rgba; break;
                        case RF_FOURCC_DXT5: result.format = rf_pixel_format_dxt5_rgba; break;
                        default: break;
                    }

                    result.data  = dst;
                    result.valid = 1;
                }
            }
        }
        else rf_log_error(rf_bad_format, "DDS file does not seem to be a valid result");
    }

    return result;
}

rf_public rf_mipmaps_image rf_load_dds_image(const void* src, rf_int src_size, rf_allocator allocator)
{
    rf_mipmaps_image result = {0};

    int dst_size = rf_get_dds_image_size(src, src_size);
    void* dst = rf_alloc(allocator, dst_size);

    result = rf_load_dds_image_to_buffer(src, src_size, dst, dst_size);

    return result;
}

rf_public rf_mipmaps_image rf_load_dds_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_mipmaps_image result = {0};

    int src_size = rf_file_size(io, file);
    void* src = rf_alloc(temp_allocator, src_size);

    if (rf_read_file(io, file, src, src_size))
    {
        result = rf_load_dds_image(src, src_size, allocator);
    }

    rf_free(temp_allocator, src);

    return result;
}
#pragma endregion

#pragma region pkm

/*
 Required extensions:
 GL_OES_compressed_ETC1_RGB8_texture  (ETC1) (OpenGL ES 2.0)
 GL_ARB_ES3_compatibility  (ETC2/EAC) (OpenGL ES 3.0)

 Supported tokens (defined by extensions)
 GL_ETC1_RGB8_OES                 0x8D64
 GL_COMPRESSED_RGB8_ETC2          0x9274
 GL_COMPRESSED_RGBA8_ETC2_EAC     0x9278

 Formats list
 version 10: format: 0=ETC1_RGB, [1=ETC1_RGBA, 2=ETC1_RGB_MIP, 3=ETC1_RGBA_MIP] (not used)
 version 20: format: 0=ETC1_RGB, 1=ETC2_RGB, 2=ETC2_RGBA_OLD, 3=ETC2_RGBA, 4=ETC2_RGBA1, 5=ETC2_R, 6=ETC2_RG, 7=ETC2_SIGNED_R, 8=ETC2_SIGNED_R

 NOTE: The extended width and height are the widths rounded up to a multiple of 4.
 NOTE: ETC is always 4bit per pixel (64 bit for each 4x4 block of pixels)
*/

// PKM file (ETC1) Header (16 bytes)
typedef struct rf_pkm_header rf_pkm_header;
struct rf_pkm_header
{
    char id[4];                 // "PKM "
    char version[2];            // "10" or "20"
    unsigned short format;      // Data format (big-endian) (Check list below)
    unsigned short width;       // Texture width (big-endian) (origWidth rounded to multiple of 4)
    unsigned short height;      // Texture height (big-endian) (origHeight rounded to multiple of 4)
    unsigned short orig_width;  // Original width (big-endian)
    unsigned short orig_height; // Original height (big-endian)
};

rf_public rf_int rf_get_pkm_image_size(const void* src, rf_int src_size)
{
    int result = 0;

    if (src && src_size > sizeof(rf_pkm_header))
    {
        rf_pkm_header header = *(rf_pkm_header*)src;

        // Verify the type of file
        if (rf_match_str_cstr(header.id, sizeof(header.id), "PKM "))
        {
            // Note: format, width and height come as big-endian, data must be swapped to little-endian
            header.format = ((header.format & 0x00FF) << 8) | ((header.format & 0xFF00) >> 8);
            header.width  = ((header.width  & 0x00FF) << 8) | ((header.width  & 0xFF00) >> 8);
            header.height = ((header.height & 0x00FF) << 8) | ((header.height & 0xFF00) >> 8);

            int bpp = 4;
            if (header.format == 3) bpp = 8;

            result = header.width * header.height * bpp / 8;
        }
        else rf_log_error(rf_bad_format, "PKM file does not seem to be a valid image");
    }

    return result;
}

// Load image from .pkm
rf_public rf_image rf_load_pkm_image_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size)
{
    rf_image result = {0};

    if (src && src_size >= sizeof(rf_pkm_header))
    {
        rf_pkm_header header = *(rf_pkm_header*)src;

        src = (char*)src + sizeof(rf_pkm_header);
        src_size -= sizeof(rf_pkm_header);

        // Verify the type of file
        if (rf_match_str_cstr(header.id, sizeof(header.id), "PKM "))
        {
            // Note: format, width and height come as big-endian, data must be swapped to little-endian
            result.format = ((header.format & 0x00FF) << 8) | ((header.format & 0xFF00) >> 8);
            result.width  = ((header.width  & 0x00FF) << 8) | ((header.width  & 0xFF00) >> 8);
            result.height = ((header.height & 0x00FF) << 8) | ((header.height & 0xFF00) >> 8);

            int bpp = (result.format == 3) ? 8 : 4;
            int size = result.width * result.height * bpp / 8;  // Total data size in bytes

            if (dst_size >= size && src_size >= size)
            {
                memcpy(dst, src, size);

                     if (header.format == 0) result.format = rf_pixel_format_etc1_rgb;
                else if (header.format == 1) result.format = rf_pixel_format_etc2_rgb;
                else if (header.format == 3) result.format = rf_pixel_format_etc2_eac_rgba;

                result.valid = 1;
            }
        }
        else rf_log_error(rf_bad_format, "PKM file does not seem to be a valid image");
    }

    return result;
}

rf_public rf_image rf_load_pkm_image(const void* src, rf_int src_size, rf_allocator allocator)
{
    rf_image result = {0};

    if (src && src_size > 0)
    {
        int dst_size = rf_get_pkm_image_size(src, src_size);
        void* dst = rf_alloc(allocator, dst_size);

        result = rf_load_pkm_image_to_buffer(src, src_size, dst, dst_size);
    }

    return result;
}

rf_public rf_image rf_load_pkm_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_image result = {0};

    int src_size = rf_file_size(io, file);
    void* src = rf_alloc(temp_allocator, src_size);

    if (rf_read_file(io, file, src, src_size))
    {
        result = rf_load_pkm_image(src, src_size, allocator);
    }

    rf_free(temp_allocator, src);

    return result;
}

#pragma endregion

#pragma region ktx

/*
 Required extensions:
 GL_OES_compressed_ETC1_RGB8_texture  (ETC1)
 GL_ARB_ES3_compatibility  (ETC2/EAC)

 Supported tokens (defined by extensions)
 GL_ETC1_RGB8_OES                 0x8D64
 GL_COMPRESSED_RGB8_ETC2          0x9274
 GL_COMPRESSED_RGBA8_ETC2_EAC     0x9278

 KTX file Header (64 bytes)
 v1.1 - https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
 v2.0 - http://github.khronos.org/KTX-Specification/

 NOTE: Before start of every mipmap data block, we have: unsigned int dataSize
 TODO: Support KTX 2.2 specs!
*/

typedef struct rf_ktx_header rf_ktx_header;
struct rf_ktx_header
{
    char id[12];                          // Identifier: "«KTX 11»\r\n\x1A\n"
    unsigned int endianness;              // Little endian: 0x01 0x02 0x03 0x04
    unsigned int gl_type;                 // For compressed textures, gl_type must equal 0
    unsigned int gl_type_size;            // For compressed texture data, usually 1
    unsigned int gl_format;               // For compressed textures is 0
    unsigned int gl_internal_format;      // Compressed internal format
    unsigned int gl_base_internal_format; // Same as gl_format (RGB, RGBA, ALPHA...)
    unsigned int width;                   // Texture image width in pixels
    unsigned int height;                  // Texture image height in pixels
    unsigned int depth;                   // For 2D textures is 0
    unsigned int elements;                // Number of array elements, usually 0
    unsigned int faces;                   // Cubemap faces, for no-cubemap = 1
    unsigned int mipmap_levels;           // Non-mipmapped textures = 1
    unsigned int key_value_data_size;     // Used to encode any arbitrary data...
};

rf_public rf_int rf_get_ktx_image_size(const void* src, rf_int src_size)
{
    int result = 0;

    if (src && src_size >= sizeof(rf_ktx_header))
    {
        rf_ktx_header header = *(rf_ktx_header*)src;
        src = (char*)src + sizeof(rf_ktx_header) + header.key_value_data_size;
        src_size -= sizeof(rf_ktx_header) + header.key_value_data_size;

        if (rf_match_str_cstr(header.id + 1, 6, "KTX 11"))
        {
            if (src_size > sizeof(unsigned int))
            {
                memcpy(&result, src, sizeof(unsigned int));
            }
        }
    }

    return result;
}

rf_public rf_mipmaps_image rf_load_ktx_image_to_buffer(const void* src, rf_int src_size, void* dst, rf_int dst_size)
{
    rf_mipmaps_image result = {0};

    if (src && src_size > sizeof(rf_ktx_header))
    {
        rf_ktx_header header = *(rf_ktx_header*)src;
        src = (char*)src + sizeof(rf_ktx_header) + header.key_value_data_size;
        src_size -= sizeof(rf_ktx_header) + header.key_value_data_size;

        if (rf_match_str_cstr(header.id + 1, 6, "KTX 11"))
        {
            result.width = header.width;
            result.height = header.height;
            result.mipmaps = header.mipmap_levels;

            int image_size = 0;
            if (src_size > sizeof(unsigned int))
            {
                memcpy(&image_size, src, sizeof(unsigned int));
                src = (char*)src + sizeof(unsigned int);
                src_size -= sizeof(unsigned int);

                if (image_size >= src_size && dst_size >= image_size)
                {
                    memcpy(dst, src, image_size);
                    result.data = dst;

                    switch (header.gl_internal_format)
                    {
                        case 0x8D64: result.format = rf_pixel_format_etc1_rgb; break;
                        case 0x9274: result.format = rf_pixel_format_etc2_rgb; break;
                        case 0x9278: result.format = rf_pixel_format_etc2_eac_rgba; break;
                        default: return result;
                    }

                    result.valid = 1;
                }
            }
        }
    }

    return result;
}

rf_public rf_mipmaps_image rf_load_ktx_image(const void* src, rf_int src_size, rf_allocator allocator)
{
    rf_mipmaps_image result = {0};

    if (src && src_size > 0)
    {
        int dst_size = rf_get_ktx_image_size(src, src_size);
        void* dst    = rf_alloc(allocator, dst_size);

        result = rf_load_ktx_image_to_buffer(src, src_size, dst, dst_size);
    }

    return result;
}

rf_public rf_mipmaps_image rf_load_ktx_image_from_file(const char* file, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_mipmaps_image result = {0};

    int src_size = rf_file_size(io, file);
    void* src = rf_alloc(temp_allocator, src_size);

    if (rf_read_file(io, file, src, src_size))
    {
        result = rf_load_ktx_image(src, src_size, allocator);
    }

    rf_free(temp_allocator, src);

    return result;
}

#pragma endregion

#pragma region gif

// Load animated GIF data
//  - rf_image.data buffer includes all frames: [image#0][image#1][image#2][...]
//  - Number of frames is returned through 'frames' parameter
//  - Frames delay is returned through 'delays' parameter (int array)
//  - All frames are returned in RGBA format
rf_public rf_gif rf_load_animated_gif(const void* data, rf_int data_size, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_gif gif = {0};

    rf_set_global_dependencies_allocator(temp_allocator);
    {
        int component_count = 0;
        void* loaded_gif = stbi_load_gif_from_memory(data, data_size, &gif.frame_delays, &gif.width, &gif.height, &gif.frames_count, &component_count, 4);

        if (loaded_gif && component_count == 4)
        {
            int loaded_gif_size = gif.width * gif.height * rf_bytes_per_pixel(rf_pixel_format_r8g8b8a8);
            void* dst = rf_alloc(allocator, loaded_gif_size);

            if (dst)
            {
                memcpy(dst, loaded_gif, loaded_gif_size);

                gif.data   = dst;
                gif.format = rf_pixel_format_r8g8b8a8;
                gif.valid  = 1;
            }
        }

        rf_free(temp_allocator, loaded_gif);
    }
    rf_set_global_dependencies_allocator((rf_allocator) {0});

    return gif;
}

rf_public rf_gif rf_load_animated_gif_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_gif result = (rf_gif) {0};

    int file_size = rf_file_size(io, filename);
    unsigned char* buffer = rf_alloc(temp_allocator, file_size);

    if (rf_read_file(io, filename, buffer, file_size))
    {
        result = rf_load_animated_gif(buffer, file_size, allocator, temp_allocator);
    }

    rf_free(temp_allocator, buffer);

    return result;
}

rf_public rf_sizei rf_gif_frame_size(rf_gif gif)
{
    rf_sizei result = {0};

    if (gif.valid)
    {
        result = (rf_sizei) { gif.width / gif.frames_count, gif.height / gif.frames_count };
    }

    return result;
}

// Returns an image pointing to the frame in the gif
rf_public rf_image rf_get_frame_from_gif(rf_gif gif, int frame)
{
    rf_image result = {0};

    if (gif.valid)
    {
        rf_sizei size = rf_gif_frame_size(gif);

        result = (rf_image)
        {
            .data   = ((unsigned char*)gif.data) + (size.width * size.height * rf_bytes_per_pixel(gif.format)) * frame,
            .width  = size.width,
            .height = size.height,
            .format = gif.format,
            .valid  = 1,
        };
    }

    return result;
}

rf_public void rf_unload_gif(rf_gif gif, rf_allocator allocator)
{
    if (gif.valid)
    {
        rf_free(allocator, gif.frame_delays);
        rf_unload_image(gif.image, allocator);
    }
}

#pragma endregion

#pragma region ez
#ifdef RAYFORK_EZ

#pragma region extract image data functions
rf_public rf_color* rf_image_pixels_to_rgba32_ez(rf_image image) { return rf_image_pixels_to_rgba32(image, rf_default_allocator); }
rf_public rf_vec4* rf_image_compute_pixels_to_normalized_ez(rf_image image) { return rf_image_compute_pixels_to_normalized(image, rf_default_allocator); }
rf_public rf_palette rf_image_extract_palette_ez(rf_image image, int palette_size) { return rf_image_extract_palette(image, palette_size, rf_default_allocator); }
#pragma endregion

#pragma region loading & unloading functions
rf_public rf_image rf_load_image_from_file_data_ez(const void* src, int src_size) { return rf_load_image_from_file_data(src, src_size, rf_default_allocator, rf_default_allocator); }
rf_public rf_image rf_load_image_from_hdr_file_data_ez(const void* src, int src_size) { return rf_load_image_from_hdr_file_data(src, src_size, rf_default_allocator, rf_default_allocator); }
rf_public rf_image rf_load_image_from_file_ez(const char* filename) { return rf_load_image_from_file(filename, rf_default_allocator, rf_default_allocator, rf_default_io); }
rf_public void rf_unload_image_ez(rf_image image) { rf_unload_image(image, rf_default_allocator); }
#pragma endregion

#pragma region image manipulation
rf_public rf_image rf_image_copy_ez(rf_image image) { return rf_image_copy(image, rf_default_allocator); }

rf_public rf_image rf_image_crop_ez(rf_image image, rf_rec crop) { return rf_image_crop(image, crop, rf_default_allocator); }

rf_public rf_image rf_image_resize_ez(rf_image image, int new_width, int new_height) { return rf_image_resize(image, new_width, new_height, rf_default_allocator, rf_default_allocator); }
rf_public rf_image rf_image_resize_nn_ez(rf_image image, int new_width, int new_height) { return rf_image_resize_nn(image, new_width, new_height, rf_default_allocator); }

rf_public rf_image rf_image_format_ez(rf_image image, rf_uncompressed_pixel_format new_format) { return rf_image_format(image, new_format, rf_default_allocator); }

rf_public rf_image rf_image_alpha_clear_ez(rf_image image, rf_color color, float threshold) { return rf_image_alpha_clear(image, color, threshold, rf_default_allocator, rf_default_allocator); }
rf_public rf_image rf_image_alpha_premultiply_ez(rf_image image) { return rf_image_alpha_premultiply(image, rf_default_allocator, rf_default_allocator); }
rf_public rf_image rf_image_alpha_crop_ez(rf_image image, float threshold) { return rf_image_alpha_crop(image, threshold, rf_default_allocator); }
rf_public rf_image rf_image_dither_ez(rf_image image, int r_bpp, int g_bpp, int b_bpp, int a_bpp) { return rf_image_dither(image, r_bpp, g_bpp, b_bpp, a_bpp, rf_default_allocator, rf_default_allocator); }

rf_public rf_image rf_image_flip_vertical_ez(rf_image image) { return rf_image_flip_vertical(image, rf_default_allocator); }
rf_public rf_image rf_image_flip_horizontal_ez(rf_image image) { return rf_image_flip_horizontal(image, rf_default_allocator); }

rf_public rf_vec2 rf_get_seed_for_cellular_image_ez(int seeds_per_row, int tile_size, int i) { return rf_get_seed_for_cellular_image(
        seeds_per_row, tile_size, i, rf_default_rand_proc); }

rf_public rf_image rf_gen_image_color_ez(int width, int height, rf_color color) { return rf_gen_image_color(width, height, color, rf_default_allocator); }
rf_public rf_image rf_gen_image_gradient_v_ez(int width, int height, rf_color top, rf_color bottom) { return rf_gen_image_gradient_v(width, height, top, bottom, rf_default_allocator); }
rf_public rf_image rf_gen_image_gradient_h_ez(int width, int height, rf_color left, rf_color right) { return rf_gen_image_gradient_h(width, height, left, right, rf_default_allocator); }
rf_public rf_image rf_gen_image_gradient_radial_ez(int width, int height, float density, rf_color inner, rf_color outer) { return rf_gen_image_gradient_radial(width, height, density, inner, outer, rf_default_allocator); }
rf_public rf_image rf_gen_image_checked_ez(int width, int height, int checks_x, int checks_y, rf_color col1, rf_color col2) { return rf_gen_image_checked(width, height, checks_x, checks_y, col1, col2, rf_default_allocator); }
rf_public rf_image rf_gen_image_white_noise_ez(int width, int height, float factor) { return rf_gen_image_white_noise(
        width, height, factor, rf_default_rand_proc, rf_default_allocator); }
rf_public rf_image rf_gen_image_perlin_noise_ez(int width, int height, int offset_x, int offset_y, float scale) { return rf_gen_image_perlin_noise(width, height, offset_x, offset_y, scale, rf_default_allocator); }
rf_public rf_image rf_gen_image_cellular_ez(int width, int height, int tile_size) { return rf_gen_image_cellular(width,
                                                                                                              height,
                                                                                                              tile_size,
                                                                                                              rf_default_rand_proc,
                                                                                                              rf_default_allocator); }
#pragma endregion

#pragma region mipmaps
rf_public rf_mipmaps_image rf_image_gen_mipmaps_ez(rf_image image, int gen_mipmaps_count) { return rf_image_gen_mipmaps(image, gen_mipmaps_count, rf_default_allocator, rf_default_allocator); }
rf_public void rf_unload_mipmaps_image_ez(rf_mipmaps_image image) { rf_unload_mipmaps_image(image, rf_default_allocator); }
#pragma endregion

#pragma region dds
rf_public rf_mipmaps_image rf_load_dds_image_ez(const void* src, int src_size) { return rf_load_dds_image(src, src_size, rf_default_allocator); }
rf_public rf_mipmaps_image rf_load_dds_image_from_file_ez(const char* file) { return rf_load_dds_image_from_file(file, rf_default_allocator, rf_default_allocator, rf_default_io); }
#pragma endregion

#pragma region pkm
rf_public rf_image rf_load_pkm_image_ez(const void* src, int src_size) { return rf_load_pkm_image(src, src_size, rf_default_allocator); }
rf_public rf_image rf_load_pkm_image_from_file_ez(const char* file) { return rf_load_pkm_image_from_file(file, rf_default_allocator, rf_default_allocator, rf_default_io); }
#pragma endregion

#pragma region ktx
rf_public rf_mipmaps_image rf_load_ktx_image_ez(const void* src, int src_size) { return rf_load_ktx_image(src, src_size, rf_default_allocator); }
rf_public rf_mipmaps_image rf_load_ktx_image_from_file_ez(const char* file) { return rf_load_ktx_image_from_file(file, rf_default_allocator, rf_default_allocator, rf_default_io); }
#pragma endregion

#endif // RAYFORK_EZ
#pragma endregion



// Load texture from file into GPU memory (VRAM)
rf_public rf_texture2d rf_load_texture_from_file(const char* filename, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_texture2d result = {0};

    rf_image img = rf_load_image_from_file(filename, temp_allocator, temp_allocator, io);

    result = rf_load_texture_from_image(img);

    rf_unload_image(img, temp_allocator);

    return result;
}

// Load texture from an image file data
rf_public rf_texture2d rf_load_texture_from_file_data(const void* data, rf_int dst_size, rf_allocator temp_allocator)
{
    rf_image img = rf_load_image_from_file_data(data, dst_size, RF_ANY_CHANNELS, temp_allocator, temp_allocator);

    rf_texture2d texture = rf_load_texture_from_image(img);

    rf_unload_image(img, temp_allocator);

    return texture;
}

// Load texture from image data
rf_public rf_texture2d rf_load_texture_from_image(rf_image image)
{
    return rf_load_texture_from_image_with_mipmaps((rf_mipmaps_image) {
        .image = image,
        .mipmaps = 1
    });
}

rf_public rf_texture2d rf_load_texture_from_image_with_mipmaps(rf_mipmaps_image image)
{
    rf_texture2d result = {0};

    if (image.valid)
    {
        result.id = rf_gfx_load_texture(image.data, image.width, image.height, image.format, image.mipmaps);

        if (result.id != 0)
        {
            result.width  = image.width;
            result.height = image.height;
            result.format = image.format;
            result.valid  = 1;
        }
    }
    else rf_log(rf_log_type_warning, "rf_texture could not be loaded from rf_image");

    return result;
}

// Load cubemap from image, multiple image cubemap layouts supported
rf_public rf_texture_cubemap rf_load_texture_cubemap_from_image(rf_image image, rf_cubemap_layout_type layout_type, rf_allocator temp_allocator)
{
    rf_texture_cubemap cubemap = {0};

    if (layout_type == RF_CUBEMAP_AUTO_DETECT) // Try to automatically guess layout type
    {
        // Check image width/height to determine the type of cubemap provided
        if (image.width > image.height)
        {
            if ((image.width / 6) == image.height) { layout_type = RF_CUBEMAP_LINE_HORIZONTAL; cubemap.width = image.width / 6; }
            else if ((image.width / 4) == (image.height/3)) { layout_type = RF_CUBEMAP_CROSS_FOUR_BY_TREE; cubemap.width = image.width / 4; }
            else if (image.width >= (int)((float)image.height * 1.85f)) { layout_type = RF_CUBEMAP_PANORAMA; cubemap.width = image.width / 4; }
        }
        else if (image.height > image.width)
        {
            if ((image.height / 6) == image.width) { layout_type = RF_CUBEMAP_LINE_VERTICAL; cubemap.width = image.height / 6; }
            else if ((image.width / 3) == (image.height/4)) { layout_type = RF_CUBEMAP_CROSS_THREE_BY_FOUR; cubemap.width = image.width / 3; }
        }

        cubemap.height = cubemap.width;
    }

    if (layout_type != RF_CUBEMAP_AUTO_DETECT)
    {
        int size = cubemap.width;

        rf_image faces = {0}; // Vertical column image
        rf_rec face_recs[6] = {0}; // Face source rectangles
        for (rf_int i = 0; i < 6; i++) face_recs[i] = (rf_rec) { 0, 0, size, size };

        if (layout_type == RF_CUBEMAP_LINE_VERTICAL)
        {
            faces = image;
            for (rf_int i = 0; i < 6; i++) face_recs[i].y = size*i;
        }
        else if (layout_type == RF_CUBEMAP_PANORAMA)
        {
            // TODO: Convert panorama image to square faces...
            // Ref: https://github.com/denivip/panorama/blob/master/panorama.cpp
        }
        else
        {
            if (layout_type == RF_CUBEMAP_LINE_HORIZONTAL) { for (rf_int i = 0; i < 6; i++) { face_recs[i].x = size * i; } }
            else if (layout_type == RF_CUBEMAP_CROSS_THREE_BY_FOUR)
            {
                face_recs[0].x = size; face_recs[0].y = size;
                face_recs[1].x = size; face_recs[1].y = 3*size;
                face_recs[2].x = size; face_recs[2].y = 0;
                face_recs[3].x = size; face_recs[3].y = 2*size;
                face_recs[4].x = 0; face_recs[4].y = size;
                face_recs[5].x = 2*size; face_recs[5].y = size;
            }
            else if (layout_type == RF_CUBEMAP_CROSS_FOUR_BY_TREE)
            {
                face_recs[0].x = 2*size; face_recs[0].y = size;
                face_recs[1].x = 0; face_recs[1].y = size;
                face_recs[2].x = size; face_recs[2].y = 0;
                face_recs[3].x = size; face_recs[3].y = 2*size;
                face_recs[4].x = size; face_recs[4].y = size;
                face_recs[5].x = 3*size; face_recs[5].y = size;
            }

            // Convert image data to 6 faces in a vertical column, that's the optimum layout for loading
            rf_image faces_colors = rf_gen_image_color(size, size * 6, rf_magenta, temp_allocator);
            faces = rf_image_format(faces_colors, image.format, temp_allocator);
            rf_unload_image(faces_colors, temp_allocator);

            // TODO: rf_image formating does not work with compressed textures!
        }

        for (rf_int i = 0; i < 6; i++)
        {
            rf_image_draw(&faces, image, face_recs[i], (rf_rec) {0, size * i, size, size }, rf_white, temp_allocator);
        }

        cubemap.id = rf_gfx_load_texture_cubemap(faces.data, size, faces.format);

        if (cubemap.id == 0) { rf_log(rf_log_type_warning, "Cubemap image could not be loaded."); }

        rf_unload_image(faces, temp_allocator);
    }
    else rf_log(rf_log_type_warning, "Cubemap image layout can not be detected.");

    return cubemap;
}

// Load texture for rendering (framebuffer)
rf_public rf_render_texture2d rf_load_render_texture(int width, int height)
{
    rf_render_texture2d target = rf_gfx_load_render_texture(width, height, rf_pixel_format_r8g8b8a8, 24, 0);

    return target;
}

// Update GPU texture with new data. Pixels data must match texture.format
rf_public void rf_update_texture(rf_texture2d texture, const void* pixels, rf_int pixels_size)
{
    rf_gfx_update_texture(texture.id, texture.width, texture.height, texture.format, pixels, pixels_size);
}

// Generate GPU mipmaps for a texture
rf_public void rf_gen_texture_mipmaps(rf_texture2d* texture)
{
    // NOTE: NPOT textures support check inside function
    // On WebGL (OpenGL ES 2.0) NPOT textures support is limited
    rf_gfx_generate_mipmaps(texture);
}

// Set texture wrapping mode
rf_public void rf_set_texture_wrap(rf_texture2d texture, rf_texture_wrap_mode wrap_mode)
{
    rf_gfx_set_texture_wrap(texture, wrap_mode);
}

// Set texture scaling filter mode
rf_public void rf_set_texture_filter(rf_texture2d texture, rf_texture_filter_mode filter_mode)
{
    rf_gfx_set_texture_filter(texture, filter_mode);
}

// Unload texture from GPU memory (VRAM)
rf_public void rf_unload_texture(rf_texture2d texture)
{
    if (texture.id > 0)
    {
        rf_gfx_delete_textures(texture.id);

        rf_log(rf_log_type_info, "[TEX ID %i] Unloaded texture data from VRAM (GPU)", texture.id);
    }
}

// Unload render texture from GPU memory (VRAM)
rf_public void rf_unload_render_texture(rf_render_texture2d target)
{
    if (target.id > 0)
    {
        rf_gfx_delete_render_textures(target);

        rf_log(rf_log_type_info, "[TEX ID %i] Unloaded render texture data from VRAM (GPU)", target.id);
    }
}



#pragma region dependencies

#pragma region stb_truetype
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_malloc(sz, u) rf_alloc(rf__global_allocator_for_dependencies, sz)
#define STBTT_free(p, u)    rf_free(rf__global_allocator_for_dependencies, p)
#define STBTT_assert(it)    rf_assert(it)
#define STBTT_STATIC
#include "stb_truetype.h"
#pragma endregion

#pragma endregion

#pragma region ttf font

rf_public rf_ttf_font_info rf_parse_ttf_font(const void* ttf_data, rf_int font_size)
{
    rf_ttf_font_info result = {0};

    if (ttf_data && font_size > 0)
    {
        stbtt_fontinfo font_info = {0};
        if (stbtt_InitFont(&font_info, ttf_data, 0))
        {
            // Calculate font scale factor
            float scale_factor = stbtt_ScaleForPixelHeight(&font_info, (float)font_size);

            // Calculate font basic metrics
            // NOTE: ascent is equivalent to font baseline
            int ascent, descent, line_gap;
            stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);

            result = (rf_ttf_font_info)
            {
                .ttf_data = ttf_data,
                .font_size = font_size,
                .scale_factor = scale_factor,
                .ascent = ascent,
                .descent = descent,
                .line_gap = line_gap,
                .valid = 1,
            };

            rf_assert(sizeof(stbtt_fontinfo) == sizeof(result.internal_stb_font_info));
            memcpy(&result.internal_stb_font_info, &font_info, sizeof(stbtt_fontinfo));
        }
        else rf_log_error(rf_stbtt_failed, "STB failed to parse ttf font.");
    }

    return result;
}

rf_public void rf_compute_ttf_font_glyph_metrics(rf_ttf_font_info* font_info, const int* codepoints, rf_int codepoints_count, rf_glyph_info* dst, rf_int dst_count)
{
    if (font_info && font_info->valid)
    {
        if (dst && dst_count >= codepoints_count)
        {
            // The stbtt functions called here should not require any allocations
            rf_set_global_dependencies_allocator((rf_allocator){0});

            font_info->largest_glyph_size = 0;

            float required_area = 0;

            // NOTE: Using simple packaging, one char after another
            for (rf_int i = 0; i < codepoints_count; i++)
            {
                stbtt_fontinfo* stbtt_ctx = (stbtt_fontinfo*) &font_info->internal_stb_font_info;

                dst[i].codepoint = codepoints[i];

                int begin_x = 0;
                int begin_y = 0;
                int end_x   = 0;
                int end_y   = 0;
                stbtt_GetCodepointBitmapBox(stbtt_ctx, dst[i].codepoint, font_info->scale_factor, font_info->scale_factor, &begin_x, &begin_y, &end_x, &end_y);

                dst[i].rec.width  = end_x - begin_x;
                dst[i].rec.height = end_y - begin_y;
                dst[i].offset_x   = begin_x;
                dst[i].offset_y   = begin_y;
                dst[i].offset_y   += (int) ((float)font_info->ascent * font_info->scale_factor);

                stbtt_GetCodepointHMetrics(stbtt_ctx, dst[i].codepoint, &dst[i].advance_x, NULL);
                dst[i].advance_x *= font_info->scale_factor;

                const int char_size = dst[i].rec.width * dst[i].rec.height;
                if (char_size > font_info->largest_glyph_size) font_info->largest_glyph_size = char_size;
            }
        }
    }
}

// Note: the atlas is square and this value is the width of the atlas
rf_public int rf_compute_ttf_font_atlas_width(int padding, rf_glyph_info* glyph_metrics, rf_int glyphs_count)
{
    int result = 0;

    for (rf_int i = 0; i < glyphs_count; i++)
    {
        // Calculate the area of all glyphs + padding
        // The padding is applied both on the (left and right) and (top and bottom) of the glyph, which is why we multiply by 2
        result += ((glyph_metrics[i].rec.width + 2 * padding) * (glyph_metrics[i].rec.height + 2 * padding));
    }

    // Calculate the width required for a square atlas containing all glyphs
    result = rf_next_pot(sqrtf(result) * 1.3f);

    return result;
}

rf_public rf_image rf_generate_ttf_font_atlas(rf_ttf_font_info* font_info, int atlas_width, int padding, rf_glyph_info* glyphs, rf_int glyphs_count, rf_font_antialias antialias, unsigned short* dst, rf_int dst_count, rf_allocator temp_allocator)
{
    rf_image result = {0};

    if (font_info && font_info->valid)
    {
        int atlas_pixel_count = atlas_width * atlas_width;

        if (dst && dst_count >= atlas_pixel_count)
        {
            memset(dst, 0, atlas_pixel_count * 2);

            int largest_glyph_size = 0;
            for (rf_int i = 0; i < glyphs_count; i++)
            {
                int area = glyphs[i].rec.width * glyphs[i].rec.height;
                if (area > largest_glyph_size)
                {
                    largest_glyph_size = area;
                }
            }

            // Allocate a grayscale buffer large enough to store the largest glyph
            unsigned char* glyph_buffer = rf_alloc(temp_allocator, largest_glyph_size);

            // Use basic packing algorithm to generate the atlas
            if (glyph_buffer)
            {
                // We update these for every pixel in the loop
                int offset_x = RF_BUILTIN_FONT_PADDING;
                int offset_y = RF_BUILTIN_FONT_PADDING;

                // Set the allocator for stbtt
                rf_set_global_dependencies_allocator(temp_allocator);
                {
                    // Using simple packaging, one char after another
                    for (rf_int i = 0; i < glyphs_count; i++)
                    {
                        // Extract these variables to shorter names
                        stbtt_fontinfo* stbtt_ctx = (stbtt_fontinfo*) &font_info->internal_stb_font_info;

                        // Get glyph bitmap
                        stbtt_MakeCodepointBitmap(stbtt_ctx, glyph_buffer, glyphs[i].rec.width, glyphs[i].rec.height, glyphs[i].rec.width, font_info->scale_factor, font_info->scale_factor, glyphs[i].codepoint);

                        // Copy pixel data from fc.data to atlas
                        for (rf_int y = 0; y < glyphs[i].rec.height; y++)
                        {
                            for (rf_int x = 0; x < glyphs[i].rec.width; x++)
                            {
                                unsigned char glyph_pixel = glyph_buffer[y * ((int)glyphs[i].rec.width) + x];
                                if (antialias == RF_FONT_NO_ANTIALIAS && glyph_pixel > RF_BITMAP_ALPHA_THRESHOLD) glyph_pixel = 0;

                                int dst_index = (offset_y + y) * atlas_width + (offset_x + x);
                                // dst is in RF_UNCOMPRESSED_GRAY_ALPHA which is 2 bytes
                                // for fonts we write the glyph_pixel in the alpha channel which is byte 2
                                ((unsigned char*)(&dst[dst_index]))[0] = 255;
                                ((unsigned char*)(&dst[dst_index]))[1] = glyph_pixel;
                            }
                        }

                        // Fill chars rectangles in atlas info
                        glyphs[i].rec.x = (float)offset_x;
                        glyphs[i].rec.y = (float)offset_y;

                        // Move atlas position X for next character drawing
                        offset_x += (glyphs[i].rec.width + 2 * padding);
                        if (offset_x >= (atlas_width - glyphs[i].rec.width - 2 * padding))
                        {
                            offset_x  = padding;
                            offset_y += font_info->font_size + padding;

                            if (offset_y > (atlas_width - font_info->font_size - padding)) break;
                            //else error dst buffer is not big enough
                        }
                    }
                }
                rf_set_global_dependencies_allocator((rf_allocator){0});

                result.data   = dst;
                result.width  = atlas_width;
                result.height = atlas_width;
                result.format = rf_pixel_format_gray_alpha;
                result.valid  = 1;
            }

            rf_free(temp_allocator, glyph_buffer);
        }
    }

    return result;
}

rf_public rf_font rf_ttf_font_from_atlas(int font_size, rf_image atlas, rf_glyph_info* glyph_metrics, rf_int glyphs_count)
{
    rf_font result = {0};

    rf_texture2d texture = rf_load_texture_from_image(atlas);

    if (texture.valid)
    {
        result = (rf_font)
        {
            .glyphs = glyph_metrics,
            .glyphs_count = glyphs_count,
            .texture = texture,
            .base_size = font_size,
            .valid = 1,
        };
    }

    return result;
}

// Load rf_font from TTF font file with generation parameters
// NOTE: You can pass an array with desired characters, those characters should be available in the font
// if array is NULL, default char set is selected 32..126
rf_public rf_font rf_load_ttf_font_from_data(const void* font_file_data, int font_size, rf_font_antialias antialias, const int* chars, rf_int char_count, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_font result = {0};

    rf_ttf_font_info font_info = rf_parse_ttf_font(font_file_data, font_size);

    // Get the glyph metrics
    rf_glyph_info* glyph_metrics = rf_alloc(allocator, char_count * sizeof(rf_glyph_info));
    rf_compute_ttf_font_glyph_metrics(&font_info, chars, char_count, glyph_metrics, char_count);

    // Build the atlas and font
    int atlas_size = rf_compute_ttf_font_atlas_width(RF_BUILTIN_FONT_PADDING, glyph_metrics, char_count);
    int atlas_pixel_count = atlas_size * atlas_size;
    unsigned short* atlas_buffer = rf_alloc(temp_allocator, atlas_pixel_count * sizeof(unsigned short));
    rf_image atlas = rf_generate_ttf_font_atlas(&font_info, atlas_size, RF_BUILTIN_FONT_PADDING, glyph_metrics, char_count, antialias, atlas_buffer, atlas_pixel_count, temp_allocator);

    // Get the font
    result = rf_ttf_font_from_atlas(font_size, atlas, glyph_metrics, char_count);

    // Free the atlas bitmap
    rf_free(temp_allocator, atlas_buffer);

    return result;
}

// Load rf_font from file into GPU memory (VRAM)
rf_public rf_font rf_load_ttf_font_from_file(const char* filename, int font_size, rf_font_antialias antialias, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_font font = {0};

    if (rf_is_file_extension(filename, ".ttf") || rf_is_file_extension(filename, ".otf"))
    {
        int file_size = rf_file_size(io, filename);
        void* data = rf_alloc(temp_allocator, file_size);

        if (rf_read_file(io, filename, data, file_size))
        {
            font = rf_load_ttf_font_from_data(data, RF_DEFAULT_FONT_SIZE, antialias, (int[]) RF_BUILTIN_FONT_CHARS, RF_BUILTIN_CODEPOINTS_COUNT, allocator, temp_allocator);

            // By default we set point filter (best performance)
            rf_set_texture_filter(font.texture, RF_FILTER_POINT);
        }

        rf_free(temp_allocator, data);
    }

    return font;
}

#pragma endregion

#pragma region image font

rf_public rf_bool rf_compute_glyph_metrics_from_image(rf_image image, rf_color key, const int* codepoints, rf_glyph_info* dst, rf_int codepoints_and_dst_count)
{
    rf_bool result = 0;

    if (image.valid && codepoints && dst && codepoints_and_dst_count > 0)
    {
        const int bpp = rf_bytes_per_pixel(image.format);
        const int image_data_size = rf_image_size(image);

        // This macro uses `bpp` and returns the pixel from the image at the index provided by calling rf_format_one_pixel_to_rgba32.
        #define RF_GET_PIXEL(index) rf_format_one_pixel_to_rgba32((char*)image.data + ((index) * bpp), image.format)

        // Parse image data to get char_spacing and line_spacing
        int char_spacing = 0;
        int line_spacing = 0;
        {
            int x = 0;
            int y = 0;
            for (y = 0; y < image.height; y++)
            {
                rf_color pixel = {0};
                for (x = 0; x < image.width; x++)
                {
                    pixel = RF_GET_PIXEL(y * image.width + x);
                    if (!rf_color_match(pixel, key)) break;
                }

                if (!rf_color_match(pixel, key)) break;
            }
            char_spacing = x;
            line_spacing = y;
        }

        // Compute char height
        int char_height = 0;
        {
            while (!rf_color_match(RF_GET_PIXEL((line_spacing + char_height) * image.width + char_spacing), key))
            {
                char_height++;
            }
        }

        // Check array values to get characters: value, x, y, w, h
        int index         = 0;
        int line_to_read  = 0;
        int x_pos_to_read = char_spacing;

        // Parse image data to get rectangle sizes
        while ((line_spacing + line_to_read * (char_height + line_spacing)) < image.height && index < codepoints_and_dst_count)
        {
            while (x_pos_to_read < image.width && !rf_color_match(RF_GET_PIXEL((line_spacing + (char_height + line_spacing) * line_to_read) * image.width + x_pos_to_read), key))
            {
                int char_width = 0;
                while (!rf_color_match(RF_GET_PIXEL(((line_spacing + (char_height + line_spacing) * line_to_read) * image.width + x_pos_to_read + char_width)), key)) {
                    char_width++;
                }

                dst[index].codepoint  = codepoints[index];
                dst[index].rec.x      = (float) x_pos_to_read;
                dst[index].rec.y      = (float) (line_spacing + line_to_read * (char_height + line_spacing));
                dst[index].rec.width  = (float) char_width;
                dst[index].rec.height = (float) char_height;

                // On image based fonts (XNA style), character offsets and x_advance are not required (set to 0)
                dst[index].offset_x  = 0;
                dst[index].offset_y  = 0;
                dst[index].advance_x = 0;

                index++;

                x_pos_to_read += (char_width + char_spacing);
            }

            line_to_read++;
            x_pos_to_read = char_spacing;
        }

        result = 1;

        #undef RF_GET_PIXEL
    }

    return result;
}

rf_public rf_font rf_load_image_font_from_data(rf_image image, rf_glyph_info* glyphs, rf_int glyphs_count)
{
    rf_font result = {
        .texture      = rf_load_texture_from_image(image),
        .glyphs       = glyphs,
        .glyphs_count = glyphs_count,
    };

    if (image.valid && glyphs && glyphs_count > 0)
    {
        result.base_size = glyphs[0].rec.height;
        result.valid     = 1;
    }

    return result;
}

rf_public rf_font rf_load_image_font(rf_image image, rf_color key, rf_allocator allocator)
{
    rf_font result = {0};

    if (image.valid)
    {
        const int codepoints[]     = RF_BUILTIN_FONT_CHARS;
        const int codepoints_count = RF_BUILTIN_CODEPOINTS_COUNT;

        rf_glyph_info* glyphs = rf_alloc(allocator, codepoints_count * sizeof(rf_glyph_info));

        rf_compute_glyph_metrics_from_image(image, key, codepoints, glyphs, codepoints_count);

        result = rf_load_image_font_from_data(image, glyphs, codepoints_count);
    }

    return result;
}

rf_public rf_font rf_load_image_font_from_file(const char* path, rf_color key, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_font result = {0};

    rf_image image = rf_load_image_from_file(path, temp_allocator, temp_allocator, io);

    result = rf_load_image_font(image, key, allocator);

    rf_unload_image(image, temp_allocator);

    return result;
}

#pragma endregion

// Unload rf_font from GPU memory (VRAM)
rf_public void rf_unload_font(rf_font font, rf_allocator allocator)
{
    if (font.valid)
    {
        rf_unload_texture(font.texture);
        rf_free(allocator, font.glyphs);
    }
}

// Returns index position for a unicode character on spritefont
rf_public rf_glyph_index rf_get_glyph_index(rf_font font, int character)
{
    rf_glyph_index result = RF_GLYPH_NOT_FOUND;

    for (rf_int i = 0; i < font.glyphs_count; i++)
    {
        if (font.glyphs[i].codepoint == character)
        {
            result = i;
            break;
        }
    }

    return result;
}

rf_public int rf_font_height(rf_font font, float font_size)
{
    float scale_factor = font_size / font.base_size;
    return (font.base_size + font.base_size / 2) * scale_factor;
}

rf_public rf_sizef rf_measure_text(rf_font font, const char* text, float font_size, float extra_spacing)
{
    rf_sizef result = rf_measure_string(font, text, strlen(text), font_size, extra_spacing);
    return result;
}

rf_public rf_sizef rf_measure_text_rec(rf_font font, const char* text, rf_rec rec, float font_size, float extra_spacing, rf_bool wrap)
{
    rf_sizef result = rf_measure_string_rec(font, text, strlen(text), rec, font_size, extra_spacing, wrap);
    return result;
}

rf_public rf_sizef rf_measure_string(rf_font font, const char* text, int len, float font_size, float extra_spacing)
{
    rf_sizef result = {0};

    if (font.valid)
    {
        int temp_len = 0; // Used to count longer text line num chars
        int len_counter = 0;

        float text_width = 0.0f;
        float temp_text_width = 0.0f; // Used to count longer text line width

        float text_height  = (float)font.base_size;
        float scale_factor = font_size/(float)font.base_size;

        int letter = 0; // Current character
        int index  = 0; // Index position in sprite font

        for (rf_int i = 0; i < len; i++)
        {
            len_counter++;

            rf_decoded_rune decoded_rune = rf_decode_utf8_char(&text[i], len - i);
            index = rf_get_glyph_index(font, decoded_rune.codepoint);

            // NOTE: normally we exit the decoding sequence as soon as a bad unsigned char is found (and return 0x3f)
            // but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set next = 1
            if (letter == 0x3f) { decoded_rune.bytes_processed = 1; }
            i += decoded_rune.bytes_processed - 1;

            if (letter != '\n')
            {
                if (font.glyphs[index].advance_x != 0) { text_width += font.glyphs[index].advance_x; }
                else { text_width += (font.glyphs[index].rec.width + font.glyphs[index].offset_x); }
            }
            else
            {
                if (temp_text_width < text_width) { temp_text_width = text_width; }

                len_counter = 0;
                text_width = 0;
                text_height += ((float)font.base_size*1.5f); // NOTE: Fixed line spacing of 1.5 lines
            }

            if (temp_len < len_counter) { temp_len = len_counter; }
        }

        if (temp_text_width < text_width) temp_text_width = text_width;

        result.width  = temp_text_width * scale_factor + (float)((temp_len - 1) * extra_spacing); // Adds chars spacing to measure
        result.height = text_height * scale_factor;
    }

    return result;
}

rf_public rf_sizef rf_measure_string_rec(rf_font font, const char* text, int text_len, rf_rec rec, float font_size, float extra_spacing, rf_bool wrap)
{
    rf_sizef result = {0};

    if (font.valid)
    {
        int text_offset_x = 0; // Offset between characters
        int text_offset_y = 0; // Required for line break!
        float scale_factor = 0.0f;

        int letter = 0; // Current character
        int index = 0; // Index position in sprite font

        scale_factor = font_size / font.base_size;

        enum
        {
            MEASURE_WRAP_STATE = 0,
            MEASURE_REGULAR_STATE = 1
        };

        int state = wrap ? MEASURE_WRAP_STATE : MEASURE_REGULAR_STATE;
        int start_line = -1; // Index where to begin drawing (where a line begins)
        int end_line = -1; // Index where to stop drawing (where a line ends)
        int lastk = -1; // Holds last value of the character position

        int max_y   = 0;
        int first_y = 0;
        rf_bool first_y_set = 0;

        for (rf_int i = 0, k = 0; i < text_len; i++, k++)
        {
            int glyph_width = 0;

            rf_decoded_rune decoded_rune = rf_decode_utf8_char(&text[i], text_len - i);
            letter = decoded_rune.codepoint;
            index = rf_get_glyph_index(font, letter);

            // NOTE: normally we exit the decoding sequence as soon as a bad unsigned char is found (and return 0x3f)
            // but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set next = 1
            if (letter == 0x3f) decoded_rune.bytes_processed = 1;
            i += decoded_rune.bytes_processed - 1;

            if (letter != '\n')
            {
                glyph_width = (font.glyphs[index].advance_x == 0) ?
                              (int)(font.glyphs[index].rec.width * scale_factor + extra_spacing) :
                              (int)(font.glyphs[index].advance_x * scale_factor + extra_spacing);
            }

            // NOTE: When word_wrap is ON we first measure how much of the text we can draw before going outside of the rec container
            // We store this info in start_line and end_line, then we change states, draw the text between those two variables
            // and change states again and again recursively until the end of the text (or until we get outside of the container).
            // When word_wrap is OFF we don't need the measure state so we go to the drawing state immediately
            // and begin drawing on the next line before we can get outside the container.
            if (state == MEASURE_WRAP_STATE)
            {
                // TODO: there are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
                // See: http://jkorpela.fi/chars/spaces.html
                if ((letter == ' ') || (letter == '\t') || (letter == '\n')) { end_line = i; }

                if ((text_offset_x + glyph_width + 1) >= rec.width)
                {
                    end_line = (end_line < 1) ? i : end_line;
                    if (i == end_line) { end_line -= decoded_rune.bytes_processed; }
                    if ((start_line + decoded_rune.bytes_processed) == end_line) { end_line = i - decoded_rune.bytes_processed; }
                    state = !state;
                }
                else if ((i + 1) == text_len)
                {
                    end_line = i;
                    state = !state;
                }
                else if (letter == '\n')
                {
                    state = !state;
                }

                if (state == MEASURE_REGULAR_STATE)
                {
                    text_offset_x = 0;
                    i = start_line;
                    glyph_width = 0;

                    // Save character position when we switch states
                    int tmp = lastk;
                    lastk = k - 1;
                    k = tmp;
                }
            }
            else
            {
                if (letter == '\n')
                {
                    if (!wrap)
                    {
                        text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                        text_offset_x = 0;
                    }
                }
                else
                {
                    if (!wrap && (text_offset_x + glyph_width + 1) >= rec.width)
                    {
                        text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                        text_offset_x = 0;
                    }

                    if ((text_offset_y + (int)(font.base_size*scale_factor)) > rec.height) break;

                    // The right side expression is the offset of the latest character plus its width (so the end of the line)
                    // We want the highest value of that expression by the end of the function
                    result.width  = rf_max_f(result.width,  rec.x + text_offset_x - 1 + glyph_width);

                    if (!first_y_set)
                    {
                        first_y = rec.y + text_offset_y;
                        first_y_set = 1;
                    }

                    max_y = rf_max_i(max_y, rec.y + text_offset_y + font.base_size * scale_factor);
                }

                if (wrap && i == end_line)
                {
                    text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                    text_offset_x = 0;
                    start_line = end_line;
                    end_line = -1;
                    glyph_width = 0;
                    k = lastk;
                    state = !state;
                }
            }

            text_offset_x += glyph_width;
        }

        result.height = max_y - first_y;
    }

    return result;
}


#pragma region dependencies

#pragma region par shapes
#define PAR_SHAPES_IMPLEMENTATION
#define PAR_MALLOC(T, N)               ((T*)rf_alloc(rf__global_allocator_for_dependencies, N * sizeof(T)))
#define PAR_CALLOC(T, N)               ((T*) rf_calloc_wrapper(rf__global_allocator_for_dependencies, N, sizeof(T)))
#define PAR_FREE(BUF)                  (rf_free(rf__global_allocator_for_dependencies, BUF))
#define PAR_REALLOC(T, BUF, N, OLD_SZ) ((T*) rf_default_realloc(rf__global_allocator_for_dependencies, BUF, sizeof(T) * (N), (OLD_SZ)))
#define PARDEF                         rf_internal
#include "par_shapes.h"
#pragma endregion

#pragma region tinyobj loader
rf_internal rf_thread_local rf_io_callbacks rf__tinyobj_io;
#define RF_SET_TINYOBJ_ALLOCATOR(allocator) rf__tinyobj_allocator = allocator
#define rf_set_tinyobj_io_callbacks(io) rf__tinyobj_io = io;

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#define TINYOBJ_MALLOC(size)             (rf_alloc(rf__global_allocator_for_dependencies, (size)))
#define TINYOBJ_REALLOC(p, oldsz, newsz) (rf_default_realloc(rf__global_allocator_for_dependencies, (p), (oldsz), (newsz)))
#define TINYOBJ_CALLOC(amount, size)     (rf_calloc_wrapper(rf__global_allocator_for_dependencies, (amount), (size)))
#define TINYOBJ_FREE(p)                  (rf_free(rf__global_allocator_for_dependencies, (p)))
#define TINYOBJDEF                       rf_internal
#include "tinyobjloader.h"

rf_internal void rf_tinyobj_file_reader_callback(const char* filename, char** buf, size_t* len)
{
    if (!filename || !buf || !len) return;

    *len = rf_file_size(rf__tinyobj_io, filename);

    if (*len)
    {
        if (!rf_read_file(rf__tinyobj_io, filename, *buf, *len))
        {
            // On error we set the size of output buffer to 0
            *len = 0;
        }
    }
}
#pragma endregion

#pragma region cgltf
#define CGLTF_IMPLEMENTATION
#define CGLTF_MALLOC(size) rf_alloc(rf__global_allocator_for_dependencies, size)
#define CGLTF_FREE(ptr)    rf_free(rf__global_allocator_for_dependencies, ptr)
#include "cgltf.h"

rf_internal cgltf_result rf_cgltf_io_read(const struct cgltf_memory_options* memory_options, const struct cgltf_file_options* file_options, const char* path, cgltf_size* size, void** data)
{
    ((void) memory_options);
    ((void) file_options);

    cgltf_result result = cgltf_result_file_not_found;
    rf_io_callbacks* io = (rf_io_callbacks*) file_options->user_data;

    int file_size = rf_file_size(*io, path);

    if (file_size > 0)
    {
        void* dst = CGLTF_MALLOC(file_size);

        if (dst == NULL)
        {
            if (rf_read_file(*io, path, data, file_size) && data && size)
            {
                *data = dst;
                *size = file_size;
                result = cgltf_result_success;
            }
            else
            {
                CGLTF_FREE(dst);
                result = cgltf_result_io_error;
            }
        }
        else
        {
            result = cgltf_result_out_of_memory;
        }
    }

    return result;
}

rf_internal void rf_cgltf_io_release(const struct cgltf_memory_options* memory_options, const struct cgltf_file_options* file_options, void* data)
{
    ((void) memory_options);
    ((void) file_options);

    CGLTF_FREE(data);
}
#pragma endregion

#pragma endregion

rf_internal rf_model rf_load_meshes_and_materials_for_model(rf_model model, rf_allocator allocator, rf_allocator temp_allocator)
{
    // Make sure model transform is set to identity matrix!
    model.transform = rf_mat_identity();

    if (model.mesh_count == 0)
    {
        rf_log(rf_log_type_warning, "No meshes can be loaded, default to cube mesh.");

        model.mesh_count = 1;
        model.meshes = (rf_mesh *) rf_alloc(allocator, sizeof(rf_mesh));
        memset(model.meshes, 0, sizeof(rf_mesh));
        model.meshes[0] = rf_gen_mesh_cube(1.0f, 1.0f, 1.0f, allocator, temp_allocator);
    }
    else
    {
        // Upload vertex data to GPU (static mesh)
        for (rf_int i = 0; i < model.mesh_count; i++)
            rf_gfx_load_mesh(&model.meshes[i], false);
    }

    if (model.material_count == 0)
    {
        rf_log(rf_log_type_warning, "No materials can be loaded, default to white material.");

        model.material_count = 1;
        model.materials = (rf_material *) rf_alloc(allocator, sizeof(rf_material));
        memset(model.materials, 0, sizeof(rf_material));
        model.materials[0] = rf_load_default_material(allocator);

        if (model.mesh_material == NULL)
        {
            model.mesh_material = (int *) rf_alloc(allocator, model.mesh_count * sizeof(int));
            memset(model.mesh_material, 0, model.mesh_count * sizeof(int));
        }
    }

    return model;
}

// Compute mesh bounding box limits. Note: min_vertex and max_vertex should be transformed by model transform matrix
rf_public rf_bounding_box rf_mesh_bounding_box(rf_mesh mesh)
{
    // Get min and max vertex to construct bounds (AABB)
    rf_vec3 min_vertex = {0};
    rf_vec3 max_vertex = {0};

    if (mesh.vertices != NULL)
    {
        min_vertex = (rf_vec3){mesh.vertices[0], mesh.vertices[1], mesh.vertices[2] };
        max_vertex = (rf_vec3){mesh.vertices[0], mesh.vertices[1], mesh.vertices[2] };

        for (rf_int i = 1; i < mesh.vertex_count; i++)
        {
            min_vertex = rf_vec3_min(min_vertex, (rf_vec3) {mesh.vertices[i * 3], mesh.vertices[i * 3 + 1],
                                                            mesh.vertices[i * 3 + 2]});
            max_vertex = rf_vec3_max(max_vertex, (rf_vec3) {mesh.vertices[i * 3], mesh.vertices[i * 3 + 1],
                                                            mesh.vertices[i * 3 + 2]});
        }
    }

    // Create the bounding box
    rf_bounding_box box = {0};
    box.min = min_vertex;
    box.max = max_vertex;

    return box;
}

// Compute mesh tangents
// NOTE: To calculate mesh tangents and binormals we need mesh vertex positions and texture coordinates
// Implementation base don: https://answers.unity.com/questions/7789/calculating-tangents-vector4.html
rf_public void rf_mesh_compute_tangents(rf_mesh* mesh, rf_allocator allocator, rf_allocator temp_allocator)
{
    if (mesh->tangents == NULL) mesh->tangents = (float*) rf_alloc(allocator, mesh->vertex_count * 4 * sizeof(float));
    else rf_log(rf_log_type_warning, "rf_mesh tangents already exist");

    rf_vec3* tan1 = (rf_vec3*) rf_alloc(temp_allocator, mesh->vertex_count * sizeof(rf_vec3));
    rf_vec3* tan2 = (rf_vec3*) rf_alloc(temp_allocator, mesh->vertex_count * sizeof(rf_vec3));

    for (rf_int i = 0; i < mesh->vertex_count; i += 3)
    {
        // Get triangle vertices
        rf_vec3 v1 = { mesh->vertices[(i + 0) * 3 + 0], mesh->vertices[(i + 0) * 3 + 1], mesh->vertices[(i + 0) * 3 + 2] };
        rf_vec3 v2 = { mesh->vertices[(i + 1) * 3 + 0], mesh->vertices[(i + 1) * 3 + 1], mesh->vertices[(i + 1) * 3 + 2] };
        rf_vec3 v3 = { mesh->vertices[(i + 2) * 3 + 0], mesh->vertices[(i + 2) * 3 + 1], mesh->vertices[(i + 2) * 3 + 2] };

        // Get triangle texcoords
        rf_vec2 uv1 = { mesh->texcoords[(i + 0) * 2 + 0], mesh->texcoords[(i + 0) * 2 + 1] };
        rf_vec2 uv2 = { mesh->texcoords[(i + 1) * 2 + 0], mesh->texcoords[(i + 1) * 2 + 1] };
        rf_vec2 uv3 = { mesh->texcoords[(i + 2) * 2 + 0], mesh->texcoords[(i + 2) * 2 + 1] };

        float x1 = v2.x - v1.x;
        float y1 = v2.y - v1.y;
        float z1 = v2.z - v1.z;
        float x2 = v3.x - v1.x;
        float y2 = v3.y - v1.y;
        float z2 = v3.z - v1.z;

        float s1 = uv2.x - uv1.x;
        float t1 = uv2.y - uv1.y;
        float s2 = uv3.x - uv1.x;
        float t2 = uv3.y - uv1.y;

        float div = s1 * t2 - s2 * t1;
        float r = (div == 0.0f) ? (0.0f) : (1.0f / div);

        rf_vec3 sdir = {(t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r };
        rf_vec3 tdir = {(s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r };

        tan1[i + 0] = sdir;
        tan1[i + 1] = sdir;
        tan1[i + 2] = sdir;

        tan2[i + 0] = tdir;
        tan2[i + 1] = tdir;
        tan2[i + 2] = tdir;
    }

    // Compute tangents considering normals
    for (rf_int i = 0; i < mesh->vertex_count; ++i)
    {
        rf_vec3 normal = {mesh->normals[i * 3 + 0], mesh->normals[i * 3 + 1], mesh->normals[i * 3 + 2] };
        rf_vec3 tangent = tan1[i];

        // TODO: Review, not sure if tangent computation is right, just used reference proposed maths...
        rf_vec3_ortho_normalize(&normal, &tangent);
        mesh->tangents[i * 4 + 0] = tangent.x;
        mesh->tangents[i * 4 + 1] = tangent.y;
        mesh->tangents[i * 4 + 2] = tangent.z;
        mesh->tangents[i * 4 + 3] = (rf_vec3_dot_product(rf_vec3_cross_product(normal, tangent), tan2[i]) < 0.0f) ? -1.0f : 1.0f;
    }

    rf_free(temp_allocator, tan1);
    rf_free(temp_allocator, tan2);

    // Load a new tangent attributes buffer
    mesh->vbo_id[RF_LOC_VERTEX_TANGENT] = rf_gfx_load_attrib_buffer(mesh->vao_id, RF_LOC_VERTEX_TANGENT, mesh->tangents, mesh->vertex_count * 4 * sizeof(float), false);

    rf_log(rf_log_type_info, "Tangents computed for mesh");
}

// Compute mesh binormals (aka bitangent)
rf_public void rf_mesh_compute_binormals(rf_mesh* mesh)
{
    for (rf_int i = 0; i < mesh->vertex_count; i++)
    {
        rf_vec3 normal = {mesh->normals[i * 3 + 0], mesh->normals[i * 3 + 1], mesh->normals[i * 3 + 2] };
        rf_vec3 tangent = {mesh->tangents[i * 4 + 0], mesh->tangents[i * 4 + 1], mesh->tangents[i * 4 + 2] };
        float tangent_w = mesh->tangents[i * 4 + 3];

        // TODO: Register computed binormal in mesh->binormal?
        // rf_vec3 binormal = rf_vec3_mul(rf_vec3_cross_product(normal, tangent), tangent_w);
    }
}

// Unload mesh from memory (RAM and/or VRAM)
rf_public void rf_unload_mesh(rf_mesh mesh, rf_allocator allocator)
{
    rf_gfx_unload_mesh(mesh);

    rf_free(allocator, mesh.vertices);
    rf_free(allocator, mesh.texcoords);
    rf_free(allocator, mesh.normals);
    rf_free(allocator, mesh.colors);
    rf_free(allocator, mesh.tangents);
    rf_free(allocator, mesh.texcoords2);
    rf_free(allocator, mesh.indices);

    rf_free(allocator, mesh.anim_vertices);
    rf_free(allocator, mesh.anim_normals);
    rf_free(allocator, mesh.bone_weights);
    rf_free(allocator, mesh.bone_ids);
    rf_free(allocator, mesh.vbo_id);
}

rf_public rf_model rf_load_model(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_model model = {0};

    if (rf_is_file_extension(filename, ".obj"))
    {
        model = rf_load_model_from_obj(filename, allocator, temp_allocator, io);
    }

    if (rf_is_file_extension(filename, ".iqm"))
    {
        model = rf_load_model_from_iqm(filename, allocator, temp_allocator, io);
    }

    if (rf_is_file_extension(filename, ".gltf") || rf_is_file_extension(filename, ".glb"))
    {
        model = rf_load_model_from_gltf(filename, allocator, temp_allocator, io);
    }

    // Make sure model transform is set to identity matrix!
    model.transform = rf_mat_identity();
    allocator = allocator;

    if (model.mesh_count == 0)
    {
        rf_log(rf_log_type_warning, "No meshes can be loaded, default to cube mesh. Filename: %s", filename);

        model.mesh_count = 1;
        model.meshes = (rf_mesh*) rf_alloc(allocator, model.mesh_count * sizeof(rf_mesh));
        memset(model.meshes, 0, model.mesh_count * sizeof(rf_mesh));
        model.meshes[0] = rf_gen_mesh_cube(1.0f, 1.0f, 1.0f, allocator, temp_allocator);
    }
    else
    {
        // Upload vertex data to GPU (static mesh)
        for (rf_int i = 0; i < model.mesh_count; i++)
        {
            rf_gfx_load_mesh(&model.meshes[i], false);
        }
    }

    if (model.material_count == 0)
    {
        rf_log(rf_log_type_warning, "No materials can be loaded, default to white material. Filename: %s", filename);

        model.material_count = 1;
        model.materials = (rf_material*) rf_alloc(allocator, model.material_count * sizeof(rf_material));
        memset(model.materials, 0, model.material_count * sizeof(rf_material));
        model.materials[0] = rf_load_default_material(allocator);

        if (model.mesh_material == NULL)
        {
            model.mesh_material = (int*) rf_alloc(allocator, model.mesh_count * sizeof(int));
        }
    }

    return model;
}

// Load OBJ mesh data. Note: This calls into a library to do io, so we need to ask the user for IO callbacks
rf_public rf_model rf_load_model_from_obj(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_model model  = {0};
    allocator = allocator;

    tinyobj_attrib_t attrib     = {0};
    tinyobj_shape_t* meshes     = NULL;
    size_t           mesh_count = 0;

    tinyobj_material_t* materials      = NULL;
    size_t              material_count = 0;

    rf_set_global_dependencies_allocator(temp_allocator); // Set to NULL at the end of the function
    rf_set_tinyobj_io_callbacks(io);
    {
        unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
        int ret            = tinyobj_parse_obj(&attrib, &meshes, (size_t*) &mesh_count, &materials, &material_count, filename, rf_tinyobj_file_reader_callback, flags);

        if (ret != TINYOBJ_SUCCESS)
        {
            rf_log(rf_log_type_warning, "Model data could not be loaded. Filename %s", filename);
        }
        else
        {
            rf_log(rf_log_type_info, "Model data loaded successfully: %i meshes / %i materials, filename: %s", mesh_count, material_count, filename);
        }

        // Init model meshes array
        {
            // TODO: Support multiple meshes... in the meantime, only one mesh is returned
            //model.mesh_count = mesh_count;
            model.mesh_count = 1;
            model.meshes     = (rf_mesh*) rf_alloc(allocator, model.mesh_count * sizeof(rf_mesh));
            memset(model.meshes, 0, model.mesh_count * sizeof(rf_mesh));
        }

        // Init model materials array
        if (material_count > 0)
        {
            model.material_count = material_count;
            model.materials      = (rf_material*) rf_alloc(allocator, model.material_count * sizeof(rf_material));
            memset(model.materials, 0, model.material_count * sizeof(rf_material));
        }

        model.mesh_material = (int*) rf_alloc(allocator, model.mesh_count * sizeof(int));
        memset(model.mesh_material, 0, model.mesh_count * sizeof(int));

        // Init model meshes
        for (rf_int m = 0; m < 1; m++)
        {
            rf_mesh mesh = (rf_mesh)
            {
                .vertex_count   = attrib.num_faces * 3,
                .triangle_count = attrib.num_faces,

                .vertices  = (float*)        rf_alloc(allocator, (attrib.num_faces * 3) * 3 * sizeof(float)),
                .texcoords = (float*)        rf_alloc(allocator, (attrib.num_faces * 3) * 2 * sizeof(float)),
                .normals   = (float*)        rf_alloc(allocator, (attrib.num_faces * 3) * 3 * sizeof(float)),
                .vbo_id    = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int)),
            };

            memset(mesh.vertices,  0, mesh.vertex_count * 3 * sizeof(float));
            memset(mesh.texcoords, 0, mesh.vertex_count * 2 * sizeof(float));
            memset(mesh.normals,   0, mesh.vertex_count * 3 * sizeof(float));
            memset(mesh.vbo_id,    0, RF_MAX_MESH_VBO       * sizeof(unsigned int));

            int vCount  = 0;
            int vtCount = 0;
            int vnCount = 0;

            for (rf_int f = 0; f < attrib.num_faces; f++)
            {
                // Get indices for the face
                tinyobj_vertex_index_t idx0 = attrib.faces[3 * f + 0];
                tinyobj_vertex_index_t idx1 = attrib.faces[3 * f + 1];
                tinyobj_vertex_index_t idx2 = attrib.faces[3 * f + 2];

                // RF_LOG(RF_LOG_TYPE_DEBUG, "Face %i index: v %i/%i/%i . vt %i/%i/%i . vn %i/%i/%i\n", f, idx0.v_idx, idx1.v_idx, idx2.v_idx, idx0.vt_idx, idx1.vt_idx, idx2.vt_idx, idx0.vn_idx, idx1.vn_idx, idx2.vn_idx);

                // Fill vertices buffer (float) using vertex index of the face
                for (rf_int v = 0; v < 3; v++) { mesh.vertices[vCount + v] = attrib.vertices[idx0.v_idx * 3 + v]; }
                vCount +=3;

                for (rf_int v = 0; v < 3; v++) { mesh.vertices[vCount + v] = attrib.vertices[idx1.v_idx * 3 + v]; }
                vCount +=3;

                for (rf_int v = 0; v < 3; v++) { mesh.vertices[vCount + v] = attrib.vertices[idx2.v_idx * 3 + v]; }
                vCount +=3;

                // Fill texcoords buffer (float) using vertex index of the face
                // NOTE: Y-coordinate must be flipped upside-down
                mesh.texcoords[vtCount + 0] = attrib.texcoords[idx0.vt_idx * 2 + 0];
                mesh.texcoords[vtCount + 1] = 1.0f - attrib.texcoords[idx0.vt_idx * 2 + 1]; vtCount += 2;
                mesh.texcoords[vtCount + 0] = attrib.texcoords[idx1.vt_idx * 2 + 0];
                mesh.texcoords[vtCount + 1] = 1.0f - attrib.texcoords[idx1.vt_idx * 2 + 1]; vtCount += 2;
                mesh.texcoords[vtCount + 0] = attrib.texcoords[idx2.vt_idx * 2 + 0];
                mesh.texcoords[vtCount + 1] = 1.0f - attrib.texcoords[idx2.vt_idx * 2 + 1]; vtCount += 2;

                // Fill normals buffer (float) using vertex index of the face
                for (rf_int v = 0; v < 3; v++) { mesh.normals[vnCount + v] = attrib.normals[idx0.vn_idx * 3 + v]; }
                vnCount +=3;

                for (rf_int v = 0; v < 3; v++) { mesh.normals[vnCount + v] = attrib.normals[idx1.vn_idx * 3 + v]; }
                vnCount +=3;

                for (rf_int v = 0; v < 3; v++) { mesh.normals[vnCount + v] = attrib.normals[idx2.vn_idx * 3 + v]; }
                vnCount +=3;
            }

            model.meshes[m] = mesh; // Assign mesh data to model

            // Assign mesh material for current mesh
            model.mesh_material[m] = attrib.material_ids[m];

            // Set unfound materials to default
            if (model.mesh_material[m] == -1) { model.mesh_material[m] = 0; }
        }

        // Init model materials
        for (rf_int m = 0; m < material_count; m++)
        {
            // Init material to default
            // NOTE: Uses default shader, only RF_MAP_DIFFUSE supported
            model.materials[m] = rf_load_default_material(allocator);
            model.materials[m].maps[RF_MAP_DIFFUSE].texture = rf_get_default_texture(); // Get default texture, in case no texture is defined

            if (materials[m].diffuse_texname != NULL)
            {
                model.materials[m].maps[RF_MAP_DIFFUSE].texture = rf_load_texture_from_file(materials[m].diffuse_texname, temp_allocator, io); //char* diffuse_texname; // map_Kd
            }

            model.materials[m].maps[RF_MAP_DIFFUSE].color = (rf_color)
            {
                (float)(materials[m].diffuse[0] * 255.0f),
                (float)(materials[m].diffuse[1] * 255.0f),
                (float)(materials[m].diffuse[2] * 255.0f),
                255
            };

            model.materials[m].maps[RF_MAP_DIFFUSE].value = 0.0f;

            if (materials[m].specular_texname != NULL)
            {
                model.materials[m].maps[RF_MAP_SPECULAR].texture = rf_load_texture_from_file(materials[m].specular_texname, temp_allocator, io); //char* specular_texname; // map_Ks
            }

            model.materials[m].maps[RF_MAP_SPECULAR].color = (rf_color)
            {
                (float)(materials[m].specular[0] * 255.0f),
                (float)(materials[m].specular[1] * 255.0f),
                (float)(materials[m].specular[2] * 255.0f),
                255
            };

            model.materials[m].maps[RF_MAP_SPECULAR].value = 0.0f;

            if (materials[m].bump_texname != NULL)
            {
                model.materials[m].maps[RF_MAP_NORMAL].texture = rf_load_texture_from_file(materials[m].bump_texname, temp_allocator, io); //char* bump_texname; // map_bump, bump
            }

            model.materials[m].maps[RF_MAP_NORMAL].color = rf_white;
            model.materials[m].maps[RF_MAP_NORMAL].value = materials[m].shininess;

            model.materials[m].maps[RF_MAP_EMISSION].color = (rf_color)
            {
                (float)(materials[m].emission[0] * 255.0f),
                (float)(materials[m].emission[1] * 255.0f),
                (float)(materials[m].emission[2] * 255.0f),
                255
            };

            if (materials[m].displacement_texname != NULL)
            {
                model.materials[m].maps[RF_MAP_HEIGHT].texture = rf_load_texture_from_file(materials[m].displacement_texname, temp_allocator, io); //char* displacement_texname; // disp
            }
        }

        tinyobj_attrib_free(&attrib);
        tinyobj_shapes_free(meshes, mesh_count);
        tinyobj_materials_free(materials, material_count);
    }
    rf_set_global_dependencies_allocator((rf_allocator) {0});
    rf_set_tinyobj_io_callbacks((rf_io_callbacks){0});

    // NOTE: At this point we have all model data loaded
    rf_log(rf_log_type_info, "Model loaded successfully in RAM. Filename: %s", filename);

    return rf_load_meshes_and_materials_for_model(model, allocator, temp_allocator);
}

// Load IQM mesh data
rf_public rf_model rf_load_model_from_iqm(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    #pragma region constants
    #define RF_IQM_MAGIC "INTERQUAKEMODEL" // IQM file magic number
    #define RF_IQM_VERSION 2 // only IQM version 2 supported

    #define RF_BONE_NAME_LENGTH 32 // rf_bone_info name string length
    #define RF_MESH_NAME_LENGTH 32 // rf_mesh name string length
    #pragma endregion

    #pragma region IQM file structs
    typedef struct rf_iqm_header rf_iqm_header;
    struct rf_iqm_header
    {
        char         magic[16];
        unsigned int version;
        unsigned int filesize;
        unsigned int flags;
        unsigned int num_text, ofs_text;
        unsigned int num_meshes, ofs_meshes;
        unsigned int num_vertexarrays, num_vertexes, ofs_vertexarrays;
        unsigned int num_triangles, ofs_triangles, ofs_adjacency;
        unsigned int num_joints, ofs_joints;
        unsigned int num_poses, ofs_poses;
        unsigned int num_anims, ofs_anims;
        unsigned int num_frames, num_framechannels, ofs_frames, ofs_bounds;
        unsigned int num_comment, ofs_comment;
        unsigned int num_extensions, ofs_extensions;
    };

    typedef struct rf_iqm_mesh rf_iqm_mesh;
    struct rf_iqm_mesh
    {
        unsigned int name;
        unsigned int material;
        unsigned int first_vertex, num_vertexes;
        unsigned int first_triangle, num_triangles;
    };

    typedef struct rf_iqm_triangle rf_iqm_triangle;
    struct rf_iqm_triangle
    {
        unsigned int vertex[3];
    };

    typedef struct rf_iqm_joint rf_iqm_joint;
    struct rf_iqm_joint
    {
        unsigned int name;
        int          parent;
        float        translate[3], rotate[4], scale[3];
    };

    typedef struct rf_iqm_vertex_array rf_iqm_vertex_array;
    struct rf_iqm_vertex_array
    {
        unsigned int type;
        unsigned int flags;
        unsigned int format;
        unsigned int size;
        unsigned int offset;
    };

    // IQM vertex data types
    typedef enum rf_iqm_vertex_type
    {
        RF_IQM_POSITION     = 0,
        RF_IQM_TEXCOORD     = 1,
        RF_IQM_NORMAL       = 2,
        RF_IQM_TANGENT      = 3,   // Note: Tangents unused by default
        RF_IQM_BLENDINDEXES = 4,
        RF_IQM_BLENDWEIGHTS = 5,
        RF_IQM_COLOR        = 6,   // Note: Vertex colors unused by default
        RF_IQM_CUSTOM       = 0x10 // Note: Custom vertex values unused by default
    }  rf_iqm_vertex_type;
    #pragma endregion

    rf_model model = {0};

    size_t data_size = rf_file_size(io, filename);
    unsigned char* data = (unsigned char*) rf_alloc(temp_allocator, data_size);

    if (rf_read_file(io, filename, data, data_size))
    {
        rf_free(temp_allocator, data);
    }

    rf_iqm_header iqm = *((rf_iqm_header*)data);

    rf_iqm_mesh*          imesh;
    rf_iqm_triangle*      tri;
    rf_iqm_vertex_array*  va;
    rf_iqm_joint*         ijoint;

    float* vertex         = NULL;
    float* normal         = NULL;
    float* text           = NULL;
    char*  blendi         = NULL;
    unsigned char* blendw = NULL;

    if (strncmp(iqm.magic, RF_IQM_MAGIC, sizeof(RF_IQM_MAGIC)))
    {
        rf_log(rf_log_type_warning, "[%s] IQM file does not seem to be valid", filename);
        return model;
    }

    if (iqm.version != RF_IQM_VERSION)
    {
        rf_log(rf_log_type_warning, "[%s] IQM file version is not supported (%i).", filename, iqm.version);
        return model;
    }

    // Meshes data processing
    imesh = (rf_iqm_mesh*) rf_alloc(temp_allocator, sizeof(rf_iqm_mesh) * iqm.num_meshes);
    memcpy(imesh, data + iqm.ofs_meshes, sizeof(rf_iqm_mesh) * iqm.num_meshes);

    model.mesh_count = iqm.num_meshes;
    model.meshes = (rf_mesh*) rf_alloc(allocator, model.mesh_count * sizeof(rf_mesh));

    char name[RF_MESH_NAME_LENGTH] = {0};
    for (rf_int i = 0; i < model.mesh_count; i++)
    {
        memcpy(name, data + (iqm.ofs_text + imesh[i].name), RF_MESH_NAME_LENGTH);

        model.meshes[i] = (rf_mesh) {
            .vertex_count = imesh[i].num_vertexes
        };

        model.meshes[i].vertices = (float*) rf_alloc(allocator, model.meshes[i].vertex_count * 3 * sizeof(float)); // Default vertex positions
        memset(model.meshes[i].vertices, 0, model.meshes[i].vertex_count * 3 * sizeof(float));

        model.meshes[i].normals = (float*) rf_alloc(allocator, model.meshes[i].vertex_count * 3 * sizeof(float)); // Default vertex normals
        memset(model.meshes[i].normals, 0, model.meshes[i].vertex_count * 3 * sizeof(float));

        model.meshes[i].texcoords = (float*) rf_alloc(allocator, model.meshes[i].vertex_count * 2 * sizeof(float)); // Default vertex texcoords
        memset(model.meshes[i].texcoords, 0, model.meshes[i].vertex_count * 2 * sizeof(float));

        model.meshes[i].bone_ids = (int*) rf_alloc(allocator, model.meshes[i].vertex_count * 4 * sizeof(float)); // Up-to 4 bones supported!
        memset(model.meshes[i].bone_ids, 0, model.meshes[i].vertex_count * 4 * sizeof(float));

        model.meshes[i].bone_weights = (float*) rf_alloc(allocator, model.meshes[i].vertex_count * 4 * sizeof(float)); // Up-to 4 bones supported!
        memset(model.meshes[i].bone_weights, 0, model.meshes[i].vertex_count * 4 * sizeof(float));

        model.meshes[i].triangle_count = imesh[i].num_triangles;

        model.meshes[i].indices = (unsigned short*) rf_alloc(allocator, model.meshes[i].triangle_count * 3 * sizeof(unsigned short));
        memset(model.meshes[i].indices, 0, model.meshes[i].triangle_count * 3 * sizeof(unsigned short));

        // Animated verted data, what we actually process for rendering
        // NOTE: Animated vertex should be re-uploaded to GPU (if not using GPU skinning)
        model.meshes[i].anim_vertices = (float*) rf_alloc(allocator, model.meshes[i].vertex_count * 3 * sizeof(float));
        memset(model.meshes[i].anim_vertices, 0, model.meshes[i].vertex_count * 3 * sizeof(float));

        model.meshes[i].anim_normals = (float*) rf_alloc(allocator, model.meshes[i].vertex_count * 3 * sizeof(float));
        memset(model.meshes[i].anim_normals, 0, model.meshes[i].vertex_count * 3 * sizeof(float));

        model.meshes[i].vbo_id = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
        memset(model.meshes[i].vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));
    }

    // Triangles data processing
    tri = (rf_iqm_triangle*) rf_alloc(temp_allocator, iqm.num_triangles * sizeof(rf_iqm_triangle));
    memcpy(tri, data + iqm.ofs_triangles, iqm.num_triangles * sizeof(rf_iqm_triangle));

    for (rf_int m = 0; m < model.mesh_count; m++)
    {
        int tcounter = 0;

        for (rf_int i = imesh[m].first_triangle; i < (imesh[m].first_triangle + imesh[m].num_triangles); i++)
        {
            // IQM triangles are stored counter clockwise, but raylib sets opengl to clockwise drawing, so we swap them around
            model.meshes[m].indices[tcounter + 2] = tri[i].vertex[0] - imesh[m].first_vertex;
            model.meshes[m].indices[tcounter + 1] = tri[i].vertex[1] - imesh[m].first_vertex;
            model.meshes[m].indices[tcounter    ] = tri[i].vertex[2] - imesh[m].first_vertex;
            tcounter += 3;
        }
    }

    // Vertex arrays data processing
    va = (rf_iqm_vertex_array*) rf_alloc(temp_allocator, iqm.num_vertexarrays * sizeof(rf_iqm_vertex_array));
    memcpy(va, data + iqm.ofs_vertexarrays, iqm.num_vertexarrays * sizeof(rf_iqm_vertex_array));

    for (rf_int i = 0; i < iqm.num_vertexarrays; i++)
    {
        switch (va[i].type)
        {
            case RF_IQM_POSITION:
            {
                vertex = (float*) rf_alloc(temp_allocator, iqm.num_vertexes * 3 * sizeof(float));
                memcpy(vertex, data + va[i].offset, iqm.num_vertexes * 3 * sizeof(float));

                for (rf_int m = 0; m < iqm.num_meshes; m++)
                {
                    int vertex_pos_counter = 0;
                    for (rf_int ii = imesh[m].first_vertex * 3; ii < (imesh[m].first_vertex + imesh[m].num_vertexes) * 3; ii++)
                    {
                        model.meshes[m].vertices     [vertex_pos_counter] = vertex[ii];
                        model.meshes[m].anim_vertices[vertex_pos_counter] = vertex[ii];
                        vertex_pos_counter++;
                    }
                }
            } break;

            case RF_IQM_NORMAL:
            {
                normal = (float*) rf_alloc(temp_allocator, iqm.num_vertexes * 3 * sizeof(float));
                memcpy(normal, data + va[i].offset, iqm.num_vertexes * 3 * sizeof(float));

                for (rf_int m = 0; m < iqm.num_meshes; m++)
                {
                    int vertex_pos_counter = 0;
                    for (rf_int ii = imesh[m].first_vertex * 3; ii < (imesh[m].first_vertex + imesh[m].num_vertexes) * 3; ii++)
                    {
                        model.meshes[m].normals     [vertex_pos_counter] = normal[ii];
                        model.meshes[m].anim_normals[vertex_pos_counter] = normal[ii];
                        vertex_pos_counter++;
                    }
                }
            } break;

            case RF_IQM_TEXCOORD:
            {
                text = (float*) rf_alloc(temp_allocator, iqm.num_vertexes * 2 * sizeof(float));
                memcpy(text, data + va[i].offset, iqm.num_vertexes * 2 * sizeof(float));

                for (rf_int m = 0; m < iqm.num_meshes; m++)
                {
                    int vertex_pos_counter = 0;
                    for (rf_int ii = imesh[m].first_vertex * 2; ii < (imesh[m].first_vertex + imesh[m].num_vertexes) * 2; ii++)
                    {
                        model.meshes[m].texcoords[vertex_pos_counter] = text[ii];
                        vertex_pos_counter++;
                    }
                }
            } break;

            case RF_IQM_BLENDINDEXES:
            {
                blendi = (char*) rf_alloc(temp_allocator, iqm.num_vertexes * 4 * sizeof(char));
                memcpy(blendi, data + va[i].offset, iqm.num_vertexes * 4 * sizeof(char));

                for (rf_int m = 0; m < iqm.num_meshes; m++)
                {
                    int bone_counter = 0;
                    for (rf_int ii = imesh[m].first_vertex * 4; ii < (imesh[m].first_vertex + imesh[m].num_vertexes) * 4; ii++)
                    {
                        model.meshes[m].bone_ids[bone_counter] = blendi[ii];
                        bone_counter++;
                    }
                }
            } break;

            case RF_IQM_BLENDWEIGHTS:
            {
                blendw = (unsigned char*) rf_alloc(temp_allocator, iqm.num_vertexes * 4 * sizeof(unsigned char));
                memcpy(blendw, data + va[i].offset, iqm.num_vertexes * 4 * sizeof(unsigned char));

                for (rf_int m = 0; m < iqm.num_meshes; m++)
                {
                    int bone_counter = 0;
                    for (rf_int ii = imesh[m].first_vertex * 4; ii < (imesh[m].first_vertex + imesh[m].num_vertexes) * 4; ii++)
                    {
                        model.meshes[m].bone_weights[bone_counter] = blendw[ii] / 255.0f;
                        bone_counter++;
                    }
                }
            } break;
        }
    }

    // Bones (joints) data processing
    ijoint = (rf_iqm_joint*) rf_alloc(temp_allocator, iqm.num_joints * sizeof(rf_iqm_joint));
    memcpy(ijoint, data + iqm.ofs_joints, iqm.num_joints * sizeof(rf_iqm_joint));

    model.bone_count = iqm.num_joints;
    model.bones      = (rf_bone_info*) rf_alloc(allocator, iqm.num_joints * sizeof(rf_bone_info));
    model.bind_pose  = (rf_transform*) rf_alloc(allocator, iqm.num_joints * sizeof(rf_transform));

    for (rf_int i = 0; i < iqm.num_joints; i++)
    {
        // Bones
        model.bones[i].parent = ijoint[i].parent;
        memcpy(model.bones[i].name, data + iqm.ofs_text + ijoint[i].name, RF_BONE_NAME_LENGTH * sizeof(char));

        // Bind pose (base pose)
        model.bind_pose[i].translation.x = ijoint[i].translate[0];
        model.bind_pose[i].translation.y = ijoint[i].translate[1];
        model.bind_pose[i].translation.z = ijoint[i].translate[2];

        model.bind_pose[i].rotation.x = ijoint[i].rotate[0];
        model.bind_pose[i].rotation.y = ijoint[i].rotate[1];
        model.bind_pose[i].rotation.z = ijoint[i].rotate[2];
        model.bind_pose[i].rotation.w = ijoint[i].rotate[3];

        model.bind_pose[i].scale.x = ijoint[i].scale[0];
        model.bind_pose[i].scale.y = ijoint[i].scale[1];
        model.bind_pose[i].scale.z = ijoint[i].scale[2];
    }

    // Build bind pose from parent joints
    for (rf_int i = 0; i < model.bone_count; i++)
    {
        if (model.bones[i].parent >= 0)
        {
            model.bind_pose[i].rotation    = rf_quaternion_mul(model.bind_pose[model.bones[i].parent].rotation, model.bind_pose[i].rotation);
            model.bind_pose[i].translation = rf_vec3_rotate_by_quaternion(model.bind_pose[i].translation, model.bind_pose[model.bones[i].parent].rotation);
            model.bind_pose[i].translation = rf_vec3_add(model.bind_pose[i].translation, model.bind_pose[model.bones[i].parent].translation);
            model.bind_pose[i].scale       = rf_vec3_mul_v(model.bind_pose[i].scale, model.bind_pose[model.bones[i].parent].scale);
        }
    }

    rf_free(temp_allocator, imesh);
    rf_free(temp_allocator, tri);
    rf_free(temp_allocator, va);
    rf_free(temp_allocator, vertex);
    rf_free(temp_allocator, normal);
    rf_free(temp_allocator, text);
    rf_free(temp_allocator, blendi);
    rf_free(temp_allocator, blendw);
    rf_free(temp_allocator, ijoint);

    return rf_load_meshes_and_materials_for_model(model, allocator, temp_allocator);
}

/***********************************************************************************
    Function based on work by Wilhem Barbier (@wbrbr)

    Features:
      - Supports .gltf and .glb files
      - Supports embedded (base64) or external textures
      - Loads the albedo/diffuse texture (other maps could be added)
      - Supports multiple mesh per model and multiple primitives per model

    Some restrictions (not exhaustive):
      - Triangle-only meshes
      - Not supported node hierarchies or transforms
      - Only loads the diffuse texture... but not too hard to support other maps (normal, roughness/metalness...)
      - Only supports unsigned short indices (no unsigned char/unsigned int)
      - Only supports float for texture coordinates (no unsigned char/unsigned short)
*************************************************************************************/
// Load texture from cgltf_image
rf_internal rf_texture2d rf_load_texture_from_cgltf_image(cgltf_image* image, const char* tex_path, rf_color tint, rf_allocator temp_allocator, rf_io_callbacks io)
{
    rf_texture2d texture = {0};

    if (image->uri)
    {
        if ((strlen(image->uri) > 5) &&
            (image->uri[0] == 'd') &&
            (image->uri[1] == 'a') &&
            (image->uri[2] == 't') &&
            (image->uri[3] == 'a') &&
            (image->uri[4] == ':'))
        {
            // Data URI
            // Format: data:<mediatype>;base64,<data>

            // Find the comma
            int i = 0;
            while ((image->uri[i] != ',') && (image->uri[i] != 0)) i++;

            if (image->uri[i] == 0)
            {
                rf_log(rf_log_type_warning, "CGLTF rf_image: Invalid data URI");
            }
            else
            {
                rf_base64_output data = rf_decode_base64((const unsigned char*)image->uri + i + 1, temp_allocator);

                rf_image rimage = rf_load_image_from_file_data(data.buffer, data.size, 4, temp_allocator, temp_allocator);

                // TODO: Tint shouldn't be applied here!
                rf_image_color_tint(rimage, tint);

                texture = rf_load_texture_from_image(rimage);

                rf_unload_image(rimage, temp_allocator);
                rf_free(temp_allocator, data.buffer);
            }
        }
        else
        {
            char buff[1024] = {0};
            snprintf(buff, 1024, "%s/%s", tex_path, image->uri);
            rf_image rimage = rf_load_image_from_file(buff, temp_allocator, temp_allocator, io);

            // TODO: Tint shouldn't be applied here!
            rf_image_color_tint(rimage, tint);

            texture = rf_load_texture_from_image(rimage);

            rf_unload_image(rimage, temp_allocator);
        }
    }
    else if (image->buffer_view)
    {
        unsigned char* data = (unsigned char*) rf_alloc(temp_allocator, image->buffer_view->size);
        int n = image->buffer_view->offset;
        int stride = image->buffer_view->stride ? image->buffer_view->stride : 1;

        for (rf_int i = 0; i < image->buffer_view->size; i++)
        {
            data[i] = ((unsigned char* )image->buffer_view->buffer->data)[n];
            n += stride;
        }

        rf_image rimage = rf_load_image_from_file_data(data, image->buffer_view->size, 4, temp_allocator, temp_allocator);

        // TODO: Tint shouldn't be applied here!
        rf_image_color_tint(rimage, tint);

        texture = rf_load_texture_from_image(rimage);

        rf_unload_image(rimage, temp_allocator);
        rf_free(temp_allocator, data);
    }
    else
    {
        texture = rf_load_texture_from_image((rf_image) {
            .data = &tint,
            .width = 1,
            .height = 1,
            .format = rf_pixel_format_r8g8b8a8,
            .valid = true
        });
    }

    return texture;
}

// Load model from files (meshes and materials)
rf_public rf_model rf_load_model_from_gltf(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    #define rf_load_accessor(type, nbcomp, acc, dst)\
    { \
        int n = 0; \
        type* buf = (type*) acc->buffer_view->buffer->data + acc->buffer_view->offset / sizeof(type) + acc->offset / sizeof(type); \
        for (rf_int k = 0; k < acc->count; k++) { \
            for (rf_int l = 0; l < nbcomp; l++) { \
                dst[nbcomp * k + l] = buf[n + l]; \
            } \
            n += acc->stride / sizeof(type); \
        } \
    }

    rf_set_global_dependencies_allocator(temp_allocator);
    rf_model model = {0};

    cgltf_options options = {
        cgltf_file_type_invalid,
        .file = {
            .read = &rf_cgltf_io_read,
            .release = &rf_cgltf_io_release,
            .user_data = &io
        }
    };

    int data_size = rf_file_size(io, filename);
    void* data = rf_alloc(temp_allocator, data_size);
    if (!rf_read_file(io, filename, data, data_size))
    {
        rf_free(temp_allocator, data);
        rf_set_global_dependencies_allocator(temp_allocator);
        return model;
    }

    cgltf_data* cgltf_data = NULL;
    cgltf_result result = cgltf_parse(&options, data, data_size, &cgltf_data);

    if (result == cgltf_result_success)
    {
        rf_log(rf_log_type_info, "[%s][%s] rf_model meshes/materials: %i/%i", filename, (cgltf_data->file_type == 2) ? "glb" : "gltf", cgltf_data->meshes_count, cgltf_data->materials_count);

        // Read cgltf_data buffers
        result = cgltf_load_buffers(&options, cgltf_data, filename);
        if (result != cgltf_result_success) {
            rf_log(rf_log_type_info, "[%s][%s] Error loading mesh/material buffers", filename, (cgltf_data->file_type == 2) ? "glb" : "gltf");
        }

        int primitivesCount = 0;

        for (rf_int i = 0; i < cgltf_data->meshes_count; i++)
        {
            primitivesCount += (int)cgltf_data->meshes[i].primitives_count;
        }

        // Process glTF cgltf_data and map to model
        allocator = allocator;
        model.mesh_count = primitivesCount;
        model.material_count = cgltf_data->materials_count + 1;
        model.meshes = (rf_mesh*) rf_alloc(allocator, model.mesh_count * sizeof(rf_mesh));
        model.materials = (rf_material*) rf_alloc(allocator, model.material_count * sizeof(rf_material));
        model.mesh_material = (int*) rf_alloc(allocator, model.mesh_count * sizeof(int));

        memset(model.meshes, 0, model.mesh_count * sizeof(rf_mesh));

        for (rf_int i = 0; i < model.mesh_count; i++)
        {
            model.meshes[i].vbo_id = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
            memset(model.meshes[i].vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));
        }

        //For each material
        for (rf_int i = 0; i < model.material_count - 1; i++)
        {
            model.materials[i] = rf_load_default_material(allocator);
            rf_color tint = (rf_color){ 255, 255, 255, 255 };
            const char* tex_path = rf_get_directory_path_from_file_path(filename);

            //Ensure material follows raylib support for PBR (metallic/roughness flow)
            if (cgltf_data->materials[i].has_pbr_metallic_roughness)
            {
                float roughness = cgltf_data->materials[i].pbr_metallic_roughness.roughness_factor;
                float metallic = cgltf_data->materials[i].pbr_metallic_roughness.metallic_factor;

                // NOTE: rf_material name not used for the moment
                //if (model.materials[i].name && cgltf_data->materials[i].name) strcpy(model.materials[i].name, cgltf_data->materials[i].name);

                // TODO: REview: shouldn't these be *255 ???
                tint.r = (unsigned char)(cgltf_data->materials[i].pbr_metallic_roughness.base_color_factor[0] * 255);
                tint.g = (unsigned char)(cgltf_data->materials[i].pbr_metallic_roughness.base_color_factor[1] * 255);
                tint.b = (unsigned char)(cgltf_data->materials[i].pbr_metallic_roughness.base_color_factor[2] * 255);
                tint.a = (unsigned char)(cgltf_data->materials[i].pbr_metallic_roughness.base_color_factor[3] * 255);

                model.materials[i].maps[RF_MAP_ROUGHNESS].color = tint;

                if (cgltf_data->materials[i].pbr_metallic_roughness.base_color_texture.texture)
                {
                    model.materials[i].maps[RF_MAP_ALBEDO].texture = rf_load_texture_from_cgltf_image(cgltf_data->materials[i].pbr_metallic_roughness.base_color_texture.texture->image, tex_path, tint, temp_allocator, io);
                }

                // NOTE: Tint isn't need for other textures.. pass null or clear?
                // Just set as white, multiplying by white has no effect
                tint = rf_white;

                if (cgltf_data->materials[i].pbr_metallic_roughness.metallic_roughness_texture.texture)
                {
                    model.materials[i].maps[RF_MAP_ROUGHNESS].texture = rf_load_texture_from_cgltf_image(cgltf_data->materials[i].pbr_metallic_roughness.metallic_roughness_texture.texture->image, tex_path, tint, temp_allocator, io);
                }
                model.materials[i].maps[RF_MAP_ROUGHNESS].value = roughness;
                model.materials[i].maps[RF_MAP_METALNESS].value = metallic;

                if (cgltf_data->materials[i].normal_texture.texture)
                {
                    model.materials[i].maps[RF_MAP_NORMAL].texture = rf_load_texture_from_cgltf_image(cgltf_data->materials[i].normal_texture.texture->image, tex_path, tint, temp_allocator, io);
                }

                if (cgltf_data->materials[i].occlusion_texture.texture)
                {
                    model.materials[i].maps[RF_MAP_OCCLUSION].texture = rf_load_texture_from_cgltf_image(cgltf_data->materials[i].occlusion_texture.texture->image, tex_path, tint, temp_allocator, io);
                }
            }
        }

        model.materials[model.material_count - 1] = rf_load_default_material(allocator);

        int primitiveIndex = 0;

        for (rf_int i = 0; i < cgltf_data->meshes_count; i++)
        {
            for (rf_int p = 0; p < cgltf_data->meshes[i].primitives_count; p++)
            {
                for (rf_int j = 0; j < cgltf_data->meshes[i].primitives[p].attributes_count; j++)
                {
                    if (cgltf_data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_position)
                    {
                        cgltf_accessor* acc = cgltf_data->meshes[i].primitives[p].attributes[j].data;
                        model.meshes[primitiveIndex].vertex_count = acc->count;
                        model.meshes[primitiveIndex].vertices = (float*) rf_alloc(allocator, sizeof(float) * model.meshes[primitiveIndex].vertex_count * 3);

                        rf_load_accessor(float, 3, acc, model.meshes[primitiveIndex].vertices)
                    }
                    else if (cgltf_data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_normal)
                    {
                        cgltf_accessor* acc = cgltf_data->meshes[i].primitives[p].attributes[j].data;
                        model.meshes[primitiveIndex].normals = (float*) rf_alloc(allocator, sizeof(float) * acc->count * 3);

                        rf_load_accessor(float, 3, acc, model.meshes[primitiveIndex].normals)
                    }
                    else if (cgltf_data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_texcoord)
                    {
                        cgltf_accessor* acc = cgltf_data->meshes[i].primitives[p].attributes[j].data;

                        if (acc->component_type == cgltf_component_type_r_32f)
                        {
                            model.meshes[primitiveIndex].texcoords = (float*) rf_alloc(allocator, sizeof(float) * acc->count * 2);
                            rf_load_accessor(float, 2, acc, model.meshes[primitiveIndex].texcoords)
                        }
                        else
                        {
                            // TODO: Support normalized unsigned unsigned char/unsigned short texture coordinates
                            rf_log(rf_log_type_warning, "[%s] rf_texture coordinates must be float", filename);
                        }
                    }
                }

                cgltf_accessor* acc = cgltf_data->meshes[i].primitives[p].indices;

                if (acc)
                {
                    if (acc->component_type == cgltf_component_type_r_16u)
                    {
                        model.meshes[primitiveIndex].triangle_count = acc->count/3;
                        model.meshes[primitiveIndex].indices = (unsigned short*) rf_alloc(allocator, sizeof(unsigned short) * model.meshes[primitiveIndex].triangle_count * 3);
                        rf_load_accessor(unsigned short, 1, acc, model.meshes[primitiveIndex].indices)
                    }
                    else
                    {
                        // TODO: Support unsigned unsigned char/unsigned int
                        rf_log(rf_log_type_warning, "[%s] Indices must be unsigned short", filename);
                    }
                }
                else
                {
                    // Unindexed mesh
                    model.meshes[primitiveIndex].triangle_count = model.meshes[primitiveIndex].vertex_count/3;
                }

                if (cgltf_data->meshes[i].primitives[p].material)
                {
                    // Compute the offset
                    model.mesh_material[primitiveIndex] = cgltf_data->meshes[i].primitives[p].material - cgltf_data->materials;
                }
                else
                {
                    model.mesh_material[primitiveIndex] = model.material_count - 1;
                }

                primitiveIndex++;
            }
        }

        cgltf_free(cgltf_data);
    }
    else
    {
        rf_log(rf_log_type_warning, "[%s] glTF cgltf_data could not be loaded", filename);
    }

    rf_set_global_dependencies_allocator((rf_allocator) {0});

    return model;

    #undef rf_load_accessor
}

// Load model from generated mesh. Note: The function takes ownership of the mesh in model.meshes[0]
rf_public rf_model rf_load_model_from_mesh(rf_mesh mesh, rf_allocator allocator)
{
    rf_model model = {0};

    model.transform = rf_mat_identity();

    model.mesh_count = 1;
    model.meshes = (rf_mesh*) rf_alloc(allocator, model.mesh_count * sizeof(rf_mesh));
    memset(model.meshes, 0, model.mesh_count * sizeof(rf_mesh));
    model.meshes[0] = mesh;

    model.material_count = 1;
    model.materials = (rf_material*) rf_alloc(allocator, model.material_count * sizeof(rf_material));
    memset(model.materials, 0, model.material_count * sizeof(rf_material));
    model.materials[0] = rf_load_default_material(allocator);

    model.mesh_material = (int*) rf_alloc(allocator, model.mesh_count * sizeof(int));
    memset(model.mesh_material, 0, model.mesh_count * sizeof(int));
    model.mesh_material[0] = 0; // First material index

    return model;
}

// Get collision info between ray and model
rf_public rf_ray_hit_info rf_collision_ray_model(rf_ray ray, rf_model model)
{
    rf_ray_hit_info result = {0};

    for (rf_int m = 0; m < model.mesh_count; m++)
    {
        // Check if mesh has vertex data on CPU for testing
        if (model.meshes[m].vertices != NULL)
        {
            // model->mesh.triangle_count may not be set, vertex_count is more reliable
            int triangle_count = model.meshes[m].vertex_count / 3;

            // Test against all triangles in mesh
            for (rf_int i = 0; i < triangle_count; i++)
            {
                rf_vec3 a, b, c;
                rf_vec3 *vertdata = (rf_vec3 *) model.meshes[m].vertices;

                if (model.meshes[m].indices)
                {
                    a = vertdata[model.meshes[m].indices[i * 3 + 0]];
                    b = vertdata[model.meshes[m].indices[i * 3 + 1]];
                    c = vertdata[model.meshes[m].indices[i * 3 + 2]];
                } else
                {
                    a = vertdata[i * 3 + 0];
                    b = vertdata[i * 3 + 1];
                    c = vertdata[i * 3 + 2];
                }

                a = rf_vec3_transform(a, model.transform);
                b = rf_vec3_transform(b, model.transform);
                c = rf_vec3_transform(c, model.transform);

                rf_ray_hit_info tri_hit_info = rf_collision_ray_triangle(ray, a, b, c);

                if (tri_hit_info.hit)
                {
                    // Save the closest hit triangle
                    if ((!result.hit) || (result.distance > tri_hit_info.distance)) result = tri_hit_info;
                }
            }
        }
    }

    return result;
}

// Unload model from memory (RAM and/or VRAM)
rf_public void rf_unload_model(rf_model model, rf_allocator allocator)
{
    for (rf_int i = 0; i < model.mesh_count; i++) rf_unload_mesh(model.meshes[i], allocator);

    // As the user could be sharing shaders and textures between models,
    // we don't unload the material but just free it's maps, the user
    // is responsible for freeing models shaders and textures
    for (rf_int i = 0; i < model.material_count; i++) rf_free(allocator, model.materials[i].maps);

    rf_free(allocator, model.meshes);
    rf_free(allocator, model.materials);
    rf_free(allocator, model.mesh_material);

    // Unload animation data
    rf_free(allocator, model.bones);
    rf_free(allocator, model.bind_pose);

    rf_log(rf_log_type_info, "Unloaded model data from RAM and VRAM");
}

#pragma region materials

// Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)
rf_public rf_material rf_load_default_material(rf_allocator allocator)
{
    rf_material material = {0};
    material.maps = (rf_material_map*) rf_alloc(allocator, RF_MAX_MATERIAL_MAPS * sizeof(rf_material_map));
    memset(material.maps, 0, RF_MAX_MATERIAL_MAPS * sizeof(rf_material_map));

    material.shader = rf_get_default_shader();
    material.maps[RF_MAP_DIFFUSE].texture = rf_get_default_texture(); // White texture (1x1 pixel)
    //material.maps[RF_MAP_NORMAL].texture;         // NOTE: By default, not set
    //material.maps[RF_MAP_SPECULAR].texture;       // NOTE: By default, not set

    material.maps[RF_MAP_DIFFUSE].color = rf_white; // Diffuse color
    material.maps[RF_MAP_SPECULAR].color = rf_white; // Specular color

    return material;
}

// TODO: Support IQM and GLTF for materials parsing
// TODO: Process materials to return
// Load materials from model file
rf_public rf_materials_array rf_load_materials_from_mtl(const char* filename, rf_allocator allocator, rf_io_callbacks io)
{
    if (!filename) return (rf_materials_array) {0};

    rf_materials_array result = {0};

    rf_set_global_dependencies_allocator(allocator);
    rf_set_tinyobj_io_callbacks(io);
    {
        size_t size = 0;
        tinyobj_material_t* mats = 0;
        if (tinyobj_parse_mtl_file(&mats, (size_t*) &size, filename, rf_tinyobj_file_reader_callback) != TINYOBJ_SUCCESS)
        {
            // Log Error
        }
        tinyobj_materials_free(mats, result.size);

        result.size = size;
    }
    rf_set_tinyobj_io_callbacks((rf_io_callbacks) {0});
    rf_set_global_dependencies_allocator((rf_allocator) {0});

    // Set materials shader to default (DIFFUSE, SPECULAR, NORMAL)
    for (rf_int i = 0; i < result.size; i++)
    {
        result.materials[i].shader = rf_get_default_shader();
    }

    return result;
}

rf_public void rf_unload_material(rf_material material, rf_allocator allocator)
{
    // Unload material shader (avoid unloading default shader, managed by raylib)
    if (material.shader.id != rf_get_default_shader().id)
    {
        rf_gfx_unload_shader(material.shader);
    }

    // Unload loaded texture maps (avoid unloading default texture, managed by raylib)
    for (rf_int i = 0; i < RF_MAX_MATERIAL_MAPS; i++)
    {
        if (material.maps[i].texture.id != rf_get_default_texture().id)
        {
            rf_gfx_delete_textures(material.maps[i].texture.id);
        }
    }

    rf_free(allocator, material.maps);
}

rf_public void rf_set_material_texture(rf_material* material, rf_material_map_type map_type, rf_texture2d texture); // Set texture for a material map type (rf_map_diffuse, rf_map_specular...)

rf_public void rf_set_model_mesh_material(rf_model* model, int mesh_id, int material_id); // Set material for a mesh

#pragma endregion

#pragma region model animations
rf_public rf_model_animation_array rf_load_model_animations_from_iqm_file(const char* filename, rf_allocator allocator, rf_allocator temp_allocator, rf_io_callbacks io)
{
    int size = rf_file_size(io, filename);
    void* data = rf_alloc(temp_allocator, size);

    rf_model_animation_array result = rf_load_model_animations_from_iqm(data, size, allocator, temp_allocator);

    rf_free(temp_allocator, data);

    return result;
}

rf_public rf_model_animation_array rf_load_model_animations_from_iqm(const unsigned char* data, int data_size, rf_allocator allocator, rf_allocator temp_allocator)
{
    if (!data || !data_size) return (rf_model_animation_array) {0};

    #define RF_IQM_MAGIC "INTERQUAKEMODEL" // IQM file magic number
    #define RF_IQM_VERSION 2 // only IQM version 2 supported

    typedef struct rf_iqm_header rf_iqm_header;
    struct rf_iqm_header
    {
        char magic[16];
        unsigned int version;
        unsigned int filesize;
        unsigned int flags;
        unsigned int num_text, ofs_text;
        unsigned int num_meshes, ofs_meshes;
        unsigned int num_vertexarrays, num_vertexes, ofs_vertexarrays;
        unsigned int num_triangles, ofs_triangles, ofs_adjacency;
        unsigned int num_joints, ofs_joints;
        unsigned int num_poses, ofs_poses;
        unsigned int num_anims, ofs_anims;
        unsigned int num_frames, num_framechannels, ofs_frames, ofs_bounds;
        unsigned int num_comment, ofs_comment;
        unsigned int num_extensions, ofs_extensions;
    };

    typedef struct rf_iqm_pose rf_iqm_pose;
    struct rf_iqm_pose
    {
        int parent;
        unsigned int mask;
        float channeloffset[10];
        float channelscale[10];
    };

    typedef struct rf_iqm_anim rf_iqm_anim;
    struct rf_iqm_anim
    {
        unsigned int name;
        unsigned int first_frame, num_frames;
        float framerate;
        unsigned int flags;
    };

    rf_iqm_header iqm;

    // Read IQM header
    memcpy(&iqm, data, sizeof(rf_iqm_header));

    if (strncmp(iqm.magic, RF_IQM_MAGIC, sizeof(RF_IQM_MAGIC)))
    {
        char temp_str[sizeof(RF_IQM_MAGIC) + 1] = {0};
        memcpy(temp_str, iqm.magic, sizeof(RF_IQM_MAGIC));
        rf_log_error(rf_bad_format, "Magic Number \"%s\"does not match.", temp_str);

        return (rf_model_animation_array){0};
    }

    if (iqm.version != RF_IQM_VERSION)
    {
        rf_log_error(rf_bad_format, "IQM version %i is incorrect.", iqm.version);

        return (rf_model_animation_array){0};
    }

    rf_model_animation_array result = {
        .size = iqm.num_anims,
    };

    // Get bones data
    rf_iqm_pose* poses = (rf_iqm_pose*) rf_alloc(temp_allocator, iqm.num_poses * sizeof(rf_iqm_pose));
    memcpy(poses, data + iqm.ofs_poses, iqm.num_poses * sizeof(rf_iqm_pose));

    // Get animations data
    rf_iqm_anim* anim = (rf_iqm_anim*) rf_alloc(temp_allocator, iqm.num_anims * sizeof(rf_iqm_anim));
    memcpy(anim, data + iqm.ofs_anims, iqm.num_anims * sizeof(rf_iqm_anim));

    rf_model_animation* animations = (rf_model_animation*) rf_alloc(allocator, iqm.num_anims * sizeof(rf_model_animation));

    result.anims       = animations;
    result.size = iqm.num_anims;

    // frameposes
    unsigned short* framedata = (unsigned short*) rf_alloc(temp_allocator, iqm.num_frames * iqm.num_framechannels * sizeof(unsigned short));
    memcpy(framedata, data + iqm.ofs_frames, iqm.num_frames*iqm.num_framechannels * sizeof(unsigned short));

    for (rf_int a = 0; a < iqm.num_anims; a++)
    {
        animations[a].frame_count = anim[a].num_frames;
        animations[a].bone_count  = iqm.num_poses;
        animations[a].bones       = (rf_bone_info*) rf_alloc(allocator, iqm.num_poses * sizeof(rf_bone_info));
        animations[a].frame_poses = (rf_transform**) rf_alloc(allocator, anim[a].num_frames * sizeof(rf_transform*));
        //animations[a].framerate = anim.framerate;     // TODO: Use framerate?

        for (rf_int j = 0; j < iqm.num_poses; j++)
        {
            strcpy(animations[a].bones[j].name, "ANIMJOINTNAME");
            animations[a].bones[j].parent = poses[j].parent;
        }

        for (rf_int j = 0; j < anim[a].num_frames; j++)
        {
            animations[a].frame_poses[j] = (rf_transform*) rf_alloc(allocator, iqm.num_poses * sizeof(rf_transform));
        }

        int dcounter = anim[a].first_frame*iqm.num_framechannels;

        for (rf_int frame = 0; frame < anim[a].num_frames; frame++)
        {
            for (rf_int i = 0; i < iqm.num_poses; i++)
            {
                animations[a].frame_poses[frame][i].translation.x = poses[i].channeloffset[0];

                if (poses[i].mask & 0x01)
                {
                    animations[a].frame_poses[frame][i].translation.x += framedata[dcounter]*poses[i].channelscale[0];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].translation.y = poses[i].channeloffset[1];

                if (poses[i].mask & 0x02)
                {
                    animations[a].frame_poses[frame][i].translation.y += framedata[dcounter]*poses[i].channelscale[1];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].translation.z = poses[i].channeloffset[2];

                if (poses[i].mask & 0x04)
                {
                    animations[a].frame_poses[frame][i].translation.z += framedata[dcounter]*poses[i].channelscale[2];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].rotation.x = poses[i].channeloffset[3];

                if (poses[i].mask & 0x08)
                {
                    animations[a].frame_poses[frame][i].rotation.x += framedata[dcounter]*poses[i].channelscale[3];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].rotation.y = poses[i].channeloffset[4];

                if (poses[i].mask & 0x10)
                {
                    animations[a].frame_poses[frame][i].rotation.y += framedata[dcounter]*poses[i].channelscale[4];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].rotation.z = poses[i].channeloffset[5];

                if (poses[i].mask & 0x20)
                {
                    animations[a].frame_poses[frame][i].rotation.z += framedata[dcounter]*poses[i].channelscale[5];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].rotation.w = poses[i].channeloffset[6];

                if (poses[i].mask & 0x40)
                {
                    animations[a].frame_poses[frame][i].rotation.w += framedata[dcounter]*poses[i].channelscale[6];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].scale.x = poses[i].channeloffset[7];

                if (poses[i].mask & 0x80)
                {
                    animations[a].frame_poses[frame][i].scale.x += framedata[dcounter]*poses[i].channelscale[7];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].scale.y = poses[i].channeloffset[8];

                if (poses[i].mask & 0x100)
                {
                    animations[a].frame_poses[frame][i].scale.y += framedata[dcounter]*poses[i].channelscale[8];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].scale.z = poses[i].channeloffset[9];

                if (poses[i].mask & 0x200)
                {
                    animations[a].frame_poses[frame][i].scale.z += framedata[dcounter]*poses[i].channelscale[9];
                    dcounter++;
                }

                animations[a].frame_poses[frame][i].rotation = rf_quaternion_normalize(animations[a].frame_poses[frame][i].rotation);
            }
        }

        // Build frameposes
        for (rf_int frame = 0; frame < anim[a].num_frames; frame++)
        {
            for (rf_int i = 0; i < animations[a].bone_count; i++)
            {
                if (animations[a].bones[i].parent >= 0)
                {
                    animations[a].frame_poses[frame][i].rotation    = rf_quaternion_mul(animations[a].frame_poses[frame][animations[a].bones[i].parent].rotation, animations[a].frame_poses[frame][i].rotation);
                    animations[a].frame_poses[frame][i].translation = rf_vec3_rotate_by_quaternion(animations[a].frame_poses[frame][i].translation, animations[a].frame_poses[frame][animations[a].bones[i].parent].rotation);
                    animations[a].frame_poses[frame][i].translation = rf_vec3_add(animations[a].frame_poses[frame][i].translation, animations[a].frame_poses[frame][animations[a].bones[i].parent].translation);
                    animations[a].frame_poses[frame][i].scale       = rf_vec3_mul_v(animations[a].frame_poses[frame][i].scale, animations[a].frame_poses[frame][animations[a].bones[i].parent].scale);
                }
            }
        }
    }

    rf_free(temp_allocator, framedata);
    rf_free(temp_allocator, poses);
    rf_free(temp_allocator, anim);

    return result;
}

// Update model animated vertex data (positions and normals) for a given frame
rf_public void rf_update_model_animation(rf_model model, rf_model_animation anim, int frame)
{
    if ((anim.frame_count > 0) && (anim.bones != NULL) && (anim.frame_poses != NULL))
    {
        return;
    }

    if (frame >= anim.frame_count)
    {
        frame = frame%anim.frame_count;
    }

    for (rf_int m = 0; m < model.mesh_count; m++)
    {
        rf_vec3 anim_vertex = {0};
        rf_vec3 anim_normal = {0};

        rf_vec3 in_translation = {0};
        rf_quaternion in_rotation = {0};
        rf_vec3 in_scale = {0};

        rf_vec3 out_translation = {0};
        rf_quaternion out_rotation = {0};
        rf_vec3 out_scale = {0};

        int vertex_pos_counter = 0;
        int bone_counter = 0;
        int bone_id = 0;

        for (rf_int i = 0; i < model.meshes[m].vertex_count; i++)
        {
            bone_id = model.meshes[m].bone_ids[bone_counter];
            in_translation = model.bind_pose[bone_id].translation;
            in_rotation = model.bind_pose[bone_id].rotation;
            in_scale = model.bind_pose[bone_id].scale;
            out_translation = anim.frame_poses[frame][bone_id].translation;
            out_rotation = anim.frame_poses[frame][bone_id].rotation;
            out_scale = anim.frame_poses[frame][bone_id].scale;

            // Vertices processing
            // NOTE: We use meshes.vertices (default vertex position) to calculate meshes.anim_vertices (animated vertex position)
            anim_vertex = (rf_vec3){model.meshes[m].vertices[vertex_pos_counter], model.meshes[m].vertices[vertex_pos_counter + 1], model.meshes[m].vertices[vertex_pos_counter + 2] };
            anim_vertex = rf_vec3_mul_v(anim_vertex, out_scale);
            anim_vertex = rf_vec3_sub(anim_vertex, in_translation);
            anim_vertex = rf_vec3_rotate_by_quaternion(anim_vertex, rf_quaternion_mul(out_rotation, rf_quaternion_invert(in_rotation)));
            anim_vertex = rf_vec3_add(anim_vertex, out_translation);
            model.meshes[m].anim_vertices[vertex_pos_counter] = anim_vertex.x;
            model.meshes[m].anim_vertices[vertex_pos_counter + 1] = anim_vertex.y;
            model.meshes[m].anim_vertices[vertex_pos_counter + 2] = anim_vertex.z;

            // Normals processing
            // NOTE: We use meshes.baseNormals (default normal) to calculate meshes.normals (animated normals)
            anim_normal = (rf_vec3){model.meshes[m].normals[vertex_pos_counter], model.meshes[m].normals[vertex_pos_counter + 1], model.meshes[m].normals[vertex_pos_counter + 2] };
            anim_normal = rf_vec3_rotate_by_quaternion(anim_normal, rf_quaternion_mul(out_rotation, rf_quaternion_invert(in_rotation)));
            model.meshes[m].anim_normals[vertex_pos_counter] = anim_normal.x;
            model.meshes[m].anim_normals[vertex_pos_counter + 1] = anim_normal.y;
            model.meshes[m].anim_normals[vertex_pos_counter + 2] = anim_normal.z;
            vertex_pos_counter += 3;

            bone_counter += 4;
        }

        // Upload new vertex data to GPU for model drawing
        rf_gfx_update_buffer(model.meshes[m].vbo_id[0], model.meshes[m].anim_vertices, model.meshes[m].vertex_count * 3 * sizeof(float)); // Update vertex position
        rf_gfx_update_buffer(model.meshes[m].vbo_id[2], model.meshes[m].anim_vertices, model.meshes[m].vertex_count * 3 * sizeof(float)); // Update vertex normals
    }
}

// Check model animation skeleton match. Only number of bones and parent connections are checked
rf_public rf_bool rf_is_model_animation_valid(rf_model model, rf_model_animation anim)
{
    int result = true;

    if (model.bone_count != anim.bone_count) result = false;
    else
    {
        for (rf_int i = 0; i < model.bone_count; i++)
        {
            if (model.bones[i].parent != anim.bones[i].parent) { result = false; break; }
        }
    }

    return result;
}

// Unload animation data
rf_public void rf_unload_model_animation(rf_model_animation anim, rf_allocator allocator)
{
    for (rf_int i = 0; i < anim.frame_count; i++) rf_free(allocator, anim.frame_poses[i]);

    rf_free(allocator, anim.bones);
    rf_free(allocator, anim.frame_poses);
}
#pragma endregion

#pragma region mesh generation

rf_public rf_mesh rf_gen_mesh_cube(float width, float height, float length, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_mesh mesh = {0};
    mesh.vbo_id = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
    memset(mesh.vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));

    #define rf_custom_mesh_gen_cube //Todo: Investigate this macro
    /*
    Platonic solids:
    par_shapes_mesh* par_shapes_create_tetrahedron();       // 4 sides polyhedron (pyramid)
    par_shapes_mesh* par_shapes_create_cube();              // 6 sides polyhedron (cube)
    par_shapes_mesh* par_shapes_create_octahedron();        // 8 sides polyhedron (dyamond)
    par_shapes_mesh* par_shapes_create_dodecahedron();      // 12 sides polyhedron
    par_shapes_mesh* par_shapes_create_icosahedron();       // 20 sides polyhedron
    */

    // Platonic solid generation: cube (6 sides)
    // NOTE: No normals/texcoords generated by default
    //RF_SET_PARSHAPES_ALLOCATOR(temp_allocator);
    {
        par_shapes_mesh* cube = par_shapes_create_cube();
        cube->tcoords = PAR_MALLOC(float, 2 * cube->npoints);

        for (rf_int i = 0; i < 2 * cube->npoints; i++)
        {
            cube->tcoords[i] = 0.0f;
        }

        par_shapes_scale(cube, width, height, length);
        par_shapes_translate(cube, -width / 2, 0.0f, -length / 2);
        par_shapes_compute_normals(cube);

        mesh.vertices = (float*) rf_alloc(allocator, cube->ntriangles * 3 * 3 * sizeof(float));
        mesh.texcoords = (float*) rf_alloc(allocator, cube->ntriangles * 3 * 2 * sizeof(float));
        mesh.normals = (float*) rf_alloc(allocator, cube->ntriangles * 3 * 3 * sizeof(float));

        mesh.vertex_count = cube->ntriangles * 3;
        mesh.triangle_count = cube->ntriangles;

        for (rf_int k = 0; k < mesh.vertex_count; k++)
        {
            mesh.vertices[k * 3] = cube->points[cube->triangles[k] * 3];
            mesh.vertices[k * 3 + 1] = cube->points[cube->triangles[k] * 3 + 1];
            mesh.vertices[k * 3 + 2] = cube->points[cube->triangles[k] * 3 + 2];

            mesh.normals[k * 3] = cube->normals[cube->triangles[k] * 3];
            mesh.normals[k * 3 + 1] = cube->normals[cube->triangles[k] * 3 + 1];
            mesh.normals[k * 3 + 2] = cube->normals[cube->triangles[k] * 3 + 2];

            mesh.texcoords[k * 2] = cube->tcoords[cube->triangles[k] * 2];
            mesh.texcoords[k * 2 + 1] = cube->tcoords[cube->triangles[k] * 2 + 1];
        }

        par_shapes_free_mesh(cube);
    }
    //RF_SET_PARSHAPES_ALLOCATOR((rf_allocator) {0});

    // Upload vertex data to GPU (static mesh)
    rf_gfx_load_mesh(&mesh, false);

    return mesh;
}

// Generate polygonal mesh
rf_public rf_mesh rf_gen_mesh_poly(int sides, float radius, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_mesh mesh = {0};
    mesh.vbo_id = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
    memset(mesh.vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));
    int vertex_count = sides * 3;

    // Vertices definition
    rf_vec3* vertices = (rf_vec3*) rf_alloc(temp_allocator, vertex_count * sizeof(rf_vec3));
    for (rf_int i = 0, v = 0; i < 360; i += 360/sides, v += 3)
    {
        vertices[v    ] = (rf_vec3){ 0.0f, 0.0f, 0.0f };
        vertices[v + 1] = (rf_vec3) {sinf(rf_deg2rad * i) * radius, 0.0f, cosf(rf_deg2rad * i) * radius };
        vertices[v + 2] = (rf_vec3) {sinf(rf_deg2rad * (i + 360 / sides)) * radius, 0.0f, cosf(rf_deg2rad * (i + 360 / sides)) * radius };
    }

    // Normals definition
    rf_vec3* normals = (rf_vec3*) rf_alloc(temp_allocator, vertex_count * sizeof(rf_vec3));
    for (rf_int n = 0; n < vertex_count; n++) normals[n] = (rf_vec3){0.0f, 1.0f, 0.0f }; // rf_vec3.up;

    // TexCoords definition
    rf_vec2* texcoords = (rf_vec2*) rf_alloc(temp_allocator, vertex_count * sizeof(rf_vec2));
    for (rf_int n = 0; n < vertex_count; n++) texcoords[n] = (rf_vec2) {0.0f, 0.0f };

    mesh.vertex_count = vertex_count;
    mesh.triangle_count = sides;
    mesh.vertices  = (float*) rf_alloc(allocator, mesh.vertex_count * 3 * sizeof(float));
    mesh.texcoords = (float*) rf_alloc(allocator, mesh.vertex_count * 2 * sizeof(float));
    mesh.normals   = (float*) rf_alloc(allocator, mesh.vertex_count * 3 * sizeof(float));

    // rf_mesh vertices position array
    for (rf_int i = 0; i < mesh.vertex_count; i++)
    {
        mesh.vertices[3*i] = vertices[i].x;
        mesh.vertices[3*i + 1] = vertices[i].y;
        mesh.vertices[3*i + 2] = vertices[i].z;
    }

    // rf_mesh texcoords array
    for (rf_int i = 0; i < mesh.vertex_count; i++)
    {
        mesh.texcoords[2*i] = texcoords[i].x;
        mesh.texcoords[2*i + 1] = texcoords[i].y;
    }

    // rf_mesh normals array
    for (rf_int i = 0; i < mesh.vertex_count; i++)
    {
        mesh.normals[3*i] = normals[i].x;
        mesh.normals[3*i + 1] = normals[i].y;
        mesh.normals[3*i + 2] = normals[i].z;
    }

    rf_free(temp_allocator, vertices);
    rf_free(temp_allocator, normals);
    rf_free(temp_allocator, texcoords);

    // Upload vertex data to GPU (static mesh)
    rf_gfx_load_mesh(&mesh, false);

    return mesh;
}

// Generate plane mesh (with subdivisions)
rf_public rf_mesh rf_gen_mesh_plane(float width, float length, int res_x, int res_z, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_mesh mesh = {0};
    mesh.vbo_id = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
    memset(mesh.vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));

    #define rf_custom_mesh_gen_plane //Todo: Investigate this macro

    rf_set_global_dependencies_allocator(temp_allocator);
    {
        par_shapes_mesh* plane = par_shapes_create_plane(res_x, res_z); // No normals/texcoords generated!!!
        par_shapes_scale(plane, width, length, 1.0f);

        float axis[] = { 1, 0, 0 };
        par_shapes_rotate(plane, -rf_pi / 2.0f, axis);
        par_shapes_translate(plane, -width / 2, 0.0f, length / 2);

        mesh.vertices   = (float*) rf_alloc(allocator, plane->ntriangles * 3 * 3 * sizeof(float));
        mesh.texcoords  = (float*) rf_alloc(allocator, plane->ntriangles * 3 * 2 * sizeof(float));
        mesh.normals    = (float*) rf_alloc(allocator, plane->ntriangles * 3 * 3 * sizeof(float));
        mesh.vbo_id     = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
        memset(mesh.vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));

        mesh.vertex_count   = plane->ntriangles * 3;
        mesh.triangle_count = plane->ntriangles;

        for (rf_int k = 0; k < mesh.vertex_count; k++)
        {
            mesh.vertices[k * 3    ] = plane->points[plane->triangles[k] * 3    ];
            mesh.vertices[k * 3 + 1] = plane->points[plane->triangles[k] * 3 + 1];
            mesh.vertices[k * 3 + 2] = plane->points[plane->triangles[k] * 3 + 2];

            mesh.normals[k * 3    ] = plane->normals[plane->triangles[k] * 3    ];
            mesh.normals[k * 3 + 1] = plane->normals[plane->triangles[k] * 3 + 1];
            mesh.normals[k * 3 + 2] = plane->normals[plane->triangles[k] * 3 + 2];

            mesh.texcoords[k * 2    ] = plane->tcoords[plane->triangles[k] * 2    ];
            mesh.texcoords[k * 2 + 1] = plane->tcoords[plane->triangles[k] * 2 + 1];
        }

        par_shapes_free_mesh(plane);
    }
    rf_set_global_dependencies_allocator((rf_allocator) {0});

    // Upload vertex data to GPU (static mesh)
    rf_gfx_load_mesh(&mesh, false);

    return mesh;
}

// Generate sphere mesh (standard sphere)
rf_public rf_mesh rf_gen_mesh_sphere(float radius, int rings, int slices, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_mesh mesh = {0};
    mesh.vbo_id = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
    memset(mesh.vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));

    rf_set_global_dependencies_allocator(temp_allocator);
    {
        par_shapes_mesh* sphere = par_shapes_create_parametric_sphere(slices, rings);
        par_shapes_scale(sphere, radius, radius, radius);
        // NOTE: Soft normals are computed internally

        mesh.vertices  = (float*) rf_alloc(allocator, sphere->ntriangles * 3 * 3 * sizeof(float));
        mesh.texcoords = (float*) rf_alloc(allocator, sphere->ntriangles * 3 * 2 * sizeof(float));
        mesh.normals   = (float*) rf_alloc(allocator, sphere->ntriangles * 3 * 3 * sizeof(float));

        mesh.vertex_count = sphere->ntriangles * 3;
        mesh.triangle_count = sphere->ntriangles;

        for (rf_int k = 0; k < mesh.vertex_count; k++)
        {
            mesh.vertices[k * 3    ] = sphere->points[sphere->triangles[k] * 3];
            mesh.vertices[k * 3 + 1] = sphere->points[sphere->triangles[k] * 3 + 1];
            mesh.vertices[k * 3 + 2] = sphere->points[sphere->triangles[k] * 3 + 2];

            mesh.normals[k * 3    ] = sphere->normals[sphere->triangles[k] * 3];
            mesh.normals[k * 3 + 1] = sphere->normals[sphere->triangles[k] * 3 + 1];
            mesh.normals[k * 3 + 2] = sphere->normals[sphere->triangles[k] * 3 + 2];

            mesh.texcoords[k * 2    ] = sphere->tcoords[sphere->triangles[k] * 2];
            mesh.texcoords[k * 2 + 1] = sphere->tcoords[sphere->triangles[k] * 2 + 1];
        }

        par_shapes_free_mesh(sphere);
    }
    rf_set_global_dependencies_allocator((rf_allocator) {0});

    // Upload vertex data to GPU (static mesh)
    rf_gfx_load_mesh(&mesh, false);

    return mesh;
}

// Generate hemi-sphere mesh (half sphere, no bottom cap)
rf_public rf_mesh rf_gen_mesh_hemi_sphere(float radius, int rings, int slices, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_mesh mesh = {0};
    mesh.vbo_id = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
    memset(mesh.vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));

    rf_set_global_dependencies_allocator(temp_allocator);
    {
        par_shapes_mesh* sphere = par_shapes_create_hemisphere(slices, rings);
        par_shapes_scale(sphere, radius, radius, radius);
        // NOTE: Soft normals are computed internally

        mesh.vertices  = (float*) rf_alloc(allocator, sphere->ntriangles * 3 * 3 * sizeof(float));
        mesh.texcoords = (float*) rf_alloc(allocator, sphere->ntriangles * 3 * 2 * sizeof(float));
        mesh.normals   = (float*) rf_alloc(allocator, sphere->ntriangles * 3 * 3 * sizeof(float));

        mesh.vertex_count   = sphere->ntriangles * 3;
        mesh.triangle_count = sphere->ntriangles;

        for (rf_int k = 0; k < mesh.vertex_count; k++)
        {
            mesh.vertices[k * 3    ] = sphere->points[sphere->triangles[k] * 3];
            mesh.vertices[k * 3 + 1] = sphere->points[sphere->triangles[k] * 3 + 1];
            mesh.vertices[k * 3 + 2] = sphere->points[sphere->triangles[k] * 3 + 2];

            mesh.normals[k * 3    ] = sphere->normals[sphere->triangles[k] * 3];
            mesh.normals[k * 3 + 1] = sphere->normals[sphere->triangles[k] * 3 + 1];
            mesh.normals[k * 3 + 2] = sphere->normals[sphere->triangles[k] * 3 + 2];

            mesh.texcoords[k * 2    ] = sphere->tcoords[sphere->triangles[k] * 2];
            mesh.texcoords[k * 2 + 1] = sphere->tcoords[sphere->triangles[k] * 2 + 1];
        }

        par_shapes_free_mesh(sphere);
    }
    rf_set_global_dependencies_allocator((rf_allocator) {0});

    // Upload vertex data to GPU (static mesh)
    rf_gfx_load_mesh(&mesh, false);

    return mesh;
}

// Generate cylinder mesh
rf_public rf_mesh rf_gen_mesh_cylinder(float radius, float height, int slices, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_mesh mesh = {0};
    mesh.vbo_id = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
    memset(mesh.vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));

    rf_set_global_dependencies_allocator(temp_allocator);
    {
        // Instance a cylinder that sits on the Z=0 plane using the given tessellation
        // levels across the UV domain.  Think of "slices" like a number of pizza
        // slices, and "stacks" like a number of stacked rings.
        // Height and radius are both 1.0, but they can easily be changed with par_shapes_scale
        par_shapes_mesh* cylinder = par_shapes_create_cylinder(slices, 8);
        par_shapes_scale(cylinder, radius, radius, height);
        float axis[] = { 1, 0, 0 };
        par_shapes_rotate(cylinder, -rf_pi / 2.0f, axis);

        // Generate an orientable disk shape (top cap)
        float center[] = { 0, 0, 0 };
        float normal[] = { 0, 0, 1 };
        float normal_minus_1[] = { 0, 0, -1 };
        par_shapes_mesh* cap_top = par_shapes_create_disk(radius, slices, center, normal);
        cap_top->tcoords = PAR_MALLOC(float, 2*cap_top->npoints);
        for (rf_int i = 0; i < 2 * cap_top->npoints; i++)
        {
            cap_top->tcoords[i] = 0.0f;
        }

        par_shapes_rotate(cap_top, -rf_pi / 2.0f, axis);
        par_shapes_translate(cap_top, 0, height, 0);

        // Generate an orientable disk shape (bottom cap)
        par_shapes_mesh* cap_bottom = par_shapes_create_disk(radius, slices, center, normal_minus_1);
        cap_bottom->tcoords = PAR_MALLOC(float, 2*cap_bottom->npoints);
        for (rf_int i = 0; i < 2*cap_bottom->npoints; i++) cap_bottom->tcoords[i] = 0.95f;
        par_shapes_rotate(cap_bottom, rf_pi / 2.0f, axis);

        par_shapes_merge_and_free(cylinder, cap_top);
        par_shapes_merge_and_free(cylinder, cap_bottom);

        mesh.vertices  = (float*) rf_alloc(allocator, cylinder->ntriangles * 3 * 3 * sizeof(float));
        mesh.texcoords = (float*) rf_alloc(allocator, cylinder->ntriangles * 3 * 2 * sizeof(float));
        mesh.normals   = (float*) rf_alloc(allocator, cylinder->ntriangles * 3 * 3 * sizeof(float));

        mesh.vertex_count   = cylinder->ntriangles * 3;
        mesh.triangle_count = cylinder->ntriangles;

        for (rf_int k = 0; k < mesh.vertex_count; k++)
        {
            mesh.vertices[k * 3    ] = cylinder->points[cylinder->triangles[k] * 3    ];
            mesh.vertices[k * 3 + 1] = cylinder->points[cylinder->triangles[k] * 3 + 1];
            mesh.vertices[k * 3 + 2] = cylinder->points[cylinder->triangles[k] * 3 + 2];

            mesh.normals[k * 3    ] = cylinder->normals[cylinder->triangles[k] * 3    ];
            mesh.normals[k * 3 + 1] = cylinder->normals[cylinder->triangles[k] * 3 + 1];
            mesh.normals[k * 3 + 2] = cylinder->normals[cylinder->triangles[k] * 3 + 2];

            mesh.texcoords[k * 2    ] = cylinder->tcoords[cylinder->triangles[k] * 2    ];
            mesh.texcoords[k * 2 + 1] = cylinder->tcoords[cylinder->triangles[k] * 2 + 1];
        }

        par_shapes_free_mesh(cylinder);
    }
    rf_set_global_dependencies_allocator((rf_allocator) {0});

    // Upload vertex data to GPU (static mesh)
    rf_gfx_load_mesh(&mesh, false);

    return mesh;
}

// Generate torus mesh
rf_public rf_mesh rf_gen_mesh_torus(float radius, float size, int rad_seg, int sides, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_mesh mesh = {0};
    mesh.vbo_id = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
    memset(mesh.vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));

    if (radius > 1.0f)      radius = 1.0f;
    else if (radius < 0.1f) radius = 0.1f;

    rf_set_global_dependencies_allocator(temp_allocator);
    {
        // Create a donut that sits on the Z=0 plane with the specified inner radius
        // The outer radius can be controlled with par_shapes_scale
        par_shapes_mesh* torus = par_shapes_create_torus(rad_seg, sides, radius);
        par_shapes_scale(torus, size/2, size/2, size/2);

        mesh.vertices  = (float*) rf_alloc(allocator, torus->ntriangles * 3 * 3 * sizeof(float));
        mesh.texcoords = (float*) rf_alloc(allocator, torus->ntriangles * 3 * 2 * sizeof(float));
        mesh.normals   = (float*) rf_alloc(allocator, torus->ntriangles * 3 * 3 * sizeof(float));

        mesh.vertex_count   = torus->ntriangles * 3;
        mesh.triangle_count = torus->ntriangles;

        for (rf_int k = 0; k < mesh.vertex_count; k++)
        {
            mesh.vertices[k * 3    ] = torus->points[torus->triangles[k] * 3    ];
            mesh.vertices[k * 3 + 1] = torus->points[torus->triangles[k] * 3 + 1];
            mesh.vertices[k * 3 + 2] = torus->points[torus->triangles[k] * 3 + 2];

            mesh.normals[k * 3    ] = torus->normals[torus->triangles[k] * 3    ];
            mesh.normals[k * 3 + 1] = torus->normals[torus->triangles[k] * 3 + 1];
            mesh.normals[k * 3 + 2] = torus->normals[torus->triangles[k] * 3 + 2];

            mesh.texcoords[k * 2    ] = torus->tcoords[torus->triangles[k] * 2    ];
            mesh.texcoords[k * 2 + 1] = torus->tcoords[torus->triangles[k] * 2 + 1];
        }

        par_shapes_free_mesh(torus);
    }
    rf_set_global_dependencies_allocator((rf_allocator) {0});

    // Upload vertex data to GPU (static mesh)
    rf_gfx_load_mesh(&mesh, false);

    return mesh;
}

// Generate trefoil knot mesh
rf_public rf_mesh rf_gen_mesh_knot(float radius, float size, int rad_seg, int sides, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_mesh mesh = {0};
    mesh.vbo_id = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
    memset(mesh.vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));

    if (radius > 3.0f)      radius = 3.0f;
    else if (radius < 0.5f) radius = 0.5f;

    rf_set_global_dependencies_allocator(temp_allocator);
    {
        par_shapes_mesh* knot = par_shapes_create_trefoil_knot(rad_seg, sides, radius);
        par_shapes_scale(knot, size, size, size);

        mesh.vertices  = (float*) rf_alloc(allocator, knot->ntriangles * 3 * 3 * sizeof(float));
        mesh.texcoords = (float*) rf_alloc(allocator, knot->ntriangles * 3 * 2 * sizeof(float));
        mesh.normals   = (float*) rf_alloc(allocator, knot->ntriangles * 3 * 3 * sizeof(float));

        mesh.vertex_count   = knot->ntriangles * 3;
        mesh.triangle_count = knot->ntriangles;

        for (rf_int k = 0; k < mesh.vertex_count; k++)
        {
            mesh.vertices[k * 3    ] = knot->points[knot->triangles[k] * 3    ];
            mesh.vertices[k * 3 + 1] = knot->points[knot->triangles[k] * 3 + 1];
            mesh.vertices[k * 3 + 2] = knot->points[knot->triangles[k] * 3 + 2];

            mesh.normals[k * 3    ] = knot->normals[knot->triangles[k] * 3    ];
            mesh.normals[k * 3 + 1] = knot->normals[knot->triangles[k] * 3 + 1];
            mesh.normals[k * 3 + 2] = knot->normals[knot->triangles[k] * 3 + 2];

            mesh.texcoords[k * 2    ] = knot->tcoords[knot->triangles[k] * 2    ];
            mesh.texcoords[k * 2 + 1] = knot->tcoords[knot->triangles[k] * 2 + 1];
        }

        par_shapes_free_mesh(knot);
    }
    rf_set_global_dependencies_allocator((rf_allocator) {0});

    // Upload vertex data to GPU (static mesh)
    rf_gfx_load_mesh(&mesh, false);

    return mesh;
}

// Generate a mesh from heightmap
// NOTE: Vertex data is uploaded to GPU
rf_public rf_mesh rf_gen_mesh_heightmap(rf_image heightmap, rf_vec3 size, rf_allocator allocator, rf_allocator temp_allocator)
{
#define RF_GRAY_VALUE(c) ((c.r+c.g+c.b)/3)

    rf_mesh mesh = {0};
    mesh.vbo_id = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
    memset(mesh.vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));

    int map_x = heightmap.width;
    int map_z = heightmap.height;

    rf_color* pixels = rf_image_pixels_to_rgba32(heightmap, temp_allocator);

    // NOTE: One vertex per pixel
    mesh.triangle_count = (map_x - 1) * (map_z - 1) * 2; // One quad every four pixels

    mesh.vertex_count = mesh.triangle_count * 3;

    mesh.vertices  = (float*) rf_alloc(allocator, mesh.vertex_count * 3 * sizeof(float));
    mesh.normals   = (float*) rf_alloc(allocator, mesh.vertex_count * 3 * sizeof(float));
    mesh.texcoords = (float*) rf_alloc(allocator, mesh.vertex_count * 2 * sizeof(float));
    mesh.colors    = NULL;

    int vertex_pos_counter      = 0; // Used to count vertices float by float
    int vertex_texcoord_counter = 0; // Used to count texcoords float by float
    int n_counter               = 0; // Used to count normals float by float
    int tris_counter            = 0;

    rf_vec3 scale_factor = { size.x / map_x, size.y / 255.0f, size.z / map_z };

    for (rf_int z = 0; z < map_z-1; z++)
    {
        for (rf_int x = 0; x < map_x-1; x++)
        {
            // Fill vertices array with data
            //----------------------------------------------------------

            // one triangle - 3 vertex
            mesh.vertices[vertex_pos_counter    ] = (float) x * scale_factor.x;
            mesh.vertices[vertex_pos_counter + 1] = (float) RF_GRAY_VALUE(pixels[x + z * map_x]) * scale_factor.y;
            mesh.vertices[vertex_pos_counter + 2] = (float) z * scale_factor.z;

            mesh.vertices[vertex_pos_counter + 3] = (float) x * scale_factor.x;
            mesh.vertices[vertex_pos_counter + 4] = (float) RF_GRAY_VALUE(pixels[x + (z + 1) * map_x]) * scale_factor.y;
            mesh.vertices[vertex_pos_counter + 5] = (float) (z + 1) * scale_factor.z;

            mesh.vertices[vertex_pos_counter + 6] = (float)(x + 1) * scale_factor.x;
            mesh.vertices[vertex_pos_counter + 7] = (float)RF_GRAY_VALUE(pixels[(x + 1) + z * map_x]) * scale_factor.y;
            mesh.vertices[vertex_pos_counter + 8] = (float)z * scale_factor.z;

            // another triangle - 3 vertex
            mesh.vertices[vertex_pos_counter + 9 ] = mesh.vertices[vertex_pos_counter + 6];
            mesh.vertices[vertex_pos_counter + 10] = mesh.vertices[vertex_pos_counter + 7];
            mesh.vertices[vertex_pos_counter + 11] = mesh.vertices[vertex_pos_counter + 8];

            mesh.vertices[vertex_pos_counter + 12] = mesh.vertices[vertex_pos_counter + 3];
            mesh.vertices[vertex_pos_counter + 13] = mesh.vertices[vertex_pos_counter + 4];
            mesh.vertices[vertex_pos_counter + 14] = mesh.vertices[vertex_pos_counter + 5];

            mesh.vertices[vertex_pos_counter + 15] = (float)(x + 1) * scale_factor.x;
            mesh.vertices[vertex_pos_counter + 16] = (float)RF_GRAY_VALUE(pixels[(x + 1) + (z + 1) * map_x]) * scale_factor.y;
            mesh.vertices[vertex_pos_counter + 17] = (float)(z + 1) * scale_factor.z;
            vertex_pos_counter += 18; // 6 vertex, 18 floats

            // Fill texcoords array with data
            //--------------------------------------------------------------
            mesh.texcoords[vertex_texcoord_counter    ] = (float)x / (map_x - 1);
            mesh.texcoords[vertex_texcoord_counter + 1] = (float)z / (map_z - 1);

            mesh.texcoords[vertex_texcoord_counter + 2] = (float)x / (map_x - 1);
            mesh.texcoords[vertex_texcoord_counter + 3] = (float)(z + 1) / (map_z - 1);

            mesh.texcoords[vertex_texcoord_counter + 4] = (float)(x + 1) / (map_x - 1);
            mesh.texcoords[vertex_texcoord_counter + 5] = (float)z / (map_z - 1);

            mesh.texcoords[vertex_texcoord_counter + 6] = mesh.texcoords[vertex_texcoord_counter + 4];
            mesh.texcoords[vertex_texcoord_counter + 7] = mesh.texcoords[vertex_texcoord_counter + 5];

            mesh.texcoords[vertex_texcoord_counter + 8] = mesh.texcoords[vertex_texcoord_counter + 2];
            mesh.texcoords[vertex_texcoord_counter + 9] = mesh.texcoords[vertex_texcoord_counter + 3];

            mesh.texcoords[vertex_texcoord_counter + 10] = (float)(x + 1) / (map_x - 1);
            mesh.texcoords[vertex_texcoord_counter + 11] = (float)(z + 1) / (map_z - 1);

            vertex_texcoord_counter += 12; // 6 texcoords, 12 floats

            // Fill normals array with data
            //--------------------------------------------------------------
            for (rf_int i = 0; i < 18; i += 3)
            {
                mesh.normals[n_counter + i    ] = 0.0f;
                mesh.normals[n_counter + i + 1] = 1.0f;
                mesh.normals[n_counter + i + 2] = 0.0f;
            }

            // TODO: Calculate normals in an efficient way

            n_counter    += 18; // 6 vertex, 18 floats
            tris_counter += 2;
        }
    }

    rf_free(temp_allocator, pixels);

    // Upload vertex data to GPU (static mesh)
    rf_gfx_load_mesh(&mesh, false);

    return mesh;
}

// Generate a cubes mesh from pixel data
// NOTE: Vertex data is uploaded to GPU
rf_public rf_mesh rf_gen_mesh_cubicmap(rf_image cubicmap, rf_vec3 cube_size, rf_allocator allocator, rf_allocator temp_allocator)
{
    rf_mesh mesh = {0};
    mesh.vbo_id = (unsigned int*) rf_alloc(allocator, RF_MAX_MESH_VBO * sizeof(unsigned int));
    memset(mesh.vbo_id, 0, RF_MAX_MESH_VBO * sizeof(unsigned int));

    rf_color* cubicmap_pixels = rf_image_pixels_to_rgba32(cubicmap, temp_allocator);

    int map_width = cubicmap.width;
    int map_height = cubicmap.height;

    // NOTE: Max possible number of triangles numCubes*(12 triangles by cube)
    int maxTriangles = cubicmap.width*cubicmap.height*12;

    int vertex_pos_counter = 0; // Used to count vertices
    int vertex_texcoord_counter = 0; // Used to count texcoords
    int n_counter = 0; // Used to count normals

    float w = cube_size.x;
    float h = cube_size.z;
    float h2 = cube_size.y;

    rf_vec3* map_vertices  = (rf_vec3*) rf_alloc(temp_allocator, maxTriangles * 3 * sizeof(rf_vec3));
    rf_vec2 *map_texcoords = (rf_vec2*) rf_alloc(temp_allocator, maxTriangles * 3 * sizeof(rf_vec2));
    rf_vec3* map_normals   = (rf_vec3*) rf_alloc(temp_allocator, maxTriangles * 3 * sizeof(rf_vec3));

    // Define the 6 normals of the cube, we will combine them accordingly later...
    rf_vec3 n1 = {  1.0f,  0.0f,  0.0f };
    rf_vec3 n2 = { -1.0f,  0.0f,  0.0f };
    rf_vec3 n3 = {  0.0f,  1.0f,  0.0f };
    rf_vec3 n4 = {  0.0f, -1.0f,  0.0f };
    rf_vec3 n5 = {  0.0f,  0.0f,  1.0f };
    rf_vec3 n6 = {  0.0f,  0.0f, -1.0f };

    // NOTE: We use texture rectangles to define different textures for top-bottom-front-back-right-left (6)
    typedef struct rf_recf rf_recf;
    struct rf_recf
    {
        float x;
        float y;
        float width;
        float height;
    };

    rf_recf right_tex_uv  = { 0.0f, 0.0f, 0.5f, 0.5f };
    rf_recf left_tex_uv   = { 0.5f, 0.0f, 0.5f, 0.5f };
    rf_recf front_tex_uv  = { 0.0f, 0.0f, 0.5f, 0.5f };
    rf_recf back_tex_uv   = { 0.5f, 0.0f, 0.5f, 0.5f };
    rf_recf top_tex_uv    = { 0.0f, 0.5f, 0.5f, 0.5f };
    rf_recf bottom_tex_uv = { 0.5f, 0.5f, 0.5f, 0.5f };

    for (rf_int z = 0; z < map_height; ++z)
    {
        for (rf_int x = 0; x < map_width; ++x)
        {
            // Define the 8 vertex of the cube, we will combine them accordingly later...
            rf_vec3 v1 = {w * (x - 0.5f), h2, h * (z - 0.5f) };
            rf_vec3 v2 = {w * (x - 0.5f), h2, h * (z + 0.5f) };
            rf_vec3 v3 = {w * (x + 0.5f), h2, h * (z + 0.5f) };
            rf_vec3 v4 = {w * (x + 0.5f), h2, h * (z - 0.5f) };
            rf_vec3 v5 = {w * (x + 0.5f), 0, h * (z - 0.5f) };
            rf_vec3 v6 = {w * (x - 0.5f), 0, h * (z - 0.5f) };
            rf_vec3 v7 = {w * (x - 0.5f), 0, h * (z + 0.5f) };
            rf_vec3 v8 = {w * (x + 0.5f), 0, h * (z + 0.5f) };

            // We check pixel color to be RF_WHITE, we will full cubes
            if ((cubicmap_pixels[z*cubicmap.width + x].r == 255) &&
                (cubicmap_pixels[z*cubicmap.width + x].g == 255) &&
                (cubicmap_pixels[z*cubicmap.width + x].b == 255))
            {
                // Define triangles (Checking Collateral Cubes!)
                //----------------------------------------------

                // Define top triangles (2 tris, 6 vertex --> v1-v2-v3, v1-v3-v4)
                map_vertices[vertex_pos_counter] = v1;
                map_vertices[vertex_pos_counter + 1] = v2;
                map_vertices[vertex_pos_counter + 2] = v3;
                map_vertices[vertex_pos_counter + 3] = v1;
                map_vertices[vertex_pos_counter + 4] = v3;
                map_vertices[vertex_pos_counter + 5] = v4;
                vertex_pos_counter += 6;

                map_normals[n_counter] = n3;
                map_normals[n_counter + 1] = n3;
                map_normals[n_counter + 2] = n3;
                map_normals[n_counter + 3] = n3;
                map_normals[n_counter + 4] = n3;
                map_normals[n_counter + 5] = n3;
                n_counter += 6;

                map_texcoords[vertex_texcoord_counter] = (rf_vec2){top_tex_uv.x, top_tex_uv.y };
                map_texcoords[vertex_texcoord_counter + 1] = (rf_vec2){top_tex_uv.x, top_tex_uv.y + top_tex_uv.height };
                map_texcoords[vertex_texcoord_counter + 2] = (rf_vec2){top_tex_uv.x + top_tex_uv.width, top_tex_uv.y + top_tex_uv.height };
                map_texcoords[vertex_texcoord_counter + 3] = (rf_vec2){top_tex_uv.x, top_tex_uv.y };
                map_texcoords[vertex_texcoord_counter + 4] = (rf_vec2){top_tex_uv.x + top_tex_uv.width, top_tex_uv.y + top_tex_uv.height };
                map_texcoords[vertex_texcoord_counter + 5] = (rf_vec2){top_tex_uv.x + top_tex_uv.width, top_tex_uv.y };
                vertex_texcoord_counter += 6;

                // Define bottom triangles (2 tris, 6 vertex --> v6-v8-v7, v6-v5-v8)
                map_vertices[vertex_pos_counter] = v6;
                map_vertices[vertex_pos_counter + 1] = v8;
                map_vertices[vertex_pos_counter + 2] = v7;
                map_vertices[vertex_pos_counter + 3] = v6;
                map_vertices[vertex_pos_counter + 4] = v5;
                map_vertices[vertex_pos_counter + 5] = v8;
                vertex_pos_counter += 6;

                map_normals[n_counter] = n4;
                map_normals[n_counter + 1] = n4;
                map_normals[n_counter + 2] = n4;
                map_normals[n_counter + 3] = n4;
                map_normals[n_counter + 4] = n4;
                map_normals[n_counter + 5] = n4;
                n_counter += 6;

                map_texcoords[vertex_texcoord_counter] = (rf_vec2){bottom_tex_uv.x + bottom_tex_uv.width, bottom_tex_uv.y };
                map_texcoords[vertex_texcoord_counter + 1] = (rf_vec2){bottom_tex_uv.x, bottom_tex_uv.y + bottom_tex_uv.height };
                map_texcoords[vertex_texcoord_counter + 2] = (rf_vec2){bottom_tex_uv.x + bottom_tex_uv.width, bottom_tex_uv.y + bottom_tex_uv.height };
                map_texcoords[vertex_texcoord_counter + 3] = (rf_vec2){bottom_tex_uv.x + bottom_tex_uv.width, bottom_tex_uv.y };
                map_texcoords[vertex_texcoord_counter + 4] = (rf_vec2){bottom_tex_uv.x, bottom_tex_uv.y };
                map_texcoords[vertex_texcoord_counter + 5] = (rf_vec2){bottom_tex_uv.x, bottom_tex_uv.y + bottom_tex_uv.height };
                vertex_texcoord_counter += 6;

                if (((z < cubicmap.height - 1) &&
                     (cubicmap_pixels[(z + 1)*cubicmap.width + x].r == 0) &&
                     (cubicmap_pixels[(z + 1)*cubicmap.width + x].g == 0) &&
                     (cubicmap_pixels[(z + 1)*cubicmap.width + x].b == 0)) || (z == cubicmap.height - 1))
                {
                    // Define front triangles (2 tris, 6 vertex) --> v2 v7 v3, v3 v7 v8
                    // NOTE: Collateral occluded faces are not generated
                    map_vertices[vertex_pos_counter] = v2;
                    map_vertices[vertex_pos_counter + 1] = v7;
                    map_vertices[vertex_pos_counter + 2] = v3;
                    map_vertices[vertex_pos_counter + 3] = v3;
                    map_vertices[vertex_pos_counter + 4] = v7;
                    map_vertices[vertex_pos_counter + 5] = v8;
                    vertex_pos_counter += 6;

                    map_normals[n_counter] = n6;
                    map_normals[n_counter + 1] = n6;
                    map_normals[n_counter + 2] = n6;
                    map_normals[n_counter + 3] = n6;
                    map_normals[n_counter + 4] = n6;
                    map_normals[n_counter + 5] = n6;
                    n_counter += 6;

                    map_texcoords[vertex_texcoord_counter] = (rf_vec2){front_tex_uv.x, front_tex_uv.y };
                    map_texcoords[vertex_texcoord_counter + 1] = (rf_vec2){front_tex_uv.x, front_tex_uv.y + front_tex_uv.height };
                    map_texcoords[vertex_texcoord_counter + 2] = (rf_vec2){front_tex_uv.x + front_tex_uv.width, front_tex_uv.y };
                    map_texcoords[vertex_texcoord_counter + 3] = (rf_vec2){front_tex_uv.x + front_tex_uv.width, front_tex_uv.y };
                    map_texcoords[vertex_texcoord_counter + 4] = (rf_vec2){front_tex_uv.x, front_tex_uv.y + front_tex_uv.height };
                    map_texcoords[vertex_texcoord_counter + 5] = (rf_vec2){front_tex_uv.x + front_tex_uv.width, front_tex_uv.y + front_tex_uv.height };
                    vertex_texcoord_counter += 6;
                }

                if (((z > 0) &&
                     (cubicmap_pixels[(z - 1)*cubicmap.width + x].r == 0) &&
                     (cubicmap_pixels[(z - 1)*cubicmap.width + x].g == 0) &&
                     (cubicmap_pixels[(z - 1)*cubicmap.width + x].b == 0)) || (z == 0))
                {
                    // Define back triangles (2 tris, 6 vertex) --> v1 v5 v6, v1 v4 v5
                    // NOTE: Collateral occluded faces are not generated
                    map_vertices[vertex_pos_counter] = v1;
                    map_vertices[vertex_pos_counter + 1] = v5;
                    map_vertices[vertex_pos_counter + 2] = v6;
                    map_vertices[vertex_pos_counter + 3] = v1;
                    map_vertices[vertex_pos_counter + 4] = v4;
                    map_vertices[vertex_pos_counter + 5] = v5;
                    vertex_pos_counter += 6;

                    map_normals[n_counter] = n5;
                    map_normals[n_counter + 1] = n5;
                    map_normals[n_counter + 2] = n5;
                    map_normals[n_counter + 3] = n5;
                    map_normals[n_counter + 4] = n5;
                    map_normals[n_counter + 5] = n5;
                    n_counter += 6;

                    map_texcoords[vertex_texcoord_counter] = (rf_vec2){back_tex_uv.x + back_tex_uv.width, back_tex_uv.y };
                    map_texcoords[vertex_texcoord_counter + 1] = (rf_vec2){back_tex_uv.x, back_tex_uv.y + back_tex_uv.height };
                    map_texcoords[vertex_texcoord_counter + 2] = (rf_vec2){back_tex_uv.x + back_tex_uv.width, back_tex_uv.y + back_tex_uv.height };
                    map_texcoords[vertex_texcoord_counter + 3] = (rf_vec2){back_tex_uv.x + back_tex_uv.width, back_tex_uv.y };
                    map_texcoords[vertex_texcoord_counter + 4] = (rf_vec2){back_tex_uv.x, back_tex_uv.y };
                    map_texcoords[vertex_texcoord_counter + 5] = (rf_vec2){back_tex_uv.x, back_tex_uv.y + back_tex_uv.height };
                    vertex_texcoord_counter += 6;
                }

                if (((x < cubicmap.width - 1) &&
                     (cubicmap_pixels[z*cubicmap.width + (x + 1)].r == 0) &&
                     (cubicmap_pixels[z*cubicmap.width + (x + 1)].g == 0) &&
                     (cubicmap_pixels[z*cubicmap.width + (x + 1)].b == 0)) || (x == cubicmap.width - 1))
                {
                    // Define right triangles (2 tris, 6 vertex) --> v3 v8 v4, v4 v8 v5
                    // NOTE: Collateral occluded faces are not generated
                    map_vertices[vertex_pos_counter] = v3;
                    map_vertices[vertex_pos_counter + 1] = v8;
                    map_vertices[vertex_pos_counter + 2] = v4;
                    map_vertices[vertex_pos_counter + 3] = v4;
                    map_vertices[vertex_pos_counter + 4] = v8;
                    map_vertices[vertex_pos_counter + 5] = v5;
                    vertex_pos_counter += 6;

                    map_normals[n_counter] = n1;
                    map_normals[n_counter + 1] = n1;
                    map_normals[n_counter + 2] = n1;
                    map_normals[n_counter + 3] = n1;
                    map_normals[n_counter + 4] = n1;
                    map_normals[n_counter + 5] = n1;
                    n_counter += 6;

                    map_texcoords[vertex_texcoord_counter] = (rf_vec2){right_tex_uv.x, right_tex_uv.y };
                    map_texcoords[vertex_texcoord_counter + 1] = (rf_vec2){right_tex_uv.x, right_tex_uv.y + right_tex_uv.height };
                    map_texcoords[vertex_texcoord_counter + 2] = (rf_vec2){right_tex_uv.x + right_tex_uv.width, right_tex_uv.y };
                    map_texcoords[vertex_texcoord_counter + 3] = (rf_vec2){right_tex_uv.x + right_tex_uv.width, right_tex_uv.y };
                    map_texcoords[vertex_texcoord_counter + 4] = (rf_vec2){right_tex_uv.x, right_tex_uv.y + right_tex_uv.height };
                    map_texcoords[vertex_texcoord_counter + 5] = (rf_vec2){right_tex_uv.x + right_tex_uv.width, right_tex_uv.y + right_tex_uv.height };
                    vertex_texcoord_counter += 6;
                }

                if (((x > 0) &&
                     (cubicmap_pixels[z*cubicmap.width + (x - 1)].r == 0) &&
                     (cubicmap_pixels[z*cubicmap.width + (x - 1)].g == 0) &&
                     (cubicmap_pixels[z*cubicmap.width + (x - 1)].b == 0)) || (x == 0))
                {
                    // Define left triangles (2 tris, 6 vertex) --> v1 v7 v2, v1 v6 v7
                    // NOTE: Collateral occluded faces are not generated
                    map_vertices[vertex_pos_counter] = v1;
                    map_vertices[vertex_pos_counter + 1] = v7;
                    map_vertices[vertex_pos_counter + 2] = v2;
                    map_vertices[vertex_pos_counter + 3] = v1;
                    map_vertices[vertex_pos_counter + 4] = v6;
                    map_vertices[vertex_pos_counter + 5] = v7;
                    vertex_pos_counter += 6;

                    map_normals[n_counter] = n2;
                    map_normals[n_counter + 1] = n2;
                    map_normals[n_counter + 2] = n2;
                    map_normals[n_counter + 3] = n2;
                    map_normals[n_counter + 4] = n2;
                    map_normals[n_counter + 5] = n2;
                    n_counter += 6;

                    map_texcoords[vertex_texcoord_counter] = (rf_vec2){left_tex_uv.x, left_tex_uv.y };
                    map_texcoords[vertex_texcoord_counter + 1] = (rf_vec2){left_tex_uv.x + left_tex_uv.width, left_tex_uv.y + left_tex_uv.height };
                    map_texcoords[vertex_texcoord_counter + 2] = (rf_vec2){left_tex_uv.x + left_tex_uv.width, left_tex_uv.y };
                    map_texcoords[vertex_texcoord_counter + 3] = (rf_vec2){left_tex_uv.x, left_tex_uv.y };
                    map_texcoords[vertex_texcoord_counter + 4] = (rf_vec2){left_tex_uv.x, left_tex_uv.y + left_tex_uv.height };
                    map_texcoords[vertex_texcoord_counter + 5] = (rf_vec2){left_tex_uv.x + left_tex_uv.width, left_tex_uv.y + left_tex_uv.height };
                    vertex_texcoord_counter += 6;
                }
            }
                // We check pixel color to be RF_BLACK, we will only draw floor and roof
            else if ((cubicmap_pixels[z*cubicmap.width + x].r == 0) &&
                     (cubicmap_pixels[z*cubicmap.width + x].g == 0) &&
                     (cubicmap_pixels[z*cubicmap.width + x].b == 0))
            {
                // Define top triangles (2 tris, 6 vertex --> v1-v2-v3, v1-v3-v4)
                map_vertices[vertex_pos_counter] = v1;
                map_vertices[vertex_pos_counter + 1] = v3;
                map_vertices[vertex_pos_counter + 2] = v2;
                map_vertices[vertex_pos_counter + 3] = v1;
                map_vertices[vertex_pos_counter + 4] = v4;
                map_vertices[vertex_pos_counter + 5] = v3;
                vertex_pos_counter += 6;

                map_normals[n_counter] = n4;
                map_normals[n_counter + 1] = n4;
                map_normals[n_counter + 2] = n4;
                map_normals[n_counter + 3] = n4;
                map_normals[n_counter + 4] = n4;
                map_normals[n_counter + 5] = n4;
                n_counter += 6;

                map_texcoords[vertex_texcoord_counter] = (rf_vec2){top_tex_uv.x, top_tex_uv.y };
                map_texcoords[vertex_texcoord_counter + 1] = (rf_vec2){top_tex_uv.x + top_tex_uv.width, top_tex_uv.y + top_tex_uv.height };
                map_texcoords[vertex_texcoord_counter + 2] = (rf_vec2){top_tex_uv.x, top_tex_uv.y + top_tex_uv.height };
                map_texcoords[vertex_texcoord_counter + 3] = (rf_vec2){top_tex_uv.x, top_tex_uv.y };
                map_texcoords[vertex_texcoord_counter + 4] = (rf_vec2){top_tex_uv.x + top_tex_uv.width, top_tex_uv.y };
                map_texcoords[vertex_texcoord_counter + 5] = (rf_vec2){top_tex_uv.x + top_tex_uv.width, top_tex_uv.y + top_tex_uv.height };
                vertex_texcoord_counter += 6;

                // Define bottom triangles (2 tris, 6 vertex --> v6-v8-v7, v6-v5-v8)
                map_vertices[vertex_pos_counter] = v6;
                map_vertices[vertex_pos_counter + 1] = v7;
                map_vertices[vertex_pos_counter + 2] = v8;
                map_vertices[vertex_pos_counter + 3] = v6;
                map_vertices[vertex_pos_counter + 4] = v8;
                map_vertices[vertex_pos_counter + 5] = v5;
                vertex_pos_counter += 6;

                map_normals[n_counter] = n3;
                map_normals[n_counter + 1] = n3;
                map_normals[n_counter + 2] = n3;
                map_normals[n_counter + 3] = n3;
                map_normals[n_counter + 4] = n3;
                map_normals[n_counter + 5] = n3;
                n_counter += 6;

                map_texcoords[vertex_texcoord_counter] = (rf_vec2){bottom_tex_uv.x + bottom_tex_uv.width, bottom_tex_uv.y };
                map_texcoords[vertex_texcoord_counter + 1] = (rf_vec2){bottom_tex_uv.x + bottom_tex_uv.width, bottom_tex_uv.y + bottom_tex_uv.height };
                map_texcoords[vertex_texcoord_counter + 2] = (rf_vec2){bottom_tex_uv.x, bottom_tex_uv.y + bottom_tex_uv.height };
                map_texcoords[vertex_texcoord_counter + 3] = (rf_vec2){bottom_tex_uv.x + bottom_tex_uv.width, bottom_tex_uv.y };
                map_texcoords[vertex_texcoord_counter + 4] = (rf_vec2){bottom_tex_uv.x, bottom_tex_uv.y + bottom_tex_uv.height };
                map_texcoords[vertex_texcoord_counter + 5] = (rf_vec2){bottom_tex_uv.x, bottom_tex_uv.y };
                vertex_texcoord_counter += 6;
            }
        }
    }

    // Move data from map_vertices temp arays to vertices float array
    mesh.vertex_count = vertex_pos_counter;
    mesh.triangle_count = vertex_pos_counter/3;

    mesh.vertices  = (float*) rf_alloc(allocator, mesh.vertex_count * 3 * sizeof(float));
    mesh.normals   = (float*) rf_alloc(allocator, mesh.vertex_count * 3 * sizeof(float));
    mesh.texcoords = (float*) rf_alloc(allocator, mesh.vertex_count * 2 * sizeof(float));
    mesh.colors = NULL;

    int f_counter = 0;

    // Move vertices data
    for (rf_int i = 0; i < vertex_pos_counter; i++)
    {
        mesh.vertices[f_counter] = map_vertices[i].x;
        mesh.vertices[f_counter + 1] = map_vertices[i].y;
        mesh.vertices[f_counter + 2] = map_vertices[i].z;
        f_counter += 3;
    }

    f_counter = 0;

    // Move normals data
    for (rf_int i = 0; i < n_counter; i++)
    {
        mesh.normals[f_counter] = map_normals[i].x;
        mesh.normals[f_counter + 1] = map_normals[i].y;
        mesh.normals[f_counter + 2] = map_normals[i].z;
        f_counter += 3;
    }

    f_counter = 0;

    // Move texcoords data
    for (rf_int i = 0; i < vertex_texcoord_counter; i++)
    {
        mesh.texcoords[f_counter] = map_texcoords[i].x;
        mesh.texcoords[f_counter + 1] = map_texcoords[i].y;
        f_counter += 2;
    }

    rf_free(temp_allocator, map_vertices);
    rf_free(temp_allocator, map_normals);
    rf_free(temp_allocator, map_texcoords);

    rf_free(temp_allocator, cubicmap_pixels); // Free image pixel data

    // Upload vertex data to GPU (static mesh)
    rf_gfx_load_mesh(&mesh, false);

    return mesh;
}

#pragma endregion



#pragma region internal functions

// Get texture to draw shapes, the user can customize this using rf_set_shapes_texture
rf_internal rf_texture2d rf_get_shapes_texture()
{
    if (rf_ctx.tex_shapes.id == 0)
    {
        rf_ctx.tex_shapes = rf_get_default_texture();
        rf_ctx.rec_tex_shapes = (rf_rec) {0.0f, 0.0f, 1.0f, 1.0f };
    }

    return rf_ctx.tex_shapes;
}

// Cubic easing in-out. Note: Required for rf_draw_line_bezier()
rf_internal float rf_shapes_ease_cubic_in_out(float t, float b, float c, float d)
{
    if ((t /= 0.5f*d) < 1) return 0.5f*c*t*t*t + b;

    t -= 2;

    return 0.5f*c*(t*t*t + 2.0f) + b;
}

#pragma endregion

rf_public rf_vec2 rf_center_to_screen(float w, float h)
{
    rf_vec2 result = { rf_ctx.render_width / 2 - w / 2, rf_ctx.render_height / 2 - h / 2 };
    return result;
}

// Set background color (framebuffer clear color)
rf_public void rf_clear(rf_color color)
{
    rf_gfx_clear_color(color.r, color.g, color.b, color.a); // Set clear color
    rf_gfx_clear_screen_buffers(); // Clear current framebuffers
}

// Setup canvas (framebuffer) to start drawing
rf_public void rf_begin()
{
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)
    rf_gfx_mult_matrixf(rf_mat_to_float16(rf_ctx.screen_scaling).v); // Apply screen scaling

    //rf_gfx_translatef(0.375, 0.375, 0);    // HACK to have 2D pixel-perfect drawing on OpenGL 1.1, not required with OpenGL 3.3+
}

// End canvas drawing and swap buffers (double buffering)
rf_public void rf_end()
{
    rf_gfx_draw();
}

// Initialize 2D mode with custom camera (2D)
rf_public void rf_begin_2d(rf_camera2d camera)
{
    rf_gfx_draw();

    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)

    // Apply screen scaling if required
    rf_gfx_mult_matrixf(rf_mat_to_float16(rf_ctx.screen_scaling).v);

    // Apply 2d camera transformation to rf_gfxobal_model_view
    rf_gfx_mult_matrixf(rf_mat_to_float16(rf_get_camera_matrix2d(camera)).v);
}

// Ends 2D mode with custom camera
rf_public void rf_end_2d()
{
    rf_gfx_draw();

    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)
    rf_gfx_mult_matrixf(rf_mat_to_float16(rf_ctx.screen_scaling).v); // Apply screen scaling if required
}

// Initializes 3D mode with custom camera (3D)
rf_public void rf_begin_3d(rf_camera3d camera)
{
    rf_gfx_draw();

    rf_gfx_matrix_mode(RF_PROJECTION); // Switch to GL_PROJECTION matrix
    rf_gfx_push_matrix(); // Save previous matrix, which contains the settings for the 2d ortho GL_PROJECTION
    rf_gfx_load_identity(); // Reset current matrix (PROJECTION)

    float aspect = (float) rf_ctx.current_width / (float)rf_ctx.current_height;

    if (camera.type == RF_CAMERA_PERSPECTIVE)
    {
        // Setup perspective GL_PROJECTION
        double top = 0.01 * tan(camera.fovy * 0.5 * rf_deg2rad);
        double right = top*aspect;

        rf_gfx_frustum(-right, right, -top, top, 0.01, 1000.0);
    }
    else if (camera.type == RF_CAMERA_ORTHOGRAPHIC)
    {
        // Setup orthographic GL_PROJECTION
        double top = camera.fovy/2.0;
        double right = top*aspect;

        rf_gfx_ortho(-right,right,-top,top, 0.01, 1000.0);
    }

    // NOTE: zNear and zFar values are important when computing depth buffer values

    rf_gfx_matrix_mode(RF_MODELVIEW); // Switch back to rf_gfxobal_model_view matrix
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)

    // Setup rf_camera3d view
    rf_mat mat_view = rf_mat_look_at(camera.position, camera.target, camera.up);
    rf_gfx_mult_matrixf(rf_mat_to_float16(mat_view).v); // Multiply MODELVIEW matrix by view matrix (camera)

    rf_gfx_enable_depth_test(); // Enable DEPTH_TEST for 3D
}

// Ends 3D mode and returns to default 2D orthographic mode
rf_public void rf_end_3d()
{
    rf_gfx_draw(); // Process internal buffers (update + draw)

    rf_gfx_matrix_mode(RF_PROJECTION); // Switch to GL_PROJECTION matrix
    rf_gfx_pop_matrix(); // Restore previous matrix (PROJECTION) from matrix rf_gfxobal_gl_stack

    rf_gfx_matrix_mode(RF_MODELVIEW); // Get back to rf_gfxobal_model_view matrix
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)

    rf_gfx_mult_matrixf(rf_mat_to_float16(rf_ctx.screen_scaling).v); // Apply screen scaling if required

    rf_gfx_disable_depth_test(); // Disable DEPTH_TEST for 2D
}

// Initializes render texture for drawing
rf_public void rf_begin_render_to_texture(rf_render_texture2d target)
{
    rf_gfx_draw();

    rf_gfx_enable_render_texture(target.id); // Enable render target

    // Set viewport to framebuffer size
    rf_gfx_viewport(0, 0, target.texture.width, target.texture.height);

    rf_gfx_matrix_mode(RF_PROJECTION); // Switch to PROJECTION matrix
    rf_gfx_load_identity(); // Reset current matrix (PROJECTION)

    // Set orthographic GL_PROJECTION to current framebuffer size
    // NOTE: Configured top-left corner as (0, 0)
    rf_gfx_ortho(0, target.texture.width, target.texture.height, 0, 0.0f, 1.0f);

    rf_gfx_matrix_mode(RF_MODELVIEW); // Switch back to MODELVIEW matrix
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)

    //rf_gfx_scalef(0.0f, -1.0f, 0.0f);      // Flip Y-drawing (?)

    // Setup current width/height for proper aspect ratio
    // calculation when using rf_begin_mode3d()
    rf_ctx.current_width = target.texture.width;
    rf_ctx.current_height = target.texture.height;
}

// Ends drawing to render texture
rf_public void rf_end_render_to_texture()
{
    rf_gfx_draw();

    rf_gfx_disable_render_texture(); // Disable render target

    // Set viewport to default framebuffer size
    rf_set_viewport(rf_ctx.render_width, rf_ctx.render_height);

    // Reset current screen size
    rf_ctx.current_width = rf_ctx.render_width;
    rf_ctx.current_height = rf_ctx.render_height;
}

// Begin scissor mode (define screen area for following drawing)
// NOTE: Scissor rec refers to bottom-left corner, we change it to upper-left
rf_public void rf_begin_scissor_mode(int x, int y, int width, int height)
{
    rf_gfx_draw(); // Force drawing elements

    rf_gfx_enable_scissor_test();
    rf_gfx_scissor(x, rf_ctx.render_width - (y + height), width, height);
}

// End scissor mode
rf_public void rf_end_scissor_mode()
{
    rf_gfx_draw(); // Force drawing elements
    rf_gfx_disable_scissor_test();
}

// Begin custom shader mode
rf_public void rf_begin_shader(rf_shader shader)
{
    if (rf_ctx.current_shader.id != shader.id)
    {
        rf_gfx_draw();
        rf_ctx.current_shader = shader;
    }
}

// End custom shader mode (returns to default shader)
rf_public void rf_end_shader()
{
    rf_begin_shader(rf_ctx.default_shader);
}

// Begin blending mode (alpha, additive, multiplied). Default blend mode is alpha
rf_public void rf_begin_blend_mode(rf_blend_mode mode)
{
    rf_gfx_blend_mode(mode);
}

// End blending mode (reset to default: alpha blending)
rf_public void rf_end_blend_mode()
{
    rf_gfx_blend_mode(RF_BLEND_ALPHA);
}

// Draw a pixel
rf_public void rf_draw_pixel(int pos_x, int pos_y, rf_color color)
{
    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2i(pos_x, pos_y);
    rf_gfx_vertex2i(pos_x + 1, pos_y + 1);
    rf_gfx_end();
}

// Draw a pixel (Vector version)
rf_public void rf_draw_pixel_v(rf_vec2 position, rf_color color)
{
    rf_draw_pixel(position.x, position.y, color);
}

// Draw a line
rf_public void rf_draw_line(int startPosX, int startPosY, int endPosX, int endPosY, rf_color color)
{
    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2i(startPosX, startPosY);
    rf_gfx_vertex2i(endPosX, endPosY);
    rf_gfx_end();
}

// Draw a line (Vector version)
rf_public void rf_draw_line_v(rf_vec2 startPos, rf_vec2 endPos, rf_color color)
{
    rf_draw_line(startPos.x, startPos.y, endPos.x, endPos.y, color);
}

// Draw a line defining thickness
rf_public void rf_draw_line_ex(rf_vec2 start_pos, rf_vec2 end_pos, float thick, rf_color color)
{
    if (start_pos.x > end_pos.x)
    {
        rf_vec2 temp_pos = start_pos;
        start_pos = end_pos;
        end_pos = temp_pos;
    }

    float dx = end_pos.x - start_pos.x;
    float dy = end_pos.y - start_pos.y;

    float d = sqrtf(dx*dx + dy*dy);
    float angle = asinf(dy/d);

    rf_gfx_enable_texture(rf_get_shapes_texture().id);

    rf_gfx_push_matrix();
    rf_gfx_translatef((float)start_pos.x, (float)start_pos.y, 0.0f);
    rf_gfx_rotatef(rf_rad2deg * angle, 0.0f, 0.0f, 1.0f);
    rf_gfx_translatef(0, (thick > 1.0f)? -thick/2.0f : -1.0f, 0.0f);

    rf_gfx_begin(RF_QUADS);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_normal3f(0.0f, 0.0f, 1.0f);

    rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(0.0f, 0.0f);

    rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(0.0f, thick);

    rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(d, thick);

    rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(d, 0.0f);
    rf_gfx_end();
    rf_gfx_pop_matrix();

    rf_gfx_disable_texture();
}

// Draw line using cubic-bezier curves in-out
rf_public void rf_draw_line_bezier(rf_vec2 start_pos, rf_vec2 end_pos, float thick, rf_color color)
{
#define RF_LINE_DIVISIONS 24 // Bezier line divisions

    rf_vec2 previous = start_pos;
    rf_vec2 current;

    for (rf_int i = 1; i <= RF_LINE_DIVISIONS; i++)
    {
        // Cubic easing in-out
        // NOTE: Easing is calculated only for y position value
        current.y = rf_shapes_ease_cubic_in_out((float)i, start_pos.y, end_pos.y - start_pos.y, (float)RF_LINE_DIVISIONS);
        current.x = previous.x + (end_pos.x - start_pos.x)/ (float)RF_LINE_DIVISIONS;

        rf_draw_line_ex(previous, current, thick, color);

        previous = current;
    }

#undef RF_LINE_DIVISIONS
}

// Draw lines sequence
rf_public void rf_draw_line_strip(rf_vec2 *points, int points_count, rf_color color)
{
    if (points_count >= 2)
    {
        if (rf_gfx_check_buffer_limit(points_count)) rf_gfx_draw();

        rf_gfx_begin(RF_LINES);
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        for (rf_int i = 0; i < points_count - 1; i++)
        {
            rf_gfx_vertex2f(points[i].x, points[i].y);
            rf_gfx_vertex2f(points[i + 1].x, points[i + 1].y);
        }
        rf_gfx_end();
    }
}

// Draw a color-filled circle
rf_public void rf_draw_circle(int center_x, int center_y, float radius, rf_color color)
{
    rf_draw_circle_sector((rf_vec2) {center_x, center_y }, radius, 0, 360, 36, color);
}

// Draw a color-filled circle (Vector version)
rf_public void rf_draw_circle_v(rf_vec2 center, float radius, rf_color color)
{
    rf_draw_circle(center.x, center.y, radius, color);
}

// Draw a piece of a circle
rf_public void rf_draw_circle_sector(rf_vec2 center, float radius, int start_angle, int end_angle, int segments, rf_color color)
{
    if (radius <= 0.0f) radius = 0.1f; // Avoid div by zero

    // Function expects (endAngle > start_angle)
    if (end_angle < start_angle)
    {
        // Swap values
        int tmp = start_angle;
        start_angle = end_angle;
        end_angle = tmp;
    }

    if (segments < 4)
    {
        // Calculate how many segments we need to draw a smooth circle, taken from https://stackoverflow.com/a/2244088
        float CIRCLE_ERROR_RATE = 0.5f;

        // Calculate the maximum angle between segments based on the error rate.
        float th = acosf(2*powf(1 - CIRCLE_ERROR_RATE/radius, 2) - 1);
        segments = (end_angle - start_angle) * ceilf(2 * rf_pi / th) / 360;

        if (segments <= 0) segments = 4;
    }

    float step_length = (float)(end_angle - start_angle)/(float)segments;
    float angle = start_angle;
    if (rf_gfx_check_buffer_limit(3*segments)) rf_gfx_draw();

    rf_gfx_begin(RF_TRIANGLES);
    for (rf_int i = 0; i < segments; i++)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        rf_gfx_vertex2f(center.x, center.y);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * radius, center.y + cosf(rf_deg2rad * angle) * radius);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * (angle + step_length)) * radius, center.y + cosf(rf_deg2rad * (angle + step_length)) * radius);

        angle += step_length;
    }
    rf_gfx_end();
}

rf_public void rf_draw_circle_sector_lines(rf_vec2 center, float radius, int start_angle, int end_angle, int segments, rf_color color)
{
    if (radius <= 0.0f) radius = 0.1f; // Avoid div by zero issue

    // Function expects (endAngle > start_angle)
    if (end_angle < start_angle)
    {
        // Swap values
        int tmp = start_angle;
        start_angle = end_angle;
        end_angle = tmp;
    }

    if (segments < 4)
    {
        // Calculate how many segments we need to draw a smooth circle, taken from https://stackoverflow.com/a/2244088

        float CIRCLE_ERROR_RATE = 0.5f;


        // Calculate the maximum angle between segments based on the error rate.
        float th = acosf(2*powf(1 - CIRCLE_ERROR_RATE/radius, 2) - 1);
        segments = (end_angle - start_angle) * ceilf(2 * rf_pi / th) / 360;

        if (segments <= 0) segments = 4;
    }

    float step_length = (float)(end_angle - start_angle)/(float)segments;
    float angle = start_angle;

    // Hide the cap lines when the circle is full
    rf_bool show_cap_lines = true;
    int limit = 2*(segments + 2);
    if ((end_angle - start_angle)%360 == 0) { limit = 2*segments; show_cap_lines = false; }

    if (rf_gfx_check_buffer_limit(limit)) rf_gfx_draw();

    rf_gfx_begin(RF_LINES);
    if (show_cap_lines)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(center.x, center.y);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * radius, center.y + cosf(rf_deg2rad * angle) * radius);
    }

    for (rf_int i = 0; i < segments; i++)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * radius, center.y + cosf(rf_deg2rad * angle) * radius);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * (angle + step_length)) * radius, center.y + cosf(rf_deg2rad * (angle + step_length)) * radius);

        angle += step_length;
    }

    if (show_cap_lines)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(center.x, center.y);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * radius, center.y + cosf(rf_deg2rad * angle) * radius);
    }
    rf_gfx_end();
}

// Draw a gradient-filled circle
// NOTE: Gradient goes from center (color1) to border (color2)
rf_public void rf_draw_circle_gradient(int center_x, int center_y, float radius, rf_color color1, rf_color color2)
{
    if (rf_gfx_check_buffer_limit(3 * 36)) rf_gfx_draw();

    rf_gfx_begin(RF_TRIANGLES);
    for (rf_int i = 0; i < 360; i += 10)
    {
        rf_gfx_color4ub(color1.r, color1.g, color1.b, color1.a);
        rf_gfx_vertex2f(center_x, center_y);
        rf_gfx_color4ub(color2.r, color2.g, color2.b, color2.a);
        rf_gfx_vertex2f(center_x + sinf(rf_deg2rad * i) * radius, center_y + cosf(rf_deg2rad * i) * radius);
        rf_gfx_color4ub(color2.r, color2.g, color2.b, color2.a);
        rf_gfx_vertex2f(center_x + sinf(rf_deg2rad * (i + 10)) * radius, center_y + cosf(rf_deg2rad * (i + 10)) * radius);
    }
    rf_gfx_end();
}

// Draw circle outline
rf_public void rf_draw_circle_lines(int center_x, int center_y, float radius, rf_color color)
{
    if (rf_gfx_check_buffer_limit(2 * 36)) rf_gfx_draw();

    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    // NOTE: Circle outline is drawn pixel by pixel every degree (0 to 360)
    for (rf_int i = 0; i < 360; i += 10)
    {
        rf_gfx_vertex2f(center_x + sinf(rf_deg2rad * i) * radius, center_y + cosf(rf_deg2rad * i) * radius);
        rf_gfx_vertex2f(center_x + sinf(rf_deg2rad * (i + 10)) * radius, center_y + cosf(rf_deg2rad * (i + 10)) * radius);
    }
    rf_gfx_end();
}

rf_public void rf_draw_ring(rf_vec2 center, float inner_radius, float outer_radius, int start_angle, int end_angle, int segments, rf_color color)
{
    if (start_angle == end_angle) return;

    // Function expects (outerRadius > innerRadius)
    if (outer_radius < inner_radius)
    {
        float tmp = outer_radius;
        outer_radius = inner_radius;
        inner_radius = tmp;

        if (outer_radius <= 0.0f) outer_radius = 0.1f;
    }

    // Function expects (endAngle > start_angle)
    if (end_angle < start_angle)
    {
        // Swap values
        int tmp = start_angle;
        start_angle = end_angle;
        end_angle = tmp;
    }

    if (segments < 4)
    {
        // Calculate how many segments we need to draw a smooth circle, taken from https://stackoverflow.com/a/2244088

        float CIRCLE_ERROR_RATE = 0.5f;


        // Calculate the maximum angle between segments based on the error rate.
        float th = acosf(2*powf(1 - CIRCLE_ERROR_RATE/outer_radius, 2) - 1);
        segments = (end_angle - start_angle) * ceilf(2 * rf_pi / th) / 360;

        if (segments <= 0) segments = 4;
    }

    // Not a ring
    if (inner_radius <= 0.0f)
    {
        rf_draw_circle_sector(center, outer_radius, start_angle, end_angle, segments, color);
        return;
    }

    float step_length = (float)(end_angle - start_angle)/(float)segments;
    float angle = start_angle;
    if (rf_gfx_check_buffer_limit(6*segments)) rf_gfx_draw();

    rf_gfx_begin(RF_TRIANGLES);
    for (rf_int i = 0; i < segments; i++)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * inner_radius, center.y + cosf(rf_deg2rad * angle) * inner_radius);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * outer_radius, center.y + cosf(rf_deg2rad * angle) * outer_radius);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * (angle + step_length)) * inner_radius, center.y + cosf(rf_deg2rad * (angle + step_length)) * inner_radius);

        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * (angle + step_length)) * inner_radius, center.y + cosf(rf_deg2rad * (angle + step_length)) * inner_radius);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * outer_radius, center.y + cosf(rf_deg2rad * angle) * outer_radius);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * (angle + step_length)) * outer_radius, center.y + cosf(rf_deg2rad * (angle + step_length)) * outer_radius);

        angle += step_length;
    }
    rf_gfx_end();

}

rf_public void rf_draw_ring_lines(rf_vec2 center, float inner_radius, float outer_radius, int start_angle, int end_angle, int segments, rf_color color)
{
    if (start_angle == end_angle) return;

    // Function expects (outerRadius > innerRadius)
    if (outer_radius < inner_radius)
    {
        float tmp = outer_radius;
        outer_radius = inner_radius;
        inner_radius = tmp;

        if (outer_radius <= 0.0f) outer_radius = 0.1f;
    }

    // Function expects (endAngle > start_angle)
    if (end_angle < start_angle)
    {
        // Swap values
        int tmp = start_angle;
        start_angle = end_angle;
        end_angle = tmp;
    }

    if (segments < 4)
    {
        // Calculate how many segments we need to draw a smooth circle, taken from https://stackoverflow.com/a/2244088

        float CIRCLE_ERROR_RATE = 0.5f;

        // Calculate the maximum angle between segments based on the error rate.
        float th = acosf(2*powf(1 - CIRCLE_ERROR_RATE/outer_radius, 2) - 1);
        segments = (end_angle - start_angle) * ceilf(2 * rf_pi / th) / 360;

        if (segments <= 0) segments = 4;
    }

    if (inner_radius <= 0.0f)
    {
        rf_draw_circle_sector_lines(center, outer_radius, start_angle, end_angle, segments, color);
        return;
    }

    float step_length = (float)(end_angle - start_angle)/(float)segments;
    float angle = start_angle;

    rf_bool show_cap_lines = true;
    int limit = 4 * (segments + 1);
    if ((end_angle - start_angle)%360 == 0) { limit = 4 * segments; show_cap_lines = false; }

    if (rf_gfx_check_buffer_limit(limit)) rf_gfx_draw();

    rf_gfx_begin(RF_LINES);
    if (show_cap_lines)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * outer_radius, center.y + cosf(rf_deg2rad * angle) * outer_radius);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * inner_radius, center.y + cosf(rf_deg2rad * angle) * inner_radius);
    }

    for (rf_int i = 0; i < segments; i++)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * outer_radius, center.y + cosf(rf_deg2rad * angle) * outer_radius);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * (angle + step_length)) * outer_radius, center.y + cosf(rf_deg2rad * (angle + step_length)) * outer_radius);

        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * inner_radius, center.y + cosf(rf_deg2rad * angle) * inner_radius);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * (angle + step_length)) * inner_radius, center.y + cosf(rf_deg2rad * (angle + step_length)) * inner_radius);

        angle += step_length;
    }

    if (show_cap_lines)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * outer_radius, center.y + cosf(rf_deg2rad * angle) * outer_radius);
        rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * inner_radius, center.y + cosf(rf_deg2rad * angle) * inner_radius);
    }
    rf_gfx_end();
}

// Draw a color-filled rectangle
rf_public void rf_draw_rectangle(int posX, int posY, int width, int height, rf_color color)
{
    rf_draw_rectangle_v((rf_vec2){ (float)posX, (float)posY }, (rf_vec2){ (float)width, (float)height }, color);
}

// Draw a color-filled rectangle (Vector version)
rf_public void rf_draw_rectangle_v(rf_vec2 position, rf_vec2 size, rf_color color)
{
    rf_draw_rectangle_pro((rf_rec) { position.x, position.y, size.x, size.y }, (rf_vec2){ 0.0f, 0.0f }, 0.0f, color);
}

// Draw a color-filled rectangle
rf_public void rf_draw_rectangle_rec(rf_rec rec, rf_color color)
{
    rf_draw_rectangle_pro(rec, (rf_vec2){ 0.0f, 0.0f }, 0.0f, color);
}

// Draw a color-filled rectangle with pro parameters
rf_public void rf_draw_rectangle_pro(rf_rec rec, rf_vec2 origin, float rotation, rf_color color)
{
    rf_gfx_enable_texture(rf_get_shapes_texture().id);

    rf_gfx_push_matrix();
    rf_gfx_translatef(rec.x, rec.y, 0.0f);
    rf_gfx_rotatef(rotation, 0.0f, 0.0f, 1.0f);
    rf_gfx_translatef(-origin.x, -origin.y, 0.0f);

    rf_gfx_begin(RF_QUADS);
    rf_gfx_normal3f(0.0f, 0.0f, 1.0f);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(0.0f, 0.0f);

    rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(0.0f, rec.height);

    rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(rec.width, rec.height);

    rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(rec.width, 0.0f);
    rf_gfx_end();
    rf_gfx_pop_matrix();

    rf_gfx_disable_texture();
}

// Draw a vertical-gradient-filled rectangle
// NOTE: Gradient goes from bottom (color1) to top (color2)
rf_public void rf_draw_rectangle_gradient_v(int pos_x, int pos_y, int width, int height, rf_color color1, rf_color color2)
{
    rf_draw_rectangle_gradient((rf_rec) {(float)pos_x, (float)pos_y, (float)width, (float)height }, color1, color2, color2, color1);
}

// Draw a horizontal-gradient-filled rectangle
// NOTE: Gradient goes from bottom (color1) to top (color2)
rf_public void rf_draw_rectangle_gradient_h(int pos_x, int pos_y, int width, int height, rf_color color1, rf_color color2)
{
    rf_draw_rectangle_gradient((rf_rec) {(float)pos_x, (float)pos_y, (float)width, (float)height }, color1, color1, color2, color2);
}

// Draw a gradient-filled rectangle
// NOTE: Colors refer to corners, starting at top-lef corner and counter-clockwise
rf_public void rf_draw_rectangle_gradient(rf_rec rec, rf_color col1, rf_color col2, rf_color col3, rf_color col4)
{
    rf_gfx_enable_texture(rf_get_shapes_texture().id);

    rf_gfx_push_matrix();
    rf_gfx_begin(RF_QUADS);
    rf_gfx_normal3f(0.0f, 0.0f, 1.0f);

    // NOTE: Default raylib font character 95 is a white square
    rf_gfx_color4ub(col1.r, col1.g, col1.b, col1.a);
    rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(rec.x, rec.y);

    rf_gfx_color4ub(col2.r, col2.g, col2.b, col2.a);
    rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(rec.x, rec.y + rec.height);

    rf_gfx_color4ub(col3.r, col3.g, col3.b, col3.a);
    rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(rec.x + rec.width, rec.y + rec.height);

    rf_gfx_color4ub(col4.r, col4.g, col4.b, col4.a);
    rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
    rf_gfx_vertex2f(rec.x + rec.width, rec.y);
    rf_gfx_end();
    rf_gfx_pop_matrix();

    rf_gfx_disable_texture();
}

// Draw rectangle outline with extended parameters
rf_public void rf_draw_rectangle_outline(rf_rec rec, int line_thick, rf_color color)
{
    if (line_thick > rec.width || line_thick > rec.height)
    {
        if (rec.width > rec.height) line_thick = (int)rec.height/2;
        else if (rec.width < rec.height) line_thick = (int)rec.width/2;
    }

    rf_draw_rectangle_pro((rf_rec) {(int)rec.x, (int)rec.y, (int)rec.width, line_thick }, (rf_vec2){0.0f, 0.0f}, 0.0f, color);
    rf_draw_rectangle_pro((rf_rec) {(int)(rec.x - line_thick + rec.width), (int)(rec.y + line_thick), line_thick, (int)(rec.height - line_thick * 2.0f) }, (rf_vec2){0.0f, 0.0f}, 0.0f, color);
    rf_draw_rectangle_pro((rf_rec) {(int)rec.x, (int)(rec.y + rec.height - line_thick), (int)rec.width, line_thick }, (rf_vec2){0.0f, 0.0f}, 0.0f, color);
    rf_draw_rectangle_pro((rf_rec) {(int)rec.x, (int)(rec.y + line_thick), line_thick, (int)(rec.height - line_thick * 2) }, (rf_vec2) {0.0f, 0.0f }, 0.0f, color);
}

// Draw rectangle with rounded edges
rf_public void rf_draw_rectangle_rounded(rf_rec rec, float roundness, int segments, rf_color color)
{
    // Not a rounded rectangle
    if ((roundness <= 0.0f) || (rec.width < 1) || (rec.height < 1 ))
    {
        rf_draw_rectangle_pro(rec, (rf_vec2){0.0f, 0.0f}, 0.0f, color);
        return;
    }

    if (roundness >= 1.0f) roundness = 1.0f;

    // Calculate corner radius
    float radius = (rec.width > rec.height)? (rec.height*roundness)/2 : (rec.width*roundness)/2;
    if (radius <= 0.0f) return;

    // Calculate number of segments to use for the corners
    if (segments < 4)
    {
        // Calculate how many segments we need to draw a smooth circle, taken from https://stackoverflow.com/a/2244088

        float CIRCLE_ERROR_RATE = 0.5f;

        // Calculate the maximum angle between segments based on the error rate.
        float th = acosf(2*powf(1 - CIRCLE_ERROR_RATE/radius, 2) - 1);
        segments = ceilf(2 * rf_pi / th) / 4;
        if (segments <= 0) segments = 4;
    }

    float step_length = 90.0f/(float)segments;

    /*  Quick sketch to make sense of all of this (there are 9 parts to draw, also mark the 12 points we'll use below)
     *  Not my best attempt at ASCII art, just preted it's rounded rectangle :)
     *     P0                    P1
     *       ____________________
     *     /|                    |\
     *    /1|          2         |3\
     *P7 /__|____________________|__\ P2
     *  /   |                    |  _\ P2
     *  |   |P8                P9|   |
     *  | 8 |          9         | 4 |
     *  | __|____________________|__ |
     *P6 \  |P11              P10|  / P3
     *    \7|          6         |5/
     *     \|____________________|/
     *     P5                    P4
     */

    const rf_vec2 point[12] = { // coordinates of the 12 points that define the rounded rect (the idea here is to make things easier)
            {(float)rec.x + radius, rec.y}, {(float)(rec.x + rec.width) - radius, rec.y}, { rec.x + rec.width, (float)rec.y + radius }, // PO, P1, P2
            {rec.x + rec.width, (float)(rec.y + rec.height) - radius}, {(float)(rec.x + rec.width) - radius, rec.y + rec.height}, // P3, P4
            {(float)rec.x + radius, rec.y + rec.height}, { rec.x, (float)(rec.y + rec.height) - radius}, {rec.x, (float)rec.y + radius}, // P5, P6, P7
            {(float)rec.x + radius, (float)rec.y + radius}, {(float)(rec.x + rec.width) - radius, (float)rec.y + radius}, // P8, P9
            {(float)(rec.x + rec.width) - radius, (float)(rec.y + rec.height) - radius}, {(float)rec.x + radius, (float)(rec.y + rec.height) - radius} // P10, P11
    };

    const rf_vec2 centers[4] = {point[8], point[9], point[10], point[11] };
    const float angles[4] = { 180.0f, 90.0f, 0.0f, 270.0f };
    if (rf_gfx_check_buffer_limit(12*segments + 5*6)) rf_gfx_draw(); // 4 corners with 3 vertices per segment + 5 rectangles with 6 vertices each

    rf_gfx_begin(RF_TRIANGLES);
    // Draw all of the 4 corners: [1] Upper Left Corner, [3] Upper Right Corner, [5] Lower Right Corner, [7] Lower Left Corner
    for (rf_int k = 0; k < 4; ++k) // Hope the compiler is smart enough to unroll this loop
    {
        float angle = angles[k];
        const rf_vec2 center = centers[k];
        for (rf_int i = 0; i < segments; i++)
        {
            rf_gfx_color4ub(color.r, color.g, color.b, color.a);
            rf_gfx_vertex2f(center.x, center.y);
            rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * radius, center.y + cosf(rf_deg2rad * angle) * radius);
            rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * (angle + step_length)) * radius, center.y + cosf(rf_deg2rad * (angle + step_length)) * radius);
            angle += step_length;
        }
    }

    // [2] Upper rf_rec
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2f(point[0].x, point[0].y);
    rf_gfx_vertex2f(point[8].x, point[8].y);
    rf_gfx_vertex2f(point[9].x, point[9].y);
    rf_gfx_vertex2f(point[1].x, point[1].y);
    rf_gfx_vertex2f(point[0].x, point[0].y);
    rf_gfx_vertex2f(point[9].x, point[9].y);

    // [4] Right rf_rec
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2f(point[9].x, point[9].y);
    rf_gfx_vertex2f(point[10].x, point[10].y);
    rf_gfx_vertex2f(point[3].x, point[3].y);
    rf_gfx_vertex2f(point[2].x, point[2].y);
    rf_gfx_vertex2f(point[9].x, point[9].y);
    rf_gfx_vertex2f(point[3].x, point[3].y);

    // [6] Bottom rf_rec
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2f(point[11].x, point[11].y);
    rf_gfx_vertex2f(point[5].x, point[5].y);
    rf_gfx_vertex2f(point[4].x, point[4].y);
    rf_gfx_vertex2f(point[10].x, point[10].y);
    rf_gfx_vertex2f(point[11].x, point[11].y);
    rf_gfx_vertex2f(point[4].x, point[4].y);

    // [8] Left rf_rec
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2f(point[7].x, point[7].y);
    rf_gfx_vertex2f(point[6].x, point[6].y);
    rf_gfx_vertex2f(point[11].x, point[11].y);
    rf_gfx_vertex2f(point[8].x, point[8].y);
    rf_gfx_vertex2f(point[7].x, point[7].y);
    rf_gfx_vertex2f(point[11].x, point[11].y);

    // [9] Middle rf_rec
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2f(point[8].x, point[8].y);
    rf_gfx_vertex2f(point[11].x, point[11].y);
    rf_gfx_vertex2f(point[10].x, point[10].y);
    rf_gfx_vertex2f(point[9].x, point[9].y);
    rf_gfx_vertex2f(point[8].x, point[8].y);
    rf_gfx_vertex2f(point[10].x, point[10].y);
    rf_gfx_end();

}

// Draw rectangle with rounded edges outline
rf_public void rf_draw_rectangle_rounded_lines(rf_rec rec, float roundness, int segments, int line_thick, rf_color color)
{
    if (line_thick < 0) line_thick = 0;

    // Not a rounded rectangle
    if (roundness <= 0.0f)
    {
        rf_draw_rectangle_outline((rf_rec) {rec.x - line_thick, rec.y - line_thick, rec.width + 2 * line_thick, rec.height + 2 * line_thick}, line_thick, color);
        return;
    }

    if (roundness >= 1.0f) roundness = 1.0f;

    // Calculate corner radius
    float radius = (rec.width > rec.height)? (rec.height*roundness)/2 : (rec.width*roundness)/2;
    if (radius <= 0.0f) return;

    // Calculate number of segments to use for the corners
    if (segments < 4)
    {
        // Calculate how many segments we need to draw a smooth circle, taken from https://stackoverflow.com/a/2244088

        float CIRCLE_ERROR_RATE = 0.5f;

        // Calculate the maximum angle between segments based on the error rate.
        float th = acosf(2*powf(1 - CIRCLE_ERROR_RATE/radius, 2) - 1);
        segments = ceilf(2 * rf_pi / th) / 2;
        if (segments <= 0) segments = 4;
    }

    float step_length = 90.0f/(float)segments;
    const float outer_radius = radius + (float)line_thick, inner_radius = radius;

    /*  Quick sketch to make sense of all of this (mark the 16 + 4(corner centers P16-19) points we'll use below)
     *  Not my best attempt at ASCII art, just preted it's rounded rectangle :)
     *     P0                     P1
     *        ====================
     *     // P8                P9 \
     *    //                        \
     *P7 // P15                  P10 \\ P2
        \\ P2
     *  ||   *P16             P17*    ||
     *  ||                            ||
     *  || P14                   P11  ||
     *P6 \\  *P19             P18*   // P3
     *    \\                        //
     *     \\ P13              P12 //
     *        ====================
     *     P5                     P4

     */
    const rf_vec2 point[16] =
            {
                    {(float)rec.x + inner_radius, rec.y - line_thick}, {(float)(rec.x + rec.width) - inner_radius, rec.y - line_thick}, { rec.x + rec.width + line_thick, (float)rec.y + inner_radius }, // PO, P1, P2
                    {rec.x + rec.width + line_thick, (float)(rec.y + rec.height) - inner_radius}, {(float)(rec.x + rec.width) - inner_radius, rec.y + rec.height + line_thick}, // P3, P4
                    {(float)rec.x + inner_radius, rec.y + rec.height + line_thick}, { rec.x - line_thick, (float)(rec.y + rec.height) - inner_radius}, {rec.x - line_thick, (float)rec.y + inner_radius}, // P5, P6, P7
                    {(float)rec.x + inner_radius, rec.y}, {(float)(rec.x + rec.width) - inner_radius, rec.y}, // P8, P9
                    { rec.x + rec.width, (float)rec.y + inner_radius }, {rec.x + rec.width, (float)(rec.y + rec.height) - inner_radius}, // P10, P11
                    {(float)(rec.x + rec.width) - inner_radius, rec.y + rec.height}, {(float)rec.x + inner_radius, rec.y + rec.height}, // P12, P13
                    { rec.x, (float)(rec.y + rec.height) - inner_radius}, {rec.x, (float)rec.y + inner_radius} // P14, P15
            };

    const rf_vec2 centers[4] =
            {
                    {(float)rec.x + inner_radius, (float)rec.y + inner_radius}, {(float)(rec.x + rec.width) - inner_radius, (float)rec.y + inner_radius}, // P16, P17
                    {(float)(rec.x + rec.width) - inner_radius, (float)(rec.y + rec.height) - inner_radius}, {(float)rec.x + inner_radius, (float)(rec.y + rec.height) - inner_radius} // P18, P19
            };

    const float angles[4] = { 180.0f, 90.0f, 0.0f, 270.0f };

    if (line_thick > 1)
    {
        if (rf_gfx_check_buffer_limit(4 * 6*segments + 4 * 6)) rf_gfx_draw(); // 4 corners with 6(2 * 3) vertices for each segment + 4 rectangles with 6 vertices each

        rf_gfx_begin(RF_TRIANGLES);

        // Draw all of the 4 corners first: Upper Left Corner, Upper Right Corner, Lower Right Corner, Lower Left Corner
        for (rf_int k = 0; k < 4; ++k) // Hope the compiler is smart enough to unroll this loop
        {
            float angle = angles[k];
            const rf_vec2 center = centers[k];

            for (rf_int i = 0; i < segments; i++)
            {
                rf_gfx_color4ub(color.r, color.g, color.b, color.a);

                rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * inner_radius, center.y + cosf(rf_deg2rad * angle) * inner_radius);
                rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * outer_radius, center.y + cosf(rf_deg2rad * angle) * outer_radius);
                rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * (angle + step_length)) * inner_radius, center.y + cosf(rf_deg2rad * (angle + step_length)) * inner_radius);

                rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * (angle + step_length)) * inner_radius, center.y + cosf(rf_deg2rad * (angle + step_length)) * inner_radius);
                rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * outer_radius, center.y + cosf(rf_deg2rad * angle) * outer_radius);
                rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * (angle + step_length)) * outer_radius, center.y + cosf(rf_deg2rad * (angle + step_length)) * outer_radius);

                angle += step_length;
            }
        }

        // Upper rectangle
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(point[0].x, point[0].y);
        rf_gfx_vertex2f(point[8].x, point[8].y);
        rf_gfx_vertex2f(point[9].x, point[9].y);
        rf_gfx_vertex2f(point[1].x, point[1].y);
        rf_gfx_vertex2f(point[0].x, point[0].y);
        rf_gfx_vertex2f(point[9].x, point[9].y);

        // Right rectangle
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(point[10].x, point[10].y);
        rf_gfx_vertex2f(point[11].x, point[11].y);
        rf_gfx_vertex2f(point[3].x, point[3].y);
        rf_gfx_vertex2f(point[2].x, point[2].y);
        rf_gfx_vertex2f(point[10].x, point[10].y);
        rf_gfx_vertex2f(point[3].x, point[3].y);

        // Lower rectangle
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(point[13].x, point[13].y);
        rf_gfx_vertex2f(point[5].x, point[5].y);
        rf_gfx_vertex2f(point[4].x, point[4].y);
        rf_gfx_vertex2f(point[12].x, point[12].y);
        rf_gfx_vertex2f(point[13].x, point[13].y);
        rf_gfx_vertex2f(point[4].x, point[4].y);

        // Left rectangle
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);
        rf_gfx_vertex2f(point[7].x, point[7].y);
        rf_gfx_vertex2f(point[6].x, point[6].y);
        rf_gfx_vertex2f(point[14].x, point[14].y);
        rf_gfx_vertex2f(point[15].x, point[15].y);
        rf_gfx_vertex2f(point[7].x, point[7].y);
        rf_gfx_vertex2f(point[14].x, point[14].y);
        rf_gfx_end();

    }
    else
    {
        // Use LINES to draw the outline
        if (rf_gfx_check_buffer_limit(8*segments + 4 * 2)) rf_gfx_draw(); // 4 corners with 2 vertices for each segment + 4 rectangles with 2 vertices each

        rf_gfx_begin(RF_LINES);

        // Draw all of the 4 corners first: Upper Left Corner, Upper Right Corner, Lower Right Corner, Lower Left Corner
        for (rf_int k = 0; k < 4; ++k) // Hope the compiler is smart enough to unroll this loop
        {
            float angle = angles[k];
            const rf_vec2 center = centers[k];

            for (rf_int i = 0; i < segments; i++)
            {
                rf_gfx_color4ub(color.r, color.g, color.b, color.a);
                rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * angle) * outer_radius, center.y + cosf(rf_deg2rad * angle) * outer_radius);
                rf_gfx_vertex2f(center.x + sinf(rf_deg2rad * (angle + step_length)) * outer_radius, center.y + cosf(rf_deg2rad * (angle + step_length)) * outer_radius);
                angle += step_length;
            }
        }
        // And now the remaining 4 lines
        for(int i = 0; i < 8; i += 2)
        {
            rf_gfx_color4ub(color.r, color.g, color.b, color.a);
            rf_gfx_vertex2f(point[i].x, point[i].y);
            rf_gfx_vertex2f(point[i + 1].x, point[i + 1].y);
        }
        rf_gfx_end();
    }
}

// Draw a triangle
// NOTE: Vertex must be provided in counter-clockwise order
rf_public void rf_draw_triangle(rf_vec2 v1, rf_vec2 v2, rf_vec2 v3, rf_color color)
{
    if (rf_gfx_check_buffer_limit(4)) rf_gfx_draw();
    rf_gfx_begin(RF_TRIANGLES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2f(v1.x, v1.y);
    rf_gfx_vertex2f(v2.x, v2.y);
    rf_gfx_vertex2f(v3.x, v3.y);
    rf_gfx_end();

}

// Draw a triangle using lines
// NOTE: Vertex must be provided in counter-clockwise order
rf_public void rf_draw_triangle_lines(rf_vec2 v1, rf_vec2 v2, rf_vec2 v3, rf_color color)
{
    if (rf_gfx_check_buffer_limit(6)) rf_gfx_draw();

    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex2f(v1.x, v1.y);
    rf_gfx_vertex2f(v2.x, v2.y);

    rf_gfx_vertex2f(v2.x, v2.y);
    rf_gfx_vertex2f(v3.x, v3.y);

    rf_gfx_vertex2f(v3.x, v3.y);
    rf_gfx_vertex2f(v1.x, v1.y);
    rf_gfx_end();
}

// Draw a triangle fan defined by points
// NOTE: First vertex provided is the center, shared by all triangles
rf_public void rf_draw_triangle_fan(rf_vec2 *points, int points_count, rf_color color)
{
    if (points_count >= 3)
    {
        if (rf_gfx_check_buffer_limit((points_count - 2) * 4)) rf_gfx_draw();

        rf_gfx_enable_texture(rf_get_shapes_texture().id);
        rf_gfx_begin(RF_QUADS);
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        for (rf_int i = 1; i < points_count - 1; i++)
        {
            rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
            rf_gfx_vertex2f(points[0].x, points[0].y);

            rf_gfx_tex_coord2f(rf_ctx.rec_tex_shapes.x / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
            rf_gfx_vertex2f(points[i].x, points[i].y);

            rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, (rf_ctx.rec_tex_shapes.y + rf_ctx.rec_tex_shapes.height) / rf_ctx.tex_shapes.height);
            rf_gfx_vertex2f(points[i + 1].x, points[i + 1].y);

            rf_gfx_tex_coord2f((rf_ctx.rec_tex_shapes.x + rf_ctx.rec_tex_shapes.width) / rf_ctx.tex_shapes.width, rf_ctx.rec_tex_shapes.y / rf_ctx.tex_shapes.height);
            rf_gfx_vertex2f(points[i + 1].x, points[i + 1].y);
        }
        rf_gfx_end();
        rf_gfx_disable_texture();
    }
}

// Draw a triangle strip defined by points
// NOTE: Every new vertex connects with previous two
rf_public void rf_draw_triangle_strip(rf_vec2 *points, int points_count, rf_color color)
{
    if (points_count >= 3)
    {
        if (rf_gfx_check_buffer_limit(points_count)) rf_gfx_draw();

        rf_gfx_begin(RF_TRIANGLES);
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        for (rf_int i = 2; i < points_count; i++)
        {
            if ((i%2) == 0)
            {
                rf_gfx_vertex2f(points[i].x, points[i].y);
                rf_gfx_vertex2f(points[i - 2].x, points[i - 2].y);
                rf_gfx_vertex2f(points[i - 1].x, points[i - 1].y);
            }
            else
            {
                rf_gfx_vertex2f(points[i].x, points[i].y);
                rf_gfx_vertex2f(points[i - 1].x, points[i - 1].y);
                rf_gfx_vertex2f(points[i - 2].x, points[i - 2].y);
            }
        }
        rf_gfx_end();
    }
}

// Draw a regular polygon of n sides (Vector version)
rf_public void rf_draw_poly(rf_vec2 center, int sides, float radius, float rotation, rf_color color)
{
    if (sides < 3) sides = 3;
    float centralAngle = 0.0f;

    if (rf_gfx_check_buffer_limit(4 * (360/sides))) rf_gfx_draw();

    rf_gfx_push_matrix();
    rf_gfx_translatef(center.x, center.y, 0.0f);
    rf_gfx_rotatef(rotation, 0.0f, 0.0f, 1.0f);
    rf_gfx_begin(RF_TRIANGLES);
    for (rf_int i = 0; i < sides; i++)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        rf_gfx_vertex2f(0, 0);
        rf_gfx_vertex2f(sinf(rf_deg2rad * centralAngle) * radius, cosf(rf_deg2rad * centralAngle) * radius);

        centralAngle += 360.0f/(float)sides;
        rf_gfx_vertex2f(sinf(rf_deg2rad * centralAngle) * radius, cosf(rf_deg2rad * centralAngle) * radius);
    }
    rf_gfx_end();

    rf_gfx_pop_matrix();
}

// Draw a rf_texture2d with extended parameters
rf_public void rf_draw_texture(rf_texture2d texture, int x, int y, rf_color tint)
{
    rf_draw_texture_ex(texture, x, y, texture.width, texture.height, 0, tint);
}

// Draw a rf_texture2d with extended parameters
rf_public void rf_draw_texture_ex(rf_texture2d texture, int x, int y, int w, int h, float rotation, rf_color tint)
{
    rf_rec source_rec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    rf_rec dest_rec = { x, y, w, h };
    rf_vec2 origin = { 0.0f, 0.0f };

    rf_draw_texture_region(texture, source_rec, dest_rec, origin, rotation, tint);
}

// Draw a part of a texture (defined by a rectangle) with 'pro' parameters. Note: origin is relative to destination rectangle size
rf_public void rf_draw_texture_region(rf_texture2d texture, rf_rec source_rec, rf_rec dest_rec, rf_vec2 origin, float rotation, rf_color tint)
{
    // Check if texture is valid
    if (texture.id > 0)
    {
        float width = (float)texture.width;
        float height = (float)texture.height;

        rf_bool flip_x = false;

        if (source_rec.width < 0) { flip_x = true; source_rec.width *= -1; }
        if (source_rec.height < 0) source_rec.y -= source_rec.height;

        rf_gfx_enable_texture(texture.id);

        rf_gfx_push_matrix();
            rf_gfx_translatef(dest_rec.x, dest_rec.y, 0.0f);
            rf_gfx_rotatef(rotation, 0.0f, 0.0f, 1.0f);
            rf_gfx_translatef(-origin.x, -origin.y, 0.0f);

            rf_gfx_begin(RF_QUADS);
                rf_gfx_color4ub(tint.r, tint.g, tint.b, tint.a);
                rf_gfx_normal3f(0.0f, 0.0f, 1.0f); // Normal vector pointing towards viewer

                // Bottom-left corner for texture and quad
                if (flip_x) rf_gfx_tex_coord2f((source_rec.x + source_rec.width) / width, source_rec.y / height);
                else rf_gfx_tex_coord2f(source_rec.x / width, source_rec.y / height);
                rf_gfx_vertex2f(0.0f, 0.0f);

                // Bottom-right corner for texture and quad
                if (flip_x) rf_gfx_tex_coord2f((source_rec.x + source_rec.width) / width, (source_rec.y + source_rec.height) / height);
                else rf_gfx_tex_coord2f(source_rec.x / width, (source_rec.y + source_rec.height) / height);
                rf_gfx_vertex2f(0.0f, dest_rec.height);

                // Top-right corner for texture and quad
                if (flip_x) rf_gfx_tex_coord2f(source_rec.x / width, (source_rec.y + source_rec.height) / height);
                else rf_gfx_tex_coord2f((source_rec.x + source_rec.width) / width, (source_rec.y + source_rec.height) / height);
                rf_gfx_vertex2f(dest_rec.width, dest_rec.height);

                // Top-left corner for texture and quad
                if (flip_x) rf_gfx_tex_coord2f(source_rec.x / width, source_rec.y / height);
                else rf_gfx_tex_coord2f((source_rec.x + source_rec.width) / width, source_rec.y / height);
                rf_gfx_vertex2f(dest_rec.width, 0.0f);
            rf_gfx_end();
        rf_gfx_pop_matrix();

        rf_gfx_disable_texture();
    }
}

// Draws a texture (or part of it) that stretches or shrinks nicely using n-patch info
rf_public void rf_draw_texture_npatch(rf_texture2d texture, rf_npatch_info n_patch_info, rf_rec dest_rec, rf_vec2 origin, float rotation, rf_color tint)
{
    if (texture.id > 0)
    {
        float width = (float)texture.width;
        float height = (float)texture.height;

        float patch_width = (dest_rec.width <= 0.0f)? 0.0f : dest_rec.width;
        float patch_height = (dest_rec.height <= 0.0f)? 0.0f : dest_rec.height;

        if (n_patch_info.source_rec.width < 0) n_patch_info.source_rec.x -= n_patch_info.source_rec.width;
        if (n_patch_info.source_rec.height < 0) n_patch_info.source_rec.y -= n_patch_info.source_rec.height;
        if (n_patch_info.type == RF_NPT_3PATCH_HORIZONTAL) patch_height = n_patch_info.source_rec.height;
        if (n_patch_info.type == RF_NPT_3PATCH_VERTICAL) patch_width = n_patch_info.source_rec.width;

        rf_bool draw_center = true;
        rf_bool draw_middle = true;
        float left_border = (float)n_patch_info.left;
        float top_border = (float)n_patch_info.top;
        float right_border = (float)n_patch_info.right;
        float bottom_border = (float)n_patch_info.bottom;

        // adjust the lateral (left and right) border widths in case patch_width < texture.width
        if (patch_width <= (left_border + right_border) && n_patch_info.type != RF_NPT_3PATCH_VERTICAL)
        {
            draw_center = false;
            left_border = (left_border / (left_border + right_border)) * patch_width;
            right_border = patch_width - left_border;
        }
        // adjust the lateral (top and bottom) border heights in case patch_height < texture.height
        if (patch_height <= (top_border + bottom_border) && n_patch_info.type != RF_NPT_3PATCH_HORIZONTAL)
        {
            draw_middle = false;
            top_border = (top_border / (top_border + bottom_border)) * patch_height;
            bottom_border = patch_height - top_border;
        }

        rf_vec2 vert_a, vert_b, vert_c, vert_d;
        vert_a.x = 0.0f; // outer left
        vert_a.y = 0.0f; // outer top
        vert_b.x = left_border; // inner left
        vert_b.y = top_border; // inner top
        vert_c.x = patch_width - right_border; // inner right
        vert_c.y = patch_height - bottom_border; // inner bottom
        vert_d.x = patch_width; // outer right
        vert_d.y = patch_height; // outer bottom

        rf_vec2 coord_a, coord_b, coord_c, coord_d;
        coord_a.x = n_patch_info.source_rec.x / width;
        coord_a.y = n_patch_info.source_rec.y / height;
        coord_b.x = (n_patch_info.source_rec.x + left_border) / width;
        coord_b.y = (n_patch_info.source_rec.y + top_border) / height;
        coord_c.x = (n_patch_info.source_rec.x + n_patch_info.source_rec.width - right_border) / width;
        coord_c.y = (n_patch_info.source_rec.y + n_patch_info.source_rec.height - bottom_border) / height;
        coord_d.x = (n_patch_info.source_rec.x + n_patch_info.source_rec.width) / width;
        coord_d.y = (n_patch_info.source_rec.y + n_patch_info.source_rec.height) / height;

        rf_gfx_enable_texture(texture.id);

        rf_gfx_push_matrix();
        rf_gfx_translatef(dest_rec.x, dest_rec.y, 0.0f);
        rf_gfx_rotatef(rotation, 0.0f, 0.0f, 1.0f);
        rf_gfx_translatef(-origin.x, -origin.y, 0.0f);

        rf_gfx_begin(RF_QUADS);
        rf_gfx_color4ub(tint.r, tint.g, tint.b, tint.a);
        rf_gfx_normal3f(0.0f, 0.0f, 1.0f); // Normal vector pointing towards viewer

        if (n_patch_info.type == RF_NPT_9PATCH)
        {
            // ------------------------------------------------------------
            // TOP-LEFT QUAD
            rf_gfx_tex_coord2f(coord_a.x, coord_b.y); rf_gfx_vertex2f(vert_a.x, vert_b.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_b.x, coord_b.y); rf_gfx_vertex2f(vert_b.x, vert_b.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_b.x, coord_a.y); rf_gfx_vertex2f(vert_b.x, vert_a.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_a.x, coord_a.y); rf_gfx_vertex2f(vert_a.x, vert_a.y); // Top-left corner for texture and quad
            if (draw_center)
            {
                // TOP-CENTER QUAD
                rf_gfx_tex_coord2f(coord_b.x, coord_b.y); rf_gfx_vertex2f(vert_b.x, vert_b.y); // Bottom-left corner for texture and quad
                rf_gfx_tex_coord2f(coord_c.x, coord_b.y); rf_gfx_vertex2f(vert_c.x, vert_b.y); // Bottom-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_c.x, coord_a.y); rf_gfx_vertex2f(vert_c.x, vert_a.y); // Top-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_b.x, coord_a.y); rf_gfx_vertex2f(vert_b.x, vert_a.y); // Top-left corner for texture and quad
            }
            // TOP-RIGHT QUAD
            rf_gfx_tex_coord2f(coord_c.x, coord_b.y); rf_gfx_vertex2f(vert_c.x, vert_b.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_b.y); rf_gfx_vertex2f(vert_d.x, vert_b.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_a.y); rf_gfx_vertex2f(vert_d.x, vert_a.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_c.x, coord_a.y); rf_gfx_vertex2f(vert_c.x, vert_a.y); // Top-left corner for texture and quad
            if (draw_middle)
            {
                // ------------------------------------------------------------
                // MIDDLE-LEFT QUAD
                rf_gfx_tex_coord2f(coord_a.x, coord_c.y); rf_gfx_vertex2f(vert_a.x, vert_c.y); // Bottom-left corner for texture and quad
                rf_gfx_tex_coord2f(coord_b.x, coord_c.y); rf_gfx_vertex2f(vert_b.x, vert_c.y); // Bottom-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_b.x, coord_b.y); rf_gfx_vertex2f(vert_b.x, vert_b.y); // Top-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_a.x, coord_b.y); rf_gfx_vertex2f(vert_a.x, vert_b.y); // Top-left corner for texture and quad
                if (draw_center)
                {
                    // MIDDLE-CENTER QUAD
                    rf_gfx_tex_coord2f(coord_b.x, coord_c.y); rf_gfx_vertex2f(vert_b.x, vert_c.y); // Bottom-left corner for texture and quad
                    rf_gfx_tex_coord2f(coord_c.x, coord_c.y); rf_gfx_vertex2f(vert_c.x, vert_c.y); // Bottom-right corner for texture and quad
                    rf_gfx_tex_coord2f(coord_c.x, coord_b.y); rf_gfx_vertex2f(vert_c.x, vert_b.y); // Top-right corner for texture and quad
                    rf_gfx_tex_coord2f(coord_b.x, coord_b.y); rf_gfx_vertex2f(vert_b.x, vert_b.y); // Top-left corner for texture and quad
                }

                // MIDDLE-RIGHT QUAD
                rf_gfx_tex_coord2f(coord_c.x, coord_c.y); rf_gfx_vertex2f(vert_c.x, vert_c.y); // Bottom-left corner for texture and quad
                rf_gfx_tex_coord2f(coord_d.x, coord_c.y); rf_gfx_vertex2f(vert_d.x, vert_c.y); // Bottom-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_d.x, coord_b.y); rf_gfx_vertex2f(vert_d.x, vert_b.y); // Top-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_c.x, coord_b.y); rf_gfx_vertex2f(vert_c.x, vert_b.y); // Top-left corner for texture and quad
            }

            // ------------------------------------------------------------
            // BOTTOM-LEFT QUAD
            rf_gfx_tex_coord2f(coord_a.x, coord_d.y); rf_gfx_vertex2f(vert_a.x, vert_d.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_b.x, coord_d.y); rf_gfx_vertex2f(vert_b.x, vert_d.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_b.x, coord_c.y); rf_gfx_vertex2f(vert_b.x, vert_c.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_a.x, coord_c.y); rf_gfx_vertex2f(vert_a.x, vert_c.y); // Top-left corner for texture and quad
            if (draw_center)
            {
                // BOTTOM-CENTER QUAD
                rf_gfx_tex_coord2f(coord_b.x, coord_d.y); rf_gfx_vertex2f(vert_b.x, vert_d.y); // Bottom-left corner for texture and quad
                rf_gfx_tex_coord2f(coord_c.x, coord_d.y); rf_gfx_vertex2f(vert_c.x, vert_d.y); // Bottom-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_c.x, coord_c.y); rf_gfx_vertex2f(vert_c.x, vert_c.y); // Top-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_b.x, coord_c.y); rf_gfx_vertex2f(vert_b.x, vert_c.y); // Top-left corner for texture and quad
            }

            // BOTTOM-RIGHT QUAD
            rf_gfx_tex_coord2f(coord_c.x, coord_d.y); rf_gfx_vertex2f(vert_c.x, vert_d.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_d.y); rf_gfx_vertex2f(vert_d.x, vert_d.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_c.y); rf_gfx_vertex2f(vert_d.x, vert_c.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_c.x, coord_c.y); rf_gfx_vertex2f(vert_c.x, vert_c.y); // Top-left corner for texture and quad
        }
        else if (n_patch_info.type == RF_NPT_3PATCH_VERTICAL)
        {
            // TOP QUAD
            // -----------------------------------------------------------
            // rf_texture coords                 Vertices
            rf_gfx_tex_coord2f(coord_a.x, coord_b.y); rf_gfx_vertex2f(vert_a.x, vert_b.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_b.y); rf_gfx_vertex2f(vert_d.x, vert_b.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_a.y); rf_gfx_vertex2f(vert_d.x, vert_a.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_a.x, coord_a.y); rf_gfx_vertex2f(vert_a.x, vert_a.y); // Top-left corner for texture and quad
            if (draw_center)
            {
                // MIDDLE QUAD
                // -----------------------------------------------------------
                // rf_texture coords                 Vertices
                rf_gfx_tex_coord2f(coord_a.x, coord_c.y); rf_gfx_vertex2f(vert_a.x, vert_c.y); // Bottom-left corner for texture and quad
                rf_gfx_tex_coord2f(coord_d.x, coord_c.y); rf_gfx_vertex2f(vert_d.x, vert_c.y); // Bottom-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_d.x, coord_b.y); rf_gfx_vertex2f(vert_d.x, vert_b.y); // Top-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_a.x, coord_b.y); rf_gfx_vertex2f(vert_a.x, vert_b.y); // Top-left corner for texture and quad
            }
            // BOTTOM QUAD
            // -----------------------------------------------------------
            // rf_texture coords                 Vertices
            rf_gfx_tex_coord2f(coord_a.x, coord_d.y); rf_gfx_vertex2f(vert_a.x, vert_d.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_d.y); rf_gfx_vertex2f(vert_d.x, vert_d.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_c.y); rf_gfx_vertex2f(vert_d.x, vert_c.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_a.x, coord_c.y); rf_gfx_vertex2f(vert_a.x, vert_c.y); // Top-left corner for texture and quad
        }
        else if (n_patch_info.type == RF_NPT_3PATCH_HORIZONTAL)
        {
            // LEFT QUAD
            // -----------------------------------------------------------
            // rf_texture coords                 Vertices
            rf_gfx_tex_coord2f(coord_a.x, coord_d.y); rf_gfx_vertex2f(vert_a.x, vert_d.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_b.x, coord_d.y); rf_gfx_vertex2f(vert_b.x, vert_d.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_b.x, coord_a.y); rf_gfx_vertex2f(vert_b.x, vert_a.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_a.x, coord_a.y); rf_gfx_vertex2f(vert_a.x, vert_a.y); // Top-left corner for texture and quad
            if (draw_center)
            {
                // CENTER QUAD
                // -----------------------------------------------------------
                // rf_texture coords                 Vertices
                rf_gfx_tex_coord2f(coord_b.x, coord_d.y); rf_gfx_vertex2f(vert_b.x, vert_d.y); // Bottom-left corner for texture and quad
                rf_gfx_tex_coord2f(coord_c.x, coord_d.y); rf_gfx_vertex2f(vert_c.x, vert_d.y); // Bottom-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_c.x, coord_a.y); rf_gfx_vertex2f(vert_c.x, vert_a.y); // Top-right corner for texture and quad
                rf_gfx_tex_coord2f(coord_b.x, coord_a.y); rf_gfx_vertex2f(vert_b.x, vert_a.y); // Top-left corner for texture and quad
            }
            // RIGHT QUAD
            // -----------------------------------------------------------
            // rf_texture coords                 Vertices
            rf_gfx_tex_coord2f(coord_c.x, coord_d.y); rf_gfx_vertex2f(vert_c.x, vert_d.y); // Bottom-left corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_d.y); rf_gfx_vertex2f(vert_d.x, vert_d.y); // Bottom-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_d.x, coord_a.y); rf_gfx_vertex2f(vert_d.x, vert_a.y); // Top-right corner for texture and quad
            rf_gfx_tex_coord2f(coord_c.x, coord_a.y); rf_gfx_vertex2f(vert_c.x, vert_a.y); // Top-left corner for texture and quad
        }
        rf_gfx_end();
        rf_gfx_pop_matrix();

        rf_gfx_disable_texture();

    }
}

// Draw text (using default font)
rf_public void rf_draw_string(const char* text, int text_len, int posX, int posY, int fontSize, rf_color color)
{
    // Check if default font has been loaded
    if (rf_get_default_font().texture.id == 0 || text_len == 0) return;

    rf_vec2 position = { (float)posX, (float)posY };

    int defaultFontSize = 10;   // Default Font chars height in pixel
    if (fontSize < defaultFontSize) fontSize = defaultFontSize;
    int spacing = fontSize/defaultFontSize;

    rf_draw_string_ex(rf_get_default_font(), text, text_len, position, (float)fontSize, (float)spacing, color);
}

// Draw text with custom font
rf_public void rf_draw_string_ex(rf_font font, const char* text, int text_len, rf_vec2 position, float font_size, float spacing, rf_color tint)
{
    int text_offset_y = 0; // Required for line break!
    float text_offset_x = 0.0f; // Offset between characters
    float scale_factor = 0.0f;

    int letter = 0; // Current character
    int index  = 0; // Index position in sprite font

    scale_factor = font_size / font.base_size;

    for (rf_int i = 0; i < text_len; i++)
    {
        rf_decoded_rune decoded_rune = rf_decode_utf8_char(&text[i], text_len - i);
        letter = decoded_rune.codepoint;
        index = rf_get_glyph_index(font, letter);

        // NOTE: Normally we exit the decoding sequence as soon as a bad unsigned char is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set 'next = 1'
        if (letter == 0x3f) decoded_rune.bytes_processed = 1;
        i += (decoded_rune.bytes_processed - 1);

        if (letter == '\n')
        {
            // NOTE: Fixed line spacing of 1.5 lines
            text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
            text_offset_x = 0.0f;
        }
        else
        {
            if (letter != ' ')
            {
                rf_rec src_rec = font.glyphs[index].rec;
                rf_rec dst_rec = {  position.x + text_offset_x + font.glyphs[index].offset_x * scale_factor,
                                    position.y + text_offset_y + font.glyphs[index].offset_y * scale_factor,
                                    font.glyphs[index].rec.width  * scale_factor,
                                    font.glyphs[index].rec.height * scale_factor };
                rf_draw_texture_region(font.texture, src_rec, dst_rec, (rf_vec2){0}, 0.0f, tint);
            }

            if (font.glyphs[index].advance_x == 0) text_offset_x += ((float)font.glyphs[index].rec.width * scale_factor + spacing);
            else text_offset_x += ((float)font.glyphs[index].advance_x * scale_factor + spacing);
        }
    }
}

// Draw text wrapped
rf_public void rf_draw_string_wrap(rf_font font, const char* text, int text_len, rf_vec2 position, float font_size, float spacing, rf_color tint, float wrap_width, rf_text_wrap_mode mode)
{
    rf_rec rec = { 0, 0, wrap_width, FLT_MAX };
    rf_draw_string_rec(font, text, text_len, rec, font_size, spacing, mode, tint);
}

// Draw text using font inside rectangle limits
rf_public void rf_draw_string_rec(rf_font font, const char* text, int text_len, rf_rec rec, float font_size, float spacing, rf_text_wrap_mode wrap, rf_color tint)
{
    int   text_offset_x = 0; // Offset between characters
    int   text_offset_y = 0; // Required for line break!
    float scale_factor  = 0.0f;

    int letter = 0; // Current character
    int index  = 0; // Index position in sprite font

    scale_factor = font_size/font.base_size;

    enum
    {
        MEASURE_WORD_WRAP_STATE = 0,
        MEASURE_RESULT_STATE = 1
    };

    int state      = wrap == RF_WORD_WRAP ? MEASURE_WORD_WRAP_STATE : MEASURE_RESULT_STATE;
    int start_line = -1; // Index where to begin drawing (where a line begins)
    int end_line   = -1; // Index where to stop drawing (where a line ends)
    int lastk      = -1; // Holds last value of the character position

    for (rf_int i = 0, k = 0; i < text_len; i++, k++)
    {
        int glyph_width = 0;

        rf_decoded_rune decoded_rune = rf_decode_utf8_char(&text[i], text_len - i);
        letter = decoded_rune.codepoint;
        index  = rf_get_glyph_index(font, letter);

        // NOTE: normally we exit the decoding sequence as soon as a bad unsigned char is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set next = 1
        if (letter == 0x3f) decoded_rune.bytes_processed = 1;
        i += decoded_rune.bytes_processed - 1;

        if (letter != '\n')
        {
            glyph_width = (font.glyphs[index].advance_x == 0)?
                          (int)(font.glyphs[index].rec.width * scale_factor + spacing):
                          (int)(font.glyphs[index].advance_x * scale_factor + spacing);
        }

        // NOTE: When wrap is ON we first measure how much of the text we can draw before going outside of the rec container
        // We store this info in start_line and end_line, then we change states, draw the text between those two variables
        // and change states again and again recursively until the end of the text (or until we get outside of the container).
        // When wrap is OFF we don't need the measure state so we go to the drawing state immediately
        // and begin drawing on the next line before we can get outside the container.
        if (state == MEASURE_WORD_WRAP_STATE)
        {
            // TODO: there are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
            // See: http://jkorpela.fi/chars/spaces.html
            if ((letter == ' ') || (letter == '\t') || (letter == '\n')) end_line = i;

            if ((text_offset_x + glyph_width + 1) >= rec.width)
            {
                end_line = (end_line < 1)? i : end_line;
                if (i == end_line) end_line -= decoded_rune.bytes_processed;
                if ((start_line + decoded_rune.bytes_processed) == end_line) end_line = i - decoded_rune.bytes_processed;
                state = !state;
            }
            else if ((i + 1) == text_len)
            {
                end_line = i;
                state = !state;
            }
            else if (letter == '\n')
            {
                state = !state;
            }

            if (state == MEASURE_RESULT_STATE)
            {
                text_offset_x = 0;
                i = start_line;
                glyph_width = 0;

                // Save character position when we switch states
                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        }
        else
        {
            if (letter == '\n')
            {
                if (!wrap)
                {
                    text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                    text_offset_x = 0;
                }
            }
            else
            {
                if (!wrap && ((text_offset_x + glyph_width + 1) >= rec.width))
                {
                    text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                    text_offset_x = 0;
                }

                if ((text_offset_y + (int)(font.base_size*scale_factor)) > rec.height) break;

                // Draw glyph
                if ((letter != ' ') && (letter != '\t'))
                {
                    rf_draw_texture_region(font.texture, font.glyphs[index].rec,
                                           (rf_rec) {
                                               rec.x + text_offset_x + font.glyphs[index].offset_x * scale_factor,
                                               rec.y + text_offset_y + font.glyphs[index].offset_y * scale_factor,
                                               font.glyphs[index].rec.width  * scale_factor,
                                               font.glyphs[index].rec.height * scale_factor
                                           },
                                           (rf_vec2){ 0, 0 }, 0.0f, tint);
                }
            }

            if (wrap && (i == end_line))
            {
                text_offset_y += (int)((font.base_size + font.base_size/2)*scale_factor);
                text_offset_x = 0;
                start_line = end_line;
                end_line = -1;
                glyph_width = 0;
                k = lastk;
                state = !state;
            }
        }

        text_offset_x += glyph_width;
    }
}

rf_public void rf_draw_text(const char* text, int posX, int posY, int font_size, rf_color color) { rf_draw_string(text, strlen(text), posX, posY, font_size, color); }

rf_public void rf_draw_text_ex(rf_font font, const char* text, rf_vec2 position, float fontSize, float spacing, rf_color tint) { rf_draw_string_ex(font, text, strlen(text), position, fontSize, spacing, tint); }

rf_public void rf_draw_text_wrap(rf_font font, const char* text, rf_vec2 position, float font_size, float spacing, rf_color tint, float wrap_width, rf_text_wrap_mode mode) { rf_draw_string_wrap(font, text, strlen(text), position, font_size, spacing, tint, wrap_width, mode); }

rf_public void rf_draw_text_rec(rf_font font, const char* text, rf_rec rec, float font_size, float spacing, rf_text_wrap_mode wrap, rf_color tint) { rf_draw_string_rec(font, text, strlen(text), rec, font_size, spacing, wrap, tint); }

rf_public void rf_draw_line3d(rf_vec3 start_pos, rf_vec3 end_pos, rf_color color)
{
    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_vertex3f(start_pos.x, start_pos.y, start_pos.z);
    rf_gfx_vertex3f(end_pos.x, end_pos.y, end_pos.z);
    rf_gfx_end();
}

// Draw a circle in 3D world space
rf_public void rf_draw_circle3d(rf_vec3 center, float radius, rf_vec3 rotation_axis, float rotationAngle, rf_color color)
{
    if (rf_gfx_check_buffer_limit(2 * 36)) rf_gfx_draw();

    rf_gfx_push_matrix();
    rf_gfx_translatef(center.x, center.y, center.z);
    rf_gfx_rotatef(rotationAngle, rotation_axis.x, rotation_axis.y, rotation_axis.z);

    rf_gfx_begin(RF_LINES);
    for (rf_int i = 0; i < 360; i += 10)
    {
        rf_gfx_color4ub(color.r, color.g, color.b, color.a);

        rf_gfx_vertex3f(sinf(rf_deg2rad * i) * radius, cosf(rf_deg2rad * i) * radius, 0.0f);
        rf_gfx_vertex3f(sinf(rf_deg2rad * (i + 10)) * radius, cosf(rf_deg2rad * (i + 10)) * radius, 0.0f);
    }
    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw cube
// NOTE: Cube position is the center position
rf_public void rf_draw_cube(rf_vec3 position, float width, float height, float length, rf_color color)
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    if (rf_gfx_check_buffer_limit(36)) rf_gfx_draw();

    rf_gfx_push_matrix();
    // NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
    rf_gfx_translatef(position.x, position.y, position.z);
    //rf_gfx_rotatef(45, 0, 1, 0);
    //rf_gfx_scalef(1.0f, 1.0f, 1.0f);   // NOTE: Vertices are directly scaled on definition

    rf_gfx_begin(RF_TRIANGLES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    // Front face
    rf_gfx_vertex3f(x - width/2, y - height/2, z + length/2); // Bottom Left
    rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Right
    rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Top Left

    rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Top Right
    rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Top Left
    rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Right

    // Back face
    rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Bottom Left
    rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Left
    rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Bottom Right

    rf_gfx_vertex3f(x + width/2, y + height/2, z - length/2); // Top Right
    rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Bottom Right
    rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Left

    // Top face
    rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Left
    rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Bottom Left
    rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Bottom Right

    rf_gfx_vertex3f(x + width/2, y + height/2, z - length/2); // Top Right
    rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Left
    rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Bottom Right

    // Bottom face
    rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Top Left
    rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Right
    rf_gfx_vertex3f(x - width/2, y - height/2, z + length/2); // Bottom Left

    rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Top Right
    rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Right
    rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Top Left

    // Right face
    rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Bottom Right
    rf_gfx_vertex3f(x + width/2, y + height/2, z - length/2); // Top Right
    rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Top Left

    rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Left
    rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Bottom Right
    rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Top Left

    // Left face
    rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Bottom Right
    rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Top Left
    rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Right

    rf_gfx_vertex3f(x - width/2, y - height/2, z + length/2); // Bottom Left
    rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Top Left
    rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Bottom Right
    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw cube wires
rf_public void rf_draw_cube_wires(rf_vec3 position, float width, float height, float length, rf_color color)
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    if (rf_gfx_check_buffer_limit(36)) rf_gfx_draw();

    rf_gfx_push_matrix();
    rf_gfx_translatef(position.x, position.y, position.z);

    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    // Front Face -----------------------------------------------------
    // Bottom Line
    rf_gfx_vertex3f(x-width/2, y-height/2, z+length/2); // Bottom Left
    rf_gfx_vertex3f(x+width/2, y-height/2, z+length/2); // Bottom Right

    // Left Line
    rf_gfx_vertex3f(x+width/2, y-height/2, z+length/2); // Bottom Right
    rf_gfx_vertex3f(x+width/2, y+height/2, z+length/2); // Top Right

    // Top Line
    rf_gfx_vertex3f(x+width/2, y+height/2, z+length/2); // Top Right
    rf_gfx_vertex3f(x-width/2, y+height/2, z+length/2); // Top Left

    // Right Line
    rf_gfx_vertex3f(x-width/2, y+height/2, z+length/2); // Top Left
    rf_gfx_vertex3f(x-width/2, y-height/2, z+length/2); // Bottom Left

    // Back Face ------------------------------------------------------
    // Bottom Line
    rf_gfx_vertex3f(x-width/2, y-height/2, z-length/2); // Bottom Left
    rf_gfx_vertex3f(x+width/2, y-height/2, z-length/2); // Bottom Right

    // Left Line
    rf_gfx_vertex3f(x+width/2, y-height/2, z-length/2); // Bottom Right
    rf_gfx_vertex3f(x+width/2, y+height/2, z-length/2); // Top Right

    // Top Line
    rf_gfx_vertex3f(x+width/2, y+height/2, z-length/2); // Top Right
    rf_gfx_vertex3f(x-width/2, y+height/2, z-length/2); // Top Left

    // Right Line
    rf_gfx_vertex3f(x-width/2, y+height/2, z-length/2); // Top Left
    rf_gfx_vertex3f(x-width/2, y-height/2, z-length/2); // Bottom Left

    // Top Face -------------------------------------------------------
    // Left Line
    rf_gfx_vertex3f(x-width/2, y+height/2, z+length/2); // Top Left Front
    rf_gfx_vertex3f(x-width/2, y+height/2, z-length/2); // Top Left Back

    // Right Line
    rf_gfx_vertex3f(x+width/2, y+height/2, z+length/2); // Top Right Front
    rf_gfx_vertex3f(x+width/2, y+height/2, z-length/2); // Top Right Back

    // Bottom Face  ---------------------------------------------------
    // Left Line
    rf_gfx_vertex3f(x-width/2, y-height/2, z+length/2); // Top Left Front
    rf_gfx_vertex3f(x-width/2, y-height/2, z-length/2); // Top Left Back

    // Right Line
    rf_gfx_vertex3f(x+width/2, y-height/2, z+length/2); // Top Right Front
    rf_gfx_vertex3f(x+width/2, y-height/2, z-length/2); // Top Right Back
    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw cube
// NOTE: Cube position is the center position
rf_public void rf_draw_cube_texture(rf_texture2d texture, rf_vec3 position, float width, float height, float length, rf_color color)
{
    float x = position.x;
    float y = position.y;
    float z = position.z;

    if (rf_gfx_check_buffer_limit(36)) rf_gfx_draw();

    rf_gfx_enable_texture(texture.id);

    //rf_gfx_push_matrix();
    // NOTE: Transformation is applied in inverse order (scale -> rotate -> translate)
    //rf_gfx_translatef(2.0f, 0.0f, 0.0f);
    //rf_gfx_rotatef(45, 0, 1, 0);
    //rf_gfx_scalef(2.0f, 2.0f, 2.0f);

    rf_gfx_begin(RF_QUADS);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    // Front Face
    rf_gfx_normal3f(0.0f, 0.0f, 1.0f); // Normal Pointing Towards Viewer
    rf_gfx_tex_coord2f(0.0f, 0.0f); rf_gfx_vertex3f(x - width/2, y - height/2, z + length/2); // Bottom Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 0.0f); rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 1.0f); rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Top Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 1.0f); rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Top Left Of The rf_texture and Quad
    // Back Face
    rf_gfx_normal3f(0.0f, 0.0f, - 1.0f); // Normal Pointing Away From Viewer
    rf_gfx_tex_coord2f(1.0f, 0.0f); rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Bottom Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 1.0f); rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 1.0f); rf_gfx_vertex3f(x + width/2, y + height/2, z - length/2); // Top Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 0.0f); rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Bottom Left Of The rf_texture and Quad
    // Top Face
    rf_gfx_normal3f(0.0f, 1.0f, 0.0f); // Normal Pointing Up
    rf_gfx_tex_coord2f(0.0f, 1.0f); rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 0.0f); rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Bottom Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 0.0f); rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Bottom Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 1.0f); rf_gfx_vertex3f(x + width/2, y + height/2, z - length/2); // Top Right Of The rf_texture and Quad
    // Bottom Face
    rf_gfx_normal3f(0.0f, - 1.0f, 0.0f); // Normal Pointing Down
    rf_gfx_tex_coord2f(1.0f, 1.0f); rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Top Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 1.0f); rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Top Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 0.0f); rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 0.0f); rf_gfx_vertex3f(x - width/2, y - height/2, z + length/2); // Bottom Right Of The rf_texture and Quad
    // Right face
    rf_gfx_normal3f(1.0f, 0.0f, 0.0f); // Normal Pointing Right
    rf_gfx_tex_coord2f(1.0f, 0.0f); rf_gfx_vertex3f(x + width/2, y - height/2, z - length/2); // Bottom Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 1.0f); rf_gfx_vertex3f(x + width/2, y + height/2, z - length/2); // Top Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 1.0f); rf_gfx_vertex3f(x + width/2, y + height/2, z + length/2); // Top Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 0.0f); rf_gfx_vertex3f(x + width/2, y - height/2, z + length/2); // Bottom Left Of The rf_texture and Quad
    // Left Face
    rf_gfx_normal3f(-1.0f, 0.0f, 0.0f); // Normal Pointing Left
    rf_gfx_tex_coord2f(0.0f, 0.0f); rf_gfx_vertex3f(x - width/2, y - height/2, z - length/2); // Bottom Left Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 0.0f); rf_gfx_vertex3f(x - width/2, y - height/2, z + length/2); // Bottom Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(1.0f, 1.0f); rf_gfx_vertex3f(x - width/2, y + height/2, z + length/2); // Top Right Of The rf_texture and Quad
    rf_gfx_tex_coord2f(0.0f, 1.0f); rf_gfx_vertex3f(x - width/2, y + height/2, z - length/2); // Top Left Of The rf_texture and Quad
    rf_gfx_end();
    //rf_gfx_pop_matrix();

    rf_gfx_disable_texture();
}

// Draw sphere
rf_public void rf_draw_sphere(rf_vec3 center_pos, float radius, rf_color color)
{
    rf_draw_sphere_ex(center_pos, radius, 16, 16, color);
}

// Draw sphere with extended parameters
rf_public void rf_draw_sphere_ex(rf_vec3 center_pos, float radius, int rings, int slices, rf_color color)
{
    int num_vertex = (rings + 2)*slices*6;
    if (rf_gfx_check_buffer_limit(num_vertex)) rf_gfx_draw();

    rf_gfx_push_matrix();
    // NOTE: Transformation is applied in inverse order (scale -> translate)
    rf_gfx_translatef(center_pos.x, center_pos.y, center_pos.z);
    rf_gfx_scalef(radius, radius, radius);

    rf_gfx_begin(RF_TRIANGLES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    for (rf_int i = 0; i < (rings + 2); i++)
    {
        for (rf_int j = 0; j < slices; j++)
        {
            rf_gfx_vertex3f(cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * i)) * sinf(rf_deg2rad * (j * 360 / slices)),
                           sinf(rf_deg2rad * (270 + (180 / (rings + 1)) * i)),
                            cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * i)) * cosf(rf_deg2rad * (j * 360 / slices)));
            rf_gfx_vertex3f(cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))) * sinf(rf_deg2rad * ((j + 1) * 360 / slices)),
                           sinf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))),
                            cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))) * cosf(rf_deg2rad * ((j + 1) * 360 / slices)));
            rf_gfx_vertex3f(cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))) * sinf(rf_deg2rad * (j * 360 / slices)),
                           sinf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))),
                            cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))) * cosf(rf_deg2rad * (j * 360 / slices)));

            rf_gfx_vertex3f(cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * i)) * sinf(rf_deg2rad * (j * 360 / slices)),
                           sinf(rf_deg2rad * (270 + (180 / (rings + 1)) * i)),
                            cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * i)) * cosf(rf_deg2rad * (j * 360 / slices)));
            rf_gfx_vertex3f(cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i))) * sinf(rf_deg2rad * ((j + 1) * 360 / slices)),
                           sinf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i))),
                            cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i))) * cosf(rf_deg2rad * ((j + 1) * 360 / slices)));
            rf_gfx_vertex3f(cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))) * sinf(rf_deg2rad * ((j + 1) * 360 / slices)),
                           sinf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))),
                            cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))) * cosf(rf_deg2rad * ((j + 1) * 360 / slices)));
        }
    }
    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw sphere wires
rf_public void rf_draw_sphere_wires(rf_vec3 center_pos, float radius, int rings, int slices, rf_color color)
{
    int num_vertex = (rings + 2)*slices*6;
    if (rf_gfx_check_buffer_limit(num_vertex)) rf_gfx_draw();

    rf_gfx_push_matrix();
    // NOTE: Transformation is applied in inverse order (scale -> translate)
    rf_gfx_translatef(center_pos.x, center_pos.y, center_pos.z);
    rf_gfx_scalef(radius, radius, radius);

    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    for (rf_int i = 0; i < (rings + 2); i++)
    {
        for (rf_int j = 0; j < slices; j++)
        {
            rf_gfx_vertex3f(cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * i)) * sinf(rf_deg2rad * (j * 360 / slices)),
                           sinf(rf_deg2rad * (270 + (180 / (rings + 1)) * i)),
                            cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * i)) * cosf(rf_deg2rad * (j * 360 / slices)));
            rf_gfx_vertex3f(cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))) * sinf(rf_deg2rad * ((j + 1) * 360 / slices)),
                           sinf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))),
                            cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))) * cosf(rf_deg2rad * ((j + 1) * 360 / slices)));

            rf_gfx_vertex3f(cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))) * sinf(rf_deg2rad * ((j + 1) * 360 / slices)),
                           sinf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))),
                            cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))) * cosf(rf_deg2rad * ((j + 1) * 360 / slices)));
            rf_gfx_vertex3f(cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))) * sinf(rf_deg2rad * (j * 360 / slices)),
                           sinf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))),
                            cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))) * cosf(rf_deg2rad * (j * 360 / slices)));

            rf_gfx_vertex3f(cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))) * sinf(rf_deg2rad * (j * 360 / slices)),
                           sinf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))),
                            cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * (i + 1))) * cosf(rf_deg2rad * (j * 360 / slices)));

            rf_gfx_vertex3f(cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * i)) * sinf(rf_deg2rad * (j * 360 / slices)),
                           sinf(rf_deg2rad * (270 + (180 / (rings + 1)) * i)),
                            cosf(rf_deg2rad * (270 + (180 / (rings + 1)) * i)) * cosf(rf_deg2rad * (j * 360 / slices)));
        }
    }

    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw a cylinder
// NOTE: It could be also used for pyramid and cone
rf_public void rf_draw_cylinder(rf_vec3 position, float radius_top, float radius_bottom, float height, int sides, rf_color color)
{
    if (sides < 3) sides = 3;

    int num_vertex = sides*6;
    if (rf_gfx_check_buffer_limit(num_vertex)) rf_gfx_draw();

    rf_gfx_push_matrix();
    rf_gfx_translatef(position.x, position.y, position.z);

    rf_gfx_begin(RF_TRIANGLES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    if (radius_top > 0)
    {
        // Draw Body -------------------------------------------------------------------------------------
        for (rf_int i = 0; i < 360; i += 360 / sides)
        {
            rf_gfx_vertex3f(sinf(rf_deg2rad * i) * radius_bottom, 0, cosf(rf_deg2rad * i) * radius_bottom);                                   // Bottom Left
            rf_gfx_vertex3f(sinf(rf_deg2rad * (i + 360 / sides)) * radius_bottom, 0, cosf(rf_deg2rad * (i + 360 / sides)) * radius_bottom); // Bottom Right
            rf_gfx_vertex3f(sinf(rf_deg2rad * (i + 360 / sides)) * radius_top, height, cosf(rf_deg2rad * (i + 360 / sides)) * radius_top);    // Top Right

            rf_gfx_vertex3f(sinf(rf_deg2rad * i) * radius_top, height, cosf(rf_deg2rad * i) * radius_top);                                 // Top Left
            rf_gfx_vertex3f(sinf(rf_deg2rad * i) * radius_bottom, 0, cosf(rf_deg2rad * i) * radius_bottom);                              // Bottom Left
            rf_gfx_vertex3f(sinf(rf_deg2rad * (i + 360 / sides)) * radius_top, height, cosf(rf_deg2rad * (i + 360 / sides)) * radius_top); // Top Right
        }

        // Draw Cap --------------------------------------------------------------------------------------
        for (rf_int i = 0; i < 360; i += 360/sides)
        {
            rf_gfx_vertex3f(0, height, 0);
            rf_gfx_vertex3f(sinf(rf_deg2rad * i) * radius_top, height, cosf(rf_deg2rad * i) * radius_top);
            rf_gfx_vertex3f(sinf(rf_deg2rad * (i + 360 / sides)) * radius_top, height, cosf(rf_deg2rad * (i + 360 / sides)) * radius_top);
        }
    }
    else
    {
        // Draw Cone -------------------------------------------------------------------------------------
        for (rf_int i = 0; i < 360; i += 360/sides)
        {
            rf_gfx_vertex3f(0, height, 0);
            rf_gfx_vertex3f(sinf(rf_deg2rad * i) * radius_bottom, 0, cosf(rf_deg2rad * i) * radius_bottom);
            rf_gfx_vertex3f(sinf(rf_deg2rad * (i + 360 / sides)) * radius_bottom, 0, cosf(rf_deg2rad * (i + 360 / sides)) * radius_bottom);
        }
    }

    // Draw Base -----------------------------------------------------------------------------------------
    for (rf_int i = 0; i < 360; i += 360/sides)
    {
        rf_gfx_vertex3f(0, 0, 0);
        rf_gfx_vertex3f(sinf(rf_deg2rad * (i + 360 / sides)) * radius_bottom, 0, cosf(rf_deg2rad * (i + 360 / sides)) * radius_bottom);
        rf_gfx_vertex3f(sinf(rf_deg2rad * i) * radius_bottom, 0, cosf(rf_deg2rad * i) * radius_bottom);
    }

    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw a wired cylinder
// NOTE: It could be also used for pyramid and cone
rf_public void rf_draw_cylinder_wires(rf_vec3 position, float radius_top, float radius_bottom, float height, int sides, rf_color color)
{
    if (sides < 3) sides = 3;

    int num_vertex = sides*8;
    if (rf_gfx_check_buffer_limit(num_vertex)) rf_gfx_draw();

    rf_gfx_push_matrix();
    rf_gfx_translatef(position.x, position.y, position.z);

    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    for (rf_int i = 0; i < 360; i += 360/sides)
    {
        rf_gfx_vertex3f(sinf(rf_deg2rad * i) * radius_bottom, 0, cosf(rf_deg2rad * i) * radius_bottom);
        rf_gfx_vertex3f(sinf(rf_deg2rad * (i + 360 / sides)) * radius_bottom, 0, cosf(rf_deg2rad * (i + 360 / sides)) * radius_bottom);

        rf_gfx_vertex3f(sinf(rf_deg2rad * (i + 360 / sides)) * radius_bottom, 0, cosf(rf_deg2rad * (i + 360 / sides)) * radius_bottom);
        rf_gfx_vertex3f(sinf(rf_deg2rad * (i + 360 / sides)) * radius_top, height, cosf(rf_deg2rad * (i + 360 / sides)) * radius_top);

        rf_gfx_vertex3f(sinf(rf_deg2rad * (i + 360 / sides)) * radius_top, height, cosf(rf_deg2rad * (i + 360 / sides)) * radius_top);
        rf_gfx_vertex3f(sinf(rf_deg2rad * i) * radius_top, height, cosf(rf_deg2rad * i) * radius_top);

        rf_gfx_vertex3f(sinf(rf_deg2rad * i) * radius_top, height, cosf(rf_deg2rad * i) * radius_top);
        rf_gfx_vertex3f(sinf(rf_deg2rad * i) * radius_bottom, 0, cosf(rf_deg2rad * i) * radius_bottom);
    }
    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw a plane
rf_public void rf_draw_plane(rf_vec3 center_pos, rf_vec2 size, rf_color color)
{
    if (rf_gfx_check_buffer_limit(4)) rf_gfx_draw();

    // NOTE: Plane is always created on XZ ground
    rf_gfx_push_matrix();
    rf_gfx_translatef(center_pos.x, center_pos.y, center_pos.z);
    rf_gfx_scalef(size.x, 1.0f, size.y);

    rf_gfx_begin(RF_QUADS);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_normal3f(0.0f, 1.0f, 0.0f);

    rf_gfx_vertex3f(-0.5f, 0.0f, -0.5f);
    rf_gfx_vertex3f(-0.5f, 0.0f, 0.5f);
    rf_gfx_vertex3f(0.5f, 0.0f, 0.5f);
    rf_gfx_vertex3f(0.5f, 0.0f, -0.5f);
    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw a ray line
rf_public void rf_draw_ray(rf_ray ray, rf_color color)
{
    float scale = 10000;

    rf_gfx_begin(RF_LINES);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);
    rf_gfx_color4ub(color.r, color.g, color.b, color.a);

    rf_gfx_vertex3f(ray.position.x, ray.position.y, ray.position.z);
    rf_gfx_vertex3f(ray.position.x + ray.direction.x*scale, ray.position.y + ray.direction.y*scale, ray.position.z + ray.direction.z*scale);
    rf_gfx_end();
}

// Draw a grid centered at (0, 0, 0)
rf_public void rf_draw_grid(int slices, float spacing)
{
    int half_slices = slices/2;

    if (rf_gfx_check_buffer_limit(slices * 4)) rf_gfx_draw();

    rf_gfx_begin(RF_LINES);
    for (rf_int i = -half_slices; i <= half_slices; i++)
    {
        if (i == 0)
        {
            rf_gfx_color3f(0.5f, 0.5f, 0.5f);
            rf_gfx_color3f(0.5f, 0.5f, 0.5f);
            rf_gfx_color3f(0.5f, 0.5f, 0.5f);
            rf_gfx_color3f(0.5f, 0.5f, 0.5f);
        }
        else
        {
            rf_gfx_color3f(0.75f, 0.75f, 0.75f);
            rf_gfx_color3f(0.75f, 0.75f, 0.75f);
            rf_gfx_color3f(0.75f, 0.75f, 0.75f);
            rf_gfx_color3f(0.75f, 0.75f, 0.75f);
        }

        rf_gfx_vertex3f((float)i*spacing, 0.0f, (float)-half_slices*spacing);
        rf_gfx_vertex3f((float)i*spacing, 0.0f, (float)half_slices*spacing);

        rf_gfx_vertex3f((float)-half_slices*spacing, 0.0f, (float)i*spacing);
        rf_gfx_vertex3f((float)half_slices*spacing, 0.0f, (float)i*spacing);
    }
    rf_gfx_end();
}

// Draw gizmo
rf_public void rf_draw_gizmo(rf_vec3 position)
{
    // NOTE: RGB = XYZ
    float length = 1.0f;

    rf_gfx_push_matrix();
    rf_gfx_translatef(position.x, position.y, position.z);
    rf_gfx_scalef(length, length, length);

    rf_gfx_begin(RF_LINES);
    rf_gfx_color3f(1.0f, 0.0f, 0.0f); rf_gfx_vertex3f(0.0f, 0.0f, 0.0f);
    rf_gfx_color3f(1.0f, 0.0f, 0.0f); rf_gfx_vertex3f(1.0f, 0.0f, 0.0f);

    rf_gfx_color3f(0.0f, 1.0f, 0.0f); rf_gfx_vertex3f(0.0f, 0.0f, 0.0f);
    rf_gfx_color3f(0.0f, 1.0f, 0.0f); rf_gfx_vertex3f(0.0f, 1.0f, 0.0f);

    rf_gfx_color3f(0.0f, 0.0f, 1.0f); rf_gfx_vertex3f(0.0f, 0.0f, 0.0f);
    rf_gfx_color3f(0.0f, 0.0f, 1.0f); rf_gfx_vertex3f(0.0f, 0.0f, 1.0f);
    rf_gfx_end();
    rf_gfx_pop_matrix();
}

// Draw a model (with texture if set)
rf_public void rf_draw_model(rf_model model, rf_vec3 position, float scale, rf_color tint)
{
    rf_vec3 vScale = { scale, scale, scale };
    rf_vec3 rotationAxis = { 0.0f, 1.0f, 0.0f };

    rf_draw_model_ex(model, position, rotationAxis, 0.0f, vScale, tint);
}

// Draw a model with extended parameters
rf_public void rf_draw_model_ex(rf_model model, rf_vec3 position, rf_vec3 rotation_axis, float rotationAngle, rf_vec3 scale, rf_color tint)
{
    // Calculate transformation matrix from function parameters
    // Get transform matrix (rotation -> scale -> translation)
    rf_mat mat_scale = rf_mat_scale(scale.x, scale.y, scale.z);
    rf_mat mat_rotation = rf_mat_rotate(rotation_axis, rotationAngle * rf_deg2rad);
    rf_mat mat_translation = rf_mat_translate(position.x, position.y, position.z);

    rf_mat mat_transform = rf_mat_mul(rf_mat_mul(mat_scale, mat_rotation), mat_translation);

    // Combine model transformation matrix (model.transform) with matrix generated by function parameters (mat_transform)
    model.transform = rf_mat_mul(model.transform, mat_transform);

    for (rf_int i = 0; i < model.mesh_count; i++)
    {
        // TODO: Review color + tint premultiplication mechanism
        rf_color color = model.materials[model.mesh_material[i]].maps[RF_MAP_DIFFUSE].color;

        rf_color color_tint = rf_white;
        color_tint.r = (((float)color.r/255.0)*((float)tint.r/255.0))*255;
        color_tint.g = (((float)color.g/255.0)*((float)tint.g/255.0))*255;
        color_tint.b = (((float)color.b/255.0)*((float)tint.b/255.0))*255;
        color_tint.a = (((float)color.a/255.0)*((float)tint.a/255.0))*255;

        model.materials[model.mesh_material[i]].maps[RF_MAP_DIFFUSE].color = color_tint;
        rf_gfx_draw_mesh(model.meshes[i], model.materials[model.mesh_material[i]], model.transform);
        model.materials[model.mesh_material[i]].maps[RF_MAP_DIFFUSE].color = color;
    }
}

// Draw a model wires (with texture if set) with extended parameters
rf_public void rf_draw_model_wires(rf_model model, rf_vec3 position, rf_vec3 rotation_axis, float rotationAngle, rf_vec3 scale, rf_color tint)
{
    rf_gfx_enable_wire_mode();

    rf_draw_model_ex(model, position, rotation_axis, rotationAngle, scale, tint);

    rf_gfx_disable_wire_mode();
}

// Draw a bounding box with wires
rf_public void rf_draw_bounding_box(rf_bounding_box box, rf_color color)
{
    rf_vec3 size;

    size.x = (float)fabs(box.max.x - box.min.x);
    size.y = (float)fabs(box.max.y - box.min.y);
    size.z = (float)fabs(box.max.z - box.min.z);

    rf_vec3 center = {box.min.x + size.x / 2.0f, box.min.y + size.y / 2.0f, box.min.z + size.z / 2.0f };

    rf_draw_cube_wires(center, size.x, size.y, size.z, color);
}

// Draw a billboard
rf_public void rf_draw_billboard(rf_camera3d camera, rf_texture2d texture, rf_vec3 center, float size, rf_color tint)
{
    rf_rec source_rec = {0.0f, 0.0f, (float)texture.width, (float)texture.height };

    rf_draw_billboard_rec(camera, texture, source_rec, center, size, tint);
}

// Draw a billboard (part of a texture defined by a rectangle)
rf_public void rf_draw_billboard_rec(rf_camera3d camera, rf_texture2d texture, rf_rec source_rec, rf_vec3 center, float size, rf_color tint)
{
    // NOTE: Billboard size will maintain source_rec aspect ratio, size will represent billboard width
    rf_vec2 size_ratio = {size, size * (float)source_rec.height / source_rec.width };

    rf_mat mat_view = rf_mat_look_at(camera.position, camera.target, camera.up);

    rf_vec3 right = {mat_view.m0, mat_view.m4, mat_view.m8 };
    //rf_vec3 up = { mat_view.m1, mat_view.m5, mat_view.m9 };

    // NOTE: Billboard locked on axis-Y
    rf_vec3 up = {0.0f, 1.0f, 0.0f };
    /*
        a-------b
        |       |
        |   *   |
        |       |
        d-------c
    */
    right = rf_vec3_scale(right, size_ratio.x / 2);
    up = rf_vec3_scale(up, size_ratio.y / 2);

    rf_vec3 p1 = rf_vec3_add(right, up);
    rf_vec3 p2 = rf_vec3_sub(right, up);

    rf_vec3 a = rf_vec3_sub(center, p2);
    rf_vec3 b = rf_vec3_add(center, p1);
    rf_vec3 c = rf_vec3_add(center, p2);
    rf_vec3 d = rf_vec3_sub(center, p1);

    if (rf_gfx_check_buffer_limit(4)) rf_gfx_draw();

    rf_gfx_enable_texture(texture.id);

    rf_gfx_begin(RF_QUADS);
    rf_gfx_color4ub(tint.r, tint.g, tint.b, tint.a);

    // Bottom-left corner for texture and quad
    rf_gfx_tex_coord2f((float)source_rec.x/texture.width, (float)source_rec.y/texture.height);
    rf_gfx_vertex3f(a.x, a.y, a.z);

    // Top-left corner for texture and quad
    rf_gfx_tex_coord2f((float)source_rec.x/texture.width, (float)(source_rec.y + source_rec.height)/texture.height);
    rf_gfx_vertex3f(d.x, d.y, d.z);

    // Top-right corner for texture and quad
    rf_gfx_tex_coord2f((float)(source_rec.x + source_rec.width)/texture.width, (float)(source_rec.y + source_rec.height)/texture.height);
    rf_gfx_vertex3f(c.x, c.y, c.z);

    // Bottom-right corner for texture and quad
    rf_gfx_tex_coord2f((float)(source_rec.x + source_rec.width)/texture.width, (float)source_rec.y/texture.height);
    rf_gfx_vertex3f(b.x, b.y, b.z);
    rf_gfx_end();

    rf_gfx_disable_texture();
}



rf_public rf_render_batch rf_create_custom_render_batch_from_buffers(rf_vertex_buffer* vertex_buffers, rf_int vertex_buffers_count, rf_draw_call* draw_calls, rf_int draw_calls_count)
{
    if (!vertex_buffers || !draw_calls || vertex_buffers_count < 0 || draw_calls_count < 0) {
        return (rf_render_batch) {0};
    }

    rf_render_batch batch = {0};
    batch.vertex_buffers = vertex_buffers;
    batch.vertex_buffers_count = vertex_buffers_count;
    batch.draw_calls = draw_calls;
    batch.draw_calls_size = draw_calls_count;

    for (rf_int i = 0; i < vertex_buffers_count; i++)
    {
        memset(vertex_buffers[i].vertices,  0, RF_GFX_VERTEX_COMPONENT_COUNT   * vertex_buffers[i].elements_count);
        memset(vertex_buffers[i].texcoords, 0, RF_GFX_TEXCOORD_COMPONENT_COUNT * vertex_buffers[i].elements_count);
        memset(vertex_buffers[i].colors,    0, RF_GFX_COLOR_COMPONENT_COUNT    * vertex_buffers[i].elements_count);

        int k = 0;

        // Indices can be initialized right now
        for (rf_int j = 0; j < (RF_GFX_VERTEX_INDEX_COMPONENT_COUNT * vertex_buffers[i].elements_count); j += 6)
        {
            vertex_buffers[i].indices[j + 0] = 4 * k + 0;
            vertex_buffers[i].indices[j + 1] = 4 * k + 1;
            vertex_buffers[i].indices[j + 2] = 4 * k + 2;
            vertex_buffers[i].indices[j + 3] = 4 * k + 0;
            vertex_buffers[i].indices[j + 4] = 4 * k + 2;
            vertex_buffers[i].indices[j + 5] = 4 * k + 3;

            k++;
        }

        vertex_buffers[i].v_counter  = 0;
        vertex_buffers[i].tc_counter = 0;
        vertex_buffers[i].c_counter  = 0;

        rf_gfx_init_vertex_buffer(&vertex_buffers[i]);
    }

    for (rf_int i = 0; i < RF_DEFAULT_BATCH_DRAW_CALLS_COUNT; i++)
    {
        batch.draw_calls[i] = (rf_draw_call) {
            .mode = RF_QUADS,
            .texture_id = rf_ctx.default_texture_id,
        };
    }

    batch.draw_calls_counter = 1; // Reset draws counter
    batch.current_depth = -1.0f;  // Reset depth value
    batch.valid = true;

    return batch;
}

// TODO: Not working yet
rf_public rf_render_batch rf_create_custom_render_batch(rf_int vertex_buffers_count, rf_int draw_calls_count, rf_int vertex_buffer_elements_count, rf_allocator allocator)
{
    if (vertex_buffers_count < 0 || draw_calls_count < 0 || vertex_buffer_elements_count < 0) {
        return (rf_render_batch) {0};
    }

    rf_render_batch result = {0};

    rf_int vertex_buffer_array_size = sizeof(rf_vertex_buffer) * vertex_buffers_count;
    rf_int vertex_buffers_memory_size = (sizeof(rf_one_element_vertex_buffer) * vertex_buffer_elements_count) * vertex_buffers_count;
    rf_int draw_calls_array_size = sizeof(rf_draw_call) * draw_calls_count;
    rf_int allocation_size = vertex_buffer_array_size + draw_calls_array_size + vertex_buffers_memory_size;

    char* memory = rf_alloc(allocator, allocation_size);

    if (memory)
    {
        rf_vertex_buffer* buffers = (rf_vertex_buffer*) memory;
        rf_draw_call* draw_calls = (rf_draw_call*) (memory + vertex_buffer_array_size);
        char* buffers_memory = memory + vertex_buffer_array_size + draw_calls_array_size;

        rf_assert(((char*)draw_calls - memory) == draw_calls_array_size + vertex_buffers_memory_size);
        rf_assert((buffers_memory - memory) == vertex_buffers_memory_size);
        rf_assert((buffers_memory - memory) == sizeof(rf_one_element_vertex_buffer) * vertex_buffer_elements_count * vertex_buffers_count);

        for (rf_int i = 0; i < vertex_buffers_count; i++)
        {
            rf_int one_vertex_buffer_memory_size = sizeof(rf_one_element_vertex_buffer) * vertex_buffer_elements_count;
            rf_int vertices_size = sizeof(rf_gfx_vertex_data_type) * vertex_buffer_elements_count;
            rf_int texcoords_size = sizeof(rf_gfx_texcoord_data_type) * vertex_buffer_elements_count;
            rf_int colors_size = sizeof(rf_gfx_color_data_type) * vertex_buffer_elements_count;
            rf_int indices_size = sizeof(rf_gfx_vertex_index_data_type) * vertex_buffer_elements_count;

            char* this_buffer_memory = buffers_memory + one_vertex_buffer_memory_size * i;

            buffers[i].elements_count = vertex_buffer_elements_count;
            buffers[i].vertices       = (rf_gfx_vertex_data_type*)       this_buffer_memory;
            buffers[i].texcoords      = (rf_gfx_texcoord_data_type*)     this_buffer_memory + vertices_size;
            buffers[i].colors         = (rf_gfx_color_data_type*)        this_buffer_memory + vertices_size + texcoords_size;
            buffers[i].indices        = (rf_gfx_vertex_index_data_type*) this_buffer_memory + vertices_size + texcoords_size + colors_size;
        }

        result = rf_create_custom_render_batch_from_buffers(buffers, vertex_buffers_count, draw_calls, draw_calls_count);
    }

    return result;
}

rf_public rf_render_batch rf_create_default_render_batch_from_memory(rf_default_render_batch* memory)
{
    if (!memory) {
        return (rf_render_batch) {0};
    }

    for (rf_int i = 0; i < RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT; i++)
    {
        memory->vertex_buffers[i].elements_count = RF_DEFAULT_BATCH_ELEMENTS_COUNT;
        memory->vertex_buffers[i].vertices = memory->vertex_buffers_memory[i].vertices;
        memory->vertex_buffers[i].texcoords = memory->vertex_buffers_memory[i].texcoords;
        memory->vertex_buffers[i].colors = memory->vertex_buffers_memory[i].colors;
        memory->vertex_buffers[i].indices = memory->vertex_buffers_memory[i].indices;
    }

    return rf_create_custom_render_batch_from_buffers(memory->vertex_buffers, RF_DEFAULT_BATCH_VERTEX_BUFFERS_COUNT, memory->draw_calls, RF_DEFAULT_BATCH_DRAW_CALLS_COUNT);
}

rf_public rf_render_batch rf_create_default_render_batch(rf_allocator allocator)
{
    rf_default_render_batch* memory = rf_alloc(allocator, sizeof(rf_default_render_batch));
    return rf_create_default_render_batch_from_memory(memory);
}

rf_public void rf_set_active_render_batch(rf_render_batch* batch)
{
    rf_ctx.current_batch = batch;
}

rf_public void rf_unload_render_batch(rf_render_batch batch, rf_allocator allocator)
{
    rf_free(allocator, batch.vertex_buffers);
}



rf_internal void rf__gfx_backend_internal_init(rf_gfx_backend_data* gfx_data);

rf_public void rf_gfx_init(rf_gfx_context* ctx, int screen_width, int screen_height, rf_gfx_backend_data* gfx_data)
{
    *ctx = (rf_gfx_context) {0};
    rf_set_global_gfx_context_pointer(ctx);

    rf_ctx.current_matrix_mode = -1;
    rf_ctx.screen_scaling = rf_mat_identity();

    rf_ctx.framebuffer_width  = screen_width;
    rf_ctx.framebuffer_height = screen_height;
    rf_ctx.render_width       = screen_width;
    rf_ctx.render_height      = screen_height;
    rf_ctx.current_width      = screen_width;
    rf_ctx.current_height     = screen_height;

    rf__gfx_backend_internal_init(gfx_data);

    // Initialize default shaders and default textures
    {
        // Init default white texture
        unsigned char pixels[4] = { 255, 255, 255, 255 }; // 1 pixel RGBA (4 bytes)
        rf_ctx.default_texture_id = rf_gfx_load_texture(pixels, 1, 1, rf_pixel_format_r8g8b8a8, 1);

        if (rf_ctx.default_texture_id != 0)
        {
            rf_log(rf_log_type_info, "Base white texture loaded successfully. [ Texture ID: %d ]", rf_ctx.default_texture_id);
        }
        else
        {
            rf_log(rf_log_type_warning, "Base white texture could not be loaded");
        }

        // Init default shader (customized for GL 3.3 and ES2)
        rf_ctx.default_shader = rf_load_default_shader();
        rf_ctx.current_shader = rf_ctx.default_shader;

        // Init transformations matrix accumulator
        rf_ctx.transform = rf_mat_identity();

        // Init internal matrix stack (emulating OpenGL 1)
        for (rf_int i = 0; i < RF_MAX_MATRIX_STACK_SIZE; i++)
        {
            rf_ctx.stack[i] = rf_mat_identity();
        }

        // Init internal matrices
        rf_ctx.projection     = rf_mat_identity();
        rf_ctx.modelview      = rf_mat_identity();
        rf_ctx.current_matrix = &rf_ctx.modelview;
    }

    // Setup default viewport
    rf_set_viewport(screen_width, screen_height);

    // Load default font
    #if !defined(RAYFORK_NO_DEFAULT_FONT)
    {
        // NOTE: Using UTF8 encoding table for Unicode U+0000..U+00FF Basic Latin + Latin-1 Supplement
        // http://www.utf8-chartable.de/unicode-utf8-table.pl

        rf_ctx.default_font.glyphs_count = 224; // Number of chars included in our default font

        // Default font is directly defined here (data generated from a sprite font image)
        // This way, we reconstruct rf_font without creating large global variables
        // This data is automatically allocated to Stack and automatically deallocated at the end of this function
        static unsigned int default_font_data[512] = {
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200020, 0x0001b000, 0x00000000, 0x00000000, 0x8ef92520, 0x00020a00, 0x7dbe8000, 0x1f7df45f,
            0x4a2bf2a0, 0x0852091e, 0x41224000, 0x10041450, 0x2e292020, 0x08220812, 0x41222000, 0x10041450, 0x10f92020, 0x3efa084c, 0x7d22103c, 0x107df7de,
            0xe8a12020, 0x08220832, 0x05220800, 0x10450410, 0xa4a3f000, 0x08520832, 0x05220400, 0x10450410, 0xe2f92020, 0x0002085e, 0x7d3e0281, 0x107df41f,
            0x00200000, 0x8001b000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc0000fbe, 0xfbf7e00f, 0x5fbf7e7d, 0x0050bee8, 0x440808a2, 0x0a142fe8, 0x50810285, 0x0050a048,
            0x49e428a2, 0x0a142828, 0x40810284, 0x0048a048, 0x10020fbe, 0x09f7ebaf, 0xd89f3e84, 0x0047a04f, 0x09e48822, 0x0a142aa1, 0x50810284, 0x0048a048,
            0x04082822, 0x0a142fa0, 0x50810285, 0x0050a248, 0x00008fbe, 0xfbf42021, 0x5f817e7d, 0x07d09ce8, 0x00008000, 0x00000fe0, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000c0180,
            0xdfbf4282, 0x0bfbf7ef, 0x42850505, 0x004804bf, 0x50a142c6, 0x08401428, 0x42852505, 0x00a808a0, 0x50a146aa, 0x08401428, 0x42852505, 0x00081090,
            0x5fa14a92, 0x0843f7e8, 0x7e792505, 0x00082088, 0x40a15282, 0x08420128, 0x40852489, 0x00084084, 0x40a16282, 0x0842022a, 0x40852451, 0x00088082,
            0xc0bf4282, 0xf843f42f, 0x7e85fc21, 0x3e0900bf, 0x00000000, 0x00000004, 0x00000000, 0x000c0180, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x04000402, 0x41482000, 0x00000000, 0x00000800,
            0x04000404, 0x4100203c, 0x00000000, 0x00000800, 0xf7df7df0, 0x514bef85, 0xbefbefbe, 0x04513bef, 0x14414500, 0x494a2885, 0xa28a28aa, 0x04510820,
            0xf44145f0, 0x474a289d, 0xa28a28aa, 0x04510be0, 0x14414510, 0x494a2884, 0xa28a28aa, 0x02910a00, 0xf7df7df0, 0xd14a2f85, 0xbefbe8aa, 0x011f7be0,
            0x00000000, 0x00400804, 0x20080000, 0x00000000, 0x00000000, 0x00600f84, 0x20080000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0xac000000, 0x00000f01, 0x00000000, 0x00000000, 0x24000000, 0x00000f01, 0x00000000, 0x06000000, 0x24000000, 0x00000f01, 0x00000000, 0x09108000,
            0x24fa28a2, 0x00000f01, 0x00000000, 0x013e0000, 0x2242252a, 0x00000f52, 0x00000000, 0x038a8000, 0x2422222a, 0x00000f29, 0x00000000, 0x010a8000,
            0x2412252a, 0x00000f01, 0x00000000, 0x010a8000, 0x24fbe8be, 0x00000f01, 0x00000000, 0x0ebe8000, 0xac020000, 0x00000f01, 0x00000000, 0x00048000,
            0x0003e000, 0x00000f00, 0x00000000, 0x00008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000038, 0x8443b80e, 0x00203a03,
            0x02bea080, 0xf0000020, 0xc452208a, 0x04202b02, 0xf8029122, 0x07f0003b, 0xe44b388e, 0x02203a02, 0x081e8a1c, 0x0411e92a, 0xf4420be0, 0x01248202,
            0xe8140414, 0x05d104ba, 0xe7c3b880, 0x00893a0a, 0x283c0e1c, 0x04500902, 0xc4400080, 0x00448002, 0xe8208422, 0x04500002, 0x80400000, 0x05200002,
            0x083e8e00, 0x04100002, 0x804003e0, 0x07000042, 0xf8008400, 0x07f00003, 0x80400000, 0x04000022, 0x00000000, 0x00000000, 0x80400000, 0x04000002,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00800702, 0x1848a0c2, 0x84010000, 0x02920921, 0x01042642, 0x00005121, 0x42023f7f, 0x00291002,
            0xefc01422, 0x7efdfbf7, 0xefdfa109, 0x03bbbbf7, 0x28440f12, 0x42850a14, 0x20408109, 0x01111010, 0x28440408, 0x42850a14, 0x2040817f, 0x01111010,
            0xefc78204, 0x7efdfbf7, 0xe7cf8109, 0x011111f3, 0x2850a932, 0x42850a14, 0x2040a109, 0x01111010, 0x2850b840, 0x42850a14, 0xefdfbf79, 0x03bbbbf7,
            0x001fa020, 0x00000000, 0x00001000, 0x00000000, 0x00002070, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x08022800, 0x00012283, 0x02430802, 0x01010001, 0x8404147c, 0x20000144, 0x80048404, 0x00823f08, 0xdfbf4284, 0x7e03f7ef, 0x142850a1, 0x0000210a,
            0x50a14684, 0x528a1428, 0x142850a1, 0x03efa17a, 0x50a14a9e, 0x52521428, 0x142850a1, 0x02081f4a, 0x50a15284, 0x4a221428, 0xf42850a1, 0x03efa14b,
            0x50a16284, 0x4a521428, 0x042850a1, 0x0228a17a, 0xdfbf427c, 0x7e8bf7ef, 0xf7efdfbf, 0x03efbd0b, 0x00000000, 0x04000000, 0x00000000, 0x00000008,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00200508, 0x00840400, 0x11458122, 0x00014210,
            0x00514294, 0x51420800, 0x20a22a94, 0x0050a508, 0x00200000, 0x00000000, 0x00050000, 0x08000000, 0xfefbefbe, 0xfbefbefb, 0xfbeb9114, 0x00fbefbe,
            0x20820820, 0x8a28a20a, 0x8a289114, 0x3e8a28a2, 0xfefbefbe, 0xfbefbe0b, 0x8a289114, 0x008a28a2, 0x228a28a2, 0x08208208, 0x8a289114, 0x088a28a2,
            0xfefbefbe, 0xfbefbefb, 0xfa2f9114, 0x00fbefbe, 0x00000000, 0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x00000020, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00210100, 0x00000004, 0x00000000, 0x00000000, 0x14508200, 0x00001402, 0x00000000, 0x00000000,
            0x00000010, 0x00000020, 0x00000000, 0x00000000, 0xa28a28be, 0x00002228, 0x00000000, 0x00000000, 0xa28a28aa, 0x000022e8, 0x00000000, 0x00000000,
            0xa28a28aa, 0x000022a8, 0x00000000, 0x00000000, 0xa28a28aa, 0x000022e8, 0x00000000, 0x00000000, 0xbefbefbe, 0x00003e2f, 0x00000000, 0x00000000,
            0x00000004, 0x00002028, 0x00000000, 0x00000000, 0x80000000, 0x00003e0f, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
        };

        int chars_height  = 10;
        int chars_divisor = 1; // Every char is separated from the consecutive by a 1 pixel divisor, horizontally and vertically

        int chars_width[224] = {
            3, 1, 4, 6, 5, 7, 6, 2, 3, 3, 5, 5, 2, 4, 1, 7, 5, 2, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 3, 4, 3, 6,
            7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 5, 6, 5, 7, 6, 6, 6, 6, 6, 6, 7, 6, 7, 7, 6, 6, 6, 2, 7, 2, 3, 5,
            2, 5, 5, 5, 5, 5, 4, 5, 5, 1, 2, 5, 2, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5, 3, 1, 3, 4, 4,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 5, 5, 5, 7, 1, 5, 3, 7, 3, 5, 4, 1, 7, 4, 3, 5, 3, 3, 2, 5, 6, 1, 2, 2, 3, 5, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 7, 6, 6, 6, 6, 6, 3, 3, 3, 3, 7, 6, 6, 6, 6, 6, 6, 5, 6, 6, 6, 6, 6, 6, 4, 6,
            5, 5, 5, 5, 5, 5, 9, 5, 5, 5, 5, 5, 2, 2, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5
        };

        // Re-construct image from rf_ctx->default_font_data and generate a texture
        //----------------------------------------------------------------------
        {
            rf_color font_pixels[128 * 128] = {0};

            int counter = 0; // rf_font data elements counter

            // Fill with default_font_data (convert from bit to pixel!)
            for (rf_int i = 0; i < 128 * 128; i += 32)
            {
                for (rf_int j = 31; j >= 0; j--)
                {
                    const int bit_check = (default_font_data[counter]) & (1u << j);
                    if (bit_check) font_pixels[i + j] = rf_white;
                }

                counter++;

                if (counter > 512) counter = 0; // Security check...
            }

            rf_bool format_success = rf_format_pixels(font_pixels, 128 * 128 * sizeof(rf_color), rf_pixel_format_r8g8b8a8,
                                                      rf_ctx.default_font_buffers.pixels, 128 * 128 * 2, rf_pixel_format_gray_alpha);

            rf_assert(format_success);
        }

        rf_image font_image = {
            .data = rf_ctx.default_font_buffers.pixels,
            .format = rf_pixel_format_gray_alpha,
            .width = 128,
            .height = 128,
            .valid = true,
        };

        rf_ctx.default_font.texture = rf_load_texture_from_image(font_image);

        // Allocate space for our characters info data
        rf_ctx.default_font.glyphs = rf_ctx.default_font_buffers.chars;

        int current_line     = 0;
        int current_pos_x    = chars_divisor;
        int test_pos_x       = chars_divisor;
        int char_pixels_iter = 0;

        for (rf_int i = 0; i < rf_ctx.default_font.glyphs_count; i++)
        {
            rf_ctx.default_font.glyphs[i].codepoint = 32 + i; // First char is 32

            rf_ctx.default_font.glyphs[i].rec.x      = (float) current_pos_x;
            rf_ctx.default_font.glyphs[i].rec.y      = (float) (chars_divisor + current_line * (chars_height + chars_divisor));
            rf_ctx.default_font.glyphs[i].rec.width  = (float) chars_width[i];
            rf_ctx.default_font.glyphs[i].rec.height = (float) chars_height;

            test_pos_x += (int) (rf_ctx.default_font.glyphs[i].rec.width + (float)chars_divisor);

            if (test_pos_x >= rf_ctx.default_font.texture.width)
            {
                current_line++;
                current_pos_x = 2 * chars_divisor + chars_width[i];
                test_pos_x = current_pos_x;

                rf_ctx.default_font.glyphs[i].rec.x = (float) (chars_divisor);
                rf_ctx.default_font.glyphs[i].rec.y = (float) (chars_divisor + current_line * (chars_height + chars_divisor));
            }
            else current_pos_x = test_pos_x;

            // NOTE: On default font character offsets and xAdvance are not required
            rf_ctx.default_font.glyphs[i].offset_x = 0;
            rf_ctx.default_font.glyphs[i].offset_y = 0;
            rf_ctx.default_font.glyphs[i].advance_x = 0;
        }

        rf_ctx.default_font.base_size = (int)rf_ctx.default_font.glyphs[0].rec.height;
        rf_ctx.default_font.valid = true;

        rf_log(rf_log_type_info, "[TEX ID %i] Default font loaded successfully", rf_ctx.default_font.texture.id);
    }
    #endif
}

#pragma region getters

// Get the default font, useful to be used with extended parameters
rf_public rf_font rf_get_default_font()
{
    return rf_ctx.default_font;
}

// Get default shader
rf_public rf_shader rf_get_default_shader()
{
    return rf_ctx.default_shader;
}

// Get default internal texture (white texture)
rf_public rf_texture2d rf_get_default_texture()
{
    rf_texture2d texture = {0};
    texture.id = rf_ctx.default_texture_id;
    texture.width = 1;
    texture.height = 1;
    texture.mipmaps = 1;
    texture.format = rf_pixel_format_r8g8b8a8;

    return texture;
}

//Get the context pointer
rf_public rf_gfx_context* rf_get_gfx_context()
{
    return &rf_ctx;
}

// Get pixel data from GPU frontbuffer and return an rf_image (screenshot)
rf_public rf_image rf_get_screen_data(rf_color* dst, rf_int dst_size)
{
    rf_image image = {0};

    if (dst && dst_size == rf_ctx.render_width * rf_ctx.render_height)
    {
        rf_gfx_read_screen_pixels(dst, rf_ctx.render_width, rf_ctx.render_height);

        image.data   = dst;
        image.width  = rf_ctx.render_width;
        image.height = rf_ctx.render_height;
        image.format = rf_pixel_format_r8g8b8a8;
        image.valid  = true;
    }

    return image;
}

rf_public rf_log_type rf_get_current_log_filter()
{
    return rf_ctx.logger_filter;
}

#pragma endregion

#pragma region setters

// Define default texture used to draw shapes
rf_public void rf_set_shapes_texture(rf_texture2d texture, rf_rec source)
{
    rf_ctx.tex_shapes = texture;
    rf_ctx.rec_tex_shapes = source;
}

// Set the global context pointer
rf_public void rf_set_global_gfx_context_pointer(rf_gfx_context* ctx)
{
    rf__global_gfx_context_ptr = ctx;
}

// Set viewport for a provided width and height
rf_public void rf_set_viewport(int width, int height)
{
    rf_ctx.render_width = width;
    rf_ctx.render_height = height;

    // Set viewport width and height
    rf_gfx_viewport(0, 0, rf_ctx.render_width, rf_ctx.render_height);

    rf_gfx_matrix_mode(RF_PROJECTION); // Switch to PROJECTION matrix
    rf_gfx_load_identity(); // Reset current matrix (PROJECTION)

    // Set orthographic GL_PROJECTION to current framebuffer size, top-left corner is (0, 0)
    rf_gfx_ortho(0, rf_ctx.render_width, rf_ctx.render_height, 0, 0.0f, 1.0f);

    rf_gfx_matrix_mode(RF_MODELVIEW); // Switch back to MODELVIEW matrix
    rf_gfx_load_identity(); // Reset current matrix (MODELVIEW)
}

rf_public inline rf_int rf_libc_rand_wrapper(rf_int min, rf_int max)
{
    return rand() % (max + 1 - min) + min;
}

#pragma endregion

#endif

#if defined(RAYFORK_NO_AUDIO)

#define STB_VORBIS_IMPLEMENTATION
#define JAR_XM_IMPLEMENTATION
#define JAR_MOD_IMPLEMENTATION
#define DR_WAV_IMPLEMENTATION
#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_WIN32_IO
#define DR_MP3_IMPLEMENTATION
#include "miniaudio.h"
#include "jar_mod.h"
#include "jar_xm.h"

RF_API rf_valid rf_audio_init()
{
    rf_valid        valid          = {0};
    ma_context      context        = {0};
    ma_device_info* playback_infos = {0};
    ma_uint32       playback_count = {0};
    ma_device_info* capture_infos  = {0};
    ma_uint32       capture_count  = {0};

    if (ma_context_init(NULL, 0, NULL, &context) == MA_SUCCESS)
    {
        if (ma_context_get_devices(&context, &playback_infos, &playback_count, &capture_infos, &capture_count) == MA_SUCCESS)
        {
            /* Loop over each device info and do something with it. Here we just print the name with their index. You may want
               to give the user the opportunity to choose which device they'd prefer. */
            for (rf_int device_iter = 0; device_iter < playback_count; device_iter++)
            {
                printf("%d - %s\n", device_iter, playback_infos[device_iter].name);
            }

            ma_device_config config = ma_device_config_init(ma_device_type_playback);
            config.playback.pDeviceID = &playback_infos[chosenPlaybackDeviceIndex].id;
            config.playback.format    = FORMAT;
            config.playback.channels  = MY_CHANNEL_COUNT;
            config.sampleRate         = MY_SAMPLE_RATE;
            config.dataCallback       = data_callback;
            config.pUserData          = pMyCustomData;

            ma_device device;
            if (ma_device_init(&context, &config, &device) != MA_SUCCESS)
            {
                // Error
            }
        }
    }
}

RF_API rf_audio_device rf_audio_device_count();
RF_API rf_audio_device rf_default_audio_device();
#endif
