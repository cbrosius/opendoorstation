#include "sip_logic.h"
#include "config.h"
#include "io_relay.h"
#include <esp_log.h>

// This is a placeholder for the actual esp_sip library header
// #include "esp_sip.h"

// --- Mock SIP types for prototype ---
typedef enum {
    SIP_EVENT_REGISTERED,
    SIP_EVENT_REGISTRATION_FAILED,
    SIP_EVENT_CALL_ESTABLISHED,
    SIP_EVENT_CALL_TERMINATED,
    SIP_EVENT_DTMF_RECEIVED,
} sip_event_type_t;

typedef struct {
    char digit;
} sip_event_dtmf_t;

typedef union {
    sip_event_dtmf_t dtmf;
} sip_event_data_t;

typedef struct {
    sip_event_type_t type;
    sip_event_data_t data;
} sip_event_t;
// --- End Mock SIP types ---

static const char *TAG = "SIP_LOGIC";
static bool is_call_active = false;

// Forward declaration for the event handler
static void sip_event_handler(sip_event_t *event);

void sip_logic_init(void) {
    ESP_LOGI(TAG, "Initializing SIP client (skeleton)...");
    // In a real implementation, you would configure and start the SIP client here.
    // This involves setting up transport, credentials, and the event handler.
    /*
    sip_config_t sip_cfg = {
        .uri = "sip:" SIP_USER "@" SIP_DOMAIN,
        .username = SIP_USER,
        .password = SIP_PASS,
        .outbound_proxy = SIP_OUTBOUND,
        .event_handler = sip_event_handler,
    };
    sip_handle_t sip_handle = esp_sip_init(&sip_cfg);
    esp_sip_start(sip_handle);
    */
    ESP_LOGI(TAG, "SIP client initialization placeholder complete.");
}

void sip_logic_call(void) {
    if (sip_logic_is_call_active()) {
        ESP_LOGW(TAG, "A call is already active.");
        return;
    }
    ESP_LOGI(TAG, "Initiating call to %s (skeleton)", CONFIG_SIP_CALLEE_URI);
    // esp_sip_connect(sip_handle, SIP_CALLEE_URI);
    
    // For prototype: simulate call established
    sip_event_t event = { .type = SIP_EVENT_CALL_ESTABLISHED };
    sip_event_handler(&event);
}

void sip_logic_hangup(void) {
    if (!sip_logic_is_call_active()) {
        ESP_LOGW(TAG, "No active call to hang up.");
        return;
    }
    ESP_LOGI(TAG, "Hanging up call (skeleton).");
    // esp_sip_disconnect(sip_handle);

    // For prototype: simulate call terminated
    sip_event_t event = { .type = SIP_EVENT_CALL_TERMINATED };
    sip_event_handler(&event);
}

bool sip_logic_is_call_active(void) {
    return is_call_active;
}

// This function would be called by the esp_sip library on different events.
static void sip_event_handler(sip_event_t *event) {
    switch (event->type) {
        case SIP_EVENT_REGISTERED:
            ESP_LOGI(TAG, "SIP client registered successfully");
            break;
        case SIP_EVENT_REGISTRATION_FAILED:
            ESP_LOGE(TAG, "SIP client registration failed");
            break;
        case SIP_EVENT_CALL_ESTABLISHED:
            ESP_LOGI(TAG, "Call established");
            is_call_active = true;
            break;
        case SIP_EVENT_CALL_TERMINATED:
            ESP_LOGI(TAG, "Call terminated");
            is_call_active = false;
            break;
        case SIP_EVENT_DTMF_RECEIVED:
            ESP_LOGI(TAG, "DTMF digit received: %c", event->data.dtmf.digit);
            if (event->data.dtmf.digit == '1') {
                relay_pulse_door(DOOR_PULSE_MS);
            } else if (event->data.dtmf.digit == '2') {
                relay_toggle_light();
            }
            break;
        default:
            break;
    }
}
