
#ifndef GABS_TESTING_ZTEST_ASYNC_DEF_HH__
#define GABS_TESTING_ZTEST_ASYNC_DEF_HH__

#ifndef GABS_TESTING_INCLUDE_DEF
#error "gabs_testing_async_def.hh should not be included directly"
#endif /* GABS_TESTING_INCLUDE_DEF */

#include <optional>
#include <type_traits>
#include <utility>

#include <zephyr/kernel.h>

#include <gabs/cc/handle.hh>

namespace gabs::testing
{

namespace detail
{

struct k_work_q *async_queue();

template <typename F> class task
{
      public:
        using result_type = std::invoke_result_t<F &>;

        explicit task(F fn) : fn_(std::move(fn))
        {
                k_work_init(&work_, run);
                (void)k_work_submit_to_queue(async_queue(), &work_);
        }

        task(const task &) = delete;
        task &operator=(const task &) = delete;

        ~task()
        {
                wait();
        }

        result_type get()
        {
                wait();

                if constexpr (!std::is_void_v<result_type>) {
                        return std::move(*result_);
                }
        }

      private:
        struct none {
        };

        using stored_type = std::conditional_t<std::is_void_v<result_type>,
                                               none, result_type>;

        static void run(struct k_work *work)
        {
                auto &self = core::container_of(work, &task::work_);

                if constexpr (std::is_void_v<result_type>) {
                        self.fn_();
                } else {
                        self.result_.emplace(self.fn_());
                }
        }

        void wait()
        {
                struct k_work_sync sync;

                (void)k_work_flush(&work_, &sync);
        }

        F fn_;
        std::optional<stored_type> result_;
        struct k_work work_;
};

} // namespace detail

template <typename F> detail::task<std::decay_t<F>> async(F &&fn)
{
        return detail::task<std::decay_t<F>>(std::forward<F>(fn));
}

} // namespace gabs::testing

#endif /* GABS_TESTING_ZTEST_ASYNC_DEF_HH__ */
