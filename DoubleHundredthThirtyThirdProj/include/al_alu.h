#ifndef _ALU_H_
#define _ALU_H_

#include <limits.h>
#include <math.h>
#ifdef HAVE_FLOAT_H
#include <float.h>
#endif
#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

#include "al_alMain.h"
#include "al_alBuffer.h"
#include "al_alFilter.h"
#include "al_alAuxEffectSlot.h"

#include "al_hrtf.h"
#include "al_align.h"
#include "al_nfcfilter.h"
#include "al_math_defs.h"


#define MAX_PITCH  (255)

/* Maximum number of buffer samples before the current pos needed for resampling. */
#define MAX_PRE_SAMPLES 12

/* Maximum number of buffer samples after the current pos needed for resampling. */
#define MAX_POST_SAMPLES 12


#ifdef __cplusplus
extern "C" {
#endif

struct ALsource;
struct ALbufferlistitem;
struct ALvoice;
struct ALeffectslot;


#define DITHER_RNG_SEED 22222


enum SpatializeMode {
    SpatializeOff = AL_FALSE,
    SpatializeOn = AL_TRUE,
    SpatializeAuto = AL_AUTO_SOFT
};

enum Resampler {
    PointResampler,
    LinearResampler,
    FIR4Resampler,
    BSincResampler,

    ResamplerMax = BSincResampler
};
extern enum Resampler ResamplerDefault;

/* The number of distinct scale and phase intervals within the filter table. */
#define BSINC_SCALE_BITS  4
#define BSINC_SCALE_COUNT (1<<BSINC_SCALE_BITS)
#define BSINC_PHASE_BITS  4
#define BSINC_PHASE_COUNT (1<<BSINC_PHASE_BITS)

/* Interpolator state.  Kind of a misnomer since the interpolator itself is
 * stateless.  This just keeps it from having to recompute scale-related
 * mappings for every sample.
 */
typedef struct BsincState {
    ALfloat sf; /* Scale interpolation factor. */
    ALuint m;   /* Coefficient count. */
    ALint l;    /* Left coefficient offset. */
    struct {
        const ALfloat *filter;   /* Filter coefficients. */
        const ALfloat *scDelta;  /* Scale deltas. */
        const ALfloat *phDelta;  /* Phase deltas. */
        const ALfloat *spDelta;  /* Scale-phase deltas. */
    } coeffs[BSINC_PHASE_COUNT];
} BsincState;

typedef union InterpState {
    BsincState bsinc;
} InterpState;

ALboolean BsincPrepare(const ALuint increment, BsincState *state);

typedef const ALfloat* (*ResamplerFunc)(const InterpState *state,
    const ALfloat *restrict src, ALsizei frac, ALint increment,
    ALfloat *restrict dst, ALsizei dstlen
);


typedef union aluVector {
    alignas(16) ALfloat v[4];
} aluVector;

inline void aluVectorSet(aluVector *vector, ALfloat x, ALfloat y, ALfloat z, ALfloat w)
{
    vector->v[0] = x;
    vector->v[1] = y;
    vector->v[2] = z;
    vector->v[3] = w;
}


typedef union aluMatrixf {
    alignas(16) ALfloat m[4][4];
} aluMatrixf;
extern const aluMatrixf IdentityMatrixf;

inline void aluMatrixfSetRow(aluMatrixf *matrix, ALuint row,
                             ALfloat m0, ALfloat m1, ALfloat m2, ALfloat m3)
{
    matrix->m[row][0] = m0;
    matrix->m[row][1] = m1;
    matrix->m[row][2] = m2;
    matrix->m[row][3] = m3;
}

inline void aluMatrixfSet(aluMatrixf *matrix, ALfloat m00, ALfloat m01, ALfloat m02, ALfloat m03,
                                              ALfloat m10, ALfloat m11, ALfloat m12, ALfloat m13,
                                              ALfloat m20, ALfloat m21, ALfloat m22, ALfloat m23,
                                              ALfloat m30, ALfloat m31, ALfloat m32, ALfloat m33)
{
    aluMatrixfSetRow(matrix, 0, m00, m01, m02, m03);
    aluMatrixfSetRow(matrix, 1, m10, m11, m12, m13);
    aluMatrixfSetRow(matrix, 2, m20, m21, m22, m23);
    aluMatrixfSetRow(matrix, 3, m30, m31, m32, m33);
}


enum ActiveFilters {
    AF_None = 0,
    AF_LowPass = 1,
    AF_HighPass = 2,
    AF_BandPass = AF_LowPass | AF_HighPass
};


typedef struct MixHrtfParams {
    const ALfloat (*Coeffs)[2];
    ALsizei Delay[2];
    ALfloat Gain;
    ALfloat GainStep;
} MixHrtfParams;


typedef struct DirectParams {
    ALfilterState LowPass;
    ALfilterState HighPass;

    NfcFilter NFCtrlFilter[MAX_AMBI_ORDER];

    struct {
        HrtfParams Old;
        HrtfParams Target;
        HrtfState State;
    } Hrtf;

    struct {
        ALfloat Current[MAX_OUTPUT_CHANNELS];
        ALfloat Target[MAX_OUTPUT_CHANNELS];
    } Gains;
} DirectParams;

typedef struct SendParams {
    ALfilterState LowPass;
    ALfilterState HighPass;

    struct {
        ALfloat Current[MAX_OUTPUT_CHANNELS];
        ALfloat Target[MAX_OUTPUT_CHANNELS];
    } Gains;
} SendParams;


struct ALvoiceProps {
    ATOMIC(struct ALvoiceProps*) next;

    ALfloat Pitch;
    ALfloat Gain;
    ALfloat OuterGain;
    ALfloat MinGain;
    ALfloat MaxGain;
    ALfloat InnerAngle;
    ALfloat OuterAngle;
    ALfloat RefDistance;
    ALfloat MaxDistance;
    ALfloat RolloffFactor;
    ALfloat Position[3];
    ALfloat Velocity[3];
    ALfloat Direction[3];
    ALfloat Orientation[2][3];
    ALboolean HeadRelative;
    enum DistanceModel DistanceModel;
    enum Resampler Resampler;
    ALboolean DirectChannels;
    enum SpatializeMode SpatializeMode;

    ALboolean DryGainHFAuto;
    ALboolean WetGainAuto;
    ALboolean WetGainHFAuto;
    ALfloat   OuterGainHF;

    ALfloat AirAbsorptionFactor;
    ALfloat RoomRolloffFactor;
    ALfloat DopplerFactor;

    ALfloat StereoPan[2];

    ALfloat Radius;

    /** Direct filter and auxiliary send info. */
    struct {
        ALfloat Gain;
        ALfloat GainHF;
        ALfloat HFReference;
        ALfloat GainLF;
        ALfloat LFReference;
    } Direct;
    struct {
        struct ALeffectslot *Slot;
        ALfloat Gain;
        ALfloat GainHF;
        ALfloat HFReference;
        ALfloat GainLF;
        ALfloat LFReference;
    } Send[];
};

/* If not 'fading', gain targets are used directly without fading. */
#define VOICE_IS_FADING (1<<0)
#define VOICE_HAS_HRTF  (1<<1)
#define VOICE_HAS_NFC   (1<<2)

typedef struct ALvoice {
    struct ALvoiceProps *Props;

    ATOMIC(struct ALvoiceProps*) Update;
    ATOMIC(struct ALvoiceProps*) FreeList;

    ATOMIC(struct ALsource*) Source;
    ATOMIC(bool) Playing;

    /**
     * Source offset in samples, relative to the currently playing buffer, NOT
     * the whole queue, and the fractional (fixed-point) offset to the next
     * sample.
     */
    ATOMIC(ALuint) position;
    ATOMIC(ALsizei) position_fraction;

    /* Current buffer queue item being played. */
    ATOMIC(struct ALbufferlistitem*) current_buffer;

    /* Buffer queue item to loop to at end of queue (will be NULL for non-
     * looping voices).
     */
    ATOMIC(struct ALbufferlistitem*) loop_buffer;

    /**
     * Number of channels and bytes-per-sample for the attached source's
     * buffer(s).
     */
    ALsizei NumChannels;
    ALsizei SampleSize;

    /** Current target parameters used for mixing. */
    ALint Step;

    ResamplerFunc Resampler;

    ALuint Flags;

    ALuint Offset; /* Number of output samples mixed since starting. */

    alignas(16) ALfloat PrevSamples[MAX_INPUT_CHANNELS][MAX_PRE_SAMPLES];

    InterpState ResampleState;

    struct {
        enum ActiveFilters FilterType;
        DirectParams Params[MAX_INPUT_CHANNELS];

        ALfloat (*Buffer)[BUFFERSIZE];
        ALsizei Channels;
        ALsizei ChannelsPerOrder[MAX_AMBI_ORDER+1];
    } Direct;

    struct {
        enum ActiveFilters FilterType;
        SendParams Params[MAX_INPUT_CHANNELS];

        ALfloat (*Buffer)[BUFFERSIZE];
        ALsizei Channels;
    } Send[];
} ALvoice;

void DeinitVoice(ALvoice *voice);


typedef void (*MixerFunc)(const ALfloat *data, ALsizei OutChans,
                          ALfloat (*restrict OutBuffer)[BUFFERSIZE], ALfloat *CurrentGains,
                          const ALfloat *TargetGains, ALsizei Counter, ALsizei OutPos,
                          ALsizei BufferSize);
typedef void (*RowMixerFunc)(ALfloat *OutBuffer, const ALfloat *gains,
                             const ALfloat (*restrict data)[BUFFERSIZE], ALsizei InChans,
                             ALsizei InPos, ALsizei BufferSize);
typedef void (*HrtfMixerFunc)(ALfloat *restrict LeftOut, ALfloat *restrict RightOut,
                              const ALfloat *data, ALsizei Offset, ALsizei OutPos,
                              const ALsizei IrSize, MixHrtfParams *hrtfparams,
                              HrtfState *hrtfstate, ALsizei BufferSize);
typedef void (*HrtfMixerBlendFunc)(ALfloat *restrict LeftOut, ALfloat *restrict RightOut,
                                   const ALfloat *data, ALsizei Offset, ALsizei OutPos,
                                   const ALsizei IrSize, const HrtfParams *oldparams,
                                   MixHrtfParams *newparams, HrtfState *hrtfstate,
                                   ALsizei BufferSize);
typedef void (*HrtfDirectMixerFunc)(ALfloat *restrict LeftOut, ALfloat *restrict RightOut,
                                    const ALfloat *data, ALsizei Offset, const ALsizei IrSize,
                                    const ALfloat (*restrict Coeffs)[2],
                                    ALfloat (*restrict Values)[2], ALsizei BufferSize);


#define GAIN_MIX_MAX  (16.0f) /* +24dB */

#define GAIN_SILENCE_THRESHOLD  (0.00001f) /* -100dB */

#define SPEEDOFSOUNDMETRESPERSEC  (343.3f)
#define AIRABSORBGAINHF           (0.99426f) /* -0.05dB */

/* Target gain for the reverb decay feedback reaching the decay time. */
#define REVERB_DECAY_GAIN  (0.001f) /* -60 dB */

#define FRACTIONBITS (12)
#define FRACTIONONE  (1<<FRACTIONBITS)
#define FRACTIONMASK (FRACTIONONE-1)


inline ALfloat minf(ALfloat a, ALfloat b)
{ return ((a > b) ? b : a); }
inline ALfloat maxf(ALfloat a, ALfloat b)
{ return ((a > b) ? a : b); }
inline ALfloat clampf(ALfloat val, ALfloat min, ALfloat max)
{ return minf(max, maxf(min, val)); }

inline ALdouble mind(ALdouble a, ALdouble b)
{ return ((a > b) ? b : a); }
inline ALdouble maxd(ALdouble a, ALdouble b)
{ return ((a > b) ? a : b); }
inline ALdouble clampd(ALdouble val, ALdouble min, ALdouble max)
{ return mind(max, maxd(min, val)); }

inline ALuint minu(ALuint a, ALuint b)
{ return ((a > b) ? b : a); }
inline ALuint maxu(ALuint a, ALuint b)
{ return ((a > b) ? a : b); }
inline ALuint clampu(ALuint val, ALuint min, ALuint max)
{ return minu(max, maxu(min, val)); }

inline ALint mini(ALint a, ALint b)
{ return ((a > b) ? b : a); }
inline ALint maxi(ALint a, ALint b)
{ return ((a > b) ? a : b); }
inline ALint clampi(ALint val, ALint min, ALint max)
{ return mini(max, maxi(min, val)); }

inline ALint64 mini64(ALint64 a, ALint64 b)
{ return ((a > b) ? b : a); }
inline ALint64 maxi64(ALint64 a, ALint64 b)
{ return ((a > b) ? a : b); }
inline ALint64 clampi64(ALint64 val, ALint64 min, ALint64 max)
{ return mini64(max, maxi64(min, val)); }

inline ALuint64 minu64(ALuint64 a, ALuint64 b)
{ return ((a > b) ? b : a); }
inline ALuint64 maxu64(ALuint64 a, ALuint64 b)
{ return ((a > b) ? a : b); }
inline ALuint64 clampu64(ALuint64 val, ALuint64 min, ALuint64 max)
{ return minu64(max, maxu64(min, val)); }


extern alignas(16) const ALfloat bsincTab[18840];
extern alignas(16) const ALfloat sinc4Tab[FRACTIONONE][4];


inline ALfloat lerp(ALfloat val1, ALfloat val2, ALfloat mu)
{
    return val1 + (val2-val1)*mu;
}
inline ALfloat resample_fir4(ALfloat val0, ALfloat val1, ALfloat val2, ALfloat val3, ALsizei frac)
{
    return sinc4Tab[frac][0]*val0 + sinc4Tab[frac][1]*val1 +
           sinc4Tab[frac][2]*val2 + sinc4Tab[frac][3]*val3;
}


enum HrtfRequestMode {
    Hrtf_Default = 0,
    Hrtf_Enable = 1,
    Hrtf_Disable = 2,
};

void aluInitMixer(void);

MixerFunc SelectMixer(void);
RowMixerFunc SelectRowMixer(void);
ResamplerFunc SelectResampler(enum Resampler resampler);

/* aluInitRenderer
 *
 * Set up the appropriate panning method and mixing method given the device
 * properties.
 */
void aluInitRenderer(ALCdevice *device, ALint hrtf_id, enum HrtfRequestMode hrtf_appreq, enum HrtfRequestMode hrtf_userreq);

void aluInitEffectPanning(struct ALeffectslot *slot);

/**
 * CalcDirectionCoeffs
 *
 * Calculates ambisonic coefficients based on a direction vector. The vector
 * must be normalized (unit length), and the spread is the angular width of the
 * sound (0...tau).
 */
void CalcDirectionCoeffs(const ALfloat dir[3], ALfloat spread, ALfloat coeffs[MAX_AMBI_COEFFS]);

/**
 * CalcAngleCoeffs
 *
 * Calculates ambisonic coefficients based on azimuth and elevation. The
 * azimuth and elevation parameters are in radians, going right and up
 * respectively.
 */
inline void CalcAngleCoeffs(ALfloat azimuth, ALfloat elevation, ALfloat spread, ALfloat coeffs[MAX_AMBI_COEFFS])
{
    ALfloat dir[3] = {
        sinf(azimuth) * cosf(elevation),
        sinf(elevation),
        -cosf(azimuth) * cosf(elevation)
    };
    CalcDirectionCoeffs(dir, spread, coeffs);
}

/**
 * CalcAnglePairwiseCoeffs
 *
 * Calculates ambisonic coefficients based on azimuth and elevation. The
 * azimuth and elevation parameters are in radians, going right and up
 * respectively. This pairwise variant warps the result such that +30 azimuth
 * is full right, and -30 azimuth is full left.
 */
void CalcAnglePairwiseCoeffs(ALfloat azimuth, ALfloat elevation, ALfloat spread, ALfloat coeffs[MAX_AMBI_COEFFS]);

/**
 * ComputeAmbientGains
 *
 * Computes channel gains for ambient, omni-directional sounds.
 */
#define ComputeAmbientGains(b, g, o) do {                                     \
    if((b).CoeffCount > 0)                                                    \
        ComputeAmbientGainsMC((b).Ambi.Coeffs, (b).NumChannels, g, o);        \
    else                                                                      \
        ComputeAmbientGainsBF((b).Ambi.Map, (b).NumChannels, g, o);           \
} while (0)
void ComputeAmbientGainsMC(const ChannelConfig *chancoeffs, ALsizei numchans, ALfloat ingain, ALfloat gains[MAX_OUTPUT_CHANNELS]);
void ComputeAmbientGainsBF(const BFChannelConfig *chanmap, ALsizei numchans, ALfloat ingain, ALfloat gains[MAX_OUTPUT_CHANNELS]);

