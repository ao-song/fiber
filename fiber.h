#ifndef FIBER_H
#define FIBER_H

#if defined (__cplusplus)
extern "C" {
#endif

#include <ucontext.h>

#define STACK_SIZE (1024 * 1024)

typedef void (*fiber_func)();

typedef struct {
    char stack[STACK_SIZE];
    ucontext_t main;
} scheduler;

typedef struct {
    char *context;
} fiber;

int fiber_new();
int fiber_resume();
int fiber_yield();

#if defined (__cplusplus)
}
#endif

#endif