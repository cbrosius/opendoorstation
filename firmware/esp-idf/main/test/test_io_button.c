#include "unity.h"
#include "cmock.h"
#include "mock_driver/gpio.h"
#include "mock_freertos/FreeRTOS.h"
#include "mock_freertos/task.h"
#include "mock_freertos/queue.h"

#include "io_button.h"

static bool callback_was_called = false;

static void test_callback(void) {
    callback_was_called = true;
}

void setUp(void) {
    callback_was_called = false;
}

void tearDown(void) {}

void test_io_button_init_should_configure_gpio_and_interrupt(void) {
    // Given
    const uint8_t pin = 13;
    gpio_config_t io_conf;

    gpio_config_ExpectAnyArgs();
    xQueueCreate_ExpectAndReturn(10, sizeof(uint32_t), (QueueHandle_t)1);
    xTaskCreate_ExpectAnyArgsAndReturn(pdPASS);
    gpio_install_isr_service_Expect(0);
    gpio_isr_handler_add_Expect(pin, gpio_isr_handler, (void*) (intptr_t) pin);

    // When
    io_button_init(pin, test_callback);

    // Then
    // The expectations set above are the assertions.
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_io_button_init_should_configure_gpio_and_interrupt);
    return UNITY_END();
}
