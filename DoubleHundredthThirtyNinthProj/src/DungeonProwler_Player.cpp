#include <algorithm>
#include "DungeonProwler_PCH.h"
#include "DungeonProwler_Player.h"

// Constructor.
Player::Player() :
m_attackDelta(0.f),
m_damageDelta(0.f),
m_manaDelta(0.f),
m_isAttacking(false),
m_canTakeDamage(true)
{
    // Generate a random class.
    m_class = static_cast<PLAYER_CLASS>(std::rand() % static_cast<int>(PLAYER_CLASS::COUNT));

    std::string className;
    // Set class-specific variables.
    switch (m_class)
    {
        case PLAYER_CLASS::WARRIOR:
            m_strength += std::rand() % 6 + 5;
            className = "warrior";
            break;
        case PLAYER_CLASS::MAGE:
            m_defense = std::rand() % 6 + 5;
            className = "mage";
            break;
        case PLAYER_CLASS::ARCHER:
            m_dexterity = std::rand() % 6 + 5;
            className = "archer";
            break;
        case PLAYER_CLASS::THIEF:
            m_stamina = std::rand() % 6 + 5;
            className = "thief";
            break;
        default:
            m_strength += std::rand() % 6 + 5;
            className = "warrior";
            break;
    }

    // Load textures.
    m_textureIDs[static_cast<int>(ANIMATION_STATE::WALK_UP)] = TextureManager::AddTexture("resources/players/" + className + "/spr_" + className + "_walk_up.png");
    m_textureIDs[static_cast<int>(ANIMATION_STATE::WALK_DOWN)] = TextureManager::AddTexture("resources/players/" + className + "/spr_" + className + "_walk_down.png");
    m_textureIDs[static_cast<int>(ANIMATION_STATE::WALK_RIGHT)] = TextureManager::AddTexture("resources/players/" + className + "/spr_" + className + "_walk_right.png");
    m_textureIDs[static_cast<int>(ANIMATION_STATE::WALK_LEFT)] = TextureManager::AddTexture("resources/players/" + className + "/spr_" + className + "_walk_left.png");
    m_textureIDs[static_cast<int>(ANIMATION_STATE::IDLE_UP)] = TextureManager::AddTexture("resources/players/" + className + "/spr_" + className + "_idle_up.png");
    m_textureIDs[static_cast<int>(ANIMATION_STATE::IDLE_DOWN)] = TextureManager::AddTexture("resources/players/" + className + "/spr_" + className + "_idle_down.png");
    m_textureIDs[static_cast<int>(ANIMATION_STATE::IDLE_RIGHT)] = TextureManager::AddTexture("resources/players/" + className + "/spr_" + className + "_idle_right.png");
    m_textureIDs[static_cast<int>(ANIMATION_STATE::IDLE_LEFT)] = TextureManager::AddTexture("resources/players/" + className + "/spr_" + className + "_idle_left.png");

    // Set initial sprite.
    SetSprite(TextureManager::GetTexture(m_textureIDs[static_cast<int>(ANIMATION_STATE::WALK_UP)]), false, 8, 12);
    m_currentTextureIndex = static_cast<int>(ANIMATION_STATE::WALK_UP);
    m_sprite.setOrigin(sf::Vector2f(13.f, 18.f));

    // Create the player's aim sprite.
    int textureID = TextureManager::AddTexture("resources/ui/spr_aim.png");
    m_aimSprite.setTexture(TextureManager::GetTexture(textureID));
    m_aimSprite.setOrigin(sf::Vector2f(16.5f, 16.5f));
    m_aimSprite.setScale(2.f, 2.f);

    // Set stats.
    m_health = m_maxHealth = 100;
    m_mana = m_maxMana = 50;
    m_speed = 200;

    // Generate players stats randomly.
    m_statPoints = 50;
    float attackBias = std::rand() % 101;
    float defenseBias = std::rand() % 101;
    float strengthBias = std::rand() % 101;
    float dexterityBias = std::rand() % 101;
    float staminaBias = std::rand() % 101;

    float total = attackBias + defenseBias + strengthBias + dexterityBias
                  + staminaBias;

    m_attack += m_statPoints * (attackBias / total);
    m_defense += m_statPoints * (defenseBias / total);
    m_strength += m_statPoints * (strengthBias / total);
    m_dexterity += m_statPoints * (dexterityBias / total);
    m_stamina += m_statPoints * (staminaBias / total);

    // Set random traits.
    SetRandomTraits();
}

