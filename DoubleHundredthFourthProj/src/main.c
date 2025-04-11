/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated July 28, 2023. Replaces all prior versions.
 *
 * Copyright (c) 2013-2023, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software or
 * otherwise create derivative works of the Spine Runtimes (collectively,
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
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THE
 * SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include <spine_sdl_c.h>
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
// #undef main

int main(int argc, char *argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("Error: %s", SDL_GetError());
		return -1;
	}
	SDL_Window *window = SDL_CreateWindow("Spine SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
	if (!window) {
		printf("Error: %s", SDL_GetError());
		return -1;
	}
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		printf("Error: %s", SDL_GetError());
		return -1;
	}

	// spAtlas *atlas = spAtlas_createFromFile("data/spineboy-pma.atlas", renderer);
	spAtlas *atlas = spAtlas_createFromFile("hero/export/hero-pma.atlas", renderer);
	spSkeletonJson *json = spSkeletonJson_create(atlas);
	json->scale = 0.5f;
	// spSkeletonData *skeletonData = spSkeletonJson_readSkeletonDataFile(json, "data/spineboy-pro.json");
	spSkeletonData *skeletonData = spSkeletonJson_readSkeletonDataFile(json, "hero/export/hero-pro.json");
	spAnimationStateData *animationStateData = spAnimationStateData_create(skeletonData);
	animationStateData->defaultMix = 0.2f;
	spSkeletonDrawable *drawable = spSkeletonDrawable_create(skeletonData, animationStateData);
	drawable->usePremultipliedAlpha = -1;
	drawable->skeleton->x = 400;
	drawable->skeleton->y = 500;
	spSkeleton_setToSetupPose(drawable->skeleton);
	spSkeletonDrawable_update(drawable, 0, SP_PHYSICS_UPDATE);
	// spAnimationState_setAnimationByName(drawable->animationState, 0, "portal", 0);
	// spAnimationState_addAnimationByName(drawable->animationState, 0, "run", -1, 0);
	spAnimationState_setAnimationByName(drawable->animationState, 0, "idle",  0);

	int quit = 0;
	uint64_t lastFrameTime = SDL_GetPerformanceCounter();
	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				quit = -1;
				break;
			}else if (event.type == SDL_KEYDOWN) { // 如果事件类型是键盘按下事件，可以检查是哪个键被按下
				// 处理键盘按下事件，例如：e.key.keysym.sym 可以获取按下的键的符号（如SDLK_a表示'a'键）
				if (event.key.keysym.sym == SDLK_ESCAPE) { // 如果按下的是ESC键，则退出程序
					quit = 1;
					break;
				}
				if (event.key.keysym.sym == SDLK_j) { // 如果按下的是ESC键，则退出程序
					spAnimationState_addAnimationByName(drawable->animationState, 0, "jump", 1, 0);
				}
				if (event.key.keysym.sym == SDLK_k) { // 如果按下的是ESC键，则退出程序
					spAnimationState_addAnimationByName(drawable->animationState, 0, "attack", 1, 0);
				}
				if (event.key.keysym.sym == SDLK_l) { // 如果按下的是ESC键，则退出程序
					spAnimationState_addAnimationByName(drawable->animationState, 0, "crouch", 1, 0);
				}
				if (event.key.keysym.sym == SDLK_u) { // 如果按下的是ESC键，则退出程序
					spAnimationState_addAnimationByName(drawable->animationState, 0, "head-turn", 1, 0);
				}
				if (event.key.keysym.sym == SDLK_i) { // 如果按下的是ESC键，则退出程序
					spAnimationState_addAnimationByName(drawable->animationState, 0, "run", 1, 0);
				}
				if (event.key.keysym.sym == SDLK_o) { // 如果按下的是ESC键，则退出程序
					spAnimationState_addAnimationByName(drawable->animationState, 0, "walk", 1, 0);
				}
			} else if (event.type == SDL_MOUSEBUTTONDOWN) { // 如果事件类型是鼠标按钮按下事件，可以检查是哪个按钮被按下
				// 处理鼠标按下事件，例如：e.button.button 可以获取按下的鼠标按钮（如SDL_BUTTON_LEFT表示左键）
			}
		}

		SDL_SetRenderDrawColor(renderer, 94, 93, 96, 255);
		SDL_RenderClear(renderer);

		uint64_t now = SDL_GetPerformanceCounter();
		double deltaTime = (now - lastFrameTime) / (double) SDL_GetPerformanceFrequency();
		lastFrameTime = now;

		spSkeletonDrawable_update(drawable, deltaTime, SP_PHYSICS_UPDATE);
		spSkeletonDrawable_draw(drawable, renderer);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
