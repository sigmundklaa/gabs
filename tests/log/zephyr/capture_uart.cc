
#include <cstdint>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/serial/uart_emul.h>

#include "../log_capture.hh"

namespace
{

class uart_capture : public gabs::tests::log_capture
{
      public:
        void reset() override
        {
                uart_emul_flush_tx_data(uart_);
                len_ = 0;
        }

        bool contains(std::string_view needle) override
        {
                len_ += uart_emul_get_tx_data(uart_, &captured_[len_],
                                              sizeof(captured_) - len_);

                return std::string_view(reinterpret_cast<char *>(captured_),
                                        len_)
                               .find(needle) != std::string_view::npos;
        }

      private:
        const struct device *const uart_ =
                DEVICE_DT_GET(DT_NODELABEL(gabs_test_log_uart));
        uint8_t captured_[4096];
        size_t len_ = 0;
};

} // namespace

gabs::tests::log_capture &gabs::tests::log_capture::instance()
{
        static uart_capture capture;

        return capture;
}