// Updates the player object.
void Player::Update(float timeDelta, Level& level)
{
    // Calculate movement speed based on the timeDelta since the last update.
    sf::Vector2f movementSpeed(0.f, 0.f);
    sf::Vector2f previousPosition = m_position;

    // Calculate where the current movement will put us.
    ANIMATION_STATE animState = static_cast<ANIMATION_STATE>(m_currentTextureIndex);

    if (Input::IsKeyPressed(Input::KEY::KEY_LEFT))
    {
        // Set movement speed.
        movementSpeed.x = -m_speed * timeDelta;

        // Chose animation state.
        animState = ANIMATION_STATE::WALK_LEFT;
    }
    else if (Input::IsKeyPressed(Input::KEY::KEY_RIGHT))
    {
        // Set movement speed.
        movementSpeed.x = m_speed * timeDelta;

        // Chose animation state.
        animState = ANIMATION_STATE::WALK_RIGHT;
    }

    if (Input::IsKeyPressed(Input::KEY::KEY_UP))
    {
        // Set movement speed.
        movementSpeed.y = -m_speed * timeDelta;

        // Chose animation state.
        animState = ANIMATION_STATE::WALK_UP;
    }
    else if (Input::IsKeyPressed(Input::KEY::KEY_DOWN))
    {
        // Set movement speed.
        movementSpeed.y = m_speed * timeDelta;

        // Chose animation state.
        animState = ANIMATION_STATE::WALK_DOWN;
    }

    // Calculate horizontal movement.
    if (CausesCollision(sf::Vector2f(movementSpeed.x, 0.0f), level))
    {
        m_position.x = previousPosition.x;
    }
    else
    {
        m_position.x += movementSpeed.x;
    }

    // Calculate horizontal movement.
    if (CausesCollision(sf::Vector2f(0.0f, movementSpeed.y), level))
    {
        m_position.y = previousPosition.y;
    }
    else
    {
        m_position.y += movementSpeed.y;
    }

    // update the sprite position
    m_sprite.setPosition(m_position);

    // Set the sprite.
    if (m_currentTextureIndex != static_cast<int>(animState))
    {
        m_currentTextureIndex = static_cast<int>(animState);
        m_sprite.setTexture(TextureManager::GetTexture(m_textureIDs[m_currentTextureIndex]));
    }

    // set animation speed
    if ((movementSpeed.x == 0) && (movementSpeed.y == 0))
    {
        // the character is still
        if (IsAnimated())
        {
            // Update sprite to idle version.
            // In our enum we have 4 walking sprites followed by 4 idle sprites.
            // Given this, we can simply add 4 to a walking sprite to get its idle counterpart.
            m_currentTextureIndex += 4;
            m_sprite.setTexture(TextureManager::GetTexture(m_textureIDs[m_currentTextureIndex]));

            // Stop movement animations.
            SetAnimated(false);
        }
    }
    else
    {
        // the character is moving
        if (!IsAnimated())
        {
            // Update sprite to walking version.
            m_currentTextureIndex -= 4;
            m_sprite.setTexture(TextureManager::GetTexture(m_textureIDs[m_currentTextureIndex]));

            // Start movement animations.
            SetAnimated(true);
        }
    }

    // Calculate aim based on mouse.
    sf::Vector2i mousePos = sf::Mouse::getPosition();
    m_aimSprite.setPosition((float)mousePos.x, (float)mousePos.y);

    // Check if shooting.
    if ((m_attackDelta += timeDelta) > 0.25f)
    {
        if (Input::IsKeyPressed(Input::KEY::KEY_ATTACK))
        {
            // Mark player as attacking.
            m_isAttacking = true;
        }
    }

    // Determine if the player can take damage.
    if (!m_canTakeDamage)
    {
        if ((m_damageDelta += timeDelta) > 1.f)
        {
            m_canTakeDamage = true;
            m_damageDelta = 0.f;
        }
    }

    // Increase player mana.
    if ((m_manaDelta += timeDelta) > 0.20)
    {
        if (m_mana < m_maxMana)
        {
            m_mana += 1;
        }

        m_manaDelta = 0.f;
    }
}

