#ifndef AL_MAIN_H
#define AL_MAIN_H

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <assert.h>
#include <math.h>
#include <limits.h>

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_FENV_H
#include <fenv.h>
#endif

#include "al_al.h"
#include "al_alc.h"
#include "al_alext.h"

#include "al_static_assert.h"
#include "al_align.h"
#include "al_atomic.h"
#include "al_uintmap.h"
#include "al_vector.h"
#include "al_alstring.h"
#include "al_almalloc.h"
#include "al_threads.h"

#ifndef ALC_SOFT_loopback2
#define ALC_SOFT_loopback2 1
#define ALC_AMBISONIC_LAYOUT_SOFT                0x1997
#define ALC_AMBISONIC_SCALING_SOFT               0x1998
#define ALC_AMBISONIC_ORDER_SOFT                 0x1999

#define ALC_BFORMAT3D_SOFT                       0x1508

/* Ambisonic layouts */
#define ALC_ACN_SOFT                             0x1600
#define ALC_FUMA_SOFT                            0x1601

/* Ambisonic scalings (normalization) */
/*#define ALC_FUMA_SOFT*/
#define ALC_SN3D_SOFT                            0x1602
#define ALC_N3D_SOFT                             0x1603

typedef ALCboolean (ALC_APIENTRY*LPALCISAMBISONICFORMATSUPPORTEDSOFT)(ALCdevice *device, ALCenum layout, ALCenum scaling, ALsizei order);
#ifdef AL_ALEXT_PROTOTYPES
ALC_API ALCboolean ALC_APIENTRY alcIsAmbisonicFormatSupportedSOFT(ALCdevice *device, ALCenum layout, ALCenum scaling, ALsizei order);
#endif
#endif

#ifndef ALC_SOFT_device_clock
#define ALC_SOFT_device_clock 1
typedef int64_t ALCint64SOFT;
typedef uint64_t ALCuint64SOFT;
#define ALC_DEVICE_CLOCK_SOFT                    0x1600
#define ALC_DEVICE_LATENCY_SOFT                  0x1601
#define ALC_DEVICE_CLOCK_LATENCY_SOFT            0x1602
typedef void (ALC_APIENTRY*LPALCGETINTEGER64VSOFT)(ALCdevice *device, ALCenum pname, ALsizei size, ALCint64SOFT *values);
#ifdef AL_ALEXT_PROTOTYPES
ALC_API void ALC_APIENTRY alcGetInteger64vSOFT(ALCdevice *device, ALCenum pname, ALsizei size, ALCint64SOFT *values);
#endif
#endif

#ifndef AL_SOFT_buffer_samples2
#define AL_SOFT_buffer_samples2 1
/* Channel configurations */
#define AL_MONO_SOFT                             0x1500
#define AL_STEREO_SOFT                           0x1501
#define AL_REAR_SOFT                             0x1502
#define AL_QUAD_SOFT                             0x1503
#define AL_5POINT1_SOFT                          0x1504
#define AL_6POINT1_SOFT                          0x1505
#define AL_7POINT1_SOFT                          0x1506
#define AL_BFORMAT2D_SOFT                        0x1507
#define AL_BFORMAT3D_SOFT                        0x1508

/* Sample types */
#define AL_BYTE_SOFT                             0x1400
#define AL_UNSIGNED_BYTE_SOFT                    0x1401
#define AL_SHORT_SOFT                            0x1402
#define AL_UNSIGNED_SHORT_SOFT                   0x1403
#define AL_INT_SOFT                              0x1404
#define AL_UNSIGNED_INT_SOFT                     0x1405
#define AL_FLOAT_SOFT                            0x1406
#define AL_DOUBLE_SOFT                           0x1407
#define AL_BYTE3_SOFT                            0x1408
#define AL_UNSIGNED_BYTE3_SOFT                   0x1409
#define AL_MULAW_SOFT                            0x140A

/* Storage formats */
#define AL_MONO8_SOFT                            0x1100
#define AL_MONO16_SOFT                           0x1101
#define AL_MONO32F_SOFT                          0x10010
#define AL_STEREO8_SOFT                          0x1102
#define AL_STEREO16_SOFT                         0x1103
#define AL_STEREO32F_SOFT                        0x10011
#define AL_QUAD8_SOFT                            0x1204
#define AL_QUAD16_SOFT                           0x1205
#define AL_QUAD32F_SOFT                          0x1206
#define AL_REAR8_SOFT                            0x1207
#define AL_REAR16_SOFT                           0x1208
#define AL_REAR32F_SOFT                          0x1209
#define AL_5POINT1_8_SOFT                        0x120A
#define AL_5POINT1_16_SOFT                       0x120B
#define AL_5POINT1_32F_SOFT                      0x120C
#define AL_6POINT1_8_SOFT                        0x120D
#define AL_6POINT1_16_SOFT                       0x120E
#define AL_6POINT1_32F_SOFT                      0x120F
#define AL_7POINT1_8_SOFT                        0x1210
#define AL_7POINT1_16_SOFT                       0x1211
#define AL_7POINT1_32F_SOFT                      0x1212
#define AL_BFORMAT2D_8_SOFT                      0x20021
#define AL_BFORMAT2D_16_SOFT                     0x20022
#define AL_BFORMAT2D_32F_SOFT                    0x20023
#define AL_BFORMAT3D_8_SOFT                      0x20031
#define AL_BFORMAT3D_16_SOFT                     0x20032
#define AL_BFORMAT3D_32F_SOFT                    0x20033

