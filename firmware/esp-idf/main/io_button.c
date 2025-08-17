#include "io_button.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static button_callback_t app_callback = NULL;

// A queue to handle events from the ISR to a task
static QueueHandle_t gpio_evt_queue = NULL;

// ISR handler for the button press
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

// Task to handle debouncing and trigger the callback
static void button_task(void* arg) {
    uint32_t io_num;
    for (;;) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            // Debounce delay
            vTaskDelay(pdMS_TO_TICKS(50));
            // If button is still pressed (active low), trigger callback
            if (gpio_get_level(io_num) == 0) {
                if (app_callback) {
                    app_callback();
                }
            }
        }
    }
}

void io_button_init(uint8_t pin, button_callback_t callback) {
    app_callback = callback;

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // Trigger on falling edge (press)
    io_conf.pin_bit_mask = (1ULL << pin);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1; // Use internal pull-up resistor
    io_conf.pull_down_en = 0;
    gpio_config(&io_conf);

    // Create a queue to handle gpio events from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    // Create the handler task
    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);

    // Install gpio isr service
    gpio_install_isr_service(0);
    // Hook isr handler for the specific gpio pin
    gpio_isr_handler_add(pin, gpio_isr_handler, (void*) (intptr_t) pin);
}
