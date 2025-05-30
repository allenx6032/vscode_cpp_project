/******************************************************************************
 * Spine Runtimes Software License
 * Version 2.3
 * 
 * Copyright (c) 2013-2015, Esoteric Software
 * All rights reserved.
 * 
 * You are granted a perpetual, non-exclusive, non-sublicensable and
 * non-transferable license to use, install, execute and perform the Spine
 * Runtimes Software (the "Software") and derivative works solely for personal
 * or internal use. Without the written permission of Esoteric Software (see
 * Section 2 of the Spine Software License Agreement), you may not (a) modify,
 * translate, adapt or otherwise create derivative works, improvements of the
 * Software or develop new applications using the Software or (b) remove,
 * delete, alter or obscure any trademarks or any copyright, trademark, patent
 * or other intellectual property or proprietary rights notices on or in the
 * Software, including any copy thereof. Redistributions in binary or source
 * form must include this license and terms.
 * 
 * THIS SOFTWARE IS PROVIDED BY ESOTERIC SOFTWARE "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ESOTERIC SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#ifndef SPINE_SLOTDATA_H_
#define SPINE_SLOTDATA_H_

#include <spine-BoneData.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	SP_BLEND_MODE_NORMAL, SP_BLEND_MODE_ADDITIVE, SP_BLEND_MODE_MULTIPLY, SP_BLEND_MODE_SCREEN
} spBlendMode;

typedef struct spSlotData {
	const char* const name;
	const spBoneData* const boneData;
	const char* attachmentName;
	float r, g, b, a;
	spBlendMode blendMode;

#ifdef __cplusplus
	spSlotData() :
		name(0),
		boneData(0),
		attachmentName(0),
		r(0), g(0), b(0), a(0),
		blendMode(SP_BLEND_MODE_NORMAL) {
	}
#endif
} spSlotData;

spSlotData* spSlotData_create (const char* name, spBoneData* boneData);
void spSlotData_dispose (spSlotData* self);

/* @param attachmentName May be 0 for no setup pose attachment. */
void spSlotData_setAttachmentName (spSlotData* self, const char* attachmentName);

#ifdef SPINE_SHORT_NAMES
typedef spBlendMode BlendMode;
#define BLEND_MODE_NORMAL SP_BLEND_MODE_NORMAL
#define BLEND_MODE_ADDITIVE SP_BLEND_MODE_ADDITIVE
#define BLEND_MODE_MULTIPLY SP_BLEND_MODE_MULTIPLY
#define BLEND_MODE_SCREEN SP_BLEND_MODE_SCREEN
typedef spSlotData SlotData;
#define SlotData_create(...) spSlotData_create(__VA_ARGS__)
#define SlotData_dispose(...) spSlotData_dispose(__VA_ARGS__)
#define SlotData_setAttachmentName(...) spSlotData_setAttachmentName(__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* SPINE_SLOTDATA_H_ */
