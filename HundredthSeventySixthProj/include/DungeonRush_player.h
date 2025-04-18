#ifndef SNAKE__PLAYER_H_
#define SNAKE__PLAYER_H_
#include "DungeonRush_types.h"
#include "DungeonRush_adt.h"

typedef enum {
  LOCAL, REMOTE, COMPUTER
} PlayerType;
typedef struct {
  LinkList* sprites;
  int moveStep, team, num, buffs[BUFF_END];
  Score* score;
  PlayerType playerType; 
} Snake;
void destroySnake(Snake*);
Snake* createSnake(int step, int team, PlayerType playerType);
#endif
