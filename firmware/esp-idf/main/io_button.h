#ifndef IO_BUTTON_H
#define IO_BUTTON_H

#include <stdint.h>

// Define a callback function pointer type for when the button is pressed.
typedef void (*button_callback_t)(void);

/**
 * @brief Initializes the button GPIO pin and sets up an interrupt.
 *
 * @param pin The GPIO pin number for the button.
 * @param callback The function to call when the button is pressed.
 */
void io_button_init(uint8_t pin, button_callback_t callback);

#endif // IO_BUTTON_H
