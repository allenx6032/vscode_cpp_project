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

#include <spine-AttachmentLoader.h>
#include <stdio.h>
#include <spine-extension.h>

typedef struct _spAttachmentLoaderVtable {
	spAttachment* (*newAttachment) (spAttachmentLoader* self, spSkin* skin, spAttachmentType type, const char* name,
			const char* path);
	void (*dispose) (spAttachmentLoader* self);
} _spAttachmentLoaderVtable;

void _spAttachmentLoader_init (spAttachmentLoader* self, /**/
void (*dispose) (spAttachmentLoader* self), /**/
		spAttachment* (*newAttachment) (spAttachmentLoader* self, spSkin* skin, spAttachmentType type, const char* name,
				const char* path)) {
	CONST_CAST(_spAttachmentLoaderVtable*, self->vtable) = NEW(_spAttachmentLoaderVtable);
	VTABLE(spAttachmentLoader, self)->dispose = dispose;
	VTABLE(spAttachmentLoader, self)->newAttachment = newAttachment;
}

void _spAttachmentLoader_deinit (spAttachmentLoader* self) {
	FREE(self->vtable);
	FREE(self->error1);
	FREE(self->error2);
}

void spAttachmentLoader_dispose (spAttachmentLoader* self) {
	VTABLE(spAttachmentLoader, self)->dispose(self);
	FREE(self);
}

spAttachment* spAttachmentLoader_newAttachment (spAttachmentLoader* self, spSkin* skin, spAttachmentType type, const char* name,
		const char* path) {
	FREE(self->error1);
	FREE(self->error2);
	self->error1 = 0;
	self->error2 = 0;
	return VTABLE(spAttachmentLoader, self)->newAttachment(self, skin, type, name, path);
}

void _spAttachmentLoader_setError (spAttachmentLoader* self, const char* error1, const char* error2) {
	FREE(self->error1);
	FREE(self->error2);
	MALLOC_STR(self->error1, error1);
	MALLOC_STR(self->error2, error2);
}

void _spAttachmentLoader_setUnknownTypeError (spAttachmentLoader* self, spAttachmentType type) {
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%d", type);
	_spAttachmentLoader_setError(self, "Unknown attachment type: ", buffer);
}
