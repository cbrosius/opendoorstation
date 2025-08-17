#include "io_relay.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static uint8_t pin_door;
static uint8_t pin_light;
static bool light_state = false;

void io_relays_init(uint8_t door_pin, uint8_t light_pin) {
    pin_door = door_pin;
    pin_light = light_pin;

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    // Bit mask for the pins, casting to 64-bit integer
    io_conf.pin_bit_mask = ((1ULL << pin_door) | (1ULL << pin_light));
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    // Initial state is off
    gpio_set_level(pin_door, 0);
    gpio_set_level(pin_light, 0);
}

void relay_pulse_door(uint32_t duration_ms) {
    gpio_set_level(pin_door, 1); // Turn on
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    gpio_set_level(pin_door, 0); // Turn off
}

void relay_toggle_light(void) {
    light_state = !light_state;
    gpio_set_level(pin_light, light_state);
}
