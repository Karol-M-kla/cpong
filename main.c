
/*
 * This example code creates an SDL window and renderer, and then clears the
 * window to a different color every frame, so you'll effectively get a window
 * that's smoothly fading between colors.
 *
 * This code is public domain. Feel free to use it for any purpose!
 */

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_timer.h>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include "logic.h"
#include "parameters.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <stdio.h>

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static gameState currentGameState;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_SetAppMetadata("cpong", "1.0", "com.cpong");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("cpong", WINDOW_WIDTH, WINDOW_HEIGHT, 0,
                                   &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  // Enable VSync
  // if (SDL_SetRenderVSync(renderer, 1) == false) {
  //   SDL_Log("Could not enable VSync! SDL error: %s\n", SDL_GetError());
  //   return SDL_APP_FAILURE;
  // }
  initGame(&currentGameState);

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {

  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.key == SDLK_J || event->key.key == SDLK_DOWN) {
      currentGameState.players[1].velocityY = PLAYER_VELOCITY_DELTA;
    }
    if (event->key.key == SDLK_K || event->key.key == SDLK_UP) {
      currentGameState.players[1].velocityY = -PLAYER_VELOCITY_DELTA;
    }

    // // right player
    // if (event->key.key == SDLK_J || event->key.key == SDLK_DOWN) {
    //   currentGameState.players[0].velocityY = PLAYER_VELOCITY_DELTA;
    // }
    // if (event->key.key == SDLK_K || event->key.key == SDLK_UP) {
    //   currentGameState.players[0].velocityY = -PLAYER_VELOCITY_DELTA;
    // }
  }
  if (event->type == SDL_EVENT_KEY_UP) {
    if (event->key.key == SDLK_J || event->key.key == SDLK_DOWN ||
        event->key.key == SDLK_K || event->key.key == SDLK_UP) {
      currentGameState.players[1].velocityY = 0;
      // currentGameState.players[0].velocityY = 0;
    }
  }

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS.
                             */
  }

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
  SDL_SetRenderDrawColorFloat(
      renderer, 0, 0, 0, SDL_ALPHA_OPAQUE_FLOAT); /* new color, full alpha. */

  /* clear the window to the draw color. */
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColorFloat(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE_FLOAT);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

  for (int i = 0; i < 2; i++) {
    SDL_RenderFillRect(renderer, &currentGameState.players[i].paddle);
  }
  SDL_RenderFillRect(renderer, &currentGameState.ball.ballRect);

  SDL_SetRenderScale(renderer, 4.0f, 4.0f);
  // SDL_RenderDebugText(renderer, 5, 5,
  // "HAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
  SDL_RenderDebugTextFormat(renderer, 140.f, 10.f,
                            "%i : %i", currentGameState.players[0].score,
                            currentGameState.players[1].score);

  SDL_SetRenderScale(renderer, 1.0f, 1.0f);
  /* put the newly-cleared rendering on the screen. */
  SDL_RenderPresent(renderer);

  botMovement(&currentGameState);
  moveObjects(&currentGameState);
  detectCollision(&currentGameState);
  SDL_Delay(1000 / 60);
  return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  /* SDL will clean up the window/renderer for us. */
}
