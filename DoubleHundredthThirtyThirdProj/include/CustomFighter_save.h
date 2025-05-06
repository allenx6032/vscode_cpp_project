#ifndef GAME_SAVE_MANAGER_H
#define GAME_SAVE_MANAGER_H

#include <map>
#include <string>
#include <filesystem>
#include <SFML_Audio.hpp>
#include <SFML_Graphics.hpp>

#include "CustomFighter_stage.h"
#include "CustomFighter_animated_texture.h"
#include "CustomFighter_clothing.h"
#include "CustomFighter_animation.h"
#include "CustomFighter_button.h"
#include "CustomFighter_player.h"

struct SaveManager {

public:
	SaveManager();
	~SaveManager();


/* Texture Assets */
public:
	sf::Texture* 								getTexture(std::filesystem::path path);
	AnimatedTexture								getEffect(std::filesystem::path path);
	AnimatedTexture								getEffectByID(int id);

private:
	std::map<std::string, sf::Texture*> 		textures;
	std::map<std::string, AnimatedTexture> 		effects;
	int 										effectID = 0;

/* Shader Assets */
public:
	sf::Shader*									getShader(std::filesystem::path path);

private:
	std::map<std::string, sf::Shader*> 			shaders;


/* Audio Assets */
public:
	sf::SoundBuffer* 							getSound(std::filesystem::path path);
	std::vector<std::string> 					getSoundList();
	sf::SoundBuffer*  							getMusic(std::filesystem::path path);
	std::vector<std::string> 					getMusicList();

private:
	std::map<std::string, sf::SoundBuffer*> 	sounds;
	std::map<std::string, sf::SoundBuffer*> 	musics;


/* Clothing Assets */
public:
	Clothing* 									getClothing(std::filesystem::path path);
	std::vector<std::string> 					getClothingList();

private:
	std::map<std::string, Clothing*> 			clothes;


/* Animation Assets */
public:
	Animation* 									getAnimation(std::filesystem::path path);
	std::vector<std::string> 					getAnimationsByFilter(std::vector<int> filters);
	std::vector<std::string> 					getAnimationsList();

private:
	std::map<std::string, Animation*> 			animations;


/* Font Assets */
public:
	sf::Font* 									getFont(std::filesystem::path path);

private:
	std::map<std::string, sf::Font*> 			fonts;


/* Stages */
public:
	int 										getRandomStage();
	Stage 										getStage(std::filesystem::path path);
	Stage 										getStage(int index);

private:
	std::vector<Stage> 							stages;


/* Button Configs */
public:
	Button::Config 								getButtonConfig(int index);
	void 										loadButtonConfig(int index);
	void 										saveButtonConfig(int index, Button::Config config);	

private:
	static const int 							maxButtonConfigs = 4;
	Button::Config 								buttonConfig[maxButtonConfigs];


/* Player Configs */
public:
	static const int 							maxPlayerConfigs = 30;	
	void 										loadPlayerConfig(int index);
	void 										savePlayerConfig(int index, Player::Config config);
	Player::Config 								getPlayerConfig(int index);

private:
	Player::Config 								playerConfig[maxPlayerConfigs];


/* Networking Configs */
public:
	void 										loadServerList();
	std::string 								getServer(int index);
	std::string 								getNetworkAddress();
	int 										getPort();

private:
	std::vector<std::string> 					serverList;
	std::string 								ip;
	int 										port;

/* Video Config */
public:
	Vector2 resolution;
	int displayMode;
	bool vsync;	

	void loadVideoConfig();
	void saveVideoConfig();
};

namespace g {
	extern SaveManager save;
};

#endif