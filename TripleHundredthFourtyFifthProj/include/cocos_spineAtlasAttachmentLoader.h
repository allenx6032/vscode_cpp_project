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

#ifndef Spine_AtlasAttachmentLoader_h
#define Spine_AtlasAttachmentLoader_h

#include <cocos_spineAttachmentLoader.h>
#include <cocos_spineVector.h>
#include <cocos_spineString.h>


namespace spine {
	class Atlas;
	class AtlasRegion;

	/// An AttachmentLoader that configures attachments using texture regions from an Atlas.
	/// See http://esotericsoftware.com/spine-loading-skeleton-data#JSON-and-binary-data about Loading Skeleton Data in the Spine Runtimes Guide.
	class SP_API AtlasAttachmentLoader : public AttachmentLoader {
	public:
		RTTI_DECL

		explicit AtlasAttachmentLoader(Atlas* atlas);

		virtual RegionAttachment* newRegionAttachment(Skin& skin, const String& name, const String& path);

		virtual MeshAttachment* newMeshAttachment(Skin& skin, const String& name, const String& path);

		virtual BoundingBoxAttachment* newBoundingBoxAttachment(Skin& skin, const String& name);

		virtual PathAttachment* newPathAttachment(Skin& skin, const String& name);

		virtual PointAttachment* newPointAttachment(Skin& skin, const String& name);

		virtual ClippingAttachment* newClippingAttachment(Skin& skin, const String& name);

		virtual void configureAttachment(Attachment* attachment);

		AtlasRegion* findRegion(const String& name);

	private:
		Atlas* _atlas;
	};
}

#endif /* Spine_AtlasAttachmentLoader_h */
