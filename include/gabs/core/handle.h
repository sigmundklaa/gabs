
#ifndef GABS_CORE_HANDLE_H__
#define GABS_CORE_HANDLE_H__

#include <gabs/core/util.h>
#include <gabs/core/api.h>

GABS_BEGIN_DECL

/* TODO: Zero length array */
#define gabs_handle                                                            \
        struct {                                                               \
                char unused;                                                   \
        }

GABS_END_DECL

#endif /* GABS_CORE_HANDLE_H__ */
