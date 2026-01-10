
#ifndef GABS_CORE_UTIL_H__
#define GABS_CORE_UTIL_H__

#include <stdint.h>

#ifdef __cplusplus
#define GABS_BEGIN_DECL extern "C" {
#define GABS_END_DECL   }
#else
#define GABS_BEGIN_DECL
#define GABS_END_DECL
#endif

GABS_BEGIN_DECL

#define GABS_UNUSED(x_) (void)(x_)

#define gabs_ptr_as_int__(x_) ((intptr_t)(void *)(x_))
#define gabs_int_as_ptr__(x_) ((void *)(intptr_t)(x_))

#define gabs_offset_of(type_, member_)                                         \
        gabs_ptr_as_int__((&(((type_ *)NULL)->member_)))

#define gabs_container_of(ptr_, type_, member_)                                \
        ((type_ *)gabs_int_as_ptr__(gabs_ptr_as_int__(ptr_) -                  \
                                    gabs_offset_of(type_, member_)))

#define gabs_min(a, b) (((a) < (b)) ? (a) : (b))
#define gabs_max(a, b) (((a) > (b)) ? (a) : (b))

GABS_END_DECL

#endif /* GABS_CORE_UTIL_H__ */
