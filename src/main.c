#include "base.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>
#define WINHEIGHT 1000
#define WINWIDTH 1000
#define CELLHEIGHT 10
#define CELLWIDTH 10

#define YCELLAMOUNT (WINHEIGHT / CELLHEIGHT)
#define XCELLAMOUNT (WINWIDTH / CELLWIDTH)

struct state_t;
void init(arena_t *arena);
void move_cell_states();
bool determine_state(bool, bool, bool);
void execute_rule();
void tick();

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;

  SDL_Rect *cells[YCELLAMOUNT][XCELLAMOUNT];
  bool cell_states[YCELLAMOUNT][XCELLAMOUNT];
} state_t;

state_t *state;

void init(arena_t *arena) {
  state = (state_t *)arena_push(arena, sizeof(state_t));
  SDL_Init(SDL_INIT_EVENTS);
  SDL_version compiled_version, linked_version;
  SDL_VERSION(&compiled_version);
  SDL_GetVersion(&linked_version);

  state->window = SDL_CreateWindow("rule 30", SDL_WINDOWPOS_CENTERED_DISPLAY(1),
                                   SDL_WINDOWPOS_CENTERED_DISPLAY(1), 1000,
                                   1000, SDL_WINDOW_SHOWN);

  state->renderer =
      SDL_CreateRenderer(state->window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RenderClear(state->renderer);

  SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  for (int y = 0; y < YCELLAMOUNT; ++y) {
    for (int x = 0; x < XCELLAMOUNT; ++x) {
      state->cells[y][x] = (SDL_Rect *)arena_push(arena, sizeof(SDL_Rect));
      state->cells[y][x]->x = x * CELLWIDTH;
      state->cells[y][x]->y = y * CELLHEIGHT;
      state->cells[y][x]->w = CELLWIDTH;
      state->cells[y][x]->h = CELLHEIGHT;
      SDL_RenderFillRect(state->renderer, state->cells[y][x]);
      state->cell_states[y][x] = false;
    }
  }
}

void move_cell_states() {
  for (int y = YCELLAMOUNT - 1; y > 0; --y) {
    for (int x = 0; x < XCELLAMOUNT; ++x) {
      state->cell_states[y][x] = state->cell_states[y - 1][x];
    }
  }
}

bool determine_state(bool left, bool center, bool right) {
  if (left && center && right) {
    return false;
  } else if (left && center && !right) {
    return false;
  } else if (left && !center && right) {
    return false;
  } else if (left && !center && !right) {
    return true;
  } else if (!left && center && right) {
    return true;
  } else if (!left && center && !right) {
    return true;
  } else if (!left && !center && right) {
    return true;
  } else {
    return false;
  }
}

void execute_rule() {
  bool left;
  bool center;
  bool right;

  // Left most cell wrap around
  left = state->cell_states[1][XCELLAMOUNT - 1];
  center = state->cell_states[1][0];
  right = state->cell_states[1][1];
  state->cell_states[0][0] = determine_state(left, center, right);

  // Right most cell wrap around
  left = state->cell_states[1][XCELLAMOUNT - 2];
  center = state->cell_states[1][XCELLAMOUNT - 1];
  right = state->cell_states[1][0];
  state->cell_states[0][XCELLAMOUNT - 1] = determine_state(left, center, right);

  for (int x = 1; x < XCELLAMOUNT - 1; x++) {
    left = state->cell_states[1][x - 1];
    center = state->cell_states[1][x];
    right = state->cell_states[1][x + 1];

    state->cell_states[0][x] = determine_state(left, center, right);
  }

  for (int y = 0; y < YCELLAMOUNT; y++) {
    for (int x = 0; x < XCELLAMOUNT; x++) {
      if (state->cell_states[y][x]) {
        SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      } else {
        SDL_SetRenderDrawColor(state->renderer, 255, 255, 255,
                               SDL_ALPHA_OPAQUE);
      }
      SDL_RenderFillRect(state->renderer, state->cells[y][x]);
    }
  }
}

void tick() {
  move_cell_states();
  execute_rule();
  SDL_RenderPresent(state->renderer);
}

int main() {
  arena_t *mainArena = arena_create();
  if (!mainArena) {
    printf("Failed to initialize memory arena\n");
    return 1; // Exit if the arena couldn't be created
  }
  init(mainArena);
  SDL_RenderPresent(state->renderer);
  bool running = 1;
  bool pause = 0;
  state->cell_states[0][XCELLAMOUNT / 2] = true;

  while (running) {
    if (!pause) {
      tick();
    }

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = 0;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_SPACE:
          pause = !pause;
          break;
        case SDLK_n:
          if (pause) {
            tick();
          }
        case SDLK_ESCAPE:
          running = 0;
          break;
        }
        break;
      }
    }
  }

  arena_release(mainArena);
}
