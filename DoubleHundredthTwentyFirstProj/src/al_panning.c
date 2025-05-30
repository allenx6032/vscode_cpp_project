/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2010 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#include "al_config.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "al_alMain.h"
#include "al_alAuxEffectSlot.h"
#include "al_alu.h"
#include "al_bool.h"
#include "al_ambdec.h"
#include "al_bformatdec.h"
#include "al_uhjfilter.h"
#include "al_bs2b.h"


extern inline void CalcAngleCoeffs(ALfloat azimuth, ALfloat elevation, ALfloat spread, ALfloat coeffs[MAX_AMBI_COEFFS]);


static const ALsizei FuMa2ACN[MAX_AMBI_COEFFS] = {
    0,  /* W */
    3,  /* X */
    1,  /* Y */
    2,  /* Z */
    6,  /* R */
    7,  /* S */
    5,  /* T */
    8,  /* U */
    4,  /* V */
    12, /* K */
    13, /* L */
    11, /* M */
    14, /* N */
    10, /* O */
    15, /* P */
    9,  /* Q */
};
static const ALsizei ACN2ACN[MAX_AMBI_COEFFS] = {
    0,  1,  2,  3,  4,  5,  6,  7,
    8,  9, 10, 11, 12, 13, 14, 15
};

/* NOTE: These are scale factors as applied to Ambisonics content. Decoder
 * coefficients should be divided by these values to get proper N3D scalings.
 */
static const ALfloat UnitScale[MAX_AMBI_COEFFS] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};
static const ALfloat SN3D2N3DScale[MAX_AMBI_COEFFS] = {
    1.000000000f, /* ACN  0 (W), sqrt(1) */
    1.732050808f, /* ACN  1 (Y), sqrt(3) */
    1.732050808f, /* ACN  2 (Z), sqrt(3) */
    1.732050808f, /* ACN  3 (X), sqrt(3) */
    2.236067978f, /* ACN  4 (V), sqrt(5) */
    2.236067978f, /* ACN  5 (T), sqrt(5) */
    2.236067978f, /* ACN  6 (R), sqrt(5) */
    2.236067978f, /* ACN  7 (S), sqrt(5) */
    2.236067978f, /* ACN  8 (U), sqrt(5) */
    2.645751311f, /* ACN  9 (Q), sqrt(7) */
    2.645751311f, /* ACN 10 (O), sqrt(7) */
    2.645751311f, /* ACN 11 (M), sqrt(7) */
    2.645751311f, /* ACN 12 (K), sqrt(7) */
    2.645751311f, /* ACN 13 (L), sqrt(7) */
    2.645751311f, /* ACN 14 (N), sqrt(7) */
    2.645751311f, /* ACN 15 (P), sqrt(7) */
};
static const ALfloat FuMa2N3DScale[MAX_AMBI_COEFFS] = {
    1.414213562f, /* ACN  0 (W), sqrt(2) */
    1.732050808f, /* ACN  1 (Y), sqrt(3) */
    1.732050808f, /* ACN  2 (Z), sqrt(3) */
    1.732050808f, /* ACN  3 (X), sqrt(3) */
    1.936491673f, /* ACN  4 (V), sqrt(15)/2 */
    1.936491673f, /* ACN  5 (T), sqrt(15)/2 */
    2.236067978f, /* ACN  6 (R), sqrt(5) */
    1.936491673f, /* ACN  7 (S), sqrt(15)/2 */
    1.936491673f, /* ACN  8 (U), sqrt(15)/2 */
    2.091650066f, /* ACN  9 (Q), sqrt(35/8) */
    1.972026594f, /* ACN 10 (O), sqrt(35)/3 */
    2.231093404f, /* ACN 11 (M), sqrt(224/45) */
    2.645751311f, /* ACN 12 (K), sqrt(7) */
    2.231093404f, /* ACN 13 (L), sqrt(224/45) */
    1.972026594f, /* ACN 14 (N), sqrt(35)/3 */
    2.091650066f, /* ACN 15 (P), sqrt(35/8) */
};


void CalcDirectionCoeffs(const ALfloat dir[3], ALfloat spread, ALfloat coeffs[MAX_AMBI_COEFFS])
{
    /* Convert from OpenAL coords to Ambisonics. */
    ALfloat x = -dir[2];
    ALfloat y = -dir[0];
    ALfloat z =  dir[1];

    /* Zeroth-order */
    coeffs[0]  = 1.0f; /* ACN 0 = 1 */
    /* First-order */
    coeffs[1]  = 1.732050808f * y; /* ACN 1 = sqrt(3) * Y */
    coeffs[2]  = 1.732050808f * z; /* ACN 2 = sqrt(3) * Z */
    coeffs[3]  = 1.732050808f * x; /* ACN 3 = sqrt(3) * X */
    /* Second-order */
    coeffs[4]  = 3.872983346f * x * y;             /* ACN 4 = sqrt(15) * X * Y */
    coeffs[5]  = 3.872983346f * y * z;             /* ACN 5 = sqrt(15) * Y * Z */
    coeffs[6]  = 1.118033989f * (3.0f*z*z - 1.0f); /* ACN 6 = sqrt(5)/2 * (3*Z*Z - 1) */
    coeffs[7]  = 3.872983346f * x * z;             /* ACN 7 = sqrt(15) * X * Z */
    coeffs[8]  = 1.936491673f * (x*x - y*y);       /* ACN 8 = sqrt(15)/2 * (X*X - Y*Y) */
    /* Third-order */
    coeffs[9]  =  2.091650066f * y * (3.0f*x*x - y*y);  /* ACN  9 = sqrt(35/8) * Y * (3*X*X - Y*Y) */
    coeffs[10] = 10.246950766f * z * x * y;             /* ACN 10 = sqrt(105) * Z * X * Y */
    coeffs[11] =  1.620185175f * y * (5.0f*z*z - 1.0f); /* ACN 11 = sqrt(21/8) * Y * (5*Z*Z - 1) */
    coeffs[12] =  1.322875656f * z * (5.0f*z*z - 3.0f); /* ACN 12 = sqrt(7)/2 * Z * (5*Z*Z - 3) */
    coeffs[13] =  1.620185175f * x * (5.0f*z*z - 1.0f); /* ACN 13 = sqrt(21/8) * X * (5*Z*Z - 1) */
    coeffs[14] =  5.123475383f * z * (x*x - y*y);       /* ACN 14 = sqrt(105)/2 * Z * (X*X - Y*Y) */
    coeffs[15] =  2.091650066f * x * (x*x - 3.0f*y*y);  /* ACN 15 = sqrt(35/8) * X * (X*X - 3*Y*Y) */

    if(spread > 0.0f)
    {
        /* Implement the spread by using a spherical source that subtends the
         * angle spread. See:
         * http://www.ppsloan.org/publications/StupidSH36.pdf - Appendix A3
         *
         * When adjusted for N3D normalization instead of SN3D, these
         * calculations are:
         *
         * ZH0 = -sqrt(pi) * (-1+ca);
         * ZH1 =  0.5*sqrt(pi) * sa*sa;
         * ZH2 = -0.5*sqrt(pi) * ca*(-1+ca)*(ca+1);
         * ZH3 = -0.125*sqrt(pi) * (-1+ca)*(ca+1)*(5*ca*ca - 1);
         * ZH4 = -0.125*sqrt(pi) * ca*(-1+ca)*(ca+1)*(7*ca*ca - 3);
         * ZH5 = -0.0625*sqrt(pi) * (-1+ca)*(ca+1)*(21*ca*ca*ca*ca - 14*ca*ca + 1);
         *
         * The gain of the source is compensated for size, so that the
         * loundness doesn't depend on the spread. Thus:
         *
         * ZH0 = 1.0f;
         * ZH1 = 0.5f * (ca+1.0f);
         * ZH2 = 0.5f * (ca+1.0f)*ca;
         * ZH3 = 0.125f * (ca+1.0f)*(5.0f*ca*ca - 1.0f);
         * ZH4 = 0.125f * (ca+1.0f)*(7.0f*ca*ca - 3.0f)*ca;
         * ZH5 = 0.0625f * (ca+1.0f)*(21.0f*ca*ca*ca*ca - 14.0f*ca*ca + 1.0f);
         */
        ALfloat ca = cosf(spread * 0.5f);
        /* Increase the source volume by up to +3dB for a full spread. */
        ALfloat scale = sqrtf(1.0f + spread/F_TAU);

        ALfloat ZH0_norm = scale;
        ALfloat ZH1_norm = 0.5f * (ca+1.f) * scale;
        ALfloat ZH2_norm = 0.5f * (ca+1.f)*ca * scale;
        ALfloat ZH3_norm = 0.125f * (ca+1.f)*(5.f*ca*ca-1.f) * scale;

        /* Zeroth-order */
        coeffs[0]  *= ZH0_norm;
        /* First-order */
        coeffs[1]  *= ZH1_norm;
        coeffs[2]  *= ZH1_norm;
        coeffs[3]  *= ZH1_norm;
        /* Second-order */
        coeffs[4]  *= ZH2_norm;
        coeffs[5]  *= ZH2_norm;
        coeffs[6]  *= ZH2_norm;
        coeffs[7]  *= ZH2_norm;
        coeffs[8]  *= ZH2_norm;
        /* Third-order */
        coeffs[9]  *= ZH3_norm;
        coeffs[10] *= ZH3_norm;
        coeffs[11] *= ZH3_norm;
        coeffs[12] *= ZH3_norm;
        coeffs[13] *= ZH3_norm;
        coeffs[14] *= ZH3_norm;
        coeffs[15] *= ZH3_norm;
    }
}

