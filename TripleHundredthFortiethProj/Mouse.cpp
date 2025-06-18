#include "Mouse.h"

#pragma warning (disable: 4244)

bool Mouse::MouseMoveArea(int x1, int y1, int x2, int y2)
{
	//check that the given coords are top left and bottom right
	if ((x1 > x2) || (y1 > y2))
		return false;

	int validX, validY, finalX, finalY;
	validX = x2 - x1;
	validY = y2 - y1;

	//choose random coordinates within the box
	finalX = RandomInt(x1, validX + x1);// rand() % validX + x1;
	finalY = RandomInt(y1, validY + y1); //rand() % validY + y1;

	if (MouseMove(finalX, finalY))
		return true;
	return false;
}

bool Mouse::MouseMoveArea(Area region)
{
	return MouseMoveArea(region.x1, region.y1, region.x2, region.y2);
}

//Deviates PATH Randomly!! mouse move - recieves pixel coordinates. change mouse position to those coords: DEVIATION CHOOSING
bool Mouse::MouseMove(int x, int y)
{
	POINT curMouse;
	float stepX, stepY, interimX, interimY, progress;
	bool getMouse = GetCursorPos(&curMouse);
	if (!getMouse)
		return false;
	int deviatePathY = RandomInt(deviation * (-1), deviation); //rand() % (deviation*2) - deviation; //deviate -250 to 250 (max 250 pixels from path) or specified deviation
	int deviatePathx = RandomInt(deviation * (-1), deviation); //deviate -250 to 250 (max 250 pixels from path) or specified deviation

	int stepSize = RandomInt(70, 220);
	float speed = MouseSpeed * stepSize;
	stepSize = static_cast<int>(speed) * 2;

	stepX = (x - curMouse.x) / static_cast<float>(stepSize);
	stepY = (y - curMouse.y) / static_cast<float>(stepSize);

	INPUT Input = { 0 };
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;

	for (int i = 1; i <= stepSize; i++)
	{
		progress = (i / (float)(stepSize)) * M_PI;
		interimX = (curMouse.x + (i * stepX)) + (deviatePathx * sin(progress));
		interimY = (curMouse.y + (i * stepY)) + (deviatePathY * sin(progress));
		//printf("itterim: %.0f, %.0f\n", round(interimX), round(interimY));

		Input.mi.dx = SENDINPUTX(round(interimX));
		Input.mi.dy = SENDINPUTY(round(interimY));
		::SendInput(1, &Input, sizeof(INPUT));

		//SetCursorPos(round(interimX), round(interimY));
		//Sleep(1);
	}
	//LOG("Moved mouse to:", x, y, "with speed of", stepSize);

	//SetCursorPos(x, y);
	return true;
}

//Deviates PATH Randomly!! mouse move - recieves pixel coordinates. change mouse position to those coords: DEVIATION CHOOSING
bool Mouse::MouseMove(POINT coord)
{
	MouseMove(coord.x, coord.y);
	return true;
}

//STRAIGHT LINE DISTANCE mouse move - recieves pixel coordinates. change mouse position to those coords
bool Mouse::MouseMoveSLD(int x, int y)
{
	POINT curMouse;
	float stepX, stepY, interimX, interimY, progress;
	bool getMouse = GetCursorPos(&curMouse);
	if (!getMouse)
		return false;

	//change below to alter the speed of the movement
	int stepSize = RandomInt(70, 240) * 2; //rand() % 170 + 70; //generate random number between 70 and 240
	stepX = (x - curMouse.x) / static_cast<float>(stepSize);
	stepY = (y - curMouse.y) / static_cast<float>(stepSize);

	INPUT Input = { 0 };
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;

	for (int i = 1; i <= stepSize; i++)
	{
		progress = i / stepSize;
		interimX = (curMouse.x + (i * stepX));
		interimY = (curMouse.y + (i * stepY));

		Input.mi.dx = SENDINPUTX(round(interimX));
		Input.mi.dy = SENDINPUTY(round(interimY));
		::SendInput(1, &Input, sizeof(INPUT));

		//SetCursorPos(round(interimX), round(interimY));
		//Sleep(1);
	}
	//LOG("Moved mouse to:", x, y, "with speed of", stepSize);

	//SetCursorPos(x, y);
	return true;
}

bool Mouse::MouseMoveInstant(int x, int y)
{
	POINT curMouse;
	bool getMouse = GetCursorPos(&curMouse);
	if (!getMouse)
		return false;

	INPUT Input = { 0 };
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
	Input.mi.dx = SENDINPUTX(x);
	Input.mi.dy = SENDINPUTY(y);
	::SendInput(1, &Input, sizeof(INPUT));
	return true;
}

//STRAIGHT LINE DISTANCE mouse move - recieves pixel coordinates. change mouse position to those coords
bool Mouse::MouseMoveSLD(POINT coord)
{
	MouseMoveSLD(coord.x, coord.y);
	return true;
}

bool Mouse::MouseLeftDown()
{
	INPUT    Input = { 0 };
	// left down
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	return true;
	//LOG("LMB down");
}

bool Mouse::MouseLeftUp()
{
	INPUT    Input = { 0 };
	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));

	return true;
}

