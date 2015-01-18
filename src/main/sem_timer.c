#include <inttypes.h>
#include <time.h>

#include "sem_error.h"
#include "sem_timer.h"

uint64_t sem_time_millis(struct timespec *ts);

int sem_timer_init(sem_timer_context* ctx) {
	return ctx->clock(&(ctx->clock_now));
}

int sem_timer_now(sem_timer_context* ctx) {
	ctx->clock_previous = ctx->clock_now;
	if (sem_timer_init(ctx) != SEM_OK) return SEM_ERROR;
	uint64_t delta = ctx->clock_now - ctx->clock_previous;
	ctx->now += delta * ctx->multiplier;
	return SEM_OK;
}

int sem_clock_monotonic(uint64_t* now) {
	struct timespec monotime;
	if (clock_gettime(CLOCK_MONOTONIC, &monotime) != 0) {
		sem_set_error("Could not get monotonic time");
		return SEM_ERROR;
	}
	*now = sem_time_millis(&monotime);
	return SEM_OK;
}

uint64_t sem_time_millis(struct timespec *ts) {
	return (uint64_t) (ts->tv_sec*1000 + ts->tv_nsec/1000000);
}