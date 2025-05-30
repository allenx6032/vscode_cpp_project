/*********************************************************
 *
 *
 *     ██████╗██╗     ███████╗ █████╗ ███╗   ██╗
 *    ██╔════╝██║     ██╔════╝██╔══██╗████╗  ██║
 *    ██║     ██║     █████╗  ███████║██╔██╗ ██║
 *    ██║     ██║     ██╔══╝  ██╔══██║██║╚██╗██║
 *    ╚██████╗███████╗███████╗██║  ██║██║ ╚████║
 *     ╚═════╝╚══════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝
 *
 *    Clean and clear all resources used in the game
 *
 *
 *********************************************************/

#include "CRougeLite.h" // NOTE: declare global extern vars
#include "CRougeLite_combatAction.h"
#include "CRougeLite_enemy.h"
#include "CRougeLite_player.h"

// ***************************
// Private Function Prototypes
// ***************************
static void clearDictionary(Dictionary *dict);
static void clearDictionaryItem(Dictionary **dict);

//========================================================
// Init Functions
//========================================================

void clearGameState() {
  int enemy_num = gameState->numOfEnemies;
  Enemy *enemies = gameState->enemies;

  clearPlayers();
  clearEnemies();
  clearCombatActions();

  clearDictionary(gameState->characterDictionary);
  printf("Deleted all directories\n");
  clearDictionary(gameState->enemyDictionary);
  clearDictionary(gameState->playerWeaponDictionary);
  clearDictionary(gameState->enemyWeaponDictionary);
  freeResource((void *)gameState);
}

void freeResource(void *item) {
  if (item == NULL)
    return;
  free(item);
}

// *****************
// PRIVATE FUNCTIONS
// *****************
// FIXME: BROKEN???
static void clearDictionary(Dictionary *dict) {
  if (dict == NULL)
    return;

  while (dict->opcode != -1) {
    clearDictionaryItem(&dict);
    dict++;
  }
  clearDictionaryItem(&dict);
  printf("Deleted Dictionary\n");
}

static void clearDictionaryItem(Dictionary **dict) {
  if (dict == NULL || *dict == NULL)
    return;

  free(*dict);
  *dict = NULL;
}

void clearMap() {
  Map *map = &(gameState->map);
  TilesMapper *tiles_mapper = &(gameState->map.tilesMapper);

  for (int i = 0; i < tiles_mapper->numOfTiles; i++) {
    free(tiles_mapper->mapper[i]);
    tiles_mapper->mapper[i] = NULL;
  }
}
