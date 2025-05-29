#ifndef INVENTORYBASEDCONTROLS_H
#define INVENTORYBASEDCONTROLS_H

#include "deceptus_inventory.h"

namespace InventoryBasedControls
{
bool isSwordButtonPressed(const Inventory& inventory, bool x_button_pressed, bool y_button_pressed);
bool isAttackButtonPressed(const Inventory& inventory, bool x_button_pressed, bool y_button_pressed);
};  // namespace InventoryBasedControls

#endif  // INVENTORYBASEDCONTROLS_H