/* Buffer attributes */
#define AL_INTERNAL_FORMAT_SOFT                  0x2008
#define AL_BYTE_LENGTH_SOFT                      0x2009
#define AL_SAMPLE_LENGTH_SOFT                    0x200A
#define AL_SEC_LENGTH_SOFT                       0x200B

#if 0
typedef void (AL_APIENTRY*LPALBUFFERSAMPLESSOFT)(ALuint,ALuint,ALenum,ALsizei,ALenum,ALenum,const ALvoid*);
typedef void (AL_APIENTRY*LPALGETBUFFERSAMPLESSOFT)(ALuint,ALsizei,ALsizei,ALenum,ALenum,ALvoid*);
typedef ALboolean (AL_APIENTRY*LPALISBUFFERFORMATSUPPORTEDSOFT)(ALenum);
#ifdef AL_ALEXT_PROTOTYPES
AL_API void AL_APIENTRY alBufferSamplesSOFT(ALuint buffer, ALuint samplerate, ALenum internalformat, ALsizei samples, ALenum channels, ALenum type, const ALvoid *data);
AL_API void AL_APIENTRY alGetBufferSamplesSOFT(ALuint buffer, ALsizei offset, ALsizei samples, ALenum channels, ALenum type, ALvoid *data);
AL_API ALboolean AL_APIENTRY alIsBufferFormatSupportedSOFT(ALenum format);
#endif
#endif
#endif


#if defined(_WIN64)
#define SZFMT "%I64u"
#elif defined(_WIN32)
#define SZFMT "%u"
#else
#define SZFMT "%zu"
#endif


#ifdef __GNUC__
/* Because of a long-standing deficiency in C, you're not allowed to implicitly
 * cast a pointer-to-type-array to a pointer-to-const-type-array. For example,
 *
 * int (*ptr)[10];
 * const int (*cptr)[10] = ptr;
 *
 * is not allowed and most compilers will generate noisy warnings about
 * incompatible types, even though it just makes the array elements const.
 * Clang will allow it if you make the array type a typedef, like this:
 *
 * typedef int int10[10];
 * int10 *ptr;
 * const int10 *cptr = ptr;
 *
 * however GCC does not and still issues the incompatible type warning. The
 * "proper" way to fix it is to add an explicit cast for the constified type,
 * but that removes the vast majority of otherwise useful type-checking you'd
 * get, and runs the risk of improper casts if types are later changed. Leaving
 * it non-const can also be an issue if you use it as a function parameter, and
 * happen to have a const type as input (and also reduce the capabilities of
 * the compiler to better optimize the function).
 *
 * So to work around the problem, we use a macro. The macro first assigns the
 * incoming variable to the specified non-const type to ensure it's the correct
 * type, then casts the variable as the desired constified type. Very ugly, but
 * I'd rather not have hundreds of lines of warnings because I want to tell the
 * compiler that some array(s) can't be changed by the code, or have lots of
 * error-prone casts.
 */
#define SAFE_CONST(T, var) __extension__({                                    \
    T _tmp = (var);                                                           \
    (const T)_tmp;                                                            \
})
#else
/* Non-GNU-compatible compilers have to use a straight cast with no extra
 * checks, due to the lack of multi-statement expressions.
 */
#define SAFE_CONST(T, var) ((const T)(var))
#endif


#ifdef __GNUC__
/* This helps cast away the const-ness of a pointer without accidentally
 * changing the pointer type. This is necessary due to Clang's inability to use
 * atomic_load on a const _Atomic variable.
 */
#define CONST_CAST(T, V) __extension__({                                      \
    const T _tmp = (V);                                                       \
    (T)_tmp;                                                                  \
})
#else
#define CONST_CAST(T, V) ((T)(V))
#endif


typedef ALint64SOFT ALint64;
typedef ALuint64SOFT ALuint64;

#ifndef U64
#if defined(_MSC_VER)
#define U64(x) ((ALuint64)(x##ui64))
#elif SIZEOF_LONG == 8
#define U64(x) ((ALuint64)(x##ul))
#elif SIZEOF_LONG_LONG == 8
#define U64(x) ((ALuint64)(x##ull))
#endif
#endif

#ifndef UINT64_MAX
#define UINT64_MAX U64(18446744073709551615)
#endif

#ifndef UNUSED
#if defined(__cplusplus)
#define UNUSED(x)
#elif defined(__GNUC__)
#define UNUSED(x) UNUSED_##x __attribute__((unused))
#elif defined(__LCLINT__)
#define UNUSED(x) /*@unused@*/ x
#else
#define UNUSED(x) x
#endif
#endif

#ifdef __GNUC__
#define DECL_FORMAT(x, y, z) __attribute__((format(x, (y), (z))))
#else
#define DECL_FORMAT(x, y, z)
#endif

/* Calculates the size of a struct with N elements of a flexible array member.
 * GCC and Clang allow offsetof(Type, fam[N]) for this, but MSVC seems to have
 * trouble, so a bit more verbose workaround is needed.
 */
#define FAM_SIZE(T, M, N)  (offsetof(T, M) + sizeof(((T*)NULL)->M[0])*(N))

