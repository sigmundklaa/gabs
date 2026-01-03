/**
 * Implementation must provide the `gabs_mutex` type
 */

#ifndef GABS_MUTEX_H__
#define GABS_MUTEX_H__

#include <gabs/core/util.h>
#include <gabs/core/api.h>

GABS_BEGIN_DECL

#define GABS_MUTEX_INCLUDE_DEF
#include <gabs_mutex_def.h>
#undef GABS_MUTEX_INCLUDE_DEF

/**
 * @brief Initialize mutex
 * @return int
 * @retval 0 Success
 * @retval !=0 Error code
 */
GABS_API int gabs_mutex_init(gabs_mutex *);

/**
 * @brief Deinitialize mutex
 * @return int
 * @retval 0 Success
 * @retval !=0 Error code
 */
GABS_API int gabs_mutex_deinit(gabs_mutex *);

/**
 * @brief Lock mutex
 *
 * @param mutex Mutex to acquire
 * @param timeout Timeout in microseconds before aborting. Can be 0, in which
 * case no waiting is done.
 * @return int
 * @retval 0 Success
 * @retval !=0 Error code
 */
GABS_API int gabs_mutex_lock(gabs_mutex *, uint64_t);

/**
 * @brief Unlock mutex
 *
 * @param mutex Mutex to release
 * @return int
 * @retval 0 Success
 * @retval !=0 Error code
 */
GABS_API int gabs_mutex_unlock(gabs_mutex *);

GABS_END_DECL

#endif /* GABS_MUTEX_H__ */
