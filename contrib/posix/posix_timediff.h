
#ifndef GABS_POSIX_TIMEDIFF_H__
#define GABS_POSIX_TIMEDIFF_H__

#include <time.h>

#include <gabs/core/util.h>
#include <gabs/core/api.h>

GABS_BEGIN_DECL

GABS_API int posix_timediff_resolve(uint64_t diff, struct timespec* spec);

GABS_END_DECL

#endif /* GABS_POSIX_TIMEDIFF_H__ */
