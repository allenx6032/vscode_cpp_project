/*********************************************************
 *
 *
 *    ██╗  ██╗███████╗ █████╗ ██████╗ ███████╗██████╗
 *    ██║  ██║██╔════╝██╔══██╗██╔══██╗██╔════╝██╔══██╗
 *    ███████║█████╗  ███████║██║  ██║█████╗  ██████╔╝
 *    ██╔══██║██╔══╝  ██╔══██║██║  ██║██╔══╝  ██╔══██╗
 *    ██║  ██║███████╗██║  ██║██████╔╝███████╗██║  ██║
 *    ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚═╝  ╚═╝
 *
 *    Main header file for includes and shared resources.
 *
 *********************************************************/

#ifndef CROUGELITE_H
#define CROUGELITE_H

#include "CRougeLite_common.h"

//========================================================
// Global Shared Variables
//========================================================
extern Music music;
extern GameState *gameState;

//========================================================
// Global Functions
//========================================================
// Init Functions
GameState *initGameState();
void initDictionary();
void initSettings();

// Clear Resources Functions

void clearGameState();
void freeResource(void *item);
void clearMap();

#endif // CROUGELITE_H

/*********************************************************
 *
 *
 *      ███████╗ ██████╗ ███████╗
 *      ██╔════╝██╔═══██╗██╔════╝
 *      █████╗  ██║   ██║█████╗
 *      ██╔══╝  ██║   ██║██╔══╝
 *      ███████╗╚██████╔╝██║
 *      ╚══════╝ ╚═════╝ ╚═╝
 *
 *
 *********************************************************/
