#ifndef __JSON_H__
#define __JSON_H__

#include "main.h"


/// @brief  Convert simple node sytax to json
/// @note   RMH1;RKOK1;T;-  -> {"Z":"MH1","S":"RKOK1","V":"T","R":"-"}
/// @param  buff = result buffer
/// @param  uart_p = pointer to uart data
/// @return  0
uint16_t json_convert_uart_node_to_json(char *buff, msg69_st *msg69_p);

/// @brief Convert sensor (float) to radio message
/// @note
/// @param  buff  target buffer
/// @param  zone char array
/// @param  sensor
/// @param  value float
/// @param  remark
/// @return
uint16_t json_convert_sensor_float_to_json(
    char *buff, 
    const char *zone, 
    const char *sensor, 
    float value, 
    const char *remark );


/// @brief  Pick data from UART Rx string
/// @param  Pointer to UART data
/// @return
void json_pick_data_from_rx(msg69_st *msg69_p, String *StrPtr);


/// @brief  decode simple node string to json 
/// @param  uart data pointer
/// @param  node char array 
/// @return -
void json_decode_to_tab_str(msg69_st *msg69_p, char *jarray);    

#endif