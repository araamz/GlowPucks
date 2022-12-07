#include <stdio.h>
#include <string.h>

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
void parse_protocol_string(char* protocol_message);
void config_protocol_handler();
void status_protocol_handler();
void update_uno_config_values();
void generate_status_response();
void generate_config_bootup_request();
enum modes { still = 0, flash = 1, alternate = 2 };
static int brightness = 0;
static int active = 0;
static enum modes mode = still;
static int r1 = 0;
static int g1 = 0;
static int b1 = 0;
static int r2 = 0;
static int g2 = 0;
static int b2 = 0; 
static int car_count = 0;

char string[] = "config\tGP1\t100\t1\t0\t200\t0\t0\t-1\t-1\t-1\t0\tNULL\t0";
void parse_protocol_string(char* protocol_message) {
  
  const char* parsed_type;
  const char* parsed_label;
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
  const char* parsed_group_target;
  int parsed_car_clear;

  char* token = strtok(protocol_message, "\t");
  char* delimiter = "\t";
  for (int token_index = 0; token != NULL; token_index++) {
      
      if (token_index == TYPE_INDEX) {
          parsed_type = token;
      }
      
      if (token_index == LABEL_INDEX) {
          parsed_label = token;
      }
      
      if (token_index == BRIGHTNESS_INDEX) {
          parsed_brightness = atoi(token);
      }
    
      if (token_index == ACTIVE_INDEX) {
          parsed_active = atoi(token);
      }
      
      if (token_index == MODE_INDEX) {
          parsed_mode = atoi(token);
      }
      
      if (token_index == R1_INDEX) {
          parsed_r1 = atoi(token);
      }
      
      token = strtok(NULL, delimiter);
  }
/*
  if (strcmp(token, (const char*)"config")) {
    
  } else {
      
  }*/

}

int main() {
    
    parse_protocol_string(string);

    return 0;
    
}