#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include <String.h>

enum modes
{
  still = 0,
  flash = 1,
  alternate = 2
};
volatile int brightness = 0;
volatile bool active = 0;
volatile enum modes mode = still;
volatile int r1 = 0;
volatile int g1 = 0;
volatile int b1 = 0;
volatile int r2 = 0;
volatile int g2 = 0;
volatile int b2 = 0;
volatile int car_count = 0;

volatile bool bootstrap_complete = 0;

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
#define CONFIG_MESSAGE_TYPE 0
#define STATUS_MESSAGE_TYPE 1
#define BOOTSTRAP_MESSAGE_TYPE 2
#define STRING_HOLE_FILLER ("NULL")
#define INTEGER_HOLE_FILLER (-1)
#define MESSAGE_DELIMITER ("\t")

void parse_protocol_message(String protocol_message, int &parsed_type, String &parsed_label, int &parsed_brightness, int &parsed_active, int &parsed_mode, int &parsed_r1, int &parsed_g1, int &parsed_b1, int &parsed_r2, int &parsed_g2, int &parsed_b2, int &parsed_group_enable, String &parsed_group_target, int &parsed_car_clear, int &parsed_car_count);
String write_uno_bootstrap_protocol_message();
String write_uno_status_protocol_message();

void execute_protocol_message(String protocol_message);
void wait_for_uno_bootstrapping();
void update_uno_config(int new_brightness, int new_active, enum modes new_mode, int new_r1, int new_g1, int new_b1, int new_r2, int new_g2, int new_b2, int new_car_clear);

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

void parse_protocol_message(String protocol_message, int &parsed_type, String &parsed_label, int &parsed_brightness, int &parsed_active, int &parsed_mode, int &parsed_r1, int &parsed_g1, int &parsed_b1, int &parsed_r2, int &parsed_g2, int &parsed_b2, int &parsed_group_enable, String &parsed_group_target, int &parsed_car_clear, int &parsed_car_count)
{

  char protocol_message_char_buffer[1024];
  strcpy(protocol_message_char_buffer, protocol_message.c_str());
  char *char_buffer_pointer = strtok(protocol_message_char_buffer, MESSAGE_DELIMITER);

  for (int message_index = 0; char_buffer_pointer != NULL; message_index++)
  {

    if (message_index == TYPE_INDEX)
    {
      parsed_type = atoi(char_buffer_pointer);
    }

    if (message_index == LABEL_INDEX)
    {
      parsed_label = String(char_buffer_pointer);
    }

    if (message_index == BRIGHTNESS_INDEX)
    {
      parsed_brightness = atoi(char_buffer_pointer);
    }

    if (message_index == ACTIVE_INDEX)
    {
      parsed_active = atoi(char_buffer_pointer);
    }

    if (message_index == MODE_INDEX)
    {
      parsed_mode = atoi(char_buffer_pointer);
    }

    if (message_index == R1_INDEX)
    {
      parsed_r1 = atoi(char_buffer_pointer);
    }

    if (message_index == G1_INDEX)
    {
      parsed_g1 = atoi(char_buffer_pointer);
    }
    if (message_index == B1_INDEX)
    {
      parsed_b1 = atoi(char_buffer_pointer);
    }
    if (message_index == R2_INDEX)
    {
      parsed_r2 = atoi(char_buffer_pointer);
    }
    if (message_index == G2_INDEX)
    {
      parsed_g2 = atoi(char_buffer_pointer);
    }

    if (message_index == B2_INDEX)
    {
      parsed_b2 = atoi(char_buffer_pointer);
    }

    if (message_index == GROUP_ENABLE_INDEX)
    {
      parsed_group_enable = atoi(char_buffer_pointer);
    }

    if (message_index == GROUP_TARGET_INDEX)
    {
      parsed_group_target = String(char_buffer_pointer);
    }

    if (message_index == CAR_CLEAR_INDEX)
    {
      parsed_car_clear = atoi(char_buffer_pointer);
    }

    if (message_index == CAR_COUNT_INDEX)
    {
      parsed_car_count = atoi(char_buffer_pointer);
    }

    char_buffer_pointer = strtok(NULL, MESSAGE_DELIMITER);
  }
}

