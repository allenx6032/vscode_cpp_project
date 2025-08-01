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

#ifndef Spine_TransformConstraint_h
#define Spine_TransformConstraint_h

#include <cocos_spineConstraintData.h>

#include <cocos_spineVector.h>

namespace spine {
	class TransformConstraintData;
	class Skeleton;
	class Bone;

	class SP_API TransformConstraint : public Updatable {
		friend class Skeleton;
		friend class TransformConstraintTimeline;

		RTTI_DECL

	public:
		TransformConstraint(TransformConstraintData& data, Skeleton& skeleton);

		void apply();

		virtual void update();

		virtual int getOrder();

		TransformConstraintData& getData();

		Vector<Bone*>& getBones();

		Bone* getTarget();
		void setTarget(Bone* inValue);

		float getRotateMix();
		void setRotateMix(float inValue);

		float getTranslateMix();
		void setTranslateMix(float inValue);

		float getScaleMix();
		void setScaleMix(float inValue);

		float getShearMix();
		void setShearMix(float inValue);

		bool isActive();

		void setActive(bool inValue);

	private:
		TransformConstraintData& _data;
		Vector<Bone*> _bones;
		Bone* _target;
		float _rotateMix, _translateMix, _scaleMix, _shearMix;
		bool _active;

		void applyAbsoluteWorld();

		void applyRelativeWorld();

		void applyAbsoluteLocal();

		void applyRelativeLocal();
	};
}

#endif /* Spine_TransformConstraint_h */
