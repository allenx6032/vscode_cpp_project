/***********************************************************************
Copyright (c) 2006-2011, Skype Limited. All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
- Neither the name of Internet Society, IETF or IETF Trust, nor the
names of specific contributors, may be used to endorse or promote
products derived from this software without specific prior written
permission.
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
***********************************************************************/

#ifdef HAVE_CONFIG_H
#include "opus_config.h"
#endif

#include "silk_SigProc_FLP.h"

/* copy and multiply a vector by a constant */
void silk_scale_copy_vector_FLP(
    float          *data_out,
    const float    *data_in,
    float          gain,
    opus_int            dataSize
)
{
    opus_int  i, dataSize4;

    /* 4x unrolled loop */
    dataSize4 = dataSize & 0xFFFC;
    for( i = 0; i < dataSize4; i += 4 ) {
        data_out[ i + 0 ] = gain * data_in[ i + 0 ];
        data_out[ i + 1 ] = gain * data_in[ i + 1 ];
        data_out[ i + 2 ] = gain * data_in[ i + 2 ];
        data_out[ i + 3 ] = gain * data_in[ i + 3 ];
    }

    /* any remaining elements */
    for( ; i < dataSize; i++ ) {
        data_out[ i ] = gain * data_in[ i ];
    }
}