#if defined(__GNUC__) && defined(__i386__)
/* force_align_arg_pointer is required for proper function arguments aligning
 * when SSE code is used. Some systems (Windows, QNX) do not guarantee our
 * thread functions will be properly aligned on the stack, even though GCC may
 * generate code with the assumption that it is. */
#define FORCE_ALIGN __attribute__((force_align_arg_pointer))
#else
#define FORCE_ALIGN
#endif

#ifdef HAVE_C99_VLA
#define DECL_VLA(T, _name, _size)  T _name[(_size)]
#else
#define DECL_VLA(T, _name, _size)  T *_name = alloca((_size) * sizeof(T))
#endif

#ifndef PATH_MAX
#ifdef MAX_PATH
#define PATH_MAX MAX_PATH
#else
#define PATH_MAX 4096
#endif
#endif


static const union {
    ALuint u;
    ALubyte b[sizeof(ALuint)];
} EndianTest = { 1 };
#define IS_LITTLE_ENDIAN (EndianTest.b[0] == 1)

#define COUNTOF(x) (sizeof(x) / sizeof(0[x]))


#define DERIVE_FROM_TYPE(t)          t t##_parent
#define STATIC_CAST(to, obj)         (&(obj)->to##_parent)
#ifdef __GNUC__
#define STATIC_UPCAST(to, from, obj) __extension__({                          \
    static_assert(__builtin_types_compatible_p(from, __typeof(*(obj))),       \
                  "Invalid upcast object from type");                         \
    (to*)((char*)(obj) - offsetof(to, from##_parent));                        \
})
#else
#define STATIC_UPCAST(to, from, obj) ((to*)((char*)(obj) - offsetof(to, from##_parent)))
#endif

