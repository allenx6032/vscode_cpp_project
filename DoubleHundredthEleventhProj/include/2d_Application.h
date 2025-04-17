#pragma once

#include <vector>
#include "2d_Mouse.h"
#include "2d_Renderer.h"
#include "2d_Cloth.h"

struct Application 
{
private:
	Renderer* renderer = nullptr;
	Mouse* mouse = nullptr;
	Cloth* cloth = nullptr;

	bool isRunning = false;

	Uint32 lastUpdateTime;
public:
	Application() = default;
	~Application() = default;
	
	bool IsRunning() const;

	void Setup(int clothWidth, int clothHeight, int clothSpacing);
	void Input();
	void Update();
	void Render() const;
	void Destroy();
};