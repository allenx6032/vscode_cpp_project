#include "DragonBones_FrameEvent.h"
#include "DragonBones_Armature.h"
#include "DragonBones_Bone.h"
#include "DragonBones_AnimationState.h"

namespace DragonBones {

	const std::string FrameEvent::MOVEMENT_FRAME_EVENT = "animationFrameEvent";
	const std::string FrameEvent::ANIMATION_FRAME_EVENT = "animationFrameEvent";
	const std::string FrameEvent::BONE_FRAME_EVENT = "boneFrameEvent";


	Armature* FrameEvent::getArmature() {
		return (Armature*) target;
	}

	FrameEvent::FrameEvent(std::string type) : Event(type){
	}

	Event* FrameEvent::clone() {
		FrameEvent* rEvent = new FrameEvent(type);
		rEvent->animationState = animationState;
		rEvent->bone = bone;
		rEvent->frameLabel = frameLabel;
		return rEvent;
	}
}