/***************************************************************
 *
 *
 *    
 *    ██╗    ██╗███████╗ █████╗ ██████╗  ██████╗ ███╗   ██╗
 *    ██║    ██║██╔════╝██╔══██╗██╔══██╗██╔═══██╗████╗  ██║
 *    ██║ █╗ ██║█████╗  ███████║██████╔╝██║   ██║██╔██╗ ██║
 *    ██║███╗██║██╔══╝  ██╔══██║██╔═══╝ ██║   ██║██║╚██╗██║
 *    ╚███╔███╔╝███████╗██║  ██║██║     ╚██████╔╝██║ ╚████║
 *     ╚══╝╚══╝ ╚══════╝╚═╝  ╚═╝╚═╝      ╚═════╝ ╚═╝  ╚═══╝
 *
 *     Weapon Module Header. (Game Object)
 *     Exposes the logic for the weapon object.
 *
 *     - Load Weapon
 *     - Use Weapon
 *     - Draw Weapon
 *
 **************************************************************/

#ifndef WEAPON_H
#define WEAPON_H

#include "CRougeLite.h" // NOTE: declare global extern vars
#include "CRougeLite_combatAction.h"

Weapon initWeapon(int opcode, bool isPlayer);
Inventory initInventory();
void updateRangedWeapon(Weapon *weapon, bool isFired, int ID, Vector2 src, Vector2 dest, float deltaTime);
void updateMeleeWeapon(Weapon *weapon, bool isFired, int ID, Vector2 src, Vector2 dest, float deltaTime);
void drawWeapon();
void clearInventory(Inventory *inventory);

#endif // INPUT_H
