#include "logic.h"
#include "parameters.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
// #include <math.h>
#include <omp.h>
#include <stdio.h>

void ballSpeedIncrement(Ball *gameBall) { gameBall->speed += 0.1; }

void ballReset(Ball *gameBall) {
  // TODO: add random direction
  gameBall->speed = DEFAULT_BALL_SPEED;
  gameBall->velocityX = gameBall->velocityY = gameBall->speed;

  gameBall->ballRect.x = WINDOW_WIDTH / 2.;
  gameBall->ballRect.y = WINDOW_HEIGHT / 2.;
}

void initGame(gameState *currentGameState) {
  Ball *gameBall = &currentGameState->ball;
  gameBall->ballRect.h = gameBall->ballRect.w = BALL_SIZE;
  ballReset(gameBall);

  // currentGameState->players[0].velocityY = 0.0;
  // currentGameState->players[1].velocityY = -0.0;

  currentGameState->players[0].paddle.x = PADDLE_WIDTH;
  currentGameState->players[1].paddle.x = WINDOW_WIDTH - 2 * PADDLE_WIDTH;
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

    currentGameState->ball.ballRect.x += currentGameState->ball.velocityX;
    currentGameState->ball.ballRect.y += currentGameState->ball.velocityY;
  }
}

float computeBounceAngle(float paddleY, float ballY) {
  float paddleCenter = (paddleY + PADDLE_HEIGHT) / 2.0;
  float ballCenter = (ballY + BALL_SIZE) / 2.0;
  float relativeBallBoucePoint = ballCenter - paddleCenter;
  float normalizedRBBP = relativeBallBoucePoint / PADDLE_HEIGHT / 2.0;

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

  float possibleYFactor = 1.0 - (normalizedRBPP * normalizedRBPP);
  // float possibleYFactor = 1.0 - powf(normalizedRBPP, 2.0);
  // float possibleYFactor = 0.5;
  float yFactor = possibleYFactor > 0.0 ? possibleYFactor : 0.0;
  // fprintf(stdout, "y factor: %f\n", yFactor);
  switch (player) {
  case 0:
    // *ballXvelocity = fabs(normalizedRBPP) * BALL_SPEED;
    *ballXvelocity = *ballSpeed;
    *ballYvelocity = *ballYvelocity >= 0.0 ? yFactor : -yFactor;
    *ballYvelocity *= *ballSpeed;
    *ballXValue = playerPaddleXValue + PADDLE_WIDTH + 0.02;
    break;
  case 1:
    // *ballXvelocity = -fabs(normalizedRBPP) * BALL_SPEED;
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
    // printf("hey %d\n", player);
    return true;
  } else {
    return false;
  }
}
void detectCollision(gameState *currentGameState) {
  // ball collision for UP and DOWN
  float ballXValue = currentGameState->ball.ballRect.x;
  float ballYValue = currentGameState->ball.ballRect.y;
  float *ballXvelocity = &currentGameState->ball.velocityX;
  float *ballYvelocity = &currentGameState->ball.velocityY;
  // #pragma omp parallel
  //   {
  if (ballYValue <= 0 || ballYValue + BALL_SIZE >= WINDOW_HEIGHT) {
    *ballYvelocity = -(*ballYvelocity);
  }

  if (ballXValue < 0) {
    currentGameState->players[1].score++;
    ballReset(&currentGameState->ball);
  }
  if (ballXValue + BALL_SIZE > WINDOW_WIDTH) {
    currentGameState->players[0].score++;
    ballReset(&currentGameState->ball);
  }
#pragma omp parallel for
  for (int i = 0; i < 2; i++) {
    if (checkPaddleCollision(currentGameState, i)) {
      // *ballXvelocity = -(*ballXvelocity);
      ballSpeedIncrement(&currentGameState->ball);
      bounceBall(currentGameState, i);
    }
  }
  // }
  // fprintf(stdout, "X: %f\tY: %f\tvelX: %f\tvelY: %f\n", ballXValue,
  // ballYValue,
  //         *ballXvelocity, *ballYvelocity);
}
