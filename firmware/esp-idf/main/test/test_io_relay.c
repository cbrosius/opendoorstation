#include "unity.h"
#include "io_relay.h"

// Include the .c file to test static variables like `light_state`
#include "io_relay.c"

// --- Mocking of hardware-specific functions ---
// We provide dummy implementations for the functions that interact with hardware
// so we can test the logic of our module in isolation.

void gpio_set_level(uint8_t gpio_num, uint32_t level) {
    // This is a mock function. In a real test, we could add more logic here,
    // for example, to check if the correct pin and level were set.
}

void gpio_config(const void *pGPIOConfig) {
    // Mock function
}

#define pdMS_TO_TICKS( xTimeInMs ) ( ( uint32_t ) ( ( ( uint32_t ) ( xTimeInMs ) * 1000 ) / ( uint32_t ) 1000U ) )
void vTaskDelay( const uint32_t xTicksToDelay ) {
    // Mock function
}
// --- End of Mocking ---


void setUp(void) {
    // This function is called before each test case.
    // We reset the state of the light to ensure tests are independent.
    light_state = false;
}

void tearDown(void) {
    // This function is called after each test case.
}

TEST_CASE("Initial light state is off", "[io_relay]")
{
    TEST_ASSERT_FALSE(light_state);
}

TEST_CASE("Toggle light changes state correctly", "[io_relay]")
{
    // Initial state should be off (as set in setUp)
    TEST_ASSERT_FALSE(light_state);

    // First toggle should turn the light on
    relay_toggle_light();
    TEST_ASSERT_TRUE(light_state);

    // Second toggle should turn the light off again
    relay_toggle_light();
    TEST_ASSERT_FALSE(light_state);
}

TEST_CASE("Pulse door function can be called", "[io_relay]")
{
    // This is a simple "smoke test". It calls the function to ensure it runs
    // without crashing. Testing the actual delay would require a more advanced setup.
    relay_pulse_door(100);
    // No assertion needed, the pass condition is that it didn't crash.
}
