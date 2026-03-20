
#ifndef GABS_TIMER_POSIX_ATOMIC_MASK_H__
#define GABS_TIMER_POSIX_ATOMIC_MASK_H__

#include <stdatomic.h>
#include <stdbool.h>

#include <gabs/core/util.h>

GABS_BEGIN_DECL

static inline void mask_set(atomic_uint *state, unsigned int mask)
{
        unsigned int masked;
        unsigned int expected;

        masked = mask;
        expected = 0;

        while (!atomic_compare_exchange_weak(state, &expected, masked)) {
                masked = expected | mask;
        }
}

/** @brief Set mask, only if previously unset */
static inline bool mask_set_strict(atomic_uint *state, unsigned int mask)
{
        unsigned int masked;
        unsigned int expected;

        expected = 0;
        masked = mask;

        while (!atomic_compare_exchange_weak(state, &expected, masked)) {
                if ((expected & mask) != 0) {
                        return false;
                }
        }

        return true;
}

static inline void mask_clear(atomic_uint *state, unsigned int mask)
{
        unsigned int masked;
        unsigned int expected;

        masked = 0;
        expected = 0;

        while (!atomic_compare_exchange_weak(state, &expected, masked)) {
                masked = expected & (~mask);
        }
}

static inline bool mask_cas(atomic_uint *state, unsigned int cmp,
                            unsigned int set)
{
        unsigned int masked;
        unsigned int expected;

        expected = cmp;
        masked = set;

        while (!atomic_compare_exchange_weak(state, &expected, masked)) {
                if ((expected & cmp) != cmp) {
                        return false;
                }

                masked = expected | set;
        }

        return true;
}

static inline bool mask_cac(atomic_uint *state, unsigned int cmp)
{
        unsigned int expected;
        unsigned int masked;

        expected = cmp;
        masked = 0;

        while (!atomic_compare_exchange_weak(state, &expected, masked)) {
                if ((expected & cmp) != cmp) {
                        return false;
                }

                masked = expected & ~(cmp);
        }

        return true;
}

static inline unsigned int mask_get_ex(atomic_uint *state, unsigned int mask,
                                       memory_order order)
{
        return atomic_load_explicit(state, order) & mask;
}

static inline unsigned int mask_get(atomic_uint *state, unsigned int mask)
{
        return mask_get_ex(state, mask, memory_order_seq_cst);
}

static inline bool mask_test_ex(atomic_uint *state, unsigned int mask,
                                memory_order order)
{
        return mask_get_ex(state, mask, order) == mask;
}

static inline bool mask_test(atomic_uint *state, unsigned int mask)
{
        return mask_test_ex(state, mask, memory_order_seq_cst);
}

GABS_END_DECL

#endif /* GABS_TIMER_POSIX_ATOMIC_MASK_H__ */
