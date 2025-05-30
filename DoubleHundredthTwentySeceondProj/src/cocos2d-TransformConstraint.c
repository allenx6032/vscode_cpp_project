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

#include <cocos2d-TransformConstraint.h>
#include <cocos2d-Skeleton.h>
#include <cocos2d-extension.h>

spTransformConstraint* spTransformConstraint_create (spTransformConstraintData* data, const spSkeleton* skeleton) {
	int i;
	spTransformConstraint* self = NEW(spTransformConstraint);
	CONST_CAST(spTransformConstraintData*, self->data) = data;
	self->rotateMix = data->rotateMix;
	self->translateMix = data->translateMix;
	self->scaleMix = data->scaleMix;
	self->shearMix = data->shearMix;
	self->bonesCount = data->bonesCount;
	CONST_CAST(spBone**, self->bones) = MALLOC(spBone*, self->bonesCount);
	for (i = 0; i < self->bonesCount; ++i)
		self->bones[i] = spSkeleton_findBone(skeleton, self->data->bones[i]->name);
	self->target = spSkeleton_findBone(skeleton, self->data->target->name);
	return self;
}

void spTransformConstraint_dispose (spTransformConstraint* self) {
	FREE(self->bones);
	FREE(self);
}

void spTransformConstraint_apply (spTransformConstraint* self) {
	float rotateMix = self->rotateMix, translateMix = self->translateMix, scaleMix = self->scaleMix, shearMix = self->shearMix;
	spBone* target = self->target;
	float ta = target->a, tb = target->b, tc = target->c, td = target->d;
	float degRadReflect = ta * td - tb * tc > 0 ? DEG_RAD : -DEG_RAD;
	float offsetRotation = self->data->offsetRotation * degRadReflect, offsetShearY = self->data->offsetShearY * degRadReflect;
	int /*bool*/ modified;
	int i;
	for (i = 0; i < self->bonesCount; ++i) {
		spBone* bone = self->bones[i];
		modified = 0;

		if (rotateMix != 0) {
			float a = bone->a, b = bone->b, c = bone->c, d = bone->d;
			float r = ATAN2(tc, ta) - ATAN2(c, a) + offsetRotation;
			float cosine, sine;
			if (r > PI) r -= PI2;
			else if (r < -PI) r += PI2;
			r *= rotateMix;
			cosine = COS(r);
			sine = SIN(r);
			CONST_CAST(float, bone->a) = cosine * a - sine * c;
			CONST_CAST(float, bone->b) = cosine * b - sine * d;
			CONST_CAST(float, bone->c) = sine * a + cosine * c;
			CONST_CAST(float, bone->d) = sine * b + cosine * d;
			modified = 1;
		}

		if (translateMix != 0) {
			float x, y;
			spBone_localToWorld(target, self->data->offsetX, self->data->offsetY, &x, &y);
			CONST_CAST(float, bone->worldX) += (x - bone->worldX) * translateMix;
			CONST_CAST(float, bone->worldY) += (y - bone->worldY) * translateMix;
			modified = 1;
		}

		if (scaleMix > 0) {
			float s = SQRT(bone->a * bone->a + bone->c * bone->c);
			float ts = SQRT(ta * ta + tc * tc);
			if (s > 0.00001f) s = (s + (ts - s + self->data->offsetScaleX) * scaleMix) / s;
			CONST_CAST(float, bone->a) *= s;
			CONST_CAST(float, bone->c) *= s;
			s = SQRT(bone->b * bone->b + bone->d * bone->d);
			ts = SQRT(tb * tb + td * td);
			if (s > 0.00001f) s = (s + (ts - s + self->data->offsetScaleY) * scaleMix) / s;
			CONST_CAST(float, bone->b) *= s;
			CONST_CAST(float, bone->d) *= s;
			modified = 1;
		}

		if (shearMix > 0) {
			float b = bone->b, d = bone->d;
			float by = ATAN2(d, b);
			float r = ATAN2(td, tb) - ATAN2(tc, ta) - (by - ATAN2(bone->c, bone->a));
			float s = SQRT(b * b + d * d);
			if (r > PI) r -= PI2;
			else if (r < -PI) r += PI2;
			r = by + (r + offsetShearY) * shearMix;
			CONST_CAST(float, bone->b) = COS(r) * s;
			CONST_CAST(float, bone->d) = SIN(r) * s;
			modified = 1;
		}

		if (modified) CONST_CAST(int, bone->appliedValid) = 0;
	}
}
