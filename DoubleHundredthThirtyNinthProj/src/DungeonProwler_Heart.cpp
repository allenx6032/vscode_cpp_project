#include "DungeonProwler_PCH.h"
#include "DungeonProwler_Heart.h"

// Default constructor.
Heart::Heart()
{
	// Set item sprite.
	SetSprite(TextureManager::GetTexture(TextureManager::AddTexture("resources/loot/heart/spr_pickup_heart.png")), false, 8, 12);

	// Set health value.
	m_health = std::rand() % 11 + 10;

	// Set item type.
	m_type = ITEM::HEART;
}

// Returns the amount of health that the heart gives.
int Heart::GetHealth() const
{
	return m_health;
}