
#include "../log_capture.h"

#include <string.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/serial/uart_emul.h>

GABS_LOGGER_DECLARE(gabs_test_log_logger, log_capture);

static const struct device *const capture_uart =
        DEVICE_DT_GET(DT_NODELABEL(gabs_test_log_uart));

static uint8_t capture_buf[4096];
static size_t capture_len;

void gabs_test_log_reset(void)
{
        (void)uart_emul_flush_tx_data(capture_uart);
        capture_len = 0;
}

const gabs_logger_h *gabs_test_log_handle(void)
{
        return gabs_test_log_logger;
}

static void capture_drain(void)
{
        uint32_t got;

        if (capture_len >= sizeof(capture_buf)) {
                return;
        }

        got = uart_emul_get_tx_data(capture_uart, &capture_buf[capture_len],
                                    sizeof(capture_buf) - capture_len);
        capture_len += got;
}

bool gabs_test_log_contains(const char *needle)
{
        size_t needle_len = strlen(needle);
        size_t i;

        capture_drain();

        if (needle_len == 0 || needle_len > capture_len) {
                return false;
        }

        for (i = 0; i + needle_len <= capture_len; i++) {
                if (memcmp(&capture_buf[i], needle, needle_len) == 0) {
                        return true;
                }
        }

        return false;
}
