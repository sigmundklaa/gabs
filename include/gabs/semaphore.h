/**
 * Must define `gabs_sem` type
 */

#ifndef GABS_SEMAPHORE_H__
#define GABS_SEMAPHORE_H__

#include <gabs/core/util.h>
#include <gabs/core/api.h>
#include <gabs/common.h>

GABS_BEGIN_DECL

#define GABS_SEMAPHORE_INCLUDE_DEF
#include <gabs_semaphore_def.h>
#undef GABS_SEMAPHORE_INCLUDE_DEF

/**
 * @brief Initialize semaphore with initial count
 *
 * @param sem
 * @param initial Initial count for semaphore to hold
 * @return int
 * @retval 0 Success
 * @retval !=0 Error code
 */
GABS_API int gabs_sem_init(gabs_sem *, unsigned int);

/**
 * @brief Deinitialize semaphore
 *
 * @param sem
 * @return int
 * @retval 0 Success
 * @retval !=0 Error code
 */
GABS_API int gabs_sem_deinit(gabs_sem *);

/**
 * @brief Take semaphore
 *
 * @param sem
 * @param timeout Timeout in microseconds before aborting. Can be 0, in which
 * case no waiting is done.
 * @return int
 * @retval 0 Success
 * @retval !=0 Error code
 */
GABS_API int gabs_sem_down(gabs_sem *, uint64_t);

/**
 * @brief Give semaphore
 *
 * @param sem
 * @return int
 * @retval 0 Success
 * @retval !=0 Error code
 */
GABS_API int gabs_sem_up(gabs_sem *);

GABS_END_DECL

#endif /* GABS_SEMAPHORE_H__ */
