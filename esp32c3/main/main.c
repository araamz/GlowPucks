#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

#define TYPE_INDEX 0
#define LABEL_INDEX 1
#define BRIGHTNESS_INDEX 2
#define ACTIVE_INDEX 3
#define MODE_INDEX 4
#define R1_INDEX 5
#define G1_INDEX 6
#define B1_INDEX 7
#define R2_INDEX 8
#define G2_INDEX 9
#define B2_INDEX 10
#define GROUP_ENABLE_INDEX 11
#define GROUP_TARGET_INDEX 12
#define CAR_CLEAR_INDEX 13
#define CAR_COUNT_INDEX 14
#define MESSAGE_FLAGS_COUNT 15
#define CONFIG_MESSAGE_TYPE (0)
#define STATUS_MESSAGE_TYPE (1)
#define BOOTSTRAP_MESSAGE_TYPE (2)
#define STRING_HOLE_FILLER ("NULL")
#define INTEGER_HOLE_FILLER (-1)
#define MESSAGE_DELIMITER ("\t")
#define FULL_MESSAGE_BYTE_SIZE (1024)
#define SEGMENT_MESSAGE_BYTE_SIZE (512)

enum modes
{
    still = 0,
    flash = 1,
    alternate = 2
};

enum communication
{
    uart = 0,
    mqtt = 1
};

static const char uuid[SEGMENT_MESSAGE_BYTE_SIZE] = "bf421b2b-27a8-4c2f-b958-30d7a9f12bfd";
static char label[SEGMENT_MESSAGE_BYTE_SIZE] = {STRING_HOLE_FILLER};
static int brightness = 100;
static int active = 1;
static enum modes mode = flash;
static int r1 = 100;
static int g1 = 0;
static int b1 = 0;
static int r2 = 0;
static int g2 = 0;
static int b2 = 0;
static int group_enable = 0;
static char group_target[SEGMENT_MESSAGE_BYTE_SIZE] = {STRING_HOLE_FILLER};
static int car_clear = 0;
static int car_count = 0;

#define WIFI_SSID ("Glowpuck Broker")
#define WIFI_PASSWORD ("F2022_EE426")
#define MAXIMUM_RETRY (15)
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
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

// 2 Read Events
// -> (MQTT) esp32c3 Config Update [1]
// -> (UART) uno Bootstrap [2]

// 2 Write Events
// -> (MQTT) backend Status [3]
// -> (UART) uno Config [4]

static const char *PROTOCOL_TAG = "PROTOCOL_TAG";
void parse_protocol_message(char *protocol_message, int *parsed_type, char **parsed_label, int *parsed_brightness, int *parsed_active, int *parsed_mode, int *parsed_r1, int *parsed_g1, int *parsed_b1, int *parsed_r2, int *parsed_g2, int *parsed_b2, int *parsed_group_enable, char **parsed_group_target, int *parsed_car_clear, int *parsed_car_count);
void write_esp32c3_status_protocol_message(char *status_protocol_message); // [3]
void write_esp32c3_config_protocol_message(char *config_protocol_message); // [4] - Be sure to reset car_clear flag after transmission
int execute_protocol_message(char *protocol_message, enum communication communication_mode);
// Remember when status gets sent to arduino, reset car_clear back to 0

static const char *DEVICE_TAG = "DEVICE_TAG";
void update_esp32c3_config(char *new_label, int new_brightness, int new_active, int new_mode, int new_r1, int new_g1, int new_b1, int new_r2, int new_g2, int new_b2, int new_group_enable, char *new_group_target, int new_car_clear); // [1]
void bootstrap_uno();
void update_car_count(int new_car_count);
void update_uno_config();

#define WIFI_STATUS_LED (GPIO_NUM_1)
#define MQTT_STATUS_LED (GPIO_NUM_0)
#define TCP_ERROR_STATUS_LED (GPIO_NUM_2)
static const char *LED_TAG = "LED_TAG";
void initalize_status_led();
void enable_status_led(int gpio_number);
void disable_status_led(int gpio_number);

static EventGroupHandle_t wifi_event_group;
static const char *WIFI_TAG = "WIFI_TAG";
static int wifi_failure_retry_count = 0;
void initalize_nvs_wifi();
void initialize_wifi();
static void wifi_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

#define MQTT_BROKER_HOST ("10.42.0.1")
#define MQTT_BROKER_PORT (25565)
static const char *MQTT_TAG = "MQTT_TAG";
static void initialize_mqtt();
static void mqtt_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

