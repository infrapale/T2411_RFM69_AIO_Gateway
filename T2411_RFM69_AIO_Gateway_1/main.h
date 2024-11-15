#ifndef __MAIN_H__
#define __MAIN_H__

#include "Arduino.h"
//#include "json.h"

/// SSID Definitions
//#define  VILLA_ASTRID
//#define  H_MOKKULA
#define PIRPANA

#define APP_NAME "T2411_RFM69_AIO_Gateway_1"
#define RFM_SEND_INTERVAL       2000
#define MAX_RFM69_MESSAGE_LEN   68
#define MSG69_ZONE_LEN          8
#define MSG69_NAME_LEN          8

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    IO_USERNAME
#define AIO_KEY         IO_KEY
#define AIO_PUBLISH_INTERVAL_ms  60000

typedef struct
{
    String zone;
    String label; 
    String value;
    String remark;
} msg69_st;


typedef struct  
{
  uint8_t  com69_task_index;
  uint8_t  wifi_task_index;
  uint8_t  mqtt_task_index;
  uint8_t  super_task_index;
  msg69_st msg69;
  boolean  wifi_is_connected;
  boolean  radio_is_available;
} gw_ctrl_st;


typedef enum {
  STATUS_AT_HOME = 0,
  STATUS_AWAY
} status_et;

typedef struct
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} time_st;


#endif