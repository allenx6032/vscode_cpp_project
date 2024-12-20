#include "SLO_MenuController.h"
#include "SLO_MenuDataStore.h"
#include "SLO_MenuUseCase.h"
#include "SLO_SDLJoystick.h"
#include "SLO_Application.h"
#include <SDL_events.h>

MenuController::MenuController(MenuUseCase &useCase, const MenuDataStore &dataStore)
	: Controller()
	, useCase_(useCase)
	, dataStore_(dataStore)
{
}

MenuController::~MenuController()
{
}

void MenuController::onIdle(uint32_t tick)
{
	setIdling(false);
	auto joystick = dataStore_.getJoystick(0);

	if (joystick && joystick->isJoystick()) {
		if (joystick->isLeftPressTrigger()) {
			//std::cout << "press left" << std::endl;
			useCase_.moveLeftCursor();
		}
		else if (joystick->isLeftPressing()) {
			//std::cout << "pressing left " << tick << std::endl;
			if (joystick->isLeftRepeat()) {
				setIdling(true);
			}
		}
		else if (joystick->isLeftReleaseTrigger()) {
			//std::cout << "release left" << std::endl;
		}

		if (joystick->isUpPressTrigger()) {
			//std::cout << "press up" << std::endl;
			useCase_.moveUpCursor();
		}
		else if (joystick->isUpPressing()) {
			//std::cout << "pressing up " << tick << std::endl;
			if (joystick->isUpRepeat()) {
				setIdling(true);
			}
		}
		else if (joystick->isUpReleaseTrigger()) {
			//std::cout << "release up" << std::endl;
		}

		if (joystick->isRightPressTrigger()) {
			//std::cout << "press right" << std::endl;
			useCase_.moveRightCursor();
		}
		else if (joystick->isRightPressing()) {
			//std::cout << "pressing right " << tick << std::endl;
			if (joystick->isRightRepeat()) {
				setIdling(true);
			}
		}
		else if (joystick->isRightReleaseTrigger()) {
			//std::cout << "release right" << std::endl;
		}

		if (joystick->isDownPressTrigger()) {
			//std::cout << "press down" << std::endl;
			useCase_.moveDownCursor();
		}
		else if (joystick->isDownPressing()) {
			//std::cout << "pressing down " << tick << std::endl;
			if (joystick->isDownRepeat()) {
				setIdling(true);
			}
		}
		else if (joystick->isDownReleaseTrigger()) {
			//std::cout << "release down" << std::endl;
		}
	}
	useCase_.action(tick);
}

