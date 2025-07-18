/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated January 1, 2020. Replaces all prior versions.
 *
 * Copyright (c) 2013-2020, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software
 * or otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THE SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#ifdef SPINE_UE4
#include "SpinePluginPrivatePCH.h"
#endif

#include <cocos_spineAttachmentTimeline.h>

#include <cocos_spineSkeleton.h>
#include <cocos_spineEvent.h>

#include <cocos_spineAnimation.h>
#include <cocos_spineBone.h>
#include <cocos_spineTimelineType.h>
#include <cocos_spineSlot.h>
#include <cocos_spineSlotData.h>

using namespace spine;

RTTI_IMPL(AttachmentTimeline, Timeline)

AttachmentTimeline::AttachmentTimeline(int frameCount) : Timeline(), _slotIndex(0) {
	_frames.ensureCapacity(frameCount);
	_attachmentNames.ensureCapacity(frameCount);

	_frames.setSize(frameCount, 0);

	for (int i = 0; i < frameCount; ++i) {
		_attachmentNames.add(String());
	}
}

void AttachmentTimeline::setAttachment(Skeleton& skeleton, Slot& slot, String* attachmentName) {
    slot.setAttachment(attachmentName == NULL || attachmentName->isEmpty() ? NULL : skeleton.getAttachment(_slotIndex, *attachmentName));
}

void AttachmentTimeline::apply(Skeleton &skeleton, float lastTime, float time, Vector<Event *> *pEvents, float alpha,
	MixBlend blend, MixDirection direction
) {
	SP_UNUSED(lastTime);
	SP_UNUSED(pEvents);
	SP_UNUSED(alpha);

	assert(_slotIndex < skeleton._slots.size());

	String *attachmentName;
	Slot *slotP = skeleton._slots[_slotIndex];
	Slot &slot = *slotP;
	if (!slot._bone.isActive()) return;

	if (direction == MixDirection_Out) {
	    if (blend == MixBlend_Setup) setAttachment(skeleton, slot, &slot._data._attachmentName);
		return;
	}

	if (time < _frames[0]) {
		// Time is before first frame.
		if (blend == MixBlend_Setup || blend == MixBlend_First) {
            setAttachment(skeleton, slot, &slot._data._attachmentName);
		}
		return;
	}

	size_t frameIndex;
	if (time >= _frames[_frames.size() - 1]) {
		// Time is after last frame.
		frameIndex = _frames.size() - 1;
	} else {
		frameIndex = Animation::binarySearch(_frames, time, 1) - 1;
	}

	attachmentName = &_attachmentNames[frameIndex];
	slot.setAttachment(attachmentName->length() == 0 ? NULL : skeleton.getAttachment(_slotIndex, *attachmentName));
}

int AttachmentTimeline::getPropertyId() {
	return ((int) TimelineType_Attachment << 24) + _slotIndex;
}

void AttachmentTimeline::setFrame(int frameIndex, float time, const String &attachmentName) {
	_frames[frameIndex] = time;
	_attachmentNames[frameIndex] = attachmentName;
}

size_t AttachmentTimeline::getSlotIndex() {
	return _slotIndex;
}

void AttachmentTimeline::setSlotIndex(size_t inValue) {
	_slotIndex = inValue;
}

Vector<float> &AttachmentTimeline::getFrames() {
	return _frames;
}

Vector<String> &AttachmentTimeline::getAttachmentNames() {
	return _attachmentNames;
}

size_t AttachmentTimeline::getFrameCount() {
	return _frames.size();
}
