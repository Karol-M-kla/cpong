#include "logic.h"
#include <SDL3/SDL.h>
#include <omp.h>

void initGame(gameState *currentGameState) {
  currentGameState->ball.velocityX = 0.0001;
  currentGameState->ball.velocityY = -0.0001;
  currentGameState->ball.ballRect.h = currentGameState->ball.ballRect.w =
      BALL_SIZE;
  currentGameState->ball.ballRect.x = WINDOW_WIDTH / 2.;
  currentGameState->ball.ballRect.y = WINDOW_HEIGHT / 2.;
  currentGameState->players[0].velocityY = 0.00001;
  currentGameState->players[1].velocityY = -0.0001;

  currentGameState->players[0].paddle.x = (WINDOW_WIDTH / 2.) - 300;
  currentGameState->players[1].paddle.x =
      (WINDOW_WIDTH / 2.) + 300 - PADDLE_WIDTH;
  // fprintf(stdout, "%f\n", WINDOW_WIDTH / 2.);
  // fprintf(stdout, "Left: %f\tRight: %f\n", player[0].paddle.x,
  //         player[1].paddle.x);
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
      currentGameState->players[i].paddle.y +=
          currentGameState->players[i].velocityY;
    }

    currentGameState->ball.ballRect.x += currentGameState->ball.velocityX;
    currentGameState->ball.ballRect.y += currentGameState->ball.velocityY;
  }
}
