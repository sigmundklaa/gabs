
#ifndef GABS_RANDOM_H__
#define GABS_RANDOM_H__

#include <stdint.h>
#include <stddef.h>

#include <gabs/core/util.h>
#include <gabs/core/api.h>

GABS_BEGIN_DECL

/**
 * @brief POSIX `random`-like
 *
 * @return Random number in the range `[0, 2^31-1)`
 */
GABS_API uint32_t gabs_random(void);

GABS_END_DECL

#endif /* GABS_RANDOM_H__ */
