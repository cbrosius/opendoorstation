#ifndef SIP_LOGIC_H
#define SIP_LOGIC_H

#include <stdbool.h>

/**
 * @brief Initializes the SIP client.
 */
void sip_logic_init(void);

/**
 * @brief Starts a call to the configured callee.
 */
void sip_logic_call(void);

/**
 * @brief Hangs up the current call.
 */
void sip_logic_hangup(void);

/**
 * @brief Checks if a call is currently active.
 *
 * @return true if a call is active, false otherwise.
 */
bool sip_logic_is_call_active(void);

/**
 * @brief Checks if the SIP client is registered.
 *
 * @return true if the client is registered, false otherwise.
 */
bool sip_logic_is_registered(void);

#endif // SIP_LOGIC_H
