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

#ifndef SPINE_SKELETON_H_
#define SPINE_SKELETON_H_

#include <cocos2d-SkeletonData.h>
#include <cocos2d-Slot.h>
#include <cocos2d-Skin.h>
#include <cocos2d-IkConstraint.h>
#include <cocos2d-TransformConstraint.h>
#include <cocos2d-PathConstraint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spSkeleton {
	spSkeletonData* const data;

	int bonesCount;
	spBone** bones;
	spBone* const root;

	int slotsCount;
	spSlot** slots;
	spSlot** drawOrder;

	int ikConstraintsCount;
	spIkConstraint** ikConstraints;

	int transformConstraintsCount;
	spTransformConstraint** transformConstraints;

	int pathConstraintsCount;
	spPathConstraint** pathConstraints;

	spSkin* const skin;
	float r, g, b, a;
	float time;
	int/*bool*/flipX, flipY;
	float x, y;

#ifdef __cplusplus
	spSkeleton() :
		data(0),
		bonesCount(0),
		bones(0),
		root(0),
		slotsCount(0),
		slots(0),
		drawOrder(0),

		ikConstraintsCount(0),
		ikConstraints(0),

		transformConstraintsCount(0),
		transformConstraints(0),

		skin(0),
		r(0), g(0), b(0), a(0),
		time(0),
		flipX(0),
		flipY(0),
		x(0), y(0) {
	}
#endif
} spSkeleton;

spSkeleton* spSkeleton_create (spSkeletonData* data);
void spSkeleton_dispose (spSkeleton* self);

/* Caches information about bones and constraints. Must be called if bones or constraints, or weighted path attachments
 * are added or removed. */
void spSkeleton_updateCache (spSkeleton* self);
void spSkeleton_updateWorldTransform (const spSkeleton* self);

/* Sets the bones, constraints, and slots to their setup pose values. */
void spSkeleton_setToSetupPose (const spSkeleton* self);
/* Sets the bones and constraints to their setup pose values. */
void spSkeleton_setBonesToSetupPose (const spSkeleton* self);
void spSkeleton_setSlotsToSetupPose (const spSkeleton* self);

/* Returns 0 if the bone was not found. */
spBone* spSkeleton_findBone (const spSkeleton* self, const char* boneName);
/* Returns -1 if the bone was not found. */
int spSkeleton_findBoneIndex (const spSkeleton* self, const char* boneName);

/* Returns 0 if the slot was not found. */
spSlot* spSkeleton_findSlot (const spSkeleton* self, const char* slotName);
/* Returns -1 if the slot was not found. */
int spSkeleton_findSlotIndex (const spSkeleton* self, const char* slotName);

/* Sets the skin used to look up attachments before looking in the SkeletonData defaultSkin. Attachments from the new skin are
 * attached if the corresponding attachment from the old skin was attached. If there was no old skin, each slot's setup mode
 * attachment is attached from the new skin.
 * @param skin May be 0.*/
void spSkeleton_setSkin (spSkeleton* self, spSkin* skin);
/* Returns 0 if the skin was not found. See spSkeleton_setSkin.
 * @param skinName May be 0. */
int spSkeleton_setSkinByName (spSkeleton* self, const char* skinName);

/* Returns 0 if the slot or attachment was not found. */
spAttachment* spSkeleton_getAttachmentForSlotName (const spSkeleton* self, const char* slotName, const char* attachmentName);
/* Returns 0 if the slot or attachment was not found. */
spAttachment* spSkeleton_getAttachmentForSlotIndex (const spSkeleton* self, int slotIndex, const char* attachmentName);
/* Returns 0 if the slot or attachment was not found.
 * @param attachmentName May be 0. */
int spSkeleton_setAttachment (spSkeleton* self, const char* slotName, const char* attachmentName);

/* Returns 0 if the IK constraint was not found. */
spIkConstraint* spSkeleton_findIkConstraint (const spSkeleton* self, const char* constraintName);

/* Returns 0 if the transform constraint was not found. */
spTransformConstraint* spSkeleton_findTransformConstraint (const spSkeleton* self, const char* constraintName);

/* Returns 0 if the path constraint was not found. */
spPathConstraint* spSkeleton_findPathConstraint (const spSkeleton* self, const char* constraintName);

void spSkeleton_update (spSkeleton* self, float deltaTime);

#ifdef SPINE_SHORT_NAMES
typedef spSkeleton Skeleton;
#define Skeleton_create(...) spSkeleton_create(__VA_ARGS__)
#define Skeleton_dispose(...) spSkeleton_dispose(__VA_ARGS__)
#define Skeleton_updateWorldTransform(...) spSkeleton_updateWorldTransform(__VA_ARGS__)
#define Skeleton_setToSetupPose(...) spSkeleton_setToSetupPose(__VA_ARGS__)
#define Skeleton_setBonesToSetupPose(...) spSkeleton_setBonesToSetupPose(__VA_ARGS__)
#define Skeleton_setSlotsToSetupPose(...) spSkeleton_setSlotsToSetupPose(__VA_ARGS__)
#define Skeleton_findBone(...) spSkeleton_findBone(__VA_ARGS__)
#define Skeleton_findBoneIndex(...) spSkeleton_findBoneIndex(__VA_ARGS__)
#define Skeleton_findSlot(...) spSkeleton_findSlot(__VA_ARGS__)
#define Skeleton_findSlotIndex(...) spSkeleton_findSlotIndex(__VA_ARGS__)
#define Skeleton_setSkin(...) spSkeleton_setSkin(__VA_ARGS__)
#define Skeleton_setSkinByName(...) spSkeleton_setSkinByName(__VA_ARGS__)
#define Skeleton_getAttachmentForSlotName(...) spSkeleton_getAttachmentForSlotName(__VA_ARGS__)
#define Skeleton_getAttachmentForSlotIndex(...) spSkeleton_getAttachmentForSlotIndex(__VA_ARGS__)
#define Skeleton_setAttachment(...) spSkeleton_setAttachment(__VA_ARGS__)
#define Skeleton_update(...) spSkeleton_update(__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* SPINE_SKELETON_H_*/
