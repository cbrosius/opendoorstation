#include "unity.h"
#include "io_relay.h"

// Include the .c file to test static variables like `light_state`
#include "io_relay.c"

// --- Mocking of hardware-specific and FreeRTOS functions ---
void gpio_set_level(uint8_t gpio_num, uint32_t level) {}
void gpio_config(const void *pGPIOConfig) {}
void vTaskDelay( const uint32_t xTicksToDelay ) {}
// Mock task creation and deletion for the pulse task
#define xTaskCreate(pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask) (1)
#define vTaskDelete(xTaskToDelete) {}
// --- End of Mocking ---


void setUp(void) {
    // Reset states before each test
    light_state = false;
    door_pulse_active = false;
}

void tearDown(void) {}

TEST_CASE("Initial light state is off", "[io_relay]")
{
    TEST_ASSERT_FALSE(light_state);
}

TEST_CASE("Toggle light changes state correctly", "[io_relay]")
{
    TEST_ASSERT_FALSE(light_state);
    relay_toggle_light();
    TEST_ASSERT_TRUE(light_state);
    relay_toggle_light();
    TEST_ASSERT_FALSE(light_state);
}

TEST_CASE("Get relay states reflects current state", "[io_relay]")
{
    relay_states_t states;

    // Check initial state
    states = relay_get_states();
    TEST_ASSERT_FALSE(states.door_active);
    TEST_ASSERT_FALSE(states.light_active);

    // Check after toggling light
    relay_toggle_light();
    states = relay_get_states();
    TEST_ASSERT_FALSE(states.door_active);
    TEST_ASSERT_TRUE(states.light_active);
}

TEST_CASE("Pulse door function can be called", "[io_relay]")
{
    // This is a simple "smoke test" to ensure the function, which now creates a task,
    // can be called without crashing. Testing the asynchronous behavior of the task
    // itself would require a more advanced testing setup.
    relay_pulse_door(100);
    // No assertion needed, the pass condition is that it didn't crash.
}
