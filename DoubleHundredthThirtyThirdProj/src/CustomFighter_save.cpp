#include "CustomFighter_save.h"
#include "CustomFighter_button.h"
#include "CustomFighter_video.h"
#include "CustomFighter_input_interpreter.h"
#include "CustomFighter_audio.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <json.hpp>
#include <curl.h>

using std::vector, std::string;

SaveManager g::save = SaveManager();

size_t write_callbackz(char *ptr, size_t size, size_t nmemb, void *userdata) {
    string* write_data = (string*)userdata; 
    *write_data += string(ptr, nmemb);
    return nmemb;
}

SaveManager::SaveManager() {
	std::srand(std::time(0));
	
	for(auto& entry : std::filesystem::directory_iterator("data/clothing"))
		getClothing(entry);

	for(auto& entry : std::filesystem::directory_iterator("data/moves"))
		getAnimation(entry);

	for(auto& entry : std::filesystem::directory_iterator("data/sounds"))
		getSound(entry);

	for(auto& entry : std::filesystem::directory_iterator("data/musics"))
		getMusic(entry);

	for(auto& entry : std::filesystem::directory_iterator("data/fonts"))
		getFont(entry);

	for(auto& entry : std::filesystem::directory_iterator("data/stages"))
		getStage(entry);

	for(auto& entry : std::filesystem::directory_iterator("data/effects"))
		getEffect(entry);

	for(auto& entry : std::filesystem::directory_iterator("data/shaders")) 
		getShader(entry);

	for(int i = 0; i < maxButtonConfigs; i ++)
		getButtonConfig(i);

	for(int i = 0; i < maxPlayerConfigs; i ++)
		loadPlayerConfig(i);

	loadServerList();
	loadVideoConfig();

	// Get network address
	port = 50000 + std::rand() % 1000;

    CURL* curl = curl_easy_init();
    if(curl) {
        string write_data = "";
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.ipify.org/");	

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, false);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callbackz);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_data);

        if(curl_easy_perform(curl) == CURLE_OK) {
        	ip = write_data;
        }
        curl_easy_cleanup(curl);
    }
}

SaveManager::~SaveManager() {

	for(auto& it : textures)
		delete it.second;

	for(auto& it : shaders)
		delete it.second;

	for(auto& it : sounds)
		delete it.second;

	for(auto& it : clothes)
		delete it.second;

	for(auto& it : animations)
		delete it.second;

	for(auto& it : fonts)
		delete it.second;	
}

sf::Texture* SaveManager::getTexture(std::filesystem::path path) {
	sf::Texture* ptr = NULL;

	if(textures.find(path.string()) == textures.end()) {
		ptr = new sf::Texture();

		if(ptr->loadFromFile(path.string())) {
			textures[path.string()] = ptr;

		}else {
			delete ptr;
			ptr = NULL;
		}

	}else {
		ptr = textures[path.string()];
	}
	return ptr;
}

AnimatedTexture SaveManager::getEffect(std::filesystem::path path) {

	if(effects.find(path.string()) == effects.end()) {
		AnimatedTexture anim;
		
		if(anim.loadFromFile(path)) {
			anim.id = effectID++;
			effects[path.stem().string()] = anim;
		}
		return anim;
	}
	return effects[path.string()];
}

AnimatedTexture SaveManager::getEffectByID(int id) {

	for(auto& it : effects) {

		if(it.second.id == id)
			return it.second;
	}
	return {};
}

sf::Shader* SaveManager::getShader(std::filesystem::path path) {
	sf::Shader* ptr = NULL;

	if(shaders.find(path.string()) == shaders.end()) {
		ptr = new sf::Shader();

		string ext = path.extension().string();
		int type = -1;

		if(ext == ".fs" || ext == ".frag") {
			type = sf::Shader::Fragment;

		}else if(ext == ".vs" || ext == ".vert") {
			type = sf::Shader::Vertex;
		}

		if(type >= 0 && ptr->loadFromFile(path.string(), (sf::Shader::Type)type)) {
			shaders[path.filename().string()] = ptr;

		}else {
			delete ptr;
			ptr = NULL;
		}

	}else {
		ptr = shaders[path.string()];
	}
	return ptr;
}

