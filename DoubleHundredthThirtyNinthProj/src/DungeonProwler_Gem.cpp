#include "DungeonProwler_PCH.h"
#include "DungeonProwler_Gem.h"

// Default Constructor.
Gem::Gem()
{
	// Set the sprite.
	SetSprite(TextureManager::GetTexture(TextureManager::AddTexture("resources/loot/gem/spr_pickup_gem.png")), false, 8, 12);

	// Set the value of the gem.
	m_scoreValue = std::rand() % 100;

	// Set the item type.
	m_type = ITEM::GEM;
}

// Gets the amount of score this pickup gives.
int Gem::GetScoreValue() const
{
	return m_scoreValue;
}