#define TXD_PIN (GPIO_NUM_19)
#define RXD_PIN (GPIO_NUM_18)
#define TXD_BUFFER_SIZE (FULL_MESSAGE_BYTE_SIZE)
#define RXD_BUFFER_SIZE (FULL_MESSAGE_BYTE_SIZE)
#define UART_QUEUE_SIZE (20)
#define UART UART_NUM_1
static QueueHandle_t uart1_queue;
static const char *UART_TAG = "UART_TAG";
void initialize_uart();
static void uart_event(void *pvParameters);

void parse_protocol_message(char *protocol_message, int *parsed_type, char **parsed_label, int *parsed_brightness, int *parsed_active, int *parsed_mode, int *parsed_r1, int *parsed_g1, int *parsed_b1, int *parsed_r2, int *parsed_g2, int *parsed_b2, int *parsed_group_enable, char **parsed_group_target, int *parsed_car_clear, int *parsed_car_count)
{

    char protocol_message_char_buffer[FULL_MESSAGE_BYTE_SIZE];
    strcpy(protocol_message_char_buffer, protocol_message);
    char *tok = strtok(protocol_message_char_buffer, MESSAGE_DELIMITER);

    for (int message_index = 0; tok != NULL; message_index++)
    {

        if (message_index == TYPE_INDEX)
        {

            *parsed_type = atoi(tok);
        }

        if (message_index == LABEL_INDEX)
        {

            *parsed_label = tok;
        }

        if (message_index == BRIGHTNESS_INDEX)
        {

            *parsed_brightness = atoi(tok);
        }

        if (message_index == ACTIVE_INDEX)
        {

            *parsed_active = atoi(tok);
        }

        if (message_index == MODE_INDEX)
        {

            *parsed_mode = atoi(tok);
        }

        if (message_index == R1_INDEX)
        {

            *parsed_r1 = atoi(tok);
        }

        if (message_index == G1_INDEX)
        {

            *parsed_g1 = atoi(tok);
        }

        if (message_index == B1_INDEX)
        {

            *parsed_b1 = atoi(tok);
        }

        if (message_index == R2_INDEX)
        {

            *parsed_r2 = atoi(tok);
        }

        if (message_index == G2_INDEX)
        {

            *parsed_g2 = atoi(tok);
        }

        if (message_index == B2_INDEX)
        {

            *parsed_b2 = atoi(tok);
        }

        if (message_index == GROUP_ENABLE_INDEX)
        {

            *parsed_group_enable = atoi(tok);
        }

        if (message_index == GROUP_TARGET_INDEX)
        {

            *parsed_group_target = tok;
        }

        if (message_index == CAR_CLEAR_INDEX)
        {

            *parsed_car_clear = atoi(tok);
        }

        if (message_index == CAR_COUNT_INDEX)
        {

            *parsed_car_count = atoi(tok);
        }

        tok = strtok(NULL, MESSAGE_DELIMITER);
    }
}
void write_esp32c3_status_protocol_message(char *status_protocol_message)
{

    int message_type = STATUS_MESSAGE_TYPE;
    char *message_label = (char *)malloc(SEGMENT_MESSAGE_BYTE_SIZE * sizeof(char));
    strlcpy(message_label, label, SEGMENT_MESSAGE_BYTE_SIZE);
    int message_brightness = brightness;
    int message_active = active;
    int message_mode = mode;
    int message_r1 = r1;
    int message_g1 = g1;
    int message_b1 = b1;
    int message_r2;
    int message_g2;
    int message_b2;
    int message_group_enable = group_enable;
    char *message_group_target = (char *)malloc(SEGMENT_MESSAGE_BYTE_SIZE * sizeof(char));
    strlcpy(message_group_target, STRING_HOLE_FILLER, SEGMENT_MESSAGE_BYTE_SIZE);
    int message_car_clear = car_clear;
    int message_car_count = INTEGER_HOLE_FILLER;

    if (mode == alternate)
    {
        message_r2 = r2;
        message_g2 = g2;
        message_b2 = b2;
    }
    else
    {
        message_r2 = INTEGER_HOLE_FILLER;
        message_g2 = INTEGER_HOLE_FILLER;
        message_b2 = INTEGER_HOLE_FILLER;
    }

    if (group_enable == 1)
    {
        strlcpy(message_group_target, group_target, SEGMENT_MESSAGE_BYTE_SIZE);
    }

    sprintf(status_protocol_message, "%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\0", message_type, message_label, message_brightness, message_active, message_mode, message_r1, message_g1, message_b1, message_r2, message_g2, message_b2, message_group_enable, message_group_target, message_car_clear, message_car_count);

    ESP_LOGI(PROTOCOL_TAG, "Written Protocol Status Message: %s.", status_protocol_message);
}
void write_esp32c3_config_protocol_message(char *config_protocol_message)
{

    int message_type = CONFIG_MESSAGE_TYPE;
    char *message_label = (char *)malloc(SEGMENT_MESSAGE_BYTE_SIZE * sizeof(char));
    strlcpy(message_label, label, SEGMENT_MESSAGE_BYTE_SIZE);
    int message_brightness = brightness;
    int message_active = active;
    int message_mode = mode;
    int message_r1 = r1;
    int message_g1 = g1;
    int message_b1 = b1;
    int message_r2;
    int message_g2;
    int message_b2;
    int message_group_enable = group_enable;
    char *message_group_target = (char *)malloc(SEGMENT_MESSAGE_BYTE_SIZE * sizeof(char));
    strlcpy(message_group_target, STRING_HOLE_FILLER, SEGMENT_MESSAGE_BYTE_SIZE);
    int message_car_clear = car_clear;
    int message_car_count = INTEGER_HOLE_FILLER;

    if (mode == alternate)
    {
        message_r2 = r2;
        message_g2 = g2;
        message_b2 = b2;
    }
    else
    {
        message_r2 = INTEGER_HOLE_FILLER;
        message_g2 = INTEGER_HOLE_FILLER;
        message_b2 = INTEGER_HOLE_FILLER;
    }

    if (group_enable == 1)
    {
        strlcpy(message_group_target, group_target, SEGMENT_MESSAGE_BYTE_SIZE);
    }

    sprintf(config_protocol_message, "%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\0", message_type, message_label, message_brightness, message_active, message_mode, message_r1, message_g1, message_b1, message_r2, message_g2, message_b2, message_group_enable, message_group_target, message_car_clear, message_car_count);

    ESP_LOGI(PROTOCOL_TAG, "Written Protocol Config Message: %s.", config_protocol_message);
}
int execute_protocol_message(char *protocol_message, enum communication communication_mode)
{

    int parsed_type;
    char *parsed_label;
    int parsed_brightness;
    int parsed_active;
    int parsed_mode;
    int parsed_r1;
    int parsed_g1;
    int parsed_b1;
    int parsed_r2;
    int parsed_g2;
    int parsed_b2;
    int parsed_group_enable;
    char *parsed_group_target;
    int parsed_car_clear;
    int parsed_car_count;

    parse_protocol_message(protocol_message, &parsed_type, &parsed_label, &parsed_brightness, &parsed_active, &parsed_mode, &parsed_r1, &parsed_g1, &parsed_b1, &parsed_r2, &parsed_g2, &parsed_b2, &parsed_group_enable, &parsed_group_target, &parsed_car_clear, &parsed_car_count);

    if ((parsed_type != CONFIG_MESSAGE_TYPE) && (parsed_type != STATUS_MESSAGE_TYPE) && (parsed_type != BOOTSTRAP_MESSAGE_TYPE))
    {
        return -1;
    }

    if (communication_mode == uart)
    {
        ESP_LOGI(PROTOCOL_TAG, "[Transport %d] Executing Message: %s.", communication_mode, protocol_message);
        ESP_LOGI(PROTOCOL_TAG, "[Transport %d] Received Message Type %d.", parsed_type, parsed_type);
        switch (parsed_type)
        {
        case STATUS_MESSAGE_TYPE:
            update_car_count(parsed_car_count);
            break;

        case BOOTSTRAP_MESSAGE_TYPE:
            update_uno_config();
            break;
        }
    }
    else
    {
    }

    return 0;
}

