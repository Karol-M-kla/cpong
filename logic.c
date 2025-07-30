#include "logic.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <omp.h>
#include <stdio.h>

void ballReset(gameState *currentGameState) {
  // TODO: add random direction

  currentGameState->ball.velocityX = 0.001;
  // currentGameState->ball.velocityY = 0.001;
  BALL_SIZE;
  currentGameState->ball.ballRect.x = WINDOW_WIDTH / 2.;
  currentGameState->ball.ballRect.y = WINDOW_HEIGHT / 2.;
}
void initGame(gameState *currentGameState) {
  currentGameState->ball.ballRect.h = currentGameState->ball.ballRect.w =
      BALL_SIZE;
  ballReset(currentGameState);

  // currentGameState->players[0].velocityY = 0.0;
  // currentGameState->players[1].velocityY = -0.0;

  currentGameState->players[0].paddle.x = (WINDOW_WIDTH / 2.) - 300;
  currentGameState->players[1].paddle.x =
      (WINDOW_WIDTH / 2.) + 300 - PADDLE_WIDTH;
  for (int i = 0; i < 2; i++) {
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

bool checkPaddleCollision(gameState *currentGameState, int player) {
  SDL_FRect ballHitbox = currentGameState->ball.ballRect;
  SDL_FRect playerPaddleHitbox = currentGameState->players[player].paddle;
  if ((ballHitbox.x < playerPaddleHitbox.x + PADDLE_WIDTH) &&
      (ballHitbox.x + BALL_SIZE > playerPaddleHitbox.x) &&
      (ballHitbox.y < playerPaddleHitbox.y + PADDLE_HEIGHT) &&
      (ballHitbox.y + BALL_SIZE > playerPaddleHitbox.y)) {
    printf("hey %d\n", player);
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
#pragma omp parallel
  {
    if (ballYValue < 0 || ballYValue + BALL_SIZE > WINDOW_HEIGHT) {
      *ballYvelocity = -(*ballYvelocity);
    }

    if (ballXValue < 0 || ballXValue + BALL_SIZE > WINDOW_WIDTH) {
      *ballXvelocity = -(*ballXvelocity);
    }
#pragma omp parallel for
    for (int i = 0; i < 2; i++) {
      if (checkPaddleCollision(currentGameState, i)) {
        *ballXvelocity = -(*ballXvelocity);
      }
    }
  }
  fprintf(stdout, "X: %f\tY: %f\tvelX: %f\tvelY: %f\n", ballXValue, ballYValue,
          *ballXvelocity, *ballYvelocity);
}
