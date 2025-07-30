#ifndef LOGIC_H
#define LOGIC_H

#include <SDL3/SDL.h>
#include <stdint.h>
#include "parameters.h"

typedef struct {
  int8_t score;
  bool isRightPlayer;
  SDL_FRect paddle;
  float velocityY;
} Player;

typedef struct {
	float velocityX;
	float velocityY;
	SDL_FRect ballRect;
} Ball;

typedef struct {
	Player players[2];
	Ball ball;
} gameState;

void initGame(gameState *currentGameState);
void moveObjects(gameState *currentGameState);
void detectCollision(gameState *currentGameState);

#endif // LOGIC_H
