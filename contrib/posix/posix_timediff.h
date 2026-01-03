
#ifndef GABS_POSIX_TIMEDIFF_H__
#define GABS_POSIX_TIMEDIFF_H__

#include <time.h>

#include <gabs/core/util.h>
#include <gabs/core/api.h>

GABS_BEGIN_DECL

/**
 * @brief Resolve time difference, in microseconds, to an absolute timespec
 *
 * @param diff Time difference in microseconds
 * @param spec Output timespec, in absolute time
 * @return int
 * @retval 0 Success
 * @retval !=0 Error code
 */
GABS_API int posix_timediff_resolve(uint64_t diff, struct timespec *spec);

GABS_END_DECL

#endif /* GABS_POSIX_TIMEDIFF_H__ */
