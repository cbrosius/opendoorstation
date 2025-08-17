#ifndef IO_RELAY_H
#define IO_RELAY_H

#include <stdint.h>

/**
 * @brief Initializes the GPIO pins for the relays.
 *
 * @param door_pin GPIO pin for the door relay.
 * @param light_pin GPIO pin for the light relay.
 */
void io_relays_init(uint8_t door_pin, uint8_t light_pin);

/**
 * @brief Pulses the door relay for a specific duration.
 *
 * @param duration_ms The duration of the pulse in milliseconds.
 */
void relay_pulse_door(uint32_t duration_ms);

/**
 * @brief Toggles the state of the light relay.
 */
void relay_toggle_light(void);

#endif // IO_RELAY_H
