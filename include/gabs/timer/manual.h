/**
 * Timers of the gabs-timer-manual implementation never fire on their own.
 * Each context has a clock that starts at 0 and only moves in
 * gabs_timer_manual_advance(). Callbacks run in the thread that calls
 * gabs_timer_manual_advance() or gabs_timer_manual_fire(), before it
 * returns.
 *
 * The implementation has no locking: calls on one context, including those
 * made from callbacks, must not run concurrently.
 */

#ifndef GABS_TIMER_MANUAL_H__
#define GABS_TIMER_MANUAL_H__

#include <stdint.h>

#include <gabs/timer.h>

GABS_BEGIN_DECL

/**
 * @brief Move the clock of @p ctx forward by @p us microseconds, firing
 * every timer that expires on the way.
 *
 * Timers fire in the order of their expiry, and those that expire at the
 * same time in the order they were started. The clock is at a timer's expiry
 * while its callback runs, so a timer restarted from its callback fires again
 * within the same call if its new expiry is not past the end.
 *
 * @return int Number of callbacks run
 */
GABS_API int gabs_timer_manual_advance(gabs_timer_ctx *ctx, uint64_t us);

/**
 * @brief Fire @p timer now, without moving the clock.
 *
 * @return int
 * @retval 0 Success
 * @retval -EINVAL Timer is not active
 */
GABS_API int gabs_timer_manual_fire(gabs_timer timer);

/** @brief Current time of the clock of @p ctx, in microseconds. */
GABS_API uint64_t gabs_timer_manual_now(const gabs_timer_ctx *ctx);

GABS_END_DECL

#endif /* GABS_TIMER_MANUAL_H__ */
