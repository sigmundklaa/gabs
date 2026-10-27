
#ifndef GABS_TESTS_LOG_CAPTURE_HH__
#define GABS_TESTS_LOG_CAPTURE_HH__

#include <string_view>

namespace gabs::tests
{

/**
 * @brief Captures the output of the log implementation under test.
 *
 * Each implementation's tests provide a subclass, and @ref instance, which
 * returns the one object of it.
 */
class log_capture
{
      public:
        static log_capture &instance();

        /** @brief Discard the output captured so far. */
        virtual void reset() = 0;

        /**
         * @brief Check whether @p needle is in the output captured since the
         * last @ref reset.
         */
        virtual bool contains(std::string_view needle) = 0;

      protected:
        ~log_capture() = default;
};

} // namespace gabs::tests

#endif /* GABS_TESTS_LOG_CAPTURE_HH__ */
