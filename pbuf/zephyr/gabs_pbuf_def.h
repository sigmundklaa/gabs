
#ifndef GABS_PBUF_ZEPHYR_DEF_H__
#define GABS_PBUF_ZEPHYR_DEF_H__

#ifndef GABS_PBUF_INCLUDE_DEF
#error "gabs_pbuf_def.h should not be included directly"
#endif /* GABS_PBUF_INCLUDE_DEF */

#include <zephyr/net_buf.h>

#include <gabs/core/util.h>
#include <gabs/core/api.h>

GABS_BEGIN_DECL

struct gabs_pbuf_zephyr {
        struct net_buf *frags;
};

struct gabs_pbuf_zephyr_ci {
        struct net_buf **head;
        struct net_buf *prev;
        struct net_buf *cur;
};

GABS_INLINE struct net_buf *gabs_pbuf_to_zephyr(struct gabs_pbuf_zephyr buf)
{
        return buf.frags;
}

GABS_INLINE struct gabs_pbuf_zephyr gabs_pbuf_from_zephyr(struct net_buf *buf)
{
        return (struct gabs_pbuf_zephyr){buf};
}

GABS_PBUF_TYPE_DEFINE(struct gabs_pbuf_zephyr);
GABS_PBUF_CI_TYPE_DEFINE(struct gabs_pbuf_zephyr_ci);

GABS_END_DECL

#endif /* GABS_PBUF_ZEPHYR_DEF_H__ */
