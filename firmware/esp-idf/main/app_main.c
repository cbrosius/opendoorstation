#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "app_main.h"

#include "config.h"
#include "io_button.h"
#include "io_relay.h"
#include "sip_logic.h"
#include "web_server.h"

static const char *TAG = "APP_MAIN";

// Forward declarations
static void wifi_init_sta(void);


// Event handler for Wi-Fi and IP events
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    wifi_event_sta_disconnected_t* event = (wifi_event_sta_disconnected_t*) event_data;
    ESP_LOGI(TAG, "Disconnected from Wi-Fi. Reason: %d. Reconnecting...", event->reason);
    esp_wifi_connect();
    }
}

static void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP address: " IPSTR, IP2STR(&event->ip_info.ip));

        // Start network-dependent services
        ESP_LOGI(TAG, "Starting web server...");
        start_webserver();
        
        ESP_LOGI(TAG, "Initializing SIP logic...");
        sip_logic_init();
    }
}

void app_main(void)
{
    // Initialize NVS (Non-Volatile Storage)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Initializing hardware...");
    io_relays_init(PIN_RELAY_DOOR, PIN_RELAY_LIGHT);
    io_button_init(PIN_BTN, button_pressed_callback);

    ESP_LOGI(TAG, "Initializing networking...");
    wifi_init_sta();

    // Debugging: Print configured SSID
    ESP_LOGI(TAG, "Configured SSID: %s", WIFI_SSID);

    // Keep the main task alive
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void wifi_init_sta(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register event handlers
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");
}

void button_pressed_callback(void) {
    ESP_LOGI(TAG, "Button pressed.");
    if (sip_logic_is_call_active()) {
        sip_logic_hangup();
    } else {
        sip_logic_call();
    }
}
