#ifndef CONFIG_H
#define CONFIG_H

// -- Wi-Fi Configuration
#define WIFI_SSID        "YourSSID"
#define WIFI_PASS        "YourPassword"

// -- SIP Configuration
#define SIP_USER         "doorbell"
#define SIP_PASS         "secret"
#define SIP_DOMAIN       "your.sip.server" // e.g., "fritz.box" or "sip.provider.com"
#define SIP_OUTBOUND     ""                // Optional outbound proxy
#define SIP_CALLEE_URI   "sip:100@your.sip.server" // The destination to call

// -- GPIO Configuration (example pins, please adjust to your board)
#define PIN_BTN          13
#define PIN_RELAY_DOOR   15
#define PIN_RELAY_LIGHT  16

// -- Behavior Configuration
#define DOOR_PULSE_MS    700   // How long the door relay should be active

#endif // CONFIG_H
