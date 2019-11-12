#include "fiber.h"

#include <stdio.h>

scheduler *fiber_open() {
    scheduler *sched = malloc(sizeof(scheduler));
    sched->fiber_num = 0;
    sched->fiber_list = NULL;

    return sched;
}

void fiber_close(scheduler *sched) {
    assert(sched != NULL);

    while (sched->fiber_list != NULL) {
        fiber *tmp = sched->fiber_list;
        sched->fiber_list = tmp->next;
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
    fb->state = DETACHED;
    fb->next = NULL;

    if (sched->fiber_list == NULL) {
        sched->fiber_list = fb;
    }
    else {
        fiber *tmp = sched->fiber_list;
        sched->fiber_list = fb;
        fb->next = tmp;
    }

    return ++(sched->fiber_num);
}