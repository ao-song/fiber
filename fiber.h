#ifndef __FIBER_H__
#define __FIBER_H__

#if defined (__cplusplus)
extern "C" {
#endif

#include <ucontext.h>

#define STACK_SIZE (1024 * 1024)

typedef enum {
    DETACHED,
    ATTACHED,
    RESUMED,
    TERMINATED
} fiber_state;

typedef void (*fiber_func)(scheduler *sched, void *arg);

typedef struct {
    char stack[STACK_SIZE]; // copy-stack
    ucontext_t main;
    int fiber_num;
    fiber *fiber_list;
} scheduler;

typedef struct {
    char *ctx;
    scheduler *sched;
    fiber_func func;
    void *arg;
    fiber_state state;
    char *stack;
    fiber *next;
} fiber;

scheduler *fiber_open();
int fiber_close(scheduler *sched);
int fiber_new(scheduler *sched, fiber_func func, void *arg);
int fiber_resume();
int fiber_yield();

#if defined (__cplusplus)
}
#endif

#endif