void CalcAnglePairwiseCoeffs(ALfloat azimuth, ALfloat elevation, ALfloat spread, ALfloat coeffs[MAX_AMBI_COEFFS])
{
    ALfloat sign = (azimuth < 0.0f) ? -1.0f : 1.0f;
    if(!(fabsf(azimuth) > F_PI_2))
        azimuth = minf(fabsf(azimuth) * F_PI_2 / (F_PI/6.0f), F_PI_2) * sign;
    CalcAngleCoeffs(azimuth, elevation, spread, coeffs);
}


void ComputeAmbientGainsMC(const ChannelConfig *chancoeffs, ALsizei numchans, ALfloat ingain, ALfloat gains[MAX_OUTPUT_CHANNELS])
{
    ALsizei i;

    for(i = 0;i < numchans;i++)
        gains[i] = chancoeffs[i][0] * 1.414213562f * ingain;
    for(;i < MAX_OUTPUT_CHANNELS;i++)
        gains[i] = 0.0f;
}

void ComputeAmbientGainsBF(const BFChannelConfig *chanmap, ALsizei numchans, ALfloat ingain, ALfloat gains[MAX_OUTPUT_CHANNELS])
{
    ALfloat gain = 0.0f;
    ALsizei i;

    for(i = 0;i < numchans;i++)
    {
        if(chanmap[i].Index == 0)
            gain += chanmap[i].Scale;
    }
    gains[0] = gain * 1.414213562f * ingain;
    for(i = 1;i < MAX_OUTPUT_CHANNELS;i++)
        gains[i] = 0.0f;
}

void ComputePanningGainsMC(const ChannelConfig *chancoeffs, ALsizei numchans, ALsizei numcoeffs, const ALfloat coeffs[MAX_AMBI_COEFFS], ALfloat ingain, ALfloat gains[MAX_OUTPUT_CHANNELS])
{
    ALsizei i, j;

    for(i = 0;i < numchans;i++)
    {
        float gain = 0.0f;
        for(j = 0;j < numcoeffs;j++)
            gain += chancoeffs[i][j]*coeffs[j];
        gains[i] = clampf(gain, 0.0f, 1.0f) * ingain;
    }
    for(;i < MAX_OUTPUT_CHANNELS;i++)
        gains[i] = 0.0f;
}

void ComputePanningGainsBF(const BFChannelConfig *chanmap, ALsizei numchans, const ALfloat coeffs[MAX_AMBI_COEFFS], ALfloat ingain, ALfloat gains[MAX_OUTPUT_CHANNELS])
{
    ALsizei i;

    for(i = 0;i < numchans;i++)
        gains[i] = chanmap[i].Scale * coeffs[chanmap[i].Index] * ingain;
    for(;i < MAX_OUTPUT_CHANNELS;i++)
        gains[i] = 0.0f;
}

void ComputeFirstOrderGainsMC(const ChannelConfig *chancoeffs, ALsizei numchans, const ALfloat mtx[4], ALfloat ingain, ALfloat gains[MAX_OUTPUT_CHANNELS])
{
    ALsizei i, j;

    for(i = 0;i < numchans;i++)
    {
        float gain = 0.0f;
        for(j = 0;j < 4;j++)
            gain += chancoeffs[i][j] * mtx[j];
        gains[i] = clampf(gain, 0.0f, 1.0f) * ingain;
    }
    for(;i < MAX_OUTPUT_CHANNELS;i++)
        gains[i] = 0.0f;
}

void ComputeFirstOrderGainsBF(const BFChannelConfig *chanmap, ALsizei numchans, const ALfloat mtx[4], ALfloat ingain, ALfloat gains[MAX_OUTPUT_CHANNELS])
{
    ALsizei i;

    for(i = 0;i < numchans;i++)
        gains[i] = chanmap[i].Scale * mtx[chanmap[i].Index] * ingain;
    for(;i < MAX_OUTPUT_CHANNELS;i++)
        gains[i] = 0.0f;
}


static inline const char *GetLabelFromChannel(enum Channel channel)
{
    switch(channel)
    {
        case FrontLeft: return "front-left";
        case FrontRight: return "front-right";
        case FrontCenter: return "front-center";
        case LFE: return "lfe";
        case BackLeft: return "back-left";
        case BackRight: return "back-right";
        case BackCenter: return "back-center";
        case SideLeft: return "side-left";
        case SideRight: return "side-right";

        case UpperFrontLeft: return "upper-front-left";
        case UpperFrontRight: return "upper-front-right";
        case UpperBackLeft: return "upper-back-left";
        case UpperBackRight: return "upper-back-right";
        case LowerFrontLeft: return "lower-front-left";
        case LowerFrontRight: return "lower-front-right";
        case LowerBackLeft: return "lower-back-left";
        case LowerBackRight: return "lower-back-right";

        case Aux0: return "aux-0";
        case Aux1: return "aux-1";
        case Aux2: return "aux-2";
        case Aux3: return "aux-3";
        case Aux4: return "aux-4";
        case Aux5: return "aux-5";
        case Aux6: return "aux-6";
        case Aux7: return "aux-7";
        case Aux8: return "aux-8";
        case Aux9: return "aux-9";
        case Aux10: return "aux-10";
        case Aux11: return "aux-11";
        case Aux12: return "aux-12";
        case Aux13: return "aux-13";
        case Aux14: return "aux-14";
        case Aux15: return "aux-15";

        case InvalidChannel: break;
    }
    return "(unknown)";
}


typedef struct ChannelMap {
    enum Channel ChanName;
    ChannelConfig Config;
} ChannelMap;

static void SetChannelMap(const enum Channel *devchans, ChannelConfig *ambicoeffs,
                          const ChannelMap *chanmap, size_t count, ALsizei *outcount)
{
    size_t j, k;
    ALsizei i;

    for(i = 0;i < MAX_OUTPUT_CHANNELS && devchans[i] != InvalidChannel;i++)
    {
        if(devchans[i] == LFE)
        {
            for(j = 0;j < MAX_AMBI_COEFFS;j++)
                ambicoeffs[i][j] = 0.0f;
            continue;
        }

        for(j = 0;j < count;j++)
        {
            if(devchans[i] != chanmap[j].ChanName)
                continue;

            for(k = 0;k < MAX_AMBI_COEFFS;++k)
                ambicoeffs[i][k] = chanmap[j].Config[k];
            break;
        }
        if(j == count)
            ERR("Failed to match %s channel (%u) in channel map\n", GetLabelFromChannel(devchans[i]), i);
    }
    *outcount = i;
}