/**
 * ComputePanningGains
 *
 * Computes panning gains using the given channel decoder coefficients and the
 * pre-calculated direction or angle coefficients.
 */
#define ComputePanningGains(b, c, g, o) do {                                  \
    if((b).CoeffCount > 0)                                                    \
        ComputePanningGainsMC((b).Ambi.Coeffs, (b).NumChannels, (b).CoeffCount, c, g, o);\
    else                                                                      \
        ComputePanningGainsBF((b).Ambi.Map, (b).NumChannels, c, g, o);        \
} while (0)
void ComputePanningGainsMC(const ChannelConfig *chancoeffs, ALsizei numchans, ALsizei numcoeffs, const ALfloat coeffs[MAX_AMBI_COEFFS], ALfloat ingain, ALfloat gains[MAX_OUTPUT_CHANNELS]);
void ComputePanningGainsBF(const BFChannelConfig *chanmap, ALsizei numchans, const ALfloat coeffs[MAX_AMBI_COEFFS], ALfloat ingain, ALfloat gains[MAX_OUTPUT_CHANNELS]);

/**
 * ComputeFirstOrderGains
 *
 * Sets channel gains for a first-order ambisonics input channel. The matrix is
 * a 1x4 'slice' of a transform matrix for the input channel, used to scale and
 * orient the sound samples.
 */
