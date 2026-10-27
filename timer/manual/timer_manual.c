
#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <gabs/core/util.h>
#include <gabs/timer.h>
#include <gabs/timer/manual.h>

static void timer_fire(struct gabs_timer_manual *t)
{
        /* Cleared first, so the callback can start the timer again. */
        t->active = false;
        t->cb(t, t->user_arg);
}

static struct gabs_timer_manual *next_expired(struct gabs_timer_manual_ctx *ctx,
                                              uint64_t end_us)
{
        struct gabs_timer_manual *next;
        struct gabs_timer_manual *cur;
        size_t i;

        next = NULL;

        for (i = 0; i < gabs_array_size(ctx->timers); i++) {
                cur = &ctx->timers[i];

                if (!cur->used || !cur->active || cur->expiry_us > end_us) {
                        continue;
                }

                if (next == NULL || cur->expiry_us < next->expiry_us ||
                    (cur->expiry_us == next->expiry_us &&
                     cur->seq < next->seq)) {
                        next = cur;
                }
        }

        return next;
}

int gabs_timer_ctx_init(struct gabs_timer_manual_ctx *ctx)
{
        (void)memset(ctx, 0, sizeof(*ctx));

        return 0;
}

int gabs_timer_ctx_deinit(struct gabs_timer_manual_ctx *ctx)
{
        (void)memset(ctx->timers, 0, sizeof(ctx->timers));

        return 0;
}

bool gabs_timer_okay(struct gabs_timer_manual *t)
{
        return t != NULL;
}

gabs_timer gabs_timer_install(struct gabs_timer_manual_ctx *ctx,
                              gabs_timer_cb cb, void *user_data)
{
        struct gabs_timer_manual *t;
        size_t i;

        for (i = 0; i < gabs_array_size(ctx->timers); i++) {
                t = &ctx->timers[i];

                if (t->used) {
                        continue;
                }

                (void)memset(t, 0, sizeof(*t));

                t->used = true;
                t->cb = cb;
                t->user_arg = user_data;
                t->ctx = ctx;

                return t;
        }

        return NULL;
}

int gabs_timer_uninstall(struct gabs_timer_manual *t)
{
        t->active = false;
        t->used = false;

        return 0;
}

int gabs_timer_start(struct gabs_timer_manual *t, uint64_t delay_us)
{
        if (t->active) {
                return -EBUSY;
        }

        return gabs_timer_restart(t, delay_us);
}

int gabs_timer_restart(struct gabs_timer_manual *t, uint64_t delay_us)
{
        t->active = true;
        t->expiry_us = t->ctx->now_us + delay_us;
        t->seq = t->ctx->next_seq++;

        return 0;
}

int gabs_timer_stop(struct gabs_timer_manual *t)
{
        t->active = false;

        return 0;
}

bool gabs_timer_active(struct gabs_timer_manual *t)
{
        return t->active;
}

int gabs_timer_manual_advance(struct gabs_timer_manual_ctx *ctx, uint64_t us)
{
        struct gabs_timer_manual *t;
        uint64_t end_us;
        int count;

        end_us = ctx->now_us + us;
        count = 0;

        while ((t = next_expired(ctx, end_us)) != NULL) {
                ctx->now_us = t->expiry_us;

                timer_fire(t);
                count++;
        }

        ctx->now_us = end_us;

        return count;
}

int gabs_timer_manual_fire(struct gabs_timer_manual *t)
{
        if (!t->active) {
                return -EINVAL;
        }

        timer_fire(t);

        return 0;
}

uint64_t gabs_timer_manual_now(const struct gabs_timer_manual_ctx *ctx)
{
        return ctx->now_us;
}