static bool MakeSpeakerMap(ALCdevice *device, const AmbDecConf *conf, ALsizei speakermap[MAX_OUTPUT_CHANNELS])
{
    ALsizei i;

    for(i = 0;i < conf->NumSpeakers;i++)
    {
        int c = -1;

        /* NOTE: AmbDec does not define any standard speaker names, however
         * for this to work we have to by able to find the output channel
         * the speaker definition corresponds to. Therefore, OpenAL Soft
         * requires these channel labels to be recognized:
         *
         * LF = Front left
         * RF = Front right
         * LS = Side left
         * RS = Side right
         * LB = Back left
         * RB = Back right
         * CE = Front center
         * CB = Back center
         *
         * Additionally, surround51 will acknowledge back speakers for side
         * channels, and surround51rear will acknowledge side speakers for
         * back channels, to avoid issues with an ambdec expecting 5.1 to
         * use the side channels when the device is configured for back,
         * and vice-versa.
         */
        if(alstr_cmp_cstr(conf->Speakers[i].Name, "LF") == 0)
            c = GetChannelIdxByName(device->RealOut, FrontLeft);
        else if(alstr_cmp_cstr(conf->Speakers[i].Name, "RF") == 0)
            c = GetChannelIdxByName(device->RealOut, FrontRight);
        else if(alstr_cmp_cstr(conf->Speakers[i].Name, "CE") == 0)
            c = GetChannelIdxByName(device->RealOut, FrontCenter);
        else if(alstr_cmp_cstr(conf->Speakers[i].Name, "LS") == 0)
        {
            if(device->FmtChans == DevFmtX51Rear)
                c = GetChannelIdxByName(device->RealOut, BackLeft);
            else
                c = GetChannelIdxByName(device->RealOut, SideLeft);
        }
        else if(alstr_cmp_cstr(conf->Speakers[i].Name, "RS") == 0)
        {
            if(device->FmtChans == DevFmtX51Rear)
                c = GetChannelIdxByName(device->RealOut, BackRight);
            else
                c = GetChannelIdxByName(device->RealOut, SideRight);
        }
        else if(alstr_cmp_cstr(conf->Speakers[i].Name, "LB") == 0)
        {
            if(device->FmtChans == DevFmtX51)
                c = GetChannelIdxByName(device->RealOut, SideLeft);
            else
                c = GetChannelIdxByName(device->RealOut, BackLeft);
        }
        else if(alstr_cmp_cstr(conf->Speakers[i].Name, "RB") == 0)
        {
            if(device->FmtChans == DevFmtX51)
                c = GetChannelIdxByName(device->RealOut, SideRight);
            else
                c = GetChannelIdxByName(device->RealOut, BackRight);
        }
        else if(alstr_cmp_cstr(conf->Speakers[i].Name, "CB") == 0)
            c = GetChannelIdxByName(device->RealOut, BackCenter);
        else
        {
            const char *name = alstr_get_cstr(conf->Speakers[i].Name);
            unsigned int n;
            char ch;

            if(sscanf(name, "AUX%u%c", &n, &ch) == 1 && n < 16)
                c = GetChannelIdxByName(device->RealOut, Aux0+n);
            else
            {
                ERR("AmbDec speaker label \"%s\" not recognized\n", name);
                return false;
            }
        }
        if(c == -1)
        {
            ERR("Failed to lookup AmbDec speaker label %s\n",
                alstr_get_cstr(conf->Speakers[i].Name));
            return false;
        }
        speakermap[i] = c;
    }

    return true;
}


static const ChannelMap MonoCfg[1] = {
    { FrontCenter, { 1.0f } },
}, StereoCfg[2] = {
    { FrontLeft,   { 5.00000000e-1f,  2.88675135e-1f, 0.0f,  1.19573156e-1f } },
    { FrontRight,  { 5.00000000e-1f, -2.88675135e-1f, 0.0f,  1.19573156e-1f } },
}, QuadCfg[4] = {
    { BackLeft,    { 3.53553391e-1f,  2.04124145e-1f, 0.0f, -2.04124145e-1f } },
    { FrontLeft,   { 3.53553391e-1f,  2.04124145e-1f, 0.0f,  2.04124145e-1f } },
    { FrontRight,  { 3.53553391e-1f, -2.04124145e-1f, 0.0f,  2.04124145e-1f } },
    { BackRight,   { 3.53553391e-1f, -2.04124145e-1f, 0.0f, -2.04124145e-1f } },
}, X51SideCfg[5] = {
    { SideLeft,    { 3.33001372e-1f,  1.89085671e-1f, 0.0f, -2.00041334e-1f, -2.12309737e-2f, 0.0f, 0.0f, 0.0f, -1.14573483e-2f } },
    { FrontLeft,   { 1.47751298e-1f,  1.28994110e-1f, 0.0f,  1.15190495e-1f,  7.44949143e-2f, 0.0f, 0.0f, 0.0f, -6.47739980e-3f } },
    { FrontCenter, { 7.73595729e-2f,  0.00000000e+0f, 0.0f,  9.71390298e-2f,  0.00000000e+0f, 0.0f, 0.0f, 0.0f,  5.18625335e-2f } },
    { FrontRight,  { 1.47751298e-1f, -1.28994110e-1f, 0.0f,  1.15190495e-1f, -7.44949143e-2f, 0.0f, 0.0f, 0.0f, -6.47739980e-3f } },
    { SideRight,   { 3.33001372e-1f, -1.89085671e-1f, 0.0f, -2.00041334e-1f,  2.12309737e-2f, 0.0f, 0.0f, 0.0f, -1.14573483e-2f } },
}, X51RearCfg[5] = {
    { BackLeft,    { 3.33001372e-1f,  1.89085671e-1f, 0.0f, -2.00041334e-1f, -2.12309737e-2f, 0.0f, 0.0f, 0.0f, -1.14573483e-2f } },
    { FrontLeft,   { 1.47751298e-1f,  1.28994110e-1f, 0.0f,  1.15190495e-1f,  7.44949143e-2f, 0.0f, 0.0f, 0.0f, -6.47739980e-3f } },
    { FrontCenter, { 7.73595729e-2f,  0.00000000e+0f, 0.0f,  9.71390298e-2f,  0.00000000e+0f, 0.0f, 0.0f, 0.0f,  5.18625335e-2f } },
    { FrontRight,  { 1.47751298e-1f, -1.28994110e-1f, 0.0f,  1.15190495e-1f, -7.44949143e-2f, 0.0f, 0.0f, 0.0f, -6.47739980e-3f } },
    { BackRight,   { 3.33001372e-1f, -1.89085671e-1f, 0.0f, -2.00041334e-1f,  2.12309737e-2f, 0.0f, 0.0f, 0.0f, -1.14573483e-2f } },
}, X61Cfg[6] = {
    { SideLeft,    { 2.04462744e-1f,  2.17178497e-1f, 0.0f, -4.39990188e-2f, -2.60787329e-2f, 0.0f, 0.0f, 0.0f, -6.87238843e-2f } },
    { FrontLeft,   { 1.18130342e-1f,  9.34633906e-2f, 0.0f,  1.08553749e-1f,  6.80658795e-2f, 0.0f, 0.0f, 0.0f,  1.08999485e-2f } },
    { FrontCenter, { 7.73595729e-2f,  0.00000000e+0f, 0.0f,  9.71390298e-2f,  0.00000000e+0f, 0.0f, 0.0f, 0.0f,  5.18625335e-2f } },
    { FrontRight,  { 1.18130342e-1f, -9.34633906e-2f, 0.0f,  1.08553749e-1f, -6.80658795e-2f, 0.0f, 0.0f, 0.0f,  1.08999485e-2f } },
    { SideRight,   { 2.04462744e-1f, -2.17178497e-1f, 0.0f, -4.39990188e-2f,  2.60787329e-2f, 0.0f, 0.0f, 0.0f, -6.87238843e-2f } },
    { BackCenter,  { 2.50001688e-1f,  0.00000000e+0f, 0.0f, -2.50000094e-1f,  0.00000000e+0f, 0.0f, 0.0f, 0.0f,  6.05133395e-2f } },
}, X71Cfg[6] = {
    { BackLeft,    { 2.04124145e-1f,  1.08880247e-1f, 0.0f, -1.88586120e-1f, -1.29099444e-1f, 0.0f, 0.0f, 0.0f,  7.45355993e-2f,  3.73460789e-2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.00000000e+0f } },
    { SideLeft,    { 2.04124145e-1f,  2.17760495e-1f, 0.0f,  0.00000000e+0f,  0.00000000e+0f, 0.0f, 0.0f, 0.0f, -1.49071198e-1f, -3.73460789e-2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.00000000e+0f } },
    { FrontLeft,   { 2.04124145e-1f,  1.08880247e-1f, 0.0f,  1.88586120e-1f,  1.29099444e-1f, 0.0f, 0.0f, 0.0f,  7.45355993e-2f,  3.73460789e-2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.00000000e+0f } },
    { FrontRight,  { 2.04124145e-1f, -1.08880247e-1f, 0.0f,  1.88586120e-1f, -1.29099444e-1f, 0.0f, 0.0f, 0.0f,  7.45355993e-2f, -3.73460789e-2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.00000000e+0f } },
    { SideRight,   { 2.04124145e-1f, -2.17760495e-1f, 0.0f,  0.00000000e+0f,  0.00000000e+0f, 0.0f, 0.0f, 0.0f, -1.49071198e-1f,  3.73460789e-2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.00000000e+0f } },
    { BackRight,   { 2.04124145e-1f, -1.08880247e-1f, 0.0f, -1.88586120e-1f,  1.29099444e-1f, 0.0f, 0.0f, 0.0f,  7.45355993e-2f, -3.73460789e-2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.00000000e+0f } },
};

