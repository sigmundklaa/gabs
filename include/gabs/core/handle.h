
#ifndef GABS_CORE_HANDLE_H__
#define GABS_CORE_HANDLE_H__

#include <gabs/core/util.h>
#include <gabs/core/api.h>

GABS_BEGIN_DECL

typedef char gabs_handle;

GABS_INLINE void gabs_handle_init(gabs_handle *h)
{
        GABS_UNUSED(h);
}

GABS_END_DECL

#endif /* GABS_CORE_HANDLE_H__ */
