#ifndef BASE_H
#define BASE_H

#include <assert.h>
#include <error.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
//////////////////
// NOTE: Context Cracking

#if defined(__clang__)
#define COMPILER_CLANG 1
#if defined(__gnu_linux__)
#define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_MAC 1
#elif defined(_WIN32)
#define OS_WINDOWS 1
#else
#error missing OS detection
#endif
#elif defined(__GNUC__)
#define COMPILER_GCC 1
#if defined(__gnu_linux__)
#define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_MAC 1
#elif defined(_WIN32)
#define OS_WINDOWS 1
#else
#error missing OS detection
#endif
#else
#error no context cracking for this compiler
#endif // __clang__

// NOTE: Zero fill missing context macros

#if !defined(COMPILER_CLANG)
#define COMPILER_CLANG 0
#endif // COMPILER_CLANG
#if !defined(COMPILER_GCC)
#define COMPILER_GCC 0
#endif // COMPILER_GCC
#if !defined(OS_LINUX)
#define OS_LINUX 0
#endif // OS_LINUX
#if !defined(OS_MAC)
#define OS_MAC 0
#endif // OS_MAC
#if !defined(OS_WINDOWS)
#define OS_WINDOWS 0
#endif

//////////////////
// NOTE: Helper macros

#define global static
#define local static

//////////////////
// NOTE: Basic types

typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef S8 B8;
typedef S16 B16;
typedef S32 B32;
typedef S64 B64;
typedef float F32;
typedef double F64;

//////////////////
// NOTE: Basic constants

global S8 min_S8 = (S8)0x80;
global S16 min_S16 = (S16)0x8000;
global S32 min_S32 = (S32)0x80000000;
global S64 min_S64 = (S64)0x8000000000000000llu;

global S8 max_S8 = (S8)0x7f;
global S16 max_S16 = (S16)0x7fff;
global S32 max_S32 = (S32)0x7fffffff;
global S64 max_S64 = (S64)0x7fffffffffffffffllu;

global U8 max_U8 = 0xff;
global U16 max_U16 = 0xffff;
global U32 max_U32 = 0xffffffff;
global U64 max_U64 = 0xffffffffffffffffllu;

//////////////////
// NOTE: Dynamic array (list)

// NOTE to future self: Should probably look into if this can be optimized
// All this code was cobbled together from a stackoverflow answer and wikipedia
// about dynamic arrays Should look into possible optimization options.

#define LIST_INITIAL_CAPACITY 8
#define List(type) type *

// Data layout - [capacity][used][data]
#define initList(list)                                                         \
  do {                                                                         \
    size_t *data =                                                             \
        malloc(2 * sizeof(size_t) + LIST_INITIAL_CAPACITY * sizeof(*(list)));  \
    data[0] = LIST_INITIAL_CAPACITY;                                           \
    data[1] = 0;                                                               \
    (list) = (typeof(*(list)) *)&data[2];                                      \
  } while (0)

#define freeList(list) free((size_t *)(list)-2);

#define listPop(list)                                                          \
  (assert(listUsed(list) > 0), (list)[--*((size_t *)(list)-1)])
#define listGet(list, idx) (assert((idx) < listUsed(list)), (list)[(idx)])

#define listAppend(list, value)                                                \
  do {                                                                         \
    if (listUsed(list) + 1 >= listCapacity(list)) {                            \
      size_t *data = ((size_t *)(list)-2);                                     \
      size_t newCap = listCapacity(list) * 2 + 2 * sizeof(size_t);             \
      data = realloc(data, newCap);                                            \
      (list) = (typeof(*(list)) *)&data[2];                                    \
    }                                                                          \
    size_t *used = ((size_t *)(list)-1);                                       \
    (list)[(*used)++] = (value);                                               \
  } while (0)

#define listCapacity(list) ((list) ? *(size_t *)(list - 2) : 01u)
#define listUsed(list) ((list) ? *(size_t *)((size_t *)(list)-1) : 0u)

#define listIsEmpty(list) (listUsed(list) == 0)

//////////////////
// NOTE: Arena allocator

#define PAGE_SIZE 4095

typedef struct arena {
  U8 *region;
  size_t size;
  U64 current;
  struct arena *next;
} arena_t;

void arena_pop(arena_t *arena, size_t size);

void arena_clear(arena_t *arena);

static arena_t *_arena_create(size_t size) {
  arena_t *arena = (arena_t *)calloc(1, sizeof(arena_t));

  if (!arena)
    return NULL;

  arena->region = (U8 *)calloc(size, sizeof(U8));
  arena->size = size;

  if (!arena->region) {
    free(arena);
    return NULL;
  }

  return arena;
}

arena_t *arena_create() { return _arena_create(PAGE_SIZE); }

void *arena_push(arena_t *arena, U64 size) {
  arena_t *last = arena;

  do {
    if ((arena->size - arena->current) >= size) {
      arena->current += size;
      return arena->region + (arena->current - size);
    }
    last = arena;
  } while ((arena = arena->next) != NULL);

  size_t asize = size > PAGE_SIZE ? size : PAGE_SIZE;

  arena_t *next = _arena_create(asize);
  last->next = next;
  next->current += size;
  return next->region;
}

void arena_release(arena_t *arena) {
  arena_t *next, *last = arena;
  do {
    next = last->next;
    free(last->region);
    free(last);
    last = next;
  } while (next != NULL);
}

U64 arena_get_position(arena_t *arena) { return arena->current; }

void arena_set_pos_back(arena_t *arena, U64 position) {
  arena->current = position;
}

#endif // BASE_H