void update_esp32c3_config(char *new_label, int new_brightness, int new_active, int new_mode, int new_r1, int new_g1, int new_b1, int new_r2, int new_g2, int new_b2, int new_group_enable, char *new_group_target, int new_car_clear)
{

    strcpy(label, new_label);
    brightness = new_brightness;
    active = new_active;
    mode = new_mode;
    r1 = new_r1;
    g1 = new_g1;
    b1 = new_b1;
    r2 = new_r2;
    g2 = new_g2;
    b2 = new_b2;
    group_enable = new_group_enable;
    strcpy(group_target, new_group_target);
    car_clear = new_car_clear;
}
void bootstrap_uno()
{
    ESP_LOGI(DEVICE_TAG, "Bootstrapping Uno.");
    char *config_protocol_message = (char *)malloc(FULL_MESSAGE_BYTE_SIZE * sizeof(char));

    write_esp32c3_config_protocol_message(config_protocol_message);

    uart_write_bytes(UART, (char *)config_protocol_message, strlen(config_protocol_message));

    free(config_protocol_message);
}
void update_car_count(int new_car_count)
{
    car_count = new_car_count;
    ESP_LOGI(DEVICE_TAG, "Updated car_count: %d", car_count);
}
void update_uno_config()
{
    ESP_LOGI(DEVICE_TAG, "Upating Uno Config.");
    char *config_protocol_message = (char *)malloc(FULL_MESSAGE_BYTE_SIZE * sizeof(char));
    write_esp32c3_config_protocol_message(config_protocol_message);

    uart_write_bytes(UART, (const char *)config_protocol_message, strlen(config_protocol_message));
    car_clear = 0;
    free(config_protocol_message);
}

