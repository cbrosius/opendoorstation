#include "web_server.h"
#include "config.h"
#include <esp_http_server.h>
#include <esp_log.h>
#include <cJSON.h>

static const char *TAG = "WEB_SERVER";
static httpd_handle_t server = NULL;

// In a real application, you would use NVS (Non-Volatile Storage)
// to store and retrieve these settings.
// For this prototype, we'll just use the values from config.h
// and won't actually save any changes.

/*
 * Handler to get current configuration
 */
static esp_err_t config_get_handler(httpd_req_t *req)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "wifi_ssid", WIFI_SSID);
    cJSON_AddStringToObject(root, "sip_user", SIP_USER);
    cJSON_AddStringToObject(root, "sip_domain", SIP_DOMAIN);
    cJSON_AddStringToObject(root, "sip_callee_uri", SIP_CALLEE_URI);

    const char *sys_info = cJSON_Print(root);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, sys_info, strlen(sys_info));

    free((void *)sys_info);
    cJSON_Delete(root);

    return ESP_OK;
}

/*
 * Handler to update configuration
 */
static esp_err_t config_post_handler(httpd_req_t *req)
{
    char buf[1024]; // Increased buffer size
    int ret, remaining = req->content_len;

    if (remaining >= sizeof(buf)) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Request content too long");
        return ESP_FAIL;
    }

    // Read the entire request content
    ret = httpd_req_recv(req, buf, remaining);
    if (ret <= 0) { 
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    buf[ret] = '\0';

    cJSON *root = cJSON_Parse(buf);
    if (root == NULL) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Failed to parse JSON");
        return ESP_FAIL;
    }

    // NOTE: In a real app, you would parse the values here and save them to NVS.
    // Example: const char *ssid = cJSON_GetObjectItem(root, "wifi_ssid")->valuestring;
    // ESP_LOGI(TAG, "Received new SSID: %s", ssid);

    cJSON_Delete(root);

    httpd_resp_send(req, "Configuration updated successfully", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


/*
 * Handler to serve the main index.html page
 */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    extern const unsigned char index_html_start[] asm("_binary_index_html_start");
    extern const unsigned char index_html_end[]   asm("_binary_index_html_end");
    const size_t index_html_size = (index_html_end - index_html_start);
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start, index_html_size);
    return ESP_OK;
}

/*
 * Handler to serve style.css
 */
static esp_err_t css_get_handler(httpd_req_t *req)
{
    extern const unsigned char style_css_start[] asm("_binary_style_css_start");
    extern const unsigned char style_css_end[]   asm("_binary_style_css_end");
    const size_t style_css_size = (style_css_end - style_css_start);
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)style_css_start, style_css_size);
    return ESP_OK;
}

/*
 * Handler to serve script.js
 */
static esp_err_t js_get_handler(httpd_req_t *req)
{
    extern const unsigned char script_js_start[] asm("_binary_script_js_start");
    extern const unsigned char script_js_end[]   asm("_binary_script_js_end");
    const size_t script_js_size = (script_js_end - script_js_start);
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)script_js_start, script_js_size);
    return ESP_OK;
}


void start_webserver(void)
{
    if (server != NULL) {
        return;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "Starting httpd...");
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = root_get_handler,
        };
        httpd_register_uri_handler(server, &root_uri);

        httpd_uri_t css_uri = {
            .uri       = "/style.css",
            .method    = HTTP_GET,
            .handler   = css_get_handler,
        };
        httpd_register_uri_handler(server, &css_uri);

        httpd_uri_t js_uri = {
            .uri       = "/script.js",
            .method    = HTTP_GET,
            .handler   = js_get_handler,
        };
        httpd_register_uri_handler(server, &js_uri);

        httpd_uri_t config_get_uri = {
            .uri       = "/api/config",
            .method    = HTTP_GET,
            .handler   = config_get_handler,
        };
        httpd_register_uri_handler(server, &config_get_uri);

        httpd_uri_t config_post_uri = {
            .uri       = "/api/config",
            .method    = HTTP_POST,
            .handler   = config_post_handler,
        };
        httpd_register_uri_handler(server, &config_post_uri);
    }
}

void stop_webserver(void)
{
    if (server != NULL) {
        httpd_stop(server);
        server = NULL;
    }
}
