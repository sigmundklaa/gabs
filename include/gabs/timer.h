
#ifndef GABS_TIMER_H__
#define GABS_TIMER_H__

#include <stdbool.h>

#include <gabs/core/util.h>
#include <gabs/core/api.h>

#define GABS_TIMER_INCLUDE_DEF
#include <gabs_timer_def.h>
#undef GABS_TIMER_INCLUDE_DEF

GABS_BEGIN_DECL

typedef void (*gabs_timer_cb)(gabs_timer, void *);

/**
 * @brief Initialize a timer context.
 *
 * Each timer context can have multiple timers associated with it. It is
 * primarily used to ensure that the timers can be safely stopped without
 * running the risk of spurious timer alarms after stopping/uninstalling the
 * individual timer.
 *
 * This functionality is especially important in cases where the timer is
 * associated with an object that gets deleted after uninstalling.
 *
 * @return int
 * @retval 0 Success
 * @retval <0 Error code
 */
GABS_API int gabs_timer_ctx_init(gabs_timer_ctx *ctx);

/**
 * @brief Deinitialize timer context
 *
 * This first stops all associated timers, and then destroys them. Attempting
 * to use the timer after this call is undefined behaviour.
 *
 * @return int
 * @retval 0 Success
 * @retval <0 Error code
 */
GABS_API int gabs_timer_ctx_deinit(gabs_timer_ctx *ctx);

/**
 * @brief Stop all associated timers
 *
 * This ensures that no timers will fire their associated callbacks after this
 * function returns.
 *
 * @return int
 * @retval 0 Success
 * @retval <0 Error code
 */
GABS_API int gabs_timer_ctx_stop(gabs_timer_ctx *ctx);

/**
 * @brief Start associated timers
 *
 * @return int
 * @retval 0 Success
 * @retval <0 Error code
 */
GABS_API int gabs_timer_ctx_start(gabs_timer_ctx *ctx);

/**
 * @brief Install (create) timer, that invokes @p cb with @p user_arg when
 * the timeout expires.
 *
 * Note that this does not start the timer. @ref gabs_timer_start or
 * @ref gabs_timer_restart must be called for that.
 *
 * @return gabs_timer Timer. Callers should verify that it is a valid ID with
 * @ref gabs_timer_okay. If not okay, an implementation specific failure
 * occured.
 */
GABS_API gabs_timer gabs_timer_install(gabs_timer_ctx *ctx, gabs_timer_cb cb,
                                       void *user_arg);

/**
 * @brief Uninstall timer
 *
 * If not already stopped, this will stop the timer before removing it.
 * It is safe to uninstall a timer from within the associated callback
 * of the timer.
 *
 * @return int
 * @retval 0 Success
 * @retval <0 Error code
 */
GABS_API int gabs_timer_uninstall(gabs_timer timer);

/**
 * @brief Verify that timer is valid
 *
 * An invalid timer can be returned from @ref gabs_timer_install if a failure
 * occurs during creation. This failure is implementation-specific, and
 * must be diagnosed as such.
 */
GABS_API bool gabs_timer_okay(gabs_timer timer);

/**
 * @brief Start @p timer if not already started, expiring in @p delay_us
 * microseconds.
 *
 * This does nothing if the timer is already started.
 *
 * @return int
 * @retval 0 Success
 * @retval -EBUSY Already started
 * @retval <0 Error code
 */
GABS_API int gabs_timer_start(gabs_timer timer, uint64_t delay_us);

/**
 * @brief Restart @p timer.
 *
 * Like @ref gabs_timer_start, but this will
 * reset the timeout if the timer is already started.
 *
 * @return int
 * @retval 0 Success
 * @retval <0 Error code
 */
GABS_API int gabs_timer_restart(gabs_timer timer, uint64_t delay_us);

/**
 * @brief Stop @p timer
 *
 * Note that this only requests a stop, and the timer may not be fully
 * stopped after the return of this function. This can occur if the timer
 * is in the process of firing when it gets the stop request.
 *
 * @return int
 * @retval 0 Success
 * @retval <0 Error code
 */
GABS_API int gabs_timer_stop(gabs_timer timer);

/**
 * @brief Check if @p timer is active (running)
 *
 * @return bool
 * @retval true Timer is active (will expire at some point)
 * @retval false Timer is inactive/idle
 */
GABS_API bool gabs_timer_active(gabs_timer timer);

GABS_END_DECL

#endif /* GABS_TIMER_H__ */
