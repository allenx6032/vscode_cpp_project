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

#ifndef Spine_PathConstraintPositionTimeline_h
#define Spine_PathConstraintPositionTimeline_h

#include <cocos_spineCurveTimeline.h>

namespace spine {

	class SP_API PathConstraintPositionTimeline : public CurveTimeline {
		friend class SkeletonBinary;
		friend class SkeletonJson;

		RTTI_DECL

	public:
		static const int ENTRIES;

		explicit PathConstraintPositionTimeline(int frameCount);

		virtual ~PathConstraintPositionTimeline();

		virtual void apply(Skeleton& skeleton, float lastTime, float time, Vector<Event*>* pEvents, float alpha, MixBlend blend, MixDirection direction);

		virtual int getPropertyId();

		/// Sets the time and value of the specified keyframe.
		void setFrame(int frameIndex, float time, float value);

	protected:
		static const int PREV_TIME;
		static const int PREV_VALUE;
		static const int VALUE;

		Vector<float> _frames;
		int _pathConstraintIndex;
	};
}

#endif /* Spine_PathConstraintPositionTimeline_h */
