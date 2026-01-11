
#ifndef GABS_CORE_UTIL_H__
#define GABS_CORE_UTIL_H__

#include <stdint.h>
#include <stddef.h>

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

#define GABS_COUNT_VA_ARGS__(x0, x1, x2, x3, N, ...) N

#define GABS_COUNT_VA_ARGS(...) GABS_COUNT_VA_ARGS__(__VA_ARGS__, 4, 3, 2, 1, 0)

#define GABS_CONCAT_IMPL__(a, b) a##b
#define GABS_CONCAT__(a, b)      GABS_CONCAT_IMPL__(a, b)
#define GABS_CONCAT_1(a)         a
#define GABS_CONCAT_2(a, b)      GABS_CONCAT__(a, GABS_CONCAT_1(b))
#define GABS_CONCAT_3(a, ...)    GABS_CONCAT__(a, GABS_CONCAT_2(__VA_ARGS__))
#define GABS_CONCAT_4(a, ...)    GABS_CONCAT__(a, GABS_CONCAT_3(__VA_ARGS__))

GABS_END_DECL

#endif /* GABS_CORE_UTIL_H__ */