sf::SoundBuffer* SaveManager::getSound(std::filesystem::path path) {
	sf::SoundBuffer* ptr = NULL;

	if(!g::audio.isEnabled())
		return ptr;

	if(sounds.find(path.string()) == sounds.end()) {
		ptr = new sf::SoundBuffer();

		if(ptr->loadFromFile(path.string())) {
			sounds[path.stem().string()] = ptr;

		}else {
			delete ptr;
			ptr = NULL;
		}

	}else {
		ptr = sounds[path.string()];
	}
	return ptr;
}

vector<string> SaveManager::getSoundList() {
	vector<string> list;

	for(auto& it : sounds) 
		list.push_back(it.first);

	return list;
}

sf::SoundBuffer* SaveManager::getMusic(std::filesystem::path path) {
	sf::SoundBuffer* ptr = NULL;

	if(!g::audio.isEnabled())
		return ptr;

	if(musics.find(path.string()) == musics.end()) {
		ptr = new sf::SoundBuffer();

		if(ptr->loadFromFile(path.string())) {
			musics[path.stem().string()] = ptr;

		}else {
			delete ptr;
			ptr = NULL;
		}

	}else {
		ptr = musics[path.string()];
	}
	return ptr;
}

vector<string> SaveManager::getMusicList() {
	vector<string> list;

	for(auto& it : musics) 
		list.push_back(it.first);

	return list;
}

sf::Font* SaveManager::getFont(std::filesystem::path path) {
	sf::Font* ptr = NULL;

	if(fonts.find(path.string()) == fonts.end()) {
		ptr = new sf::Font();

		if(ptr->loadFromFile(path.string())) {
			fonts[path.stem().string()] = ptr;

		}else {
			delete ptr;
			ptr = NULL;
		}

	}else {
		ptr = fonts[path.string()];
	}
	return ptr;
}

Clothing* SaveManager::getClothing(std::filesystem::path path) {
	Clothing* ptr = NULL;

	if(clothes.find(path.string()) == clothes.end()) {
		ptr = new Clothing;

		// Ensure something was loaded
		if(ptr->loadFromFile(path)) {
			clothes[path.stem().string()] = ptr;		

		}else{
			delete ptr;
			ptr = NULL;
		}
	
	}else {
		ptr = clothes.at(path.string());
	}
	return ptr;
}

vector<string> SaveManager::getClothingList() {
	vector<string> list;

	for(auto& it : clothes) 
		list.push_back(it.first);
	
	return list;
}

Animation* SaveManager::getAnimation(std::filesystem::path path) {
	Animation* ptr = NULL;

	if(animations.find(path.string()) == animations.end()) {
		ptr = new Animation();

		if(ptr->loadFromFile(path.string())) {
			animations[path.stem().string()] = ptr;	
	
		}else {
			delete ptr;
			ptr = NULL;
		}

	}else {
		ptr = animations.at(path.string());		
	}

	return ptr;	
}

vector<string> SaveManager::getAnimationsList() {
	vector<string> list;

	for(auto& it : animations) 
		list.push_back(it.first);
	
	return list;
}


vector<string> SaveManager::getAnimationsByFilter(vector<int> filters) {
	vector<string> list;

	for(auto& it : animations) {

		if(std::find(filters.begin(), filters.end(), it.second->category) != filters.end())
			list.push_back(it.first);
	}
	return list;
}

void SaveManager::loadButtonConfig(int index) {

	if(index < 0 || index >= maxButtonConfigs)
		return;

	// Default configs
	if(index == 0) {
		buttonConfig[index].index	= KEYBOARD_INDEX;
		buttonConfig[index].Up 		= sf::Keyboard::W;
		buttonConfig[index].Down 	= sf::Keyboard::S;
		buttonConfig[index].Left 	= sf::Keyboard::A;
		buttonConfig[index].Right 	= sf::Keyboard::D;
		buttonConfig[index].A 		= sf::Keyboard::U;
		buttonConfig[index].B 		= sf::Keyboard::J;
		buttonConfig[index].C 		= sf::Keyboard::I;
		buttonConfig[index].D 		= sf::Keyboard::O;
		buttonConfig[index].Taunt 	= sf::Keyboard::Semicolon;	

	}else if(index == 1) {
		buttonConfig[index].index	= KEYBOARD_INDEX;
		buttonConfig[index].Up 		= sf::Keyboard::Up;
		buttonConfig[index].Down 	= sf::Keyboard::Down;
		buttonConfig[index].Left 	= sf::Keyboard::Left;
		buttonConfig[index].Right 	= sf::Keyboard::Right;
		buttonConfig[index].A 		= sf::Keyboard::Numpad4;
		buttonConfig[index].B 		= sf::Keyboard::Numpad1;
		buttonConfig[index].C 		= sf::Keyboard::Numpad5;
		buttonConfig[index].D 		= sf::Keyboard::Numpad6;
		buttonConfig[index].Taunt 	= sf::Keyboard::Enter;	
	}

	buttonConfig[index].loadFromFile("save/input" + std::to_string(index) + ".json");
}