#define ComputeFirstOrderGains(b, m, g, o) do {                               \
    if((b).CoeffCount > 0)                                                    \
        ComputeFirstOrderGainsMC((b).Ambi.Coeffs, (b).NumChannels, m, g, o);  \
    else                                                                      \
        ComputeFirstOrderGainsBF((b).Ambi.Map, (b).NumChannels, m, g, o);     \
} while (0)
void ComputeFirstOrderGainsMC(const ChannelConfig *chancoeffs, ALsizei numchans, const ALfloat mtx[4], ALfloat ingain, ALfloat gains[MAX_OUTPUT_CHANNELS]);
void ComputeFirstOrderGainsBF(const BFChannelConfig *chanmap, ALsizei numchans, const ALfloat mtx[4], ALfloat ingain, ALfloat gains[MAX_OUTPUT_CHANNELS]);


ALboolean MixSource(struct ALvoice *voice, struct ALsource *Source, ALCdevice *Device, ALsizei SamplesToDo);

void aluMixData(ALCdevice *device, ALvoid *OutBuffer, ALsizei NumSamples);
/* Caller must lock the device. */
void aluHandleDisconnect(ALCdevice *device);

extern ALfloat ConeScale;
extern ALfloat ZScale;

#ifdef __cplusplus
}
#endif

#endif
