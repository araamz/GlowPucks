#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "mqtt_client.h"
#include "led_strip.h"

// Initialize RGB (for TCP connected/disconnected check)
// Initialize TCP == C:SETUP/OPERATION
// Initialize MQTT 
// Initialize UART == C:SETUP/OPERATION
// * Make Interrupt for: protocol receive of config update
// * Make Interrupt for: protocol receive of status update [type=status car_count]  ** FIRST ACT **
// * Make Interrupt for: protocol transmit of status update [type=status car_count] ** SECOND ACT **
// Initialize EPROM (NVS)
// * When updating config:
// * -> Construct data with new configuration
// * -> Flash constructed data into EPROM

//#define WIFI_SSID ("mqtt")
//#define WIFI_PASSWORD ("v9Qad9frLg")

#define WIFI_SSID ("Cyrus")
#define WIFI_PASSWORD ("pnq5u3s597")
#define MAXIMUM_RETRY  (15)
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif


#define LED_PIN (GPIO_NUM_6)
static struct led_color_t led_strip_buf_1[1];
static struct led_color_t led_strip_buf_2[1];
void initalize_rgb_led();

static EventGroupHandle_t wifi_event_group;
static const char *WIFI_TAG = "WIFI_TAG";
static int wifi_failure_retry_count = 0;
void initalize_nvs_wifi();
void initialize_wifi();
static void wifi_event(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

#define MQTT_BROKER_HOST ("192.168.1.75")
#define MQTT_BROKER_PORT (25565)
static const char *MQTT_TAG = "MQTT_TAG";
static void initialize_mqtt();
static void mqtt_event(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

#define TXD_PIN (GPIO_NUM_19)
#define RXD_PIN (GPIO_NUM_18)
#define TXD_BUFFER_SIZE (1024)
#define RXD_BUFFER_SIZE (1024)
#define UART_QUEUE_SIZE (20)
#define UART UART_NUM_1
static QueueHandle_t uart1_queue;
static const char *UART_TAG = "UART_TAG";
void initialize_uart();
static void uart_event(void *pvParameters);

void initalize_rgb_led() {

    pStrip_a = led_strip_init(CONFIG_BLINK_LED_RMT_CHANNEL, BLINK_GPIO, 1);
    pStrip_a->clear(pStrip_a, 50);
}

}

void initalize_nvs_wifi() {

    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {

        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();

    }

}
void initialize_wifi() {

    wifi_event_group = xEventGroupCreate();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_init_config);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event, NULL, &instance_got_ip);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WEP,
            .sae_pwe_h2e = WPA3_SAE_PWE_UNSPECIFIED,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(WIFI_TAG, "connected to ap SSID:%s password:%s", WIFI_SSID, WIFI_PASSWORD);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(WIFI_TAG, "Failed to connect to SSID:%s, password:%s", WIFI_SSID, WIFI_PASSWORD);
    } else {
        ESP_LOGE(WIFI_TAG, "UNEXPECTED EVENT");
    }

}
static void wifi_event(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (wifi_failure_retry_count > MAXIMUM_RETRY) {
            esp_wifi_connect();
            wifi_failure_retry_count++;
        } else {
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(WIFI_TAG, "Got IP:" IPSTR, IP2STR(&event -> ip_info.ip));
        wifi_failure_retry_count = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void initialize_mqtt() {

    esp_mqtt_client_config_t mqtt_config = {
        .host = MQTT_BROKER_HOST,
        .port = MQTT_BROKER_PORT
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, &mqtt_event, NULL);
    esp_mqtt_client_start(client);


}
static void mqtt_event(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {

    ESP_LOGD(MQTT_TAG, "Event dispatched from event loop base=%s, event_id=%d", event_base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event -> client;
    int message_id;

    switch ((esp_mqtt_event_id_t) event_id) {

        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(MQTT_TAG, "MQTT Connected");
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(MQTT_TAG, "MQTT Disconnected");
            break;

        case MQTT_EVENT_SUBSCRIBED:
        
            break;

        case MQTT_EVENT_UNSUBSCRIBED:

            break;

        case MQTT_EVENT_PUBLISHED:

            break;

        case MQTT_EVENT_DATA:

            break;

        case MQTT_EVENT_ERROR:

            break;

        default:

            break;

    }
}

void initialize_uart() {

    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_driver_install(UART, RXD_BUFFER_SIZE, TXD_BUFFER_SIZE, UART_QUEUE_SIZE, &uart1_queue, 0);
    uart_param_config(UART, &uart_config);
    uart_set_pin(UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    esp_log_level_set(UART_TAG, ESP_LOG_INFO);

}
static void uart_event(void *pvParameters) {

    uart_event_t event;
    uint8_t* data_dump = (uint8_t*) malloc(RXD_BUFFER_SIZE);

    while (true) {

        if (xQueueReceive(uart1_queue, (void*)&event, (TickType_t)portMAX_DELAY)) {

            bzero(data_dump, RXD_BUFFER_SIZE);
            ESP_LOGI(UART_TAG, "UART[%d] Seeing new event on stack.", UART);
            
            switch (event.type) {

                case UART_DATA:
                    ESP_LOGI(UART_TAG, "UART[%d] Reading %d bytes.", UART, event.size);
                    uart_read_bytes(UART, data_dump, event.size, portMAX_DELAY);
                    ESP_LOGI(UART_TAG, "UART[%d] Seeing string '%s'.", UART, (char*) data_dump);
                    uart_write_bytes(UART, (const char*) data_dump, event.size);
                    break;

                default:
                    ESP_LOGI(UART_TAG, "UART[%d] Event Type: %d", UART, event.type);
                    break;

            }

        }

    }

    free(data_dump);
    data_dump = NULL;
    vTaskDelete(NULL);

}

void app_main(void) {

    initalize_rgb_led();

    initalize_nvs_wifi();

    initialize_wifi();

    initialize_mqtt();

    initialize_uart();
    xTaskCreate(uart_event, "uart_event", 2048, NULL, 12, NULL);

    esp_log_level_set(UART_TAG, ESP_LOG_INFO);
bool led_strip_set_pixel_color(struct led_strip_t *led_strip, uint32_t pixel_num, struct led_color_t *color);
bool led_strip_set_pixel_rgb(struct led_strip_t *led_strip, uint32_t pixel_num, uint8_t red, uint8_t green, uint8_t blue);

}