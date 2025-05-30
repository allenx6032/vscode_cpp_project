/******************************************************************************
 * Spine Runtimes Software License v2.5
 *
 * Copyright (c) 2013-2016, Esoteric Software
 * All rights reserved.
 *
 * You are granted a perpetual, non-exclusive, non-sublicensable, and
 * non-transferable license to use, install, execute, and perform the Spine
 * Runtimes software and derivative works solely for personal or internal
 * use. Without the written permission of Esoteric Software (see Section 2 of
 * the Spine Software License Agreement), you may not (a) modify, translate,
 * adapt, or develop new applications using the Spine Runtimes or otherwise
 * create derivative works or improvements of the Spine Runtimes or (b) remove,
 * delete, alter, or obscure any trademarks or any copyright, trademark, patent,
 * or other intellectual property or proprietary rights notices on or in the
 * Software, including any copy thereof. Redistributions in binary or source
 * form must include this license and terms.
 *
 * THIS SOFTWARE IS PROVIDED BY ESOTERIC SOFTWARE "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ESOTERIC SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, BUSINESS INTERRUPTION, OR LOSS OF
 * USE, DATA, OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include <cocos2d-RegionAttachment.h>
#include <cocos2d-extension.h>

void _spRegionAttachment_dispose (spAttachment* attachment) {
	spRegionAttachment* self = SUB_CAST(spRegionAttachment, attachment);
	_spAttachment_deinit(attachment);
	FREE(self->path);
	FREE(self);
}

spRegionAttachment* spRegionAttachment_create (const char* name) {
	spRegionAttachment* self = NEW(spRegionAttachment);
	self->scaleX = 1;
	self->scaleY = 1;
	self->r = 1;
	self->g = 1;
	self->b = 1;
	self->a = 1;
	_spAttachment_init(SUPER(self), name, SP_ATTACHMENT_REGION, _spRegionAttachment_dispose);
	return self;
}

void spRegionAttachment_setUVs (spRegionAttachment* self, float u, float v, float u2, float v2, int/*bool*/rotate) {
	if (rotate) {
		self->uvs[SP_VERTEX_X2] = u;
		self->uvs[SP_VERTEX_Y2] = v2;
		self->uvs[SP_VERTEX_X3] = u;
		self->uvs[SP_VERTEX_Y3] = v;
		self->uvs[SP_VERTEX_X4] = u2;
		self->uvs[SP_VERTEX_Y4] = v;
		self->uvs[SP_VERTEX_X1] = u2;
		self->uvs[SP_VERTEX_Y1] = v2;
	} else {
		self->uvs[SP_VERTEX_X1] = u;
		self->uvs[SP_VERTEX_Y1] = v2;
		self->uvs[SP_VERTEX_X2] = u;
		self->uvs[SP_VERTEX_Y2] = v;
		self->uvs[SP_VERTEX_X3] = u2;
		self->uvs[SP_VERTEX_Y3] = v;
		self->uvs[SP_VERTEX_X4] = u2;
		self->uvs[SP_VERTEX_Y4] = v2;
	}
}

void spRegionAttachment_updateOffset (spRegionAttachment* self) {
	float regionScaleX = self->width / self->regionOriginalWidth * self->scaleX;
	float regionScaleY = self->height / self->regionOriginalHeight * self->scaleY;
	float localX = -self->width / 2 * self->scaleX + self->regionOffsetX * regionScaleX;
	float localY = -self->height / 2 * self->scaleY + self->regionOffsetY * regionScaleY;
	float localX2 = localX + self->regionWidth * regionScaleX;
	float localY2 = localY + self->regionHeight * regionScaleY;
	float radians = self->rotation * DEG_RAD;
	float cosine = COS(radians), sine = SIN(radians);
	float localXCos = localX * cosine + self->x;
	float localXSin = localX * sine;
	float localYCos = localY * cosine + self->y;
	float localYSin = localY * sine;
	float localX2Cos = localX2 * cosine + self->x;
	float localX2Sin = localX2 * sine;
	float localY2Cos = localY2 * cosine + self->y;
	float localY2Sin = localY2 * sine;
	self->offset[SP_VERTEX_X1] = localXCos - localYSin;
	self->offset[SP_VERTEX_Y1] = localYCos + localXSin;
	self->offset[SP_VERTEX_X2] = localXCos - localY2Sin;
	self->offset[SP_VERTEX_Y2] = localY2Cos + localXSin;
	self->offset[SP_VERTEX_X3] = localX2Cos - localY2Sin;
	self->offset[SP_VERTEX_Y3] = localY2Cos + localX2Sin;
	self->offset[SP_VERTEX_X4] = localX2Cos - localYSin;
	self->offset[SP_VERTEX_Y4] = localYCos + localX2Sin;
}

void spRegionAttachment_computeWorldVertices (spRegionAttachment* self, spBone* bone, float* vertices) {
	const float* offset = self->offset;
	float x = bone->worldX, y = bone->worldY;
	vertices[SP_VERTEX_X1] = offset[SP_VERTEX_X1] * bone->a + offset[SP_VERTEX_Y1] * bone->b + x;
	vertices[SP_VERTEX_Y1] = offset[SP_VERTEX_X1] * bone->c + offset[SP_VERTEX_Y1] * bone->d + y;
	vertices[SP_VERTEX_X2] = offset[SP_VERTEX_X2] * bone->a + offset[SP_VERTEX_Y2] * bone->b + x;
	vertices[SP_VERTEX_Y2] = offset[SP_VERTEX_X2] * bone->c + offset[SP_VERTEX_Y2] * bone->d + y;
	vertices[SP_VERTEX_X3] = offset[SP_VERTEX_X3] * bone->a + offset[SP_VERTEX_Y3] * bone->b + x;
	vertices[SP_VERTEX_Y3] = offset[SP_VERTEX_X3] * bone->c + offset[SP_VERTEX_Y3] * bone->d + y;
	vertices[SP_VERTEX_X4] = offset[SP_VERTEX_X4] * bone->a + offset[SP_VERTEX_Y4] * bone->b + x;
	vertices[SP_VERTEX_Y4] = offset[SP_VERTEX_X4] * bone->c + offset[SP_VERTEX_Y4] * bone->d + y;
}
