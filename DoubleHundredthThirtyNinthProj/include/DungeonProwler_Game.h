//-------------------------------------------------------------------------------------
// Game.h
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Written by Dale Green. Copyright (c) Packt Publishing. All rights reserved.
//-------------------------------------------------------------------------------------
#ifndef GAME_H
#define GAME_H

#include "DungeonProwler_Player.h"
#include "DungeonProwler_Item.h"
#include "DungeonProwler_Level.h"
#include "DungeonProwler_Gold.h"
#include "DungeonProwler_Key.h"
#include "DungeonProwler_Gem.h"
#include "DungeonProwler_Potion.h"
#include "DungeonProwler_Heart.h"
#include "DungeonProwler_Slime.h"
#include "DungeonProwler_Humanoid.h"

static float const FPS = 60.0;						// Constant for fixed time - step loop. We'll lock it at 60fps.
static float const MS_PER_STEP = 1.0f / FPS;		// Roughly (0.017) @ 60fps.

static int const MAX_ITEM_SPAWN_COUNT = 50;
static int const MAX_ENEMY_SPAWN_COUNT = 30;
static int const MAX_FLOOR_ALT_COUNT = 80;

static int const AMBIENT_SOUNDS_COUNT = 3;
static float const GAME_OVER_TEXT_SHIFT = 50.f;


class Game
{
public:
	/**
	 * Constructor.
	 * @param window A pointer to the main render window.
	 */
	Game(sf::RenderWindow* window);

	/**
	 * Initializes the game object by initializing all objects the main game uses.
	 * @return True if the game was initialized successfully.
	 */
	void Initialize();

	/**
	 * The main game loop. This loop in turn updates the game, and draws all objects to screen.
	 * It's also responsible for the game logic timing.
	 */
	void Run();

	/**
	 * Returns true if the game is currently running.
	 * @return True if the game is running.
	 */
	bool IsRunning();

	/**
	 * The main update loop. This loop in turns calls the update loops of all game objects.
	 * @param timeDelta The time, in MS, since the last update call.
	 */
	void Update(float timeDelta);

	/**
	 * Draws all game objects to screen.
	 * @param tileDelta The time, in MS, since the last draw call.
	 */
	void Draw(float timeDelta);

private:

	/**
	 * Populates the current game room with items and enemies.
	 */
	void PopulateLevel();

	/**
	 * Loads all sprites needed for the UI.
	 */
	void LoadUI();

	/**
	 * Calculates the distance between two points
	 * @param position1 The position of the first point.
	 * @param position2 The position of the second item.
	 * @return The distance between the two points.
	 */
	float DistanceBetweenPoints(sf::Vector2f position1, sf::Vector2f position2);

	/**
	 * Draws text at a given location on the screen.
	 * @param text The string you wish to draw.
	 * @param position The top-left position of the string.
	 * @param size (Optional) The font-size to use. Default value is 10.
	 */
	void DrawString(std::string text, sf::Vector2f position, unsigned int size = 10);

	/**
	 * Constructs the grid of sprites that are used to draw the game light system.
	 */
	void ConstructLightGrid();

	/**
	 * Updates the level light.
	 * @param playerPosition The position of the players within the level.
	 */
	void UpdateLight(sf::Vector2f playerPosition);

	/**
	 * Updates all items in the level.
	 * @param playerPosition The position of the players within the level.
	 */
	void UpdateItems(sf::Vector2f playerPosition);

	/**
	 * Updates all enemies in the level.
	 * @param playerPosition The position of the players within the level.
	 * @param timeDelta The amount of time that has passed since the last update.
	 * @param level Current level of the game.
	 */
	void UpdateEnemies(sf::Vector2f playerPosition, float timeDelta, Level &level);

	/**
	 * Updates all projectiles in the level.
	 * @param timeDelta The amount of time that has passed since the last update.
	 */
	void UpdateProjectiles(float timeDelta);

    /**
     * Spawns a given item in the level.
     * @param itemType Item what need to generated on the level.
     * @param position The position of the item within the level.
     */
	void SpawnItem(ITEM itemType, sf::Vector2f position = { -1.f, -1.f });

    /**
     * Spawns a given enemy in the level.
     * @param enemyType Enemy that should be generated on the level.
     * @param position The position of the enemy within the level.
     */
    void SpawnEnemy(ENEMY enemyType, sf::Vector2f position = { -1.f, -1.f });

    /**
     * Spawns a given number of a certain tile at random locations in the level.
     * @param tileType Tile type that should be used for generating on the level.
     * @param count The count of generated procedurally tiles.
     */
    void SpawnRandomTiles(TILE tileType, int count);

    /**
    * Plays the given sound effect, with randomized parameters./
    */
    void PlaySound(sf::Sound& sound, sf::Vector2f position = { 0.f, 0.f });

    /**
     * Generates a level goal.
     */
    void GenerateLevelGoal();

	/**
     * Generates a game level.
     */
	void GenerateLevel();
private:
	/**
	 * The main application window.
	 */
	sf::RenderWindow& m_window;

	/**
	 * An array of the different views the game needs.
	 */
	sf::View m_views[static_cast<int>(VIEW::COUNT)];

	/**
	 * Used in the main game time step.
	 */
	sf::Clock m_timestepClock;

	/**
	 * The default font to be used when drawing text.
	 */
	sf::Font m_font;

	/**
	 * The game state.
	 */
	GAME_STATE m_gameState;