#define DECLARE_FORWARD(T1, T2, rettype, func)                                \
rettype T1##_##func(T1 *obj)                                                  \
{ return T2##_##func(STATIC_CAST(T2, obj)); }

#define DECLARE_FORWARD1(T1, T2, rettype, func, argtype1)                     \
rettype T1##_##func(T1 *obj, argtype1 a)                                      \
{ return T2##_##func(STATIC_CAST(T2, obj), a); }

#define DECLARE_FORWARD2(T1, T2, rettype, func, argtype1, argtype2)           \
rettype T1##_##func(T1 *obj, argtype1 a, argtype2 b)                          \
{ return T2##_##func(STATIC_CAST(T2, obj), a, b); }

#define DECLARE_FORWARD3(T1, T2, rettype, func, argtype1, argtype2, argtype3) \
rettype T1##_##func(T1 *obj, argtype1 a, argtype2 b, argtype3 c)              \
{ return T2##_##func(STATIC_CAST(T2, obj), a, b, c); }


#define GET_VTABLE1(T1)     (&(T1##_vtable))
#define GET_VTABLE2(T1, T2) (&(T1##_##T2##_vtable))

#define SET_VTABLE1(T1, obj)     ((obj)->vtbl = GET_VTABLE1(T1))
#define SET_VTABLE2(T1, T2, obj) (STATIC_CAST(T2, obj)->vtbl = GET_VTABLE2(T1, T2))

#define DECLARE_THUNK(T1, T2, rettype, func)                                  \
static rettype T1##_##T2##_##func(T2 *obj)                                    \
{ return T1##_##func(STATIC_UPCAST(T1, T2, obj)); }

#define DECLARE_THUNK1(T1, T2, rettype, func, argtype1)                       \
static rettype T1##_##T2##_##func(T2 *obj, argtype1 a)                        \
{ return T1##_##func(STATIC_UPCAST(T1, T2, obj), a); }

#define DECLARE_THUNK2(T1, T2, rettype, func, argtype1, argtype2)             \
static rettype T1##_##T2##_##func(T2 *obj, argtype1 a, argtype2 b)            \
{ return T1##_##func(STATIC_UPCAST(T1, T2, obj), a, b); }

#define DECLARE_THUNK3(T1, T2, rettype, func, argtype1, argtype2, argtype3)   \
static rettype T1##_##T2##_##func(T2 *obj, argtype1 a, argtype2 b, argtype3 c) \
{ return T1##_##func(STATIC_UPCAST(T1, T2, obj), a, b, c); }

#define DECLARE_THUNK4(T1, T2, rettype, func, argtype1, argtype2, argtype3, argtype4) \
static rettype T1##_##T2##_##func(T2 *obj, argtype1 a, argtype2 b, argtype3 c, argtype4 d) \
{ return T1##_##func(STATIC_UPCAST(T1, T2, obj), a, b, c, d); }

#define DECLARE_DEFAULT_ALLOCATORS(T)                                         \
static void* T##_New(size_t size) { return al_malloc(16, size); }             \
static void T##_Delete(void *ptr) { al_free(ptr); }

/* Helper to extract an argument list for VCALL. Not used directly. */
#define EXTRACT_VCALL_ARGS(...)  __VA_ARGS__))

/* Call a "virtual" method on an object, with arguments. */
#define V(obj, func)  ((obj)->vtbl->func((obj), EXTRACT_VCALL_ARGS
/* Call a "virtual" method on an object, with no arguments. */
#define V0(obj, func) ((obj)->vtbl->func((obj) EXTRACT_VCALL_ARGS

#define DELETE_OBJ(obj) do {                                                  \
    if((obj) != NULL)                                                         \
    {                                                                         \
        V0((obj),Destruct)();                                                 \
        V0((obj),Delete)();                                                   \
    }                                                                         \
} while(0)


#define EXTRACT_NEW_ARGS(...)  __VA_ARGS__);                                  \
    }                                                                         \
} while(0)

#define NEW_OBJ(_res, T) do {                                                 \
    _res = T##_New(sizeof(T));                                                \
    if(_res)                                                                  \
    {                                                                         \
        memset(_res, 0, sizeof(T));                                           \
        T##_Construct(_res, EXTRACT_NEW_ARGS
#define NEW_OBJ0(_res, T) do {                                                \
    _res = T##_New(sizeof(T));                                                \
    if(_res)                                                                  \
    {                                                                         \
        memset(_res, 0, sizeof(T));                                           \
        T##_Construct(_res EXTRACT_NEW_ARGS


#ifdef __cplusplus
extern "C" {
#endif

struct Hrtf;
struct HrtfEntry;
struct Compressor;


#define DEFAULT_OUTPUT_RATE  (44100)
#define MIN_OUTPUT_RATE      (8000)


/* Find the next power-of-2 for non-power-of-2 numbers. */
inline ALuint NextPowerOf2(ALuint value)
{
    if(value > 0)
    {
        value--;
        value |= value>>1;
        value |= value>>2;
        value |= value>>4;
        value |= value>>8;
        value |= value>>16;
    }
    return value+1;
}

/** Round up a value to the next multiple. */
inline size_t RoundUp(size_t value, size_t r)
{
    value += r-1;
    return value - (value%r);
}

/* Scales the given value using 64-bit integer math, rounding the result. */
inline ALuint64 ScaleRound(ALuint64 val, ALuint64 new_scale, ALuint64 old_scale)
{
    return (val*new_scale + old_scale/2) / old_scale;
}

/* Scales the given value using 64-bit integer math, flooring the result. */
inline ALuint64 ScaleFloor(ALuint64 val, ALuint64 new_scale, ALuint64 old_scale)
{
    return val * new_scale / old_scale;
}

/* Scales the given value using 64-bit integer math, ceiling the result. */
inline ALuint64 ScaleCeil(ALuint64 val, ALuint64 new_scale, ALuint64 old_scale)
{
    return (val*new_scale + old_scale-1) / old_scale;
}

/* Fast float-to-int conversion. Assumes the FPU is already in round-to-zero
 * mode. */
inline ALint fastf2i(ALfloat f)
{
#ifdef HAVE_LRINTF
    return lrintf(f);
#elif defined(_MSC_VER) && defined(_M_IX86)
    ALint i;
    __asm fld f
    __asm fistp i
    return i;
#else
    return (ALint)f;
#endif
}


enum DevProbe {
    ALL_DEVICE_PROBE,
    CAPTURE_DEVICE_PROBE
};

struct ALCbackend;


enum DistanceModel {
    InverseDistanceClamped  = AL_INVERSE_DISTANCE_CLAMPED,
    LinearDistanceClamped   = AL_LINEAR_DISTANCE_CLAMPED,
    ExponentDistanceClamped = AL_EXPONENT_DISTANCE_CLAMPED,
    InverseDistance  = AL_INVERSE_DISTANCE,
    LinearDistance   = AL_LINEAR_DISTANCE,
    ExponentDistance = AL_EXPONENT_DISTANCE,
    DisableDistance  = AL_NONE,

    DefaultDistanceModel = InverseDistanceClamped
};

enum Channel {
    FrontLeft = 0,
    FrontRight,
    FrontCenter,
    LFE,
    BackLeft,
    BackRight,
    BackCenter,
    SideLeft,
    SideRight,

    UpperFrontLeft,
    UpperFrontRight,
    UpperBackLeft,
    UpperBackRight,
    LowerFrontLeft,
    LowerFrontRight,
    LowerBackLeft,
    LowerBackRight,

    Aux0,
    Aux1,
    Aux2,
    Aux3,
    Aux4,
    Aux5,
    Aux6,
    Aux7,
    Aux8,
    Aux9,
    Aux10,
    Aux11,
    Aux12,
    Aux13,
    Aux14,
    Aux15,

    InvalidChannel
};


/* Device formats */
enum DevFmtType {
    DevFmtByte   = ALC_BYTE_SOFT,
    DevFmtUByte  = ALC_UNSIGNED_BYTE_SOFT,
    DevFmtShort  = ALC_SHORT_SOFT,
    DevFmtUShort = ALC_UNSIGNED_SHORT_SOFT,
    DevFmtInt    = ALC_INT_SOFT,
    DevFmtUInt   = ALC_UNSIGNED_INT_SOFT,
    DevFmtFloat  = ALC_FLOAT_SOFT,

    DevFmtTypeDefault = DevFmtFloat
};
enum DevFmtChannels {
    DevFmtMono   = ALC_MONO_SOFT,
    DevFmtStereo = ALC_STEREO_SOFT,
    DevFmtQuad   = ALC_QUAD_SOFT,
    DevFmtX51    = ALC_5POINT1_SOFT,
    DevFmtX61    = ALC_6POINT1_SOFT,
    DevFmtX71    = ALC_7POINT1_SOFT,
    DevFmtAmbi3D = ALC_BFORMAT3D_SOFT,

    /* Similar to 5.1, except using rear channels instead of sides */
    DevFmtX51Rear = 0x80000000,

    DevFmtChannelsDefault = DevFmtStereo
};
#define MAX_OUTPUT_CHANNELS  (16)

ALsizei BytesFromDevFmt(enum DevFmtType type);
ALsizei ChannelsFromDevFmt(enum DevFmtChannels chans, ALsizei ambiorder);
inline ALsizei FrameSizeFromDevFmt(enum DevFmtChannels chans, enum DevFmtType type, ALsizei ambiorder)
{
    return ChannelsFromDevFmt(chans, ambiorder) * BytesFromDevFmt(type);
}

enum AmbiLayout {
    AmbiLayout_FuMa = ALC_FUMA_SOFT, /* FuMa channel order */
    AmbiLayout_ACN = ALC_ACN_SOFT,   /* ACN channel order */

    AmbiLayout_Default = AmbiLayout_ACN
};

enum AmbiNorm {
    AmbiNorm_FuMa = ALC_FUMA_SOFT, /* FuMa normalization */
    AmbiNorm_SN3D = ALC_SN3D_SOFT, /* SN3D normalization */
    AmbiNorm_N3D = ALC_N3D_SOFT,   /* N3D normalization */

    AmbiNorm_Default = AmbiNorm_SN3D
};


extern const struct EffectList {
    const char *name;
    int type;
    const char *ename;
    ALenum val;
} EffectList[];


enum DeviceType {
    Playback,
    Capture,
    Loopback
};


enum RenderMode {
    NormalRender,
    StereoPair,
    HrtfRender
};


/* The maximum number of Ambisonics coefficients. For a given order (o), the
 * size needed will be (o+1)**2, thus zero-order has 1, first-order has 4,
 * second-order has 9, third-order has 16, and fourth-order has 25.
 */
#define MAX_AMBI_ORDER  3
#define MAX_AMBI_COEFFS ((MAX_AMBI_ORDER+1) * (MAX_AMBI_ORDER+1))

/* A bitmask of ambisonic channels with height information. If none of these
 * channels are used/needed, there's no height (e.g. with most surround sound
 * speaker setups). This only specifies up to 4th order, which is the highest
 * order a 32-bit mask value can specify (a 64-bit mask could handle up to 7th
 * order). This is ACN ordering, with bit 0 being ACN 0, etc.
 */
#define AMBI_PERIPHONIC_MASK (0xfe7ce4)

/* The maximum number of Ambisonic coefficients for 2D (non-periphonic)
 * representation. This is 2 per each order above zero-order, plus 1 for zero-
 * order. Or simply, o*2 + 1.
 */
#define MAX_AMBI2D_COEFFS (MAX_AMBI_ORDER*2 + 1)


typedef ALfloat ChannelConfig[MAX_AMBI_COEFFS];
typedef struct BFChannelConfig {
    ALfloat Scale;
    ALsizei Index;
} BFChannelConfig;

typedef union AmbiConfig {
    /* Ambisonic coefficients for mixing to the dry buffer. */
    ChannelConfig Coeffs[MAX_OUTPUT_CHANNELS];
    /* Coefficient channel mapping for mixing to the dry buffer. */
    BFChannelConfig Map[MAX_OUTPUT_CHANNELS];
} AmbiConfig;


#define HRTF_HISTORY_BITS   (6)
#define HRTF_HISTORY_LENGTH (1<<HRTF_HISTORY_BITS)
#define HRTF_HISTORY_MASK   (HRTF_HISTORY_LENGTH-1)

#define HRIR_BITS        (7)
#define HRIR_LENGTH      (1<<HRIR_BITS)
#define HRIR_MASK        (HRIR_LENGTH-1)

typedef struct HrtfState {
    alignas(16) ALfloat History[HRTF_HISTORY_LENGTH];
    alignas(16) ALfloat Values[HRIR_LENGTH][2];
} HrtfState;

typedef struct HrtfParams {
    alignas(16) ALfloat Coeffs[HRIR_LENGTH][2];
    ALsizei Delay[2];
    ALfloat Gain;
} HrtfParams;

typedef struct DirectHrtfState {
    /* HRTF filter state for dry buffer content */
    ALsizei Offset;
    ALsizei IrSize;
    struct {
        alignas(16) ALfloat Values[HRIR_LENGTH][2];
        alignas(16) ALfloat Coeffs[HRIR_LENGTH][2];
    } Chan[];
} DirectHrtfState;

typedef struct EnumeratedHrtf {
    al_string name;

    struct HrtfEntry *hrtf;
} EnumeratedHrtf;
TYPEDEF_VECTOR(EnumeratedHrtf, vector_EnumeratedHrtf)


/* Maximum delay in samples for speaker distance compensation. */
#define MAX_DELAY_LENGTH 1024

typedef struct DistanceComp {
    ALfloat Gain;
    ALsizei Length; /* Valid range is [0...MAX_DELAY_LENGTH). */
    ALfloat *Buffer;
} DistanceComp;

/* Size for temporary storage of buffer data, in ALfloats. Larger values need
 * more memory, while smaller values may need more iterations. The value needs
 * to be a sensible size, however, as it constrains the max stepping value used
 * for mixing, as well as the maximum number of samples per mixing iteration.
 */
#define BUFFERSIZE 2048

struct ALCdevice_struct
{
    RefCount ref;

    ALCboolean Connected;
    enum DeviceType Type;

    ALuint Frequency;
    ALuint UpdateSize;
    ALuint NumUpdates;
    enum DevFmtChannels FmtChans;
    enum DevFmtType     FmtType;
    ALboolean IsHeadphones;
    ALsizei AmbiOrder;
    /* For DevFmtAmbi* output only, specifies the channel order and
     * normalization.
     */
    enum AmbiLayout AmbiLayout;
    enum AmbiNorm   AmbiScale;

    al_string DeviceName;

    ATOMIC(ALCenum) LastError;

    // Maximum number of sources that can be created
    ALuint SourcesMax;
    // Maximum number of slots that can be created
    ALuint AuxiliaryEffectSlotMax;

    ALCuint NumMonoSources;
    ALCuint NumStereoSources;
    ALsizei NumAuxSends;

    // Map of Buffers for this device
    UIntMap BufferMap;

    // Map of Effects for this device
    UIntMap EffectMap;

    // Map of Filters for this device
    UIntMap FilterMap;

    /* HRTF state and info */
    DirectHrtfState *Hrtf;
    al_string HrtfName;
    struct Hrtf *HrtfHandle;
    vector_EnumeratedHrtf HrtfList;
    ALCenum HrtfStatus;

    /* UHJ encoder state */
    struct Uhj2Encoder *Uhj_Encoder;

    /* High quality Ambisonic decoder */
    struct BFormatDec *AmbiDecoder;

    /* Stereo-to-binaural filter */
    struct bs2b *Bs2b;

    /* First-order ambisonic upsampler for higher-order output */
    struct AmbiUpsampler *AmbiUp;

    /* Rendering mode. */
    enum RenderMode Render_Mode;

    // Device flags
    ALuint Flags;

    ALuint64 ClockBase;
    ALuint SamplesDone;

    /* Temp storage used for each source when mixing. */
    alignas(16) ALfloat SourceData[BUFFERSIZE];
    alignas(16) ALfloat ResampledData[BUFFERSIZE];
    alignas(16) ALfloat FilteredData[BUFFERSIZE];
    alignas(16) ALfloat NFCtrlData[BUFFERSIZE];

    /* The "dry" path corresponds to the main output. */
    struct {
        AmbiConfig Ambi;
        /* Number of coefficients in each Ambi.Coeffs to mix together (4 for
         * first-order, 9 for second-order, etc). If the count is 0, Ambi.Map
         * is used instead to map each output to a coefficient index.
         */
        ALsizei CoeffCount;

        ALfloat (*Buffer)[BUFFERSIZE];
        ALsizei NumChannels;
        ALsizei NumChannelsPerOrder[MAX_AMBI_ORDER+1];
    } Dry;

    /* First-order ambisonics output, to be upsampled to the dry buffer if different. */
    struct {
        AmbiConfig Ambi;
        /* Will only be 4 or 0. */
        ALsizei CoeffCount;

        ALfloat (*Buffer)[BUFFERSIZE];
        ALsizei NumChannels;
    } FOAOut;

    /* "Real" output, which will be written to the device buffer. May alias the
     * dry buffer.
     */
    struct {
        enum Channel ChannelName[MAX_OUTPUT_CHANNELS];

        ALfloat (*Buffer)[BUFFERSIZE];
        ALsizei NumChannels;
    } RealOut;

    struct Compressor *Limiter;

    /* The average speaker distance as determined by the ambdec configuration
     * (or alternatively, by the NFC-HOA reference delay). Only used for NFC.
     */
    ALfloat AvgSpeakerDist;

    /* Delay buffers used to compensate for speaker distances. */
    DistanceComp ChannelDelay[MAX_OUTPUT_CHANNELS];

    /* Dithering control. */
    ALfloat DitherDepth;
    ALuint DitherSeed;

    /* Running count of the mixer invocations, in 31.1 fixed point. This
     * actually increments *twice* when mixing, first at the start and then at
     * the end, so the bottom bit indicates if the device is currently mixing
     * and the upper bits indicates how many mixes have been done.
     */
    RefCount MixCount;

    // Contexts created on this device
    ATOMIC(ALCcontext*) ContextList;

    almtx_t BackendLock;
    struct ALCbackend *Backend;

    ALCdevice *volatile next;
};

// Frequency was requested by the app or config file
#define DEVICE_FREQUENCY_REQUEST                 (1u<<1)
// Channel configuration was requested by the config file
#define DEVICE_CHANNELS_REQUEST                  (1u<<2)
// Sample type was requested by the config file
#define DEVICE_SAMPLE_TYPE_REQUEST               (1u<<3)

// Specifies if the DSP is paused at user request
#define DEVICE_PAUSED                            (1u<<30)

// Specifies if the device is currently running
#define DEVICE_RUNNING                           (1u<<31)


/* Nanosecond resolution for the device clock time. */
#define DEVICE_CLOCK_RES  U64(1000000000)


/* Must be less than 15 characters (16 including terminating null) for
 * compatibility with pthread_setname_np limitations. */
#define MIXER_THREAD_NAME "alsoft-mixer"

#define RECORD_THREAD_NAME "alsoft-record"


struct ALCcontext_struct {
    RefCount ref;

    struct ALlistener *Listener;

    UIntMap SourceMap;
    UIntMap EffectSlotMap;

    ATOMIC(ALenum) LastError;

    enum DistanceModel DistanceModel;
    ALboolean SourceDistanceModel;

    ALfloat DopplerFactor;
    ALfloat DopplerVelocity;
    ALfloat SpeedOfSound;
    ATOMIC(ALenum) DeferUpdates;

    RWLock PropLock;

    /* Counter for the pre-mixing updates, in 31.1 fixed point (lowest bit
     * indicates if updates are currently happening).
     */
    RefCount UpdateCount;
    ATOMIC(ALenum) HoldUpdates;

    ALfloat GainBoost;

    struct ALvoice **Voices;
    ALsizei VoiceCount;
    ALsizei MaxVoices;

    ATOMIC(struct ALeffectslotArray*) ActiveAuxSlots;

    /* Default effect slot */
    struct ALeffectslot *DefaultSlot;

    ALCdevice  *Device;
    const ALCchar *ExtensionList;

    ALCcontext *volatile next;

    /* Memory space used by the listener (and possibly default effect slot) */
    alignas(16) ALCbyte _listener_mem[];
};

ALCcontext *GetContextRef(void);

void ALCcontext_IncRef(ALCcontext *context);
void ALCcontext_DecRef(ALCcontext *context);

void AllocateVoices(ALCcontext *context, ALsizei num_voices, ALsizei old_sends);

void AppendAllDevicesList(const ALCchar *name);
void AppendCaptureDeviceList(const ALCchar *name);

void ALCdevice_Lock(ALCdevice *device);
void ALCdevice_Unlock(ALCdevice *device);

void ALCcontext_DeferUpdates(ALCcontext *context);
void ALCcontext_ProcessUpdates(ALCcontext *context);


typedef struct {
#ifdef HAVE_FENV_H
    DERIVE_FROM_TYPE(fenv_t);
#ifdef _WIN32
    int round_mode;
#endif
#else
    int state;
#endif
#ifdef HAVE_SSE
    int sse_state;
#endif
} FPUCtl;
void SetMixerFPUMode(FPUCtl *ctl);
void RestoreFPUMode(const FPUCtl *ctl);
#ifdef __GNUC__
/* Use an alternate macro set with GCC to avoid accidental continue or break
 * statements within the mixer mode.
 */
#define START_MIXER_MODE() __extension__({ FPUCtl _oldMode; SetMixerFPUMode(&_oldMode);
#define END_MIXER_MODE() RestoreFPUMode(&_oldMode); })
#else
#define START_MIXER_MODE() do { FPUCtl _oldMode; SetMixerFPUMode(&_oldMode);
#define END_MIXER_MODE() RestoreFPUMode(&_oldMode); } while(0)
#endif
#define LEAVE_MIXER_MODE() RestoreFPUMode(&_oldMode)


typedef struct ll_ringbuffer ll_ringbuffer_t;
typedef struct ll_ringbuffer_data {
    char *buf;
    size_t len;
} ll_ringbuffer_data_t;
ll_ringbuffer_t *ll_ringbuffer_create(size_t sz, size_t elem_sz);
void ll_ringbuffer_free(ll_ringbuffer_t *rb);
void ll_ringbuffer_get_read_vector(const ll_ringbuffer_t *rb, ll_ringbuffer_data_t *vec);
void ll_ringbuffer_get_write_vector(const ll_ringbuffer_t *rb, ll_ringbuffer_data_t *vec);
size_t ll_ringbuffer_read(ll_ringbuffer_t *rb, char *dest, size_t cnt);
size_t ll_ringbuffer_peek(ll_ringbuffer_t *rb, char *dest, size_t cnt);
void ll_ringbuffer_read_advance(ll_ringbuffer_t *rb, size_t cnt);
size_t ll_ringbuffer_read_space(const ll_ringbuffer_t *rb);
int ll_ringbuffer_mlock(ll_ringbuffer_t *rb);
void ll_ringbuffer_reset(ll_ringbuffer_t *rb);
size_t ll_ringbuffer_write(ll_ringbuffer_t *rb, const char *src, size_t cnt);
void ll_ringbuffer_write_advance(ll_ringbuffer_t *rb, size_t cnt);
size_t ll_ringbuffer_write_space(const ll_ringbuffer_t *rb);

void ReadALConfig(void);
void FreeALConfig(void);
int ConfigValueExists(const char *devName, const char *blockName, const char *keyName);
const char *GetConfigValue(const char *devName, const char *blockName, const char *keyName, const char *def);
int GetConfigValueBool(const char *devName, const char *blockName, const char *keyName, int def);
int ConfigValueStr(const char *devName, const char *blockName, const char *keyName, const char **ret);
int ConfigValueInt(const char *devName, const char *blockName, const char *keyName, int *ret);
int ConfigValueUInt(const char *devName, const char *blockName, const char *keyName, unsigned int *ret);
int ConfigValueFloat(const char *devName, const char *blockName, const char *keyName, float *ret);
int ConfigValueBool(const char *devName, const char *blockName, const char *keyName, int *ret);

void SetRTPriority(void);

void SetDefaultChannelOrder(ALCdevice *device);
void SetDefaultWFXChannelOrder(ALCdevice *device);

const ALCchar *DevFmtTypeString(enum DevFmtType type);
const ALCchar *DevFmtChannelsString(enum DevFmtChannels chans);

/**
 * GetChannelIdxByName
 *
 * Returns the index for the given channel name (e.g. FrontCenter), or -1 if it
 * doesn't exist.
 */
inline ALint GetChannelIndex(const enum Channel names[MAX_OUTPUT_CHANNELS], enum Channel chan)
{
    ALint i;
    for(i = 0;i < MAX_OUTPUT_CHANNELS;i++)
    {
        if(names[i] == chan)
            return i;
    }
    return -1;
}
#define GetChannelIdxByName(x, c) GetChannelIndex((x).ChannelName, (c))

extern FILE *LogFile;

#if defined(__GNUC__) && !defined(_WIN32) && !defined(IN_IDE_PARSER)
#define AL_PRINT(T, MSG, ...) fprintf(LogFile, "AL lib: %s %s: "MSG, T, __FUNCTION__ , ## __VA_ARGS__)
#else
void al_print(const char *type, const char *func, const char *fmt, ...) DECL_FORMAT(printf, 3,4);
#define AL_PRINT(T, ...) al_print((T), __FUNCTION__, __VA_ARGS__)
#endif

#ifdef __ANDROID__
#include <android/log.h>
#define LOG_ANDROID(T, MSG, ...) __android_log_print(T, "openal", "AL lib: %s: "MSG, __FUNCTION__ , ## __VA_ARGS__)
#else
#define LOG_ANDROID(T, MSG, ...) ((void)0)
#endif

enum LogLevel {
    NoLog,
    LogError,
    LogWarning,
    LogTrace,
    LogRef
};
extern enum LogLevel LogLevel;

#define TRACEREF(...) do {                                                    \
    if(LogLevel >= LogRef)                                                    \
        AL_PRINT("(--)", __VA_ARGS__);                                        \
} while(0)

#define TRACE(...) do {                                                       \
    if(LogLevel >= LogTrace)                                                  \
        AL_PRINT("(II)", __VA_ARGS__);                                        \
    LOG_ANDROID(ANDROID_LOG_DEBUG, __VA_ARGS__);                              \
} while(0)

#define WARN(...) do {                                                        \
    if(LogLevel >= LogWarning)                                                \
        AL_PRINT("(WW)", __VA_ARGS__);                                        \
    LOG_ANDROID(ANDROID_LOG_WARN, __VA_ARGS__);                               \
} while(0)

#define ERR(...) do {                                                         \
    if(LogLevel >= LogError)                                                  \
        AL_PRINT("(EE)", __VA_ARGS__);                                        \
    LOG_ANDROID(ANDROID_LOG_ERROR, __VA_ARGS__);                              \
} while(0)


extern ALint RTPrioLevel;


extern ALuint CPUCapFlags;
enum {
    CPU_CAP_SSE    = 1<<0,
    CPU_CAP_SSE2   = 1<<1,
    CPU_CAP_SSE3   = 1<<2,
    CPU_CAP_SSE4_1 = 1<<3,
    CPU_CAP_NEON   = 1<<4,
};

void FillCPUCaps(ALuint capfilter);

vector_al_string SearchDataFiles(const char *match, const char *subdir);

/* Small hack to use a pointer-to-array types as a normal argument type.
 * Shouldn't be used directly.
 */
typedef ALfloat ALfloatBUFFERSIZE[BUFFERSIZE];
typedef ALfloat ALfloat2[2];


/* The compressor requires the following information for proper
 * initialization:
 *
 *   PreGainDb      - Gain applied before detection (in dB).
 *   PostGainDb     - Gain applied after compression (in dB).
 *   SummedLink     - Whether to use summed (true) or maxed (false) linking.
 *   RmsSensing     - Whether to use RMS (true) or Peak (false) sensing.
 *   AttackTimeMin  - Minimum attack time (in seconds).
 *   AttackTimeMax  - Maximum attack time.  Automates when min != max.
 *   ReleaseTimeMin - Minimum release time (in seconds).
 *   ReleaseTimeMax - Maximum release time.  Automates when min != max.
 *   Ratio          - Compression ratio (x:1).  Set to 0 for true limiter.
 *   ThresholdDb    - Triggering threshold (in dB).
 *   KneeDb         - Knee width (below threshold; in dB).
 *   SampleRate     - Sample rate to process.
 */
struct Compressor *CompressorInit(const ALfloat PreGainDb, const ALfloat PostGainDb,
    const ALboolean SummedLink, const ALboolean RmsSensing, const ALfloat AttackTimeMin,
    const ALfloat AttackTimeMax, const ALfloat ReleaseTimeMin, const ALfloat ReleaseTimeMax,
    const ALfloat Ratio, const ALfloat ThresholdDb, const ALfloat KneeDb,
    const ALuint SampleRate);

ALuint GetCompressorSampleRate(const struct Compressor *Comp);

void ApplyCompression(struct Compressor *Comp, const ALsizei NumChans, const ALsizei SamplesToDo,
                      ALfloat (*restrict OutBuffer)[BUFFERSIZE]);

#ifdef __cplusplus
}
#endif

#endif