void initalize_status_led()
{
    gpio_set_direction(TCP_ERROR_STATUS_LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(WIFI_STATUS_LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(MQTT_STATUS_LED, GPIO_MODE_OUTPUT);
}
void enable_status_led(int gpio_number)
{
    gpio_set_level(gpio_number, 1);
    ESP_LOGI(LED_TAG, "GPIO[%d] Enabling LED.", gpio_number);
}
void disable_status_led(int gpio_number)
{
    gpio_set_level(gpio_number, 0);
    ESP_LOGI(LED_TAG, "GPIO[%d] Disabling LED.", gpio_number);
}

void initalize_nvs_wifi()
{

    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {

        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
}
void initialize_wifi()
{

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

    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(WIFI_TAG, "connected to ap SSID:%s password:%s", WIFI_SSID, WIFI_PASSWORD);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(WIFI_TAG, "Failed to connect to SSID:%s, password:%s", WIFI_SSID, WIFI_PASSWORD);
    }
    else
    {
        ESP_LOGE(WIFI_TAG, "UNEXPECTED EVENT");
    }
}
static void wifi_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (wifi_failure_retry_count > MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            wifi_failure_retry_count++;
        }
        else
        {
            enable_status_led(TCP_ERROR_STATUS_LED);
            disable_status_led(WIFI_STATUS_LED);
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        enable_status_led(WIFI_STATUS_LED);
        disable_status_led(TCP_ERROR_STATUS_LED);
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(WIFI_TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
        wifi_failure_retry_count = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void initialize_mqtt()
{

    esp_mqtt_client_config_t mqtt_config = {
        .host = MQTT_BROKER_HOST,
        .port = MQTT_BROKER_PORT};
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, &mqtt_event, NULL);
    esp_mqtt_client_start(client);
}
static void mqtt_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{

    ESP_LOGD(MQTT_TAG, "Seeing new event on stack.");
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int message_id;

    switch ((esp_mqtt_event_id_t)event_id)
    {

    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(MQTT_TAG, "MQTT Connected.");
        enable_status_led(MQTT_STATUS_LED);
        disable_status_led(TCP_ERROR_STATUS_LED);

        ESP_LOGI(MQTT_TAG, "%s", uuid);
        ESP_LOGI(MQTT_TAG, "car_count %d", car_count);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(MQTT_TAG, "MQTT Disconnected.");
        enable_status_led(TCP_ERROR_STATUS_LED);
        disable_status_led(MQTT_STATUS_LED);
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

void initialize_uart()
{

    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

    uart_driver_install(UART, RXD_BUFFER_SIZE, TXD_BUFFER_SIZE, UART_QUEUE_SIZE, &uart1_queue, 0);
    uart_param_config(UART, &uart_config);
    uart_set_pin(UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    esp_log_level_set(UART_TAG, ESP_LOG_INFO);
}
static void uart_event(void *pvParameters)
{

    uart_event_t event;
    uint8_t *data_dump = (uint8_t *)malloc(RXD_BUFFER_SIZE);

    while (true)
    {

        if (xQueueReceive(uart1_queue, (void *)&event, (TickType_t)portMAX_DELAY))
        {

            bzero(data_dump, RXD_BUFFER_SIZE);
            ESP_LOGI(UART_TAG, "UART[%d] Seeing new event on stack.", UART);

            switch (event.type)
            {

            case UART_DATA:

                ESP_LOGI(UART_TAG, "UART[%d] Reading %d bytes.", UART, event.size);
                uart_read_bytes(UART, data_dump, event.size, portMAX_DELAY);
                ESP_LOGI(UART_TAG, "UART[%d] Seeing string: '%s'.", UART, (char *)data_dump);

                if (execute_protocol_message((char *)data_dump, uart) == -1)
                {
                    ESP_LOGE(UART_TAG, "UART [%d] Seeing rejected message: %s", UART, (char *)data_dump);
                }
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

void app_main(void)
{
    initalize_status_led();

    initalize_nvs_wifi();

    initialize_wifi();

    initialize_mqtt();

    initialize_uart();
    xTaskCreate(uart_event, "uart_event", 2048, NULL, 12, NULL);

    esp_log_level_set(UART_TAG, ESP_LOG_INFO);
}