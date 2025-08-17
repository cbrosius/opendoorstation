#include "unity.h"
#include "sip_logic.h"
#include "config.h"

// By including the .c files, we gain access to static functions and variables.
// This is a common technique for unit testing in C.
#include "io_relay.c"
#include "sip_logic.c"

// Mock the logging functions used by the modules under test
#define ESP_LOGI( tag, format, ... ) {}
#define ESP_LOGW( tag, format, ... ) {}
#define ESP_LOGE( tag, format, ... ) {}

// Mock the FreeRTOS functions
#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * configTICK_RATE_HZ ) / ( TickType_t ) 1000U ) )
void vTaskDelay( const uint32_t xTicksToDelay ) {}


void setUp(void) {
    // Reset the state before each test
    is_call_active = false;
    light_state = false;
}

void tearDown(void) {
    // Clean up after each test
}

TEST_CASE("Initial state is idle", "[sip_logic]")
{
    TEST_ASSERT_FALSE(sip_logic_is_call_active());
}

TEST_CASE("Can start and end a call", "[sip_logic]")
{
    // Initial state
    TEST_ASSERT_FALSE(sip_logic_is_call_active());

    // Start a call
    sip_logic_call();
    TEST_ASSERT_TRUE(sip_logic_is_call_active());

    // Hang up
    sip_logic_hangup();
    TEST_ASSERT_FALSE(sip_logic_is_call_active());
}

TEST_CASE("Calling while active does nothing", "[sip_logic]")
{
    sip_logic_call();
    TEST_ASSERT_TRUE(sip_logic_is_call_active());
    sip_logic_call(); // Second call should be ignored
    TEST_ASSERT_TRUE(sip_logic_is_call_active());
    sip_logic_hangup();
}

TEST_CASE("Hanging up while idle does nothing", "[sip_logic]")
{
    TEST_ASSERT_FALSE(sip_logic_is_call_active());
    sip_logic_hangup(); // Should be ignored
    TEST_ASSERT_FALSE(sip_logic_is_call_active());
}

TEST_CASE("Handles DTMF for door", "[sip_logic]")
{
    sip_logic_call(); // Must be in a call to process DTMF

    sip_event_t dtmf_event = { .type = SIP_EVENT_DTMF_RECEIVED, .data.dtmf.digit = '1' };
    sip_event_handler(&dtmf_event);
    
    // More advanced tests would mock relay_pulse_door and verify it was called.
    // For now, we just ensure it runs without error.

    sip_logic_hangup();
}

TEST_CASE("Handles DTMF for light", "[sip_logic]")
{
    sip_logic_call();
    TEST_ASSERT_FALSE(light_state); // Initial state

    sip_event_t dtmf_event = { .type = SIP_EVENT_DTMF_RECEIVED, .data.dtmf.digit = '2' };
    sip_event_handler(&dtmf_event);
    TEST_ASSERT_TRUE(light_state); // Toggled to true

    sip_event_handler(&dtmf_event);
    TEST_ASSERT_FALSE(light_state); // Toggled back to false

    sip_logic_hangup();
}

// This is the entry point for the test runner
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    unity_run_all_tests();
    return UNITY_END();
}
