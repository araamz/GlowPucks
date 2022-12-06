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

#define PIR_MOTION_SENSOR_STATUS_LED LED_BUILTIN
void initialize_pir_motion_sensor_status_led();
void enable_status_led(int gpio_number);
void disable_status_led(int gpio_number);

#define PIR_MOTION_SENSOR 3
void initialize_pir_motion_sensor();
void pir_motion_sensor_event();

#define NEOPIXEL 8
#define NEOPIXEL_COUNT 16
void initialize_neopixel_ring();
void drive_neopixel_ring();
void drive_still_mode_neopixel_ring();
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL, NEO_GRB + NEO_KHZ800);
enum modes { still, flash, alternate };

volatile int brightness = 0;
volatile bool active = false;
volatile enum modes mode = still;
volatile int r1 = 0;
volatile int g1 = 0;
volatile int b1 = 0;
volatile int r2 = 0;
volatile int g2 = 0;
volatile int b2 = 0; 

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
  } else if (pir_motion_sensor_status == LOW ) {
    disable_status_led(PIR_MOTION_SENSOR_STATUS_LED);
  }

}

void initialize_neopixel_ring() {
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(1);
}
void drive_neopixel_ring() {

  switch (mode) {

    case still:

      drive_still_mode_neopixel_ring();
      break;

    case flash:

      break;
    
    case alternate:

      break;
  }

}
void configure_still_mode_neopixel_ring(int r, int g, int b) {

  for (int pixel_index = 0; pixel_index < NEOPIXEL_COUNT; pixel_index++) {

    pixels.setPixelColor(pixel_index, pixels.Color(255, 255, 255));
    pixels.show();

  }

}
void configure_flash_mode_neopixel_ring(int r, int g, int b, int brightness) {

  for (int pixel_index = 0; pixel_index < NEOPIXEL_COUNT; pixel_index++) {

    pixels.setPixelColor(pixel_index, pixels.Color(255, 255, 255));
    pixels.show();

  }

}
void configure_alternate_mode_neopixel_ring(int r, int g, int b, int brightness) {

  for (int pixel_index = 0; pixel_index < NEOPIXEL_COUNT; pixel_index++) {

    pixels.setPixelColor(pixel_index, pixels.Color(255, 255, 255));
    pixels.show();

  }

}
// Functions with lighting mode_updates

void setup() {
  // put your setup code here, to run once:
  initialize_pir_motion_sensor_status();
  initialize_pir_motion_sensor();
  Serial.begin(9600);

  initialize_neopixel_ring();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Device Test");
  delay(2000);
  update_neopixel_ring();
}
