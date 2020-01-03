#include "fiber.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

static fiber *_get_fiber_(scheduler *sched, int id);
static void _store_running_stack_(char *stack, fiber *fb);

scheduler *fiber_open() {
    scheduler *sched = malloc(sizeof(scheduler));
    sched->id_count = 0;
    sched->fiber_list = NULL;
    sched->running_fiber = NULL;

    return sched;
}

void fiber_close(scheduler *sched) {
    assert(sched != NULL);

    while (sched->fiber_list != NULL) {
        fiber *tmp = sched->fiber_list;
        sched->fiber_list = tmp->next;
        if (tmp->stack != NULL) {
            free(tmp->stack);
        }
        free(tmp);            
    }

    free(sched);
    sched = NULL;
}

int fiber_new(scheduler *sched, fiber_func func, void *arg) {
    fiber *fb = malloc(sizeof(fiber));
    fb->sched = sched;
    fb->func = func;
    fb->arg = arg;
    fb->state = NEW;    
    fb->stack = NULL;
    fb->size = 0;
    fb->next = NULL;

    if (sched->fiber_list == NULL) {
        sched->fiber_list = fb;
    } else {
        fiber *tmp = sched->fiber_list;
        sched->fiber_list = fb;
        fb->next = tmp;
    }

    return fb->id = ++(sched->id_count);
}

void fiber_resume(scheduler *sched, int id) {
    fiber *fb = _get_fiber_(sched, id);
    assert(fb != NULL);

    switch (fb->state) {
    case NEW:
        getcontext(&fb->ctx);
        fb->ctx.uc_stack.ss_sp = sched->stack;
        fb->ctx.uc_stack.ss_size = STACK_SIZE;
        fb->ctx.uc_link = &sched->main;
        fb->state = RUNNING;
        sched->running_fiber = fb;
        makecontext(&fb->ctx, (void (*)(void)) fb->func, 1, fb->arg);
        swapcontext(&sched->main, &fb->ctx);
        break;
    case SUSPENDED:
        memcpy(fb->low, fb->stack, fb->size);
        fb->state = RUNNING;
        sched->running_fiber = fb;
        swapcontext(&sched->main, &fb->ctx);
        break;
    default:
        break;
    }
}

void fiber_yield(scheduler *sched) {
    fiber *fb = sched->running_fiber;
    _store_running_stack_(sched->stack, fb);
    fb->state = SUSPENDED;
    sched->running_fiber = NULL;
    swapcontext(&fb->ctx, &sched->main);
}


// Internal functions
static fiber *
_get_fiber_(scheduler *sched, int id) {
    if (id < 0 || id > sched->id_count) {
        return NULL;
    }

    fiber *fb = sched->fiber_list;
    while (fb != NULL && fb->id != id) {
        fb = fb->next;
    }

    return fb;
}

static void
_store_running_stack_(char *stack, fiber *fb) {
    fb->high = fb->low = stack;
    char x = 0;
    if (&x > stack) {
        fb->high = &x;
    } else {
        fb->low = &x;
    }

    if (fb->high - fb->low > fb->size) {
        free(fb->stack);
        fb->size = fb->high - fb->low;
        fb->stack = malloc(fb->size);
    }
    memcpy(fb->stack, stack, fb->size);
}