static void InitNearFieldCtrl(ALCdevice *device, ALfloat ctrl_dist, ALsizei order, bool periphonic)
{
    const char *devname = alstr_get_cstr(device->DeviceName);
    ALsizei i;

    if(GetConfigValueBool(devname, "decoder", "nfc", 1) && ctrl_dist > 0.0f)
    {
        /* NFC is only used when AvgSpeakerDist is greater than 0, and
         * METERS_PER_UNIT is also greater than 0. In addition, NFC can only be
         * used when rendering to an ambisonic buffer.
         */
        device->AvgSpeakerDist = ctrl_dist;

        device->Dry.NumChannelsPerOrder[0] = 1;
        if(periphonic)
            for(i = 1;i < order+1;i++)
                device->Dry.NumChannelsPerOrder[i] = (i+1)*(i+1) - i*i;
        else
            for(i = 1;i < order+1;i++)
                device->Dry.NumChannelsPerOrder[i] = (i*2+1) - ((i-1)*2+1);
        for(;i < MAX_AMBI_ORDER+1;i++)
            device->Dry.NumChannelsPerOrder[i] = 0;
    }
}

static void InitDistanceComp(ALCdevice *device, const AmbDecConf *conf, const ALsizei speakermap[MAX_OUTPUT_CHANNELS])
{
    const char *devname = alstr_get_cstr(device->DeviceName);
    ALfloat maxdist = 0.0f;
    ALsizei total = 0;
    ALsizei i;

    for(i = 0;i < conf->NumSpeakers;i++)
        maxdist = maxf(maxdist, conf->Speakers[i].Distance);

    if(GetConfigValueBool(devname, "decoder", "distance-comp", 1) && maxdist > 0.0f)
    {
        ALfloat srate = (ALfloat)device->Frequency;
        for(i = 0;i < conf->NumSpeakers;i++)
        {
            ALsizei chan = speakermap[i];
            ALfloat delay;

            /* Distance compensation only delays in steps of the sample rate.
             * This is a bit less accurate since the delay time falls to the
             * nearest sample time, but it's far simpler as it doesn't have to
             * deal with phase offsets. This means at 48khz, for instance, the
             * distance delay will be in steps of about 7 millimeters.
             */
            delay = floorf((maxdist-conf->Speakers[i].Distance) / SPEEDOFSOUNDMETRESPERSEC *
                           srate + 0.5f);
            if(delay >= (ALfloat)MAX_DELAY_LENGTH)
                ERR("Delay for speaker \"%s\" exceeds buffer length (%f >= %u)\n",
                    alstr_get_cstr(conf->Speakers[i].Name), delay, MAX_DELAY_LENGTH);

            device->ChannelDelay[chan].Length = (ALsizei)clampf(
                delay, 0.0f, (ALfloat)(MAX_DELAY_LENGTH-1)
            );
            device->ChannelDelay[chan].Gain = conf->Speakers[i].Distance / maxdist;
            TRACE("Channel %u \"%s\" distance compensation: %d samples, %f gain\n", chan,
                  alstr_get_cstr(conf->Speakers[i].Name), device->ChannelDelay[chan].Length,
                device->ChannelDelay[chan].Gain
            );

            /* Round up to the next 4th sample, so each channel buffer starts
             * 16-byte aligned.
             */
            total += RoundUp(device->ChannelDelay[chan].Length, 4);
        }
    }

    if(total > 0)
    {
        device->ChannelDelay[0].Buffer = al_calloc(16, total * sizeof(ALfloat));
        for(i = 1;i < MAX_OUTPUT_CHANNELS;i++)
        {
            size_t len = RoundUp(device->ChannelDelay[i-1].Length, 4);
            device->ChannelDelay[i].Buffer = device->ChannelDelay[i-1].Buffer + len;
        }
    }
}

static void InitPanning(ALCdevice *device)
{
    const ChannelMap *chanmap = NULL;
    ALsizei coeffcount = 0;
    ALsizei count = 0;
    ALsizei i, j;

    switch(device->FmtChans)
    {
        case DevFmtMono:
            count = COUNTOF(MonoCfg);
            chanmap = MonoCfg;
            coeffcount = 1;
            break;

        case DevFmtStereo:
            count = COUNTOF(StereoCfg);
            chanmap = StereoCfg;
            coeffcount = 4;
            break;

        case DevFmtQuad:
            count = COUNTOF(QuadCfg);
            chanmap = QuadCfg;
            coeffcount = 4;
            break;

        case DevFmtX51:
            count = COUNTOF(X51SideCfg);
            chanmap = X51SideCfg;
            coeffcount = 9;
            break;

        case DevFmtX51Rear:
            count = COUNTOF(X51RearCfg);
            chanmap = X51RearCfg;
            coeffcount = 9;
            break;

        case DevFmtX61:
            count = COUNTOF(X61Cfg);
            chanmap = X61Cfg;
            coeffcount = 9;
            break;

        case DevFmtX71:
            count = COUNTOF(X71Cfg);
            chanmap = X71Cfg;
            coeffcount = 16;
            break;

        case DevFmtAmbi3D:
            break;
    }

    if(device->FmtChans == DevFmtAmbi3D)
    {
        const char *devname = alstr_get_cstr(device->DeviceName);
        const ALsizei *acnmap = (device->AmbiLayout == AmbiLayout_FuMa) ? FuMa2ACN : ACN2ACN;
        const ALfloat *n3dscale = (device->AmbiScale == AmbiNorm_FuMa) ? FuMa2N3DScale :
                                  (device->AmbiScale == AmbiNorm_SN3D) ? SN3D2N3DScale :
                                  /*(device->AmbiScale == AmbiNorm_N3D) ?*/ UnitScale;
        ALfloat nfc_delay = 0.0f;

        count = (device->AmbiOrder == 3) ? 16 :
                (device->AmbiOrder == 2) ? 9 :
                (device->AmbiOrder == 1) ? 4 : 1;
        for(i = 0;i < count;i++)
        {
            ALsizei acn = acnmap[i];
            device->Dry.Ambi.Map[i].Scale = 1.0f/n3dscale[acn];
            device->Dry.Ambi.Map[i].Index = acn;
        }
        device->Dry.CoeffCount = 0;
        device->Dry.NumChannels = count;

        if(device->AmbiOrder < 2)
        {
            device->FOAOut.Ambi = device->Dry.Ambi;
            device->FOAOut.CoeffCount = device->Dry.CoeffCount;
            device->FOAOut.NumChannels = 0;
        }
        else
        {
            /* FOA output is always ACN+N3D for higher-order ambisonic output.
             * The upsampler expects this and will convert it for output.
             */
            memset(&device->FOAOut.Ambi, 0, sizeof(device->FOAOut.Ambi));
            for(i = 0;i < 4;i++)
            {
                device->FOAOut.Ambi.Map[i].Scale = 1.0f;
                device->FOAOut.Ambi.Map[i].Index = i;
            }
            device->FOAOut.CoeffCount = 0;
            device->FOAOut.NumChannels = 4;

            ambiup_reset(device->AmbiUp, device);
        }

        if(ConfigValueFloat(devname, "decoder", "nfc-ref-delay", &nfc_delay) && nfc_delay > 0.0f)
        {
            nfc_delay = clampf(nfc_delay, 0.001f, 1000.0f);
            InitNearFieldCtrl(device, nfc_delay * SPEEDOFSOUNDMETRESPERSEC,
                              device->AmbiOrder, true);
        }
    }
    else
    {
        ALfloat w_scale, xyz_scale;

        SetChannelMap(device->RealOut.ChannelName, device->Dry.Ambi.Coeffs,
                      chanmap, count, &device->Dry.NumChannels);
        device->Dry.CoeffCount = coeffcount;

        w_scale = (device->Dry.CoeffCount > 9) ? W_SCALE2D_THIRD :
                  (device->Dry.CoeffCount > 4) ? W_SCALE2D_SECOND : 1.0f;
        xyz_scale = (device->Dry.CoeffCount > 9) ? XYZ_SCALE2D_THIRD :
                    (device->Dry.CoeffCount > 4) ? XYZ_SCALE2D_SECOND : 1.0f;

        memset(&device->FOAOut.Ambi, 0, sizeof(device->FOAOut.Ambi));
        for(i = 0;i < device->Dry.NumChannels;i++)
        {
            device->FOAOut.Ambi.Coeffs[i][0] = device->Dry.Ambi.Coeffs[i][0] * w_scale;
            for(j = 1;j < 4;j++)
                device->FOAOut.Ambi.Coeffs[i][j] = device->Dry.Ambi.Coeffs[i][j] * xyz_scale;
        }
        device->FOAOut.CoeffCount = 4;
        device->FOAOut.NumChannels = 0;
    }
    device->RealOut.NumChannels = 0;
}