bool Mouse::MouseRightDown()
{
	INPUT    Input = { 0 };
	// left down
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	return true;
}

bool Mouse::MouseRightUp()
{
	INPUT    Input = { 0 };
	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	::SendInput(1, &Input, sizeof(INPUT));

	return true;
}

bool Mouse::MouseMiddleDown()
{
	INPUT    Input = { 0 };
	// left down
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	return true;
}

bool Mouse::MouseMiddleUp()
{
	INPUT    Input = { 0 };
	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
	::SendInput(1, &Input, sizeof(INPUT));

	return true;
}

bool Mouse::RightClick()
{
	MouseRightDown();
	std::this_thread::sleep_for(std::chrono::milliseconds(RandomInt(13, 15)));
	MouseRightUp();
	return true;
}

bool Mouse::LeftClick()
{
	MouseLeftDown();
	std::this_thread::sleep_for(std::chrono::milliseconds(RandomInt(13, 15)));
	MouseLeftUp();
	return true;
}

bool Mouse::LeftClickHold(int t)
{
	MouseLeftDown();

	//LOG("Holding LMB for", t, "ms");
	std::this_thread::sleep_for(std::chrono::milliseconds(t));

	MouseLeftUp();
	return true;
}

bool Mouse::RightClickWithKey(int vk)
{
	INPUT    Input = { 0 };

	// Press key
	Input.type = INPUT_KEYBOARD;
	Input.ki.dwFlags = KEYEVENTF_SCANCODE;
	Input.ki.wScan = MapVirtualKey(vk, 0);
	::SendInput(1, &Input, sizeof(Input));

	std::this_thread::sleep_for(std::chrono::milliseconds(VERYFAST_REACTION_TIME));

	RightClick();

	std::this_thread::sleep_for(std::chrono::milliseconds(VERYFAST_REACTION_TIME));

	// Release key
	Input.type = INPUT_KEYBOARD;
	Input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
	Input.ki.wScan = MapVirtualKey(vk, 0);
	::SendInput(1, &Input, sizeof(Input));

	return true;
}

bool Mouse::LeftClickKWithKey(int vk)
{
	INPUT    Input = { 0 };

	// Press key
	Input.type = INPUT_KEYBOARD;
	Input.ki.dwFlags = KEYEVENTF_SCANCODE;
	Input.ki.wScan = MapVirtualKey(vk, 0);
	::SendInput(1, &Input, sizeof(Input));

	std::this_thread::sleep_for(std::chrono::milliseconds(VERYFAST_REACTION_TIME));

	LeftClick();

	std::this_thread::sleep_for(std::chrono::milliseconds(VERYFAST_REACTION_TIME));

	// Release key
	Input.type = INPUT_KEYBOARD;
	Input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
	Input.ki.wScan = MapVirtualKey(vk, 0);
	::SendInput(1, &Input, sizeof(Input));

	return true;
}

bool Mouse::Scroll(int ammount)
{
	INPUT    Input = { 0 };
	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_WHEEL;
	Input.mi.mouseData = 120 * ammount;
	::SendInput(1, &Input, sizeof(INPUT));

	return true;
}

POINT Mouse::GetPos() const
{
	POINT curMouse;
	GetCursorPos(&curMouse);
	return curMouse;
}

void Mouse::ChangeSpeed(float speed)
{
	MouseSpeed = speed;
	//LOG("Set mouse speed to", speed);
}

void Mouse::SetDeviation(int dev)
{
	deviation = dev;
	//LOG("Set deviation to", dev);
}

void Mouse::DragAndDrop(int x1, int y1, int x2, int y2, bool SLD)
{
	//LOG("Dragging from", x1, y1);
	if (SLD)
		MouseMoveSLD(x1, y1);
	else
		MouseMove(x1, y1);
	std::this_thread::sleep_for(std::chrono::milliseconds(FAST_REACTION_TIME * 5));
	MouseLeftDown();
	std::this_thread::sleep_for(std::chrono::milliseconds(FAST_REACTION_TIME));
	if (SLD)
		MouseMoveSLD(x2, y2);
	else
		MouseMove(x2, y2);
	std::this_thread::sleep_for(std::chrono::milliseconds(FAST_REACTION_TIME));
	MouseLeftUp();
	//LOG("Dropped on", x2, y2);
}

void Mouse::DragAndDrop(POINT start, POINT end, bool SLD)
{
	DragAndDrop(start.x, start.y, end.x, end.y, SLD);
}

void Mouse::StoreCurrentPos()
{
	POINT current = GetPos();
	PrevX = current.x;
	PrevY = current.y;
	//LOG("Stored mose position", current.x, current.y);
}

void Mouse::ClearStoredPos()
{
	PrevX = 0;
	PrevY = 0;
	//LOG("Cleared stored mouse position");
}

POINT Mouse::GetStoredPos() const
{
	POINT ret = { PrevX, PrevY };
	return ret;
}

double Mouse::GetExecutionTime()
{
	QueryPerformanceCounter(&endCount);

	return(endCount.QuadPart - startCount.QuadPart) * 1000.0 / frequency.QuadPart;
}

Mouse* mouse = new Mouse();