#include "logic.h"
#include <SDL3/SDL.h>
#include <omp.h>
#include <stdio.h>

void initGame(gameState *currentGameState) {
  currentGameState->ball.velocityX = 0.001;
  currentGameState->ball.velocityY = 0.001;
  currentGameState->ball.ballRect.h = currentGameState->ball.ballRect.w =
      BALL_SIZE;
  currentGameState->ball.ballRect.x = WINDOW_WIDTH / 2.;
  currentGameState->ball.ballRect.y = WINDOW_HEIGHT / 2.;
  currentGameState->players[0].velocityY = 0.01;
  currentGameState->players[1].velocityY = -0.01;

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
      float playerPaddleY = currentGameState->players[i].paddle.y;
      if (!(playerPaddleY < 0) &&
          !(playerPaddleY + PADDLE_HEIGHT > WINDOW_HEIGHT)) {
        currentGameState->players[i].paddle.y +=
            currentGameState->players[i].velocityY;
      }
    }

    currentGameState->ball.ballRect.x += currentGameState->ball.velocityX;
    currentGameState->ball.ballRect.y += currentGameState->ball.velocityY;
  }
}

void detectCollision(gameState *currentGameState) {
  // ball collision for UP and DOWN
  float ballXValue = currentGameState->ball.ballRect.x;
  float ballYValue = currentGameState->ball.ballRect.y;
  float ballXvelocity = currentGameState->ball.velocityX;
  float ballYvelocity = currentGameState->ball.velocityY;
#pragma omp parallel
  {
    if (ballYValue < 0 || ballYValue + BALL_SIZE > WINDOW_HEIGHT) {
      currentGameState->ball.velocityY = -currentGameState->ball.velocityY;
    }

    if (ballXValue < 0 || ballXValue + BALL_SIZE > WINDOW_WIDTH) {
      currentGameState->ball.velocityX = -currentGameState->ball.velocityX;
    }
#pragma omp parallel for
    for (int i = 0; i < 2; i++) {
      // if
    }
  }
  fprintf(stdout, "\n%d %d\n\n", WINDOW_WIDTH, WINDOW_HEIGHT);
  fprintf(stdout, "X: %f\tY: %f\tvelX: %f\tvelY: %f\n", ballXValue, ballYValue,
          ballXvelocity, ballYvelocity);
}
