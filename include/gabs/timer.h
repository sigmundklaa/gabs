
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
GABS_API gabs_timer gabs_timer_install(gabs_timer_cb cb, void *user_arg);

/**
 * @brief Uninstall timer
 *
 * If not already stopped, this will stop the timer before removing it.
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
GABS_API int gabs_timer_start(gabs_timer timer, uint32_t delay_us);

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
GABS_API int gabs_timer_restart(gabs_timer timer, uint32_t delay_us);

/**
 * @brief Stop @p timer
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
