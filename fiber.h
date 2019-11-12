#ifndef __FIBER_H__
#define __FIBER_H__

#if defined (__cplusplus)
extern "C" {
#endif

#include <ucontext.h>

#define STACK_SIZE (128 * 1024)

typedef enum {
    NEW,
    SUSPENDED,
    RUNNING,
    TERMINATED
} fiber_state;

typedef void (*fiber_func)(void *arg);

typedef struct fiber fiber;
typedef struct scheduler scheduler;

struct scheduler {
    char stack[STACK_SIZE]; // copy-stack
    ucontext_t main;
    int id_count;
    fiber *running_fiber;
    fiber *fiber_list;
};

struct fiber {
    int id;
    ucontext_t ctx;
    scheduler *sched;
    fiber_func func;
    void *arg;
    fiber_state state;
    char *stack;
    int size;
    char *high;
    char *low;
    fiber *next;
};

scheduler *fiber_open();
void fiber_close(scheduler *sched);
int fiber_new(scheduler *sched, fiber_func func, void *arg);
void fiber_resume(scheduler *sched, int id);
void fiber_yield(scheduler *sched);

#if defined (__cplusplus)
}
#endif

#endif