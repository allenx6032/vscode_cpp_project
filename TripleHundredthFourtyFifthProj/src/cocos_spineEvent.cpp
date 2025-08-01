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

#include <cocos_spineEvent.h>

#include <cocos_spineEventData.h>

spine::Event::Event(float time, const spine::EventData &data) :
		_data(data),
		_time(time),
		_intValue(0),
		_floatValue(0),
		_stringValue(),
		_volume(1),
		_balance(0) {
}

const spine::EventData &spine::Event::getData() {
	return _data;
}

float spine::Event::getTime() {
	return _time;
}

int spine::Event::getIntValue() {
	return _intValue;
}

void spine::Event::setIntValue(int inValue) {
	_intValue = inValue;
}

float spine::Event::getFloatValue() {
	return _floatValue;
}

void spine::Event::setFloatValue(float inValue) {
	_floatValue = inValue;
}

const spine::String &spine::Event::getStringValue() {
	return _stringValue;
}

void spine::Event::setStringValue(const spine::String &inValue) {
	_stringValue = inValue;
}


float spine::Event::getVolume() {
	return _volume;
}

void spine::Event::setVolume(float inValue) {
	_volume = inValue;
}

float spine::Event::getBalance() {
	return _balance;
}

void spine::Event::setBalance(float inValue) {
	_balance = inValue;
}
