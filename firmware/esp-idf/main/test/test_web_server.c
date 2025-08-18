#include "unity.h"
#include "cmock.h"
#include "mock_esp_http_server.h"
#include "mock_io_relay.h"
#include "mock_app_main.h"

#include "web_server.h"

void setUp(void) {}

void tearDown(void) {}

void test_start_webserver_should_start_httpd_and_register_handlers(void) {
    // Given
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = (httpd_handle_t)1;

    httpd_start_ExpectAndReturn(&server, &config, ESP_OK);
    httpd_register_uri_handler_ExpectAnyArgsAndReturn(ESP_OK);
    httpd_register_uri_handler_ExpectAnyArgsAndReturn(ESP_OK);
    httpd_register_uri_handler_ExpectAnyArgsAndReturn(ESP_OK);
    httpd_register_uri_handler_ExpectAnyArgsAndReturn(ESP_OK);
    httpd_register_uri_handler_ExpectAnyArgsAndReturn(ESP_OK);
    httpd_register_uri_handler_ExpectAnyArgsAndReturn(ESP_OK);

    // When
    start_webserver();

    // Then
    // The expectations set above are the assertions.
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_start_webserver_should_start_httpd_and_register_handlers);
    return UNITY_END();
}