void SaveManager::saveButtonConfig(int index, Button::Config config) {

	if(index < 0 || index >= maxButtonConfigs)
		return;

	buttonConfig[index] = config;
	buttonConfig[index].saveToFile("save/input" + std::to_string(index) + ".json");
}

Button::Config SaveManager::getButtonConfig(int index) {
	
	if(index < 0 || index >= maxButtonConfigs)
		return {};

	if(buttonConfig[index].index == -1)
		loadButtonConfig(index);
	
	return buttonConfig[index];
}

void SaveManager::loadPlayerConfig(int index) {

	if(index < 0 || index >= maxPlayerConfigs)
		return;

	std::string path = "save/player" + std::to_string(index) + ".json";
	playerConfig[index].loadFromFile(path);
}

void SaveManager::savePlayerConfig(int index, Player::Config config) {

	if(index < 0 || index >= maxPlayerConfigs)
		return;

	std::string path = "save/player" + std::to_string(index) + ".json";
	playerConfig[index] = config;
	playerConfig[index].saveToFile(path);
}

Player::Config SaveManager::getPlayerConfig(int index) {	

	if(index < 0 || index >= maxPlayerConfigs)
		return {};

	return playerConfig[index];
}

void SaveManager::loadServerList() {
	std::fstream file("save/serverList", std::fstream::in);

	if(!file.good()) {
		file.close();
		return;
	}

	std::string line;
	do{
		std::getline(file, line);
		serverList.push_back(line);

	}while(!file.eof());

	file.close();
}

string SaveManager::getServer(int index) {

	if(index < 0 || index > serverList.size()-1 || serverList.size() == 0)
		return "";

	return serverList[index];
}

string SaveManager::getNetworkAddress() {
	return ip + ":" + std::to_string(port);
}

int SaveManager::getPort() {
	return port;
}

int SaveManager::getRandomStage() {
	return rand() % stages.size();
}

Stage SaveManager::getStage(std::filesystem::path path) {
	Stage add;

	if(add.loadFromFile(path)) 
		stages.push_back(add);

	return add;
}

Stage SaveManager::getStage(int index) {
	
	if(index < 0 || index > stages.size()-1 || stages.size() == 0)
		return {};

	return stages[index];
}

void SaveManager::loadVideoConfig() {

	// Default video config
	resolution.x = sf::VideoMode::getFullscreenModes()[0].width;
	resolution.y = sf::VideoMode::getFullscreenModes()[0].height;
	displayMode = DisplayMode::Borderless;
	vsync = true;

	// Open file config
	std::fstream file("save/video.json", std::fstream::in);

	if(!file.good()) {
		file.close();
		return;
	}

	nlohmann::json json = nlohmann::json::parse(file);
	file.close();

	if(json["width"].is_number())
		resolution.x = json["width"];

	if(json["height"].is_number())
		resolution.y = json["height"];

	if(json["displayMode"].is_number())
		displayMode = json["displayMode"];

	if(json["vsync"].is_boolean())	
		json["vsync"] = vsync;
}

void SaveManager::saveVideoConfig() {
	std::fstream file("save/video.json", std::fstream::out | std::fstream::trunc);

	if(!file.good()) {
		file.close();
		return;
	}

	nlohmann::json json;
	json["width"] = resolution.x;
	json["height"] = resolution.y;
	json["displayMode"] = displayMode;
	json["vsync"] = vsync;

	file << json;
	file.close();
}