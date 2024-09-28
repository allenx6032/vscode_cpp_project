#include "2dge_game.hpp"

const sf::Time deltaTime = sf::seconds(0.01f);

void map_test()
{
	ObjectManager oManager;
	Map map(oManager);
	map.generate(200, 0.4, 4, 3, 4);
}
void path_test()
{
	ObjectManager oManager;
	Map map(oManager);
	map.setMatrix({
		{0,1,0,1,0,0,0},
		{0,1,0,0,0,0,0},
		{0,1,1,1,0,0,0},
		{0,0,0,1,0,0,0},
		{0,1,1,1,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
				   });
	auto path = map.getPath(sf::Vector2i(0, 0), sf::Vector2i(6, 6));
	for (auto& current: path) {
		std::cout << "(" << current.x << "," << current.y << ")";
	}
}
int main()
{
	//map_test();
	//path_test();
	App game;
	int exitCode = game.run();
	return exitCode;
	return 0;
}