#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>

#define WINHEIGHT 1000
#define WINWIDTH 1000
#define CELLHEIGHT 10
#define CELLWIDTH 10

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;

  SDL_Rect *cells[(WINHEIGHT / CELLHEIGHT)][(WINWIDTH / CELLHEIGHT)];
} state_t;

state_t *state;

int main() {
  state = malloc(sizeof(state_t));
  SDL_Init(SDL_INIT_EVENTS);
  SDL_version compiled_version, linked_version;
  SDL_VERSION(&compiled_version);
  SDL_GetVersion(&linked_version);

  state->window = SDL_CreateWindow("rule 30", SDL_WINDOWPOS_CENTERED_DISPLAY(1),
                                   SDL_WINDOWPOS_CENTERED_DISPLAY(1), 1000,
                                   1000, SDL_WINDOW_SHOWN);

  state->renderer =
      SDL_CreateRenderer(state->window, -1, SDL_RENDERER_ACCELERATED);
  SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(state->renderer);

  for (int y = 0; y < WINHEIGHT / CELLHEIGHT; ++y) {
    for (int x = 0; x < WINWIDTH / CELLWIDTH; ++x) {
      state->cells[y][x] = malloc(sizeof(SDL_Rect));
      state->cells[y][x]->x = x * CELLWIDTH;
      state->cells[y][x]->y = y * CELLHEIGHT;
      state->cells[y][x]->w = CELLWIDTH;
      state->cells[y][x]->h = CELLHEIGHT;
    }
  }

  SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

  for (int y = 0; y < WINHEIGHT / CELLHEIGHT; ++y) {
    for (int x = 0; x < WINWIDTH / CELLWIDTH; ++x) {
      SDL_RenderFillRect(state->renderer, state->cells[y][x]);
    }
  }

  SDL_RenderPresent(state->renderer);
  bool running = 1;

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = 0;
      }
    }
  }

  free(state);
}