	/**
	 * A vector that holds all items within the level.
	 */
	std::vector<std::unique_ptr<Item>> m_items;

	/**
	 * A vector that holds all the enemies within the level.
	 */
	std::vector<std::unique_ptr<Enemy>> m_enemies;

	/**
	 * A bool that tracks the running state of the game. It's used in the main loop.
	 */
	bool m_isRunning;

	/**
	 * The main level object. All data and functionally regarding the level lives in this class/object.
	 */
	Level m_level;

	/**
	 * The main player object. Only one instance of this object should be created at any one time.
	 */
	Player m_player;

	/**
	 * String stream used by the DrawText() function.
	 */
	std::ostringstream m_stringStream;

	/**
	 * String used by the DrawText() function.
	 */
	std::string m_string;

	/**
	 * Text used by the DrawText() function.
	 */
	sf::Text m_text;

	/**
	 * A vector containing all sprites that make up the lighting grid.
	 */
	std::vector<sf::Sprite> m_lightGrid;

	/**
	 * The size of the screen and window.
	 */
	sf::Vector2u m_screenSize;

	/**
	* The center of the screen.
	*/
	sf::Vector2f m_screenCenter;

	/**
	 * The current game score.
	 */
	int m_scoreTotal;

	/**
	* The amount of gold that the player currently has.
	*/
	int m_goldTotal;

	/**
	 * The sprite that shows the player class in the UI.
	 */
	std::shared_ptr<sf::Sprite> m_playerUiSprite;

	/**
	 * The sprite used to show how many coins the player has.
	 */
	std::shared_ptr<sf::Sprite> m_coinUiSprite;

	/**
	* The sprite used to show how much score the player has.
	*/
	std::shared_ptr<sf::Sprite> m_gemUiSprite;

	/**
	 * Key ui sprite.
	 */
	std::shared_ptr<sf::Sprite> m_keyUiSprite;

	/**
	 * The sprite for the players attack stat.
	 */
	std::shared_ptr<sf::Sprite> m_attackStatSprite;

	/**
	 * The texture IDs for the attack stat textures.
	 */
	int m_attackStatTextureIDs[2];

	/**
	* The sprite for the players defense stat.
	*/
	std::shared_ptr<sf::Sprite> m_defenseStatSprite;

	/**
	* The texture IDs for the defense stat textures.
	*/
	int m_defenseStatTextureIDs[2];

	/**
	* The sprite for the players strength stat.
	*/
	std::shared_ptr<sf::Sprite> m_strengthStatSprite;

	/**
	* The texture IDs for the strength stat textures.
	*/
	int m_strengthStatTextureIDs[2];

	/**
	* The sprite for the players dexterity stat.
	*/
	std::shared_ptr<sf::Sprite> m_dexterityStatSprite;

	/**
	* The texture IDs for the dexterity stat textures.
	*/
	int m_dexterityStatTextureIDs[2];

	/**
	* The sprite for the players stamina stat.
	*/
	std::shared_ptr<sf::Sprite> m_staminaStatSprite;

	/**
	* The texture IDs for the stamina stat textures.
	*/
	int m_staminaStatTextureIDs[2];

	/**
	 * A vector of all the player's projectiles.
	 */
	std::vector<std::unique_ptr<Projectile>> m_playerProjectiles;

	/**
	 * The ID of the player's projectile texture.
	 */
	int m_projectileTextureID;

	/**
	 * A boolean denoting if a new level was generated.
	 */
	bool m_levelWasGenerated;

	/**
	 * Sprite for the health bar.
	 */
	std::shared_ptr<sf::Sprite> m_healthBarSprite;

	/**
	* Sprite for the health bar outline.
	*/
	std::shared_ptr<sf::Sprite> m_healthBarOutlineSprite;

	/**
	 * Sprite for the mana bar.
	 */
	std::shared_ptr<sf::Sprite> m_manaBarSprite;

	/**
	* Sprite for the mana bar outline.
	*/
	std::shared_ptr<sf::Sprite> m_manaBarOutlineSprite;

	/**
	 * A vector of all ui sprites.
	 */
	std::vector<std::shared_ptr<sf::Sprite>> m_uiSprites;

	/**
	 * The main music.
	 */
	sf::Music m_music;

    /**
     * Torch sound.
     */
    sf::Sound m_fireSound;

    /**
     * Gem pickup sound.
     */
    sf::Sound m_gemPickupSound;

    /**
     * Coin pickup sound.
     */
    sf::Sound m_coinPickupSound;

    /**
    * Key collect sound.
    */
    sf::Sound m_keyPickupSound;

    /**
     * Enemy die sound.
     */
    sf::Sound m_enemyDieSound;

    /**
     * Player hit sound.
     */
    sf::Sound m_playerHitSound;

    /**
     * Ambient sound effects.
     */
    std::vector<std::shared_ptr<sf::Sound>> m_ambientSounds;

	/**
    * The last tile that the player was on.
    */
	Tile* m_playerPreviousTile;

    /**
    * The value of gold remaining for the current goal.
    */
    int m_goldGoal;

    /**
     * The value of gems remaining for the current goal.
     */
    int m_gemGoal;

    /**
     * The number of kills remaining for the current goal.
     */
    int m_killGoal;

    /**
     * A string describing the current level goal.
     */
    sf::String m_goalString;

    /**
     * A boolean denoting if a goal is currently active.
     */
    bool m_activeGoal;
};
#endif