// Returns a reference to the player's aim sprite.
sf::Sprite& Player::GetAimSprite()
{
    return m_aimSprite;
}

// Checks if the player is attacking.
bool Player::IsAttacking()
{
    if (m_isAttacking)
    {
        m_isAttacking = false;
        m_attackDelta = 0.f;
        return true;
    }
    else
    {
        return false;
    }
}

// Checks if the player can take damage.
bool Player::CanTakeDamage()
{
    return m_canTakeDamage;
}

// Apply the given amount of damage to the player.
void Player::Damage(int damage)
{
    int applied_damage = static_cast<int>(damage - (PLAYER_TAKEN_DAMAGE_REDUCTION_SCALE * m_defense));
    m_health -= applied_damage > 0 ? applied_damage : 0;

    if (m_health < 0)
    {
        m_health = 0;
    }

    m_canTakeDamage = false;
}

// Gets the player's mana.
int Player::GetMana() const
{
    return m_mana;
}

// Gets the player's max mana.
int Player::GetMaxMana() const
{
    return m_maxMana;
}

// Set the player's mana level.
void Player::SetMana(int manaValue)
{
    if (manaValue >= 0)
    {
        m_mana = manaValue;
    }
}

// Set the player's health.
void Player::SetHealth(int healthValue)
{
    m_health = healthValue;

    if (m_health > m_maxHealth)
    {
        m_health = m_maxHealth;
    }
}


// Returns the player's class.
PLAYER_CLASS Player::GetClass() const
{
    return m_class;
}


// Chooses random traits for the character.
void Player::SetRandomTraits()
{
    // Local vector that storing random indices for PLAYER_TRAIT enum.
    std::vector<int> traitIndex(PLAYER_TRAIT_COUNT, -1);
    for (int i = 0; i < PLAYER_TRAIT_COUNT; ++i)
    {
        int index(0);
        while (true)
        {
            index = std::rand() % static_cast<int>(PLAYER_TRAIT::COUNT);
            if (std::find(traitIndex.begin(), traitIndex.end(), index) == traitIndex.end()) break;
        }

        traitIndex[i] = index;
    }

    // Generate the traits.
    for (int i = 0; i < PLAYER_TRAIT_COUNT; ++i)
    {
        m_traits[i] = static_cast<PLAYER_TRAIT>(traitIndex[i]);
    }

    // Action the traits.
    for (PLAYER_TRAIT trait : m_traits)
    {
        switch (trait)
        {
            case PLAYER_TRAIT::ATTACK:
                m_attack += std::rand() % 6 + 5;
                break;
            case PLAYER_TRAIT::DEFENSE:
                m_defense += std::rand() % 6 + 5;
                break;
            case PLAYER_TRAIT::STRENGTH:
                m_strength += std::rand() % 6 + 5;
                break;
            case PLAYER_TRAIT::DEXTERITY:
                m_dexterity += std::rand() % 6 + 5;
                break;
            case PLAYER_TRAIT::STAMINA:
                m_stamina += std::rand() % 6 + 5;
                break;
            default:
                m_defense += std::rand() % 6 + 5;
                break;
        }
    }
}


// Returns the number of traits the player has.
int Player::GetTraitCount()
{
    return PLAYER_TRAIT_COUNT;
}


// Gets the players current traits.
PLAYER_TRAIT* Player::GetTraits()
{
    return &m_traits[0];
}


// Calculate an amount of damage to an enemy.
int Player::CalculateDamage()
{
    float damage_scale(1.f);
    damage_scale += PLAYER_DEXTERITY_DAMAGE_SCALE * m_dexterity;
    float damage = (std::rand() % (PLAYER_MAX_DAMAGE + 1)) + (PLAYER_ATTACK_DAMAGE_SCALE * m_attack);
    return static_cast<int>(damage * damage_scale);
}