String write_uno_bootstrap_protocol_message()
{

  String protocol_message = "";

  for (int message_index = 0; message_index < MESSAGE_FLAGS_COUNT; message_index++)
  {

    if (message_index == TYPE_INDEX)
    {

      String message_segment = String(BOOTSTRAP_MESSAGE_TYPE) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == LABEL_INDEX)
    {

      String message_segment = String(STRING_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == BRIGHTNESS_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == ACTIVE_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == MODE_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == R1_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == G1_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == B1_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == R2_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == G2_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == B2_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == GROUP_ENABLE_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == GROUP_TARGET_INDEX)
    {

      String message_segment = String(STRING_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == CAR_CLEAR_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == CAR_COUNT_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER);
      protocol_message = protocol_message + message_segment;
    }
  }

  return protocol_message;
}

String write_uno_status_protocol_message()
{

  String protocol_message = "";

  for (int message_index = 0; message_index < MESSAGE_FLAGS_COUNT; message_index++)
  {

    if (message_index == TYPE_INDEX)
    {

      String message_segment = String(STATUS_MESSAGE_TYPE) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == LABEL_INDEX)
    {

      String message_segment = String(STRING_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == BRIGHTNESS_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == ACTIVE_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == MODE_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == R1_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == G1_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == B1_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == R2_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == G2_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == B2_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == GROUP_ENABLE_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == GROUP_TARGET_INDEX)
    {

      String message_segment = String(STRING_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == CAR_CLEAR_INDEX)
    {

      String message_segment = String(INTEGER_HOLE_FILLER) + MESSAGE_DELIMITER;
      protocol_message = protocol_message + message_segment;
    }

    if (message_index == CAR_COUNT_INDEX)
    {

      String message_segment = String(car_count);
      protocol_message = protocol_message + message_segment;
    }
    
  }

  return protocol_message;
}

void execute_protocol_message(String protocol_message)
{

  int parsed_type;
  String parsed_label;
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
  String parsed_group_target;
  int parsed_car_clear;
  int parsed_car_count;

  parse_protocol_message(protocol_message, parsed_type, parsed_label, parsed_brightness, parsed_active, parsed_mode, parsed_r1, parsed_g1, parsed_b1, parsed_r2, parsed_g2, parsed_b2, parsed_group_enable, parsed_group_target, parsed_car_clear, parsed_car_count);

  switch (parsed_type)
  {

  case CONFIG_MESSAGE_TYPE:

    update_uno_config(parsed_brightness, parsed_active, parsed_mode, parsed_r1, parsed_g1, parsed_b1, parsed_r2, parsed_g2, parsed_b2, parsed_car_clear);

    break;

  case STATUS_MESSAGE_TYPE:

    String status_message = write_uno_status_protocol_message().c_str();
    Serial.print(status_message);

    break;
  }
}

void wait_for_uno_bootstrapping()
{

  while (bootstrap_complete == 0)
  {

    String bootstrap_message = write_uno_bootstrap_protocol_message().c_str();
    Serial.print(bootstrap_message);
    delay(2000);

    if (Serial.available())
    {

      String protocol_message = Serial.readString();
      execute_protocol_message(protocol_message);
    }
  }
}

void update_uno_config(int new_brightness, int new_active, enum modes new_mode, int new_r1, int new_g1, int new_b1, int new_r2, int new_g2, int new_b2, int new_car_clear)
{

  bootstrap_complete = 1;

  brightness = new_brightness;
  active = new_active;
  mode = new_mode;
  r1 = new_r1;
  g1 = new_g1;
  b1 = new_b1;
  r2 = new_r2;
  g2 = new_g2;
  b2 = new_b2;

  if (new_car_clear == 1)
  {

    eeprom_clear_clear_count();
  }
}

void initialize_uart()
{
  Serial.begin(9600);
}
void uart_event()
{

  if (Serial.available())
  {

    String protocol_message = Serial.readString();
    execute_protocol_message(protocol_message);
  }
}

void initialize_eeprom()
{

  int eeprom_car_count_value = EEPROM.read(EEPROM_MEMORY_ADDRESS);
  car_count = eeprom_car_count_value;
}
void eeprom_increase_car_count()
{

  car_count = car_count + 1;
  EEPROM.write(EEPROM_MEMORY_ADDRESS, car_count);
}
void eeprom_clear_clear_count()
{

  EEPROM.write(EEPROM_MEMORY_ADDRESS, 0);
}

void initialize_pir_motion_sensor_status()
{

  pinMode(PIR_MOTION_SENSOR_STATUS_LED, OUTPUT);
}
void enable_status_led(int gpio_number)
{

  digitalWrite(gpio_number, HIGH);
}
void disable_status_led(int gpio_number)
{

  digitalWrite(gpio_number, LOW);
}

void initialize_pir_motion_sensor()
{

  pinMode(PIR_MOTION_SENSOR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIR_MOTION_SENSOR), pir_motion_sensor_event, CHANGE);
}
void pir_motion_sensor_event()
{

  bool pir_motion_sensor_status = digitalRead(PIR_MOTION_SENSOR);

  if (pir_motion_sensor_status == HIGH)
  {

    enable_status_led(PIR_MOTION_SENSOR_STATUS_LED);
    eeprom_increase_car_count();
  }
  else if (pir_motion_sensor_status == LOW)
  {

    disable_status_led(PIR_MOTION_SENSOR_STATUS_LED);
  }
}

void initialize_neopixel_ring()
{

  pixels.begin();
  pixels.clear();
  pixels.show();
}
void drive_neopixel_ring()
{

  if (active == 1)
  {

    pixels.setBrightness(brightness);

    switch (mode)
    {

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
  }
  else
  {

    pixels.clear();
    pixels.show();
  }
}
void configure_still_mode_neopixel_ring()
{

  for (int pixel_index = 0; pixel_index < NEOPIXEL_COUNT; pixel_index++)
  {

    pixels.setPixelColor(pixel_index, pixels.Color(r1, g1, b1));
    pixels.show();
  }
}
void configure_flash_mode_neopixel_ring()
{

  delay(NEOPIXEL_FLASH_DELAY);

  for (int pixel_index = 0; pixel_index < NEOPIXEL_COUNT; pixel_index++)
  {

    pixels.setPixelColor(pixel_index, pixels.Color(r1, g1, b1));
    pixels.show();
  }

  delay(NEOPIXEL_FLASH_DELAY);

  pixels.clear();
  pixels.show();
}
void configure_alternate_mode_neopixel_ring()
{

  delay(NEOPIXEL_FLASH_DELAY);

  for (int pixel_index = 0; pixel_index < NEOPIXEL_COUNT; pixel_index++)
  {

    pixels.setPixelColor(pixel_index, pixels.Color(r1, g1, b1));
    pixels.show();
  }

  delay(NEOPIXEL_FLASH_DELAY);

  for (int pixel_index = 0; pixel_index < NEOPIXEL_COUNT; pixel_index++)
  {

    pixels.setPixelColor(pixel_index, pixels.Color(r2, g2, b2));
    pixels.show();
  }
}

void setup()
{

  initialize_uart();

  initialize_eeprom();

  initialize_pir_motion_sensor_status();
  initialize_pir_motion_sensor();

  initialize_neopixel_ring();

  wait_for_uno_bootstrapping();
}

void loop()
{

  drive_neopixel_ring();
  uart_event();
}
