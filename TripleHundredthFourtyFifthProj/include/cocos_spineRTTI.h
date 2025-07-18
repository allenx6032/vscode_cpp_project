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

#ifndef Spine_RTTI_h
#define Spine_RTTI_h

#include <cocos_spineObject.h>

namespace spine {
class SP_API RTTI : public SpineObject {
public:
	explicit RTTI(const char *className);

	RTTI(const char *className, const RTTI &baseRTTI);

	const char *getClassName() const;

	bool isExactly(const RTTI &rtti) const;

	bool instanceOf(const RTTI &rtti) const;

private:
	// Prevent copying
	RTTI(const RTTI &obj);

	RTTI &operator=(const RTTI &obj);

	const char* _className;
	const RTTI *_pBaseRTTI;
};
}

#define RTTI_DECL \
public: \
static const spine::RTTI rtti; \
virtual const spine::RTTI& getRTTI() const;

#define RTTI_IMPL_NOPARENT(name) \
const spine::RTTI name::rtti(#name); \
const spine::RTTI& name::getRTTI() const { return rtti; }

#define RTTI_IMPL(name, parent) \
const spine::RTTI name::rtti(#name, parent::rtti); \
const spine::RTTI& name::getRTTI() const { return rtti; }

#endif /* Spine_RTTI_h */