static void InitCustomPanning(ALCdevice *device, const AmbDecConf *conf, const ALsizei speakermap[MAX_OUTPUT_CHANNELS])
{
    ChannelMap chanmap[MAX_OUTPUT_CHANNELS];
    const ALfloat *coeff_scale = UnitScale;
    ALfloat w_scale = 1.0f;
    ALfloat xyz_scale = 1.0f;
    ALsizei i, j;

    if(conf->FreqBands != 1)
        ERR("Basic renderer uses the high-frequency matrix as single-band (xover_freq = %.0fhz)\n",
            conf->XOverFreq);

    if((conf->ChanMask&AMBI_PERIPHONIC_MASK))
    {
        if(conf->ChanMask > 0x1ff)
        {
            w_scale = W_SCALE3D_THIRD;
            xyz_scale = XYZ_SCALE3D_THIRD;
        }
        else if(conf->ChanMask > 0xf)
        {
            w_scale = W_SCALE3D_SECOND;
            xyz_scale = XYZ_SCALE3D_SECOND;
        }
    }
    else
    {
        if(conf->ChanMask > 0x1ff)
        {
            w_scale = W_SCALE2D_THIRD;
            xyz_scale = XYZ_SCALE2D_THIRD;
        }
        else if(conf->ChanMask > 0xf)
        {
            w_scale = W_SCALE2D_SECOND;
            xyz_scale = XYZ_SCALE2D_SECOND;
        }
    }

    if(conf->CoeffScale == ADS_SN3D)
        coeff_scale = SN3D2N3DScale;
    else if(conf->CoeffScale == ADS_FuMa)
        coeff_scale = FuMa2N3DScale;

    for(i = 0;i < conf->NumSpeakers;i++)
    {
        ALsizei chan = speakermap[i];
        ALfloat gain;
        ALsizei k = 0;

        for(j = 0;j < MAX_AMBI_COEFFS;j++)
            chanmap[i].Config[j] = 0.0f;

        chanmap[i].ChanName = device->RealOut.ChannelName[chan];
        for(j = 0;j < MAX_AMBI_COEFFS;j++)
        {
            if(j == 0) gain = conf->HFOrderGain[0];
            else if(j == 1) gain = conf->HFOrderGain[1];
            else if(j == 4) gain = conf->HFOrderGain[2];
            else if(j == 9) gain = conf->HFOrderGain[3];
            if((conf->ChanMask&(1<<j)))
                chanmap[i].Config[j] = conf->HFMatrix[i][k++] / coeff_scale[j] * gain;
        }
    }

    SetChannelMap(device->RealOut.ChannelName, device->Dry.Ambi.Coeffs, chanmap,
                  conf->NumSpeakers, &device->Dry.NumChannels);
    device->Dry.CoeffCount = (conf->ChanMask > 0x1ff) ? 16 :
                             (conf->ChanMask > 0xf) ? 9 : 4;

    memset(&device->FOAOut.Ambi, 0, sizeof(device->FOAOut.Ambi));
    for(i = 0;i < device->Dry.NumChannels;i++)
    {
        device->FOAOut.Ambi.Coeffs[i][0] = device->Dry.Ambi.Coeffs[i][0] * w_scale;
        for(j = 1;j < 4;j++)
            device->FOAOut.Ambi.Coeffs[i][j] = device->Dry.Ambi.Coeffs[i][j] * xyz_scale;
    }
    device->FOAOut.CoeffCount = 4;
    device->FOAOut.NumChannels = 0;

    device->RealOut.NumChannels = 0;

    InitDistanceComp(device, conf, speakermap);
}

static void InitHQPanning(ALCdevice *device, const AmbDecConf *conf, const ALsizei speakermap[MAX_OUTPUT_CHANNELS])
{
    ALfloat avg_dist;
    ALsizei count;
    ALsizei i;

    if((conf->ChanMask&AMBI_PERIPHONIC_MASK))
    {
        count = (conf->ChanMask > 0x1ff) ? 16 :
                (conf->ChanMask > 0xf) ? 9 : 4;
        for(i = 0;i < count;i++)
        {
            device->Dry.Ambi.Map[i].Scale = 1.0f;
            device->Dry.Ambi.Map[i].Index = i;
        }
    }
    else
    {
        static const int map[MAX_AMBI2D_COEFFS] = { 0, 1, 3, 4, 8, 9, 15 };

        count = (conf->ChanMask > 0x1ff) ? 7 :
                (conf->ChanMask > 0xf) ? 5 : 3;
        for(i = 0;i < count;i++)
        {
            device->Dry.Ambi.Map[i].Scale = 1.0f;
            device->Dry.Ambi.Map[i].Index = map[i];
        }
    }
    device->Dry.CoeffCount = 0;
    device->Dry.NumChannels = count;

    TRACE("Enabling %s-band %s-order%s ambisonic decoder\n",
        (conf->FreqBands == 1) ? "single" : "dual",
        (conf->ChanMask > 0xf) ? (conf->ChanMask > 0x1ff) ? "third" : "second" : "first",
        (conf->ChanMask&AMBI_PERIPHONIC_MASK) ? " periphonic" : ""
    );
    bformatdec_reset(device->AmbiDecoder, conf, count, device->Frequency, speakermap);

    if(!(conf->ChanMask > 0xf))
    {
        device->FOAOut.Ambi = device->Dry.Ambi;
        device->FOAOut.CoeffCount = device->Dry.CoeffCount;
        device->FOAOut.NumChannels = 0;
    }
    else
    {
        memset(&device->FOAOut.Ambi, 0, sizeof(device->FOAOut.Ambi));
        if((conf->ChanMask&AMBI_PERIPHONIC_MASK))
        {
            count = 4;
            for(i = 0;i < count;i++)
            {
                device->FOAOut.Ambi.Map[i].Scale = 1.0f;
                device->FOAOut.Ambi.Map[i].Index = i;
            }
        }
        else
        {
            static const int map[3] = { 0, 1, 3 };
            count = 3;
            for(i = 0;i < count;i++)
            {
                device->FOAOut.Ambi.Map[i].Scale = 1.0f;
                device->FOAOut.Ambi.Map[i].Index = map[i];
            }
        }
        device->FOAOut.CoeffCount = 0;
        device->FOAOut.NumChannels = count;
    }

    device->RealOut.NumChannels = ChannelsFromDevFmt(device->FmtChans, device->AmbiOrder);

    avg_dist = 0.0f;
    for(i = 0;i < conf->NumSpeakers;i++)
        avg_dist += conf->Speakers[i].Distance;
    avg_dist /= (ALfloat)conf->NumSpeakers;
    InitNearFieldCtrl(device, avg_dist,
        (conf->ChanMask > 0x1ff) ? 3 : (conf->ChanMask > 0xf) ? 2 : 1,
        !!(conf->ChanMask&AMBI_PERIPHONIC_MASK)
    );

    InitDistanceComp(device, conf, speakermap);
}

