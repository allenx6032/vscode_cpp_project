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

#include <cocos_spine-cocos2dx.h>
#if COCOS2D_VERSION >= 0x00040000

#include <cocos_spineExtension.h>
#include <algorithm>

USING_NS_CC;
#define EVENT_AFTER_DRAW_RESET_POSITION "director_after_draw"
using std::max;
#define INITIAL_SIZE (10000)

#include "cocos_ccShaders.h"
#include "cocos_backend/Device.h"

namespace spine {

static SkeletonBatch* instance = nullptr;

SkeletonBatch* SkeletonBatch::getInstance () {
	if (!instance) instance = new SkeletonBatch();
	return instance;
}

void SkeletonBatch::destroyInstance () {
	if (instance) {
		delete instance;
		instance = nullptr;
	}
}

SkeletonBatch::SkeletonBatch () {

    auto program = backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_TEXTURE_COLOR);
    _programState = std::make_shared<backend::ProgramState>(program);
    
    auto vertexLayout = _programState->getVertexLayout();

	auto locPosition = _programState->getAttributeLocation("a_position");
	auto locTexcoord = _programState->getAttributeLocation("a_texCoord");
	auto locColor = _programState->getAttributeLocation("a_color");
    vertexLayout->setAttribute("a_position", locPosition, backend::VertexFormat::FLOAT3, offsetof(V3F_C4B_T2F, vertices), false);
    vertexLayout->setAttribute("a_color", locColor, backend::VertexFormat::UBYTE4, offsetof(V3F_C4B_T2F, colors), true);
    vertexLayout->setAttribute("a_texCoord", locTexcoord, backend::VertexFormat::FLOAT2, offsetof(V3F_C4B_T2F, texCoords), false);
    vertexLayout->setLayout(sizeof(_vertices[0]));


    _locMVP = _programState->getUniformLocation("u_MVPMatrix");
    _locTexture = _programState->getUniformLocation("u_texture");

    for (unsigned int i = 0; i < INITIAL_SIZE; i++) {
        _commandsPool.push_back(createNewTrianglesCommand());
    }
    reset();
    // callback after drawing is finished so we can clear out the batch state
    // for the next frame
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_AFTER_DRAW_RESET_POSITION, [this](EventCustom* eventCustom) {
        this->update(0);
        });;
}

SkeletonBatch::~SkeletonBatch () {
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_AFTER_DRAW_RESET_POSITION);

	for (unsigned int i = 0; i < _commandsPool.size(); i++) {
        CC_SAFE_RELEASE(_commandsPool[i]->getPipelineDescriptor().programState);
		delete _commandsPool[i];
		_commandsPool[i] = nullptr;
	}
}

void SkeletonBatch::update (float delta) {
	reset();
}

cocos2d::V3F_C4B_T2F* SkeletonBatch::allocateVertices(uint32_t numVertices) {
	if (_vertices.size() - _numVertices < numVertices) {
		cocos2d::V3F_C4B_T2F* oldData = _vertices.data();
		_vertices.resize((_vertices.size() + numVertices) * 2 + 1);
		cocos2d::V3F_C4B_T2F* newData = _vertices.data();
		for (uint32_t i = 0; i < this->_nextFreeCommand; i++) {
			TrianglesCommand* command = _commandsPool[i];
			cocos2d::TrianglesCommand::Triangles& triangles = (cocos2d::TrianglesCommand::Triangles&)command->getTriangles();
			triangles.verts = newData + (triangles.verts - oldData);
		}
	}

	cocos2d::V3F_C4B_T2F* vertices = _vertices.data() + _numVertices;
	_numVertices += numVertices;
	return vertices;
}

void SkeletonBatch::deallocateVertices(uint32_t numVertices) {
	_numVertices -= numVertices;
}


unsigned short* SkeletonBatch::allocateIndices(uint32_t numIndices) {
	if (_indices.getCapacity() - _indices.size() < numIndices) {
		unsigned short* oldData = _indices.buffer();
		int oldSize = _indices.size();
		_indices.ensureCapacity(_indices.size() + numIndices);
		unsigned short* newData = _indices.buffer();
		for (uint32_t i = 0; i < this->_nextFreeCommand; i++) {
			TrianglesCommand* command = _commandsPool[i];
			cocos2d::TrianglesCommand::Triangles& triangles = (cocos2d::TrianglesCommand::Triangles&)command->getTriangles();
			if (triangles.indices >= oldData && triangles.indices < oldData + oldSize) {
				triangles.indices = newData + (triangles.indices - oldData);
			}
		}
	}

	unsigned short* indices = _indices.buffer() + _indices.size();
	_indices.setSize(_indices.size() + numIndices, 0);
	return indices;
}

void SkeletonBatch::deallocateIndices(uint32_t numIndices) {
	_indices.setSize(_indices.size() - numIndices, 0);
}


cocos2d::TrianglesCommand* SkeletonBatch::addCommand(cocos2d::Renderer* renderer, float globalOrder, cocos2d::Texture2D* texture, cocos2d::BlendFunc blendType, const cocos2d::TrianglesCommand::Triangles& triangles, const cocos2d::Mat4& mv, uint32_t flags) {
	TrianglesCommand* command = nextFreeCommand();
    const cocos2d::Mat4& projectionMat = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);    

    auto programState = command->getPipelineDescriptor().programState;
    CCASSERT(programState, "programState should not be null");

    programState->setUniform(_locMVP, projectionMat.m, sizeof(projectionMat.m));
    programState->setTexture(_locTexture, 0, texture->getBackendTexture());

    command->init(globalOrder, texture, blendType, triangles, mv, flags);
    renderer->addCommand(command);
	return command;
}

void SkeletonBatch::reset() {
	_nextFreeCommand = 0;
	_numVertices = 0;
	_indices.setSize(0, 0);
}

cocos2d::TrianglesCommand* SkeletonBatch::nextFreeCommand() {
    if (_commandsPool.size() <= _nextFreeCommand) {
        unsigned int newSize = _commandsPool.size() * 2 + 1;
        for (int i = _commandsPool.size(); i < newSize; i++) {
            _commandsPool.push_back(createNewTrianglesCommand());
        }
    }
    auto* command = _commandsPool[_nextFreeCommand++];
    auto& pipelineDescriptor = command->getPipelineDescriptor();
    if (pipelineDescriptor.programState == nullptr)
    {
        CCASSERT(_programState, "programState should not be null");
        pipelineDescriptor.programState = _programState->clone();
    }
    return command;
}

cocos2d::TrianglesCommand *SkeletonBatch::createNewTrianglesCommand() {
    auto* command = new TrianglesCommand();
    return command;
}
}

#endif
