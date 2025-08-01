#ifndef LOGIC_H
#define LOGIC_H

#include "parameters.h"
#include <SDL3/SDL.h>
#include <stdint.h>

typedef struct {
  int score;
  bool isRightPlayer;
  SDL_FRect paddle;
  float velocityY;
} Player;

typedef struct {
  float velocityX;
  float velocityY;
  float speed;
  SDL_FRect ballRect;
} Ball;

typedef struct {
  Player players[2];
  Ball ball;
} gameState;

void ballSpeedIncrement(Ball *gameBall);
float objectCenterY(float y, float height);
void ballReset(Ball *gameBall);
void initGame(gameState *currentGameState);

void moveObjects(gameState *currentGameState);

float computeBounceAngle(float paddleY, float ballY);
void bounceBall(gameState *currentGameState, int player);
bool checkPaddleCollision(gameState *currentGameState, int player);
void detectCollision(gameState *currentGameState);

void botMovement(gameState *currentGameState);

#endif // LOGIC_H