static void InitHrtfPanning(ALCdevice *device)
{
    /* NOTE: azimuth goes clockwise. */
    static const ALfloat AmbiPoints[][2] = {
        { DEG2RAD( 90.0f), DEG2RAD(   0.0f) },
        { DEG2RAD( 35.0f), DEG2RAD( -45.0f) },
        { DEG2RAD( 35.0f), DEG2RAD(  45.0f) },
        { DEG2RAD( 35.0f), DEG2RAD( 135.0f) },
        { DEG2RAD( 35.0f), DEG2RAD(-135.0f) },
        { DEG2RAD(  0.0f), DEG2RAD(   0.0f) },
        { DEG2RAD(  0.0f), DEG2RAD(  90.0f) },
        { DEG2RAD(  0.0f), DEG2RAD( 180.0f) },
        { DEG2RAD(  0.0f), DEG2RAD( -90.0f) },
        { DEG2RAD(-35.0f), DEG2RAD( -45.0f) },
        { DEG2RAD(-35.0f), DEG2RAD(  45.0f) },
        { DEG2RAD(-35.0f), DEG2RAD( 135.0f) },
        { DEG2RAD(-35.0f), DEG2RAD(-135.0f) },
        { DEG2RAD(-90.0f), DEG2RAD(   0.0f) },
    };
    static const ALfloat AmbiMatrixFOA[][2][MAX_AMBI_COEFFS] = {
        { { 1.88982237e-001f,  0.00000000e+000f,  1.90399923e-001f,  0.00000000e+000f }, { 7.14285714e-002f,  0.00000000e+000f,  1.24646009e-001f,  0.00000000e+000f } },
        { { 1.88982237e-001f,  1.09057783e-001f,  1.09208910e-001f,  1.09057783e-001f }, { 7.14285714e-002f,  7.13950780e-002f,  7.14940135e-002f,  7.13950780e-002f } },
        { { 1.88982237e-001f, -1.09057783e-001f,  1.09208910e-001f,  1.09057783e-001f }, { 7.14285714e-002f, -7.13950780e-002f,  7.14940135e-002f,  7.13950780e-002f } },
        { { 1.88982237e-001f, -1.09057783e-001f,  1.09208910e-001f, -1.09057783e-001f }, { 7.14285714e-002f, -7.13950780e-002f,  7.14940135e-002f, -7.13950780e-002f } },
        { { 1.88982237e-001f,  1.09057783e-001f,  1.09208910e-001f, -1.09057783e-001f }, { 7.14285714e-002f,  7.13950780e-002f,  7.14940135e-002f, -7.13950780e-002f } },
        { { 1.88982237e-001f,  0.00000000e+000f,  0.00000000e+000f,  1.88281281e-001f }, { 7.14285714e-002f,  0.00000000e+000f,  0.00000000e+000f,  1.23259031e-001f } },
        { { 1.88982237e-001f, -1.88281281e-001f,  0.00000000e+000f,  0.00000000e+000f }, { 7.14285714e-002f, -1.23259031e-001f,  0.00000000e+000f,  0.00000000e+000f } },
        { { 1.88982237e-001f,  0.00000000e+000f,  0.00000000e+000f, -1.88281281e-001f }, { 7.14285714e-002f,  0.00000000e+000f,  0.00000000e+000f, -1.23259031e-001f } },
        { { 1.88982237e-001f,  1.88281281e-001f,  0.00000000e+000f,  0.00000000e+000f }, { 7.14285714e-002f,  1.23259031e-001f,  0.00000000e+000f,  0.00000000e+000f } },
        { { 1.88982237e-001f,  1.09057783e-001f, -1.09208910e-001f,  1.09057783e-001f }, { 7.14285714e-002f,  7.13950780e-002f, -7.14940135e-002f,  7.13950780e-002f } },
        { { 1.88982237e-001f, -1.09057783e-001f, -1.09208910e-001f,  1.09057783e-001f }, { 7.14285714e-002f, -7.13950780e-002f, -7.14940135e-002f,  7.13950780e-002f } },
        { { 1.88982237e-001f, -1.09057783e-001f, -1.09208910e-001f, -1.09057783e-001f }, { 7.14285714e-002f, -7.13950780e-002f, -7.14940135e-002f, -7.13950780e-002f } },
        { { 1.88982237e-001f,  1.09057783e-001f, -1.09208910e-001f, -1.09057783e-001f }, { 7.14285714e-002f,  7.13950780e-002f, -7.14940135e-002f, -7.13950780e-002f } },
        { { 1.88982237e-001f,  0.00000000e+000f, -1.90399923e-001f,  0.00000000e+000f }, { 7.14285714e-002f,  0.00000000e+000f, -1.24646009e-001f,  0.00000000e+000f } }
    }, AmbiMatrixHOA[][2][MAX_AMBI_COEFFS] = {
        { { 1.43315266e-001f,  0.00000000e+000f,  1.90399923e-001f,  0.00000000e+000f,  0.00000000e+000f,  0.00000000e+000f,  1.18020996e-001f,  0.00000000e+000f,  0.00000000e+000f }, { 7.26741039e-002f,  0.00000000e+000f,  1.24646009e-001f,  0.00000000e+000f,  0.00000000e+000f,  0.00000000e+000f,  1.49618920e-001f,  0.00000000e+000f,  0.00000000e+000f } },
        { { 1.40852210e-001f,  1.09057783e-001f,  1.09208910e-001f,  1.09057783e-001f,  7.58818830e-002f,  7.66295578e-002f, -3.28314629e-004f,  7.66295578e-002f,  0.00000000e+000f }, { 7.14251066e-002f,  7.13950780e-002f,  7.14940135e-002f,  7.13950780e-002f,  9.61978444e-002f,  9.71456952e-002f, -4.16214759e-004f,  9.71456952e-002f,  0.00000000e+000f } },
        { { 1.40852210e-001f, -1.09057783e-001f,  1.09208910e-001f,  1.09057783e-001f, -7.58818830e-002f, -7.66295578e-002f, -3.28314629e-004f,  7.66295578e-002f,  0.00000000e+000f }, { 7.14251066e-002f, -7.13950780e-002f,  7.14940135e-002f,  7.13950780e-002f, -9.61978444e-002f, -9.71456952e-002f, -4.16214759e-004f,  9.71456952e-002f,  0.00000000e+000f } },
        { { 1.40852210e-001f, -1.09057783e-001f,  1.09208910e-001f, -1.09057783e-001f,  7.58818830e-002f, -7.66295578e-002f, -3.28314629e-004f, -7.66295578e-002f,  0.00000000e+000f }, { 7.14251066e-002f, -7.13950780e-002f,  7.14940135e-002f, -7.13950780e-002f,  9.61978444e-002f, -9.71456952e-002f, -4.16214759e-004f, -9.71456952e-002f,  0.00000000e+000f } },
        { { 1.40852210e-001f,  1.09057783e-001f,  1.09208910e-001f, -1.09057783e-001f, -7.58818830e-002f,  7.66295578e-002f, -3.28314629e-004f, -7.66295578e-002f,  0.00000000e+000f }, { 7.14251066e-002f,  7.13950780e-002f,  7.14940135e-002f, -7.13950780e-002f, -9.61978444e-002f,  9.71456952e-002f, -4.16214759e-004f, -9.71456952e-002f,  0.00000000e+000f } },
        { { 1.39644596e-001f,  0.00000000e+000f,  0.00000000e+000f,  1.88281281e-001f,  0.00000000e+000f,  0.00000000e+000f, -5.83538687e-002f,  0.00000000e+000f,  1.01835015e-001f }, { 7.08127349e-002f,  0.00000000e+000f,  0.00000000e+000f,  1.23259031e-001f,  0.00000000e+000f,  0.00000000e+000f, -7.39770307e-002f,  0.00000000e+000f,  1.29099445e-001f } },
        { { 1.39644596e-001f, -1.88281281e-001f,  0.00000000e+000f,  0.00000000e+000f,  0.00000000e+000f,  0.00000000e+000f, -5.83538687e-002f,  0.00000000e+000f, -1.01835015e-001f }, { 7.08127349e-002f, -1.23259031e-001f,  0.00000000e+000f,  0.00000000e+000f,  0.00000000e+000f,  0.00000000e+000f, -7.39770307e-002f,  0.00000000e+000f, -1.29099445e-001f } },
        { { 1.39644596e-001f,  0.00000000e+000f,  0.00000000e+000f, -1.88281281e-001f,  0.00000000e+000f,  0.00000000e+000f, -5.83538687e-002f,  0.00000000e+000f,  1.01835015e-001f }, { 7.08127349e-002f,  0.00000000e+000f,  0.00000000e+000f, -1.23259031e-001f,  0.00000000e+000f,  0.00000000e+000f, -7.39770307e-002f,  0.00000000e+000f,  1.29099445e-001f } },
        { { 1.39644596e-001f,  1.88281281e-001f,  0.00000000e+000f,  0.00000000e+000f,  0.00000000e+000f,  0.00000000e+000f, -5.83538687e-002f,  0.00000000e+000f, -1.01835015e-001f }, { 7.08127349e-002f,  1.23259031e-001f,  0.00000000e+000f,  0.00000000e+000f,  0.00000000e+000f,  0.00000000e+000f, -7.39770307e-002f,  0.00000000e+000f, -1.29099445e-001f } },
        { { 1.40852210e-001f,  1.09057783e-001f, -1.09208910e-001f,  1.09057783e-001f,  7.58818830e-002f, -7.66295578e-002f, -3.28314629e-004f, -7.66295578e-002f,  0.00000000e+000f }, { 7.14251066e-002f,  7.13950780e-002f, -7.14940135e-002f,  7.13950780e-002f,  9.61978444e-002f, -9.71456952e-002f, -4.16214759e-004f, -9.71456952e-002f,  0.00000000e+000f } },
        { { 1.40852210e-001f, -1.09057783e-001f, -1.09208910e-001f,  1.09057783e-001f, -7.58818830e-002f,  7.66295578e-002f, -3.28314629e-004f, -7.66295578e-002f,  0.00000000e+000f }, { 7.14251066e-002f, -7.13950780e-002f, -7.14940135e-002f,  7.13950780e-002f, -9.61978444e-002f,  9.71456952e-002f, -4.16214759e-004f, -9.71456952e-002f,  0.00000000e+000f } },
        { { 1.40852210e-001f, -1.09057783e-001f, -1.09208910e-001f, -1.09057783e-001f,  7.58818830e-002f,  7.66295578e-002f, -3.28314629e-004f,  7.66295578e-002f,  0.00000000e+000f }, { 7.14251066e-002f, -7.13950780e-002f, -7.14940135e-002f, -7.13950780e-002f,  9.61978444e-002f,  9.71456952e-002f, -4.16214759e-004f,  9.71456952e-002f,  0.00000000e+000f } },
        { { 1.40852210e-001f,  1.09057783e-001f, -1.09208910e-001f, -1.09057783e-001f, -7.58818830e-002f, -7.66295578e-002f, -3.28314629e-004f,  7.66295578e-002f,  0.00000000e+000f }, { 7.14251066e-002f,  7.13950780e-002f, -7.14940135e-002f, -7.13950780e-002f, -9.61978444e-002f, -9.71456952e-002f, -4.16214759e-004f,  9.71456952e-002f,  0.00000000e+000f } },
        { { 1.43315266e-001f,  0.00000000e+000f, -1.90399923e-001f,  0.00000000e+000f,  0.00000000e+000f,  0.00000000e+000f,  1.18020996e-001f,  0.00000000e+000f,  0.00000000e+000f }, { 7.26741039e-002f,  0.00000000e+000f, -1.24646009e-001f,  0.00000000e+000f,  0.00000000e+000f,  0.00000000e+000f,  1.49618920e-001f,  0.00000000e+000f,  0.00000000e+000f } },
    };
    const ALfloat (*AmbiMatrix)[2][MAX_AMBI_COEFFS] = device->AmbiUp ? AmbiMatrixHOA :
                                                                       AmbiMatrixFOA;
    ALsizei count = device->AmbiUp ? 9 : 4;
    ALsizei i;

    static_assert(COUNTOF(AmbiPoints) <= HRTF_AMBI_MAX_CHANNELS, "HRTF_AMBI_MAX_CHANNELS is too small");

    device->Hrtf = al_calloc(16, FAM_SIZE(DirectHrtfState, Chan, count));

    for(i = 0;i < count;i++)
    {
        device->Dry.Ambi.Map[i].Scale = 1.0f;
        device->Dry.Ambi.Map[i].Index = i;
    }
    device->Dry.CoeffCount = 0;
    device->Dry.NumChannels = count;

    if(device->AmbiUp)
    {
        memset(&device->FOAOut.Ambi, 0, sizeof(device->FOAOut.Ambi));
        for(i = 0;i < 4;i++)
        {
            device->FOAOut.Ambi.Map[i].Scale = 1.0f;
            device->FOAOut.Ambi.Map[i].Index = i;
        }
        device->FOAOut.CoeffCount = 0;
        device->FOAOut.NumChannels = 4;

        ambiup_reset(device->AmbiUp, device);
    }
    else
    {
        device->FOAOut.Ambi = device->Dry.Ambi;
        device->FOAOut.CoeffCount = device->Dry.CoeffCount;
        device->FOAOut.NumChannels = 0;
    }

    device->RealOut.NumChannels = ChannelsFromDevFmt(device->FmtChans, device->AmbiOrder);

    device->Hrtf->IrSize = BuildBFormatHrtf(device->HrtfHandle,
        device->Hrtf, device->Dry.NumChannels,
        AmbiPoints, AmbiMatrix, COUNTOF(AmbiPoints)
    );
}