void MenuController::onKeyDown(const SDL_KeyboardEvent &key)
{
	switch(key.keysym.sym)
	{
	case SDLK_ESCAPE:
		useCase_.quit();
		break;

	case SDLK_BACKSPACE:
		useCase_.finish();
		break;

	case SDLK_RETURN:
		useCase_.pushEnter();
		break;

	case SDLK_UP:
		useCase_.moveUpCursor();
		break;

	case SDLK_DOWN:
		useCase_.moveDownCursor();
		break;

	case SDLK_LEFT:
		useCase_.moveLeftCursor();
		break;

	case SDLK_RIGHT:
		useCase_.moveRightCursor();
		break;

	case SDLK_q:
		// menu icon のサイズを小さくします
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addScale(-0.01f);
		}
		else {
			useCase_.addScale(-0.1f);
		}
		break;
	case SDLK_w:
		// menu icon のサイズを大きくします
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addScale(0.01f);
		}
		else {
			useCase_.addScale(0.1f);
		}
		break;
	case SDLK_e:
		// menu circle と camera の距離を広げます
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addZPos(0.1f);
		}
		else {
			useCase_.addZPos(1.0f);
		}
		break;
	case SDLK_r:
		// menu circle と camera の距離を縮めます
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addZPos(-0.1f);
		}
		else {
			useCase_.addZPos(-1.0f);
		}
		break;
	case SDLK_a:
		// menu circle の半径を小さくします
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addRadius(-0.1f);
		}
		else {
			useCase_.addRadius(-1.0f);
		}
		break;
	case SDLK_s:
		// menu circle の半径を大きくします
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addRadius(0.1f);
		}
		else {
			useCase_.addRadius(1.0f);
		}
		break;
	case SDLK_d:
		// menu circle を向こう側へ傾けます
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addDegrees(-0.1f);
		}
		else {
			useCase_.addDegrees(-1.0f);
		}
		break;
	case SDLK_f:
		// menu circle を手前側へ傾けます
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addDegrees(0.1f);
		}
		else {
			useCase_.addDegrees(1.0f);
		}
		break;
	case SDLK_z:
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addViewAngle(-0.1f);
		}
		else {
			useCase_.addViewAngle(-1.0f);
		}
		break;
	case SDLK_x:
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addViewAngle(0.1f);
		}
		else {
			useCase_.addViewAngle(1.0f);
		}
		break;
	case SDLK_c:
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addCameraYEye(-0.1f);
		}
		else {
			useCase_.addCameraYEye(-1.0f);
		}
		break;
	case SDLK_v:
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addCameraYEye(0.1f);
		}
		else {
			useCase_.addCameraYEye(1.0f);
		}
		break;
	case SDLK_t:
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addZOffset(-0.1f);
		}
		else {
			useCase_.addZOffset(-1.0f);
		}
		break;
	case SDLK_y:
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addZOffset(0.1f);
		}
		else {
			useCase_.addZOffset(1.0f);
		}
		break;

	case SDLK_g:
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addCameraYCenter(-0.1f);
		}
		else {
			useCase_.addCameraYCenter(-1.0f);
		}
		break;
	case SDLK_h:
		if (key.keysym.mod == KMOD_LSHIFT || key.keysym.mod == KMOD_RSHIFT) {
			useCase_.addCameraYCenter(0.1f);
		}
		else {
			useCase_.addCameraYCenter(1.0f);
		}
		break;

	case SDLK_SPACE:
		// 今のパラメータを標準出力に表示します
		useCase_.showParam();
		break;

	case SDLK_TAB:
		useCase_.flipBillboard();
		break;

	default:
		break;
	}
}

void MenuController::onWindow(const SDL_WindowEvent &window)
{
	switch(window.event)
	{
	case SDL_WINDOWEVENT_EXPOSED:
		useCase_.exposed();
		break;

	case SDL_WINDOWEVENT_CLOSE:
		useCase_.finish();
		break;

	default:
		break;
	}
}

void MenuController::onJoyHat(const SDL_JoyHatEvent &jhat)
{
//	std::cout << "JoyHat value = " << static_cast<int>(jhat.value) << std::endl;
}

void MenuController::onJoyButtonDown(const SDL_JoyButtonEvent &jbutton)
{
	auto joystick = dataStore_.getJoystick(0);
	if (joystick && joystick->isJoystick()) {
		if (jbutton.button == dataStore_.getEnterButton()) {
			useCase_.pushEnter();
		}
		else if (jbutton.button == dataStore_.getCancelButton()) {
			useCase_.finish();
		}
	}
}

void MenuController::setJoystickOption()
{
	const auto delay = dataStore_.getRepeatDelay();
	const auto interval = dataStore_.getRepeatInterval();
	auto joystick = dataStore_.getJoystick(0);
	if (joystick && joystick->isJoystick()) {
		joystick->setLeftRepeatEnable(delay, interval);
		joystick->setUpRepeatEnable(delay, interval);
		joystick->setRightRepeatEnable(delay, interval);
		joystick->setDownRepeatEnable(delay, interval);
	}
}

void MenuController::clearJoystickOption()
{
	auto joystick = dataStore_.getJoystick(0);
	if (joystick && joystick->isJoystick()) {
		joystick->setLeftRepeatDisable();
		joystick->setUpRepeatDisable();
		joystick->setRightRepeatDisable();
		joystick->setDownRepeatDisable();
	}
}
