#include <SFML_Graphics.hpp>
#include "2dge_state.hpp"

MenuState::MenuState(sf::RenderWindow& window_, AssetManager& asManager_):
State(window_, asManager_)
{
	nextStateIndex = STATE::GAME;
	isPaused = false;
	__DEBUG_EXEC(std::cout << "MenuState(sf::RenderWindow&, AssetManager&)\n");
}
MenuState::~MenuState()
{
	__DEBUG_EXEC(std::cout << "~MenuState()\n");
}
void MenuState::init()
{

}
void MenuState::pause()
{

}
void MenuState::resume()
{

}
void MenuState::processInput()
{

}
void MenuState::update()
{

}
int MenuState::gameLoop()
{
	return 0;
}
void MenuState::render(sf::RenderTarget& target, sf::Time frameTime)
{

}
