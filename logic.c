#include "logic.h"
#include "parameters.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>

void ballSpeedIncrement(Ball *gameBall) {
  gameBall->speed += BALL_SPEED_CHANGE;
}
float objectCenterY(float y, float height) { return ((2 * y) + height / 2.0); }

void ballReset(Ball *gameBall) {
  gameBall->speed = DEFAULT_BALL_SPEED;
  int randomDirection = SDL_rand(4);
  switch (randomDirection) {
  case 0:
    gameBall->velocityX = gameBall->velocityY = gameBall->speed;
    break;
  case 1:
    gameBall->velocityX = gameBall->velocityY = -gameBall->speed;
    break;
  case 2:
    gameBall->velocityX = -gameBall->speed;
    gameBall->velocityY = gameBall->speed;
    break;
  case 3:
    gameBall->velocityX = -gameBall->speed;
    gameBall->velocityY = gameBall->speed;
    break;
  }

  gameBall->ballRect.x = WINDOW_WIDTH / 2.;
  gameBall->ballRect.y = WINDOW_HEIGHT / 2.;
}

void initGame(gameState *currentGameState) {
  Ball *gameBall = &currentGameState->ball;
  gameBall->ballRect.h = gameBall->ballRect.w = BALL_SIZE;
  ballReset(gameBall);

  currentGameState->players[0].paddle.x = PADDLE_WIDTH;
  currentGameState->players[1].paddle.x = WINDOW_WIDTH - 2 * PADDLE_WIDTH;
#pragma omp parallel for
  for (int i = 0; i < 2; i++) {
    currentGameState->players[i].score = 0;
    currentGameState->players[i].paddle.y = WINDOW_HEIGHT / 2.;
    currentGameState->players[i].paddle.h = PADDLE_HEIGHT;
    currentGameState->players[i].paddle.w = PADDLE_WIDTH;
  }
}

void moveObjects(gameState *currentGameState) {
#pragma omp parallel
  {
    currentGameState->ball.ballRect.x += currentGameState->ball.velocityX;
    currentGameState->ball.ballRect.y += currentGameState->ball.velocityY;

#pragma omp parallel for
    for (int i = 0; i < 2; i++) {
      float *playerPaddleY = &currentGameState->players[i].paddle.y;

      if (*playerPaddleY - PLAYER_VELOCITY_DELTA < 0) {
        *playerPaddleY = 0;
      } else if (*playerPaddleY + PADDLE_HEIGHT + PLAYER_VELOCITY_DELTA >
                 WINDOW_HEIGHT) {
        *playerPaddleY = WINDOW_HEIGHT - PADDLE_HEIGHT;
      }
      *playerPaddleY += currentGameState->players[i].velocityY;
    }
  }
}

float computeBounceAngle(float paddleY, float ballY) {
  float paddleCenterY = objectCenterY(paddleY, PADDLE_HEIGHT);
  float ballCenterY = objectCenterY(ballY, BALL_SIZE);
  float relativeBallBoucePoint = ballCenterY - paddleCenterY;
  float normalizedRBBP = relativeBallBoucePoint / PADDLE_HEIGHT / 2.0;
  // float normalizedRBBP = relativeBallBoucePoint / PADDLE_HEIGHT;

  float angleTreshold = 0.95;

  if (normalizedRBBP < -angleTreshold) {
    return -angleTreshold;
  } else if (normalizedRBBP > angleTreshold) {
    return angleTreshold;
  }
  return normalizedRBBP;
}

void bounceBall(gameState *currentGameState, int player) {

  float *ballXvelocity = &currentGameState->ball.velocityX;
  float *ballYvelocity = &currentGameState->ball.velocityY;
  float *ballXValue = &currentGameState->ball.ballRect.x;
  float *ballSpeed = &currentGameState->ball.speed;
  float playerPaddleXValue = currentGameState->players[player].paddle.x;

  // normalized relative ball bounce point
  float normalizedRBPP =
      computeBounceAngle(currentGameState->players[player].paddle.y,
                         currentGameState->ball.ballRect.y);

  float possibleYFactor = 1.0 - pow(normalizedRBPP, 2);
  float yFactor = possibleYFactor > 0.0 ? possibleYFactor : 0.0;
  switch (player) {
  case 0:
    *ballXvelocity = *ballSpeed;
    *ballYvelocity = *ballYvelocity >= 0.0 ? yFactor : -yFactor;
    *ballYvelocity *= *ballSpeed;
    *ballXValue = playerPaddleXValue + PADDLE_WIDTH + 0.02;
    break;
  case 1:
    *ballXvelocity = -(*ballSpeed);
    *ballYvelocity = *ballYvelocity >= 0.0 ? yFactor : -yFactor;
    *ballYvelocity *= *ballSpeed;
    *ballXValue = playerPaddleXValue - BALL_SIZE - 0.02;
    break;
  default:
    fprintf(stderr, "Wrong player id %d\n", player);
    break;
  }
}

bool checkPaddleCollision(gameState *currentGameState, int player) {
  SDL_FRect ballHitbox = currentGameState->ball.ballRect;
  SDL_FRect playerPaddleHitbox = currentGameState->players[player].paddle;
  if ((ballHitbox.x < playerPaddleHitbox.x + PADDLE_WIDTH) &&
      (ballHitbox.x + BALL_SIZE > playerPaddleHitbox.x) &&
      (ballHitbox.y < playerPaddleHitbox.y + PADDLE_HEIGHT) &&
      (ballHitbox.y + BALL_SIZE > playerPaddleHitbox.y)) {
    return true;
  } else {
    return false;
  }
}
void detectCollision(gameState *currentGameState) {
  float ballXValue = currentGameState->ball.ballRect.x;
  float ballYValue = currentGameState->ball.ballRect.y;
  float ballSpeed = currentGameState->ball.speed;
  float *ballXvelocity = &currentGameState->ball.velocityX;
  float *ballYvelocity = &currentGameState->ball.velocityY;
// #pragma omp parallel
//   {
    // ball collision for UP and DOWN
    if (ballYValue <= 0) {
      *ballYvelocity = ballSpeed;
    }

    if (ballYValue + BALL_SIZE >= WINDOW_HEIGHT) {
      *ballYvelocity = -ballSpeed;
    }

    // ball collision for LEFT and RIGHT
    if (ballXValue <= 0) {
      ballReset(&currentGameState->ball);
      currentGameState->players[1].score++;
    }
    if (ballXValue + BALL_SIZE >= WINDOW_WIDTH) {
      ballReset(&currentGameState->ball);
      currentGameState->players[0].score++;
    }
#pragma omp parallel for
    for (int i = 0; i < 2; i++) {
      if (checkPaddleCollision(currentGameState, i)) {
        ballSpeedIncrement(&currentGameState->ball);
        bounceBall(currentGameState, i);
      }
    }
  // }
}

void botMovement(gameState *currentGameState) {
  float botPaddleCenterY =
      objectCenterY(currentGameState->players[0].paddle.y, PADDLE_HEIGHT);
  float ballCenterY =
      objectCenterY(currentGameState->ball.ballRect.y, BALL_SIZE);

  float direction = botPaddleCenterY - ballCenterY;

  float *botVelocityY = &currentGameState->players[0].velocityY;
  float botSpeedY = PLAYER_VELOCITY_DELTA + (BALL_SPEED_CHANGE / 5.0f);
  if (fabs(direction) > 60.0f) {
    if (direction > 0) {
      *botVelocityY = -botSpeedY;
    } else {
      *botVelocityY = botSpeedY;
    }
  } else {
    *botVelocityY = 0;
  }
}
