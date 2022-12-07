/*
  2 Interrupt Handlers 
  -> 1 for Data Receive from UART
  -> 1 for Rising/Falling Edges from PIR Motion Sensor
  1 UART Hamdlers
  -> 1 for UART Receive for Config/Status Requests/Responses
  1 EPROM Handlers 
  -> 1 for Saving of car_count

  System Start Call
  -> Get All necessary values from esp32c3 via UART
  -> If all values come back "good," stop the call and begin watching for normal events, else rerun the call
*/
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include <String.h>
#include <stdlib.h>

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
#define CAR_COUNT_INDEX 13
void parse_protocol_string(String protocol_message);
void config_protocol_handler();
void status_protocol_handler();
void update_uno_config_values();
void generate_status_response();
void generate_config_bootup_request();

void initialize_uart();
void uart_event();

#define EEPROM_MEMORY_ADDRESS 0
void initialize_eeprom();
void eeprom_increase_car_count();
void eeprom_clear_clear_count();

#define PIR_MOTION_SENSOR_STATUS_LED LED_BUILTIN
void initialize_pir_motion_sensor_status_led();
void enable_status_led(int gpio_number);
void disable_status_led(int gpio_number);

#define PIR_MOTION_SENSOR 3
void initialize_pir_motion_sensor();
void pir_motion_sensor_event();

#define NEOPIXEL 8
#define NEOPIXEL_COUNT 16
#define NEOPIXEL_FLASH_DELAY 1000
void initialize_neopixel_ring();
void drive_neopixel_ring();
void configure_still_mode_neopixel_ring();
void configure_flash_mode_neopixel_ring();
void configure_alternate_mode_neopixel_ring();
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL, NEO_GRB + NEO_KHZ800);

enum modes { still = 0, flash = 1, alternate = 2 };
volatile int brightness = 0;
volatile bool active = false;
volatile enum modes mode = still;
volatile int r1 = 0;
volatile int g1 = 0;
volatile int b1 = 0;
volatile int r2 = 0;
volatile int g2 = 0;
volatile int b2 = 0; 
volatile int car_count = 0;

volatile bool inital_bootup_complete = false;

void parse_protocol_string(String protocol_message) {

  char* tokens = tokens = strtok(protocol_message, "\t");

  if (tokens[TYPE_INDEX] == "config") {

    String protocol_message_label = tokens[LABEL_INDEX]; 
    int protocol_message_brightness = atoi(tokens[BRIGHTNESS_INDEX]); 
    int protocol_message_active = atoi(tokens[ACTIVE_INDEX]); 
    int protocol_message_mode = atoi(tokens[MODE_INDEX]); 
    int protocol_message_r1 = atoi(tokens[R1_INDEX]); 
    int protocol_message_g1 = atoi(tokens[G1_INDEX]); 
    int protocol_message_b1 = atoi(tokens[B1_INDEX]); 
    int protocol_message_r2 = atoi(tokens[R2_INDEX]); 
    int protocol_message_g2 = atoi(tokens[G2_INDEX]); 
    int protocol_message_b2 = atoi(tokens[B2_INDEX]);  
    int protocol_message_group_enable = tokens[LABEL_INDEX]; 
    int protocol_message_group_target = tokens[LABEL_INDEX]; 
    int protocol_message_car_clear = atoi(tokens[LABEL_INDEX]); 

  } else if (tokens[TYPE_INDEX]) == "status") {

  } else {

  }

}
void update_uno_config_values() {
  
}

void initialize_uart() {
  Serial.begin(9600);
}
void uart_event() {
  
  if (Serial.available()) {

    String t_string = Serial.readString();

      Serial.print(t_string);
  }

}

void initialize_eeprom() {

  int eeprom_car_count_value = EEPROM.read(EEPROM_MEMORY_ADDRESS);

  if (eeprom_car_count_value > 0) {

    car_count = eeprom_car_count_value;

  }

}
void eeprom_increase_car_count() {

  car_count = car_count + 1;
  EEPROM.write(EEPROM_MEMORY_ADDRESS, car_count);

}
void eeprom_clear_clear_count() {

  EEPROM.write(EEPROM_MEMORY_ADDRESS, 0);

}

void initialize_pir_motion_sensor_status() {

  pinMode(PIR_MOTION_SENSOR_STATUS_LED, OUTPUT);

}
void enable_status_led(int gpio_number) {

  digitalWrite(gpio_number, HIGH);

}
void disable_status_led(int gpio_number) {

  digitalWrite(gpio_number, LOW);

}

void initialize_pir_motion_sensor() {

  pinMode(PIR_MOTION_SENSOR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIR_MOTION_SENSOR), pir_motion_sensor_event, CHANGE);

}
void pir_motion_sensor_event() {

  bool pir_motion_sensor_status = digitalRead(PIR_MOTION_SENSOR);

  if (pir_motion_sensor_status == HIGH) {

    enable_status_led(PIR_MOTION_SENSOR_STATUS_LED);
    eeprom_increase_car_count();

  } else if (pir_motion_sensor_status == LOW ) {

    disable_status_led(PIR_MOTION_SENSOR_STATUS_LED);

  }

}

void initialize_neopixel_ring() {

  pixels.begin();
  pixels.clear();

}
void drive_neopixel_ring() {

  if (active) {

    pixels.setBrightness(brightness);

    switch (mode) {

      case still:

        configure_still_mode_neopixel_ring();
        break;

      case flash:

        configure_flash_mode_neopixel_ring();
        break;
      
      case alternate:

        configure_alternate_mode_neopixel_ring();
        break;

    }

  } else {

    pixels.clear();
    pixels.show();

  }

}
void configure_still_mode_neopixel_ring() {

  for (int pixel_index = 0; pixel_index < NEOPIXEL_COUNT; pixel_index++) {

    pixels.setPixelColor(pixel_index, pixels.Color(r1, g1, b1));
    pixels.show();

  }

}
void configure_flash_mode_neopixel_ring() {

  delay(NEOPIXEL_FLASH_DELAY);

  for (int pixel_index = 0; pixel_index < NEOPIXEL_COUNT; pixel_index++) {

    pixels.setPixelColor(pixel_index, pixels.Color(r1, g1, b1));
    pixels.show();

  }

  delay(NEOPIXEL_FLASH_DELAY);

  pixels.clear();
  pixels.show();

}
void configure_alternate_mode_neopixel_ring() {

  delay(NEOPIXEL_FLASH_DELAY);

  for (int pixel_index = 0; pixel_index < NEOPIXEL_COUNT; pixel_index++) {

    pixels.setPixelColor(pixel_index, pixels.Color(r1, g1, b1));
    pixels.show();

  }

  delay(NEOPIXEL_FLASH_DELAY);

  for (int pixel_index = 0; pixel_index < NEOPIXEL_COUNT; pixel_index++) {

    pixels.setPixelColor(pixel_index, pixels.Color(r2, g2, b2));
    pixels.show();

  }

}

void setup() {

  initialize_uart();

  initialize_eeprom();

  initialize_pir_motion_sensor_status();
  initialize_pir_motion_sensor();

  initialize_neopixel_ring();

}

void loop() {

  drive_neopixel_ring();
  uart_event();

}