static void InitUhjPanning(ALCdevice *device)
{
    ALsizei count = 3;
    ALsizei i;

    for(i = 0;i < count;i++)
    {
        ALsizei acn = FuMa2ACN[i];
        device->Dry.Ambi.Map[i].Scale = 1.0f/FuMa2N3DScale[acn];
        device->Dry.Ambi.Map[i].Index = acn;
    }
    device->Dry.CoeffCount = 0;
    device->Dry.NumChannels = count;

    device->FOAOut.Ambi = device->Dry.Ambi;
    device->FOAOut.CoeffCount = device->Dry.CoeffCount;
    device->FOAOut.NumChannels = 0;

    device->RealOut.NumChannels = ChannelsFromDevFmt(device->FmtChans, device->AmbiOrder);
}

void aluInitRenderer(ALCdevice *device, ALint hrtf_id, enum HrtfRequestMode hrtf_appreq, enum HrtfRequestMode hrtf_userreq)
{
    /* Hold the HRTF the device last used, in case it's used again. */
    struct Hrtf *old_hrtf = device->HrtfHandle;
    const char *mode;
    bool headphones;
    int bs2blevel;
    size_t i;

    al_free(device->Hrtf);
    device->Hrtf = NULL;
    device->HrtfHandle = NULL;
    alstr_clear(&device->HrtfName);
    device->Render_Mode = NormalRender;

    memset(&device->Dry.Ambi, 0, sizeof(device->Dry.Ambi));
    device->Dry.CoeffCount = 0;
    device->Dry.NumChannels = 0;
    for(i = 0;i < MAX_AMBI_ORDER+1;i++)
        device->Dry.NumChannelsPerOrder[i] = 0;

    device->AvgSpeakerDist = 0.0f;
    memset(device->ChannelDelay, 0, sizeof(device->ChannelDelay));
    for(i = 0;i < MAX_OUTPUT_CHANNELS;i++)
    {
        device->ChannelDelay[i].Gain = 1.0f;
        device->ChannelDelay[i].Length = 0;
    }

    if(device->FmtChans != DevFmtStereo)
    {
        ALsizei speakermap[MAX_OUTPUT_CHANNELS];
        const char *devname, *layout = NULL;
        AmbDecConf conf, *pconf = NULL;

        if(old_hrtf)
            Hrtf_DecRef(old_hrtf);
        old_hrtf = NULL;
        if(hrtf_appreq == Hrtf_Enable)
            device->HrtfStatus = ALC_HRTF_UNSUPPORTED_FORMAT_SOFT;

        ambdec_init(&conf);

        devname = alstr_get_cstr(device->DeviceName);
        switch(device->FmtChans)
        {
            case DevFmtQuad: layout = "quad"; break;
            case DevFmtX51: /* fall-through */
            case DevFmtX51Rear: layout = "surround51"; break;
            case DevFmtX61: layout = "surround61"; break;
            case DevFmtX71: layout = "surround71"; break;
            /* Mono, Stereo, and Ambisonics output don't use custom decoders. */
            case DevFmtMono:
            case DevFmtStereo:
            case DevFmtAmbi3D:
                break;
        }
        if(layout)
        {
            const char *fname;
            if(ConfigValueStr(devname, "decoder", layout, &fname))
            {
                if(!ambdec_load(&conf, fname))
                    ERR("Failed to load layout file %s\n", fname);
                else
                {
                    if(conf.ChanMask > 0xffff)
                        ERR("Unsupported channel mask 0x%04x (max 0xffff)\n", conf.ChanMask);
                    else
                    {
                        if(MakeSpeakerMap(device, &conf, speakermap))
                            pconf = &conf;
                    }
                }
            }
        }

        if(pconf && GetConfigValueBool(devname, "decoder", "hq-mode", 0))
        {
            ambiup_free(device->AmbiUp);
            device->AmbiUp = NULL;
            if(!device->AmbiDecoder)
                device->AmbiDecoder = bformatdec_alloc();
        }
        else
        {
            bformatdec_free(device->AmbiDecoder);
            device->AmbiDecoder = NULL;
            if(device->FmtChans == DevFmtAmbi3D && device->AmbiOrder > 1)
            {
                if(!device->AmbiUp)
                    device->AmbiUp = ambiup_alloc();
            }
            else
            {
                ambiup_free(device->AmbiUp);
                device->AmbiUp = NULL;
            }
        }

        if(!pconf)
            InitPanning(device);
        else if(device->AmbiDecoder)
            InitHQPanning(device, pconf, speakermap);
        else
            InitCustomPanning(device, pconf, speakermap);

        ambdec_deinit(&conf);
        return;
    }

    bformatdec_free(device->AmbiDecoder);
    device->AmbiDecoder = NULL;

    headphones = device->IsHeadphones;
    if(device->Type != Loopback)
    {
        const char *mode;
        if(ConfigValueStr(alstr_get_cstr(device->DeviceName), NULL, "stereo-mode", &mode))
        {
            if(strcasecmp(mode, "headphones") == 0)
                headphones = true;
            else if(strcasecmp(mode, "speakers") == 0)
                headphones = false;
            else if(strcasecmp(mode, "auto") != 0)
                ERR("Unexpected stereo-mode: %s\n", mode);
        }
    }

    if(hrtf_userreq == Hrtf_Default)
    {
        bool usehrtf = (headphones && hrtf_appreq != Hrtf_Disable) ||
                       (hrtf_appreq == Hrtf_Enable);
        if(!usehrtf) goto no_hrtf;

        device->HrtfStatus = ALC_HRTF_ENABLED_SOFT;
        if(headphones && hrtf_appreq != Hrtf_Disable)
            device->HrtfStatus = ALC_HRTF_HEADPHONES_DETECTED_SOFT;
    }
    else
    {
        if(hrtf_userreq != Hrtf_Enable)
        {
            if(hrtf_appreq == Hrtf_Enable)
                device->HrtfStatus = ALC_HRTF_DENIED_SOFT;
            goto no_hrtf;
        }
        device->HrtfStatus = ALC_HRTF_REQUIRED_SOFT;
    }

    if(VECTOR_SIZE(device->HrtfList) == 0)
    {
        VECTOR_DEINIT(device->HrtfList);
        device->HrtfList = EnumerateHrtf(device->DeviceName);
    }

    if(hrtf_id >= 0 && (size_t)hrtf_id < VECTOR_SIZE(device->HrtfList))
    {
        const EnumeratedHrtf *entry = &VECTOR_ELEM(device->HrtfList, hrtf_id);
        struct Hrtf *hrtf = GetLoadedHrtf(entry->hrtf);
        if(hrtf && hrtf->sampleRate == device->Frequency)
        {
            device->HrtfHandle = hrtf;
            alstr_copy(&device->HrtfName, entry->name);
        }
        else if(hrtf)
            Hrtf_DecRef(hrtf);
    }

    for(i = 0;!device->HrtfHandle && i < VECTOR_SIZE(device->HrtfList);i++)
    {
        const EnumeratedHrtf *entry = &VECTOR_ELEM(device->HrtfList, i);
        struct Hrtf *hrtf = GetLoadedHrtf(entry->hrtf);
        if(hrtf && hrtf->sampleRate == device->Frequency)
        {
            device->HrtfHandle = hrtf;
            alstr_copy(&device->HrtfName, entry->name);
        }
        else if(hrtf)
            Hrtf_DecRef(hrtf);
    }

    if(device->HrtfHandle)
    {
        if(old_hrtf)
            Hrtf_DecRef(old_hrtf);
        old_hrtf = NULL;

        device->Render_Mode = HrtfRender;
        if(ConfigValueStr(alstr_get_cstr(device->DeviceName), NULL, "hrtf-mode", &mode))
        {
            if(strcasecmp(mode, "full") == 0)
                device->Render_Mode = HrtfRender;
            else if(strcasecmp(mode, "basic") == 0)
                device->Render_Mode = NormalRender;
            else
                ERR("Unexpected hrtf-mode: %s\n", mode);
        }

        if(device->Render_Mode == HrtfRender)
        {
            /* Don't bother with HOA when using full HRTF rendering. Nothing
             * needs it, and it eases the CPU/memory load.
             */
            ambiup_free(device->AmbiUp);
            device->AmbiUp = NULL;
        }
        else
        {
            if(!device->AmbiUp)
                device->AmbiUp = ambiup_alloc();
        }

        TRACE("%s HRTF rendering enabled, using \"%s\"\n",
            ((device->Render_Mode == HrtfRender) ? "Full" : "Basic"),
            alstr_get_cstr(device->HrtfName)
        );
        InitHrtfPanning(device);
        return;
    }
    device->HrtfStatus = ALC_HRTF_UNSUPPORTED_FORMAT_SOFT;

no_hrtf:
    if(old_hrtf)
        Hrtf_DecRef(old_hrtf);
    old_hrtf = NULL;
    TRACE("HRTF disabled\n");

    device->Render_Mode = StereoPair;

    ambiup_free(device->AmbiUp);
    device->AmbiUp = NULL;

    bs2blevel = ((headphones && hrtf_appreq != Hrtf_Disable) ||
                 (hrtf_appreq == Hrtf_Enable)) ? 5 : 0;
    if(device->Type != Loopback)
        ConfigValueInt(alstr_get_cstr(device->DeviceName), NULL, "cf_level", &bs2blevel);
    if(bs2blevel > 0 && bs2blevel <= 6)
    {
        device->Bs2b = al_calloc(16, sizeof(*device->Bs2b));
        bs2b_set_params(device->Bs2b, bs2blevel, device->Frequency);
        TRACE("BS2B enabled\n");
        InitPanning(device);
        return;
    }

    TRACE("BS2B disabled\n");

    if(ConfigValueStr(alstr_get_cstr(device->DeviceName), NULL, "stereo-encoding", &mode))
    {
        if(strcasecmp(mode, "uhj") == 0)
            device->Render_Mode = NormalRender;
        else if(strcasecmp(mode, "panpot") != 0)
            ERR("Unexpected stereo-encoding: %s\n", mode);
    }
    if(device->Render_Mode == NormalRender)
    {
        device->Uhj_Encoder = al_calloc(16, sizeof(Uhj2Encoder));
        TRACE("UHJ enabled\n");
        InitUhjPanning(device);
        return;
    }

    TRACE("UHJ disabled\n");
    InitPanning(device);
}


void aluInitEffectPanning(ALeffectslot *slot)
{
    ALsizei i;

    memset(slot->ChanMap, 0, sizeof(slot->ChanMap));
    slot->NumChannels = 0;

    for(i = 0;i < MAX_EFFECT_CHANNELS;i++)
    {
        slot->ChanMap[i].Scale = 1.0f;
        slot->ChanMap[i].Index = i;
    }
    slot->NumChannels = i;
}
