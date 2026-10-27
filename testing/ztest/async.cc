
#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include <gabs/testing/async.hh>

namespace
{

K_THREAD_STACK_DEFINE(stack, CONFIG_GABS_TESTING_ASYNC_STACK_SIZE);
struct k_work_q queue;

int start_queue()
{
        k_work_queue_init(&queue);
        k_work_queue_start(&queue, stack, K_THREAD_STACK_SIZEOF(stack),
                           CONFIG_GABS_TESTING_ASYNC_PRIORITY, nullptr);

        return 0;
}

} // namespace

SYS_INIT(start_queue, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

struct k_work_q *gabs::testing::detail::async_queue()
{
        return &queue;
}
