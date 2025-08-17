#include "io_relay.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static uint8_t pin_door;
static uint8_t pin_light;

// State tracking variables
static volatile bool light_state = false;
static volatile bool door_pulse_active = false;

void io_relays_init(uint8_t door_pin, uint8_t light_pin) {
    pin_door = door_pin;
    pin_light = light_pin;

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = ((1ULL << pin_door) | (1ULL << pin_light));
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    gpio_set_level(pin_door, 0);
    gpio_set_level(pin_light, 0);
}

// Task to handle the non-blocking pulse
static void relay_pulse_door_task(void *pvParameters) {
    uint32_t duration_ms = (uint32_t)pvParameters;
    
    door_pulse_active = true;
    gpio_set_level(pin_door, 1);
    
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    
    gpio_set_level(pin_door, 0);
    door_pulse_active = false;
    
    vTaskDelete(NULL); // The task is done, delete it
}

void relay_pulse_door(uint32_t duration_ms) {
    // Create a temporary task to handle the pulse so this function doesn't block.
    xTaskCreate(relay_pulse_door_task, "pulse_task", 2048, (void*)duration_ms, 5, NULL);
}

void relay_toggle_light(void) {
    light_state = !light_state;
    gpio_set_level(pin_light, light_state);
}

relay_states_t relay_get_states(void) {
    return (relay_states_t){
        .door_active = door_pulse_active,
        .light_active = light_state
    };
}